

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "SkillDecisionScoreEvaluator.h"
#include "Skill.h"

#include "SkillSet.generated.h"

/**
 * A selection of skills that an AI can have.
 * For example, a "weapon" SkillSet would have things like "shoot,"
 * "reload," "aim," "charge," etc.
 *
 * SkillSets are similar to DecisionMakers, but a DecisionMaker gets
 * run for things "in the background," like moving to a point.
 * SkillSets are run for "active" behaviors that should be able to
 * happen even when the character is moving -- for example, shooting.
 */
UCLASS()
class UTILITYAI_API USkillSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// All our skills, mapped to DSEs which score them.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<USkill*, USkillDecisionScoreEvaluator*> Skills;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score")
	USkill* FindBestSkill(const FDecisionContext& StartContext, FDecisionContext& OutContext, float& OutBestScore) const;
};
