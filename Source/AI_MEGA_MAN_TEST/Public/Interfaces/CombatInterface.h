#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Shared combat hooks for characters that own a plasma weapon and charge state.
 */
class AI_MEGA_MAN_TEST_API ICombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	FVector GetProjectileSpawnDirection() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	float GetWeaponChargeNormalized() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void SetWeaponChargeVisual(float NormalizedCharge);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	USkeletalMeshComponent* GetCombatMesh() const;
};
