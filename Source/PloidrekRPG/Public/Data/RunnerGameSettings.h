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

    // --- Fundo do menu ---
    /**
     * Arte de cenario desenhada atras do painel do menu (a praia de Kardys no login, a fileira de
     * casulos na selecao). Vazio deixa o fundo 3D do mapa aparecer sozinho.
     */
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> TexturaDoFundo;

    /** Escurecimento aplicado sobre a arte do fundo, para a UI continuar legivel (0 a 1). */
    UPROPERTY(config, EditAnywhere, Category = "Fundo", meta = (ClampMin = "0.0", ClampMax = "0.95"))
    float EscurecimentoDoFundo = 0.35f;

    /**
     * Host:porta do Epic Dev Auth Tool no DEV (Project Settings > Game > Runner > EOS).
     * E o valor que o campo 1 do login preenche e que o texto de ajuda mostra. A porta e escolhida
     * no proprio tool; trocar aqui evita mexer em codigo.
     */
    UPROPERTY(config, EditAnywhere, Category = "EOS") FString EOSDevAuthHost;

    // --- Vitrine 3D do menu (o corpo que aparece girando enquanto voce escolhe) ---
    /** Giro do corpo na vitrine, em graus por segundo (0 deixa parado de frente). */
    UPROPERTY(config, EditAnywhere, Category = "Vitrine") float GiroDaVitrine = 16.f;

    /** Distancia da camera da vitrine ao corpo, em centimetros (maior = corpo menor no monitor). */
    UPROPERTY(config, EditAnywhere, Category = "Vitrine") float DistanciaDaVitrine = 280.f;

    /** Campo de visao da camera da vitrine (maior = mais corpo no quadro). */
    UPROPERTY(config, EditAnywhere, Category = "Vitrine") float CampoDeVisaoDaVitrine = 45.f;

    // --- Veu da interface (a neblina que circula pela tela e segue o mouse) ---
    /** Liga/desliga o veu. Tambem da para desligar em execucao com o console: Runner.Veil 0 */
    UPROPERTY(config, EditAnywhere, Category = "Veu") bool bVeuLigado = true;

    /** 0 = praticamente invisivel, 1 = como desenhado, 1.5 = carregado. */
    UPROPERTY(config, EditAnywhere, Category = "Veu") float IntensidadeDoVeu = 1.f;

    /** Raio, em pixels, da bola de dispersao que acompanha o mouse. */
    UPROPERTY(config, EditAnywhere, Category = "Veu") float RaioDaBolaDoMouse = 300.f;

    /** Velocidade da circulacao dos fios. */
    UPROPERTY(config, EditAnywhere, Category = "Veu") float VelocidadeDoVeu = 1.f;

    // Animacoes do corpo (pacote do manequim da engine) enquanto nao existe AnimBP proprio.
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> IdleAnim;
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> WalkAnim;
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> RunAnim;
    UPROPERTY(config, EditAnywhere, Category = "Animacao") TSoftObjectPtr<UAnimSequenceBase> JumpAnim;

    virtual FName GetCategoryName() const override { return FName(TEXT("Game")); }
};
