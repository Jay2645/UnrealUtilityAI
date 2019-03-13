

#include "DecisionMaker.h"

UDecision* UDecisionMaker::FindBestDecision(FDecisionContext Context, float& OutBestScore, float MinToBeat) const
{
	// Set best score to just below MinToBeat, just so we always lose if we fail
	OutBestScore = MinToBeat - 0.01f;
	UDecision* best = NULL;
	for (UDecision* decision : Decisions)
	{
		if (decision != NULL)
		{
			Context.Decision = decision;
			float score = decision->CalculateScore(Context, MinToBeat);
			if (score > MinToBeat)
			{
				best = decision;
				MinToBeat = score;
				OutBestScore = score;
			}
		}
	}
	return best;
}