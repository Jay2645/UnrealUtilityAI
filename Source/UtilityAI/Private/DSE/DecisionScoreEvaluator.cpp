

#include "DecisionScoreEvaluator.h"

UDecisionScoreEvaluator::UDecisionScoreEvaluator()
{
	Description = "A generic decision score evaluator.";
	Weight = 1.0f;
	Priority = EEvaluatorPriority::Ambient;
}

float UDecisionScoreEvaluator::GetMomentum_Implementation(const FDecisionContext& Context) const
{
	// By default, we don't give any bonus nor penalty for anything being "stale."
	return 0.0f;
}

float UDecisionScoreEvaluator::GetCompensationFactor(float Score, float Bonus) const
{
	// This gets run to offset the fact that an evaluator with many considerations
	// gets effectively "penalized" over time due to the high number of considerations.
	//
	// Since the score is multiplicative and clamped between 0 and 1, a DSE with 25
	// considerations that all give a score of 0.9 all get multiplied together, 
	// effectively bringing down its overall score to 0.072. Meanwhile, a DSE with 1
	// consideration that gives a score of 0.1 would be selected.
	// 
	// The DSE with a lot of considerations got a higher score overall and is (likely)
	// more "fit," but because everything was multiplicative it lowered the score
	// significantly. This corrects for that.

	if (Considerations.Num() == 0 || Score == 0.0f)
	{
		return Score;
	}

	// Using the above example of 25 considerations, and a score of 0.072,
	// assuming that the bonus is 1.0:
	// Modification factor = 0.96
	float modificationFactor = Bonus - (Bonus / (float)Considerations.Num());
	// Make-up value = 0.928 * 0.96 = 0.891
	float makeUpValue = (Bonus - Score) * modificationFactor;
	// Final score = 0.136 -- an improvement!
	// It's not perfect, but it's at least been slightly compensated for.
	return Score + (makeUpValue * Score);
}

float UDecisionScoreEvaluator::GetWeight() const
{
	return (float)Priority * Weight;
}

float UDecisionScoreEvaluator::CalculateScore_Implementation(FDecisionContext& Context, float MinToBeat, float Bonus) const
{
	float startingBonus = GetWeight() + GetMomentum(Context) + Bonus;
	float score = startingBonus;
	for (const UConsideration* consideration : Considerations)
	{
		score *= FMath::Clamp(consideration->CalculateScore(Context), 0.0f, 1.0f);
		if (score == 0.0f || score < MinToBeat)
		{
			break;
		}
	}
	return GetCompensationFactor(score, startingBonus);
}

void UDecisionScoreEvaluator::FindBestContext(TArray<FDecisionContext>& Contexts, const UDecisionScoreEvaluator* Evaluator, FDecisionContext& OutContext, float& OutBestScore)
{
	if (Evaluator == NULL)
	{
		return;
	}

	for (FDecisionContext& context : Contexts)
	{
		float score = Evaluator->CalculateScore(context, OutBestScore);
		if (score > OutBestScore)
		{
			OutBestScore = score;
			OutContext = context;
		}
	}
}