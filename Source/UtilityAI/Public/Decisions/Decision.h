

#pragma once

#include "CoreMinimal.h"

#include "DecisionBase.h"
#include "NonSkillDecisionScoreEvaluator.h"

#include "Decision.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UTILITYAI_API UDecision : public UDecisionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UNonSkillDecisionScoreEvaluator* DecisionScoreEvaluator;

public:
	// Calculates the score for this decision.
	// By default, just returns the output of the DecisionScoreEvaluator, if one is hooked up.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score")
	float CalculateScore(FDecisionContext& Context, float MinToBeat, float Bonus = 0.0f) const;
};
