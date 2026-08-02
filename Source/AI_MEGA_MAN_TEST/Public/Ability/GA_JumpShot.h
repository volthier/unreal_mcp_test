#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_BasicShot.h"
#include "GA_JumpShot.generated.h"

UCLASS()
class AI_MEGA_MAN_TEST_API UGA_JumpShot : public UGA_BasicShot
{
	GENERATED_BODY()

public:
	UGA_JumpShot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float AerialDamageMultiplier = 1.35f;
};
