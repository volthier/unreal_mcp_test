#pragma once

#include "CoreMinimal.h"
#include "RunnerTypes.generated.h"

/** Os seis atributos do 5e — usados por chassi, classe, equipamento e efeitos. */
UENUM(BlueprintType)
enum class ERunnerAttribute : uint8
{
    Strength     UMETA(DisplayName = "Forca"),
    Dexterity    UMETA(DisplayName = "Destreza"),
    Constitution UMETA(DisplayName = "Constituicao"),
    Intelligence UMETA(DisplayName = "Inteligencia"),
    Wisdom       UMETA(DisplayName = "Sabedoria"),
    Charisma     UMETA(DisplayName = "Carisma")
};

/**
 * O corpo do chassi define o defeito (GDD v3 par. 4).
 * Esguio -> FOR -1 · Truncoso -> DEX -1 · Fragil -> CON -1 · Instavel -> SAB -1
 */
UENUM(BlueprintType)
enum class ERunnerBodyType : uint8
{
    Slender  UMETA(DisplayName = "Esguio"),
    Stocky   UMETA(DisplayName = "Truncoso"),
    Fragile  UMETA(DisplayName = "Fragil"),
    Unstable UMETA(DisplayName = "Instavel")
};

UENUM(BlueprintType)
enum class ERunnerRole : uint8
{
    DPS      UMETA(DisplayName = "DPS"),
    Tank     UMETA(DisplayName = "Tanque"),
    Support  UMETA(DisplayName = "Suporte"),
    Control  UMETA(DisplayName = "Controle"),
    Hybrid   UMETA(DisplayName = "Hibrido")
};
