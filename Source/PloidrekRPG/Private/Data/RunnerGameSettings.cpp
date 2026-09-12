#include "Data/RunnerGameSettings.h"

URunnerGameSettings::URunnerGameSettings()
{
    ChassisTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/Data/DT_Chassis.DT_Chassis")));
    ClassTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/Data/DT_Classes.DT_Classes")));
    AccentOverlayMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Runners/M_RunnerAccent.M_RunnerAccent")));

    // Animacoes que ja vem no pacote do manequim, usadas enquanto nao existe AnimBP proprio.
    IdleAnim = TSoftObjectPtr<UAnimSequenceBase>(FSoftObjectPath(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle.MM_Idle")));
    WalkAnim = TSoftObjectPtr<UAnimSequenceBase>(FSoftObjectPath(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd.MF_Unarmed_Walk_Fwd")));
    RunAnim  = TSoftObjectPtr<UAnimSequenceBase>(FSoftObjectPath(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jog/MF_Unarmed_Jog_Fwd.MF_Unarmed_Jog_Fwd")));
    JumpAnim = TSoftObjectPtr<UAnimSequenceBase>(FSoftObjectPath(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jump/MM_Jump.MM_Jump")));

    // Porta escolhida no Epic Dev Auth Tool do DEV.
    EOSDevAuthHost = TEXT("localhost:8081");

    // O NUCLEO: o chassi E o cristal. A malha e a mesma para todos (o chassi e igual para todos); o que
    // muda e a COR, e ela vem de uma instancia de material por chassi (MI_Nucleo_<Chassi>), gerada a
    // partir do CoreColor da DataTable - ver Docs/Prompts_Arte_Ploidrek.md secao 9b.
    NucleoMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/AI_Assets/prop/cristal_nucleo.cristal_nucleo")));
    PastaDosMateriaisDoNucleo = TEXT("/Game/AI_Assets/materials");

    // Kit visual AAA do menu: a moldura de latao do painel e o emblema do titulo. Sao TEXTURAS de
    // configuracao, nao codigo - trocar a arte nao exige recompilar.
    TexturaDaMoldura = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_moldura_latao.ui_moldura_latao")));
    EmblemaDoTitulo = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_emblema.ui_emblema")));
    LogoDoTitulo = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_logo_aether.ui_logo_aether")));
    IconeDoUsuario = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_icone_pessoa.ui_icone_pessoa")));
    IconeDaSenha = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_icone_cadeado.ui_icone_cadeado")));
    IconeDoOlho = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_icone_olho.ui_icone_olho")));
    IconeDoOlhoFechado = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/AI_Assets/ui/ui_icone_olho_fechado.ui_icone_olho_fechado")));
    EscalaDoNucleo = 1.0f;   // o cristal limpo tem 1,61 m: em 1.0 ele preenche a caixa da vitrine
    // A aura: casca aditiva com a textura de geada, tingida com a cor do nucleo de cada chassi.
    MaterialDaAura = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/AI_Assets/materials/M_AuraGeada.M_AuraGeada")));
    EscalaDaAura = 2.6f;
    // O Niagara entra por cima quando o sistema estiver salvo em disco (hoje so existe na memoria do editor).

    // Enquadramento da vitrine: ajustavel sem recompilar depois de ver o corpo no monitor.
    GiroDaVitrine = 16.f;
    DistanciaDaVitrine = 280.f;
    CampoDeVisaoDaVitrine = 45.f;
}
