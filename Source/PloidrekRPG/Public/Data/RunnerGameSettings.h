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

    /**
     * Mapa do MUNDO ABERTO que o botao Jogar abre, com o personagem escolhido. E aqui que a area inicial
     * vive (praça industrial + cidade procedimental em PCG).
     */
    UPROPERTY(config, EditAnywhere, Category = "Dados") FString MapaDoMundo = TEXT("/Game/Maps/MundoAberto");

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

    /** Moldura de latao do painel do menu (kit visual). Vazio = painel liso. */
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> TexturaDaMoldura;

    /** Emblema do titulo: o cristal em anel de latao. Vazio = so o texto. */
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> EmblemaDoTitulo;

    /**
     * Logotipo da tela de entrada (AETHER FORGE com o X em degrade), do alvo do autor em Art/Tela_login.
     * Quando existe, ele SUBSTITUI o titulo em texto - e o que da o acabamento do guia. Sem ele, a tela cai no
     * texto, entao o menu nunca fica sem titulo.
     */
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> LogoDoTitulo;

    /** Icones dos campos de entrada (usuario e senha), do mesmo kit. */
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> IconeDoUsuario;
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> IconeDaSenha;
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> IconeDoOlho;

    // ================================================================================================
    // STYLE SYSTEM DA TELA DE LOGIN (spec secao 28).
    //
    // A secao pede que as cores e medidas NAO fiquem espalhadas nos widgets: aqui elas ficam num lugar so,
    // editavel em Project Settings > Game > Runner, sem tocar em codigo. Mudar o tema e mudar estes valores.
    // ================================================================================================
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginCorPrimaria = FLinearColor(0.f, 0.898f, 1.f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginCorSecundaria = FLinearColor(0.f, 0.549f, 1.f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginCorDeAcento = FLinearColor(1.f, 0.180f, 0.945f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginCorDeFundo = FLinearColor(0.020f, 0.031f, 0.078f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginCorDeBorda = FLinearColor(0.f, 0.898f, 1.f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginTextoPrincipal = FLinearColor(0.918f, 0.949f, 1.f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginTextoSecundario = FLinearColor(0.478f, 0.737f, 0.675f);
    UPROPERTY(config, EditAnywhere, Category = "Login|Cores") FLinearColor LoginCorDeErro = FLinearColor(1.f, 0.400f, 0.533f);

    /** Brilho do emissivo (secao 7): repouso 3 a 7, hover 8 a 12, pressed 4 a 6. */
    UPROPERTY(config, EditAnywhere, Category = "Login|Medidas") float LoginGlowIntensity = 4.f;
    UPROPERTY(config, EditAnywhere, Category = "Login|Medidas") float LoginGlowHover = 9.f;
    UPROPERTY(config, EditAnywhere, Category = "Login|Medidas") float LoginGlowPressed = 5.f;
    /** Tamanho do corte de canto, em unidades de referencia (secao 9: cantos de 8 a 10). */
    UPROPERTY(config, EditAnywhere, Category = "Login|Medidas") float LoginCornerSize = 9.f;
    /** Padding interno do painel (secao 9: de 20 a 32). */
    UPROPERTY(config, EditAnywhere, Category = "Login|Medidas") float LoginPadding = 24.f;
    /** Espaco entre campos (secao 9: de 14 a 20). */
    UPROPERTY(config, EditAnywhere, Category = "Login|Medidas") float LoginEspacoEntreCampos = 14.f;
    /** Velocidade das animacoes de idle: multiplica os periodos do pulso (secao 21). */
    UPROPERTY(config, EditAnywhere, Category = "Login|Animacao") float LoginAnimationSpeed = 1.f;
    /** Transicao entre Login e Cadastro, em segundos (secao 10: de 0,15 a 0,25). */
    UPROPERTY(config, EditAnywhere, Category = "Login|Animacao") float LoginTransitionTime = 0.2f;
    UPROPERTY(config, EditAnywhere, Category = "Fundo") TSoftObjectPtr<UTexture2D> IconeDoOlhoFechado;

    /** Escurecimento aplicado sobre a arte do fundo, para a UI continuar legivel (0 a 1). */
    UPROPERTY(config, EditAnywhere, Category = "Fundo", meta = (ClampMin = "0.0", ClampMax = "0.95"))
    float EscurecimentoDoFundo = 0.35f;

    /**
     * Host:porta do Epic Dev Auth Tool no DEV (Project Settings > Game > Runner > EOS).
     * E o valor que o campo 1 do login preenche e que o texto de ajuda mostra. A porta e escolhida
     * no proprio tool; trocar aqui evita mexer em codigo.
     */
    UPROPERTY(config, EditAnywhere, Category = "EOS") FString EOSDevAuthHost;

    // --- Nucleo (o cristal do chassi) ---
    /**
     * Malha do cristal - o chassi E o cristal, nao o corpo. Aparece girando na vitrine enquanto o jogador
     * escolhe o CHASSI; o corpo (da CLASSE) entra depois, na aba da classe.
     */
    UPROPERTY(config, EditAnywhere, Category = "Nucleo") TSoftObjectPtr<UStaticMesh> NucleoMesh;

    /**
     * Caminho do material da cor do nucleo, com dois %s (o id do chassi). Ex.:
     * /Game/AI_Assets/materials/MI_Nucleo_%s.MI_Nucleo_%s
     * Assim a cor de cada chassi e dado, nao codigo - ver Docs/Prompts_Arte_Ploidrek.md secao 9b.
     */
    UPROPERTY(config, EditAnywhere, Category = "Nucleo") FString PastaDosMateriaisDoNucleo = TEXT("/Game/AI_Assets/materials");

    /**
     * Material da casca da aura: a nuvem de gelo que gira em volta do cristal (aditivo, com textura de
     * geada). Ela e tingida com a cor do nucleo do chassi, entao a mesma casca serve para os dez.
     */
    UPROPERTY(config, EditAnywhere, Category = "Nucleo") TSoftObjectPtr<class UMaterialInterface> MaterialDaAura;

    /** Escala da casca da aura em relacao ao cristal (1.0 = colada nele). */
    UPROPERTY(config, EditAnywhere, Category = "Nucleo") float EscalaDaAura = 2.6f;

    /** Efeito Niagara por cima da casca (opcional; a casca sozinha ja entrega a nevoa). */
    UPROPERTY(config, EditAnywhere, Category = "Nucleo") TSoftObjectPtr<class UNiagaraSystem> AuraDoNucleo;

    /** Escala do cristal na vitrine (a malha vem em tamanho de mundo). */
    UPROPERTY(config, EditAnywhere, Category = "Nucleo") float EscalaDoNucleo = 0.35f;

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
