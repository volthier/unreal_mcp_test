#include "UI/RunnerVeilWidget.h"

#include "Components/Widget.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "UI/RunnerPalette.h"
#include "Widgets/SWindow.h"

namespace
{
    /** Lado da textura de neblina gerada em memoria (potencia de dois, com folga para o degrade). */
    constexpr int32 TamanhoDaTextura = 128;

    /** Angulo aureo: espalha as fases dos fios sem que eles se sobreponham no mesmo lugar. */
    constexpr float AnguloAureo = 2.399963f;

    /** Converte qualquer vetor de Slate (FVector2D ou FVector2f) para FVector2f, sem ambiguidade. */
    template <typename TVetor>
    FVector2f ParaVector2f(const TVetor& Vetor)
    {
        return FVector2f(static_cast<float>(Vetor.X), static_cast<float>(Vetor.Y));
    }
}

static TAutoConsoleVariable<int32> CVarRunnerVeu(
    TEXT("Runner.Veil"),
    1,
    TEXT("Veu de vapor na interface do menu: 1 liga, 0 desliga."),
    ECVF_Default);

void URunnerVeilWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // O veu e so pintura: nao pode roubar clique dos botoes.
    SetVisibility(ESlateVisibility::HitTestInvisible);

    CriarNeblina();
}

void URunnerVeilWidget::SetTargets(const TArray<UWidget*>& InTargets)
{
    Alvos.Reset();
    for (UWidget* Alvo : InTargets)
    {
        if (Alvo)
        {
            Alvos.Add(Alvo);
        }
    }
}

void URunnerVeilWidget::CriarNeblina()
{
    UTexture2D* Textura = UTexture2D::CreateTransient(TamanhoDaTextura, TamanhoDaTextura, PF_B8G8R8A8);
    if (!Textura)
    {
        return;
    }

    Textura->SRGB = false;
    Textura->Filter = TF_Bilinear;
    Textura->AddressX = TA_Clamp;
    Textura->AddressY = TA_Clamp;
    Textura->NeverStream = true;
    Textura->CompressionSettings = TC_VectorDisplacementmap; // sem compressao: alfa intacto

    const float Centro = TamanhoDaTextura * 0.5f;
    FTexture2DMipMap& Mip = Textura->GetPlatformData()->Mips[0];
    uint8* Dados = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
    for (int32 Y = 0; Y < TamanhoDaTextura; ++Y)
    {
        for (int32 X = 0; X < TamanhoDaTextura; ++X)
        {
            const float dx = (X + 0.5f - Centro) / Centro;
            const float dy = (Y + 0.5f - Centro) / Centro;
            const float Distancia = FMath::Sqrt(dx * dx + dy * dy);

            // Nucleo denso, borda que se dissolve: e o que faz a bola ler como vapor, nao como disco.
            const float Queda = FMath::Clamp(1.f - Distancia, 0.f, 1.f);
            const float Alfa = FMath::Pow(Queda, 2.2f);

            uint8* Pixel = Dados + ((Y * TamanhoDaTextura) + X) * 4;
            Pixel[0] = 255;
            Pixel[1] = 255;
            Pixel[2] = 255;
            Pixel[3] = static_cast<uint8>(FMath::Clamp(Alfa, 0.f, 1.f) * 255.f);
        }
    }
    Mip.BulkData.Unlock();
    Textura->UpdateResource();

    NeblinaTexture = Textura;
    NeblinaBrush.SetResourceObject(Textura);
    NeblinaBrush.ImageSize = FVector2D(TamanhoDaTextura, TamanhoDaTextura);
    NeblinaBrush.DrawAs = ESlateBrushDrawType::Image;
    NeblinaBrush.TintColor = FSlateColor(FLinearColor::White);
}

void URunnerVeilWidget::DesenharNeblina(FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                        const FGeometry& AllottedGeometry, const FVector2f& Centro, float Raio,
                                        const FLinearColor& Cor) const
{
    const FSlateBrush* Brush = GetNeblinaBrush();
    if (!Brush || Raio <= 1.f)
    {
        return;
    }

    const FVector2f Tamanho(Raio * 2.f, Raio * 2.f);
    const FSlateLayoutTransform Posicao(Centro - FVector2f(Raio, Raio));
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(Tamanho, Posicao),
                               Brush, ESlateDrawEffect::None, Cor);
}

