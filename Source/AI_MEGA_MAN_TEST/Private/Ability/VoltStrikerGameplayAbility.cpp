#include "Ability/VoltStrikerGameplayAbility.h"
#include "Character/VoltStrikerCharacter.h"
#include "Projectile/VoltStrikerProjectile.h"
#include "Weapon/VoltStrikerWeapon.h"
#include "AbilitySystemComponent.h"

UVoltStrikerGameplayAbility::UVoltStrikerGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

AVoltStrikerCharacter* UVoltStrikerGameplayAbility::GetVoltStrikerCharacter() const
{
	return Cast<AVoltStrikerCharacter>(GetAvatarActorFromActorInfo());
}

AVoltStrikerProjectile* UVoltStrikerGameplayAbility::SpawnProjectile(TSubclassOf<AVoltStrikerProjectile> ProjectileClass, float Damage, bool bPiercing, float AoERadius, float Scale)
{
	AVoltStrikerCharacter* Character = GetVoltStrikerCharacter();
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

	AVoltStrikerProjectile* Projectile = Character->GetWorld()->SpawnActor<AVoltStrikerProjectile>(ProjectileClass, SpawnTM, Params);
	if (Projectile)
	{
		Projectile->SetActorScale3D(FVector(Scale));
		Projectile->LaunchProjectile(Direction, Character, Damage, bPiercing, AoERadius);
		if (AVoltStrikerWeapon* Weapon = Character->GetEquippedWeapon())
		{
			Weapon->PlayMuzzleFlash();
		}
	}
	return Projectile;
}
