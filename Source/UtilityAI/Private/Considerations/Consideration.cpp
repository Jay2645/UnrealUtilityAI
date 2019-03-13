

#include "Consideration.h"
#include "GameplayTagAssetInterface.h"
#include "UtilityAIInterface.h"

UConsideration::UConsideration()
{
	Description = "Default consideration, will return a score of 0";
}

bool UConsideration::CanScore(const FDecisionContext& Context) const
{
	AActor* target = Context.OurTarget;
	AAIController* us = Context.OurController;
	if (!HasAllRequiredGameplayTags(target, us))
	{
		return false;
	}

	if (!IsInRange(target, us))
	{
		return false;
	}


	return CooldownIsValid(Context);
}

bool UConsideration::IsInRange(AActor* Target, AAIController* Us) const
{
	if (Radius <= 0.0f)
	{
		return true;
	}

	if (Target == NULL || Us == NULL)
	{
		return false;
	}

	APawn* ourPawn = Us->GetPawn();
	if (ourPawn == NULL)
	{
		return false;
	}

	FVector ourLocation = ourPawn->GetActorLocation();
	FVector targetLocation = Target->GetActorLocation();
	return FVector::DistSquared(targetLocation, ourLocation) <= Radius * Radius;
}

bool UConsideration::HasAllRequiredGameplayTags(AActor* Target, AAIController* Us) const
{
	if (Target == NULL || Us == NULL)
	{
		return false;
	}

	// Target gameplay tags
	if (TargetRequiredTags.Num() > 0)
	{
		IGameplayTagAssetInterface* targetInterface = Cast<IGameplayTagAssetInterface>(Target);
		if (targetInterface != NULL)
		{
			if (!targetInterface->HasAllMatchingGameplayTags(TargetRequiredTags))
			{
				return false;
			}
		}
	}

	// Our gameplay tags
	if (OurRequiredTags.Num() > 0)
	{
		IGameplayTagAssetInterface* ourInterface = Cast<IGameplayTagAssetInterface>(Us->GetPawn());
		if (ourInterface != NULL)
		{
			if (!ourInterface->HasAllMatchingGameplayTags(OurRequiredTags))
			{
				return false;
			}
		}
	}

	return true;
}

bool UConsideration::CooldownIsValid(const FDecisionContext& Context) const
{
	// See if we can get away with skipping this entirely
	if (CooldownTime <= 0.0f && CooldownDecisions.Num() == 0)
	{
		return true;
	}

	// Check to see if we inherit the right interface
	IUtilityAIInterface* ourAI = Cast<IUtilityAIInterface>(Context.OurController);
	if (ourAI == NULL)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("You can only \"properly\" use Utility AI if your Controller inherits UtilityAIInterface"));
		UE_LOG(LogTemp, Warning, TEXT("Make sure to add it as a valid interface to get everything working correctly!"));
#endif
		return false;
	}

	TArray<FMadeDecision> decisions = ourAI->GetRecentDecisions();
#if WITH_EDITOR
	if (decisions.Num() > 2)
	{
		if (decisions[1].DecisionTime < decisions[0].DecisionTime)
		{
			UE_LOG(LogTemp, Error, TEXT("Decisions are being added in the wrong order!"));
			UE_LOG(LogTemp, Error, TEXT("Make sure that decisions made later get placed at the end of the array."));
		}
	}
#endif

	// Check cooldowns
	if (CooldownTime > 0.0f)
	{
		if (!DecisionsAreValid(Context, decisions, CooldownTime, bInvertCooldown))
		{
			return false;
		}
	}

	// Check other decisions
	// This happens after cooldowns since it's slower (n * m as opposed to n)
	for (int i = 0; i < CooldownDecisions.Num(); i++)
	{
		if (!DecisionsAreValid(Context, decisions, CooldownDecisions[i].Cooldown, CooldownDecisions[i].bInvert))
		{
			return false;
		}
	}

	return true;
}

bool UConsideration::DecisionsAreValid(const FDecisionContext& Context, const TArray<FMadeDecision>& Decisions, float Cooldown, bool bInvert) const
{
	// Decisions are made in UTC
	FDateTime now = FDateTime::UtcNow();
	for (int i = Decisions.Num() - 1; i <= 0; i++)
	{
		FTimespan elapsed = now - Decisions[i].DecisionTime;
		// All decisions are made with the earliest first
		// This means as soon as we exceed the cooldown period for one decision,
		// we've exceeded the cooldown period for all decisions
		if (elapsed.GetSeconds() > Cooldown)
		{
			// We've processed everything we care about without hitting 
			// any snags, move on to the next step
			if (bInvert)
			{
				// We can only process this consideration in that window
				return false;
			}
			break;
		}

		// We're within the cooldown period for this decision
		if (Decisions[i].Decision == Context.Decision)
		{
			// We've made this decision recently
			if (bInvert)
			{
				// We can only do this decision recently in the first place!
				break;
			}
			else
			{
				// Too soon, Executus!
				return false;
			}
		}
	}
	return true;
}

float UConsideration::Score_Implementation(const FDecisionContext& Context) const
{
	return 0.0f;
}

float UConsideration::CalculateScore(const FDecisionContext& Context) const
{
	if (CanScore(Context))
	{
		return Score(Context);
	}
	else
	{
		return 0.0f;
	}
}
