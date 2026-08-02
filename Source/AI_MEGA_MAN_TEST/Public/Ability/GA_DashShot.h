#pragma once

#include "CoreMinimal.h"
#include "Ability/VoltStrikerGameplayAbility.h"
#include "GA_DashShot.generated.h"

class AVoltStrikerProjectile;

UCLASS()
class AI_MEGA_MAN_TEST_API UGA_DashShot : public UVoltStrikerGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_DashShot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashStrength = 1800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.18f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	TSubclassOf<AVoltStrikerProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float Damage = 20.f;
};
