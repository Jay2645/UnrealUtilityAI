

#pragma once

#include "CoreMinimal.h"
#include "DecisionBase.h"
#include "Skill.generated.h"

/**
 * A skill is an action that a character can perform alongside things like movement.
 * Essentially, if "move to" is the background action, "shoot" would be the skill --
 * the foreground action.
 *
 * Skills are designed to have multiple "variants" that can be selected. For example,
 * "Melee attack 1," "Melee attack 2," etc.
 */
UCLASS(Abstract)
class UTILITYAI_API USkill : public UDecisionBase
{
	GENERATED_BODY()
};
