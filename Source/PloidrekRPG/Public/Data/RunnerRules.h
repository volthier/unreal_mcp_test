#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RunnerTypes.h"
#include "RunnerChassisData.h"
#include "RunnerRules.generated.h"

/**
 * Regras puras do jogo (GDD v3). Sem estado e sem mundo: por isso sao testaveis por script.
 * Toda a matematica de atributo, HP, CA e proficiencia vive aqui — nada de numero magico espalhado.
 */
UCLASS()
class PLOIDREKRPG_API URunnerRules : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Modificador de atributo do 5e: floor((valor - 10) / 2). Score 7 = -2, score 8 = -1. */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetAttributeModifier(int32 Score);

    /** Valor base de todo atributo na criacao: 8 (nao existe compra de pontos — GDD 3.1). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetBaseAttributeScore();

    /** Valor final = base 8 + bonus do chassi (que ja inclui o -1 do corpo). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetFinalAttributeScore(const FRunnerChassisData& Chassis, ERunnerAttribute Attribute);

    /** Cap de atributo por capitulo: I=20, II=24, III=28 (GDD 3.2). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetAttributeCapForChapter(int32 Chapter);

    /** Bonus de proficiencia por nivel: +2 (1-4) ... +6 (17-20); sem crescimento no epico. */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetProficiencyBonus(int32 Level);

    /** HP no nivel 1 = 3 x dado de vida + mod CON (GDD 3.3). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetLevelOneHitPoints(int32 HitDie, int32 ConstitutionScore);

    /** HP por nivel seguinte = dado cheio + mod CON, sem rolagem (GDD 3.3). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetHitPointsPerLevel(int32 HitDie, int32 ConstitutionScore);

    /** CA = 10 + mod DES + bonus de armadura/modulo. */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetArmorClass(int32 DexterityScore, int32 ArmorBonus);

    /** Celulas de Eter: 100 fixo no nivel 1 (GDD 3.3). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetMaxEtherCells();

    /** Numero de ataques por Acao de Ataque por nivel: 1 / 2 / 3 / 4, teto 4 (GDD 3.4). */
    UFUNCTION(BlueprintPure, Category = "Runner|Regras")
    static int32 GetAttacksPerAttackAction(int32 Level);
};
