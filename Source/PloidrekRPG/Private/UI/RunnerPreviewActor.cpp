#include "UI/RunnerPreviewActor.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/StaticMesh.h"
#include "Data/RunnerGameSettings.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UI/RunnerPalette.h"

namespace
{
    /** Bem longe e ABAIXO do mapa: o jogador nunca ve o boneco no cenario do menu. */
    const FVector LocalDaVitrine(0.f, 0.f, -50000.f);

    /** Altura da camera em relacao ao corpo (o manequim vai de -90 a +90). */
    constexpr float AlturaDaCamera = 20.f;
}

ARunnerPreviewActor::ARunnerPreviewActor()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
    SetRootComponent(Raiz);

    // Mobilidade MOVEL: sem isso o ator nasce estatico, o SetActorLocation do BeginPlay nao
    // funciona e o boneco fica parado no ORIGEM do mundo — em cima da camera do menu (foi o
    // "bonecao voando" do primeiro teste).
    Raiz->SetMobility(EComponentMobility::Movable);

    Corpo = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Corpo"));
    Corpo->SetupAttachment(Raiz);
    Corpo->SetMobility(EComponentMobility::Movable);
    Corpo->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // mesmo ajuste do pawn do jogo
    Corpo->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    Corpo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Corpo->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    Corpo->bCastDynamicShadow = true;

    // O corpo responde a luz do mundo (canal 0) E a luz da vitrine (canal 1).
    Corpo->SetLightingChannels(true, true, false);

    // O CRISTAL: o chassi e ele. Nasce escondido - quem decide mostrar e a aba (CHASSI ou CLASSE).
    Nucleo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Nucleo"));
    Nucleo->SetupAttachment(Raiz);
    Nucleo->SetMobility(EComponentMobility::Movable);
    Nucleo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Nucleo->SetLightingChannels(true, true, false);
    Nucleo->SetVisibility(false);

    // A CASCA DA AURA: a nuvem de gelo em volta do cristal. Esfera aditiva com textura de geada - e ela que
    // da o volume da nevoa sem depender de particula, e ela que recebe a COR do chassi.
    // Os tres planos da aura nascem aqui, em pe (pitch 90) e com giro de 60 graus entre eles: assim a
    // nevoa existe de qualquer angulo da camera. Sem o giro, dois planos ficariam de perfil e sumiriam.
    PlanosDaAura.Reserve(3);
    for (int32 Indice = 0; Indice < 3; ++Indice)
    {
        UStaticMeshComponent* Plano = CreateDefaultSubobject<UStaticMeshComponent>(
            *FString::Printf(TEXT("PlanoDaAura%d"), Indice));
        Plano->SetupAttachment(Raiz);
        Plano->SetMobility(EComponentMobility::Movable);
        Plano->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Plano->SetLightingChannels(false, true, false);
        Plano->SetCastShadow(false);
        Plano->SetVisibility(false);
        Plano->SetRelativeRotation(FRotator(-90.f, Indice * 60.f, 0.f));
        PlanosDaAura.Add(Plano);
    }

    // A AURA: nuvem de gelo em Niagara, por cima da casca (ligada por configuracao, quando existir em disco).
    Aura = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Aura"));
    Aura->SetupAttachment(Raiz);
    Aura->SetMobility(EComponentMobility::Movable);
    Aura->SetAutoActivate(false);
    Aura->SetVisibility(false);

    // Luz do nucleo: banha o cristal com a cor dele e ajuda a aura a ler no escuro.
    LuzDoNucleo = CreateDefaultSubobject<UPointLightComponent>(TEXT("LuzDoNucleo"));
    LuzDoNucleo->SetupAttachment(Raiz);
    LuzDoNucleo->SetMobility(EComponentMobility::Movable);
    LuzDoNucleo->SetIntensity(0.f);
    LuzDoNucleo->SetAttenuationRadius(420.f);
    LuzDoNucleo->SetCastShadows(false);
    LuzDoNucleo->SetLightingChannels(false, true, false);

    Captura = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Captura"));
    Captura->SetupAttachment(Raiz);
    Captura->SetRelativeLocation(FVector(280.f, 0.f, AlturaDaCamera));
    Captura->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
    Captura->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    Captura->bCaptureEveryFrame = true;
    Captura->bCaptureOnMovement = false;
    Captura->FOVAngle = 45.f;
    // So o nosso ator entra na imagem: o mapa do menu fica de fora.
    Captura->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Captura->ShowFlags.SetAtmosphere(false);
    Captura->ShowFlags.SetFog(false);
    Captura->ShowFlags.SetCloud(false);
    Captura->ShowFlags.SetMotionBlur(false);
    Captura->ShowFlags.SetAntiAliasing(true);

    // As luzes da vitrine vivem no CANAL 1: iluminam o corpo dela e nao mexem no mapa do menu
    // (que usa o canal 0). E o que permite ter luz propria sem estragar o cenario.
    LuzPrincipal = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("LuzPrincipal"));
    LuzPrincipal->SetupAttachment(Raiz);
    LuzPrincipal->SetMobility(EComponentMobility::Movable);
    LuzPrincipal->SetRelativeRotation(FRotator(-24.f, 150.f, 0.f));
    LuzPrincipal->SetIntensity(3.2f);
    LuzPrincipal->SetLightColor(RunnerPalette::LuzDeRua(1.f));
    LuzPrincipal->SetCastShadows(false);
    LuzPrincipal->SetLightingChannels(false, true, false);

    LuzDePreenchimento = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("LuzDePreenchimento"));
    LuzDePreenchimento->SetupAttachment(Raiz);
    LuzDePreenchimento->SetMobility(EComponentMobility::Movable);
    LuzDePreenchimento->SetRelativeRotation(FRotator(-8.f, -40.f, 0.f));
    LuzDePreenchimento->SetIntensity(1.4f);
    LuzDePreenchimento->SetLightColor(RunnerPalette::Ciano(1.f));
    LuzDePreenchimento->SetCastShadows(false);
    LuzDePreenchimento->SetLightingChannels(false, true, false);
}

