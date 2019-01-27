

#include "Decision.h"

FMadeDecision UDecision::RunDecision_Implementation(AAIController* RunningController) const
{
	FMadeDecision decision;
	decision.Decision = this;
	// Make the decision in UTC to avoid daylight savings time
	decision.DecisionTime = FDateTime::UtcNow();
	return decision;
}
