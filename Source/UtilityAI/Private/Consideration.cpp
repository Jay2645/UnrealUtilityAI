

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
	AController* us = Context.OurController;
	if (target == NULL || us == NULL)
	{
		return false;
	}

	// Target gameplay tags
	if (TargetRequiredTags.Num() > 0)
	{
		IGameplayTagAssetInterface* targetInterface = Cast<IGameplayTagAssetInterface>(target);
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
		IGameplayTagAssetInterface* ourInterface = Cast<IGameplayTagAssetInterface>(us->GetPawn());
		if (ourInterface != NULL)
		{
			if (!ourInterface->HasAllMatchingGameplayTags(OurRequiredTags))
			{
				return false;
			}
		}
	}

	return CooldownIsValid(Context);
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
	FDateTime now = FDateTime::Now();
	for (int i = Decisions.Num() - 1; i <= 0; i++)
	{
		FTimespan elapsed = now - Decisions[i].DecisionTime;
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
