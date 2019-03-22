


#include "AllyConsideration.h"
#include "UtilityIntelligence.h"

UAllyConsideration::UAllyConsideration()
{
	Description = "A consideration based on how many allies we have nearby.";
	MaxAllies = 4;
}

float UAllyConsideration::Score_Implementation(const FDecisionContext & Context) const
{
	if (ResponseCurve == nullptr || MaxAllies == 0)
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
		float allyCount = (float)intelligence->GetAllies().Num();
		return ResponseCurve->GetFloatValue(allyCount / (float)MaxAllies);
	}
}
