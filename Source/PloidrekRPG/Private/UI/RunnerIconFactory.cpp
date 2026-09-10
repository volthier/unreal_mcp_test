#include "UI/RunnerIconFactory.h"

#include "Engine/Texture2D.h"
#include "UI/RunnerPalette.h"

namespace
{
    /** Distancia de um ponto a um segmento: e o que permite desenhar tracos (chevron, X, mais). */
    float DistanciaAoSegmento(const FVector2D& P, const FVector2D& A, const FVector2D& B)
    {
        const FVector2D AB = B - A;
        const FVector2D AP = P - A;
        const float Denominador = FVector2D::DotProduct(AB, AB);
        const float T = Denominador > KINDA_SMALL_NUMBER
            ? FMath::Clamp(FVector2D::DotProduct(AP, AB) / Denominador, 0.f, 1.f) : 0.f;
        return FVector2D::Distance(P, A + AB * T);
    }

    /** Cobertura (0..1) a partir de uma distancia assinada — a suavidade evita serrilhado. */
    float Cobertura(const float Sdf, const float Suavidade = 0.07f)
    {
        return FMath::Clamp(0.5f - Sdf / Suavidade, 0.f, 1.f);
    }

    /** Contorno de um traco: desenha a BORDA do glifo, nao o glifo cheio. */
    float ContornoDoTracado(const float Distancia, const float Espessura, const float MeiaLargura)
    {
        return Cobertura(FMath::Abs(Distancia - Espessura) - MeiaLargura);
    }

    /** Distancia assinada do tracado de um glifo, em coordenadas normalizadas (-1..1). */
    float SdfDoGlifo(const ERunnerIconGlyph Glifo, const FVector2D& P)
    {
        switch (Glifo)
        {
            case ERunnerIconGlyph::Triangulo:
            {
                // Chevron: duas hastes formando um ^.
                const float A = DistanciaAoSegmento(P, FVector2D(-0.58f, 0.34f), FVector2D(0.f, -0.42f));
                const float B = DistanciaAoSegmento(P, FVector2D(0.f, -0.42f), FVector2D(0.58f, 0.34f));
                return FMath::Min(A, B) - 0.30f;
            }
            case ERunnerIconGlyph::Quadrado:
            {
                const FVector2D Q(FMath::Abs(P.X), FMath::Abs(P.Y));
                return FMath::Max(Q.X, Q.Y) - 0.62f;
            }
            case ERunnerIconGlyph::Losango:
                return FMath::Abs(P.X) + FMath::Abs(P.Y) - 0.78f;
            case ERunnerIconGlyph::Cruz:
            {
                const float A = DistanciaAoSegmento(P, FVector2D(-0.52f, -0.52f), FVector2D(0.52f, 0.52f));
                const float B = DistanciaAoSegmento(P, FVector2D(-0.52f, 0.52f), FVector2D(0.52f, -0.52f));
                return FMath::Min(A, B) - 0.30f;
            }
            case ERunnerIconGlyph::Anel:
                return FMath::Abs(P.Size() - 0.62f);
            case ERunnerIconGlyph::Mais:
            {
                const float A = DistanciaAoSegmento(P, FVector2D(-0.55f, 0.f), FVector2D(0.55f, 0.f));
                const float B = DistanciaAoSegmento(P, FVector2D(0.f, -0.55f), FVector2D(0.f, 0.55f));
                return FMath::Min(A, B) - 0.30f;
            }
            case ERunnerIconGlyph::Escudo:
            default:
            {
                // Escudo: caixa com os cantos de baixo arredondados.
                const float Rabisco = FMath::Max(FMath::Abs(P.X) - 0.55f, P.Y - 0.58f);
                const float Fundo = FMath::Sqrt(FMath::Square(FMath::Max(FMath::Abs(P.X) - 0.28f, 0.f))
                                              + FMath::Square(FMath::Max(P.Y - 0.28f, 0.f))) - 0.27f;
                return FMath::Max(Rabisco, Fundo);
            }
        }
    }
}

ERunnerIconGlyph RunnerIcons::GlifoDoCorpo(const ERunnerBodyType Corpo)
{
    switch (Corpo)
    {
        case ERunnerBodyType::Slender:  return ERunnerIconGlyph::Triangulo;
        case ERunnerBodyType::Stocky:   return ERunnerIconGlyph::Quadrado;
        case ERunnerBodyType::Fragile:  return ERunnerIconGlyph::Losango;
        case ERunnerBodyType::Unstable: return ERunnerIconGlyph::Cruz;
        default:                        return ERunnerIconGlyph::Anel;
    }
}