void ARunnerPreviewActor::BeginPlay()
{
    Super::BeginPlay();

    // A vitrine vive ABAIXO do mapa: de baixo do chao a camera do menu nao tem como ver o corpo.
    SetActorLocation(LocalDaVitrine);

    if (!RenderTarget)
    {
        // Retrato 4:5, o mesmo do monitor: captura quadrada em caixa retangular esticaria o corpo.
        RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, LadoDaCaptura,
                                                                   FMath::RoundToInt(LadoDaCaptura * 1.25f),
                                                                   RTF_RGBA8, FLinearColor::Black);
    }

    if (Captura)
    {
        Captura->TextureTarget = RenderTarget;
        Captura->SetRelativeLocation(FVector(DistanciaDaCamera, 0.f, AlturaDaCamera));
        Captura->FOVAngle = CampoDeVisao;
        Captura->ShowOnlyActors.Reset();
        Captura->ShowOnlyActors.Add(this);
    }
}

void ARunnerPreviewActor::SetNucleo(const TSoftObjectPtr<UStaticMesh>& InMesh, UMaterialInterface* Material, UNiagaraSystem* AuraSystem, const FLinearColor& CorDoNucleo)
{
    UStaticMesh* Malha = InMesh.LoadSynchronous();
    if (!Nucleo || !Malha)
    {
        return;
    }

    Nucleo->SetStaticMesh(Malha);
    if (Material)
    {
        Nucleo->SetMaterial(0, Material);
    }

    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    const float Escala = Ajustes ? Ajustes->EscalaDoNucleo : 0.35f;
    Nucleo->SetRelativeScale3D(FVector(Escala));
    Nucleo->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    Nucleo->SetVisibility(true);

    // O corpo sai de cena: chassi e cristal, corpo e classe.
    if (Corpo)
    {
        Corpo->SetVisibility(false);
        Corpo->SetComponentTickEnabled(false);
    }

    // A casca da aura: esfera aditiva de geada tingida com a COR deste chassi (o mesmo gelo fica azul no
    // Cryonix e branco incandescente no Overcore).
    if (Ajustes && PlanosDaAura.Num() > 0)
    {
        UStaticMesh* PlanoBase = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
        if (!AuraDinamica)
        {
            if (UMaterialInterface* Base = Ajustes->MaterialDaAura.LoadSynchronous())
            {
                AuraDinamica = UMaterialInstanceDynamic::Create(Base, this);
            }
        }
        if (AuraDinamica)
        {
            AuraDinamica->SetVectorParameterValue(TEXT("CorDaAura"), CorDoNucleo);
            // Fraca de proposito: a aura e nevoa, nao luz. Se subir, ela come o cristal.
            AuraDinamica->SetScalarParameterValue(TEXT("BrilhoDaAura"), 0.22f);
        }

        for (int32 Indice = 0; Indice < PlanosDaAura.Num(); ++Indice)
        {
            if (UStaticMeshComponent* Plano = PlanosDaAura[Indice])
            {
                if (PlanoBase)
                {
                    Plano->SetStaticMesh(PlanoBase);
                }
                if (AuraDinamica)
                {
                    Plano->SetMaterial(0, AuraDinamica);
                }
                // O plano do engine tem 100 cm: esta escala da uma nuvem de ~2,2 m em volta de um cristal de 1,6 m.
                EscalaBaseDaAura = Ajustes->EscalaDaAura * Escala;
                Plano->SetRelativeScale3D(FVector(EscalaBaseDaAura));
                Plano->SetVisibility(true);
            }
        }
    }

    // A aura: a nuvem de gelo em volta do cristal.
    if (Aura)
    {
        if (AuraSystem)
        {
            Aura->SetAsset(AuraSystem);
            Aura->SetVisibility(true);
            Aura->Activate(true);
        }
        else
        {
            Aura->Deactivate();
            Aura->SetVisibility(false);
        }
    }

    // A luz do nucleo usa a COR QUE VEIO DO DADO (o CoreColor do chassi), nao uma leitura de volta do
    // material: assim a vitrine e a tabela do jogo nao podem divergir.
    if (LuzDoNucleo)
    {
        LuzDoNucleo->SetLightColor(CorDoNucleo);
        LuzDoNucleo->SetIntensity(3200.f);
    }

    // A LUMINESCENCIA e do cristal, e o codigo e que manda nela: o cristal aceso por dentro com a cor do
    // chassi, forte o bastante para ele ler como fonte de luz propria dentro da caixa da vitrine.
    if (Material)
    {
        if (UMaterialInstanceDynamic* CristalDinamico = UMaterialInstanceDynamic::Create(Material, this))
        {
            CristalDinamico->SetVectorParameterValue(TEXT("CorDoNucleo"), CorDoNucleo);
            CristalDinamico->SetScalarParameterValue(TEXT("Brilho"), 0.75f);
            Nucleo->SetMaterial(0, CristalDinamico);
        }
    }
}

