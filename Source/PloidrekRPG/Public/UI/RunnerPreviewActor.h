#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunnerPreviewActor.generated.h"

class UAnimSequenceBase;
class UDirectionalLightComponent;
class UMaterialInstanceDynamic;
class USceneCaptureComponent2D;
class USkeletalMesh;
class USkeletalMeshComponent;
class UTextureRenderTarget2D;

/**
 * A vitrine do menu: o corpo do chassi escolhido, girando devagar, capturado para uma textura que a
 * interface mostra como se fosse um monitor de oficina.
 *
 * Como funciona: o ator fica bem longe do mapa (ninguem ve o boneco no cenario), com uma camera de
 * captura (USceneCaptureComponent2D) apontada para o corpo. A captura renderiza SO este ator
 * (PrimitiveRenderMode = UseShowOnlyList), entao o resto do mapa nao entra na imagem; a luz do mundo
 * (sol e skylight) continua valendo, e uma luz de preenchimento bem fraca tira o lado morto.
 *
 * A cor de destaque do chassi entra pelo mesmo material de overlay do jogo (parametro AccentColor).
 */
UCLASS()
class PLOIDREKRPG_API ARunnerPreviewActor : public AActor
{
    GENERATED_BODY()

public:
    ARunnerPreviewActor();

    virtual void Tick(float DeltaSeconds) override;

    /** Monta a vitrine com o corpo do chassi, a tinta dele e a animacao de espera. */
    void SetPreview(const TSoftObjectPtr<USkeletalMesh>& InMesh, const FLinearColor& Accent, UAnimSequenceBase* IdleAnim);

    /** Textura que o widget mostra (o "monitor"). */
    UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

    /** Giro da vitrine, em graus por segundo. */
    UPROPERTY(EditAnywhere, Category = "Vitrine") float GrausPorSegundo = 16.f;

    /** Lado da textura de captura. */
    UPROPERTY(EditAnywhere, Category = "Vitrine") int32 LadoDaCaptura = 512;

    /** Distancia da camera ao corpo, em centimetros. */
    UPROPERTY(EditAnywhere, Category = "Vitrine") float DistanciaDaCamera = 280.f;

    /** Campo de visao da camera da vitrine. */
    UPROPERTY(EditAnywhere, Category = "Vitrine") float CampoDeVisao = 45.f;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<USkeletalMeshComponent> Corpo;
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<USceneCaptureComponent2D> Captura;

    /** Luz principal da vitrine (canal 1: so o corpo da vitrine responde). */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<UDirectionalLightComponent> LuzPrincipal;

    /** Preenchimento frio do lado oposto (canal 1). */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<UDirectionalLightComponent> LuzDePreenchimento;

    UPROPERTY(Transient) TObjectPtr<UTextureRenderTarget2D> RenderTarget;
    UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> OverlayDinamico;
};
