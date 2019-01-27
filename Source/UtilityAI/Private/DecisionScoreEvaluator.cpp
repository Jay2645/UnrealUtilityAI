

#include "DecisionScoreEvaluator.h"

UDecisionScoreEvaluator::UDecisionScoreEvaluator()
{
	Description = "A generic decision score evaluator.";
	Weight = 1.0f;
	Priority = EEvaluatorPriority::Ambient;
}

float UDecisionScoreEvaluator::GetWeight()
{
	return (float)Priority + Weight;
}

float UDecisionScoreEvaluator::CalculateScore(const FDecisionContext& Context, float MinToBeat, float Bonus)
{
	float score = Bonus;
	for (const UConsideration* consideration : Considerations)
	{
		score *= FMath::Clamp(consideration->CalculateScore(Context), 0.0f, 1.0f);
		if (score < 0.0f || score < MinToBeat)
		{
			break;
		}
	}
	return score * GetWeight();
}
