#pragma once

#include "CoreMinimal.h"
#include "RunnerTypes.h"
#include "RunnerCharacterProfile.generated.h"

class USkeletalMesh;

/**
 * Ficha resultante da criacao: o que o jogo precisa para montar o personagem.
 * Nada aqui e escolhido pelo jogador alem de chassi e classe — o resto e derivado pelas regras.
 */
USTRUCT(BlueprintType)
struct PLOIDREKRPG_API FRunnerCharacterProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Perfil") FString AccountName;
    UPROPERTY(BlueprintReadOnly, Category = "Perfil") FName ChassisId;
    UPROPERTY(BlueprintReadOnly, Category = "Perfil") FName ClassId;

    /** Valores finais: base 8 + bonus do chassi (ja com o -1 do corpo). */
    UPROPERTY(BlueprintReadOnly, Category = "Atributos") TMap<ERunnerAttribute, int32> Attributes;

    UPROPERTY(BlueprintReadOnly, Category = "Ficha") int32 MaxHitPoints = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Ficha") int32 HitPointsPerLevel = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Ficha") int32 ArmorClass = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Ficha") int32 EtherCells = 100;
    UPROPERTY(BlueprintReadOnly, Category = "Ficha") int32 AttacksPerAttackAction = 1;

    /** Corpo: enquanto nao existe arte propria, o manequim da engine. */
    UPROPERTY(BlueprintReadOnly, Category = "Corpo") TSoftObjectPtr<USkeletalMesh> BodyMesh;
    UPROPERTY(BlueprintReadOnly, Category = "Corpo") FLinearColor AccentColor = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, Category = "Perfil") bool bValid = false;

    int32 GetAttribute(ERunnerAttribute Attribute) const
    {
        const int32* Found = Attributes.Find(Attribute);
        return Found ? *Found : 0;
    }
};
