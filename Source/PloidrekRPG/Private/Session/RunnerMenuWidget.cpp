#include "Session/RunnerMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
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
#include "Components/Spacer.h"
#include "Components/Widget.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerClassData.h"
#include "Data/RunnerGameSettings.h"
#include "Data/RunnerRules.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "UI/RunnerPreviewActor.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
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

    UTextBlock* CriarTexto(UWidgetTree* Arvore, const FString& Conteudo, const float Tamanho,
                           const FLinearColor& Cor, const FName Tipo = TEXT("Bold"))
    {
        UTextBlock* Texto = Arvore->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Texto->SetText(FText::FromString(Conteudo));
        Texto->SetColorAndOpacity(FSlateColor(Cor));
        Texto->SetAutoWrapText(true);
        Texto->SetFont(FCoreStyle::GetDefaultFontStyle(Tipo, Tamanho));
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
        Estilo.SetNormalPadding(FMargin(12.f, 9.f));
        Estilo.SetPressedPadding(FMargin(12.f, 10.f, 12.f, 8.f));
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

void URunnerMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
    WidgetTree->RootWidget = Canvas;

    // Painel: placa de aco escuro com contorno de latao escovado (paleta canonica).
    UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Panel"));
    Panel->SetBrush(Caixa(RunnerPalette::FundoPainel(), RaioDoPainel, RunnerPalette::BordaPainel(), 1.5f));
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
    const float AlturaDoPainel = FMath::Clamp(Tela.Y * 0.92f, 560.f, 1040.f);
    if (PanelSlotDoPainel)
    {
        PanelSlotDoPainel->SetSize(FVector2D(FMath::Clamp(Tela.X * 0.68f, 740.f, 1180.f), AlturaDoPainel));
    }

    // A vitrine e a lista dividem a altura do painel: em tela baixa tudo encolhe, nada some.
    AlturaDaVitrine = FMath::Clamp(AlturaDoPainel * 0.40f, 168.f, 290.f);
    AlturaDaLista = FMath::Clamp(AlturaDoPainel * 0.24f, 96.f, 185.f);

    // Cabecalho: marca em latao polido, o passo atual em letra miuda e um filete de metal.
    Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("PLOIDREKRPG"), 26.f, RunnerPalette::LataoPolido()), 2.f);

    FString Passo;
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:           Passo = TEXT("NEXUS-7 · ACESSO"); break;
        case ERunnerMenuStep::CreateAccount:   Passo = TEXT("NEXUS-7 · NOVA CONTA"); break;
        case ERunnerMenuStep::CharacterSelect: Passo = TEXT("SEUS RUNNERS"); break;
        case ERunnerMenuStep::Chassis:         Passo = TEXT("ESCOLHA O CHASSI"); break;
        case ERunnerMenuStep::Class:           Passo = TEXT("ESCOLHA A CLASSE"); break;
        case ERunnerMenuStep::Confirm:         Passo = TEXT("CONFIRMAR"); break;
    }
    Adicionar(RootBox, CriarTexto(WidgetTree, Passo, 11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 10.f);
    Adicionar(RootBox, CriarFilete(WidgetTree, RunnerPalette::LataoEscovado(0.75f), 1.5f), 14.f);

    // Vitrine 3D a esquerda, ficha do que esta selecionado a direita.
    const bool bPassoDeEscolha = CurrentStep == ERunnerMenuStep::Chassis || CurrentStep == ERunnerMenuStep::Class
                              || CurrentStep == ERunnerMenuStep::CharacterSelect;
    if (bPassoDeEscolha)
    {
        UHorizontalBox* Topo = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

        if (UHorizontalBoxSlot* SlotDaVitrine = Cast<UHorizontalBoxSlot>(Topo->AddChild(CriarBlocoDaVitrine())))
        {
            SlotDaVitrine->SetPadding(FMargin(0.f, 0.f, 14.f, 0.f));
            SlotDaVitrine->SetVerticalAlignment(VAlign_Top);
        }
        if (UHorizontalBoxSlot* SlotDaInfo = Cast<UHorizontalBoxSlot>(Topo->AddChild(CriarBlocoDeInfo())))
        {
            SlotDaInfo->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            SlotDaInfo->SetVerticalAlignment(VAlign_Fill);
        }
        // Altura propria: a ficha rola dentro dela e nao empurra os botoes para fora do painel.
        USizeBox* CaixaDoTopo = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        CaixaDoTopo->SetHeightOverride(AlturaDaVitrine);
        CaixaDoTopo->AddChild(Topo);
        Adicionar(RootBox, CaixaDoTopo, 12.f);

        // O que a vitrine mostra ao abrir o passo.
        FName ChassiDaInfo = NAME_None;
        FName ClasseDaInfo = NAME_None;
        if (Session)
        {
            if (CurrentStep == ERunnerMenuStep::CharacterSelect)
            {
                const TArray<FRunnerSavedCharacter> Meus = Session->GetSavedCharacters();
                if (Meus.Num() > 0)
                {
                    ChassiDaInfo = Meus[0].ChassisId;
                    ClasseDaInfo = Meus[0].ClassId;
                }
            }
            else
            {
                ChassiDaInfo = Session->GetSelectedChassis();
                ClasseDaInfo = Session->GetSelectedClass();
            }
        }
        MostrarNaVitrine(ChassiDaInfo);
        PreencherInfo(InfoBox, ChassiDaInfo, ClasseDaInfo);
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
        AccountBox->SetHintText(FText::FromString(bCamposDoDevAuth ? TEXT("localhost:8081") : TEXT("e-mail")));
        EstilizarCampo(AccountBox);
        Adicionar(RootBox, AccountBox, 10.f);
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
                ? TEXT("senha (8+, maiuscula, minuscula, especial)") : TEXT("senha"))));
        PasswordBox->SetIsPassword(!bCamposDoDevAuth);
        EstilizarCampo(PasswordBox);
        Adicionar(RootBox, PasswordBox, 10.f);
        AlvosDoVeu.Add(PasswordBox);

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

    // Listas de opcao. Os ids vem sempre dos dados (DataTable) ou dos personagens da conta.
    if (CurrentStep == ERunnerMenuStep::Chassis || CurrentStep == ERunnerMenuStep::Class
        || CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        UScrollBox* List = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
        USizeBox* CaixaDaLista = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        CaixaDaLista->SetHeightOverride(AlturaDaLista);
        CaixaDaLista->AddChild(List);
        Adicionar(RootBox, CaixaDaLista, 10.f);
        AlvosDoVeu.Add(CaixaDaLista);

        auto AdicionarOpcao = [this, List, &AlvosDoVeu](const FString& Rotulo, const FName Id,
                                                        const ERunnerOptionAction Acao, const FString& CharacterId)
        {
            // Cada linha e uma caixa: o botao grande (jogar/escolher) e, quando cabe, o de apagar.
            UHorizontalBox* LinhaDaOpcao = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

            UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
            EstilizarBotao(Button, RunnerPalette::LinhaDeOpcao(), RunnerPalette::LinhaDeOpcaoHover(),
                           RunnerPalette::BordaCampo(), 1.f);
            AlvosDoVeu.Add(Button);

            UTextBlock* Text = CriarTexto(WidgetTree, Rotulo, 13.f, RunnerPalette::TextoCorpo());
            Text->SetAutoWrapText(false);
            Button->AddChild(Text);

            URunnerOptionButton* Binding = NewObject<URunnerOptionButton>(this);
            Binding->OptionId = Id;
            Binding->Owner = this;
            Binding->Action = Acao;
            Binding->CharacterId = CharacterId;
            OptionBindings.Add(Binding);
            Button->OnClicked.AddDynamic(Binding, &URunnerOptionButton::HandleClicked);
            Button->OnHovered.AddDynamic(Binding, &URunnerOptionButton::HandleHovered);

            if (UHorizontalBoxSlot* SlotDaLinha = Cast<UHorizontalBoxSlot>(LinhaDaOpcao->AddChild(Button)))
            {
                SlotDaLinha->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            }

            // Apagar so faz sentido para personagem que ja existe.
            if (Acao == ERunnerOptionAction::Jogar && !CharacterId.IsEmpty())
            {
                UButton* BotaoApagar = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
                EstilizarBotao(BotaoApagar, RunnerPalette::CobreOxidado(0.35f), RunnerPalette::Cobre(0.6f),
                               RunnerPalette::Cobre(0.7f), 1.f);
                BotaoApagar->AddChild(CriarTexto(WidgetTree, TEXT("Excluir"), 12.f, RunnerPalette::Cobre(0.95f)));

                URunnerOptionButton* BindingApagar = NewObject<URunnerOptionButton>(this);
                BindingApagar->OptionId = Id;
                BindingApagar->Owner = this;
                BindingApagar->Action = ERunnerOptionAction::Excluir;
                BindingApagar->CharacterId = CharacterId;
                OptionBindings.Add(BindingApagar);
                BotaoApagar->OnClicked.AddDynamic(BindingApagar, &URunnerOptionButton::HandleClicked);

                // Largura fixa: sem isto o texto do Runner empurrava o botao para fora da linha.
                USizeBox* LarguraDoApagar = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
                LarguraDoApagar->SetWidthOverride(86.f);
                LarguraDoApagar->AddChild(BotaoApagar);
                if (UHorizontalBoxSlot* SlotDoApagar = Cast<UHorizontalBoxSlot>(LinhaDaOpcao->AddChild(LarguraDoApagar)))
                {
                    SlotDoApagar->SetPadding(FMargin(8.f, 0.f, 0.f, 0.f));
                    SlotDoApagar->SetVerticalAlignment(VAlign_Fill);
                }
                AlvosDoVeu.Add(BotaoApagar);
            }

            List->AddChild(LinhaDaOpcao);
        };

        if (CurrentStep == ERunnerMenuStep::CharacterSelect)
        {
            // Personagens que ja existem nesta conta: clicar entra, o botao ao lado apaga.
            if (Session)
            {
                for (const FRunnerSavedCharacter& Salvo : Session->GetSavedCharacters())
                {
                    // O rotulo e o nome do Runner: chassi e classe ficam na ficha, ao lado.
                    AdicionarOpcao(Salvo.GetDisplayName() + TEXT("   [") + Salvo.CharacterId + TEXT("]"),
                                   FName(*Salvo.CharacterId), ERunnerOptionAction::Jogar, Salvo.CharacterId);
                }
            }
        }
        else
        {
            const TArray<FName> Ids = (CurrentStep == ERunnerMenuStep::Chassis)
                ? (Session ? Session->GetChassisIds(false) : TArray<FName>())
                : (Session ? Session->GetClassIds() : TArray<FName>());

            for (const FName& Id : Ids)
            {
                const FString Label = (CurrentStep == ERunnerMenuStep::Chassis)
                    ? Id.ToString() + ((Id == PendingChassis) ? TEXT("  [escolhido]") : TEXT(""))
                    : Id.ToString() + ((Id == PendingClass) ? TEXT("  [escolhido]") : TEXT(""));
                AdicionarOpcao(Label, Id, ERunnerOptionAction::Jogar, FString());
            }
        }
    }

    // Nome do personagem: e por ele que o jogador reconhece o Runner na lista depois.
    if (CurrentStep == ERunnerMenuStep::Class)
    {
        Adicionar(RootBox, CriarTexto(WidgetTree, TEXT("NOME DO RUNNER"), 11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 4.f);

        NameBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        NameBox->SetHintText(FText::FromString(TEXT("nome do personagem (3 a 16 caracteres)")));
        NameBox->SetText(FText::FromString(Session ? Session->GetPendingCharacterName() : FString()));
        EstilizarCampo(NameBox);
        Adicionar(RootBox, NameBox, 12.f);
        AlvosDoVeu.Add(NameBox);
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
                   RunnerPalette::LuzDeRua(0.9f), 1.f);
    UTextBlock* PrimaryText = CriarTexto(WidgetTree, FString(), 15.f, RunnerPalette::TextoDoBotao());
    PrimaryText->SetAutoWrapText(false);
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            PrimaryText->SetText(FText::FromString(bEOS ? TEXT("Entrar com o EOS (Dev Auth Tool)") : TEXT("Entrar (local)")));
            break;
        case ERunnerMenuStep::CreateAccount:     PrimaryText->SetText(FText::FromString(TEXT("Criar conta"))); break;
        case ERunnerMenuStep::CharacterSelect:   PrimaryText->SetText(FText::FromString(TEXT("Criar novo Runner"))); break;
        case ERunnerMenuStep::Class:             PrimaryText->SetText(FText::FromString(TEXT("Criar personagem"))); break;
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
                   RunnerPalette::LataoEscovado(0.8f), 1.f);
    UTextBlock* SecondaryText = CriarTexto(WidgetTree, FString(), 14.f, RunnerPalette::TextoCorpo());
    SecondaryText->SetAutoWrapText(false);
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            SecondaryText->SetText(FText::FromString(bEOS ? TEXT("Entrar com a conta Epic (EOS)") : TEXT("Criar conta nova")));
            break;
        case ERunnerMenuStep::CreateAccount:   SecondaryText->SetText(FText::FromString(TEXT("Voltar"))); break;
        case ERunnerMenuStep::CharacterSelect: SecondaryText->SetText(FText::FromString(TEXT("Sair da conta"))); break;
        case ERunnerMenuStep::Chassis:         SecondaryText->SetText(FText::FromString(TEXT("Voltar"))); break;
        case ERunnerMenuStep::Class:           SecondaryText->SetText(FText::FromString(TEXT("Trocar chassi"))); break;
        case ERunnerMenuStep::Confirm:         SecondaryText->SetText(FText::FromString(TEXT("Cancelar"))); break;
    }
    Secondary->AddChild(SecondaryText);
    Secondary->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnSecondaryClicked);
    Adicionar(RootBox, Secondary, 8.f);
    AlvosDoVeu.Add(Secondary);

    // Terceiro botao: so aparece onde ha uma terceira acao util.
    TertiaryButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    EstilizarBotao(TertiaryButton, RunnerPalette::BotaoTerciario(), RunnerPalette::BotaoTerciarioHover(),
                   RunnerPalette::Cobre(0.7f), 1.f);
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
        TertiaryText->SetText(FText::FromString(TEXT("Atualizar lista")));
        bMostraTerceiro = true;
    }
    TertiaryButton->AddChild(TertiaryText);
    TertiaryButton->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnTertiaryClicked);
    TertiaryButton->SetVisibility(bMostraTerceiro ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    Adicionar(RootBox, TertiaryButton, 4.f);
    AlvosDoVeu.Add(TertiaryButton);

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
            SetStatus(TEXT("Escolha um dos seus Runners, ou crie um novo."));
            break;
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
            GoToStep(ERunnerMenuStep::Chassis);
            break;

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
        case ERunnerMenuStep::Chassis:
            GoToStep(ERunnerMenuStep::Login);
            break;

        case ERunnerMenuStep::Class:
            GoToStep(ERunnerMenuStep::Chassis);
            break;

        case ERunnerMenuStep::CharacterSelect:
            if (URunnerSession* Session = GetSession())
            {
                Session->Logout();
            }
            GoToStep(ERunnerMenuStep::Login);
            break;

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
            RebuildLayout();
            SetStatus(TEXT("Lista de Runners atualizada."));
            break;

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

    UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)), true,
        TEXT("game=/Script/PloidrekRPG.AFGameMode"));
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

