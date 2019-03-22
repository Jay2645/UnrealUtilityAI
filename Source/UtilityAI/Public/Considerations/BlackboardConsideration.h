

#pragma once

#include "CoreMinimal.h"
#include "Considerations/Consideration.h"

#include "BehaviorTree/BehaviorTreeTypes.h"

#include "BlackboardConsideration.generated.h"

/**
 * Uses a key from a Blackboard to score a consideration.
 */
UCLASS()
class UTILITYAI_API UBlackboardConsideration : public UConsideration
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FName BlackboardKey;

public:
	UBlackboardConsideration();

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const override;
};
