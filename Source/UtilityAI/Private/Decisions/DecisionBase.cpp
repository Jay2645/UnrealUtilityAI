

#include "DecisionBase.h"

FMadeDecision UDecisionBase::RunDecision_Implementation(const FDecisionContext& Decision) const
{
	FMadeDecision decision;
	decision.Decision = this;
	// Make the decision in UTC to avoid daylight savings time
	decision.DecisionTime = FDateTime::UtcNow();
	decision.DecisionStatus = EBTNodeResult::Succeeded;
	return decision;
}
