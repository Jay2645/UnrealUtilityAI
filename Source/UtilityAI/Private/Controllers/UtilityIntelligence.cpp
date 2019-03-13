


#include "UtilityIntelligence.h"

// Sets default values for this component's properties
UUtilityIntelligence::UUtilityIntelligence()
{
	PrimaryComponentTick.bCanEverTick = false;
	SkillTick = 0.05f;
	DecisionTick = 0.125f;
}


void UUtilityIntelligence::TickSkills()
{
	FDecisionContext context = GetDecisionContext();
	float bestScore = -1.0f;
	const USkill* best = NULL;
	for (const USkillSet* skillSet : SkillSets)
	{
		float currentScore = 0.0f;
		const USkill* skill = skillSet->FindBestSkill(context, currentScore, bestScore);
		if (currentScore > bestScore)
		{
			bestScore = currentScore;
			best = skill;
		}
	}

	// Make the actual decision
	if (best != NULL)
	{
		MakeDecision(best);
	}
}

void UUtilityIntelligence::TickDecisions()
{
	FDecisionContext context = GetDecisionContext();
	float bestScore = -1.0f;
	const UDecisionBase* best = NULL;
	for (const UDecisionMaker* decsionMaker : Decisions)
	{
		float currentScore = 0.0f;
		const UDecisionBase* decision = decsionMaker->FindBestDecision(context, currentScore, bestScore);
		if (currentScore > bestScore)
		{
			bestScore = currentScore;
			best = decision;
		}
	}

	// Make the actual decision
	if (best != NULL)
	{
		MakeDecision(best);
	}
}

FDecisionContext UUtilityIntelligence::GetDecisionContext() const
{
	FDecisionContext context;
	context.OurController = OurController;
	context.Decision = NULL;
	context.OurTarget = NULL;
	return context;
}

// Called when the game starts
void UUtilityIntelligence::BeginPlay()
{
	Super::BeginPlay();

	OurController = Cast<AAIController>(GetOwner());

#if !UE_BUILD_SHIPPING
	if (OurController == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("A UtilityIntelligence needs to be attached to an AI Controller."));
		return;
	}
#endif

	FTimerManager& worldManager = GetWorld()->GetTimerManager();
	worldManager.SetTimer(DecisionTickTimer, this, &UUtilityIntelligence::TickDecisions, DecisionTick, true, 0.0f);
	worldManager.SetTimer(SkillTickTimer, this, &UUtilityIntelligence::TickSkills, SkillTick, true, 0.0f);
}

void UUtilityIntelligence::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

TArray<FMadeDecision> UUtilityIntelligence::GetRecentDecisions_Implementation() const
{
	return RecentDecisions;
}

void UUtilityIntelligence::MakeDecision(const UDecisionBase* Decision)
{
	if (Decision == NULL)
	{
		return;
	}

	RecentDecisions.Add(Decision->RunDecision(OurController));
}
