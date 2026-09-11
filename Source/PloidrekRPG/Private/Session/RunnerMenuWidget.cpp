#include "Session/RunnerMenuWidget.h"

namespace
{
    /**
     * Linha de campo com o icone do kit a esquerda (usuario, senha), como no alvo do autor.
     *
     * Declarada aqui em cima porque o corpo dela vive la embaixo, junto do fundo - e o RebuildLayout, que e
     * quem a usa, vem antes no arquivo. Sem esta declaracao o compilador nao a enxerga.
     */
    UWidget* LinhaDeCampoComIcone(UWidget* Campo, UTexture2D* Icone);
}

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Spacer.h"
#include "Components/Widget.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerCharacterFactory.h"
#include "Data/RunnerClassData.h"
#include "Data/RunnerGameSettings.h"
#include "Data/RunnerRules.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NiagaraSystem.h"
#include "Engine/World.h"
#include "UI/RunnerPreviewActor.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "UI/RunnerIconFactory.h"
#include "UI/RunnerPalette.h"
#include "UI/RunnerVeilWidget.h"
#include "Data/RunnerClassData.h"
#include "Session/RunnerSession.h"
#include "Session/RunnerSessionSubsystem.h"

namespace
{
    // Cores: todas vindas de RunnerPalette, que segue Docs/SteampunkPalette.md. Nenhum hexadecimal
    // solto aqui — se a cor nao esta na paleta, a discussao e de arte.
    constexpr float RaioDoPainel = 12.f;
    constexpr float RaioDoBotao = 6.f;
    constexpr float RaioDoCampo = 5.f;

    /** Caixa arredondada (com contorno opcional): o acabamento de latao sobre aco escuro. */
    FSlateBrush Caixa(const FLinearColor& Preenchimento, const float Raio,
                      const FLinearColor& Contorno = FLinearColor::Transparent, const float Espessura = 0.f)
    {
        if (Contorno.A <= 0.f || Espessura <= 0.f)
        {
            return FSlateRoundedBoxBrush(Preenchimento, Raio);
        }
        return FSlateRoundedBoxBrush(Preenchimento, Raio, Contorno, Espessura);
    }

    /**
     * A FONTE DO GUIA (Art/Tela_login): Orbitron nos titulos e Exo 2 na interface.
     *
     * Os TTF vivem em Content/Slate/Fonts, que e o caminho que o Slate resolve SOZINHO, sem asset de fonte.
     * Isso nao e preguica: o import de fonte exige a aplicacao Slate, e em commandlet (-nullrhi) ele aborta com
     * "Assertion failed: CurrentApplication.IsValid()" - testado e visto no log. Pelo caminho do Slate a fonte
     * funciona sem depender de import.
     *
     * Titulo e tudo de 20 pt para cima; abaixo disso e interface. As duas fontes sao variaveis, entao o peso e a
     * instancia padrao (Regular) - para peso especifico o caminho seria asset de fonte, que nao esta disponivel.
     */
    FSlateFontInfo FonteDoGuia(const float Tamanho)
    {
        const TCHAR* NomeDaFonte = (Tamanho >= 20.f) ? TEXT("Orbitron") : TEXT("Exo2");
        return FSlateFontInfo(FString(NomeDaFonte), Tamanho);
    }

    UTextBlock* CriarTexto(UWidgetTree* Arvore, const FString& Conteudo, const float Tamanho,
                           const FLinearColor& Cor, const FName Tipo = TEXT("Bold"))
    {
        // O Tipo (Regular/Bold) veio da fonte do engine e nao existe no caminho de TTF do Slate: a fonte do
        // guia tem um peso so. Fica marcado como nao usado de proposito, para o -Werror nao acusar.
        (void)Tipo;
        UTextBlock* Texto = Arvore->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Texto->SetText(FText::FromString(Conteudo));
        Texto->SetColorAndOpacity(FSlateColor(Cor));
        Texto->SetAutoWrapText(true);
        Texto->SetFont(FonteDoGuia(Tamanho));
        return Texto;
    }

    /** Filete de latao: separa o cabecalho do corpo, como uma placa de metal. */
    UWidget* CriarFilete(UWidgetTree* Arvore, const FLinearColor& Cor, const float Altura)
    {
        USpacer* Espaco = Arvore->ConstructWidget<USpacer>(USpacer::StaticClass());
        Espaco->SetSize(FVector2D(0.f, Altura));

        UBorder* Filete = Arvore->ConstructWidget<UBorder>(UBorder::StaticClass());
        Filete->SetBrush(Caixa(Cor, Altura * 0.5f));
        Filete->SetPadding(FMargin(0.f));
        Filete->SetContent(Espaco);
        return Filete;
    }

    /** Botao no estilo do jogo: caixa arredondada com contorno, hover que acende. */
    void EstilizarBotao(UButton* Botao, const FLinearColor& Normal, const FLinearColor& Hover,
                        const FLinearColor& Contorno, const float Espessura)
    {
        if (!Botao)
        {
            return;
        }
        FButtonStyle Estilo;
        Estilo.SetNormal(Caixa(Normal, RaioDoBotao, Contorno, Espessura));
        Estilo.SetHovered(Caixa(Hover, RaioDoBotao, Contorno, Espessura));
        Estilo.SetPressed(Caixa(Normal * 0.75f, RaioDoBotao, Contorno, Espessura));
        Estilo.SetDisabled(Caixa(Normal * 0.35f, RaioDoBotao));
        // Padding do alvo (Art/Tela_login): os botoes de la sao ALTOS e folgados, nao fitas finas. 15 px de
        // folga vertical com a fonte de 16 pt da ~46 px de altura, que e a proporcao do guia.
        Estilo.SetNormalPadding(FMargin(16.f, 15.f));
        Estilo.SetPressedPadding(FMargin(16.f, 16.f, 16.f, 14.f));
        Botao->SetStyle(Estilo);
        Botao->SetBackgroundColor(FLinearColor::White);
    }

    /** Campo de texto: fundo escuro, contorno de latao que acende no foco. */
    void EstilizarCampo(UEditableTextBox* Campo)
    {
        if (!Campo)
        {
            return;
        }
        FEditableTextBoxStyle Estilo;
        Estilo.SetBackgroundImageNormal(Caixa(RunnerPalette::FundoCampo(), RaioDoCampo, RunnerPalette::BordaCampo(), 1.f));
        Estilo.SetBackgroundImageHovered(Caixa(RunnerPalette::FundoCampo(), RaioDoCampo, RunnerPalette::LataoPolido(0.8f), 1.f));
        Estilo.SetBackgroundImageFocused(Caixa(RunnerPalette::Silhueta(0.95f), RaioDoCampo, RunnerPalette::Ambar(0.9f), 1.5f));
        Estilo.SetBackgroundImageReadOnly(Caixa(RunnerPalette::AcoEscuro(0.8f), RaioDoCampo, RunnerPalette::BordaCampo(), 1.f));
        Estilo.SetForegroundColor(RunnerPalette::TextoCorpo());
        Estilo.SetFocusedForegroundColor(RunnerPalette::Rebites());
        Estilo.SetPadding(FMargin(10.f, 7.f));
        Campo->SetWidgetStyle(Estilo);
    }

    /** Nome do atributo como o jogador le (sem acento no codigo, como o resto do projeto). */
    FString NomeDoAtributo(ERunnerAttribute Atributo)
    {
        switch (Atributo)
        {
            case ERunnerAttribute::Strength:     return TEXT("Forca");
            case ERunnerAttribute::Dexterity:    return TEXT("Destreza");
            case ERunnerAttribute::Constitution: return TEXT("Constituicao");
            case ERunnerAttribute::Intelligence: return TEXT("Inteligencia");
            case ERunnerAttribute::Wisdom:       return TEXT("Sabedoria");
            case ERunnerAttribute::Charisma:     return TEXT("Carisma");
            default:                             return TEXT("?");
        }
    }

    /** Os seis, na ordem em que a ficha os le. */
    TArray<ERunnerAttribute> TodosOsAtributos()
    {
        return { ERunnerAttribute::Strength, ERunnerAttribute::Dexterity, ERunnerAttribute::Constitution,
                 ERunnerAttribute::Intelligence, ERunnerAttribute::Wisdom, ERunnerAttribute::Charisma };
    }

    /** Corpo em uma palavra (o tipo vem dos dados do chassi). */
    FString NomeDoCorpo(ERunnerBodyType Corpo)
    {
        switch (Corpo)
        {
            case ERunnerBodyType::Slender:  return TEXT("esguio");
            case ERunnerBodyType::Stocky:   return TEXT("truncoso");
            case ERunnerBodyType::Fragile:  return TEXT("fragil");
            case ERunnerBodyType::Unstable: return TEXT("instavel");
            default:                        return TEXT("corpo");
        }
    }

    /** Cor do icone da classe, por papel: cada papel tem a sua luz na paleta. */
    FLinearColor CorDoPapel(const ERunnerRole Papel)
    {
        switch (Papel)
        {
            case ERunnerRole::Tank:    return RunnerPalette::LataoPolido();
            case ERunnerRole::DPS:     return RunnerPalette::Brasa();
            case ERunnerRole::Support: return RunnerPalette::Aether();
            case ERunnerRole::Control: return RunnerPalette::Ciano();
            case ERunnerRole::Hybrid:  return RunnerPalette::Cobre();
            default:                   return RunnerPalette::TextoCorpo();
        }
    }

    /** Cada filho do VerticalBox com um respiro embaixo. */
    void Adicionar(UVerticalBox* Caixa, UWidget* Filho, const float EspacoAbaixo = 8.f)
    {
        if (!Caixa || !Filho)
        {
            return;
        }
        if (UVerticalBoxSlot* Slot = Cast<UVerticalBoxSlot>(Caixa->AddChild(Filho)))
        {
            Slot->SetPadding(FMargin(0.f, 0.f, 0.f, EspacoAbaixo));
        }
    }
}

void URunnerOptionButton::HandleClicked()
{
    if (!Owner)
    {
        return;
    }

    // Apagar nunca acontece direto: abre a janela de confirmacao.
    if (Action == ERunnerOptionAction::Excluir)
    {
        Owner->AbrirConfirmacao(ERunnerConfirmAction::ExcluirPersonagem, FString(), CharacterId);
        return;
    }

    // Na lista de Runners, clicar MARCA o personagem (o jogador ve os detalhes antes de entrar).
    if (Action == ERunnerOptionAction::Selecionar)
    {
        Owner->SelecionarPersonagem(CharacterId, false);
        return;
    }

    // O tipo do id decide se ele e chassi ou classe conforme o passo atual.
    Owner->SelectChassis(OptionId);
}

void URunnerOptionButton::HandleHovered()
{
    // Passar o mouse ja mostra na vitrine: e assim que o jogador compara chassis sem clicar.
    if (Owner)
    {
        Owner->PrevisualizarOpcao(OptionId, Action, CharacterId);
    }
}

URunnerSession* URunnerMenuWidget::GetSession() const
{
    if (const APlayerController* PC = GetOwningPlayer())
    {
        if (UGameInstance* GameInstance = PC->GetGameInstance())
        {
            if (const URunnerSessionSubsystem* Subsystem = GameInstance->GetSubsystem<URunnerSessionSubsystem>())
            {
                return Subsystem->GetRunnerSession();
            }
        }
    }
    return nullptr;
}

