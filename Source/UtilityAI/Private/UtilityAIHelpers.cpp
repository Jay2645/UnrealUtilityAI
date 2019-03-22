


#include "UtilityAIHelpers.h"
#include "AITypes.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Navigation/PathFollowingComponent.h"
#include "AISystem.h"

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

		if (TargetActor != nullptr)
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
	if (Intelligence == nullptr)
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

UUtilityEnvQueryInstance* UUtilityAIHelpers::RunEQSQuery(const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, FEnvQueryRequest& Query, UUtilityEnvQueryInstance* Wrapper)
{
	if (Context.OurIntelligence == nullptr)
	{
		return nullptr;
	}
	AAIController* controller = Context.OurIntelligence->GetController();
	if (controller == nullptr)
	{
		return nullptr;
	}
	APawn* pawn = controller->GetPawn();
	if (pawn == nullptr)
	{
		return nullptr;
	}

	// Create the wrapper if it doesn't already exist
	if (Wrapper == nullptr)
	{
		// The wrapper needs to be attached to the EQS manager
		UAISystem* aiSys = UAISystem::GetCurrentSafe(pawn->GetWorld());
		if (aiSys == nullptr)
		{
			return nullptr;
		}
		UEnvQueryManager* eqsManager = aiSys->GetEnvironmentQueryManager();
		if (eqsManager == nullptr)
		{
			return nullptr;
		}
		Wrapper = NewObject<UUtilityEnvQueryInstance>(eqsManager, UUtilityEnvQueryInstance::StaticClass());
		// Verify it initialized correctly
		if (Wrapper == nullptr)
		{
			return nullptr;
		}
	}

	// Run the query
	Wrapper->SetInstigator(pawn);
	Wrapper->RunQuery(RunMode, Query);
	return Wrapper;
}

UUtilityEnvQueryInstance* UUtilityAIHelpers::RunEQSQueryFromTemplate(const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, UEnvQuery* QueryTemplate, UUtilityEnvQueryInstance* Wrapper)
{
	if (QueryTemplate == nullptr)
	{
		return nullptr;
	}
	// Create a request and run the query
	FEnvQueryRequest request = FEnvQueryRequest(QueryTemplate);
	return RunEQSQuery(Context, RunMode, request, Wrapper);
}

TEnumAsByte<EBTNodeResult::Type> UUtilityAIHelpers::MoveToPoint(UUtilityIntelligence* Intelligence, float MaxDistance, const FVector& Point)
{
	FBTMoveToTaskMemory memory = FBTMoveToTaskMemory();
	return MoveTo(Intelligence, memory, MaxDistance, nullptr, Point);
}

TEnumAsByte<EBTNodeResult::Type> UUtilityAIHelpers::MoveToActor(UUtilityIntelligence* Intelligence, float MaxDistance, AActor* Actor)
{
	FBTMoveToTaskMemory memory = FBTMoveToTaskMemory();
	return MoveTo(Intelligence, memory, MaxDistance, Actor);
}