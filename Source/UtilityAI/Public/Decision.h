

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
	// What time we made this decision.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime DecisionTime;
};

/**
 * 
 */
UCLASS()
class UTILITYAI_API UDecision : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
};
