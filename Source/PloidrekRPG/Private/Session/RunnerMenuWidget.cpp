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
        case ERunnerMenuStep::Chassis:
            Title->SetText(FText::FromString(TEXT("Escolha o chassi")));
            break;
        case ERunnerMenuStep::Class:
            Title->SetText(FText::FromString(TEXT("Escolha a classe")));
            break;
    }
    RootBox->AddChild(Title);

    if (CurrentStep == ERunnerMenuStep::Login || CurrentStep == ERunnerMenuStep::CreateAccount)
    {
        AccountBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        AccountBox->SetHintText(FText::FromString(TEXT("conta")));
        RootBox->AddChild(AccountBox);

        PasswordBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
        PasswordBox->SetHintText(FText::FromString(TEXT("senha")));
        PasswordBox->SetIsPassword(true);
        RootBox->AddChild(PasswordBox);
    }

    if (CurrentStep == ERunnerMenuStep::Chassis || CurrentStep == ERunnerMenuStep::Class)
    {
        UScrollBox* List = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
        RootBox->AddChild(List);

        // A lista sai do DataTable: a UI nao conhece os chassis, so pergunta quais existem.
        const TArray<FName> Ids = (CurrentStep == ERunnerMenuStep::Chassis)
            ? (Session ? Session->GetChassisIds(false) : TArray<FName>())
            : (Session ? Session->GetClassIds() : TArray<FName>());

        for (const FName& Id : Ids)
        {
            const FString Label = (CurrentStep == ERunnerMenuStep::Chassis)
                ? Id.ToString() + ((Id == PendingChassis) ? TEXT("  [escolhido]") : TEXT(""))
                : Id.ToString() + ((Id == PendingClass) ? TEXT("  [escolhido]") : TEXT(""));

            UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
            UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            Text->SetText(FText::FromString(Label));
            Text->SetColorAndOpacity(FSlateColor(TextColor));
            Button->AddChild(Text);

            URunnerOptionButton* Binding = NewObject<URunnerOptionButton>(this);
            Binding->OptionId = Id;
            Binding->Owner = this;
            OptionBindings.Add(Binding);
            Button->OnClicked.AddDynamic(Binding, &URunnerOptionButton::HandleClicked);

            List->AddChild(Button);
        }
    }

    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    StatusText->SetColorAndOpacity(FSlateColor(TextColor));
    RootBox->AddChild(StatusText);

    // Botao principal
    UButton* Primary = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* PrimaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    PrimaryText->SetColorAndOpacity(FSlateColor(TextColor));
    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:         PrimaryText->SetText(FText::FromString(TEXT("Entrar"))); break;
        case ERunnerMenuStep::CreateAccount: PrimaryText->SetText(FText::FromString(TEXT("Criar conta"))); break;
        case ERunnerMenuStep::Class:         PrimaryText->SetText(FText::FromString(TEXT("Criar personagem"))); break;
        default:                             PrimaryText->SetText(FText::FromString(TEXT("Escolha uma opcao acima"))); break;
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
        case ERunnerMenuStep::Login:         SecondaryText->SetText(FText::FromString(TEXT("Criar conta nova"))); break;
        case ERunnerMenuStep::CreateAccount: SecondaryText->SetText(FText::FromString(TEXT("Voltar"))); break;
        case ERunnerMenuStep::Chassis:       SecondaryText->SetText(FText::FromString(TEXT("Voltar"))); break;
        case ERunnerMenuStep::Class:         SecondaryText->SetText(FText::FromString(TEXT("Trocar chassi"))); break;
    }
    Secondary->AddChild(SecondaryText);
    Secondary->OnClicked.AddDynamic(this, &URunnerMenuWidget::OnSecondaryClicked);
    RootBox->AddChild(Secondary);
}

void URunnerMenuWidget::GoToStep(const ERunnerMenuStep NewStep)
{
    CurrentStep = NewStep;
    RebuildLayout();

    switch (CurrentStep)
    {
        case ERunnerMenuStep::Login:
            SetStatus(TEXT("Entre com a sua conta, ou crie uma."));
            break;
        case ERunnerMenuStep::CreateAccount:
            SetStatus(TEXT("Escolha um nome de conta e uma senha."));
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
            if (!Session->Login(AccountBox->GetText().ToString(), PasswordBox->GetText().ToString(), Erro))
            {
                SetStatus(Erro);
                return;
            }

            // Conta que ja tem personagem volta direto para o jogo.
            if (Session->RestoreCharacter(Erro))
            {
                FRunnerCharacterProfile Perfil;
                Session->GetActiveProfile(Perfil);
                PendingChassis = Perfil.ChassisId;
                PendingClass = Perfil.ClassId;
                SetStatus(TEXT("Bem-vindo de volta — entrando com o seu Runner."));
                UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)), true,
                    TEXT("game=/Script/PloidrekRPG.AFGameMode"));
                return;
            }

            GoToStep(ERunnerMenuStep::Chassis);
            break;
        }

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
            GoToStep(ERunnerMenuStep::CreateAccount);
            break;

        case ERunnerMenuStep::CreateAccount:
        case ERunnerMenuStep::Chassis:
            GoToStep(ERunnerMenuStep::Login);
            break;

        case ERunnerMenuStep::Class:
            GoToStep(ERunnerMenuStep::Chassis);
            break;
    }
}
