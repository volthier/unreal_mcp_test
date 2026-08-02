#include "Projectile/VoltStrikerChargeProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AVoltStrikerChargeProjectile::AVoltStrikerChargeProjectile()
{
	Damage = 70.f;
	bPiercing = true;
	AoERadius = 140.f;
	LifeSeconds = 5.f;
	ExplosionLightIntensity = 20000.f;
	ExplosionLightDuration = 0.25f;
	if (CollisionSphere)
	{
		CollisionSphere->InitSphereRadius(28.f);
	}
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = 2400.f;
		ProjectileMovement->MaxSpeed = 2400.f;
	}
}
