

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Considerations/Consideration.h"

#include "DecisionScoreEvaluator.generated.h"

UENUM(BlueprintType)
enum class EEvaluatorPriority : uint8
{
	// Emotes, patrolling, etc.
	Ambient = 0,
	// Chasing after an enemy, taking cover
	Tactical = 10,
	// Firing a weapon, throwing a grenade
	Skill = 20,
	// Probably should take action soon, we're pretty concerned
	MinorEmergency = 30,
	// Going to die if we don't take action ASAP
	MajorEmergency = 40
};

/**
 * This takes a list of considerations and outputs a total weight for all of them.
 * Essentially, it evaluates a score which then gets used to select a decision.
 * Each decision should have a DecisionScoreEvaluator mapped to it, and when that
 * DecisionScoreEvaluator "wins," the decision gets executed.
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
	
	// What priority this DSE has.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEvaluatorPriority Priority;
public:
	UDecisionScoreEvaluator();

protected:
	virtual float GetMomentum_Implementation(const FDecisionContext& Context) const;

	// A compensation factor that should be applied to make a DSE more "elastic" when there are
	// many considerations to make.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score|Weight")
	virtual float GetCompensationFactor(float Score, float Bonus = 1.0f) const;
	// A weight given to this DSE, based on the assigned weight in the properties window and
	// the assigned priority.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score|Weight")
	virtual float GetWeight() const;
	// How likely are we to change our mind?
	// This can make the AI more or less likely to switch once committed.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score|Weight")
	float GetMomentum(const FDecisionContext& Context) const;

public:
	// The scoring function for this DSE.
	// Note that while each Consideration's score might be between 0 and 1, this can be any number.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Score")
	virtual float CalculateScore(FDecisionContext Context, float MinToBeat, float Bonus = 0.0f) const;
};
