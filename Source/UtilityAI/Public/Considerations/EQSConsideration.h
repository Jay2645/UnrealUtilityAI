

#pragma once

#include "CoreMinimal.h"

#include "Considerations/Consideration.h"
#include "UtilityEnvQueryInstance.h"

#include "EQSConsideration.generated.h"

/**
 * An EQS Consideration runs an EQS query when determining a score.
 * The result of this query can be stored in a Blackboard Cache Key,
 * which can later be used in a Blackboard EQS Decision.
 */
UCLASS()
class UTILITYAI_API UEQSConsideration : public UConsideration
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite)
	mutable UUtilityEnvQueryInstance* EnvQueryWrapper;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FName BlackboardCacheKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	UEnvQuery* QueryTemplate;

public:
	UEQSConsideration();

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const override;
};