ERunnerMenuStep RunnerDestinoDoBotao(const ERunnerMenuButton Botao, const ERunnerMenuStep Passo, const bool bTemPersonagens)
{
    switch (Botao)
    {
        case ERunnerMenuButton::Principal:
            // Na criacao o botao principal leva para a aba da classe (e de la cria).
            return Passo == ERunnerMenuStep::Chassis ? ERunnerMenuStep::Class : Passo;

        case ERunnerMenuButton::Secundario:
            switch (Passo)
            {
                case ERunnerMenuStep::CreateAccount:   return ERunnerMenuStep::Login;
                case ERunnerMenuStep::CharacterSelect: return ERunnerMenuStep::Chassis;   // "Criar novo"
                case ERunnerMenuStep::Chassis:         return bTemPersonagens ? ERunnerMenuStep::CharacterSelect
                                                                             : ERunnerMenuStep::Login;
                case ERunnerMenuStep::Class:           return ERunnerMenuStep::Chassis;   // "Voltar ao chassi"
                default:                               return Passo;
            }

        case ERunnerMenuButton::Terceiro:
            // "Criar conta nova (local)" no login; "Excluir" na lista (que abre a confirmacao).
            return Passo == ERunnerMenuStep::Login ? ERunnerMenuStep::CreateAccount : Passo;

        case ERunnerMenuButton::Quarto:
            // "Voltar" na lista de Runners = sair da conta.
            return Passo == ERunnerMenuStep::CharacterSelect ? ERunnerMenuStep::Login : Passo;
    }
    return Passo;
}

void URunnerTabButton::HandleClicked()
{
    if (Owner)
    {
        Owner->TrocarAba(Destino);
    }
}

void URunnerMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
    WidgetTree->RootWidget = Canvas;

    // Fundo primeiro: quem entra antes desenha atras, entao a arte de cenario fica atras do painel.
    CriarFundo(Canvas);

    // Painel: placa de aco escuro com contorno de latao escovado (paleta canonica).
    UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Panel"));
    Panel->SetBrush(Caixa(RunnerPalette::FundoPainel(), RaioDoPainel, RunnerPalette::BordaPainel(), 1.5f));

    // A MOLDURA do kit visual, quando configurada: a textura entra como pincel BOX (9 fatias), entao os
    // cantos com rebite ficam do tamanho certo e so as bordas esticam - nada de moldura achatada.
    if (const URunnerGameSettings* AjustesDaMoldura = GetDefault<URunnerGameSettings>())
    {
        if (!AjustesDaMoldura->TexturaDaMoldura.IsNull())
        {
            if (UTexture2D* Moldura = AjustesDaMoldura->TexturaDaMoldura.LoadSynchronous())
            {
                FSlateBrush PincelDaMoldura;
                PincelDaMoldura.SetResourceObject(Moldura);
                PincelDaMoldura.ImageSize = FVector2D(Moldura->GetSizeX(), Moldura->GetSizeY());
                PincelDaMoldura.DrawAs = ESlateBrushDrawType::Box;
                PincelDaMoldura.Margin = FMargin(0.14f);
                PincelDaMoldura.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
                Panel->SetBrush(PincelDaMoldura);
            }
        }
    }
    Panel->SetPadding(FMargin(26.f, 22.f));
    UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(Panel);
    PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));
    PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    PanelSlot->SetSize(FVector2D(820.f, 760.f));
    PanelSlotDoPainel = PanelSlot;

    RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
    Panel->SetContent(RootBox);

    // O veu entra por ULTIMO no canvas: aqui quem entra depois desenha por cima, e e isso que faz
    // a neblina passar por cima dos botoes em vez de ficar so atras deles.
    Veil = CreateWidget<URunnerVeilWidget>(GetOwningPlayer(), URunnerVeilWidget::StaticClass());
    if (Veil)
    {
        // Calibragem vem do Project Settings > Game > Runner > Veu (nada fixo no codigo).
        if (const URunnerGameSettings* AjustesDoVeu = GetDefault<URunnerGameSettings>())
        {
            Veil->Intensidade = AjustesDoVeu->bVeuLigado ? AjustesDoVeu->IntensidadeDoVeu : 0.f;
            Veil->RaioDaBolaDoMouse = AjustesDoVeu->RaioDaBolaDoMouse;
            Veil->Velocidade = AjustesDoVeu->VelocidadeDoVeu;
        }

        UCanvasPanelSlot* VeilSlot = Canvas->AddChildToCanvas(Veil);
        VeilSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
        VeilSlot->SetOffsets(FMargin(0.f));
    }

    // O EOS responde assincrono: a tela reage ao fim do login por este delegate.
    if (URunnerSession* Sessao = GetSession())
    {
        Sessao->OnLoginComplete.AddDynamic(this, &URunnerMenuWidget::HandleLoginComplete);
    }

    RebuildLayout();
}

void URunnerMenuWidget::HandleLembrarMe(bool bMarcado)
{
    bLembrarMe = bMarcado;
    // Guarda (ou esquece) o que esta digitado AGORA: e o que o autor espera de "lembrar de mim".
    if (bMarcado && AccountBox)
    {
        EmailLembrado = AccountBox->GetText().ToString();
    }
    else if (!bMarcado)
    {
        EmailLembrado.Reset();
    }
    SetStatus(bMarcado
        ? TEXT("Lembrar de mim: marcado - o e-mail volta preenchido na proxima vez.")
        : TEXT("Lembrar de mim: desmarcado - o e-mail nao sera lembrado."));
}

void URunnerMenuWidget::SetStatus(const FString& Message)
{
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(Message));
    }
}

