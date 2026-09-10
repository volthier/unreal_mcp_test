#include "Character/RunnerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attributes/RunnerAttributeSet.h"
#include "Weapon/WeaponBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "InputCoreTypes.h"
#include "Components/CapsuleComponent.h"
#include "Character/RunnerAnimInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/SkeletalMesh.h"
#include "Ability/GA_BasicShot.h"
#include "Ability/GA_ChargeShot.h"
#include "Ability/GA_DashShot.h"

ARunnerCharacter::ARunnerCharacter()
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

	AttributeSet = CreateDefaultSubobject<URunnerAttributeSet>(TEXT("AttributeSet"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
}

void ARunnerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARunnerCharacter, EquippedWeapon);
}

void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FRotator ControlRot = PC->GetControlRotation();
		ControlRot.Pitch = DefaultCameraPitch;
		PC->SetControlRotation(ControlRot);

		// Capture the mouse so mouse-look (Turn/LookUp & Enhanced IA_Look) gets deltas.
		PC->SetShowMouseCursor(false);
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(true);
		PC->SetInputMode(InputMode);
	}
}

void ARunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Continuous keyboard movement (hold a key to keep moving).
	if (Controller && (bMoveForward || bMoveBack || bMoveLeft || bMoveRight))
	{
		const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Fwd = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		const float F = (bMoveForward ? 1.f : 0.f) - (bMoveBack ? 1.f : 0.f);
		const float R = (bMoveRight ? 1.f : 0.f) - (bMoveLeft ? 1.f : 0.f);
		AddMovementInput(Fwd, F);
		AddMovementInput(Right, R);
	}

	// Animation state vars + wall-slide detection.
	UCharacterMovementComponent* CMC = GetCharacterMovement();
	URunnerAnimInstance* Anim = Cast<URunnerAnimInstance>(GetMesh()->GetAnimInstance());
	if (CMC && Anim)
	{
		const float MaxWalk = CMC->MaxWalkSpeed > 0.f ? CMC->MaxWalkSpeed : 1.f;
		Anim->Speed = CMC->Velocity.Size() / MaxWalk;
		Anim->bIsInAir = CMC->IsFalling();

		bWallSlideActive = false;
		if (CMC->IsFalling())
		{
			FVector Facing(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f);
			if (Facing.IsNearlyZero()) { Facing = FVector(1.f, 0.f, 0.f); }
			FCollisionQueryParams QP(SCENE_QUERY_STAT(WallSlideProbe), false, this);
			FHitResult Hit;
			const FVector Origin = GetActorLocation() + FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.4f);
			const FVector End = Origin + Facing.GetSafeNormal() * 80.f;
			if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_WorldStatic, QP) &&
				Hit.bBlockingHit && FMath::Abs(Hit.Normal.Z) < 0.4f)
			{
				bWallSlideActive = true;
				LastWallNormal = Hit.Normal;
				FVector V = CMC->Velocity;
				V.Z = FMath::Max(V.Z, -280.f);
				CMC->Velocity = V;
			}
		}
		Anim->bIsWallSlide = bWallSlideActive;

		if (WallJumpTimer > 0.f) { WallJumpTimer -= DeltaSeconds; }
		bWallJumpActive = WallJumpTimer > 0.f;
		Anim->bIsWallJump = bWallJumpActive;
	}

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

void ARunnerCharacter::PossessedBy(AController* NewController)
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

void ARunnerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

UAbilitySystemComponent* ARunnerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARunnerCharacter::InitAbilityActorInfo()
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

void ARunnerCharacter::AddStartupAbilities()
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

void ARunnerCharacter::SpawnWeapon()
{
	if (!HasAuthority() || !WeaponClass || EquippedWeapon)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetActorTransform(), Params);
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToOwnerMesh(GetMesh(), FName("weapon_r"));
	}
}

void ARunnerCharacter::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToOwnerMesh(GetMesh(), FName("weapon_r"));
	}
}

void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Move);
		}
		if (LookAction)
		{
			EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Look);
		}
		if (JumpAction)
		{
			EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (FireAction)
		{
			EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ARunnerCharacter::FirePressed);
			EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ARunnerCharacter::FireReleased);
		}
		if (DashAction)
		{
			EIC->BindAction(DashAction, ETriggerEvent::Started, this, &ARunnerCharacter::DashPressed);
		}
	}

	// Robust default-style raw keyboard bindings (guaranteed movement/jump regardless of IMC mapping).
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &ARunnerCharacter::MoveForwardPressed);
	PlayerInputComponent->BindKey(EKeys::W, IE_Released, this, &ARunnerCharacter::MoveForwardReleased);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &ARunnerCharacter::MoveBackPressed);
	PlayerInputComponent->BindKey(EKeys::S, IE_Released, this, &ARunnerCharacter::MoveBackReleased);
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &ARunnerCharacter::MoveLeftPressed);
	PlayerInputComponent->BindKey(EKeys::A, IE_Released, this, &ARunnerCharacter::MoveLeftReleased);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &ARunnerCharacter::MoveRightPressed);
	PlayerInputComponent->BindKey(EKeys::D, IE_Released, this, &ARunnerCharacter::MoveRightReleased);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ARunnerCharacter::PlayerJump);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ACharacter::StopJumping);
	// Raw mouse look via the legacy Turn/LookUp axes defined in DefaultInput.ini (reliable, no IMC dependency).
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ARunnerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ARunnerCharacter::AddControllerPitchInput);
}

