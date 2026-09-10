#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "RunnerVeilWidget.generated.h"

class UTexture2D;
class UWidget;

/**
 * O veu: neblina de vapor que circula pela tela, ronda os botoes disponiveis (chegando a passar
 * por cima deles) e se abre numa bola grande de dispersao ao redor do mouse.
 *
 * Por que em C++/Slate e nao num material: e um efeito de INTERFACE — ele segue o cursor e o
 * retangulo dos botoes, coisas que o UMG conhece. E nao depende de nenhum asset: o gradiente
 * radial e gerado em memoria (UTexture2D::CreateTransient).
 *
 * Cores da paleta canonica: vapor #c8c0a8 (a neblina) e forja #c86a30 (o calor do nucleo do mouse).
 *
 * Console: Runner.Veil 0 desliga (e 1 liga).
 */
UCLASS()
class PLOIDREKRPG_API URunnerVeilWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                              FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                              bool bParentEnabled) const override;

    /** Botoes e caixas que o veu ronda. Sao guardados fracos: a tela pode se reconstruir. */
    void SetTargets(const TArray<UWidget*>& InTargets);

    /** Intensidade geral do veu (0 desliga na pratica). */
    UPROPERTY(EditAnywhere, Category = "Veu") float Intensidade = 1.f;

    /** Raio da bola de dispersao que sempre acompanha o mouse, em pixels. */
    UPROPERTY(EditAnywhere, Category = "Veu") float RaioDaBolaDoMouse = 300.f;

    /** Quantos fios de neblina circulam pela tela. */
    UPROPERTY(EditAnywhere, Category = "Veu") int32 NumeroDeFios = 9;

    /** Velocidade da circulacao. */
    UPROPERTY(EditAnywhere, Category = "Veu") float Velocidade = 1.f;

private:
    /** Cria a textura do degrade radial uma vez, na construcao (nao no meio do desenho). */
    void CriarNeblina();

    const FSlateBrush* GetNeblinaBrush() const { return NeblinaTexture ? &NeblinaBrush : nullptr; }

    void DesenharNeblina(FSlateWindowElementList& OutDrawElements, int32 LayerId, const FGeometry& AllottedGeometry,
                         const FVector2f& Centro, float Raio, const FLinearColor& Cor) const;

    UPROPERTY(Transient) TObjectPtr<UTexture2D> NeblinaTexture;
    FSlateBrush NeblinaBrush;
    UPROPERTY() TArray<TObjectPtr<UWidget>> Alvos;
};
