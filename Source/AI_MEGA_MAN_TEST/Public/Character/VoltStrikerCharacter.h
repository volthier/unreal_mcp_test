#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CombatInterface.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"
#include "VoltStrikerCharacter.generated.h"

class UAbilitySystemComponent;
class UVoltStrikerAttributeSet;
class UGameplayAbility;
class USpringArmComponent;
class UCameraComponent;
class AVoltStrikerWeapon;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class AI_MEGA_MAN_TEST_API AVoltStrikerCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AVoltStrikerCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ICombatInterface
	virtual FVector GetProjectileSpawnLocation_Implementation() const override;
	virtual FVector GetProjectileSpawnDirection_Implementation() const override;
	virtual float GetWeaponChargeNormalized_Implementation() const override;
	virtual void SetWeaponChargeVisual_Implementation(float NormalizedCharge) override;
	virtual USkeletalMeshComponent* GetCombatMesh_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddStartupAbilities();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AVoltStrikerWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UVoltStrikerAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AVoltStrikerWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributesEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float DefaultCameraArmLength = 320.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ChargeCameraArmLength = 280.f;

	/** Default look-down pitch when possessing (degrees). Negative = look down at character. */
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float DefaultCameraPitch = -20.f;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void FirePressed();
	void FireReleased();
	void DashPressed();

	void InitAbilityActorInfo();
	void SpawnWeapon();

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AVoltStrikerWeapon> EquippedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	bool bAbilitiesGranted = false;
	float ChargeHoldTime = 0.f;
	float LastReleasedChargeHold = 0.f;
	bool bIsCharging = false;

public:
	float GetLastReleasedChargeHold() const { return LastReleasedChargeHold; }
};
