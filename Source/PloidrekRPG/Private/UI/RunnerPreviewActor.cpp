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
    // QUATRO cartoes de nevoa (referencia do autor: fumaca de gelo seco numa foto):
    //   [0] a POCA   - um cartao DEITADO no chao, largo: a nevoa que escorre e se acumula na base;
    //   [1..3] as PLUMAS - tres cartoes EM PE, cruzados a 60 graus, com a textura de VOLUTAS.
    // O que faz ler como fumaca e a TEXTURA (volutas densas com fios soltos), nao a geometria - foi a licao
    // das esferas lisas, que liam como blobo, e dos planos com puff macio, que liam como painel.
    CascasDaNevoa.Reserve(4);
    for (int32 Indice = 0; Indice < 4; ++Indice)
    {
        UStaticMeshComponent* Casca = CreateDefaultSubobject<UStaticMeshComponent>(
            *FString::Printf(TEXT("CascaDaNevoa%d"), Indice));
        Casca->SetupAttachment(Raiz);
        Casca->SetMobility(EComponentMobility::Movable);
        Casca->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Casca->SetLightingChannels(false, true, false);
        Casca->SetCastShadow(false);
        Casca->SetVisibility(false);
        CascasDaNevoa.Add(Casca);
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
    if (Ajustes && CascasDaNevoa.Num() > 0)
    {
        UStaticMesh* CartaoBase = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));

        // A POCA usa material proprio (a textura de nevoa baixa e larga). O caminho e configurado aqui porque e
        // um material do kit, nao um dado de balanceamento.
        UMaterialInterface* MaterialDaPoca = LoadObject<UMaterialInterface>(nullptr,
            TEXT("/Game/AI_Assets/materials/M_AuraPoca.M_AuraPoca"));
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

        // O cartao do engine tem 100 cm. A POCA fica DEITADA (sem rotacao: o plano nasce deitado) e bem larga;
        // as PLUMAS ficam EM PE (pitch 90) e cruzadas a 60 graus, para a fumaca existir de qualquer angulo.
        const float Base = Ajustes->EscalaDaAura * Escala;
        EscalaBaseDaAura = Base;
        for (int32 Indice = 0; Indice < CascasDaNevoa.Num(); ++Indice)
        {
            if (UStaticMeshComponent* Casca = CascasDaNevoa[Indice])
            {
                if (CartaoBase)
                {
                    Casca->SetStaticMesh(CartaoBase);
                }
                if (Indice == 0)
                {
                    // a POCA: deitada no chao, larga e rasa, um pouco abaixo do cristal
                    if (MaterialDaPoca)
                    {
                        Casca->SetMaterial(0, MaterialDaPoca);
                    }
                    Casca->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
                    Casca->SetRelativeScale3D(FVector(Base * 2.2f, Base * 1.5f, 1.f));
                    Casca->SetRelativeLocation(FVector(0.f, 0.f, -75.f));
                }
                else
                {
                    // as PLUMAS: em pe, cruzadas, de alturas levemente diferentes para nao parecer um objeto
                    if (AuraDinamica)
                    {
                        Casca->SetMaterial(0, AuraDinamica);
                    }
                    const float Altura = 1.f + 0.12f * (Indice - 2);
                    Casca->SetRelativeRotation(FRotator(90.f, 60.f * (Indice - 1), 0.f));
                    Casca->SetRelativeScale3D(FVector(Base * 1.1f, Base * 1.35f * Altura, 1.f));
                    Casca->SetRelativeLocation(FVector(0.f, 0.f, 10.f * (Indice - 2)));
                }
                Casca->SetVisibility(true);
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
    for (UStaticMeshComponent* Casca : CascasDaNevoa)
    {
        if (Casca)
        {
            Casca->SetVisibility(false);
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
    // A NEVOA DE GELO SECO tem dois movimentos, e nenhum deles e giro rapido:
    //   - ROLAGEM lenta em torno do eixo Z (a nevoa se arrasta, nao orbita);
    //   - RESPIRACAO assimetrica: a poca incha e espalha (e o que faz ler como vapor acumulando), enquanto o
    //     manto sobe e desce de leve. O movimento para BAIXO em si vem do material (panner vertical negativo).
    TempoDaAura += DeltaSeconds;
    for (int32 Indice = 0; Indice < CascasDaNevoa.Num(); ++Indice)
    {
        UStaticMeshComponent* Casca = CascasDaNevoa[Indice];
        if (!Casca || !Casca->IsVisible())
        {
            continue;
        }

        const float PassoDaNevoa = GrausPorSegundo * DeltaSeconds * (Indice == 0 ? 0.18f : 0.30f);
        Casca->AddLocalRotation(FRotator(0.f, PassoDaNevoa, 0.f));

        const FVector Base = (Indice == 0)
            ? FVector(EscalaBaseDaAura * 2.4f, EscalaBaseDaAura * 2.4f, EscalaBaseDaAura * 0.42f)
            : FVector(EscalaBaseDaAura * 0.95f, EscalaBaseDaAura * 0.95f, EscalaBaseDaAura * 0.70f);

        // A poca respira mais devagar e mais fundo que o manto: vapor acumulado se espalha, nao pulsa.
        const float Fase = TempoDaAura * (Indice == 0 ? 0.45f : 0.8f) + Indice * 1.7f;
        const float Respiro = 1.f + (Indice == 0 ? 0.14f : 0.08f) * FMath::Sin(Fase);
        Casca->SetRelativeScale3D(FVector(Base.X * Respiro, Base.Y * Respiro, Base.Z));
    }
}
