#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_BasicShot.h"
#include "GA_RapidShot.generated.h"

UCLASS()
class AI_MEGA_MAN_TEST_API UGA_RapidShot : public UGA_BasicShot
{
	GENERATED_BODY()

public:
	UGA_RapidShot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	int32 BurstCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float BurstInterval = 0.07f;

protected:
	void FireBurstShot();
	int32 ShotsRemaining = 0;
	FTimerHandle BurstTimer;
};
