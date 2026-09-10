#include "Attributes/RunnerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"

URunnerAttributeSet::URunnerAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitEnergy(100.f);
	InitMaxEnergy(100.f);
	InitAttackPower(15.f);
	InitChargeLevel(0.f);
	InitMoveSpeed(600.f);
	InitDamage(0.f);
}

void URunnerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, ChargeLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URunnerAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void URunnerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetMaxEnergyAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetChargeLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 3.f);
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
	}
}

void URunnerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		SetDamage(0.f);
		if (DamageDone > 0.f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - DamageDone, 0.f, GetMaxHealth());
			SetHealth(NewHealth);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		SetEnergy(FMath::Clamp(GetEnergy(), 0.f, GetMaxEnergy()));
	}
	else if (Data.EvaluatedData.Attribute == GetChargeLevelAttribute())
	{
		SetChargeLevel(FMath::Clamp(GetChargeLevel(), 0.f, 3.f));
	}
}

void URunnerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, Health, OldHealth);
}

void URunnerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, MaxHealth, OldMaxHealth);
}

void URunnerAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, Energy, OldEnergy);
}

void URunnerAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, MaxEnergy, OldMaxEnergy);
}

void URunnerAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, AttackPower, OldAttackPower);
}

void URunnerAttributeSet::OnRep_ChargeLevel(const FGameplayAttributeData& OldChargeLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, ChargeLevel, OldChargeLevel);
}

void URunnerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URunnerAttributeSet, MoveSpeed, OldMoveSpeed);
}
