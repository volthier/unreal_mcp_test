#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RunnerGameSettings.generated.h"

class UDataTable;

/** Configuracao do jogo em Project Settings > Game > Runner (nada de caminho magico no codigo). */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Runner"))
class PLOIDREKRPG_API URunnerGameSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    URunnerGameSettings();

    /** Tabela dos chassis jogaveis (linhas FRunnerChassisData). */
    UPROPERTY(config, EditAnywhere, Category = "Dados") TSoftObjectPtr<UDataTable> ChassisTable;

    /** Tabela das classes base (linhas FRunnerClassData). */
    UPROPERTY(config, EditAnywhere, Category = "Dados") TSoftObjectPtr<UDataTable> ClassTable;

    /** Material usado para tingir o corpo com a cor do chassi (parametro AccentColor). */
    UPROPERTY(config, EditAnywhere, Category = "Corpo") TSoftObjectPtr<UMaterialInterface> AccentOverlayMaterial;

    virtual FName GetCategoryName() const override { return FName(TEXT("Game")); }
};