void URunnerMenuWidget::RebuildLayout()
{
    if (!RootBox)
    {
        return;
    }

    RootBox->ClearChildren();
    OptionBindings.Reset();

    // Tudo que o veu vai rondar neste passo (botoes e campos).
    TArray<UWidget*> AlvosDoVeu;

    StatusText = nullptr;
    AccountBox = nullptr;
    PasswordBox = nullptr;
    UserNameBox = nullptr;
    ConfirmBox = nullptr;
    NameBox = nullptr;

    URunnerSession* Session = GetSession();

    // O painel acompanha a tela: tamanho fixo cortava o botao de criar em tela mais baixa.
    FVector2D Tela(1920.f, 1080.f);
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(Tela);
    }
    // PROPORCAO DO PAINEL, conforme o guia (Art/Tela_login): o painel do login e ESTREITO e vertical, com a
    // cidade visivel de sobra dos dois lados. Antes ele era 68 por cento da largura, o que esticava os campos
    // por uma faixa enorme e deixava um vazio embaixo - era isso que o autor viu como "longe do guia".
    // Nas telas de escolha (chassi/classe/runners) o painel continua largo, porque ali ha tres colunas.
    const bool bTelaDeEntrada = (CurrentStep == ERunnerMenuStep::Login || CurrentStep == ERunnerMenuStep::CreateAccount);
    // Na entrada, o painel acompanha o CONTEUDO: com 86 por cento da tela sobrava um terco vazio embaixo, e o
    // painel parecia maior que a tela. 74 por cento deixa o painel justo, com a cidade aparecendo mais.
    const float AlturaDoPainel = bTelaDeEntrada
        ? FMath::Clamp(Tela.Y * 0.74f, 520.f, 820.f)
        : FMath::Clamp(Tela.Y * 0.92f, 560.f, 1040.f);
    if (PanelSlotDoPainel)
    {
        PanelSlotDoPainel->SetSize(FVector2D(
            bTelaDeEntrada ? FMath::Clamp(Tela.X * 0.32f, 480.f, 640.f)
                           : FMath::Clamp(Tela.X * 0.68f, 740.f, 1180.f),
            AlturaDoPainel));
    }

    // A vitrine e a lista dividem a altura do painel: em tela baixa tudo encolhe, nada some.
    AlturaDaVitrine = FMath::Clamp(AlturaDoPainel * 0.40f, 168.f, 290.f);
    AlturaDaLista = FMath::Clamp(AlturaDoPainel * 0.24f, 96.f, 185.f);

    // O EMBLEMA SAIU DE VEZ, e nao so da entrada. Era o selo do cristal dentro do anel de latao, do primeiro
    // kit, e o autor foi direto: "e ridiculo esse asset la". Marca do jogo agora e SO o logotipo AETHER FORGE
    // - uma marca, um lugar. A textura continua no projeto (ui_emblema) caso o autor queira reusa-la em outro
    // contexto, mas nenhuma tela a desenha.
    // BLOCO DO TITULO, conforme o guia (Art/Tela_login/TL_Tela_Login_art_final.png):
    //   AETHER FORGE  - titulo grande, metalico frio;
    //   P R O T O C O L   Z E R O - subtitulo espacado em violeta;
    //   mais que jogo, um novo amanha - a frase do guia, discreta.
    // O espacamento entre letras do subtitulo vem de espacos no proprio texto: a fonte do engine nao tem
    // eixo de tracking, e inventar um asset de fonte aqui seria excecao sem necessidade (ver EXCECOES.md).
    if (CurrentStep == ERunnerMenuStep::Login || CurrentStep == ERunnerMenuStep::CreateAccount)
    {
        // SEM SetRenderScale aqui: o UMG mede o texto pelo tamanho da fonte e ignora a escala de render,
        // entao um titulo escalado TRANSBORDA em cima do que vem depois. Foi o que aconteceu na primeira
        // versao - o AETHER FORGE caia por cima das abas. O peso vem da fonte, nao de escala.
        // O TITULO e o LOGOTIPO do kit (AETHER FORGE com o X em degrade e PROTOCOL ZERO espacado), como no alvo
        // do autor. Se a textura nao estiver no projeto, cai no texto - a tela nunca fica sem titulo.
        // Ajustes locais: o Ajustes do resto da funcao e declarado mais abaixo, e aqui em cima ainda nao existe.
        const URunnerGameSettings* AjustesDoTitulo = GetDefault<URunnerGameSettings>();
        UTexture2D* Logotipo = AjustesDoTitulo ? AjustesDoTitulo->LogoDoTitulo.LoadSynchronous() : nullptr;
        if (Logotipo)
        {
            // Largura E altura fixadas num SizeBox, com a PROPORCAO da textura: com so a largura, a caixa
            // vertical esticava a imagem e o PROTOCOL ZERO subia por cima do titulo (foi o que o autor viu).
            // 300 px de largura: o painel da entrada mede 32 por cento da tela, e numa tela de 1200 px ele fica
            // com ~383 px de largura util (~330 livres depois do padding). Com 400 o logotipo ESTOURAVA o painel
            // - foi o que o autor viu na foto, com o conteudo caindo para fora.
            const float LarguraDoLogo = 300.f;
            const float ProporcaoDoLogo = (float)Logotipo->GetSizeY() / FMath::Max(1.f, (float)Logotipo->GetSizeX());
            UImage* ImagemDoTitulo = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("LogoDoJogo"));
            ImagemDoTitulo->SetBrushFromTexture(Logotipo, false);
            USizeBox* CaixaDoLogo = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
            CaixaDoLogo->SetWidthOverride(LarguraDoLogo);
            CaixaDoLogo->SetHeightOverride(LarguraDoLogo * ProporcaoDoLogo);
            CaixaDoLogo->AddChild(ImagemDoTitulo);
            Adicionar(RootBox, CaixaDoLogo, 0.f);
        }
        else
        {
            Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("AETHER FORGE"), 32.f, RunnerPalette::Branco()), 0.f);
            Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("P R O T O C O L   Z E R O"), 13.f, RunnerPalette::Violeta()), 6.f);
        }
        Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("mais que jogo, um novo amanha"), 10.f, RunnerPalette::TextoFraco()), 8.f);
    }
    else
    {
        Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("AETHER FORGE"), 26.f, RunnerPalette::LataoPolido()), 2.f);
    }

    FString Passo;
    switch (CurrentStep)
    {
        // Abas do guia: a ativa em branco, a outra apagada. Nao sao clicaveis porque os botoes LOGIN e
        // CADASTRO logo abaixo ja fazem essa troca - duas formas de trocar de aba na mesma tela confundem.
        case ERunnerMenuStep::Login:           Passo = TEXT("LOGIN          cadastro"); break;
        case ERunnerMenuStep::CreateAccount:   Passo = TEXT("login          CADASTRO"); break;
        case ERunnerMenuStep::CharacterSelect: Passo = TEXT("SEUS RUNNERS"); break;
        case ERunnerMenuStep::Chassis:         Passo = TEXT("ESCOLHA O CHASSI"); break;
        case ERunnerMenuStep::Class:           Passo = TEXT("ESCOLHA A CLASSE"); break;
        case ERunnerMenuStep::Confirm:         Passo = TEXT("CONFIRMAR"); break;
    }
    Adicionar(RootBox, CriarTexto(WidgetTree, Passo, 11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 10.f);
    Adicionar(RootBox, CriarFilete(WidgetTree, RunnerPalette::LataoEscovado(0.75f), 1.5f), 14.f);

    // Tela de escolha: criacao (abas chassi/classe) ou os Runners que ja existem.
    if (CurrentStep == ERunnerMenuStep::Chassis || CurrentStep == ERunnerMenuStep::Class)
    {
        ConstruirTelaDeCriacao(AlvosDoVeu);
    }
    else if (CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        ConstruirTelaDeRunners(AlvosDoVeu);
    }

    // Janela de confirmacao (criar personagem / apagar personagem).
    if (CurrentStep == ERunnerMenuStep::Confirm)
    {
        UBorder* Dialogo = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
        Dialogo->SetBrush(Caixa(RunnerPalette::AcoEscuro(0.9f), 8.f, RunnerPalette::LataoPolido(0.85f), 1.5f));
        Dialogo->SetPadding(FMargin(18.f, 16.f));
        Dialogo->SetContent(CriarTexto(WidgetTree, MensagemDaConfirmacao, 14.f, RunnerPalette::TextoCorpo(), TEXT("Regular")));
        Adicionar(RootBox, Dialogo, 16.f);
    }

    // Com EOS ativo, os campos do login sao os do Dev Auth Tool: campo 1 = onde o tool esta
    // ouvindo, campo 2 = o NOME da credencial criada nele (nao e senha). Sem EOS, e o e-mail e a
    // senha da conta local. A tela de criar conta sempre pede os dados da conta local.
    const bool bCamposDoDevAuth = CurrentStep == ERunnerMenuStep::Login && Session && Session->IsEOSAvailable();

    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    const FString HostDevAuth = (Ajustes && !Ajustes->EOSDevAuthHost.IsEmpty())
        ? Ajustes->EOSDevAuthHost : FString(TEXT("localhost:8081"));

    if (CurrentStep == ERunnerMenuStep::Login || CurrentStep == ERunnerMenuStep::CreateAccount)
    {
        if (bCamposDoDevAuth)
        {
            UTextBlock* AjudaEOS = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            AjudaEOS->SetColorAndOpacity(FSlateColor(RunnerPalette::TextoFraco()));
            AjudaEOS->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 11.f));
            AjudaEOS->SetAutoWrapText(true);
            AjudaEOS->SetText(FText::FromString(FString::Printf(
                TEXT("Dev Auth Tool: campo 1 = onde o tool esta ouvindo (%s); campo 2 = o nome que voce deu a credencial. O tool precisa estar rodando."),
                *HostDevAuth)));
            Adicionar(RootBox, AjudaEOS, 12.f);
        }
        else if (CurrentStep == ERunnerMenuStep::CreateAccount)
        {
            UTextBlock* AjudaConta = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            AjudaConta->SetColorAndOpacity(FSlateColor(RunnerPalette::TextoFraco()));
            AjudaConta->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 11.f));
            AjudaConta->SetAutoWrapText(true);
            AjudaConta->SetText(FText::FromString(TEXT("Senha: 8+ caracteres, com uma maiuscula, uma minuscula e um caractere especial (ex.: ! @ # $ %).")));
            Adicionar(RootBox, AjudaConta, 12.f);
        }

        // Campo 1: o host do Dev Auth (pre-preenchido) ou o e-mail da conta.
        AccountBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        AccountBox->SetHintText(FText::FromString(bCamposDoDevAuth ? TEXT("localhost:8081") : TEXT("E-mail ou Usuario")));
        // "Lembrar de mim": o e-mail guardado volta preenchido (so quando nao e o host do Dev Auth, que ja
        // vem da configuracao e nao e do usuario).
        if (!bCamposDoDevAuth && bLembrarMe && !EmailLembrado.IsEmpty())
        {
            AccountBox->SetText(FText::FromString(EmailLembrado));
        }
        EstilizarCampo(AccountBox);
        Adicionar(RootBox, LinhaDeCampoComIcone(AccountBox,
            Ajustes ? Ajustes->IconeDoUsuario.LoadSynchronous() : nullptr), 10.f);
        AlvosDoVeu.Add(AccountBox);

        // Nome de usuario existe so na criacao da conta local.
        if (CurrentStep == ERunnerMenuStep::CreateAccount)
        {
            UserNameBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
            UserNameBox->SetHintText(FText::FromString(TEXT("nome de usuario (3+, letras numeros _ -)")));
            EstilizarCampo(UserNameBox);
            Adicionar(RootBox, UserNameBox, 10.f);
            AlvosDoVeu.Add(UserNameBox);
        }

        PasswordBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        PasswordBox->SetHintText(FText::FromString(
            bCamposDoDevAuth ? TEXT("nome da credencial")
            : (CurrentStep == ERunnerMenuStep::CreateAccount
                ? TEXT("Senha (8+, maiuscula, minuscula, especial)") : TEXT("Senha"))));
        PasswordBox->SetIsPassword(!bCamposDoDevAuth);
        EstilizarCampo(PasswordBox);
        Adicionar(RootBox, LinhaDeCampoComIcone(PasswordBox,
            Ajustes ? Ajustes->IconeDaSenha.LoadSynchronous() : nullptr), 10.f);
        AlvosDoVeu.Add(PasswordBox);

        // A linha do guia: "Lembrar de mim" a esquerda e "Esqueci a senha?" a direita.
        if (CurrentStep == ERunnerMenuStep::Login)
        {
            UHorizontalBox* LinhaDeAjuda = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

            LembrarMeBox = WidgetTree->ConstructWidget<UCheckBox>(UCheckBox::StaticClass());
            LembrarMeBox->SetIsChecked(bLembrarMe);
            LembrarMeBox->OnCheckStateChanged.AddDynamic(this, &URunnerMenuWidget::HandleLembrarMe);
            UHorizontalBoxSlot* EspacoDaCaixa = LinhaDeAjuda->AddChildToHorizontalBox(LembrarMeBox);
            EspacoDaCaixa->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
            EspacoDaCaixa->SetVerticalAlignment(VAlign_Center);

            UTextBlock* RotuloLembrar = CriarTexto(WidgetTree, TEXT("  Lembrar de mim"), 11.f, RunnerPalette::TextoCorpo());
            UHorizontalBoxSlot* EspacoDoRotulo = LinhaDeAjuda->AddChildToHorizontalBox(RotuloLembrar);
            EspacoDoRotulo->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            EspacoDoRotulo->SetVerticalAlignment(VAlign_Center);

            // Honestidade antes de simetria: o guia pede o atalho, e o atalho diz o estado real - a
            // recuperacao de senha chega junto com a conta EOS, entao o texto diz isso em vez de fingir.
            UTextBlock* Esqueci = CriarTexto(WidgetTree, TEXT("Esqueci a senha?  (chega com a conta EOS)"), 10.f,
                                             RunnerPalette::AzulNeon(0.75f));
            UHorizontalBoxSlot* EspacoDoEsqueci = LinhaDeAjuda->AddChildToHorizontalBox(Esqueci);
            EspacoDoEsqueci->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
            EspacoDoEsqueci->SetVerticalAlignment(VAlign_Center);

            Adicionar(RootBox, LinhaDeAjuda, 10.f);
        }

        if (CurrentStep == ERunnerMenuStep::CreateAccount)
        {
            ConfirmBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
            ConfirmBox->SetHintText(FText::FromString(TEXT("confirmar senha")));
            ConfirmBox->SetIsPassword(true);
            EstilizarCampo(ConfirmBox);
            Adicionar(RootBox, ConfirmBox, 10.f);
            AlvosDoVeu.Add(ConfirmBox);
        }

        // Com EOS o campo 1 ja vem preenchido: o jogador digita so a credencial.
        if (bCamposDoDevAuth && AccountBox)
        {
            AccountBox->SetText(FText::FromString(HostDevAuth));
        }
    }



    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    StatusText->SetColorAndOpacity(FSlateColor(RunnerPalette::Vapor(0.85f)));
    StatusText->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 12.f));
    StatusText->SetAutoWrapText(true);
    Adicionar(RootBox, StatusText, 12.f);

    const bool bEOS = Session && Session->IsEOSAvailable();

    // Botao principal: ambar cheio, texto escuro — e a acao que o passo pede.
    UButton* Primary = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    EstilizarBotao(Primary, RunnerPalette::BotaoPrincipal(), RunnerPalette::BotaoPrincipalHover(),
                   RunnerPalette::AzulNeon(1.f), 2.f);
    UTextBlock* PrimaryText = CriarTexto(WidgetTree, FString(), 18.f, RunnerPalette::TextoDoBotao());
    PrimaryText->SetAutoWrapText(false);
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            PrimaryText->SetText(FText::FromString(bEOS ? TEXT("ENTRAR  (conta EOS)") : TEXT("ENTRAR")));
            break;
        case ERunnerMenuStep::CreateAccount:     PrimaryText->SetText(FText::FromString(TEXT("Criar conta"))); break;
        case ERunnerMenuStep::CharacterSelect:   PrimaryText->SetText(FText::FromString(TEXT("Jogar"))); break;
        case ERunnerMenuStep::Class:             PrimaryText->SetText(FText::FromString(TEXT("Criar personagem"))); break;
        case ERunnerMenuStep::Chassis:           PrimaryText->SetText(FText::FromString(TEXT("Criar personagem"))); break;
        case ERunnerMenuStep::Confirm:           PrimaryText->SetText(FText::FromString(TEXT("Confirmar"))); break;
        default:                                 PrimaryText->SetText(FText::FromString(TEXT("Escolha uma opcao acima"))); break;
    }
    Primary->AddChild(PrimaryText);
    Primary->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnPrimaryClicked);
    Adicionar(RootBox, Primary, 8.f);
    AlvosDoVeu.Add(Primary);

    // Botao secundario: ferro forjado com contorno de latao.
    UButton* Secondary = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    EstilizarBotao(Secondary, RunnerPalette::BotaoSecundario(), RunnerPalette::BotaoSecundarioHover(),
                   RunnerPalette::Violeta(0.80f), 1.5f);
    UTextBlock* SecondaryText = CriarTexto(WidgetTree, FString(), 16.f, RunnerPalette::TextoCorpo());
    SecondaryText->SetAutoWrapText(false);
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            SecondaryText->SetText(FText::FromString(bEOS ? TEXT("LOGIN  (conta Epic)") : TEXT("CADASTRO")));
            break;
        case ERunnerMenuStep::CreateAccount:   SecondaryText->SetText(FText::FromString(TEXT("Voltar"))); break;
        case ERunnerMenuStep::CharacterSelect: SecondaryText->SetText(FText::FromString(TEXT("Criar novo"))); break;
        case ERunnerMenuStep::Chassis:         SecondaryText->SetText(FText::FromString(TEXT("Voltar"))); break;
        case ERunnerMenuStep::Class:           SecondaryText->SetText(FText::FromString(TEXT("Voltar ao chassi"))); break;
        case ERunnerMenuStep::Confirm:         SecondaryText->SetText(FText::FromString(TEXT("Cancelar"))); break;
    }
    Secondary->AddChild(SecondaryText);
    Secondary->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnSecondaryClicked);
    Adicionar(RootBox, Secondary, 8.f);
    AlvosDoVeu.Add(Secondary);

    // A FILEIRA SOCIAL DO GUIA ("Entrar com"): Facebook, Instagram, Apple, Xbox, Google, Epic, Steam.
    // Elas nascem DESLIGADAS de proposito: o projeto nao tem SDK nenhum dessas plataformas, e botao que
    // promete login e nao faz e pior que botao apagado. O desenho do guia fica, a mentira nao.
    if (CurrentStep == ERunnerMenuStep::Login)
    {
        Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("—  ou continue com  —"), 10.f, RunnerPalette::TextoFraco()), 14.f);

        const TCHAR* Provedores[] = { TEXT("Facebook"), TEXT("Instagram"), TEXT("Apple"),
                                      TEXT("Xbox"), TEXT("Google"), TEXT("Epic Games"), TEXT("Steam") };
        UHorizontalBox* Fileira = nullptr;
        for (int32 Indice = 0; Indice < UE_ARRAY_COUNT(Provedores); ++Indice)
        {
            if (Indice % 3 == 0)
            {
                Fileira = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
                Adicionar(RootBox, Fileira, 6.f);
            }
            UButton* Social = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
            EstilizarBotao(Social, RunnerPalette::BotaoTerciario(), RunnerPalette::BotaoTerciarioHover(),
                           RunnerPalette::AzulNeon(0.45f), 1.f);
            Social->SetIsEnabled(false);
            Social->SetToolTipText(FText::FromString(TEXT("Integracao nao conectada neste projeto.")));
            Social->AddChild(CriarTexto(WidgetTree, Provedores[Indice], 13.f, RunnerPalette::TextoCorpo()));
            UHorizontalBoxSlot* Espaco = Fileira->AddChildToHorizontalBox(Social);
            Espaco->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            Espaco->SetPadding(FMargin(3.f, 0.f));
        }
    }

    // Terceiro botao: so aparece onde ha uma terceira acao util.
    TertiaryButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    EstilizarBotao(TertiaryButton, RunnerPalette::BotaoTerciario(), RunnerPalette::BotaoTerciarioHover(),
                   RunnerPalette::AzulNeon(0.65f), 1.f);
    UTextBlock* TertiaryText = CriarTexto(WidgetTree, FString(), 13.f, RunnerPalette::Cobre(0.95f));
    TertiaryText->SetAutoWrapText(false);
    bool bMostraTerceiro = false;
    if (CurrentStep == ERunnerMenuStep::Login)
    {
        TertiaryText->SetText(FText::FromString(TEXT("Criar conta nova (local)")));
        bMostraTerceiro = true;
    }
    else if (CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        TertiaryText->SetText(FText::FromString(TEXT("Excluir")));
        bMostraTerceiro = true;
    }
    TertiaryButton->AddChild(TertiaryText);
    TertiaryButton->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnTertiaryClicked);
    TertiaryButton->SetVisibility(bMostraTerceiro ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    Adicionar(RootBox, TertiaryButton, 8.f);
    AlvosDoVeu.Add(TertiaryButton);

    // Quarto botao: Voltar (sair da conta) na lista de Runners. A referencia pede os quatro:
    // Jogar, Voltar, Criar e Excluir.
    QuartoButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    EstilizarBotao(QuartoButton, RunnerPalette::AcoEscuro(0.9f), RunnerPalette::FerroForjado(1.f),
                   RunnerPalette::BordaCampo(), 1.f);
    QuartoButton->AddChild(CriarTexto(WidgetTree, TEXT("Voltar"), 13.f, RunnerPalette::TextoCorpo()));
    QuartoButton->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnQuartoClicked);
    QuartoButton->SetVisibility(CurrentStep == ERunnerMenuStep::CharacterSelect
        ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    Adicionar(RootBox, QuartoButton, 4.f);
    AlvosDoVeu.Add(QuartoButton);

    // O veu passa a rondar o que existe neste passo (os alvos sao fracos: a tela se reconstroi).
    if (Veil)
    {
        Veil->SetTargets(AlvosDoVeu);
    }
}

