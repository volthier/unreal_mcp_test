#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "RunnerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Core combat attributes for a Runner.
 * Replicated for multiplayer / listen-server MMORPG prototype.
 */
UCLASS()
class PLOIDREKRPG_API URunnerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	URunnerAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, Energy)

	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, MaxEnergy)

	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_ChargeLevel)
	FGameplayAttributeData ChargeLevel;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, ChargeLevel)

	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Meta")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(URunnerAttributeSet, Damage)

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldEnergy);

	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy);

	UFUNCTION()
	virtual void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);

	UFUNCTION()
	virtual void OnRep_ChargeLevel(const FGameplayAttributeData& OldChargeLevel);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};
