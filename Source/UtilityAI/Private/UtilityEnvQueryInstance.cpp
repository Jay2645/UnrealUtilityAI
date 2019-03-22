#include "UtilityEnvQueryInstance.h"
#include "AITypes.h"

float UUtilityEnvQueryInstance::GetBestScore()
{
	CachedScore = QueryResult.IsValid() && QueryResult->Items.Num() > 0 ? QueryResult->Items[0].Score : CachedScore;
	return CachedScore;
}

bool UUtilityEnvQueryInstance::IsFinished() const
{
	return QueryResult.IsValid() && QueryResult->IsFinished();
}

AActor* UUtilityEnvQueryInstance::GetBestActor() const
{
	TArray<AActor*> actors;
	GetQueryResultsAsActors(actors);
	if (actors.Num() == 0)
	{
		return nullptr;
	}
	else
	{
		return actors[0];
	}
}

FVector UUtilityEnvQueryInstance::GetBestVector() const
{
	TArray<FVector> positions;
	GetQueryResultsAsLocations(positions);
	if (positions.Num() == 0)
	{
		return FAISystem::InvalidLocation;
	}
	else
	{
		return positions[0];
	}
}