void URunnerMenuWidget::GoToStep(const ERunnerMenuStep NewStep)
{
    CurrentStep = NewStep;
    RebuildLayout();

    // A VITRINE PRECISA SER AVISADA DA ETAPA NOVA. Este era o defeito que o autor viu duas vezes: indo do
    // CHASSI para a CLASSE, a vitrine e recriada pelo RebuildLayout e ficava no estado padrao - o CRISTAL -,
    // porque AtualizarVitrine so era chamado ao trocar de chassi (e o chassi nao muda nesse caminho). Agora a
    // troca de etapa sempre diz o que a vitrine mostra: cristal na aba CHASSI, corpo na aba CLASSE.
    AtualizarVitrine();

    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
        {
            URunnerSession* Sessao = GetSession();
            SetStatus(Sessao && Sessao->IsEOSAvailable()
                ? TEXT("Entre pela Epic Online Services (conta + credencial do Dev Auth Tool), ou use a conta Epic.")
                : TEXT("EOS indisponivel: entre com a conta local, ou crie uma."));
            break;
        }
        case ERunnerMenuStep::CreateAccount:
            SetStatus(TEXT("Escolha um nome de conta e uma senha (conta local, para desenvolvimento)."));
            break;
        case ERunnerMenuStep::CharacterSelect:
        {
            URunnerSession* Sessao = GetSession();
            SetStatus(Sessao && !Sessao->HasSavedCharacters()
                ? TEXT("Esta conta ainda nao tem Runner. Use \"Criar novo\".")
                : TEXT("Clique num Runner para marcar e use Jogar."));
            break;
        }
        case ERunnerMenuStep::Chassis:
            SetStatus(TEXT("O chassi e o corpo: ele define a sua vocacao. Os Clyffen estao extintos."));
            break;
        case ERunnerMenuStep::Class:
            SetStatus(FString::Printf(TEXT("Chassi: %s. Agora escolha a classe."), *PendingChassis.ToString()));
            break;
        case ERunnerMenuStep::Confirm:
            SetStatus(TEXT("Confirme para valer: e a ultima pergunta antes de acontecer."));
            break;
    }
}

void URunnerMenuWidget::SelectChassis(const FName ChassisId)
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        SetStatus(TEXT("Sessao indisponivel."));
        return;
    }

    if (CurrentStep == ERunnerMenuStep::Class)
    {
        SelectClass(ChassisId);
        return;
    }

    // Na tela de personagens a opcao e um Runner que ja existe na conta.
    if (CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        FString ErroRunner;
        if (!Session->SelectSavedCharacter(ChassisId.ToString(), ErroRunner))
        {
            SetStatus(ErroRunner);
            return;
        }

        FRunnerCharacterProfile Perfil;
        Session->GetActiveProfile(Perfil);
        SetStatus(FString::Printf(TEXT("Entrando com %s (%s)..."), *ChassisId.ToString(), *Perfil.ClassId.ToString()));
        OpenGameLevel();
        return;
    }

    FString Erro;
    if (!Session->SelectChassis(ChassisId, Erro))
    {
        SetStatus(Erro);
        return;
    }

    PendingChassis = ChassisId;
    GoToStep(ERunnerMenuStep::Class);

    // O jogador precisa saber o que esta escolhendo: a descricao e a vantagem vem do DataTable.
    FRunnerChassisData Dados;
    if (Session->GetChassisData(ChassisId, Dados))
    {
        SetStatus(FString::Printf(TEXT("%s — %s | Vantagem: %s | Agora escolha a classe."),
            *Dados.DisplayName.ToString(),
            *Dados.Description.ToString(),
            *Dados.AdvantagePrimary.ToString()));
    }
}

void URunnerMenuWidget::SelectClass(const FName ClassId)
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        SetStatus(TEXT("Sessao indisponivel."));
        return;
    }

    FString Erro;
    if (!Session->SelectClass(ClassId, Erro))
    {
        SetStatus(Erro);
        return;
    }

    PendingClass = ClassId;
    RebuildLayout();

    FRunnerClassData DadosClasse;
    if (Session->GetClassData(ClassId, DadosClasse))
    {
        SetStatus(FString::Printf(TEXT("%s — %s | Chassi %s + Classe %s · confirme para criar."),
            *DadosClasse.DisplayName.ToString(),
            *DadosClasse.Description.ToString(),
            *PendingChassis.ToString(),
            *PendingClass.ToString()));
    }
    else
    {
        SetStatus(FString::Printf(TEXT("Chassi: %s · Classe: %s — confirme para criar."),
            *PendingChassis.ToString(), *PendingClass.ToString()));
    }
}

void URunnerMenuWidget::OnPrimaryClicked()
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        SetStatus(TEXT("Sessao indisponivel."));
        return;
    }

    FString Erro;

    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
        {
            if (!AccountBox || !PasswordBox)
            {
                return;
            }

            const FString Conta = AccountBox->GetText().ToString();
            const FString Senha = PasswordBox->GetText().ToString();

            if (Session->IsEOSAvailable())
            {
                // Dev Auth do EOS: os dois campos viram o Id e o Token do Dev Auth Tool da Epic.
                SetStatus(TEXT("Entrando pela Epic Online Services..."));
                if (!Session->LoginWithEOS(TEXT("developer"), Conta, Senha, Erro))
                {
                    SetStatus(Erro);
                }
                // O resto do fluxo continua em HandleLoginComplete (o EOS responde assincrono).
                return;
            }

            if (!Session->Login(Conta, Senha, Erro))
            {
                SetStatus(Erro);
                return;
            }
            // Sucesso: quem continua o fluxo e HandleLoginComplete, disparado pelo proprio Login.
            return;
        }

        case ERunnerMenuStep::CharacterSelect:
        {
            // "Jogar": entra com o Runner que esta marcado na lista.
            if (IdPersonagemSelecionado.IsEmpty())
            {
                SetStatus(TEXT("Marque um Runner na lista para jogar."));
                return;
            }
            SelecionarPersonagem(IdPersonagemSelecionado, true);
            return;
        }

        case ERunnerMenuStep::Chassis:
        {
            // "Criar personagem" na aba do chassi: primeiro escolha a classe.
            TrocarAba(ERunnerMenuStep::Class);
            SetStatus(Session->GetSelectedChassis().IsNone()
                ? TEXT("Escolha um chassi na grade.")
                : TEXT("Chassi escolhido. Agora escolha a classe na grade."));
            return;
        }

        case ERunnerMenuStep::CreateAccount:
        {
            if (!AccountBox || !PasswordBox || !UserNameBox || !ConfirmBox)
            {
                return;
            }

            const FString Email = AccountBox->GetText().ToString();
            const FString NomeDeUsuario = UserNameBox->GetText().ToString();
            const FString Senha = PasswordBox->GetText().ToString();
            const FString ConfirmacaoDaSenha = ConfirmBox->GetText().ToString();

            // A regra mora na sessao (e o teste cobra ela); a tela so mostra o que voltou.
            if (!URunnerSession::ValidateNewAccount(Email, NomeDeUsuario, Senha, ConfirmacaoDaSenha, Erro))
            {
                SetStatus(Erro);
                return;
            }
            if (!Session->CreateAccount(Email, NomeDeUsuario, Senha, Erro))
            {
                SetStatus(Erro);
                return;
            }

            Session->Login(Email, Senha, Erro);
            GoToStep(ERunnerMenuStep::Chassis);
            break;
        }

        case ERunnerMenuStep::Class:
        {
            // O nome digitado vai para a sessao, que valida a regra antes de criar a ficha.
            if (NameBox)
            {
                Session->SetPendingCharacterName(NameBox->GetText().ToString());
            }

            // Nome errado nao merece uma janela de confirmacao: o erro aparece na hora.
            FString ErroDoNome;
            if (!URunnerSession::ValidateCharacterName(Session->GetPendingCharacterName(), ErroDoNome))
            {
                SetStatus(ErroDoNome);
                return;
            }

            // A janela mostra o resumo com os numeros que as regras ja calculam.
            FRunnerChassisData Chassi;
            FRunnerClassData Classe;
            Session->GetChassisData(Session->GetSelectedChassis(), Chassi);
            Session->GetClassData(Session->GetSelectedClass(), Classe);

            const int32 Constituicao = URunnerRules::GetFinalAttributeScore(Chassi, ERunnerAttribute::Constitution);
            const int32 Destreza = URunnerRules::GetFinalAttributeScore(Chassi, ERunnerAttribute::Dexterity);
            const int32 Vida = URunnerRules::GetLevelOneHitPoints(Classe.HitDie, Constituicao);
            const int32 ClasseDeArmadura = URunnerRules::GetArmorClass(Destreza, 0);

            AbrirConfirmacao(ERunnerConfirmAction::CriarPersonagem,
                FString::Printf(TEXT("Criar o Runner \"%s\"?\n\nCorpo: %s (%s)\nClasse: %s\nHP nivel 1: %d    CA: %d    dado de vida: d%d"),
                    *Session->GetPendingCharacterName(),
                    *Chassi.DisplayName.ToString(), *NomeDoCorpo(Chassi.Body),
                    *Classe.DisplayName.ToString(),
                    Vida, ClasseDeArmadura, Classe.HitDie),
                FString());
            return;
        }

        case ERunnerMenuStep::Confirm:
            ExecutarConfirmacao();
            return;

        default:
            SetStatus(TEXT("Escolha uma opcao da lista acima."));
            break;
    }
}

