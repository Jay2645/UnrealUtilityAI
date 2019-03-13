

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score")
	float CalculateScore(FDecisionContext Context, float MinToBeat);
};
