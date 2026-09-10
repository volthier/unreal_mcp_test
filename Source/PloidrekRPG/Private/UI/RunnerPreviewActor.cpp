#include "UI/RunnerPreviewActor.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
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

void ARunnerPreviewActor::SetPreview(const TSoftObjectPtr<USkeletalMesh>& InMesh, const FLinearColor& Accent, UAnimSequenceBase* IdleAnim)
{
    if (!Corpo)
    {
        return;
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
    if (Corpo && GrausPorSegundo != 0.f)
    {
        Corpo->AddLocalRotation(FRotator(0.f, GrausPorSegundo * DeltaSeconds, 0.f));
    }
}
