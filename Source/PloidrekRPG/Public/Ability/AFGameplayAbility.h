#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AFGameplayAbility.generated.h"

class ARunnerCharacter;
class AProjectileBolt;

UCLASS(Abstract)
class PLOIDREKRPG_API UAFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAFGameplayAbility();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	ARunnerCharacter* GetRunnerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AProjectileBolt* SpawnProjectile(TSubclassOf<AProjectileBolt> ProjectileClass, float Damage, bool bPiercing = false, float AoERadius = 0.f, float Scale = 1.f);
};
