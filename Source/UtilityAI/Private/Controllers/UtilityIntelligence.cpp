


#include "UtilityIntelligence.h"
#include "AISystem.h"
#include "TimerManager.h"
#include "UtilityAIHelpers.h"

// Sets default values for this component's properties
UUtilityIntelligence::UUtilityIntelligence()
{
	PrimaryComponentTick.bCanEverTick = false;
	SkillTick = 0.05f;
	DecisionTick = 0.125f;

	ContextTarget = nullptr;

	AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);
	bReachTestIncludesGoalRadius = bReachTestIncludesAgentRadius = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
	bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);
	bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);
	bTrackMovingGoal = true;
	bProjectGoalLocation = true;
	bUsePathfinding = true;

}


void UUtilityIntelligence::TickSkills()
{
	// This ticks anything which should be happening WHILE another action happens
	// For example, this will handle shooting while we run away
	FDecisionContext startContext = GetDecisionContext();
	float bestScore = -1.0f;
	FDecisionContext bestContext = startContext;
	for (const USkillSet* skillSet : SkillSets)
	{
		skillSet->FindBestSkill(startContext, bestContext, bestScore);
	}

	if (bestScore <= 0.0f)
	{
		// Score too low to make a decision
		return;
	}

	// Make the actual decision
	MakeDecision(bestContext);
}

void UUtilityIntelligence::TickDecisions()
{
	// This handles strategic planning and long-term decisions
	// For example, this plans out a good spot to run away to and starts running there
	FDecisionContext startContext = GetDecisionContext();
	float bestScore = -1.0f;
	FDecisionContext bestContext = startContext;
	for (const UDecisionMaker* decsionMaker : Decisions)
	{
		// Iterate over every DecisionMaker
		// These can be added or removed at will, so if the AI enters a tavern
		// a "tavern" DecisionMaker can be added to the list, allowing for special
		// tavern behaviors
		decsionMaker->FindBestDecision(startContext, bestContext, bestScore);
	}

	if (bestScore <= 0.0f)
	{
		// Score too low to make a decision
		UE_LOG(LogUtilityAI, Log, TEXT("%s is skipping decision for this frame since no targets scored above 0."), *GetName());
		return;
	}

	// Make the actual decision
	MakeDecision(bestContext);
}

FDecisionContext UUtilityIntelligence::GetDecisionContext()
{
	FDecisionContext context;
	context.OurIntelligence = this;
	context.Decision = nullptr;
	context.OurTarget = ContextTarget;
	context.AIBlackboard = AIBlackboard;
	return context;
}

void UUtilityIntelligence::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopAI();
}

TArray<FMadeDecision> UUtilityIntelligence::GetRecentDecisions_Implementation() const
{
	return RecentDecisions;
}

void UUtilityIntelligence::OnTargetDestroyed(AActor* Target)
{
	APawn* pawn = Cast<APawn>(Target);
	if (pawn == nullptr)
	{
		return;
	}
	// Only one of these will be successful, assuming the actor is in one of the lists
	RemoveAlly(pawn);
	RemovePossibleTarget(pawn);
}

void UUtilityIntelligence::GetOwnedGameplayTags(FGameplayTagContainer & TagContainer) const
{
	if (bPawnHasGameplayTags && OurController != nullptr && OurController->GetPawn() != nullptr)
	{
		APawn* pawn = OurController->GetPawn();
		IGameplayTagAssetInterface* gameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(pawn);
		if (gameplayTagAssetInterface != nullptr)
		{
			gameplayTagAssetInterface->GetOwnedGameplayTags(TagContainer);
		}
	}
	else
	{
		TagContainer = GameplayTags;
	}
}

void UUtilityIntelligence::RemoveGameplayTag(const FGameplayTag& TagToRemove)
{
	if (!TagToRemove.IsValid())
	{
		return;
	}
	FGameplayTagContainer& container = GameplayTags;
	if (bPawnHasGameplayTags && OurController != nullptr && OurController->GetPawn() != nullptr)
	{
		APawn* pawn = OurController->GetPawn();
		IGameplayTagAssetInterface* gameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(pawn);
		if (gameplayTagAssetInterface != nullptr)
		{
			gameplayTagAssetInterface->GetOwnedGameplayTags(container);
		}
		else
		{
			// Pawn no longer has GameplayTags
			bPawnHasGameplayTags = false;
		}
	}
	container.RemoveTag(TagToRemove);
}

void UUtilityIntelligence::AddGameplayTag(const FGameplayTag& TagToAdd)
{
	if (!TagToAdd.IsValid())
	{
		return;
	}
	FGameplayTagContainer& container = GameplayTags;
	if (bPawnHasGameplayTags && OurController != nullptr && OurController->GetPawn() != nullptr)
	{
		APawn* pawn = OurController->GetPawn();
		IGameplayTagAssetInterface* gameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(pawn);
		if (gameplayTagAssetInterface != nullptr)
		{
			gameplayTagAssetInterface->GetOwnedGameplayTags(container);
		}
		else
		{
			// Pawn no longer has GameplayTags
			bPawnHasGameplayTags = false;
		}
	}
	container.AddTag(TagToAdd);
}

void UUtilityIntelligence::MakeDecision(const FDecisionContext& Context)
{
	if (Context.Decision == nullptr)
	{
		UE_LOG(LogUtilityAI, Warning, TEXT("Skipping decision for this frame since it was set to null."));
		return;
	}

	UE_LOG(LogUtilityAI, Verbose, TEXT("%s is running decision %s."), *GetName(), *Context.Decision->GetName());
	AddGameplayTag(Context.Decision->TagToApply);
	RecentDecisions.Add(Context.Decision->RunDecision(Context));
}

