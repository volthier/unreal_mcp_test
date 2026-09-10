#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RunnerTypes.h"
#include "RunnerClassData.generated.h"

class UGameplayAbility;

/** Linha de classe base (6 no total). Subclasse entra no nivel 3 (GDD v3 par. 5). */
USTRUCT(BlueprintType)
struct PLOIDREKRPG_API FRunnerClassData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classe") FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classe") ERunnerRole Role = ERunnerRole::DPS;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classe") FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") ERunnerAttribute PrimaryAttribute = ERunnerAttribute::Strength;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") ERunnerAttribute SecondaryAttribute = ERunnerAttribute::Dexterity;

    /** Dado de vida: 8, 10 ou 12. HP nivel 1 = 3 x dado + mod CON (GDD v3 par. 3.3). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progressao") int32 HitDie = 8;

    /** Ids de subclasse (futuro DT_Subclasses). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progressao") TArray<FName> Subclasses;
    /** Habilidades iniciais — classes de Gameplay Ability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progressao") TArray<TSoftClassPtr<UGameplayAbility>> StartingAbilities;
};