void URunnerMenuWidget::OnSecondaryClicked()
{
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
        {
            URunnerSession* Sessao = GetSession();
            if (Sessao && Sessao->IsEOSAvailable())
            {
                // Login pelo portal da conta Epic (Auth Interface, tipo accountportal).
                FString ErroPortal;
                SetStatus(TEXT("Abrindo o portal da conta Epic..."));
                if (!Sessao->LoginWithEOS(TEXT("accountportal"), TEXT(""), TEXT(""), ErroPortal))
                {
                    SetStatus(ErroPortal);
                }
                break;
            }
            GoToStep(ERunnerMenuStep::CreateAccount);
            break;
        }

        case ERunnerMenuStep::CreateAccount:
            GoToStep(ERunnerMenuStep::Login);
            break;

        case ERunnerMenuStep::Chassis:
        case ERunnerMenuStep::Class:
        case ERunnerMenuStep::CharacterSelect:
        {
            // A navegacao e a tabela pura; o handler so cuida do efeito colateral.
            URunnerSession* Sessao = GetSession();
            const ERunnerMenuStep Destino = RunnerDestinoDoBotao(ERunnerMenuButton::Secundario, CurrentStep,
                                                                 Sessao && Sessao->HasSavedCharacters());
            if (Destino == ERunnerMenuStep::Chassis)
            {
                IdPersonagemSelecionado.Reset();
            }
            GoToStep(Destino);
            break;
        }

        case ERunnerMenuStep::Confirm:
            // Cancelar nao apaga nem cria nada: volta para o passo que pediu a confirmacao.
            Confirmacao = ERunnerConfirmAction::Nada;
            GoToStep(PassoAntesDaConfirmacao);
            break;
    }
}

void URunnerMenuWidget::OnTertiaryClicked()
{
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            GoToStep(ERunnerMenuStep::CreateAccount);
            break;

        case ERunnerMenuStep::CharacterSelect:
        {
            // Excluir age no Runner marcado e passa pela confirmacao.
            if (IdPersonagemSelecionado.IsEmpty())
            {
                SetStatus(TEXT("Marque um Runner na lista para excluir."));
                break;
            }
            AbrirConfirmacao(ERunnerConfirmAction::ExcluirPersonagem, FString(), IdPersonagemSelecionado);
            break;
        }

        default:
            break;
    }
}

void URunnerMenuWidget::HandleLoginComplete(const bool bSuccess, const FString& Error)
{
    if (!bSuccess)
    {
        SetStatus(Error.IsEmpty() ? TEXT("O login falhou.") : Error);
        return;
    }

    AfterLogin();
}

void URunnerMenuWidget::AfterLogin()
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        SetStatus(TEXT("Sessao indisponivel."));
        return;
    }

    // Conta com personagens: mostra a lista. Conta nova: vai direto criar o primeiro.
    if (Session->HasSavedCharacters())
    {
        GoToStep(ERunnerMenuStep::CharacterSelect);
        SetStatus(FString::Printf(TEXT("Bem-vindo, %s. Entre com um Runner ou crie um novo."),
            *Session->GetAccountName()));
    }
    else
    {
        GoToStep(ERunnerMenuStep::Chassis);
        SetStatus(TEXT("Primeiro Runner desta conta. Escolha o chassi."));
    }
}

void URunnerMenuWidget::OpenGameLevel()
{
    // A vitrine vive no mundo do menu: sai junto com ele.
    DestruirVitrine();

    // O botao Jogar leva para o mapa do MUNDO ABERTO (configurado em Project Settings > Game > Runner).
    // Sem mapa configurado, cai no comportamento antigo: reabre o mapa atual em modo jogo.
    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    FString Mapa = Ajustes ? Ajustes->MapaDoMundo : FString();
    if (Mapa.IsEmpty())
    {
        Mapa = UGameplayStatics::GetCurrentLevelName(this, true);
    }

    UE_LOG(LogTemp, Log, TEXT("URunnerMenuWidget: entrando no mundo (%s)."), *Mapa);
    UGameplayStatics::OpenLevel(this, FName(*Mapa), true, TEXT("game=/Script/PloidrekRPG.AFGameMode"));
}

// ---------------------------------------------------------------------------
// Vitrine 3D + janela de atributos
// ---------------------------------------------------------------------------

void URunnerMenuWidget::NativeDestruct()
{
    DestruirVitrine();
    Super::NativeDestruct();
}

UWidget* URunnerMenuWidget::CriarBlocoDaVitrine()
{
    // O ator da vitrine nasce no mundo do menu (bem longe) e so existe enquanto o menu existe.
    if (!Vitrine)
    {
        if (UWorld* Mundo = GetWorld())
        {
            FActorSpawnParameters Parametros;
            Parametros.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            // Nasce ja longe e abaixo do mapa: o corpo nunca aparece no cenario do menu.
            const FTransform OndeNasce(FVector(0.f, 0.f, -50000.f));
            Vitrine = Mundo->SpawnActor<ARunnerPreviewActor>(ARunnerPreviewActor::StaticClass(), OndeNasce, Parametros);

            // Enquadramento vem do Project Settings (nada fixo no codigo).
            if (Vitrine)
            {
                if (const URunnerGameSettings* AjustesDaVitrine = GetDefault<URunnerGameSettings>())
                {
                    Vitrine->GrausPorSegundo = AjustesDaVitrine->GiroDaVitrine;
                    Vitrine->DistanciaDaCamera = AjustesDaVitrine->DistanciaDaVitrine;
                    Vitrine->CampoDeVisao = AjustesDaVitrine->CampoDeVisaoDaVitrine;
                }
            }
        }
    }

    // Moldura de monitor de oficina: aco escuro com filete de latao.
    UBorder* Moldura = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
    Moldura->SetBrush(Caixa(RunnerPalette::Silhueta(0.95f), 8.f, RunnerPalette::LataoEscovado(0.85f), 1.5f));
    Moldura->SetPadding(FMargin(6.f));

    USizeBox* TamanhoDoMonitor = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    const float AlturaDoMonitor = FMath::Max(140.f, AlturaDaVitrine - 16.f);
    TamanhoDoMonitor->SetWidthOverride(AlturaDoMonitor * 0.8f); // a captura e 4:5, como a caixa
    TamanhoDoMonitor->SetHeightOverride(AlturaDoMonitor);

    PreviewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    if (Vitrine && Vitrine->GetRenderTarget())
    {
        // O alvo de captura e UTextureRenderTarget2D (nao UTexture2D), entao o pincel e montado na mao.
        FSlateBrush PincelDoMonitor;
        PincelDoMonitor.SetResourceObject(Vitrine->GetRenderTarget());
        const float AlturaDoPincel = FMath::Max(140.f, AlturaDaVitrine - 16.f);
        PincelDoMonitor.ImageSize = FVector2D(AlturaDoPincel * 0.8f, AlturaDoPincel);
        PincelDoMonitor.DrawAs = ESlateBrushDrawType::Image;
        PreviewImage->SetBrush(PincelDoMonitor);
    }
    TamanhoDoMonitor->AddChild(PreviewImage);
    Moldura->SetContent(TamanhoDoMonitor);
    return Moldura;
}

UWidget* URunnerMenuWidget::CriarBlocoDeInfo()
{
    UBorder* Painel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
    Painel->SetBrush(Caixa(RunnerPalette::AcoEscuro(0.8f), 8.f, RunnerPalette::BordaCampo(), 1.f));
    Painel->SetPadding(FMargin(16.f, 14.f));

    UScrollBox* Rolagem = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
    InfoBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    Rolagem->AddChild(InfoBox);
    Painel->SetContent(Rolagem);
    return Painel;
}

UWidget* URunnerMenuWidget::CriarLinhaDeAtributo(ERunnerAttribute Atributo, int32 Bonus)
{
    const int32 Final = URunnerRules::GetBaseAttributeScore() + Bonus;
    const int32 Modificador = URunnerRules::GetAttributeModifier(Final);

    UHorizontalBox* Linha = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

    USizeBox* ColunaDoNome = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    ColunaDoNome->SetWidthOverride(110.f);
    ColunaDoNome->AddChild(CriarTexto(WidgetTree, NomeDoAtributo(Atributo), 12.f, RunnerPalette::TextoCorpo(), TEXT("Regular")));
    Linha->AddChild(ColunaDoNome);

    // Cor pelo sinal: o que o chassi da de bom aparece em Aether, o que tira aparece em cobre.
    const FLinearColor Cor = Modificador > 0 ? RunnerPalette::Aether()
                          : (Modificador < 0 ? RunnerPalette::Cobre() : RunnerPalette::TextoFraco());
    UTextBlock* Valor = CriarTexto(WidgetTree,
        FString::Printf(TEXT("%d   (%s%d)"), Final, Modificador >= 0 ? TEXT("+") : TEXT(""), Modificador), 12.f, Cor);
    Valor->SetAutoWrapText(false);
    Linha->AddChild(Valor);
    return Linha;
}

