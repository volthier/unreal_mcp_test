#pragma once

#include "CoreMinimal.h"
#include "Ability/VoltStrikerGameplayAbility.h"
#include "GA_ChargeShot.generated.h"

class AVoltStrikerProjectile;

UCLASS()
class AI_MEGA_MAN_TEST_API UGA_ChargeShot : public UVoltStrikerGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ChargeShot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	TSubclassOf<AVoltStrikerProjectile> ChargeProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float Level1HoldSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float Level2HoldSeconds = 1.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float Level3HoldSeconds = 2.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float DamageL1 = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float DamageL2 = 70.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float DamageL3 = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float AoEL1 = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float AoEL2 = 140.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shot")
	float AoEL3 = 220.f;
};
