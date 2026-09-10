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
#include "Kismet/GameplayStatics.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerClassData.h"
#include "Session/RunnerSession.h"
#include "Session/RunnerSessionSubsystem.h"

namespace
{
    const FLinearColor PanelColor(0.02f, 0.03f, 0.06f, 0.94f);
    const FLinearColor TextColor(0.85f, 0.92f, 1.0f, 1.0f);
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

    UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Panel"));
    Panel->SetBrushColor(PanelColor);
    UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(Panel);
    PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));
    PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    PanelSlot->SetSize(FVector2D(620.f, 560.f));

    RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
    Panel->AddChild(RootBox);

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
    StatusText = nullptr;
    AccountBox = nullptr;
    PasswordBox = nullptr;

    URunnerSession* Session = GetSession();

    UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Title->SetColorAndOpacity(FSlateColor(TextColor));
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            Title->SetText(FText::FromString(TEXT("PLOIDREKRPG — entrar")));
            break;
        case ERunnerMenuStep::CreateAccount:
            Title->SetText(FText::FromString(TEXT("Criar conta")));
            break;
        case ERunnerMenuStep::CharacterSelect:
            Title->SetText(FText::FromString(TEXT("Seus Runners")));
            break;
        case ERunnerMenuStep::Chassis:
            Title->SetText(FText::FromString(TEXT("Escolha o chassi")));
            break;
        case ERunnerMenuStep::Class:
            Title->SetText(FText::FromString(TEXT("Escolha a classe")));
            break;
    }
    RootBox->AddChild(Title);

    // Com EOS ativo, os dois campos sao os do Dev Auth Tool: campo 1 = host:porta onde o tool
    // esta ouvindo, campo 2 = o NOME da credencial criada nele (nao e senha). Sem EOS, sao a
    // conta e a senha locais.
    const bool bCamposDoDevAuth = CurrentStep == ERunnerMenuStep::Login && Session && Session->IsEOSAvailable();

    if (CurrentStep == ERunnerMenuStep::Login || CurrentStep == ERunnerMenuStep::CreateAccount)
    {
        if (bCamposDoDevAuth)
        {
            UTextBlock* AjudaEOS = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            AjudaEOS->SetColorAndOpacity(FSlateColor(TextColor));
            AjudaEOS->SetAutoWrapText(true);
            AjudaEOS->SetText(FText::FromString(TEXT("Dev Auth Tool: campo 1 = host:porta do tool (ex.: localhost:6547); campo 2 = o nome que voce deu a credencial. O tool precisa estar rodando.")));
            RootBox->AddChild(AjudaEOS);
        }

        AccountBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        AccountBox->SetHintText(FText::FromString(bCamposDoDevAuth ? TEXT("localhost:6547") : TEXT("conta")));
        RootBox->AddChild(AccountBox);

        PasswordBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        PasswordBox->SetHintText(FText::FromString(bCamposDoDevAuth ? TEXT("nome da credencial") : TEXT("senha")));
        PasswordBox->SetIsPassword(!bCamposDoDevAuth);
        RootBox->AddChild(PasswordBox);
    }

    // Listas de opcao. Os ids vem sempre dos dados (DataTable) ou dos personagens da conta.
    if (CurrentStep == ERunnerMenuStep::Chassis || CurrentStep == ERunnerMenuStep::Class
        || CurrentStep == ERunnerMenuStep::CharacterSelect)
    {
        UScrollBox* List = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
        RootBox->AddChild(List);

        auto AdicionarOpcao = [this, List](const FString& Rotulo, const FName Id)
        {
            UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
            UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            Text->SetText(FText::FromString(Rotulo));
            Text->SetColorAndOpacity(FSlateColor(TextColor));
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

    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    StatusText->SetColorAndOpacity(FSlateColor(TextColor));
    RootBox->AddChild(StatusText);

    const bool bEOS = Session && Session->IsEOSAvailable();

    // Botao principal
    UButton* Primary = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* PrimaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    PrimaryText->SetColorAndOpacity(FSlateColor(TextColor));
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
    RootBox->AddChild(Primary);

    // Botao secundario
    UButton* Secondary = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* SecondaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    SecondaryText->SetColorAndOpacity(FSlateColor(TextColor));
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
    RootBox->AddChild(Secondary);

    // Terceiro botao: so aparece onde ha uma terceira acao util.
    TertiaryButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* TertiaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    TertiaryText->SetColorAndOpacity(FSlateColor(TextColor));
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
    RootBox->AddChild(TertiaryButton);
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
            if (!AccountBox || !PasswordBox)
            {
                return;
            }
            if (!Session->CreateAccount(AccountBox->GetText().ToString(), PasswordBox->GetText().ToString(), Erro))
            {
                SetStatus(Erro);
                return;
            }

            Session->Login(AccountBox->GetText().ToString(), PasswordBox->GetText().ToString(), Erro);
            GoToStep(ERunnerMenuStep::Chassis);
            break;
        }

        case ERunnerMenuStep::Class:
        {
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
