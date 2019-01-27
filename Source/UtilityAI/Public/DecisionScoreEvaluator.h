

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Consideration.h"

#include "DecisionScoreEvaluator.generated.h"

UENUM(BlueprintType)
enum class EEvaluatorPriority : uint8
{
	// Emotes, patrolling, etc.
	Ambient = 1,
	// Chasing after an enemy, taking cover
	Tactical = 2,
	// Firing a weapon, throwing a grenade
	Skill = 3,
	// Probably should take action soon, we're pretty concerned
	MinorEmergency = 4,
	// Going to die if we don't take action ASAP
	MajorEmergency = 5
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class UTILITYAI_API UDecisionScoreEvaluator : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// A simple description of what this evaluator does.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	// A list of all considerations that this evaluator takes into account.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<const UConsideration*> Considerations;

	// A weight assigned to this DSE.
	// Higher weights will inflate the score.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEvaluatorPriority Priority;
public:
	UDecisionScoreEvaluator();

protected:
	virtual float GetWeight();

	virtual float CalculateScore(const FDecisionContext& Context, float MinToBeat, float Bonus = 1.0f);
};
