#include "Projectile/ProjectileBolt.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "TimerManager.h"

AProjectileBolt::AProjectileBolt()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	InitialLifeSpan = 0.f;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	CollisionSphere->InitSphereRadius(12.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionSphere->SetGenerateOverlapEvents(false);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionSphere);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetRelativeScale3D(FVector(0.25f));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3200.f;
	ProjectileMovement->MaxSpeed = 3200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bSweepCollision = true;

	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	TrailComponent->SetupAttachment(CollisionSphere);
	TrailComponent->bAutoActivate = false;
}

void AProjectileBolt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileBolt, bIsActive);
}

void AProjectileBolt::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileBolt::OnSphereHit);
}

void AProjectileBolt::LifeSpanExpired()
{
	DeactivateProjectile();
}

void AProjectileBolt::LaunchProjectile(const FVector& Direction, AActor* InInstigator, float InDamage, bool bInPiercing, float InAoERadius)
{
	SetInstigator(Cast<APawn>(InInstigator));
	SetOwner(InInstigator);
	Damage = InDamage;
	bPiercing = bInPiercing;
	AoERadius = InAoERadius;
	bIsActive = true;
	HitActors.Reset();

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	const FVector LaunchDir = Direction.GetSafeNormal();
	ProjectileMovement->Velocity = LaunchDir * ProjectileMovement->InitialSpeed;
	ProjectileMovement->SetUpdatedComponent(CollisionSphere);
	ProjectileMovement->Activate(true);

	if (TrailComponent)
	{
		TrailComponent->Activate(true);
	}

	SetLifeSpan(LifeSeconds);
}

void AProjectileBolt::ResetProjectile()
{
	DeactivateProjectile();
}

void AProjectileBolt::OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority() || !bIsActive || !OtherActor || OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}

	if (HitActors.Contains(OtherActor))
	{
		return;
	}

	HitActors.Add(OtherActor);
	ApplyDamageAt(Hit);
	Multicast_PlayImpact(Hit.ImpactPoint, Hit.ImpactNormal);

	if (!bPiercing)
	{
		DeactivateProjectile();
	}
}

void AProjectileBolt::ApplyDamageAt(const FHitResult& Hit)
{
	TArray<AActor*> Targets;
	if (AoERadius > 0.f)
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(ProjectileAoE), false, this);
		GetWorld()->OverlapMultiByChannel(Overlaps, Hit.ImpactPoint, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AoERadius), Params);
		for (const FOverlapResult& Result : Overlaps)
		{
			if (AActor* Actor = Result.GetActor())
			{
				Targets.AddUnique(Actor);
			}
		}
	}
	else if (Hit.GetActor())
	{
		Targets.Add(Hit.GetActor());
	}

	for (AActor* Target : Targets)
	{
		if (!Target || Target == GetInstigator() || Target == GetOwner())
		{
			continue;
		}

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			if (DamageEffectSpecHandle.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
			else
			{
				UGameplayStatics::ApplyDamage(Target, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
			}
		}
		else
		{
			UGameplayStatics::ApplyDamage(Target, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
}

void AProjectileBolt::Multicast_PlayImpact_Implementation(FVector Location, FVector Normal)
{
	if (ImpactFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, Location, Normal.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}

	UPointLightComponent* Flash = NewObject<UPointLightComponent>(this);
	if (Flash)
	{
		Flash->RegisterComponent();
		Flash->SetWorldLocation(Location);
		Flash->SetIntensity(ExplosionLightIntensity);
		Flash->SetLightColor(FLinearColor(0.35f, 0.75f, 1.f));
		Flash->SetAttenuationRadius(450.f);
		FTimerHandle LightTimer;
		TWeakObjectPtr<UPointLightComponent> WeakFlash(Flash);
		GetWorldTimerManager().SetTimer(LightTimer, FTimerDelegate::CreateLambda([WeakFlash]()
		{
			if (UPointLightComponent* Comp = WeakFlash.Get())
			{
				Comp->DestroyComponent();
			}
		}), ExplosionLightDuration, false);
	}

	if (ImpactCameraShake)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->ClientStartCameraShake(ImpactCameraShake);
		}
	}
}

void AProjectileBolt::DeactivateProjectile()
{
	bIsActive = false;
	SetLifeSpan(0.f);
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();
	if (TrailComponent)
	{
		TrailComponent->Deactivate();
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActors.Reset();
}
