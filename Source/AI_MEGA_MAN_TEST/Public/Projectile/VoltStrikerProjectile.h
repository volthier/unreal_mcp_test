#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"
#include "VoltStrikerProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Pooling-friendly replicated plasma projectile.
 */
UCLASS()
class AI_MEGA_MAN_TEST_API AVoltStrikerProjectile : public AActor
{
	GENERATED_BODY()

public:
	AVoltStrikerProjectile();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void LifeSpanExpired() override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void LaunchProjectile(const FVector& Direction, AActor* InInstigator, float InDamage, bool bInPiercing = false, float InAoERadius = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void ResetProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> TrailComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bPiercing = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float AoERadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float LifeSeconds = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UNiagaraSystem> ImpactFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TSubclassOf<UCameraShakeBase> ImpactCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	float ExplosionLightIntensity = 8000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	float ExplosionLightDuration = 0.15f;

	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayImpact(FVector Location, FVector Normal);

	void ApplyDamageAt(const FHitResult& Hit);
	void DeactivateProjectile();

	UPROPERTY(Replicated)
	bool bIsActive = false;

	TSet<TWeakObjectPtr<AActor>> HitActors;
};