ERunnerIconGlyph RunnerIcons::GlifoDoPapel(const ERunnerRole Papel)
{
    switch (Papel)
    {
        case ERunnerRole::Tank:    return ERunnerIconGlyph::Escudo;
        case ERunnerRole::Support: return ERunnerIconGlyph::Mais;
        case ERunnerRole::Control: return ERunnerIconGlyph::Anel;
        case ERunnerRole::Hybrid:  return ERunnerIconGlyph::Losango;
        case ERunnerRole::DPS:
        default:                   return ERunnerIconGlyph::Cruz;
    }
}

UTexture2D* RunnerIcons::CriarIcone(UObject* Dono, const ERunnerIconGlyph Glifo, const FLinearColor& CorDoItem, const int32 Lado)
{
    if (Lado <= 0)
    {
        return nullptr;
    }

    UTexture2D* Textura = UTexture2D::CreateTransient(Lado, Lado, PF_B8G8R8A8);
    if (!Textura)
    {
        return nullptr;
    }

    Textura->SRGB = true;
    Textura->Filter = TF_Bilinear;
    Textura->AddressX = TA_Clamp;
    Textura->AddressY = TA_Clamp;
    Textura->NeverStream = true;
    Textura->CompressionSettings = TC_VectorDisplacementmap; // sem compressao: alfa intacto

    // A placa e a cor de fundo da interface (silhueta), um degrau mais clara que o painel.
    const FColor CorDaPlaca = RunnerPalette::AcoEscuro(1.f).ToFColor(true);
    const FColor CorDoGlifo = CorDoItem.ToFColor(true);

    FTexture2DMipMap& Mip = Textura->GetPlatformData()->Mips[0];
    uint8* Dados = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_WRITE));

    for (int32 Y = 0; Y < Lado; ++Y)
    {
        for (int32 X = 0; X < Lado; ++X)
        {
            // Coordenadas normalizadas em -1..1 (independentes do tamanho do icone).
            const FVector2D P((X + 0.5f) / Lado * 2.f - 1.f, (Y + 0.5f) / Lado * 2.f - 1.f);

            // Placa arredondada: e ela que da o formato de "chip" do icone.
            const FVector2D Q(FMath::Abs(P.X) - 0.84f, FMath::Abs(P.Y) - 0.84f);
            const float Rabisco = FVector2D(FMath::Max(Q.X, 0.f), FMath::Max(Q.Y, 0.f)).Size();
            const float SdfDaPlaca = Rabisco + FMath::Min(FMath::Max(Q.X, Q.Y), 0.f) - 0.16f;
            const float AlfaDaPlaca = Cobertura(SdfDaPlaca, 0.06f);

            // Brilho suave atras do glifo: da volume sem precisar de sombra.
            const float Raio = P.Size();
            const float Brilho = FMath::Pow(FMath::Clamp(1.f - Raio / 1.15f, 0.f, 1.f), 2.f) * 0.55f;

            const float SdfDoContorno = SdfDoGlifo(Glifo, P);
            const float Traco = ContornoDoTracado(FMath::Abs(SdfDoContorno), 0.0f, 0.055f);

            // Compõe: placa -> brilho -> glifo.
            const float R = FMath::Lerp(CorDaPlaca.R, CorDoGlifo.R, FMath::Clamp(Traco, 0.f, 1.f)) + CorDoGlifo.R * Brilho * 0.35f;
            const float G = FMath::Lerp(CorDaPlaca.G, CorDoGlifo.G, FMath::Clamp(Traco, 0.f, 1.f)) + CorDoGlifo.G * Brilho * 0.35f;
            const float B = FMath::Lerp(CorDaPlaca.B, CorDoGlifo.B, FMath::Clamp(Traco, 0.f, 1.f)) + CorDoGlifo.B * Brilho * 0.35f;

            uint8* Pixel = Dados + ((Y * Lado) + X) * 4;
            Pixel[0] = static_cast<uint8>(FMath::Clamp(R, 0.f, 255.f));
            Pixel[1] = static_cast<uint8>(FMath::Clamp(G, 0.f, 255.f));
            Pixel[2] = static_cast<uint8>(FMath::Clamp(B, 0.f, 255.f));
            Pixel[3] = static_cast<uint8>(AlfaDaPlaca * 255.f);
        }
    }

    Mip.BulkData.Unlock();
    Textura->UpdateResource();
    return Textura;
}
