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
#include "Components/Spacer.h"
#include "Components/Widget.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerGameSettings.h"
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
    // O tipo do id decide se ele e chassi ou classe conforme o passo atual.
    Owner->SelectChassis(OptionId);
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
    PanelSlot->SetSize(FVector2D(680.f, 620.f));

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
    }
    Adicionar(RootBox, CriarTexto(WidgetTree, Passo, 11.f, RunnerPalette::TextoFraco(), TEXT("Regular")), 10.f);
    Adicionar(RootBox, CriarFilete(WidgetTree, RunnerPalette::LataoEscovado(0.75f), 1.5f), 14.f);

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
        Adicionar(RootBox, List, 12.f);

        auto AdicionarOpcao = [this, List, &AlvosDoVeu](const FString& Rotulo, const FName Id)
        {
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
            OptionBindings.Add(Binding);
            Button->OnClicked.AddDynamic(Binding, &URunnerOptionButton::HandleClicked);

            List->AddChild(Button);
        };

        if (CurrentStep == ERunnerMenuStep::CharacterSelect)
        {
            // Personagens que ja existem nesta conta.
            if (Session)
            {
                for (const FRunnerSavedCharacter& Salvo : Session->GetSavedCharacters())
                {
                    AdicionarOpcao(Salvo.GetDisplayName() + TEXT("   [") + Salvo.CharacterId + TEXT("]"),
                        FName(*Salvo.CharacterId));
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
                AdicionarOpcao(Label, Id);
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
            const FString Confirmacao = ConfirmBox->GetText().ToString();

            // A regra mora na sessao (e o teste cobra ela); a tela so mostra o que voltou.
            if (!URunnerSession::ValidateNewAccount(Email, NomeDeUsuario, Senha, Confirmacao, Erro))
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

            FRunnerCharacterProfile Perfil;
            if (!Session->CreateCharacterProfile(Perfil, Erro))
            {
                SetStatus(Erro);
                return;
            }

            SetStatus(FString::Printf(TEXT("Runner %s criado: HP %d, CA %d. Entrando..."),
                *Perfil.ChassisId.ToString(), Perfil.MaxHitPoints, Perfil.ArmorClass));

            // Troca para o GameMode do jogo: o pawn nasce e recebe o corpo do chassi.
            UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)), true,
                TEXT("game=/Script/PloidrekRPG.AFGameMode"));
            break;
        }

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
    UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)), true,
        TEXT("game=/Script/PloidrekRPG.AFGameMode"));
}
