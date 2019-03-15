

#pragma once

#include "CoreMinimal.h"
#include "Consideration.h"
#include "DistanceConsideration.generated.h"

/**
 * This uses the provided curve to provide the score for the overall consideration.
 * The curve will be extrapolated to match the bounds of the radius, if one is provided.
 */
UCLASS()
class UTILITYAI_API UDistanceConsideration : public UConsideration
{
	GENERATED_BODY()

public:
	UDistanceConsideration();

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const override;
};
