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

    /**
     * Mostra o CRISTAL do chassi girando (a aba CHASSI), com a aura em volta.
     * O corpo sai de cena: chassi e cristal, corpo e classe - sao eixos diferentes.
     */
    void SetNucleo(const TSoftObjectPtr<UStaticMesh>& InMesh, class UMaterialInterface* Material, class UNiagaraSystem* Aura, const FLinearColor& CorDoNucleo);

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

    /** O cristal do chassi (a aba CHASSI mostra ele, nao o corpo). */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<class UStaticMeshComponent> Nucleo;

    /**
     * A aura em volta do cristal: TRES PLANOS CRUZADOS de nevoa, nao uma casca esferica.
     * Uma esfera aditiva sempre le como bola acesa (foi o que a primeira versao mostrou); planos com
     * sprite de nevoa macia, girando em velocidades diferentes e respirando, leem como uma nuvem fraca
     * que envolve o cristal e se dissipa - que e o pedido do autor.
     */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TArray<TObjectPtr<class UStaticMeshComponent>> PlanosDaAura;

    /** Efeito Niagara opcional, por cima da casca. */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<class UNiagaraComponent> Aura;

    /** Luz do nucleo: acende o cristal com a cor dele (canal 1). */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<class UPointLightComponent> LuzDoNucleo;

    /** Luz principal da vitrine (canal 1: so o corpo da vitrine responde). */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<UDirectionalLightComponent> LuzPrincipal;

    /** Preenchimento frio do lado oposto (canal 1). */
    UPROPERTY(VisibleAnywhere, Category = "Vitrine") TObjectPtr<UDirectionalLightComponent> LuzDePreenchimento;

    UPROPERTY(Transient) TObjectPtr<UTextureRenderTarget2D> RenderTarget;
    UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> OverlayDinamico;

    /** Instancia dinamica da aura: recebe a COR do chassi para tingir a nevoa de gelo. */
    UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> AuraDinamica;

    /** Escala-base dos planos da nevoa (guardada para o pulso nao compor a cada quadro). */
    float EscalaBaseDaAura = 2.6f;

    /** Relogio proprio da nevoa, so para a respiracao. */
    float TempoDaAura = 0.f;
};
