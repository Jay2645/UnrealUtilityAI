

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "UtilityAIInterface.h"
#include "SkillSet.h"
#include "DecisionMaker.h"

#include "UtilityIntelligence.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UTILITYAI_API UUtilityIntelligence : public UActorComponent, public IUtilityAIInterface
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FTimerHandle SkillTickTimer;
	UPROPERTY()
	FTimerHandle DecisionTickTimer;

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FMadeDecision> RecentDecisions;
	UPROPERTY(BlueprintReadOnly)
	AAIController* OurController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SkillTick;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DecisionTick;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<const USkillSet*> SkillSets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<const UDecisionMaker*> Decisions;

public:	
	UUtilityIntelligence();

private:
	UFUNCTION()
	void TickSkills();
	UFUNCTION()
	void TickDecisions();

	FDecisionContext GetDecisionContext() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual TArray<FMadeDecision> GetRecentDecisions_Implementation() const override;

public:
	UFUNCTION(BlueprintCallable)
	void MakeDecision(const UDecisionBase* Decision);
};
