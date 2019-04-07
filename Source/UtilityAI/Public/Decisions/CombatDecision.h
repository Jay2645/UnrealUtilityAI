

#pragma once

#include "CoreMinimal.h"
#include "UtilityEnvQueryInstance.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQuery.h"

#include "Decisions/Decision.h"

#include "CombatDecision.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAI_API UCombatDecision : public UDecision
{
	GENERATED_BODY()

public:
	// The DSE which gets used to select a target.
	// Each potential target will get passed into this DSE,
	// and the one with the highest score will be selected as
	// our new target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	const UNonSkillDecisionScoreEvaluator* FindTargetDSE;

	// How close we need to be to our target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float MaxCombatRange;

	// A key in a Blackboard that we use to save our target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	FName TargetBlackboardKey;

	// What template should be used in our EQS, if we're using it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	UEnvQuery* QueryTemplate;
	// How the EQS should be run
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;

public:
	UCombatDecision();

protected:
	virtual FDecisionContext FindTarget(const FDecisionContext& Context) const;
	virtual FVector FindEQSPosition(const FDecisionContext& Context) const;
	virtual FMadeDecision RunDecision_Implementation(const FDecisionContext& Context) const override;
	virtual void DoCombatLogic_Implementation(FMadeDecision& Decision, const FDecisionContext& Context) const;

	// Perform any special combat logic -- moving to a target, selecting a weapon, etc.
	// You can "nest" decisions in here, if you'd like.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DoCombatLogic(UPARAM(Ref) FMadeDecision& Decision, const FDecisionContext& Context) const;
};
