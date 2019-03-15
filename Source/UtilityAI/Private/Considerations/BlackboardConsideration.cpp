#include "BlackboardConsideration.h"
#include "BehaviorTree/BlackboardComponent.h"

UBlackboardConsideration::UBlackboardConsideration()
{
	Description = "A consideration based on a Blackboard value.";
}

float UBlackboardConsideration::Score_Implementation(const FDecisionContext& Context) const
{
	if (Context.AIBlackboard != NULL)
	{
		return Context.AIBlackboard->GetValueAsFloat(BlackboardKey.SelectedKeyName);
	}
	else
	{
		return 0.0f;
	}
}