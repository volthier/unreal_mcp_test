#include "Weapon/VoltStrikerWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"

AVoltStrikerWeapon::AVoltStrikerWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	SetRootComponent(WeaponRoot);

	Barrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));
	Barrel->SetupAttachment(WeaponRoot);
	Barrel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Core = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Core"));
	Core->SetupAttachment(WeaponRoot);
	Core->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CoolingFins = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoolingFins"));
	CoolingFins->SetupAttachment(WeaponRoot);
	CoolingFins->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ChargingChamber = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChargingChamber"));
	ChargingChamber->SetupAttachment(WeaponRoot);
	ChargingChamber->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Muzzle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(WeaponRoot);
	Muzzle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MuzzleFlashSocket = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleFlash"));
	MuzzleFlashSocket->SetupAttachment(Muzzle);
	MuzzleFlashSocket->SetRelativeLocation(FVector(18.f, 0.f, 0.f));

	ProjectileSpawnSocket = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	ProjectileSpawnSocket->SetupAttachment(Muzzle);
	ProjectileSpawnSocket->SetRelativeLocation(FVector(22.f, 0.f, 0.f));

	FXSocket = CreateDefaultSubobject<USceneComponent>(TEXT("FX"));
	FXSocket->SetupAttachment(Core);

	ChargeParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChargeParticles"));
	ChargeParticles->SetupAttachment(FXSocket);
	ChargeParticles->bAutoActivate = false;
}

void AVoltStrikerWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVoltStrikerWeapon, ChargeNormalized);
}

void AVoltStrikerWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AVoltStrikerWeapon::AttachToOwnerMesh(USkeletalMeshComponent* OwnerMesh, FName SocketName)
{
	if (!OwnerMesh)
	{
		return;
	}

	AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

FTransform AVoltStrikerWeapon::GetMuzzleTransform() const
{
	return MuzzleFlashSocket ? MuzzleFlashSocket->GetComponentTransform() : GetActorTransform();
}

FVector AVoltStrikerWeapon::GetProjectileSpawnLocation() const
{
	return ProjectileSpawnSocket ? ProjectileSpawnSocket->GetComponentLocation() : GetActorLocation();
}

void AVoltStrikerWeapon::SetChargeVisual(float NormalizedCharge)
{
	ChargeNormalized = FMath::Clamp(NormalizedCharge, 0.f, 1.f);
	ApplyChargeMaterials(ChargeNormalized);

	if (ChargeParticles)
	{
		if (ChargeNormalized > 0.05f)
		{
			if (!ChargeParticles->IsActive())
			{
				ChargeParticles->Activate(true);
			}
			ChargeParticles->SetFloatParameter(FName("Charge"), ChargeNormalized);
		}
		else
		{
			ChargeParticles->Deactivate();
		}
	}
}

void AVoltStrikerWeapon::OnRep_ChargeNormalized()
{
	ApplyChargeMaterials(ChargeNormalized);
}

void AVoltStrikerWeapon::ApplyChargeMaterials(float Normalized)
{
	TArray<UStaticMeshComponent*> Modules = { Barrel, Core, CoolingFins, ChargingChamber, Muzzle };
	for (UStaticMeshComponent* Module : Modules)
	{
		if (!Module)
		{
			continue;
		}
		for (int32 Index = 0; Index < Module->GetNumMaterials(); ++Index)
		{
			UMaterialInstanceDynamic* MID = Module->CreateAndSetMaterialInstanceDynamic(Index);
			if (MID)
			{
				MID->SetScalarParameterValue(FName("EmissionStrength"), FMath::Lerp(1.f, 8.f, Normalized));
				MID->SetVectorParameterValue(FName("PlasmaColor"), FLinearColor(0.2f, 0.7f + Normalized * 0.3f, 1.f));
			}
		}
	}
}

void AVoltStrikerWeapon::PlayMuzzleFlash()
{
	if (MuzzleFlashSystem && MuzzleFlashSocket)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			MuzzleFlashSystem,
			MuzzleFlashSocket->GetComponentLocation(),
			MuzzleFlashSocket->GetComponentRotation());
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetProjectileSpawnLocation());
	}
}
