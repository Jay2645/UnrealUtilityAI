

#pragma once

#include "CoreMinimal.h"
#include "DateTime.h"
#include "Engine/DataAsset.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardData.h"

#include "Decision.generated.h"

class UDecision;

/**
* A struct representing a decision that we've made,
* as well as the time we made it.
*/
USTRUCT(BlueprintType)
struct UTILITYAI_API FMadeDecision
{
	GENERATED_BODY()

public:
	// The Decision that we've made.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UDecision* Decision;
	// What time we made this decision (in UTC time).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime DecisionTime;
};

/**
 * 
 */
UCLASS(Blueprintable)
class UTILITYAI_API UDecision : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// A Blackboard containing any kind of static data relevant to
	// every single time this decision is run, no matter the actor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UBlackboardData* StaticDecisionBlackboard;

protected:
	FMadeDecision RunDecision_Implementation(AAIController* RunningController) const;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Utility AI|Decision")
	FMadeDecision RunDecision(AAIController* RunningController) const;
};
