#include "EQSConsideration.h"
#include "UtilityAIHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

UEQSConsideration::UEQSConsideration()
{
	RunMode = EEnvQueryRunMode::SingleResult;
	Description = "A Consideration which uses the score from an EQS query. Can optionally store the value in the AI's blackboard.";
}

float UEQSConsideration::Score_Implementation(const FDecisionContext& Context) const
{
	float score = 0.0f;
	// See if we have a cached version that we've scored already
	// We do this before creating the query because the call is async and we want to try
	// and get the values from the query we already have before resetting it with a new one.
	if (EnvQueryWrapper != nullptr)
	{
		// Query is done, check the score
		// This will be the score from the previous iteration if the EQS hasn't completed yet
		score = EnvQueryWrapper->GetBestScore();
		if (EnvQueryWrapper->IsFinished())
		{
			// If we have a blackboard key and a valid score, cache the results in the blackboard
			if (score > 0.0f && !BlackboardCacheKey.IsNone())
			{
				AActor* bestActor = EnvQueryWrapper->GetBestActor();
				if (bestActor == nullptr)
				{
					FVector bestLocation = EnvQueryWrapper->GetBestVector();
					if (bestLocation != FAISystem::InvalidLocation)
					{
						Context.AIBlackboard->SetValueAsVector(BlackboardCacheKey, bestLocation);
					}
				}
				else
				{
					Context.AIBlackboard->SetValueAsObject(BlackboardCacheKey, bestActor);
				}
			}
		}
		else
		{
			// Not done yet, return our cached score
			return score;
		}
	}

	// Make a new query
	EnvQueryWrapper = UUtilityAIHelpers::RunEQSQueryFromTemplate(Context, RunMode, QueryTemplate, EnvQueryWrapper);
	return score;
}
