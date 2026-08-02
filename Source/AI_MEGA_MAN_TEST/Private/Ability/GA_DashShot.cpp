#include "Ability/GA_DashShot.h"
#include "Character/VoltStrikerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UGA_DashShot::UGA_DashShot()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.DashShot"), false));
}

void UGA_DashShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AVoltStrikerCharacter* Character = GetVoltStrikerCharacter();
	if (Character)
	{
		const FVector DashDir = ICombatInterface::Execute_GetProjectileSpawnDirection(Character).GetSafeNormal2D();
		Character->LaunchCharacter(DashDir * DashStrength + FVector(0.f, 0.f, 120.f), true, true);
		SpawnProjectile(ProjectileClass, Damage, false, 40.f, 1.1f);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