void ARunnerCharacter::Move(const FInputActionValue& Value)
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

void ARunnerCharacter::MoveForwardPressed() { bMoveForward = true; }
void ARunnerCharacter::MoveForwardReleased() { bMoveForward = false; }
void ARunnerCharacter::MoveBackPressed() { bMoveBack = true; }
void ARunnerCharacter::MoveBackReleased() { bMoveBack = false; }
void ARunnerCharacter::MoveLeftPressed() { bMoveLeft = true; }
void ARunnerCharacter::MoveLeftReleased() { bMoveLeft = false; }
void ARunnerCharacter::MoveRightPressed() { bMoveRight = true; }
void ARunnerCharacter::MoveRightReleased() { bMoveRight = false; }

void ARunnerCharacter::PlayerJump()
{
	if (bWallSlideActive && !LastWallNormal.IsNearlyZero())
	{
		// Wall-jump: launch up and away from the wall.
		if (UCharacterMovementComponent* CMC = GetCharacterMovement())
		{
			FVector LaunchVel = LastWallNormal * 900.f + FVector::UpVector * 720.f;
			CMC->Velocity = LaunchVel;
			CMC->SetMovementMode(MOVE_Falling);
			bWallSlideActive = false;
			bWallJumpActive = true;
			WallJumpTimer = 0.35f;
		}
	}
	else
	{
		Jump();
	}
}

void ARunnerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ARunnerCharacter::FirePressed()
{
	bIsCharging = true;
	ChargeHoldTime = 0.f;
}

void ARunnerCharacter::FireReleased()
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

void ARunnerCharacter::DashPressed()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(UGA_DashShot::StaticClass());
	}
}

FVector ARunnerCharacter::GetProjectileSpawnLocation_Implementation() const
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->GetProjectileSpawnLocation();
	}
	return GetMesh()->GetSocketLocation(FName("weapon_r"));
}

FVector ARunnerCharacter::GetProjectileSpawnDirection_Implementation() const
{
	if (FollowCamera)
	{
		return FollowCamera->GetForwardVector();
	}
	return GetActorForwardVector();
}

float ARunnerCharacter::GetWeaponChargeNormalized_Implementation() const
{
	return EquippedWeapon ? EquippedWeapon->ChargeNormalized : 0.f;
}

void ARunnerCharacter::SetWeaponChargeVisual_Implementation(float NormalizedCharge)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetChargeVisual(NormalizedCharge);
	}
}

USkeletalMeshComponent* ARunnerCharacter::GetCombatMesh_Implementation() const
{
	return GetMesh();
}

void ARunnerCharacter::ApplyProfile(const FRunnerCharacterProfile& Profile)
{
	CharacterProfile = Profile;
	AccentColor = Profile.AccentColor;

	// Corpo: enquanto nao existe arte propria, o chassi aponta para o manequim da engine
	// (Manny/Quinn), que ja traz esqueleto, physics asset e Control Rigs.
	if (USkeletalMesh* Body = Profile.BodyMesh.LoadSynchronous())
	{
		GetMesh()->SetSkeletalMesh(Body);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeScale3D(FVector(1.f));

		// Tinta de placeholder. Os materiais do manequim nao expoem parametro de cor, entao
		// isto so aparece quando existir material de overlay proprio (tarefa registrada).
		if (UMaterialInterface* BaseMaterial = GetMesh()->GetMaterial(0))
		{
			if (UMaterialInstanceDynamic* Dynamic = UMaterialInstanceDynamic::Create(BaseMaterial, this))
			{
				Dynamic->SetVectorParameterValue(TEXT("Color"), Profile.AccentColor);
				Dynamic->SetVectorParameterValue(TEXT("BaseColor"), Profile.AccentColor);
				GetMesh()->SetMaterial(0, Dynamic);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ApplyProfile: chassi '%s' sem malha apontada na DataTable — o personagem fica sem corpo."),
			*Profile.ChassisId.ToString());
	}

	UE_LOG(LogTemp, Log,
		TEXT("ApplyProfile: conta=%s chassi=%s classe=%s HP=%d CA=%d corpo=%s"),
		*Profile.AccountName, *Profile.ChassisId.ToString(), *Profile.ClassId.ToString(),
		Profile.MaxHitPoints, Profile.ArmorClass,
		Profile.BodyMesh.IsNull() ? TEXT("nenhum") : *Profile.BodyMesh.ToString());
}
