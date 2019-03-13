

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "DecisionBase.h"

#include "UtilityAIInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUtilityAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UTILITYAI_API IUtilityAIInterface
{
	GENERATED_BODY()

protected:
	virtual TArray<FMadeDecision> GetRecentDecisions_Implementation() const;

public:
	// Gets all decisions that we have made recently.
	// IMPORTANT: Decisions which we have made most recently should be at the END of the array.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Utility AI|Consideration")
	TArray<FMadeDecision> GetRecentDecisions() const;
};
