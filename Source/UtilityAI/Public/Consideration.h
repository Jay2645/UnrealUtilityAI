

#pragma once

#include "CoreMinimal.h"
#include "UnrealString.h"
#include "Curves/CurveFloat.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Controller.h"
#include "Consideration.generated.h"

USTRUCT(BlueprintType)
struct UTILITYAI_API FDecisionContext
{
	GENERATED_BODY()

public:
	// The controller which is generating our considerations.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AController* OurController;
	// The actor which we are currently "thinking about."
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OurTarget;
};

/**
 * A Consideration is the base class governing the choices that our AI makes.
 * Examples of possible considerations could be "how much health do I have?"
 * It takes context about what is going on inside the game and uses that to
 * provide a score, which gets used as a multiplier when making choices.
 */
UCLASS(Blueprintable)
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

public:
	UConsideration();

protected:
	virtual float Score_Implementation(const FDecisionContext& Context) const;

public:
	// The heart of the Consideration.
	// Returns a multiplier which gives data about how likely it is that an AI should
	// choose this action. 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Consideration")
	float Score(const FDecisionContext& Context) const;
};
