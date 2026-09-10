#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RunnerGameSettings.generated.h"

class UAnimSequenceBase;
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

    // Animacoes do corpo (pacote do manequim da engine) enquanto nao existe AnimBP proprio.
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> IdleAnim;
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> WalkAnim;
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> RunAnim;
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> JumpAnim;

    virtual FName GetCategoryName() const override { return FName(TEXT("Game")); }
};
