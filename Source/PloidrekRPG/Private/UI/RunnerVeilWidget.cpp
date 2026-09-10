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
    CriarNuvens();
}

void URunnerVeilWidget::CriarNuvens()
{
    // Semente fixa: o ceu do menu e sempre o mesmo desenho, quem muda e o tempo.
    FRandomStream Sorteio(20260913);

    Nuvens.Reset();
    for (int32 Indice = 0; Indice < NumeroDeNuvens; ++Indice)
    {
        FRunnerNuvemDeVeu Nuvem;
        Nuvem.Base = FVector2f(Sorteio.FRandRange(-0.2f, 1.2f), Sorteio.FRandRange(-0.15f, 1.15f));

        // Quase horizontal, com duas direcoes: nuvens cruzando em sentidos opostos dao profundidade.
        const float Inclinacao = Sorteio.FRandRange(-0.30f, 0.30f);
        const float Sentido = Sorteio.FRand() < 0.5f ? 1.f : -1.f;
        Nuvem.Direcao = FVector2f(FMath::Cos(Inclinacao) * Sentido, FMath::Sin(Inclinacao));

        Nuvem.Velocidade = Sorteio.FRandRange(0.012f, 0.048f);
        Nuvem.Escala = Sorteio.FRandRange(170.f, 430.f);
        Nuvem.Fase = Sorteio.FRandRange(0.f, 120.f);
        // Quatro em cada dez sombreiam: e o que faz a interface ler como sob nevoa, nao sob luz.
        Nuvem.bEscurece = Sorteio.FRand() < 0.4f;

        Nuvens.Add(Nuvem);
    }
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
        const float Respiro = 1.f + 0.06f * FMath::Sin(Tempo * 1.3f);
        // A dispersao abre no cursor, mas macia e larga: o veu nao pode virar foco de lanterna.
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Cursor, RaioDaBolaDoMouse * 1.90f * Respiro,
                        RunnerPalette::Vapor(0.13f * Intensidade));
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Cursor, RaioDaBolaDoMouse * 1.15f * Respiro,
                        RunnerPalette::Vapor(0.24f * Intensidade));
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, Cursor, RaioDaBolaDoMouse * 0.50f * Respiro,
                        RunnerPalette::HorizonteForja(0.20f * Intensidade));
    }

    // ------------------------------------------------------------------
    // 2. NUVENS que voam pela tela e dão a volta pelas bordas. Cada uma tem direção, tamanho e
    //    peso próprios; quatro em cada dez SOMBREIAM a interface (névoa de guerra) e o resto
    //    clareia de leve. É o movimento delas — não o cursor — que dá a leitura de nuvem.
    // ------------------------------------------------------------------
    const float Margem = 520.f;
    const float FaixaX = Tamanho.X + Margem * 2.f;
    const float FaixaY = Tamanho.Y + Margem * 2.f;

    for (const FRunnerNuvemDeVeu& Nuvem : Nuvens)
    {
        float X = FMath::Fmod(Nuvem.Base.X * FaixaX + Nuvem.Direcao.X * Nuvem.Velocidade * FaixaX * Tempo, FaixaX);
        if (X < 0.f)
        {
            X += FaixaX;
        }
        float Y = FMath::Fmod(Nuvem.Base.Y * FaixaY + Nuvem.Direcao.Y * Nuvem.Velocidade * FaixaY * Tempo, FaixaY);
        if (Y < 0.f)
        {
            Y += FaixaY;
        }

        const FVector2f CentroDaNuvem(X - Margem, Y - Margem);
        const float RespiroDaNuvem = 1.f + 0.14f * FMath::Sin(Tempo * 0.22f + Nuvem.Fase);
        const float Raio = Nuvem.Escala * RespiroDaNuvem;

        const FLinearColor Cor = Nuvem.bEscurece
            ? RunnerPalette::Silhueta(0.17f * Intensidade)
            : RunnerPalette::Vapor(0.10f * Intensidade);

        // Três lóbulos: dá contorno de nuvem em vez de um disco perfeito.
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry, CentroDaNuvem, Raio, Cor);
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry,
                        CentroDaNuvem + FVector2f(Raio * 0.58f, -Raio * 0.20f), Raio * 0.70f, Cor);
        DesenharNeblina(OutDrawElements, Camada, AllottedGeometry,
                        CentroDaNuvem + FVector2f(-Raio * 0.52f, Raio * 0.24f), Raio * 0.62f, Cor);
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
                        RunnerPalette::Vapor(0.07f * Intensidade));
        ++IndiceAlvo;
    }

    return Camada;
}
