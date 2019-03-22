

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"

#include "Controllers/UtilityIntelligence.h"
#include "UtilityEnvQueryInstance.h"

#include "UtilityAIHelpers.generated.h"

/**
 * A collection of helpers for the utility AI.
 */
UCLASS()
class UTILITYAI_API UUtilityAIHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static TEnumAsByte<EBTNodeResult::Type> PerformMoveTask(UUtilityIntelligence* Intelligence, FBTMoveToTaskMemory& OutMemory, float MaxDistance, AActor* TargetActor, const FVector& TargetLocation);
	static TEnumAsByte<EBTNodeResult::Type> MoveTo(UUtilityIntelligence* Intelligence, FBTMoveToTaskMemory& OutMemory, float MaxDistance, AActor* MoveActor = nullptr, FVector MoveLocation = FVector(FLT_MAX));
	static UUtilityEnvQueryInstance* RunEQSQuery(const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, FEnvQueryRequest& Query, UUtilityEnvQueryInstance* Wrapper = nullptr);
public:
	// Runs a given EQS query. Assumes that the query is already ready to go.
	// Will return a UUtilityEnvQueryInstance object which allows users
	// to see the results of the query.
	// Runs a query from a given template.
	// Will return a UUtilityEnvQueryInstance object which allows users
	// to see the results of the query.
	UFUNCTION(BlueprintCallable, Category = "AI")
	static UUtilityEnvQueryInstance* RunEQSQueryFromTemplate(const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, UEnvQuery* QueryTemplate, UUtilityEnvQueryInstance* Wrapper = nullptr);
	UFUNCTION(BlueprintCallable, Category = "AI")
	static TEnumAsByte<EBTNodeResult::Type> MoveToPoint(UUtilityIntelligence* Intelligence, float MaxDistance, const FVector& Point);
	UFUNCTION(BlueprintCallable, Category = "AI")
	static TEnumAsByte<EBTNodeResult::Type> MoveToActor(UUtilityIntelligence* Intelligence, float MaxDistance, AActor* Actor);
};
