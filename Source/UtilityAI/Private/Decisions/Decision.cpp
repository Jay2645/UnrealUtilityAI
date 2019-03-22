


#include "Decision.h"

float UDecision::CalculateScore(FDecisionContext& Context, float MinToBeat, float Bonus) const
{
	if (DecisionScoreEvaluator == nullptr)
	{
		return 0.0f;
	}
	else
	{
		return DecisionScoreEvaluator->CalculateScore(Context, MinToBeat, Bonus);
	}
}