int32 URunnerVeilWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                                     FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                                     bool bParentEnabled) const
{
    const int32 BaseLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId,
                                               InWidgetStyle, bParentEnabled);

    if (CVarRunnerVeu.GetValueOnGameThread() == 0 || Intensidade <= 0.f)
    {
        return BaseLayer;
    }

    const FVector2f Tamanho = AllottedGeometry.GetLocalSize();
    if (Tamanho.X < 8.f || Tamanho.Y < 8.f)
    {
        return BaseLayer;
    }

    const int32 Camada = BaseLayer + 1;
    const float Tempo = static_cast<float>(FPlatformTime::Seconds()) * Velocidade;

    // ------------------------------------------------------------------
    // 1. A bola de dispersao do mouse: sempre, grande e cheia.
    // ------------------------------------------------------------------
    FVector2D CursorNaTela = FSlateApplication::Get().GetCursorPos();
    if (const TSharedPtr<SWidget> MeuSlate = GetCachedWidget())
    {
        if (const TSharedPtr<SWindow> Janela = FSlateApplication::Get().FindWidgetWindow(MeuSlate.ToSharedRef()))
        {
            // O cursor vem em coordenadas da tela; a geometria do widget fala em coordenadas da janela.
            CursorNaTela -= Janela->GetPositionInScreen();
        }
    }
    const FVector2f Cursor = ParaVector2f(AllottedGeometry.AbsoluteToLocal(CursorNaTela));

    const bool bCursorDentro = Cursor.X >= -RaioDaBolaDoMouse && Cursor.X <= Tamanho.X + RaioDaBolaDoMouse
                            && Cursor.Y >= -RaioDaBolaDoMouse && Cursor.Y <= Tamanho.Y + RaioDaBolaDoMouse;
    if (bCursorDentro)
    {
        // Tres camadas: halo largo (assenta a bola no fundo), neblina cheia e o calor da forja no
        // centro. Os pesos foram calibrados na previa (Tools/preview/veil_preview.py).
        const float Respiro = 1.f + 0.05f * FMath::Sin(Tempo * 1.7f);
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Cursor, RaioDaBolaDoMouse * 1.35f * Respiro,
                        RunnerPalette::Vapor(0.20f * Intensidade));
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Cursor, RaioDaBolaDoMouse * Respiro,
                        RunnerPalette::Vapor(0.55f * Intensidade));
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Cursor, RaioDaBolaDoMouse * 0.42f * Respiro,
                        RunnerPalette::HorizonteForja(0.38f * Intensidade));
    }

    // ------------------------------------------------------------------
    // 2. Fios que circulam pela tela (a nebula de fundo).
    // ------------------------------------------------------------------
    const FVector2f MeioDaTela(Tamanho.X * 0.5f, Tamanho.Y * 0.5f);
    for (int32 Indice = 0; Indice < NumeroDeFios; ++Indice)
    {
        const float Fase = Indice * AnguloAureo;
        const float Alcance = 0.30f + 0.16f * FMath::Sin(Tempo * 0.35f + Fase * 1.7f);

        const FVector2f Centro(MeioDaTela.X + FMath::Cos(Tempo * 0.21f + Fase) * Tamanho.X * Alcance,
                               MeioDaTela.Y + FMath::Sin(Tempo * 0.17f + Fase * 1.3f) * Tamanho.Y * Alcance);
        const float Raio = 120.f + 70.f * FMath::Sin(Tempo * 0.5f + Fase);

        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Centro, Raio,
                        RunnerPalette::Vapor(0.05f * Intensidade));
    }

    // ------------------------------------------------------------------
    // 3. Fios que rondam os botoes — de proposito com raio maior que o botao, para a neblina
    //    passar POR CIMA deles e nao ler como fundo.
    // ------------------------------------------------------------------
    int32 IndiceAlvo = 0;
    for (const TObjectPtr<UWidget>& Alvo : Alvos)
    {
        if (!Alvo)
        {
            continue;
        }

        const FGeometry& GeometriaDoAlvo = Alvo->GetCachedGeometry();
        if (GeometriaDoAlvo.GetLocalSize().IsNearlyZero())
        {
            continue; // ainda sem layout neste quadro
        }

        const FSlateRect Caixa = GeometriaDoAlvo.GetLayoutBoundingRect();
        const FVector2f CentroDoAlvo = ParaVector2f(AllottedGeometry.AbsoluteToLocal(Caixa.GetCenter()));
        const FVector2f TamanhoDoAlvo = ParaVector2f(Caixa.GetSize());

        const float Fase = IndiceAlvo * 1.7f;
        const float RaioX = TamanhoDoAlvo.X * 0.5f + 55.f;
        const float RaioY = TamanhoDoAlvo.Y * 0.5f + 40.f;

        const FVector2f Volta(CentroDoAlvo.X + FMath::Cos(Tempo * 0.9f + Fase) * RaioX,
                              CentroDoAlvo.Y + FMath::Sin(Tempo * 1.25f + Fase) * RaioY);
        const float Raio = 95.f + 35.f * FMath::Sin(Tempo * 0.8f + Fase);

        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Volta, Raio,
                        RunnerPalette::Vapor(0.10f * Intensidade));
        ++IndiceAlvo;
    }

    return Camada;
}
