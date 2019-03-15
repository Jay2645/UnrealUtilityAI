

#pragma once

#include "CoreMinimal.h"
#include "DateTime.h"
#include "Engine/DataAsset.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "DecisionBase.generated.h"

class UDecisionBase;
class UUtilityIntelligence;

/*
* A context that gets passed in giving real-world data about a decision.
* This passes in the AI making the decision, the decision we're trying
* to make, as well as the actor we're acting towards.
*/
USTRUCT(BlueprintType)
struct UTILITYAI_API FDecisionContext
{
	GENERATED_BODY()

public:
	// The intelligence which is generating our considerations.
	// i.e. "Who is asking?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUtilityIntelligence* OurIntelligence;
	// The actor which we are currently "thinking about."
	// i.e. "What are you going to do the action to?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OurTarget;
	// A Blackboard containing any "extra" context.
	// i.e. "What else is going on?"
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Transient)
	UBlackboardComponent* AIBlackboard;
	// The decision that we'll make if we succeed.
	// i.e. "What are you trying to do?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UDecisionBase* Decision;

public:
	FDecisionContext()
	{
		OurIntelligence = NULL;
		OurTarget = NULL;
		AIBlackboard = NULL;
		Decision = NULL;
	}
};

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
	// The status of the decision after it was made.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EBTNodeResult::Type> DecisionStatus;
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
	virtual FMadeDecision RunDecision_Implementation(const FDecisionContext& Context) const;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Utility AI|Decision")
	FMadeDecision RunDecision(const FDecisionContext& Context) const;
};
