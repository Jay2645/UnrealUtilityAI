

#pragma once

#include "CoreMinimal.h"

#include "DecisionScoreEvaluator.h"

#include "NonSkillDecisionScoreEvaluator.generated.h"

/**
 * This is a DSE designed to be used for decisions which are *not* skills.
 * This runs in parallel to the Skill DSE to determine AI actions like "move to target"
 * and whatnot -- things which happen in the "background," as it were.
 */
UCLASS()
class UTILITYAI_API UNonSkillDecisionScoreEvaluator : public UDecisionScoreEvaluator
{
	GENERATED_BODY()
};
