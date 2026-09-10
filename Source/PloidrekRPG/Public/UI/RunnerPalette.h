#pragma once

#include "CoreMinimal.h"

/**
 * Cores da interface, tiradas do documento canonico Docs/SteampunkPalette.md.
 *
 * Regra: nenhum hexadecimal solto no codigo dos widgets — se a cor nao existe aqui e porque
 * nao esta na paleta do jogo, e ai a discussao e de arte, nao de codigo.
 *
 * Estilo: forja industrial x sci-fi de acao — latao, cobre e ferro forjado, ceu dusk, luzes
 * quentes (ambar) e frias (ciano).
 */
namespace RunnerPalette
{
    /** Converte o hex do documento (0xRRGGBB, sRGB) para linear, com alfa opcional. */
    inline FLinearColor Hex(const uint32 RGB, const float Alpha = 1.f)
    {
        const FColor Cor(static_cast<uint8>((RGB >> 16) & 0xFF),
                         static_cast<uint8>((RGB >> 8) & 0xFF),
                         static_cast<uint8>(RGB & 0xFF));
        FLinearColor Linear = FLinearColor::FromSRGBColor(Cor);
        Linear.A = Alpha;
        return Linear;
    }

    // --- Paleta central (metais) ---
    inline FLinearColor LataoPolido(const float A = 1.f)    { return Hex(0xc9a227, A); }
    inline FLinearColor LataoEscovado(const float A = 1.f)  { return Hex(0xa8842c, A); }
    inline FLinearColor Cobre(const float A = 1.f)          { return Hex(0xb87333, A); }
    inline FLinearColor CobreOxidado(const float A = 1.f)   { return Hex(0x8a4a2a, A); }
    inline FLinearColor FerroForjado(const float A = 1.f)   { return Hex(0x3a3a42, A); }
    inline FLinearColor AcoEscuro(const float A = 1.f)      { return Hex(0x2a2a30, A); }
    inline FLinearColor Rebites(const float A = 1.f)        { return Hex(0xd8c890, A); }
    inline FLinearColor Ambar(const float A = 1.f)          { return Hex(0xffb040, A); }
    inline FLinearColor Vapor(const float A = 1.f)          { return Hex(0xc8c0a8, A); }

    // --- Ceu dusk e cidade ---
    inline FLinearColor CeuTopo(const float A = 1.f)        { return Hex(0x1a2a5e, A); }
    inline FLinearColor HorizonteForja(const float A = 1.f) { return Hex(0xc86a30, A); }
    inline FLinearColor Brasa(const float A = 1.f)          { return Hex(0xff9040, A); }
    inline FLinearColor LuzDeRua(const float A = 1.f)       { return Hex(0xffcf70, A); }
    inline FLinearColor Silhueta(const float A = 1.f)       { return Hex(0x1a1a2e, A); }

    // --- Energia (Aether) e gelo ---
    inline FLinearColor Ciano(const float A = 1.f)          { return Hex(0x40e0ff, A); }
    inline FLinearColor Aether(const float A = 1.f)         { return Hex(0x40ffc0, A); }

    // --- Papeis na interface (quem usa o que) ---
    inline FLinearColor FundoPainel()      { return Silhueta(0.94f); }
    inline FLinearColor FundoCampo()       { return Silhueta(0.85f); }
    inline FLinearColor BordaPainel()      { return LataoEscovado(0.95f); }
    inline FLinearColor BordaCampo()       { return LataoEscovado(0.55f); }
    inline FLinearColor TextoTitulo()      { return Ambar(); }
    inline FLinearColor TextoCorpo()       { return Rebites(0.92f); }
    inline FLinearColor TextoFraco()       { return Rebites(0.55f); }
    inline FLinearColor TextoDoBotao()     { return Silhueta(); }
    inline FLinearColor BotaoPrincipal()   { return Ambar(); }
    inline FLinearColor BotaoPrincipalHover() { return LuzDeRua(); }
    inline FLinearColor BotaoSecundario()  { return FerroForjado(0.95f); }
    inline FLinearColor BotaoSecundarioHover() { return AcoEscuro(1.f); }
    inline FLinearColor BotaoTerciario()   { return CobreOxidado(0.35f); }
    inline FLinearColor BotaoTerciarioHover() { return Cobre(0.55f); }
    inline FLinearColor LinhaDeOpcao()     { return AcoEscuro(0.85f); }
    inline FLinearColor LinhaDeOpcaoHover() { return FerroForjado(0.95f); }
}