void UUtilityIntelligence::StartAI(AAIController* Controller, UBlackboardComponent* Blackboard)
{
	OurController = Controller;
	AIBlackboard = Blackboard;

	if (OurController != nullptr && OurController->GetPawn() != nullptr)
	{
		if (AIBlackboard == nullptr)
		{
			AIBlackboard = OurController->GetBlackboardComponent();
		}
		APawn* pawn = OurController->GetPawn();
		IGameplayTagAssetInterface* gameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(pawn);
		bPawnHasGameplayTags = gameplayTagAssetInterface != nullptr;
	}
	else
	{
		bPawnHasGameplayTags = false;
	}

#if !UE_BUILD_SHIPPING
	if (AIBlackboard == nullptr)
	{
		UE_LOG(LogUtilityAI, Error, TEXT("No blackboard available for the AI. Blackboard logic will be disabled!"));
	}
#endif

	FTimerManager& worldManager = GetWorld()->GetTimerManager();
	worldManager.SetTimer(DecisionTickTimer, this, &UUtilityIntelligence::TickDecisions, DecisionTick, true, 0.0f);
	worldManager.SetTimer(SkillTickTimer, this, &UUtilityIntelligence::TickSkills, SkillTick, true, 0.0f);

}

void UUtilityIntelligence::StopAI()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UUtilityIntelligence::UpdateContextTarget(AActor* NewTarget)
{
	ContextTarget = NewTarget;
}

void UUtilityIntelligence::AddNewPossibleTarget(APawn* NewTarget)
{
	if (NewTarget == nullptr || NewTarget->IsPendingKill() || !NewTarget->IsValidLowLevel() || ValidTargets.Contains(NewTarget))
	{
		return;
	}
	ValidTargets.Add(NewTarget);
	NewTarget->OnDestroyed.AddDynamic(this, &UUtilityIntelligence::OnTargetDestroyed);
}

void UUtilityIntelligence::RemovePossibleTarget(APawn* Target)
{
	if (Target == nullptr || !ValidTargets.Contains(Target))
	{
		return;
	}
	ValidTargets.Remove(Target);
	Target->OnDestroyed.RemoveDynamic(this, &UUtilityIntelligence::OnTargetDestroyed);
}

TSet<APawn*> UUtilityIntelligence::GetPossibleTargets() const
{
	return ValidTargets;
}

void UUtilityIntelligence::AddNewAlly(APawn* NewAlly)
{
	if (NewAlly == nullptr || NewAlly->IsPendingKill() || !NewAlly->IsValidLowLevel() || ValidAllies.Contains(NewAlly))
	{
		return;
	}
	ValidAllies.Add(NewAlly);
	NewAlly->OnDestroyed.AddDynamic(this, &UUtilityIntelligence::OnTargetDestroyed);
}

void UUtilityIntelligence::RemoveAlly(APawn* Ally)
{
	if (Ally == nullptr || !ValidAllies.Contains(Ally))
	{
		return;
	}
	ValidAllies.Remove(Ally);
	Ally->OnDestroyed.RemoveDynamic(this, &UUtilityIntelligence::OnTargetDestroyed);
}

TSet<APawn*> UUtilityIntelligence::GetAllies() const
{
	return ValidAllies;
}

void UUtilityIntelligence::AddAreaOfInterest(const FVector& InterestingPoint)
{
	AreasOfInterest.Add(InterestingPoint);
}

void UUtilityIntelligence::RemoveAreaOfInterest(const FVector& InterestingPoint)
{
	AreasOfInterest.Remove(InterestingPoint);
}

AAIController* UUtilityIntelligence::GetController() const
{
	return OurController;
}

APawn* UUtilityIntelligence::GetPawn() const
{
	AController* controller = GetController();
	if (controller == nullptr)
	{
		return nullptr;
	}
	else
	{
		return controller->GetPawn();
	}
}

UUtilityEnvQueryInstance* UUtilityIntelligence::GetDecisonEQS(const UDecisionBase* Decision)
{
	if (DecisionEQSHelpers.Contains(Decision))
	{
		return DecisionEQSHelpers[Decision];
	}
	else
	{
		return nullptr;
	}
}

UUtilityEnvQueryInstance* UUtilityIntelligence::RunDecisonEQS(const UDecisionBase* Decision, const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, UEnvQuery* QueryTemplate)
{
	UUtilityEnvQueryInstance* envQueryWrapper = UUtilityAIHelpers::RunEQSQueryFromTemplate(Context, RunMode, QueryTemplate, GetDecisonEQS(Decision));
	DecisionEQSHelpers.Add(Decision, envQueryWrapper);
	return envQueryWrapper;
}

UUtilityEnvQueryInstance* UUtilityIntelligence::GetConsiderationEQS(const UEQSConsideration* Consideration)
{
	if (ConsiderationEQSHelpers.Contains(Consideration))
	{
		return ConsiderationEQSHelpers[Consideration];
	}
	else
	{
		return nullptr;
	}
}

UUtilityEnvQueryInstance* UUtilityIntelligence::RunConsiderationEQS(const UEQSConsideration* Consideration, const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, UEnvQuery* QueryTemplate)
{
	UUtilityEnvQueryInstance* envQueryWrapper = UUtilityAIHelpers::RunEQSQueryFromTemplate(Context, RunMode, QueryTemplate, GetConsiderationEQS(Consideration));
	ConsiderationEQSHelpers.Add(Consideration, envQueryWrapper);
	return envQueryWrapper;
}
