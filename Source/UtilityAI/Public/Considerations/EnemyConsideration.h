

#pragma once

#include "CoreMinimal.h"
#include "Considerations/Consideration.h"
#include "EnemyConsideration.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAI_API UEnemyConsideration : public UConsideration
{
	GENERATED_BODY()

public:
	// How many enemies it takes to send a value of 1 to the curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	uint8 MaxEnemies;

public:
	UEnemyConsideration();

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const override;
};
