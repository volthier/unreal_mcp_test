#include "Ability/GA_ChargeShot.h"
#include "Character/RunnerCharacter.h"
#include "Projectile/ProjectileBolt.h"

UGA_ChargeShot::UGA_ChargeShot()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.ChargeShot"), false));
}

void UGA_ChargeShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	float Hold = Level1HoldSeconds;
	if (const ARunnerCharacter* Character = GetRunnerCharacter())
	{
		Hold = Character->GetLastReleasedChargeHold();
	}
	else if (TriggerEventData)
	{
		Hold = TriggerEventData->EventMagnitude;
	}

	float OutDamage = DamageL1;
	float AoE = AoEL1;
	float Scale = 1.4f;

	if (Hold >= Level3HoldSeconds)
	{
		OutDamage = DamageL3;
		AoE = AoEL3;
		Scale = 2.6f;
	}
	else if (Hold >= Level2HoldSeconds)
	{
		OutDamage = DamageL2;
		AoE = AoEL2;
		Scale = 2.0f;
	}

	SpawnProjectile(ChargeProjectileClass, OutDamage, true, AoE, Scale);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
