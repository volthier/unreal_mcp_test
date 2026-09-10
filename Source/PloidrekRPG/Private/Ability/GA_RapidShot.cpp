#include "Ability/GA_RapidShot.h"
#include "TimerManager.h"

UGA_RapidShot::UGA_RapidShot()
{
	Damage = 8.f;
	CooldownSeconds = 0.45f;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.RapidShot"), false));
}

void UGA_RapidShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ShotsRemaining = BurstCount;
	FireBurstShot();
}

void UGA_RapidShot::FireBurstShot()
{
	SpawnProjectile(ProjectileClass, Damage, false, 0.f, 0.85f);
	--ShotsRemaining;

	if (ShotsRemaining <= 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BurstTimer, this, &UGA_RapidShot::FireBurstShot, BurstInterval, false);
	}
}
