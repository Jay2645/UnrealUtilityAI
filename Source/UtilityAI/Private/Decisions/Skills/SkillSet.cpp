

#include "SkillSet.h"

USkill* USkillSet::FindBestSkill(const FDecisionContext& StartContext, FDecisionContext& OutContext, float& OutBestScore) const
{
	USkill* best = NULL;
	for (auto& kvp : Skills)
	{
		USkill* skill = kvp.Key;
		USkillDecisionScoreEvaluator* dse = kvp.Value;
		if (dse != NULL)
		{
			FDecisionContext context = FDecisionContext(StartContext);
			context.Decision = skill;

			float score = dse->CalculateScore(context, OutBestScore);
			if (score > OutBestScore)
			{
				best = skill;
				OutBestScore = score;
				OutContext = context;
			}
		}
	}
	return best;
}