

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EQSConsideration.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "GameplayTagAssetInterface.h"

#include "UtilityAIInterface.h"
#include "SkillSet.h"
#include "DecisionMaker.h"
#include "UtilityEnvQueryInstance.h"
#include "DecisionBase.h"

#include "UtilityIntelligence.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UTILITYAI_API UUtilityIntelligence : public UActorComponent, public IUtilityAIInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FTimerHandle SkillTickTimer;
	UPROPERTY()
	FTimerHandle DecisionTickTimer;

	UPROPERTY(Transient)
	AActor* ContextTarget;

	UPROPERTY(Transient)
	bool bPawnHasGameplayTags;
	UPROPERTY(Transient)
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<FMadeDecision> RecentDecisions;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	AAIController* OurController;
	
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* AIBlackboard;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Decisions")
	float SkillTick;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Decisions")
	float DecisionTick;

	UPROPERTY(Transient, EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	TSet<APawn*> ValidTargets;
	UPROPERTY(Transient, EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	TSet<APawn*> ValidAllies;
	UPROPERTY(Transient, EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	TSet<FVector> AreasOfInterest;

	// Since Decisions and Considerations are stored on disk, they can't be modified
	// on an instance-by-instance basis without some serious side effects (notably,
	// the loss of the ability to store "presets" on the disk).
	// However, the EQS system needs an instance of an environmental query to modify.
	// A compromise is to store the EQS instance on the INTELLIGENCE, with a map to
	// the Decision/Consideration using the EQS. That way, the EQS can be modified
	// and the system can easily look up which intelligence is "supposed" to handle
	// the data without modifying anything on the disk.
	UPROPERTY(Transient)
	TMap<const UDecisionBase*, UUtilityEnvQueryInstance*> DecisionEQSHelpers;
	UPROPERTY(Transient)
	TMap<const UEQSConsideration*, UUtilityEnvQueryInstance*> ConsiderationEQSHelpers;

public:
	// Fixed distance added to threshold between AI and goal location in destination reach test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	float AcceptableRadius;
	// "None" will result in default filter being used 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	TSubclassOf<UNavigationQueryFilter> FilterClass;
	// Should we allow strafing towards our goal?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	uint32 bAllowStrafe : 1;
	// If set, use incomplete path when goal can't be reached
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	uint32 bAllowPartialPath : 1;
	// If set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Pathfinding")
	uint32 bReachTestIncludesAgentRadius : 1;
	// If set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	uint32 bReachTestIncludesGoalRadius : 1;
	// If set, goal location will be projected on navigation data (navmesh) before using
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Pathfinding")
	uint32 bProjectGoalLocation : 1;
	// If set, move will use pathfinding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Pathfinding")
	uint32 bUsePathfinding : 1;
	// If set, path to goal actor will update itself when actor moves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Pathfinding")
	uint32 bTrackMovingGoal : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decisions")
	TSet<const USkillSet*> SkillSets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decisions")
	TSet<const UDecisionMaker*> Decisions;

public:
	UUtilityIntelligence();

private:
	UFUNCTION()
	void TickSkills();
	UFUNCTION()
	void TickDecisions();

	FDecisionContext GetDecisionContext();

protected:
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual TArray<FMadeDecision> GetRecentDecisions_Implementation() const override;

	UFUNCTION()
	virtual void OnTargetDestroyed(AActor* Target);

public:
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	virtual void RemoveGameplayTag(const FGameplayTag& TagToRemove);
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	virtual void AddGameplayTag(const FGameplayTag& TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence")
	void MakeDecision(const FDecisionContext& Context);

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence")
	void StartAI(AAIController* Controller, UBlackboardComponent* Blackboard);
	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence")
	void StopAI();

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void UpdateContextTarget(AActor* NewTarget);
	
	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void AddNewPossibleTarget(APawn* NewTarget);
	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void RemovePossibleTarget(APawn* Target);

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	TSet<APawn*> GetPossibleTargets() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void AddNewAlly(APawn* NewAlly);
	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void RemoveAlly(APawn* Ally);

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	TSet<APawn*> GetAllies() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void AddAreaOfInterest(const FVector& InterestingPoint);
	UFUNCTION(BlueprintCallable, Category = "AI|Utility AI|Intelligence|Context")
	void RemoveAreaOfInterest(const FVector& InterestingPoint);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Intelligence")
	AAIController* GetController() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Utility AI|Intelligence")
	APawn* GetPawn() const;

	
	UUtilityEnvQueryInstance* GetDecisonEQS(const UDecisionBase* Decision);
	UUtilityEnvQueryInstance* RunDecisonEQS(const UDecisionBase* Decision, const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, UEnvQuery* QueryTemplate);
	
	UUtilityEnvQueryInstance* GetConsiderationEQS(const UEQSConsideration* Consideration);
	UUtilityEnvQueryInstance* RunConsiderationEQS(const UEQSConsideration* Consideration, const FDecisionContext& Context, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, UEnvQuery* QueryTemplate);
};
