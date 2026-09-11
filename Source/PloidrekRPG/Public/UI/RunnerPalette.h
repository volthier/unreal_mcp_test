#pragma once

#include "CoreMinimal.h"

/**
 * Cores da interface — paleta CYBERPUNK NEON, definida nos guias de conceito do autor:
 * Art/Tela_login/TL_guia_conceitual_part1.png e part2.png.
 *
 * Regra: nenhum hexadecimal solto no codigo dos widgets — se a cor nao existe aqui e porque
 * nao esta na paleta do jogo, e ai a discussao e de arte, nao de codigo.
 *
 * Os NOMES das funcoes ficaram os mesmos de proposito: a interface inteira muda de uma vez, e nenhum
 * widget precisou ser tocado. Os hexadecimais sao exatamente os do guia:
 *   #00E5FF azul neon · #FF2E88 rosa neon · #A855F7 violeta · #1A1A2E fundo escuro
 *   #0F172A fundo de UI · #94A3B8 texto secundario · #FF6B00 perigo · #FFFFFF branco
 *
 * Estilo: interface de ficcao cientifica inspirada em Mega Man X8 — vidro escuro, neon violeta e ciano,
 * cidade em ruinas ao fundo, nuvem de nano-robos, hologramas.
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

    // --- Paleta central do guia (os nomes antigos ficam como apelido, para nao quebrar nenhum widget) ---
    inline FLinearColor AzulNeon(const float A = 1.f)       { return Hex(0x00e5ff, A); }   // #00E5FF
    inline FLinearColor RosaNeon(const float A = 1.f)       { return Hex(0xff2e88, A); }   // #FF2E88
    inline FLinearColor Violeta(const float A = 1.f)        { return Hex(0xa855f7, A); }   // #A855F7
    inline FLinearColor FundoEscuro(const float A = 1.f)    { return Hex(0x1a1a2e, A); }   // #1A1A2E
    inline FLinearColor FundoUI(const float A = 1.f)        { return Hex(0x0f172a, A); }   // #0F172A
    inline FLinearColor CinzaTexto(const float A = 1.f)     { return Hex(0x94a3b8, A); }   // #94A3B8
    inline FLinearColor Perigo(const float A = 1.f)         { return Hex(0xff6b00, A); }   // #FF6B00
    inline FLinearColor Branco(const float A = 1.f)         { return Hex(0xffffff, A); }   // #FFFFFF

    // Metais do guia: o titulo e metalico claro, sem latao quente.
    inline FLinearColor LataoPolido(const float A = 1.f)    { return Hex(0xe8f6ff, A); }   // titulo metalico frio
    inline FLinearColor LataoEscovado(const float A = 1.f)  { return Hex(0x94a3b8, A); }
    inline FLinearColor Cobre(const float A = 1.f)          { return Hex(0xa855f7, A); }
    inline FLinearColor CobreOxidado(const float A = 1.f)   { return Hex(0x6d28d9, A); }
    inline FLinearColor FerroForjado(const float A = 1.f)   { return Hex(0x1e293b, A); }
    inline FLinearColor AcoEscuro(const float A = 1.f)      { return Hex(0x0f172a, A); }
    inline FLinearColor Rebites(const float A = 1.f)        { return Hex(0x00e5ff, A); }
    inline FLinearColor Ambar(const float A = 1.f)          { return Hex(0xff2e88, A); }   // o destaque agora e rosa neon
    inline FLinearColor Vapor(const float A = 1.f)          { return Hex(0x94a3b8, A); }

    // --- Ceu e cidade (a tempestade de nano-robos) ---
    inline FLinearColor CeuTopo(const float A = 1.f)        { return Hex(0x1a1a2e, A); }
    inline FLinearColor HorizonteForja(const float A = 1.f) { return Hex(0xa855f7, A); }
    inline FLinearColor Brasa(const float A = 1.f)          { return Hex(0xff2e88, A); }
    inline FLinearColor LuzDeRua(const float A = 1.f)       { return Hex(0x00e5ff, A); }
    inline FLinearColor Silhueta(const float A = 1.f)       { return Hex(0x0f172a, A); }

    // --- Energia (Aether) e gelo ---
    inline FLinearColor Ciano(const float A = 1.f)          { return Hex(0x00e5ff, A); }
    inline FLinearColor Aether(const float A = 1.f)         { return Hex(0x40ffc0, A); }

    // --- Papeis na interface (quem usa o que) ---
    inline FLinearColor FundoPainel()      { return FundoUI(0.88f); }
    inline FLinearColor FundoCampo()       { return Silhueta(0.72f); }
    inline FLinearColor BordaPainel()      { return AzulNeon(0.85f); }
    inline FLinearColor BordaCampo()       { return Violeta(0.55f); }
    inline FLinearColor TextoTitulo()      { return Branco(); }
    inline FLinearColor TextoCorpo()       { return CinzaTexto(0.95f); }
    inline FLinearColor TextoFraco()       { return CinzaTexto(0.60f); }
    inline FLinearColor TextoDoBotao()     { return Branco(); }
    // Botoes do guia: o principal e violeta cheio (borda ciano, no EstilizarBotao), o secundario e vidro
    // (escuro translucido, para ler como painel e nao como bloco) e o terciario e o chip escuro.
    inline FLinearColor BotaoPrincipal()   { return Violeta(0.90f); }
    inline FLinearColor BotaoPrincipalHover() { return RosaNeon(0.95f); }
    inline FLinearColor BotaoSecundario()  { return FundoUI(0.55f); }
    inline FLinearColor BotaoSecundarioHover() { return Violeta(0.45f); }
    inline FLinearColor BotaoTerciario()   { return FerroForjado(0.90f); }
    inline FLinearColor BotaoTerciarioHover() { return Violeta(0.35f); }
    inline FLinearColor LinhaDeOpcao()     { return FerroForjado(0.85f); }
    inline FLinearColor LinhaDeOpcaoHover() { return Violeta(0.35f); }
}