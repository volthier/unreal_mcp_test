#pragma once

#include "CoreMinimal.h"
#include "Ability/VoltStrikerGameplayAbility.h"
#include "GA_BasicShot.generated.h"

class AVoltStrikerProjectile;

UCLASS()
class AI_MEGA_MAN_TEST_API UGA_BasicShot : public UVoltStrikerGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BasicShot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	TSubclassOf<AVoltStrikerProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float CooldownSeconds = 0.18f;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
};
