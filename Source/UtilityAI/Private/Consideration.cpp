

#include "Consideration.h"

UConsideration::UConsideration()
{
	Description = "Default consideration, will return a score of 0";
}

float UConsideration::Score_Implementation(const FDecisionContext& Context) const
{
	return 0.0f;
}
