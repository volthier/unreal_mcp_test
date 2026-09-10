#pragma once

#include "CoreMinimal.h"
#include "Data/RunnerTypes.h"

class UTexture2D;

/**
 * Os icones da criacao sao DESENHADOS por codigo — nada de asset de arte ainda.
 *
 * Cada chassi e cada classe ganham uma placa escura com um brilho suave e um glifo na cor do item
 * (a cor de tinta do chassi, ou a cor do papel da classe). Quando existir arte propria, e so trocar
 * a chamada do widget por um asset: o resto da tela nao muda.
 */
enum class ERunnerIconGlyph : uint8
{
    Triangulo,
    Quadrado,
    Losango,
    Cruz,
    Anel,
    Mais,
    Escudo
};

namespace RunnerIcons
{
    /** Glifo que representa o tipo de corpo do chassi. */
    PLOIDREKRPG_API ERunnerIconGlyph GlifoDoCorpo(ERunnerBodyType Corpo);

    /** Glifo que representa o papel da classe. */
    PLOIDREKRPG_API ERunnerIconGlyph GlifoDoPapel(ERunnerRole Papel);

    /**
     * Cria a textura do icone em memoria (Lado x Lado).
     * Dono guarda o objeto (o widget) para o texture nao ser coletado.
     */
    PLOIDREKRPG_API UTexture2D* CriarIcone(UObject* Dono, ERunnerIconGlyph Glifo,
                                           const FLinearColor& CorDoItem, int32 Lado = 64);
}
