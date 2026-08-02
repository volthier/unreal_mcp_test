#include "Character/VoltStrikerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attributes/VoltStrikerAttributeSet.h"
#include "Weapon/VoltStrikerWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "Ability/GA_BasicShot.h"
#include "Ability/GA_ChargeShot.h"
#include "Ability/GA_DashShot.h"

AVoltStrikerCharacter::AVoltStrikerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Mesh is authored/exported in centimeters (UE units). Origin at feet → drop by capsule half-height.
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeScale3D(FVector(1.f));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultCameraArmLength;
	CameraBoom->bUsePawnControlRotation = true;
	// Elevated over-shoulder boom so the camera sits outside the mesh and looks down ~20°.
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 75.f);
	CameraBoom->TargetOffset = FVector(0.f, 0.f, 20.f);
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeSize = 12.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetRelativeLocation(FVector::ZeroVector);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UVoltStrikerAttributeSet>(TEXT("AttributeSet"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
}

void AVoltStrikerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVoltStrikerCharacter, EquippedWeapon);
}

void AVoltStrikerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FRotator ControlRot = PC->GetControlRotation();
		ControlRot.Pitch = DefaultCameraPitch;
		PC->SetControlRotation(ControlRot);
	}
}

void AVoltStrikerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsCharging)
	{
		ChargeHoldTime += DeltaSeconds;
		const float Normalized = FMath::Clamp(ChargeHoldTime / 2.5f, 0.f, 1.f);
		SetWeaponChargeVisual_Implementation(Normalized);

		if (CameraBoom)
		{
			CameraBoom->TargetArmLength = FMath::FInterpTo(
				CameraBoom->TargetArmLength,
				FMath::Lerp(DefaultCameraArmLength, ChargeCameraArmLength, Normalized),
				DeltaSeconds,
				6.f);
		}

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->PlayDynamicForceFeedback(Normalized * 0.45f, 0.05f, true, true, true, true);
		}
	}
	else if (CameraBoom && !FMath::IsNearlyEqual(CameraBoom->TargetArmLength, DefaultCameraArmLength, 1.f))
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, DefaultCameraArmLength, DeltaSeconds, 8.f);
	}
}

void AVoltStrikerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddStartupAbilities();

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		FRotator ControlRot = PC->GetControlRotation();
		ControlRot.Pitch = DefaultCameraPitch;
		PC->SetControlRotation(ControlRot);
	}
}

void AVoltStrikerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

UAbilitySystemComponent* AVoltStrikerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AVoltStrikerCharacter::InitAbilityActorInfo()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (HasAuthority() && DefaultAttributesEffect)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);
		const FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.f, Context);
		if (Spec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

void AVoltStrikerCharacter::AddStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent || bAbilitiesGranted)
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
		AbilitySystemComponent->GiveAbility(Spec);
	}

	bAbilitiesGranted = true;
}

void AVoltStrikerCharacter::SpawnWeapon()
{
	if (!HasAuthority() || !WeaponClass || EquippedWeapon)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	EquippedWeapon = GetWorld()->SpawnActor<AVoltStrikerWeapon>(WeaponClass, GetActorTransform(), Params);
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToOwnerMesh(GetMesh(), FName("weapon_r"));
	}
}

void AVoltStrikerCharacter::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToOwnerMesh(GetMesh(), FName("weapon_r"));
	}
}

void AVoltStrikerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVoltStrikerCharacter::Move);
		}
		if (LookAction)
		{
			EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVoltStrikerCharacter::Look);
		}
		if (JumpAction)
		{
			EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (FireAction)
		{
			EIC->BindAction(FireAction, ETriggerEvent::Started, this, &AVoltStrikerCharacter::FirePressed);
			EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &AVoltStrikerCharacter::FireReleased);
		}
		if (DashAction)
		{
			EIC->BindAction(DashAction, ETriggerEvent::Started, this, &AVoltStrikerCharacter::DashPressed);
		}
	}
}

void AVoltStrikerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, Axis.Y);
		AddMovementInput(Right, Axis.X);
	}
}

void AVoltStrikerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AVoltStrikerCharacter::FirePressed()
{
	bIsCharging = true;
	ChargeHoldTime = 0.f;
}

void AVoltStrikerCharacter::FireReleased()
{
	bIsCharging = false;
	LastReleasedChargeHold = ChargeHoldTime;
	ChargeHoldTime = 0.f;
	SetWeaponChargeVisual_Implementation(0.f);

	if (!AbilitySystemComponent)
	{
		return;
	}

	if (LastReleasedChargeHold < 0.2f)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(UGA_BasicShot::StaticClass());
	}
	else
	{
		AbilitySystemComponent->TryActivateAbilityByClass(UGA_ChargeShot::StaticClass());
	}
}

void AVoltStrikerCharacter::DashPressed()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(UGA_DashShot::StaticClass());
	}
}

FVector AVoltStrikerCharacter::GetProjectileSpawnLocation_Implementation() const
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->GetProjectileSpawnLocation();
	}
	return GetMesh()->GetSocketLocation(FName("weapon_r"));
}

FVector AVoltStrikerCharacter::GetProjectileSpawnDirection_Implementation() const
{
	if (FollowCamera)
	{
		return FollowCamera->GetForwardVector();
	}
	return GetActorForwardVector();
}

float AVoltStrikerCharacter::GetWeaponChargeNormalized_Implementation() const
{
	return EquippedWeapon ? EquippedWeapon->ChargeNormalized : 0.f;
}

void AVoltStrikerCharacter::SetWeaponChargeVisual_Implementation(float NormalizedCharge)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetChargeVisual(NormalizedCharge);
	}
}

USkeletalMeshComponent* AVoltStrikerCharacter::GetCombatMesh_Implementation() const
{
	return GetMesh();
}