void URunnerMenuWidget::PreencherInfo(UVerticalBox* Caixa, FName ChassiId, FName ClasseId, const FString& TituloDoRunner)
{
    URunnerSession* Session = GetSession();
    if (!Caixa || !Session)
    {
        return;
    }

    Caixa->ClearChildren();

    FRunnerChassisData Chassi;
    if (ChassiId.IsNone() || !Session->GetChassisData(ChassiId, Chassi))
    {
        Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("Escolha um chassi e uma classe: aqui aparecem os atributos, as vantagens e a conta de HP e CA."),
            12.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 0.f);
        return;
    }

    // Nome do Runner primeiro (quando e um personagem que ja existe), com o nivel.
    if (!TituloDoRunner.IsEmpty())
    {
        Adicionar(Caixa, CriarTexto(WidgetTree, TituloDoRunner, 19.f, RunnerPalette::Ambar()), 1.f);
        Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("Nivel 1"), 11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 8.f);
    }

    // O icone do que foi escolhido, como na referencia de interface.
    if (UTexture2D* Icone = IconeDoChassi(ChassiId))
    {
        UImage* Imagem = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        FSlateBrush Pincel;
        Pincel.SetResourceObject(Icone);
        Pincel.ImageSize = FVector2D(46.f, 46.f);
        Pincel.DrawAs = ESlateBrushDrawType::Image;
        Imagem->SetBrush(Pincel);

        USizeBox* CaixaDoIcone = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        CaixaDoIcone->SetWidthOverride(46.f);
        CaixaDoIcone->SetHeightOverride(46.f);
        CaixaDoIcone->AddChild(Imagem);
        UHorizontalBox* Centro = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
        Centro->AddChild(CaixaDoIcone);
        Adicionar(Caixa, Centro, 6.f);
    }

    Adicionar(Caixa, CriarTexto(WidgetTree, Chassi.DisplayName.ToString().ToUpper(), 17.f, RunnerPalette::LataoPolido()), 2.f);
    Adicionar(Caixa, CriarTexto(WidgetTree,
        // A COR DO NUCLEO em hexadecimal, e nao o RGB cru: e a identidade do chassi (o nucleo e o cristal)
        // e o hexadecimal e o mesmo codigo que o dado usa no DT_Chassis, entao a tela e a tabela falam igual.
        FString::Printf(TEXT("corpo %s · núcleo #%02X%02X%02X"), *NomeDoCorpo(Chassi.Body),
            FMath::RoundToInt(FMath::Clamp(Chassi.CoreColor.R, 0.f, 1.f) * 255.f),
            FMath::RoundToInt(FMath::Clamp(Chassi.CoreColor.G, 0.f, 1.f) * 255.f),
            FMath::RoundToInt(FMath::Clamp(Chassi.CoreColor.B, 0.f, 1.f) * 255.f)),
        11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 8.f);
    Adicionar(Caixa, CriarFilete(WidgetTree, RunnerPalette::LataoEscovado(0.6f), 1.f), 8.f);

    Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("ATRIBUTOS"), 10.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 4.f);
    for (const ERunnerAttribute Atributo : TodosOsAtributos())
    {
        Adicionar(Caixa, CriarLinhaDeAtributo(Atributo, Chassi.GetBonus(Atributo)), 2.f);
    }

    Adicionar(Caixa, CriarFilete(WidgetTree, RunnerPalette::LataoEscovado(0.6f), 1.f), 8.f);
    Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("VANTAGENS"), 10.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 4.f);
    Adicionar(Caixa, CriarTexto(WidgetTree, Chassi.AdvantagePrimary.ToString(), 12.f, RunnerPalette::TextoCorpo(), TEXT("Regular")), 3.f);
    if (!Chassi.AdvantageSecondary.IsEmpty())
    {
        Adicionar(Caixa, CriarTexto(WidgetTree, Chassi.AdvantageSecondary.ToString(), 12.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 3.f);
    }

    // Com a classe escolhida a ficha fecha: entram HP, CA e o foco da classe.
    FRunnerClassData Classe;
    if (!ClasseId.IsNone() && Session->GetClassData(ClasseId, Classe))
    {
        Adicionar(Caixa, CriarFilete(WidgetTree, RunnerPalette::LataoEscovado(0.6f), 1.f), 8.f);
        Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("CLASSE"), 10.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 4.f);
        Adicionar(Caixa, CriarTexto(WidgetTree, Classe.DisplayName.ToString().ToUpper(), 15.f, RunnerPalette::LataoPolido()), 3.f);
        Adicionar(Caixa, CriarTexto(WidgetTree, Classe.Description.ToString(), 12.f, RunnerPalette::TextoCorpo(), TEXT("Regular")), 6.f);

        const int32 Constituicao = URunnerRules::GetFinalAttributeScore(Chassi, ERunnerAttribute::Constitution);
        const int32 Destreza = URunnerRules::GetFinalAttributeScore(Chassi, ERunnerAttribute::Dexterity);
        const int32 Vida = URunnerRules::GetLevelOneHitPoints(Classe.HitDie, Constituicao);
        const int32 ClasseDeArmadura = URunnerRules::GetArmorClass(Destreza, 0);

        Adicionar(Caixa, CriarTexto(WidgetTree,
            FString::Printf(TEXT("HP nivel 1  %d     CA  %d     d%d de vida"), Vida, ClasseDeArmadura, Classe.HitDie),
            13.f, RunnerPalette::Ambar()), 4.f);
        Adicionar(Caixa, CriarTexto(WidgetTree,
            FString::Printf(TEXT("Foco: %s e %s"), *NomeDoAtributo(Classe.PrimaryAttribute), *NomeDoAtributo(Classe.SecondaryAttribute)),
            12.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 4.f);
        Adicionar(Caixa, CriarTexto(WidgetTree,
            FString::Printf(TEXT("Celulas de Eter  %d"), URunnerRules::GetMaxEtherCells()),
            12.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 0.f);
    }
    else
    {
        Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("Escolha a classe para ver HP, CA e o foco."),
            12.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 0.f);
    }
}

void URunnerMenuWidget::MostrarNaVitrine(FName ChassiId)
{
    if (!Vitrine || ChassiId.IsNone() || ChassiId == ChassiNaVitrine)
    {
        return;
    }

    URunnerSession* Session = GetSession();
    FRunnerChassisData Dados;
    if (!Session || !Session->GetChassisData(ChassiId, Dados))
    {
        return;
    }

    ChassiNaVitrine = ChassiId;

    // Quem decide cristal ou corpo e a aba atual (o chassi E o cristal; o corpo e da classe).
    AtualizarVitrine();
}

UMaterialInterface* URunnerMenuWidget::MaterialDoNucleo(const FName& ChassiId)
{
    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    if (!Ajustes || ChassiId.IsNone())
    {
        return nullptr;
    }

    // Convencao de nome: <pasta>/MI_Nucleo_<Chassi>.MI_Nucleo_<Chassi> (a cor e dado, nao codigo).
    const FString Nome = ChassiId.ToString();
    const FString NomeDoMaterial = FString::Printf(TEXT("MI_Nucleo_%s"), *Nome);
    const FString Caminho = FString::Printf(TEXT("%s/%s.%s"), *Ajustes->PastaDosMateriaisDoNucleo, *NomeDoMaterial, *NomeDoMaterial);
    UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *Caminho);
    if (!Material)
    {
        UE_LOG(LogTemp, Warning, TEXT("URunnerMenuWidget: material do nucleo nao encontrado em %s"), *Caminho);
    }
    return Material;
}

void URunnerMenuWidget::LigarMonitorNaVitrine()
{
    if (!PreviewImage || !Vitrine)
    {
        return;
    }
    if (UTextureRenderTarget2D* Alvo = Vitrine->GetRenderTarget())
    {
        FSlateBrush Pincel;
        Pincel.SetResourceObject(Alvo);
        Pincel.ImageSize = FVector2D(Vitrine->LadoDaCaptura, Vitrine->LadoDaCaptura);
        Pincel.DrawAs = ESlateBrushDrawType::Image;
        PreviewImage->SetBrush(Pincel);
    }
}

void URunnerMenuWidget::AtualizarVitrine()
{
    // O monitor sempre aponta para a vitrine DESTE passo: sem isso ele mostra a captura da aba anterior.
    LigarMonitorNaVitrine();

    URunnerSession* Session = GetSession();
    if (!Vitrine || !Session)
    {
        return;
    }

    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();

    // NUNCA deixar a vitrine vazia. Sem escolha do jogador, ela mostra o PRIMEIRO chassi selecionavel:
    // uma caixa escura e vazia nao e vitrine, e um defeito de apresentacao - e era exatamente o que a tela
    // mostrava antes de o jogador escolher.
    FName ChassiDaVez = Session->GetSelectedChassis();
    if (ChassiDaVez.IsNone())
    {
        ChassiDaVez = ChassiNaVitrine;
    }
    if (ChassiDaVez.IsNone() && Ajustes)
    {
        if (const UDataTable* Tabela = Ajustes->ChassisTable.LoadSynchronous())
        {
            const TArray<FName> Ids = URunnerCharacterFactory::GetChassisIds(Tabela, false);
            if (Ids.Num() > 0)
            {
                ChassiDaVez = Ids[0];
            }
        }
    }

    // Aba CHASSI: o cristal girando com a aura em volta.
    if (CurrentStep == ERunnerMenuStep::Chassis && Ajustes && !Ajustes->NucleoMesh.IsNull())
    {
        FLinearColor CorDoNucleo = FLinearColor(0.4f, 0.75f, 1.f);
        FRunnerChassisData DadosDoChassi;
        if (Session->GetChassisData(ChassiDaVez, DadosDoChassi))
        {
            CorDoNucleo = DadosDoChassi.CoreColor;
        }
        Vitrine->SetNucleo(Ajustes->NucleoMesh, MaterialDoNucleo(ChassiDaVez),
                           Ajustes->AuraDoNucleo.LoadSynchronous(), CorDoNucleo);
        return;
    }

    // Aba CLASSE e lista de Runners: o corpo.
    FRunnerChassisData Dados;
    if (Session->GetChassisData(ChassiDaVez, Dados))
    {
        Vitrine->SetPreview(Dados.Mesh, Dados.AccentColor, Ajustes ? Ajustes->IdleAnim.LoadSynchronous() : nullptr);
    }
}

void URunnerMenuWidget::DestruirVitrine()
{
    if (Vitrine)
    {
        Vitrine->Destroy();
        Vitrine = nullptr;
    }
    ChassiNaVitrine = NAME_None;
    PreviewImage = nullptr;
}

void URunnerMenuWidget::PrevisualizarOpcao(const FName& OptionId, ERunnerOptionAction Action, const FString& CharacterId)
{
    URunnerSession* Session = GetSession();
    if (!Session || Action == ERunnerOptionAction::Excluir)
    {
        return;
    }

    // Lista de personagens: o mouse em cima mostra aquele Runner (sem mexer na sessao).
    if (CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        for (const FRunnerSavedCharacter& Salvo : Session->GetSavedCharacters())
        {
            if (Salvo.CharacterId == CharacterId)
            {
                MostrarNaVitrine(Salvo.ChassisId);
                PreencherInfo(DetalhesBox, Salvo.ChassisId, Salvo.ClassId, Salvo.GetDisplayName());
                return;
            }
        }
        return;
    }

    // Passo da classe: o corpo continua o chassi escolhido; a ficha mostra a classe sob o mouse.
    if (CurrentStep == ERunnerMenuStep::Class)
    {
        MostrarNaVitrine(Session->GetSelectedChassis());
        PreencherInfo(DetalhesBox, Session->GetSelectedChassis(), OptionId);
        return;
    }

    // Aba do chassi: corpo e detalhes do que esta sob o mouse.
    MostrarNaVitrine(OptionId);
    PreencherInfo(DetalhesBox, OptionId, Session->GetSelectedClass());
}

void URunnerMenuWidget::TrocarAba(const ERunnerMenuStep Aba)
{
    URunnerSession* Session = GetSession();
    if (Aba == ERunnerMenuStep::Class && (!Session || Session->GetSelectedChassis().IsNone()))
    {
        SetStatus(TEXT("Escolha um chassi primeiro: e ele que decide a sua vocacao."));
        return;
    }
    GoToStep(Aba);

    // Trocar de aba troca o que a vitrine mostra: cristal (chassi) x corpo (classe).
    AtualizarVitrine();
}

void URunnerMenuWidget::SelecionarPersonagem(const FString& CharacterId, const bool bEntrarNoJogo)
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        return;
    }

    IdPersonagemSelecionado = CharacterId;

    // Marcar so mostra: o jogador ve nivel, nome, chassi e classe antes de entrar.
    if (!bEntrarNoJogo)
    {
        GoToStep(ERunnerMenuStep::CharacterSelect);
        return;
    }

    FString Erro;
    if (!Session->SelectSavedCharacter(CharacterId, Erro))
    {
        SetStatus(Erro);
        return;
    }

    FRunnerCharacterProfile Perfil;
    Session->GetActiveProfile(Perfil);
    SetStatus(FString::Printf(TEXT("Entrando com %s..."), *Perfil.CharacterName));
    OpenGameLevel();
}

void URunnerMenuWidget::OnQuartoClicked()
{
    // O quarto botao so existe na lista de Runners: Voltar = sair da conta.
    const ERunnerMenuStep Destino = RunnerDestinoDoBotao(ERunnerMenuButton::Quarto, CurrentStep, true);
    if (Destino == ERunnerMenuStep::Login)
    {
        if (URunnerSession* Session = GetSession())
        {
            Session->Logout();
        }
        IdPersonagemSelecionado.Reset();
        GoToStep(Destino);
    }
}

