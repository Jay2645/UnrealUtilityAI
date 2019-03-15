

#pragma once

#include "CoreMinimal.h"
#include "Decisions/Decision.h"
#include "CombatDecision.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAI_API UCombatDecision : public UDecision
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UNonSkillDecisionScoreEvaluator* FindTargetDSE;

	// How close we need to be to our target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxCombatRange;

public:
	UCombatDecision();

protected:
	virtual APawn* FindTarget(const FDecisionContext& Context) const;
	virtual TEnumAsByte<EBTNodeResult::Type> MoveToTarget(const FDecisionContext& Context, APawn* Target) const;
	virtual FMadeDecision RunDecision_Implementation(const FDecisionContext& Context) const override;
};
