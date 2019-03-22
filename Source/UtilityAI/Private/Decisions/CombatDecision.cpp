


#include "CombatDecision.h"
#include "UtilityIntelligence.h"
#include "UtilityAIHelpers.h"

UCombatDecision::UCombatDecision()
{
	MaxCombatRange = 5000.0f;
}

APawn* UCombatDecision::FindTarget(const FDecisionContext& Context) const
{
	UUtilityIntelligence* intelligence = Context.OurIntelligence;
	TArray<APawn*> targets;
	if (intelligence == nullptr)
	{
		return nullptr;
	}

	targets = intelligence->GetPossibleTargets().Array();
	if (targets.Num() == 0)
	{
		return nullptr;
	}

	TArray<FDecisionContext> contexts = TArray<FDecisionContext>();
	contexts.SetNum(targets.Num());
	for (int i = 0; i < targets.Num(); i++)
	{
#if !UE_BUILD_SHIPPING
		if (targets[i] == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Detected a null target!"));
			return nullptr;
		}
#endif
		contexts[i] = FDecisionContext(Context);
		contexts[i].OurTarget = targets[i];
	}

	FDecisionContext bestContext = FDecisionContext();
	float bestScore = 0.0f;
	UDecisionScoreEvaluator::FindBestContext(contexts, FindTargetDSE, bestContext, bestScore);
	AActor* target = bestContext.OurTarget;
	if (target == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no target from %d contexts and %d targets!"), contexts.Num(), targets.Num());
		return nullptr;
	}

	// Inform the intelligence about our target
	intelligence->UpdateContextTarget(target);
	return Cast<APawn>(target);
}

TEnumAsByte<EBTNodeResult::Type> UCombatDecision::MoveToTarget(const FDecisionContext& Context, APawn* Target) const
{
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	return UUtilityAIHelpers::MoveToActor(Context.OurIntelligence, MaxCombatRange, Target);
}


FMadeDecision UCombatDecision::RunDecision_Implementation(const FDecisionContext& Context) const
{
	FMadeDecision decision = Super::RunDecision_Implementation(Context);
	APawn* target = FindTarget(Context);
	decision.DecisionStatus = MoveToTarget(Context, target);
	return decision;
}