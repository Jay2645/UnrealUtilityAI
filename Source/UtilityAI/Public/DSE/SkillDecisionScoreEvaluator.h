

#pragma once

#include "CoreMinimal.h"

#include "DecisionScoreEvaluator.h"

#include "SkillDecisionScoreEvaluator.generated.h"

/**
 * This type of DecisionScoreEvaluator is meant to decide things about skills.
 * Skills are special actions that a character can take -- "shoot gun," "swing sword," etc.
 * They are *not* actions like "move to target" -- since only 1 skill will be executed at a
 * time, you should use a Non-Skill DSE.
 *
 * Additionally, this type of DSE isn't inherently associated with 1 particular skill.
 * Instead, it can be a set of related skills -- for example, 4 different melee attack "variants."
 */
UCLASS()
class UTILITYAI_API USkillDecisionScoreEvaluator : public UDecisionScoreEvaluator
{
	GENERATED_BODY()
public:

};
