


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

void UUtilityIntelligence::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopAI();
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

void UUtilityIntelligence::StartAI(AAIController* Controller, UBlackboardComponent* Blackboard)
{
	OurController = Controller;
	AIBlackboard = Blackboard;

	FTimerManager& worldManager = GetWorld()->GetTimerManager();
	worldManager.SetTimer(DecisionTickTimer, this, &UUtilityIntelligence::TickDecisions, DecisionTick, true, 0.0f);
	worldManager.SetTimer(SkillTickTimer, this, &UUtilityIntelligence::TickSkills, SkillTick, true, 0.0f);

}

void UUtilityIntelligence::StopAI()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}