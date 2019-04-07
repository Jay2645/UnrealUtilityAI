


#include "CombatDecision.h"
#include "UtilityIntelligence.h"
#include "UtilityAIHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

UCombatDecision::UCombatDecision()
{
	MaxCombatRange = 5000.0f;
	TargetBlackboardKey = NAME_None;
}

FDecisionContext UCombatDecision::FindTarget(const FDecisionContext& Context) const
{
	FDecisionContext newContext = FDecisionContext(Context);
	newContext.OurTarget = nullptr;

	UUtilityIntelligence* intelligence = Context.OurIntelligence;
	if (intelligence == nullptr)
	{
		return newContext;
	}

	// Clear the target on the intelligence
	intelligence->UpdateContextTarget(nullptr);

	TArray<APawn*> targets;
	targets = intelligence->GetPossibleTargets().Array();
	if (targets.Num() == 0)
	{
		return newContext;
	}

	TArray<FDecisionContext> contexts = TArray<FDecisionContext>();
	contexts.SetNum(targets.Num());
	for (int i = 0; i < targets.Num(); i++)
	{
#if !UE_BUILD_SHIPPING
		if (targets[i] == nullptr)
		{
			UE_LOG(LogUtilityAI, Warning, TEXT("Detected a null target!"));
			return newContext;
		}
#endif
		contexts[i] = FDecisionContext(newContext);
		contexts[i].OurTarget = targets[i];
	}

	FDecisionContext bestContext = FDecisionContext(newContext);
	float bestScore = 0.0f;
	UDecisionScoreEvaluator::FindBestContext(contexts, FindTargetDSE, bestContext, bestScore);
	AActor* target = bestContext.OurTarget;
	if (target == nullptr)
	{
		UE_LOG(LogUtilityAI, Warning, TEXT("Found no target from %d contexts and %d targets!"), contexts.Num(), targets.Num());
		return newContext;
	}

	// Inform the intelligence about our new target
	intelligence->UpdateContextTarget(target);
	return bestContext;
}

FVector UCombatDecision::FindEQSPosition(const FDecisionContext& Context) const
{
	if (QueryTemplate == nullptr)
	{
		UE_LOG(LogUtilityAI, Warning, TEXT("Can't find EQS position due to invalid query template."));
		return INVALID_LOCATION;
	}
	if (Context.OurIntelligence == nullptr)
	{
		UE_LOG(LogUtilityAI, Warning, TEXT("Can't find EQS position due to invalid intelligence."));
		return INVALID_LOCATION;
	}

	FVector outLocation = INVALID_LOCATION;
	// See if we have a cached version that we've scored already
	// We do this before creating the query because the call is async and we want to try
	// and get the values from the query we already have before resetting it with a new one.
	UUtilityEnvQueryInstance* envQueryWrapper = Context.OurIntelligence->GetDecisonEQS(this);
	if (envQueryWrapper != nullptr)
	{
		// This will be the vector from the previous iteration if the EQS hasn't completed yet
		if (envQueryWrapper->GetBestScore() > 0.0f)
		{
			outLocation = envQueryWrapper->GetBestVector();
		}
	}

	// If we have a target and:
	// * We're running this query for the first time
	// * OR we've already completed our last query
	// Go ahead and run another query
	if (Context.OurTarget != nullptr && (envQueryWrapper == nullptr || envQueryWrapper->IsFinished()))
	{
		// Make a new query
		Context.OurIntelligence->RunDecisonEQS(this, Context, RunMode, QueryTemplate);
	}
	return outLocation;
}

FMadeDecision UCombatDecision::RunDecision_Implementation(const FDecisionContext& Context) const
{
	FMadeDecision decision = Super::RunDecision_Implementation(Context);
	FDecisionContext newContext = FindTarget(Context);
	// If we have a blackboard key specified, update the key with our new target
	if (TargetBlackboardKey != NAME_None && newContext.AIBlackboard != nullptr)
	{
#if !UE_BUILD_SHIPPING
		if (newContext.OurTarget == nullptr)
		{
			UE_LOG(LogUtilityAI, Log, TEXT("Setting blackboard key %s to null."), *TargetBlackboardKey.ToString());
		}
		else
		{
			UE_LOG(LogUtilityAI, Log, TEXT("Setting blackboard key %s to %s."), *TargetBlackboardKey.ToString(), *newContext.OurTarget->GetName());
		}
#endif
		newContext.AIBlackboard->SetValueAsObject(TargetBlackboardKey, newContext.OurTarget);
	}
	DoCombatLogic(decision, newContext);
	return decision;
}

void UCombatDecision::DoCombatLogic_Implementation(FMadeDecision& Decision, const FDecisionContext& Context) const
{
	FVector location = FindEQSPosition(Context);
	if (location == INVALID_LOCATION)
	{
		if (Context.OurTarget == nullptr)
		{
			// No target, nowhere to move, so therefore we failed
			Decision.DecisionStatus = EBTNodeResult::Failed;
		}
		else
		{
			Decision.DecisionStatus = UUtilityAIHelpers::MoveToActor(Context.OurIntelligence, MaxCombatRange, Context.OurTarget);
		}
	}
	else
	{
		UE_LOG(LogUtilityAI, Log, TEXT("Moving to location (%f, %f, %f)."), location.X, location.Y, location.Z);
		Decision.DecisionStatus = UUtilityAIHelpers::MoveToPoint(Context.OurIntelligence, MaxCombatRange, location);
	}
}
