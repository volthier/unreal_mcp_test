#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CombatInterface.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"
#include "Data/RunnerCharacterProfile.h"
#include "RunnerCharacter.generated.h"

class UAbilitySystemComponent;
class URunnerAttributeSet;
class UGameplayAbility;
class USpringArmComponent;
class UCameraComponent;
class AWeaponBase;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PLOIDREKRPG_API ARunnerCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ARunnerCharacter();

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

	/** Aplica a ficha escolhida na criacao: corpo (malha) + cor de destaque do chassi. */
	UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
	void ApplyProfile(const FRunnerCharacterProfile& Profile);

	/** Estado de locomocao atual (decide qual animacao o corpo toca). */
	UPROPERTY(BlueprintReadOnly, Category = "Runner|Animacao")
	ERunnerLocomotion LocomotionState = ERunnerLocomotion::Idle;

	/**
	 * Enquanto nao existe Animation Blueprint proprio, o corpo anima por animacao unica
	 * (idle / andar / correr / no ar). Desligue quando o AnimBP existir.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Animacao")
	bool bUseSingleNodeLocomotion = true;

	/** Escolhe e toca a animacao conforme o movimento atual. */
	UFUNCTION(BlueprintCallable, Category = "Runner|Animacao")
	void UpdateLocomotionAnimation();

	/** Ficha ativa deste personagem (chassi + classe + numeros derivados). */
	UPROPERTY(BlueprintReadOnly, Category = "Runner|Criacao")
	FRunnerCharacterProfile CharacterProfile;

	/** Cor de destaque do chassi — placeholder visual ate existir arte propria. */
	UPROPERTY(BlueprintReadOnly, Category = "Runner|Criacao")
	FLinearColor AccentColor = FLinearColor::White;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<URunnerAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

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

	// Robust default-style raw keyboard handlers (guaranteed movement regardless of IMC mapping).
	void MoveForwardPressed();
	void MoveForwardReleased();
	void MoveBackPressed();
	void MoveBackReleased();
	void MoveLeftPressed();
	void MoveLeftReleased();
	void MoveRightPressed();
	void MoveRightReleased();

	// Jump (supports wall-jump).
	void PlayerJump();

	void InitAbilityActorInfo();
	void SpawnWeapon();

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeaponBase> EquippedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	bool bAbilitiesGranted = false;
	float ChargeHoldTime = 0.f;
	float LastReleasedChargeHold = 0.f;
	bool bIsCharging = false;

	// Continuous keyboard movement held-state.
	bool bMoveForward = false;
	bool bMoveBack = false;
	bool bMoveLeft = false;
	bool bMoveRight = false;

	// Wall-slide / wall-jump state.
	bool bWallSlideActive = false;
	bool bWallJumpActive = false;
	float WallJumpTimer = 0.f;
	FVector LastWallNormal = FVector::ZeroVector;

public:
	float GetLastReleasedChargeHold() const { return LastReleasedChargeHold; }
};
