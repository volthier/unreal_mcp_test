#pragma once

#include "CoreMinimal.h"
#include "Ability/AFGameplayAbility.h"
#include "GA_BasicShot.generated.h"

class AProjectileBolt;

UCLASS()
class PLOIDREKRPG_API UGA_BasicShot : public UAFGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BasicShot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	TSubclassOf<AProjectileBolt> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float CooldownSeconds = 0.18f;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
};
