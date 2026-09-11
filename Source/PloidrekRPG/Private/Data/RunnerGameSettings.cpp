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
    EscalaDoNucleo = 0.35f;
    // A aura: casca aditiva com a textura de geada, tingida com a cor do nucleo de cada chassi.
    MaterialDaAura = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/AI_Assets/materials/M_AuraGeada.M_AuraGeada")));
    EscalaDaAura = 2.6f;
    // O Niagara entra por cima quando o sistema estiver salvo em disco (hoje so existe na memoria do editor).

    // Enquadramento da vitrine: ajustavel sem recompilar depois de ver o corpo no monitor.
    GiroDaVitrine = 16.f;
    DistanciaDaVitrine = 280.f;
    CampoDeVisaoDaVitrine = 45.f;
}
