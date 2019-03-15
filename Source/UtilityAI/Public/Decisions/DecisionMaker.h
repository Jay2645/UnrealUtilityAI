

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Decision.h"

#include "DecisionMaker.generated.h"

/**
 * This is a list of DecisionScoreEvaluators, mapped to Decisions.
 * You can add or remove DecisionMakers to AI to enable special behaviors.
 * 
 * For example, if a character enters a tavern, you can have a special
 * "tavern" DecisionMaker object which gets associated with that character.
 * The "tavern" DecisionMaker will give increased weights that cause the
 * character to eat, drink, and be merry. Once the character leaves the
 * tavern, the DecisionMaker is removed and the character goes back to normal.
 *
 * A DecisionMaker is very similar to a SkillSet, except that a DecisionMaker
 * operates on *decisions* (i.e. actions that any AI can perform, in theory --
 * for example, "Move to a target"), whereas a SkillSet operates on *skills* 
 * (special actions AIs can perform in parallel to their decisions -- i.e. 
 * "Shoot a gun").
 */
UCLASS()
class UTILITYAI_API UDecisionMaker : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<UDecision*> Decisions;
	// A flat bonus given to this DecisionMaker, allowing for increased
	// weights for special behaviors associated with it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Bonus;

public:
	UDecisionMaker();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score")
	UDecision* FindBestDecision(const FDecisionContext& StartContext, FDecisionContext& OutContext, float& OutBestScore) const;
};
