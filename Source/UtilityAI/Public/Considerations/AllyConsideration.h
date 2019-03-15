

#pragma once

#include "CoreMinimal.h"
#include "Considerations/Consideration.h"
#include "AllyConsideration.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAI_API UAllyConsideration : public UConsideration
{
	GENERATED_BODY()

public:
	// How many allies it takes to send a value of 1 to the curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	uint8 MaxAllies;

public:
	UAllyConsideration();

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const override;
};
