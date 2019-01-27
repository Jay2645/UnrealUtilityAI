

#pragma once

#include "CoreMinimal.h"

#include "DecisionScoreEvaluator.h"
#include "Decision.h"

#include "NonSkillDecisionScoreEvaluator.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAI_API UNonSkillDecisionScoreEvaluator : public UDecisionScoreEvaluator
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UDecision* Decision;
};
