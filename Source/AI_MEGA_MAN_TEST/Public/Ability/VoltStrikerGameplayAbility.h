#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "VoltStrikerGameplayAbility.generated.h"

class AVoltStrikerCharacter;
class AVoltStrikerProjectile;

UCLASS(Abstract)
class AI_MEGA_MAN_TEST_API UVoltStrikerGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UVoltStrikerGameplayAbility();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AVoltStrikerCharacter* GetVoltStrikerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AVoltStrikerProjectile* SpawnProjectile(TSubclassOf<AVoltStrikerProjectile> ProjectileClass, float Damage, bool bPiercing = false, float AoERadius = 0.f, float Scale = 1.f);
};