void URunnerMenuWidget::PreencherInfo(UVerticalBox* Caixa, FName ChassiId, FName ClasseId)
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
        Adicionar(Caixa, CriarTexto(WidgetTree, TEXT("Passe o mouse numa opcao da lista para ver o corpo, os atributos e as vantagens."),
            12.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 0.f);
        return;
    }

    Adicionar(Caixa, CriarTexto(WidgetTree, Chassi.DisplayName.ToString().ToUpper(), 17.f, RunnerPalette::LataoPolido()), 2.f);
    Adicionar(Caixa, CriarTexto(WidgetTree,
        FString::Printf(TEXT("corpo %s · tinta %d,%d,%d"), *NomeDoCorpo(Chassi.Body),
            FMath::RoundToInt(Chassi.AccentColor.R * 255.f), FMath::RoundToInt(Chassi.AccentColor.G * 255.f),
            FMath::RoundToInt(Chassi.AccentColor.B * 255.f)),
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

    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    Vitrine->SetPreview(Dados.Mesh, Dados.AccentColor, Ajustes ? Ajustes->IdleAnim.LoadSynchronous() : nullptr);
    ChassiNaVitrine = ChassiId;
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
                PreencherInfo(InfoBox, Salvo.ChassisId, Salvo.ClassId);
                return;
            }
        }
        return;
    }

    // Passo da classe: o corpo continua o chassi escolhido; a ficha mostra a classe sob o mouse.
    if (CurrentStep == ERunnerMenuStep::Class)
    {
        MostrarNaVitrine(Session->GetSelectedChassis());
        PreencherInfo(InfoBox, Session->GetSelectedChassis(), OptionId);
        return;
    }

    // Passo do chassi: corpo e ficha do que esta sob o mouse.
    MostrarNaVitrine(OptionId);
    PreencherInfo(InfoBox, OptionId, Session->GetSelectedClass());
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

