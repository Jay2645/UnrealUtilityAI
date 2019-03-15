

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

#include "Controllers/UtilityIntelligence.h"

#include "UtilityAIHelpers.generated.h"

/**
 * A collection of helpers for the utlity AI.
 */
UCLASS()
class UTILITYAI_API UUtilityAIHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static TEnumAsByte<EBTNodeResult::Type> PerformMoveTask(UUtilityIntelligence* Intelligence, FBTMoveToTaskMemory& OutMemory, float MaxDistance, AActor* TargetActor, const FVector& TargetLocation);
	static TEnumAsByte<EBTNodeResult::Type> MoveTo(UUtilityIntelligence* Intelligence, FBTMoveToTaskMemory& OutMemory, float MaxDistance, AActor* MoveActor = NULL, FVector MoveLocation = FVector(FLT_MAX));

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	static TEnumAsByte<EBTNodeResult::Type> MoveToPoint(UUtilityIntelligence* Intelligence, float MaxDistance, const FVector& Point);
	UFUNCTION(BlueprintCallable, Category = "AI")
	static TEnumAsByte<EBTNodeResult::Type> MoveToActor(UUtilityIntelligence* Intelligence, float MaxDistance, AActor* Actor);
};
