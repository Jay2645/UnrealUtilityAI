


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
	if (intelligence == NULL)
	{
		return NULL;
	}

	targets = intelligence->GetPossibleTargets().Array();
	if (targets.Num() == 0)
	{
		return NULL;
	}

	TArray<FDecisionContext> contexts = TArray<FDecisionContext>();
	contexts.SetNum(targets.Num());
	for (int i = 0; i < targets.Num(); i++)
	{
		contexts[i] = FDecisionContext(Context);
		contexts[i].OurTarget = targets[i];
	}

	FDecisionContext bestContext = FDecisionContext();
	float bestScore = 0.0f;
	UDecisionScoreEvaluator::FindBestContext(contexts, FindTargetDSE, bestContext, bestScore);
	AActor* target = bestContext.OurTarget;
	if (target == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no target from %d contexts and %d targets!"), contexts.Num(), targets.Num());
		return NULL;
	}

	// Inform the intelligence about our target
	intelligence->UpdateContextTarget(target);
	return Cast<APawn>(target);
}

TEnumAsByte<EBTNodeResult::Type> UCombatDecision::MoveToTarget(const FDecisionContext& Context, APawn* Target) const
{
	if (Target == NULL)
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