

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "UtilityEnvQueryInstance.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAI_API UUtilityEnvQueryInstance : public UEnvQueryInstanceBlueprintWrapper
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	float CachedScore;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	float GetBestScore();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	bool IsFinished() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	AActor* GetBestActor() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	FVector GetBestVector() const;
};
