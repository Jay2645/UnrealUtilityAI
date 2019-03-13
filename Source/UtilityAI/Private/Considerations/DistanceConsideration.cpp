


#include "DistanceConsideration.h"

float UDistanceConsideration::Score_Implementation(const FDecisionContext& Context) const
{
	if (Radius <= 0.0f)
	{
		return 0.0f;
	}
	if (ResponseCurve == NULL)
	{
		return 0.0f;
	}
	AActor* target = Context.OurTarget;
	AAIController* us = Context.OurController;
	if (target == NULL || us == NULL)
	{
		return 0.0f;
	}
	APawn* ourPawn = us->GetPawn();
	if (ourPawn == NULL)
	{
		return 0.0f;
	}

	// All checks are valid, time for the actual function
	float dist = FVector::DistSquared(ourPawn->GetActorLocation(), target->GetActorLocation());
	float sqrRadius = Radius * Radius;
	// Return the actual value from the curve
	return ResponseCurve->GetFloatValue(FMath::Clamp(dist / sqrRadius, 0.0f, 1.0f));
}

UDistanceConsideration::UDistanceConsideration()
{
	Description = "A Consideration which uses the distance between us and our target to provide a score based on a curve.";
}