void URunnerMenuWidget::AbrirConfirmacao(ERunnerConfirmAction Acao, const FString& Mensagem, const FString& CharacterId)
{
    Confirmacao = Acao;
    IdDaConfirmacao = CharacterId;
    MensagemDaConfirmacao = Mensagem;
    PassoAntesDaConfirmacao = CurrentStep;

    // Apagar: a mensagem e montada aqui, com o nome que o jogador conhece.
    if (Acao == ERunnerConfirmAction::ExcluirPersonagem)
    {
        FString NomeDoRunner = CharacterId;
        if (URunnerSession* Session = GetSession())
        {
            for (const FRunnerSavedCharacter& Salvo : Session->GetSavedCharacters())
            {
                if (Salvo.CharacterId == CharacterId)
                {
                    NomeDoRunner = FString::Printf(TEXT("%s (%s · %s)"), *Salvo.GetDisplayName(),
                        *Salvo.ChassisId.ToString(), *Salvo.ClassId.ToString());
                    break;
                }
            }
        }

        MensagemDaConfirmacao = FString::Printf(
            TEXT("Apagar o Runner\n\n%s\n\nIsto nao tem volta: a ficha e o nome dele saem da conta."), *NomeDoRunner);
    }

    GoToStep(ERunnerMenuStep::Confirm);
}

void URunnerMenuWidget::ExecutarConfirmacao()
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        return;
    }

    const ERunnerConfirmAction Acao = Confirmacao;
    const FString Id = IdDaConfirmacao;
    Confirmacao = ERunnerConfirmAction::Nada;
    IdDaConfirmacao.Reset();

    FString Erro;

    if (Acao == ERunnerConfirmAction::CriarPersonagem)
    {
        FRunnerCharacterProfile Perfil;
        if (!Session->CreateCharacterProfile(Perfil, Erro))
        {
            // Nome repetido ou limite da conta cai aqui: volta para a tela com o motivo.
            GoToStep(ERunnerMenuStep::Class);
            SetStatus(Erro);
            return;
        }

        SetStatus(FString::Printf(TEXT("Runner %s entrou em campo: HP %d, CA %d."),
            *Perfil.CharacterName, Perfil.MaxHitPoints, Perfil.ArmorClass));
        OpenGameLevel();
        return;
    }

    if (Acao == ERunnerConfirmAction::ExcluirPersonagem)
    {
        if (!Session->DeleteSavedCharacter(Id, Erro))
        {
            GoToStep(ERunnerMenuStep::CharacterSelect);
            SetStatus(Erro);
            return;
        }

        GoToStep(ERunnerMenuStep::CharacterSelect);
        SetStatus(TEXT("Runner apagado. A conta segue com os outros."));
        return;
    }

    GoToStep(ERunnerMenuStep::CharacterSelect);
}


// ---------------------------------------------------------------------------
// Telas: criacao (abas, grade de icones, vitrine, detalhes) e Runners existentes
// ---------------------------------------------------------------------------

UTexture2D* URunnerMenuWidget::IconeDoChassi(FName ChassiId)
{
    if (ChassiId.IsNone())
    {
        return nullptr;
    }
    if (const TObjectPtr<UTexture2D>* Achado = IconesDeChassi.Find(ChassiId))
    {
        return *Achado;
    }

    URunnerSession* Session = GetSession();
    FRunnerChassisData Dados;
    if (!Session || !Session->GetChassisData(ChassiId, Dados))
    {
        return nullptr;
    }

    // O icone e desenhado por codigo (RunnerIconFactory): placa + glifo na cor do chassi.
    UTexture2D* Icone = RunnerIcons::CriarIcone(this, RunnerIcons::GlifoDoCorpo(Dados.Body), Dados.AccentColor, 64);
    IconesDeChassi.Add(ChassiId, Icone);
    return Icone;
}

UTexture2D* URunnerMenuWidget::IconeDaClasse(FName ClassId)
{
    if (ClassId.IsNone())
    {
        return nullptr;
    }
    if (const TObjectPtr<UTexture2D>* Achado = IconesDeClasse.Find(ClassId))
    {
        return *Achado;
    }

    URunnerSession* Session = GetSession();
    FRunnerClassData Dados;
    if (!Session || !Session->GetClassData(ClassId, Dados))
    {
        return nullptr;
    }

    UTexture2D* Icone = RunnerIcons::CriarIcone(this, RunnerIcons::GlifoDoPapel(Dados.Role),
                                                CorDoPapel(Dados.Role), 64);
    IconesDeClasse.Add(ClassId, Icone);
    return Icone;
}

UWidget* URunnerMenuWidget::CriarAbas()
{
    UHorizontalBox* Abas = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

    auto AdicionarAba = [this, Abas](const FString& Rotulo, const ERunnerMenuStep Destino)
    {
        const bool bAtiva = CurrentStep == Destino;

        UButton* Botao = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
        // A aba ativa e latao cheio; as outras sao aco, para o olho saber onde esta.
        EstilizarBotao(Botao,
                       bAtiva ? RunnerPalette::LataoEscovado(0.9f) : RunnerPalette::AcoEscuro(0.85f),
                       bAtiva ? RunnerPalette::LataoPolido(1.f) : RunnerPalette::FerroForjado(1.f),
                       bAtiva ? RunnerPalette::Ambar() : RunnerPalette::BordaCampo(), bAtiva ? 2.f : 1.f);
        Botao->AddChild(CriarTexto(WidgetTree, Rotulo, 13.f,
            bAtiva ? RunnerPalette::Silhueta() : RunnerPalette::TextoCorpo()));

        URunnerTabButton* Binding = NewObject<URunnerTabButton>(this);
        Binding->Owner = this;
        Binding->Destino = Destino;
        TabBindings.Add(Binding); // mantem vivo junto com os outros
        Botao->OnClicked.AddDynamic(Binding, &URunnerTabButton::HandleClicked);

        if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Abas->AddChild(Botao)))
        {
            Slot->SetPadding(FMargin(0.f, 0.f, 8.f, 0.f));
        }
    };

    AdicionarAba(TEXT("1 · CHASSI"), ERunnerMenuStep::Chassis);
    AdicionarAba(TEXT("2 · CLASSE"), ERunnerMenuStep::Class);
    return Abas;
}

UWidget* URunnerMenuWidget::CriarGradeDeIcones(TArray<UWidget*>& AlvosDoVeu)
{
    URunnerSession* Session = GetSession();
    const bool bChassi = CurrentStep != ERunnerMenuStep::Class;

    UScrollBox* Rolagem = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
    UUniformGridPanel* Grade = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass());
    Grade->SetSlotPadding(FMargin(5.f));
    Rolagem->AddChild(Grade);

    if (!Session)
    {
        return Rolagem;
    }

    const FName Escolhido = bChassi ? Session->GetSelectedChassis() : Session->GetSelectedClass();

    auto AdicionarCelula = [this, Grade, &AlvosDoVeu, Escolhido](const int32 Coluna, const int32 Linha,
                                                                 const FName Id, const FString& Rotulo,
                                                                 UTexture2D* Icone)
    {
        const bool bSelecionado = (Id == Escolhido);

        UButton* Celula = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
        EstilizarBotao(Celula,
                       bSelecionado ? RunnerPalette::FerroForjado(1.f) : RunnerPalette::AcoEscuro(0.9f),
                       RunnerPalette::FerroForjado(1.1f),
                       bSelecionado ? RunnerPalette::Ambar() : RunnerPalette::BordaCampo(),
                       bSelecionado ? 2.f : 1.f);
        AlvosDoVeu.Add(Celula);

        UVerticalBox* Conteudo = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());

        UImage* Imagem = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        if (Icone)
        {
            FSlateBrush Pincel;
            Pincel.SetResourceObject(Icone);
            Pincel.ImageSize = FVector2D(38.f, 38.f);
            Pincel.DrawAs = ESlateBrushDrawType::Image;
            Imagem->SetBrush(Pincel);
        }
        USizeBox* CaixaDoIcone = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        CaixaDoIcone->SetWidthOverride(38.f);
        CaixaDoIcone->SetHeightOverride(38.f);
        CaixaDoIcone->AddChild(Imagem);
        UHorizontalBox* CentroDoIcone = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
        CentroDoIcone->AddChild(CaixaDoIcone);
        Adicionar(Conteudo, CentroDoIcone, 3.f);

        UTextBlock* Texto = CriarTexto(WidgetTree, Rotulo, 11.f,
            bSelecionado ? RunnerPalette::Ambar() : RunnerPalette::TextoCorpo(), TEXT("Regular"));
        Texto->SetJustification(ETextJustify::Center);
        Texto->SetAutoWrapText(false);
        Conteudo->AddChild(Texto);

        Celula->AddChild(Conteudo);

        URunnerOptionButton* Binding = NewObject<URunnerOptionButton>(this);
        Binding->OptionId = Id;
        Binding->Owner = this;
        Binding->Action = ERunnerOptionAction::Jogar;
        OptionBindings.Add(Binding);
        Celula->OnClicked.AddDynamic(Binding, &URunnerOptionButton::HandleClicked);
        Celula->OnHovered.AddDynamic(Binding, &URunnerOptionButton::HandleHovered);

        Grade->AddChildToUniformGrid(Celula, Linha, Coluna);
    };

    if (bChassi)
    {
        const TArray<FName> Ids = Session->GetChassisIds(false);
        for (int32 Indice = 0; Indice < Ids.Num(); ++Indice)
        {
            const FName Id = Ids[Indice];
            FRunnerChassisData Dados;
            const FString Rotulo = Session->GetChassisData(Id, Dados) ? Dados.DisplayName.ToString() : Id.ToString();
            AdicionarCelula(Indice % ColunasDaGrade, Indice / ColunasDaGrade, Id, Rotulo, IconeDoChassi(Id));
        }
    }
    else
    {
        const TArray<FName> Ids = Session->GetClassIds();
        for (int32 Indice = 0; Indice < Ids.Num(); ++Indice)
        {
            const FName Id = Ids[Indice];
            FRunnerClassData Dados;
            const FString Rotulo = Session->GetClassData(Id, Dados) ? Dados.DisplayName.ToString() : Id.ToString();
            AdicionarCelula(Indice % ColunasDaGrade, Indice / ColunasDaGrade, Id, Rotulo, IconeDaClasse(Id));
        }
    }

    return Rolagem;
}

UWidget* URunnerMenuWidget::CriarColunaDeDetalhes()
{
    UBorder* Painel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
    Painel->SetBrush(Caixa(RunnerPalette::AcoEscuro(0.85f), 8.f, RunnerPalette::BordaCampo(), 1.f));
    Painel->SetPadding(FMargin(16.f, 14.f));

    UScrollBox* Rolagem = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
    DetalhesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    Rolagem->AddChild(DetalhesBox);
    Painel->SetContent(Rolagem);
    return Painel;
}

namespace
{
    /**
     * Linha de campo com o icone do kit a esquerda (usuario, senha), como no alvo do autor. O icone e
     * opcional: sem textura a linha e so o campo - campo sem icone le melhor que campo com quadrado vazio.
     */
    UWidget* LinhaDeCampoComIcone(UWidget* Campo, UTexture2D* Icone)
    {
        if (!Campo || !Icone)
        {
            return Campo;
        }
        UHorizontalBox* Linha = NewObject<UHorizontalBox>(Campo->GetOuter());
        UImage* Marca = NewObject<UImage>(Campo->GetOuter());
        Marca->SetBrushFromTexture(Icone, false);
        Marca->SetDesiredSizeOverride(FVector2D(20.f, 20.f));
        Marca->SetColorAndOpacity(FLinearColor(0.f, 0.9f, 1.f, 0.95f));
        if (UHorizontalBoxSlot* EspacoDoIcone = Linha->AddChildToHorizontalBox(Marca))
        {
            EspacoDoIcone->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
            EspacoDoIcone->SetPadding(FMargin(0.f, 0.f, 10.f, 0.f));
            EspacoDoIcone->SetVerticalAlignment(VAlign_Center);
        }
        if (UHorizontalBoxSlot* EspacoDoCampo = Linha->AddChildToHorizontalBox(Campo))
        {
            EspacoDoCampo->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        }
        return Linha;
    }

