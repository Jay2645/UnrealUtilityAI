

#include "SkillSet.h"

USkill* USkillSet::FindBestSkill(FDecisionContext Context, float& OutBestScore, float MinToBeat) const
{
	// Set best score to just below MinToBeat, just so we always lose if we fail
	OutBestScore = MinToBeat - 0.01f;
	USkill* best = NULL;
	for (auto& kvp : Skills)
	{
		USkill* skill = kvp.Key;
		USkillDecisionScoreEvaluator* dse = kvp.Value;
		if (dse != NULL)
		{
			Context.Decision = skill;

			float score = dse->CalculateScore(Context, MinToBeat);
			if (score > MinToBeat)
			{
				best = skill;
				MinToBeat = score;
				OutBestScore = score;
			}
		}
	}
	return best;
}