#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RunnerTypes.h"
#include "RunnerChassisData.generated.h"

class USkeletalMesh;

/**
 * Linha de chassi (Runner model) — a unica variavel da criacao de personagem.
 * Base: 8 em tudo. O chassi aplica +3 positivos e -1 pelo corpo (GDD v3 par. 3.1 e 4).
 */
USTRUCT(BlueprintType)
struct PLOIDREKRPG_API FRunnerChassisData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chassi") FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chassi") ERunnerBodyType Body = ERunnerBodyType::Slender;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chassi") FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") int32 Strength = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") int32 Dexterity = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") int32 Constitution = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") int32 Intelligence = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") int32 Wisdom = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atributos") int32 Charisma = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vantagens") FText AdvantagePrimary;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vantagens") FText AdvantageSecondary;

    /** Corpo usado no jogo. Enquanto nao existe arte propria, aponta para o manequim da engine. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Corpo") TSoftObjectPtr<USkeletalMesh> Mesh;
    /** Tinta de placeholder para diferenciar chassi sem arte. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Corpo") FLinearColor AccentColor = FLinearColor::White;

    /** Slot dos Clyffen: aparece na criacao marcado EXTINTO e nunca pode ser selecionado. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot") bool bExtinct = false;

    /** Bonus do chassi em um atributo (ja inclui o -1 do corpo). */
    int32 GetBonus(ERunnerAttribute Attribute) const
    {
        switch (Attribute)
        {
            case ERunnerAttribute::Strength:     return Strength;
            case ERunnerAttribute::Dexterity:    return Dexterity;
            case ERunnerAttribute::Constitution: return Constitution;
            case ERunnerAttribute::Intelligence: return Intelligence;
            case ERunnerAttribute::Wisdom:       return Wisdom;
            case ERunnerAttribute::Charisma:     return Charisma;
            default:                             return 0;
        }
    }
};