    /**
     * Texto de canto do guia (Art/Tela_login): a moldura de frases que da clima a tela de entrada sem
     * competir com o painel. Entra no canvas do fundo, com ancoragem propria em cada canto.
     */
    void PorTextoDeCanto(UWidgetTree* Arvore, UCanvasPanel* Canvas, const FString& Texto, const float Tamanho,
                         const FLinearColor& Cor, const FAnchors& Ancora, const FVector2D& Deslocamento,
                         const FVector2D& Alinhamento)
    {
        UTextBlock* Bloco = Arvore->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Bloco->SetText(FText::FromString(Texto));
        Bloco->SetColorAndOpacity(FSlateColor(Cor));
        Bloco->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), Tamanho));
        if (UCanvasPanelSlot* Espaco = Canvas->AddChildToCanvas(Bloco))
        {
            Espaco->SetAnchors(Ancora);
            Espaco->SetAlignment(Alinhamento);
            Espaco->SetAutoSize(true);
            Espaco->SetPosition(Deslocamento);
        }
    }
}

void URunnerMenuWidget::CriarFundo(UCanvasPanel* Canvas)
{
    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    if (!Canvas || !Ajustes || Ajustes->TexturaDoFundo.IsNull())
    {
        return;
    }

    UTexture2D* Textura = Ajustes->TexturaDoFundo.LoadSynchronous();
    if (!Textura)
    {
        UE_LOG(LogTemp, Warning, TEXT("URunnerMenuWidget: TexturaDoFundo configurada mas nao carregou."));
        return;
    }

    // a arte cobre a tela inteira (ancoras esticadas nos dois eixos)
    UImage* Fundo = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Fundo"));
    Fundo->SetBrushFromTexture(Textura, false);
    if (UCanvasPanelSlot* FundoSlot = Canvas->AddChildToCanvas(Fundo))
    {
        FundoSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
        FundoSlot->SetOffsets(FMargin(0.f));
    }

    // veu escuro por cima da arte: o painel e o texto precisam ler
    if (Ajustes->EscurecimentoDoFundo > 0.f)
    {
        UBorder* Sombra = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SombraDoFundo"));
        FSlateBrush Pincel;
        Pincel.TintColor = FSlateColor(FLinearColor(0.02f, 0.02f, 0.04f, Ajustes->EscurecimentoDoFundo));
        Sombra->SetBrush(Pincel);
        if (UCanvasPanelSlot* SombraSlot = Canvas->AddChildToCanvas(Sombra))
        {
            SombraSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
            SombraSlot->SetOffsets(FMargin(0.f));
        }
    }

    // A MOLDURA DE FRASES DO GUIA: quatro cantos e um rodape, exatamente como no guia do autor. Elas entram
    // depois do veu, entao ficam ACIMA dele e legiveis, e ficam nas bordas para nao competir com o painel.
    const FAnchors CantoEsquerdo(0.f, 0.f);
    const FAnchors CantoDireito(1.f, 0.f);
    const FAnchors PeEsquerdo(0.f, 1.f);
    const FAnchors PeDireito(1.f, 1.f);
    PorTextoDeCanto(WidgetTree, Canvas, TEXT("A NOVO MUNDO\nSERA REFORJADO"), 10.f, RunnerPalette::TextoFraco(),
                    CantoEsquerdo, FVector2D(34.f, 34.f), FVector2D(0.f, 0.f));
    PorTextoDeCanto(WidgetTree, Canvas, TEXT("NANOS\nNUNCA ESQUECEM"), 10.f, RunnerPalette::TextoFraco(),
                    CantoDireito, FVector2D(-34.f, 34.f), FVector2D(1.f, 0.f));
    PorTextoDeCanto(WidgetTree, Canvas, TEXT("PROTOCOL ZERO\nINICIA AGORA"), 10.f, RunnerPalette::AzulNeon(0.8f),
                    PeEsquerdo, FVector2D(34.f, -34.f), FVector2D(0.f, 1.f));
    PorTextoDeCanto(WidgetTree, Canvas, TEXT("HUMANIDADE // VERSAO 2.0"), 10.f, RunnerPalette::Violeta(0.85f),
                    PeDireito, FVector2D(-34.f, -34.f), FVector2D(1.f, 1.f));
    PorTextoDeCanto(WidgetTree, Canvas, TEXT("CONEXAO COM UM MUNDO MAIOR"), 10.f, RunnerPalette::TextoFraco(),
                    FAnchors(0.5f, 1.f), FVector2D(0.f, -20.f), FVector2D(0.5f, 1.f));
}

void URunnerMenuWidget::ConstruirTelaDeCriacao(TArray<UWidget*>& AlvosDoVeu)
{
    Adicionar(RootBox, CriarAbas(), 10.f);

    UHorizontalBox* Colunas = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

    // Esquerda: a grade de icones — o que se escolhe.
    USizeBox* LarguraDaGrade = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    LarguraDaGrade->SetWidthOverride(330.f);
    LarguraDaGrade->AddChild(CriarGradeDeIcones(AlvosDoVeu));
    if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Colunas->AddChild(LarguraDaGrade)))
    {
        Slot->SetPadding(FMargin(0.f, 0.f, 14.f, 0.f));
    }

    // Meio: a vitrine 3D.
    if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Colunas->AddChild(CriarBlocoDaVitrine())))
    {
        Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        Slot->SetHorizontalAlignment(HAlign_Center);
        Slot->SetVerticalAlignment(VAlign_Center);
    }

    // Direita: ficha e detalhes do que esta escolhido (com o icone).
    USizeBox* LarguraDosDetalhes = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    LarguraDosDetalhes->SetWidthOverride(320.f);
    LarguraDosDetalhes->AddChild(CriarColunaDeDetalhes());
    if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Colunas->AddChild(LarguraDosDetalhes)))
    {
        Slot->SetPadding(FMargin(14.f, 0.f, 0.f, 0.f));
    }

    USizeBox* AlturaDasColunas = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    AlturaDasColunas->SetHeightOverride(AlturaDaVitrine);
    AlturaDasColunas->AddChild(Colunas);
    Adicionar(RootBox, AlturaDasColunas, 10.f);

    // Nome do Runner: e por ele que o jogador reconhece o personagem na lista.
    Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("NOME DO RUNNER"), 10.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 4.f);
    NameBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
    NameBox->SetHintText(FText::FromString(TEXT("nome do personagem (3 a 16 caracteres)")));
    NameBox->SetText(FText::FromString(GetSession() ? GetSession()->GetPendingCharacterName() : FString()));
    EstilizarCampo(NameBox);
    Adicionar(RootBox, NameBox, 8.f);
    AlvosDoVeu.Add(NameBox);

    AtualizarSelecao();
}

void URunnerMenuWidget::ConstruirTelaDeRunners(TArray<UWidget*>& AlvosDoVeu)
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        return;
    }

    // A marcacao tem que existir de verdade: ela pode ter sido apagada (o id fica preso apontando
    // para um Runner que nao esta mais na conta) e a lista ficaria sem ninguem marcado.
    const TArray<FRunnerSavedCharacter> Meus = Session->GetSavedCharacters();
    const bool bMarcacaoValida = Meus.ContainsByPredicate(
        [this](const FRunnerSavedCharacter& Salvo) { return Salvo.CharacterId == IdPersonagemSelecionado; });
    if (!bMarcacaoValida)
    {
        IdPersonagemSelecionado = Meus.Num() > 0 ? Meus[0].CharacterId : FString();
    }

    UHorizontalBox* Colunas = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

    // Esquerda: a lista dos Runners da conta (clicar marca, nao entra no jogo).
    USizeBox* LarguraDaLista = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    LarguraDaLista->SetWidthOverride(330.f);
    UScrollBox* Lista = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
    LarguraDaLista->AddChild(Lista);

    for (const FRunnerSavedCharacter& Salvo : Meus)
    {
        const bool bMarcado = Salvo.CharacterId == IdPersonagemSelecionado;

        UButton* Linha = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
        EstilizarBotao(Linha,
                       bMarcado ? RunnerPalette::FerroForjado(1.f) : RunnerPalette::AcoEscuro(0.9f),
                       RunnerPalette::FerroForjado(1.1f),
                       bMarcado ? RunnerPalette::Ambar() : RunnerPalette::BordaCampo(), bMarcado ? 2.f : 1.f);
        AlvosDoVeu.Add(Linha);

        UHorizontalBox* Conteudo = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

        // Retrato: o icone do chassi daquele Runner.
        UImage* Retrato = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        if (UTexture2D* Icone = IconeDoChassi(Salvo.ChassisId))
        {
            FSlateBrush Pincel;
            Pincel.SetResourceObject(Icone);
            Pincel.ImageSize = FVector2D(30.f, 30.f);
            Pincel.DrawAs = ESlateBrushDrawType::Image;
            Retrato->SetBrush(Pincel);
        }
        USizeBox* CaixaDoRetrato = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        CaixaDoRetrato->SetWidthOverride(30.f);
        CaixaDoRetrato->SetHeightOverride(30.f);
        CaixaDoRetrato->AddChild(Retrato);
        Conteudo->AddChild(CaixaDoRetrato);

        UVerticalBox* Textos = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
        Adicionar(Textos, CriarTexto(WidgetTree, Salvo.GetDisplayName(), 13.f,
            bMarcado ? RunnerPalette::Ambar() : RunnerPalette::TextoCorpo()), 1.f);
        Adicionar(Textos, CriarTexto(WidgetTree,
            FString::Printf(TEXT("Nivel 1 · %s · %s"), *Salvo.ChassisId.ToString(), *Salvo.ClassId.ToString()),
            11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 0.f);

        if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Conteudo->AddChild(Textos)))
        {
            Slot->SetPadding(FMargin(10.f, 0.f, 0.f, 0.f));
        }
        Linha->AddChild(Conteudo);

        URunnerOptionButton* Binding = NewObject<URunnerOptionButton>(this);
        Binding->OptionId = FName(*Salvo.CharacterId);
        Binding->Owner = this;
        Binding->Action = ERunnerOptionAction::Selecionar;
        Binding->CharacterId = Salvo.CharacterId;
        OptionBindings.Add(Binding);
        Linha->OnClicked.AddDynamic(Binding, &URunnerOptionButton::HandleClicked);
        Linha->OnHovered.AddDynamic(Binding, &URunnerOptionButton::HandleHovered);

        Lista->AddChild(Linha);
    }

    if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Colunas->AddChild(LarguraDaLista)))
    {
        Slot->SetPadding(FMargin(0.f, 0.f, 14.f, 0.f));
    }

    if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Colunas->AddChild(CriarBlocoDaVitrine())))
    {
        Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        Slot->SetHorizontalAlignment(HAlign_Center);
        Slot->SetVerticalAlignment(VAlign_Center);
    }

    USizeBox* LarguraDosDetalhes = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    LarguraDosDetalhes->SetWidthOverride(320.f);
    LarguraDosDetalhes->AddChild(CriarColunaDeDetalhes());
    if (UHorizontalBoxSlot* Slot = Cast<UHorizontalBoxSlot>(Colunas->AddChild(LarguraDosDetalhes)))
    {
        Slot->SetPadding(FMargin(14.f, 0.f, 0.f, 0.f));
    }

    USizeBox* AlturaDasColunas = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    AlturaDasColunas->SetHeightOverride(AlturaDaVitrine);
    AlturaDasColunas->AddChild(Colunas);
    Adicionar(RootBox, AlturaDasColunas, 10.f);

    AtualizarSelecao();
}

void URunnerMenuWidget::AtualizarSelecao()
{
    URunnerSession* Session = GetSession();
    if (!Session)
    {
        return;
    }

    FName ChassiId = Session->GetSelectedChassis();
    FName ClasseId = Session->GetSelectedClass();
    FString TituloDoRunner;

    if (CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        for (const FRunnerSavedCharacter& Salvo : Session->GetSavedCharacters())
        {
            if (Salvo.CharacterId == IdPersonagemSelecionado)
            {
                ChassiId = Salvo.ChassisId;
                ClasseId = Salvo.ClassId;
                TituloDoRunner = Salvo.GetDisplayName();
                break;
            }
        }
    }

    MostrarNaVitrine(ChassiId);
    PreencherInfo(DetalhesBox, ChassiId, ClasseId, TituloDoRunner);
}
