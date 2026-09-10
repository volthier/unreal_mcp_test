#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * Modular integrated plasma cannon (barrel, core, fins, chamber, muzzle).
 */
UCLASS()
class PLOIDREKRPG_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachToOwnerMesh(USkeletalMeshComponent* OwnerMesh, FName SocketName = FName("weapon_r"));

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FTransform GetMuzzleTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetChargeVisual(float NormalizedCharge);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayMuzzleFlash();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> WeaponRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modules")
	TObjectPtr<UStaticMeshComponent> Barrel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modules")
	TObjectPtr<UStaticMeshComponent> Core;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modules")
	TObjectPtr<UStaticMeshComponent> CoolingFins;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modules")
	TObjectPtr<UStaticMeshComponent> ChargingChamber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modules")
	TObjectPtr<UStaticMeshComponent> Muzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sockets")
	TObjectPtr<USceneComponent> MuzzleFlashSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sockets")
	TObjectPtr<USceneComponent> ProjectileSpawnSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sockets")
	TObjectPtr<USceneComponent> FXSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UNiagaraComponent> ChargeParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UNiagaraSystem> MuzzleFlashSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> ChargeLoopSound;

	UPROPERTY(ReplicatedUsing = OnRep_ChargeNormalized, BlueprintReadOnly, Category = "Weapon")
	float ChargeNormalized = 0.f;

protected:
	UFUNCTION()
	void OnRep_ChargeNormalized();

	void ApplyChargeMaterials(float Normalized);
};
