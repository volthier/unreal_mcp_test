#include "Attributes/VoltStrikerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"

UVoltStrikerAttributeSet::UVoltStrikerAttributeSet()
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

void UVoltStrikerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, ChargeLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVoltStrikerAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UVoltStrikerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
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

void UVoltStrikerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

void UVoltStrikerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, Health, OldHealth);
}

void UVoltStrikerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, MaxHealth, OldMaxHealth);
}

void UVoltStrikerAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, Energy, OldEnergy);
}

void UVoltStrikerAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, MaxEnergy, OldMaxEnergy);
}

void UVoltStrikerAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, AttackPower, OldAttackPower);
}

void UVoltStrikerAttributeSet::OnRep_ChargeLevel(const FGameplayAttributeData& OldChargeLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, ChargeLevel, OldChargeLevel);
}

void UVoltStrikerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVoltStrikerAttributeSet, MoveSpeed, OldMoveSpeed);
}
