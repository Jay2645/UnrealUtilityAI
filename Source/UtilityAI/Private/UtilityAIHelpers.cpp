


#include "UtilityAIHelpers.h"
#include "AITypes.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Navigation/PathFollowingComponent.h"

TEnumAsByte<EBTNodeResult::Type> UUtilityAIHelpers::PerformMoveTask(UUtilityIntelligence* Intelligence, FBTMoveToTaskMemory& OutMemory, float MaxDistance, AActor* TargetActor, const FVector& TargetLocation)
{
	AAIController* myController = Intelligence->GetController();

	TEnumAsByte<EBTNodeResult::Type> nodeResult = EBTNodeResult::Failed;
	if (myController)
	{
		FAIMoveRequest moveReq;
		moveReq.SetAcceptanceRadius(Intelligence->AcceptableRadius + MaxDistance);
		moveReq.SetNavigationFilter(*Intelligence->FilterClass ? Intelligence->FilterClass : myController->GetDefaultNavigationFilterClass());
		moveReq.SetCanStrafe(Intelligence->bAllowStrafe);
		moveReq.SetAllowPartialPath(Intelligence->bAllowPartialPath);
		moveReq.SetReachTestIncludesAgentRadius(Intelligence->bReachTestIncludesAgentRadius);
		moveReq.SetReachTestIncludesGoalRadius(Intelligence->bReachTestIncludesGoalRadius);
		moveReq.SetProjectGoalLocation(Intelligence->bProjectGoalLocation);
		moveReq.SetUsePathfinding(Intelligence->bUsePathfinding);

		if (TargetActor != NULL)
		{
			if (Intelligence->bTrackMovingGoal)
			{
				moveReq.SetGoalActor(TargetActor);
			}
			else
			{
				const FVector goalLocation = TargetActor->GetActorLocation();
				moveReq.SetGoalLocation(goalLocation);
				OutMemory.PreviousGoalLocation = goalLocation;
			}
		}
		else
		{
			moveReq.SetGoalLocation(TargetLocation);
			OutMemory.PreviousGoalLocation = TargetLocation;
		}

		if (moveReq.IsValid())
		{
			FPathFollowingRequestResult requestResult = myController->MoveTo(moveReq);
			switch (requestResult.Code)
			{
				case EPathFollowingRequestResult::RequestSuccessful:
					OutMemory.MoveRequestID = requestResult.MoveId;
					nodeResult = EBTNodeResult::InProgress;
					break;
				case EPathFollowingRequestResult::AlreadyAtGoal:
					nodeResult = EBTNodeResult::Succeeded;
					break;
				case EPathFollowingRequestResult::Failed:
					nodeResult = EBTNodeResult::Failed;
					break;
				default:
					checkNoEntry();
					break;
			}
		}
	}

	return nodeResult;
}

TEnumAsByte<EBTNodeResult::Type> UUtilityAIHelpers::MoveTo(UUtilityIntelligence* Intelligence, FBTMoveToTaskMemory& OutMemory, float MaxDistance, AActor* MoveActor, FVector MoveLocation)
{
	if (Intelligence == NULL)
	{
		return EBTNodeResult::Failed;
	}

	TEnumAsByte<EBTNodeResult::Type> nodeResult = EBTNodeResult::InProgress;

	OutMemory.PreviousGoalLocation = FAISystem::InvalidLocation;
	OutMemory.MoveRequestID = FAIRequestID::InvalidRequest;

	AAIController* myController = Intelligence->GetController();
	OutMemory.bWaitingForPath = myController->ShouldPostponePathUpdates();
	if (!OutMemory.bWaitingForPath)
	{
		nodeResult = PerformMoveTask(Intelligence, OutMemory, MaxDistance, MoveActor, MoveLocation);
	}

	return nodeResult;
}

TEnumAsByte<EBTNodeResult::Type> UUtilityAIHelpers::MoveToPoint(UUtilityIntelligence* Intelligence, float MaxDistance, const FVector& Point)
{
	FBTMoveToTaskMemory memory = FBTMoveToTaskMemory();
	return MoveTo(Intelligence, memory, MaxDistance, NULL, Point);
}

TEnumAsByte<EBTNodeResult::Type> UUtilityAIHelpers::MoveToActor(UUtilityIntelligence* Intelligence, float MaxDistance, AActor* Actor)
{
	FBTMoveToTaskMemory memory = FBTMoveToTaskMemory();
	return MoveTo(Intelligence, memory, MaxDistance, Actor);
}