

#pragma once

#include "CoreMinimal.h"
#include "DateTime.h"
#include "Engine/DataAsset.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardData.h"

#include "DecisionBase.generated.h"

class UDecisionBase;

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
	const UDecisionBase* Decision;
	// What time we made this decision (in UTC time).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime DecisionTime;
};

/**
 * This implements the actual AI code.
 * Each time this decision gets selected, `RunDecision` gets called to execute
 * any AI code. Note that the AI may not choose this decision next frame, so 
 * any actions should be atomic, or at least not matter frame-to-frame.
 */
UCLASS(Abstract, Blueprintable)
class UTILITYAI_API UDecisionBase : public UPrimaryDataAsset
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
