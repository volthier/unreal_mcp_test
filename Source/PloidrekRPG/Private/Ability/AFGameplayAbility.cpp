#include "Ability/AFGameplayAbility.h"
#include "Character/RunnerCharacter.h"
#include "Projectile/ProjectileBolt.h"
#include "Weapon/WeaponBase.h"
#include "AbilitySystemComponent.h"

UAFGameplayAbility::UAFGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

ARunnerCharacter* UAFGameplayAbility::GetRunnerCharacter() const
{
	return Cast<ARunnerCharacter>(GetAvatarActorFromActorInfo());
}

AProjectileBolt* UAFGameplayAbility::SpawnProjectile(TSubclassOf<AProjectileBolt> ProjectileClass, float Damage, bool bPiercing, float AoERadius, float Scale)
{
	ARunnerCharacter* Character = GetRunnerCharacter();
	if (!Character || !ProjectileClass || !Character->HasAuthority())
	{
		return nullptr;
	}

	const FVector Location = ICombatInterface::Execute_GetProjectileSpawnLocation(Character);
	const FVector Direction = ICombatInterface::Execute_GetProjectileSpawnDirection(Character);
	const FTransform SpawnTM(Direction.Rotation(), Location);

	FActorSpawnParameters Params;
	Params.Owner = Character;
	Params.Instigator = Character;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AProjectileBolt* Projectile = Character->GetWorld()->SpawnActor<AProjectileBolt>(ProjectileClass, SpawnTM, Params);
	if (Projectile)
	{
		Projectile->SetActorScale3D(FVector(Scale));
		Projectile->LaunchProjectile(Direction, Character, Damage, bPiercing, AoERadius);
		if (AWeaponBase* Weapon = Character->GetEquippedWeapon())
		{
			Weapon->PlayMuzzleFlash();
		}
	}
	return Projectile;
}
