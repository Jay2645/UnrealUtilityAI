


#include "EnemyConsideration.h"
#include "UtilityIntelligence.h"

UEnemyConsideration::UEnemyConsideration()
{
	Description = "A consideration which takes into account how many enemies we know about.";
	MaxEnemies = 4;
}

float UEnemyConsideration::Score_Implementation(const FDecisionContext & Context) const
{
	if (ResponseCurve == nullptr || MaxEnemies == 0)
	{
		return 0.0f;
	}
	else
	{
		UUtilityIntelligence* intelligence = Context.OurIntelligence;
		if (intelligence == nullptr)
		{
			return 0.0f;
		}
		float enemyCount = (float)intelligence->GetPossibleTargets().Num();
		return ResponseCurve->GetFloatValue(enemyCount / (float)MaxEnemies);
	}
}
