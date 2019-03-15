#pragma once

#include "CoreMinimal.h"
#include "UnrealString.h"
#include "AIController.h"
#include "Curves/CurveFloat.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "DecisionBase.h"

#include "Consideration.generated.h"


/*
* A context that checks if we have make a decision recently.
*/
USTRUCT(BlueprintType)
struct UTILITYAI_API FCooldownDecisionContext
{
	GENERATED_BODY()

public:
	// The Decision that the Consideration should check.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UDecisionBase* Decision;
	// How long we should wait until after the given action before
	// this context can be valid again.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MinValue = "0.1"))
	float Cooldown;
	// If this is set to true, then this context is only valid during
	// the given time period (instead of only valid once the time
	// period has elapsed).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInvert;
};

/**
 * A Consideration is the base class governing the choices that our AI makes.
 * Examples of possible considerations could be "how much health do I have?"
 * It takes context about what is going on inside the game and uses that to
 * provide a score, which gets used as a multiplier when making choices.
 * This score is normalized between 0 and 1.
 */
UCLASS(Abstract, Blueprintable)
class UTILITYAI_API UConsideration : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// A simple description of what this consideration does.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	// A curve used in our scoring function to dictate behavior.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* ResponseCurve;

	// Only run this consideration if the target is within a certain radius of our controller.
	// A value of <= 0 disables the radius check.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	// How long before this Consideration can run again after it succeeds.
	// A value of <= 0 disables cooldown check.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime;
	// Whether we should invert the cooldown timer.
	// Note that if you enable this, you should make sure that any Decisions
	// associated with this Consideration can be called somewhere else.
	// Otherwise, they'll never get called!
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInvertCooldown;

	// All tags that need to be on the pawn associated with the controller which is
	// calling the score function for it to consider returning a value.
	// Skipped if the pawn doesn't implement IGameplayTagAssetInterface.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer OurRequiredTags;

	// All tags that need to be on the controller calling the score function for it to
	// consider returning a value.
	// Skipped if the target doesn't implement IGameplayTagAssetInterface.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TargetRequiredTags;

	// We check to see if the pawn has performed any of these decisions within the specified
	// time period. If we've performed one of these decisions recently, then we skip this
	// consideration. An empty array means we won't check anything.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCooldownDecisionContext> CooldownDecisions;

public:
	UConsideration();

private:
	bool CanScore(const FDecisionContext& Context) const;
	bool IsInRange(AActor* Target, AAIController* Us) const;
	bool HasAllRequiredGameplayTags(AActor* target, AAIController* us) const;
	bool CooldownIsValid(const FDecisionContext& Context) const;
	bool DecisionsAreValid(const FDecisionContext& Context, const TArray<FMadeDecision>& Decisions, float Cooldown, bool bInvert) const;

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const;

	// The heart of the Consideration.
	// Returns a multiplier which gives data about how likely it is that an AI should
	// choose this action.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Consideration")
	float Score(const FDecisionContext& Context) const;

public: 
	// Calculates a score for this Consideration given a context.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Consideration")
	float CalculateScore(const FDecisionContext& Context) const;
};