void ARunnerPreviewActor::SetPreview(const TSoftObjectPtr<USkeletalMesh>& InMesh, const FLinearColor& Accent, UAnimSequenceBase* IdleAnim)
{
    if (!Corpo)
    {
        return;
    }

    // Corpo na tela: o cristal e a aura saem (a aba CLASSE nao e sobre o chassi).
    Corpo->SetVisibility(true);
    Corpo->SetComponentTickEnabled(true);
    if (Nucleo)
    {
        Nucleo->SetVisibility(false);
    }
    if (Aura)
    {
        Aura->Deactivate();
        Aura->SetVisibility(false);
    }
    for (UStaticMeshComponent* Plano : PlanosDaAura)
    {
        if (Plano)
        {
            Plano->SetVisibility(false);
        }
    }
    if (LuzDoNucleo)
    {
        LuzDoNucleo->SetIntensity(0.f);
    }

    if (USkeletalMesh* Malha = InMesh.LoadSynchronous())
    {
        Corpo->SetSkeletalMeshAsset(Malha);
    }

    // Mesma tinta do jogo: material de overlay com o parametro AccentColor.
    if (const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>())
    {
        if (UMaterialInterface* Overlay = Ajustes->AccentOverlayMaterial.LoadSynchronous())
        {
            if (!OverlayDinamico)
            {
                OverlayDinamico = UMaterialInstanceDynamic::Create(Overlay, this);
            }
            if (OverlayDinamico)
            {
                OverlayDinamico->SetVectorParameterValue(TEXT("AccentColor"), Accent);
                // Opacidade parcial: na vitrine a tinta precisa deixar ler o volume do corpo.
                OverlayDinamico->SetScalarParameterValue(TEXT("AccentOpacity"), 0.45f);
                Corpo->SetOverlayMaterial(OverlayDinamico);
            }
        }
    }

    if (IdleAnim)
    {
        Corpo->SetAnimationMode(EAnimationMode::AnimationSingleNode);
        Corpo->PlayAnimation(IdleAnim, true);
    }
}

void ARunnerPreviewActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Giro lento: mostra o volume do chassi em vez de uma foto de frente.
    if (GrausPorSegundo == 0.f)
    {
        return;
    }

    const FRotator Passo(0.f, GrausPorSegundo * DeltaSeconds, 0.f);
    if (Corpo && Corpo->IsVisible())
    {
        Corpo->AddLocalRotation(Passo);
    }
    // O cristal gira junto com a aura: e o que faz a nuvem de gelo circundar o nucleo.
    if (Nucleo && Nucleo->IsVisible())
    {
        Nucleo->AddLocalRotation(Passo);
    }
    // A nevoa gira mais devagar que o cristal: da a leitura de nuvem circulando em volta.
    // A nevoa gira em velocidade DIFERENTE da do cristal e cada plano no seu passo: e o que da a leitura de
    // nuvem circulando em volta, e nao de um objeto preso ao cristal.
    TempoDaAura += DeltaSeconds;
    for (int32 Indice = 0; Indice < PlanosDaAura.Num(); ++Indice)
    {
        UStaticMeshComponent* Plano = PlanosDaAura[Indice];
        if (!Plano || !Plano->IsVisible())
        {
            continue;
        }

        const float PassoDaNevoa = GrausPorSegundo * DeltaSeconds * (0.32f + 0.13f * Indice);
        Plano->AddLocalRotation(FRotator(0.f, PassoDaNevoa, 0.f));

        // Respiracao: a nuvem cresce e encolhe, e a mudanca de densidade e que faz a nevoa parecer que se
        // dissipa - em vez de ser uma placa fixa em volta do cristal.
        const float Fase = TempoDaAura * 0.7f + Indice * 2.1f;
        const float Respiro = 1.f + 0.10f * FMath::Sin(Fase);
        // A escala-base e guardada, nao lida de volta do componente: ler de volta faria o pulso compor a
        // cada quadro e a nuvem cresceria sem parar.
        Plano->SetRelativeScale3D(FVector(EscalaBaseDaAura * Respiro));
    }
}
