


#include "UtilityIntelligence.h"
#include "AISystem.h"

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
		return;
	}

	AddGameplayTag(Context.Decision->TagToApply);

	RecentDecisions.Add(Context.Decision->RunDecision(Context));
}

void UUtilityIntelligence::StartAI(AAIController* Controller, UBlackboardComponent* Blackboard)
{
	OurController = Controller;
	AIBlackboard = Blackboard;

	if (OurController != nullptr && OurController->GetPawn() != nullptr)
	{
		APawn* pawn = OurController->GetPawn();
		IGameplayTagAssetInterface* gameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(pawn);
		bPawnHasGameplayTags = gameplayTagAssetInterface != nullptr;
	}
	else
	{
		bPawnHasGameplayTags = false;
	}

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
	ValidTargets.Add(NewTarget);
}

void UUtilityIntelligence::RemovePossibleTarget(APawn* Target)
{
	ValidTargets.Remove(Target);
}

TSet<APawn*> UUtilityIntelligence::GetPossibleTargets() const
{
	return ValidTargets;
}

void UUtilityIntelligence::AddNewAlly(APawn* NewAlly)
{
	ValidAllies.Add(NewAlly);
}

void UUtilityIntelligence::RemoveAlly(APawn* Ally)
{
	ValidAllies.Remove(Ally);
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