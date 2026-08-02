#include "Ability/GA_BasicShot.h"
#include "Projectile/VoltStrikerProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UGA_BasicShot::UGA_BasicShot()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShot"), false));
}

void UGA_BasicShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	SpawnProjectile(ProjectileClass, Damage, false, 0.f, 1.f);

	if (FireCameraShake)
	{
		if (APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get()))
		{
			PC->ClientStartCameraShake(FireCameraShake);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
