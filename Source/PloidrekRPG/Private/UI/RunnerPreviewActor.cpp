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
    /** Bem longe e bem alto: o jogador nunca ve o boneco no cenario do menu. */
    const FVector LocalDaVitrine(0.f, 0.f, 50000.f);

    /** Altura da camera em relacao ao corpo (o manequim vai de -90 a +90). */
    constexpr float AlturaDaCamera = 20.f;
}

ARunnerPreviewActor::ARunnerPreviewActor()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
    SetRootComponent(Raiz);

    Corpo = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Corpo"));
    Corpo->SetupAttachment(Raiz);
    Corpo->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // mesmo ajuste do pawn do jogo
    Corpo->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    Corpo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Corpo->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    Corpo->bCastDynamicShadow = true;

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

    LuzDePreenchimento = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("LuzDePreenchimento"));
    LuzDePreenchimento->SetupAttachment(Raiz);
    LuzDePreenchimento->SetMobility(EComponentMobility::Movable);
    LuzDePreenchimento->SetRelativeRotation(FRotator(-20.f, -35.f, 0.f));
    LuzDePreenchimento->SetIntensity(0.6f);
    LuzDePreenchimento->SetLightColor(RunnerPalette::Ciano(1.f));
    LuzDePreenchimento->SetCastShadows(false);
}

void ARunnerPreviewActor::BeginPlay()
{
    Super::BeginPlay();

    // A vitrine vive no ceu, fora do alcance da vista do menu.
    SetActorLocation(LocalDaVitrine);

    if (!RenderTarget)
    {
        RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, LadoDaCaptura, LadoDaCaptura,
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
