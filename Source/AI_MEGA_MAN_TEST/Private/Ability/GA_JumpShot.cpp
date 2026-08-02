#include "Ability/GA_JumpShot.h"
#include "Character/VoltStrikerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_JumpShot::UGA_JumpShot()
{
	Damage = 18.f;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.JumpShot"), false));
}

void UGA_JumpShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AVoltStrikerCharacter* Character = GetVoltStrikerCharacter();
	if (!Character || Character->GetCharacterMovement()->IsMovingOnGround())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	SpawnProjectile(ProjectileClass, Damage * AerialDamageMultiplier, false, 30.f, 1.15f);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
