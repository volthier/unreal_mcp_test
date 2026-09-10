#include "Session/RunnerSession.h"

#include "Data/RunnerCharacterFactory.h"
#include "Data/RunnerGameSettings.h"
#include "Engine/DataTable.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "OnlineSubsystem.h"

namespace
{
    constexpr int32 MinAccountLength = 3;
    constexpr int32 MinPasswordLength = 4;
    constexpr int32 MaxCharacterSlots = 8;
    const TCHAR* AccountsFileName = TEXT("RunnerAccounts.tsv");
    const TCHAR* CharactersFileName = TEXT("RunnerCharacters.tsv");
}

// ---------------------------------------------------------------------------
// Carga e gravacao
// ---------------------------------------------------------------------------

void URunnerSession::LoadAccountsIfNeeded()
{
    if (bAccountsLoaded)
    {
        return;
    }
    bAccountsLoaded = true;

    // Contas: "conta<TAB>hash"
    TArray<FString> Lines;
    if (FFileHelper::LoadFileToStringArray(Lines, *(FPaths::ProjectSavedDir() / AccountsFileName)))
    {
        for (const FString& Line : Lines)
        {
            FString Account;
            FString Hash;
            if (Line.Split(TEXT("\t"), &Account, &Hash) && !Account.IsEmpty() && !Hash.IsEmpty())
            {
                Accounts.Add(Account, Hash);
            }
        }
    }

    // Personagens: "conta<TAB>id<TAB>chassi<TAB>classe" (uma linha por personagem)
    TArray<FString> CharacterLines;
    if (FFileHelper::LoadFileToStringArray(CharacterLines, *(FPaths::ProjectSavedDir() / CharactersFileName)))
    {
        for (const FString& Line : CharacterLines)
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT("\t"), true);
            if (Parts.Num() == 4 && !Parts[0].IsEmpty() && !Parts[1].IsEmpty())
            {
                FRunnerSavedCharacter Salvo;
                Salvo.AccountName = Parts[0];
                Salvo.CharacterId = Parts[1];
                Salvo.ChassisId = FName(*Parts[2]);
                Salvo.ClassId = FName(*Parts[3]);
                SavedCharacters.Add(Salvo);
            }
        }
    }
}

bool URunnerSession::SaveAccounts() const
{
    TArray<FString> Lines;
    for (const TPair<FString, FString>& Pair : Accounts)
    {
        Lines.Add(Pair.Key + TEXT("\t") + Pair.Value);
    }
    return FFileHelper::SaveStringArrayToFile(Lines, *(FPaths::ProjectSavedDir() / AccountsFileName));
}

bool URunnerSession::SaveCharacters() const
{
    TArray<FString> Lines;
    for (const FRunnerSavedCharacter& Salvo : SavedCharacters)
    {
        Lines.Add(Salvo.AccountName + TEXT("\t") + Salvo.CharacterId + TEXT("\t")
            + Salvo.ChassisId.ToString() + TEXT("\t") + Salvo.ClassId.ToString());
    }
    return FFileHelper::SaveStringArrayToFile(Lines, *(FPaths::ProjectSavedDir() / CharactersFileName));
}

FString URunnerSession::HashPassword(const FString& Password)
{
    // Dev-grade: hash simples so para nao guardar a senha em texto. Producao usa servico de identidade.
    return FMD5::HashAnsiString(*Password);
}

// ---------------------------------------------------------------------------
// Conta local
// ---------------------------------------------------------------------------

bool URunnerSession::CreateAccount(const FString& Account, const FString& Password, FString& OutError)
{
    LoadAccountsIfNeeded();

    const FString Trimmed = Account.TrimStartAndEnd();
    if (Trimmed.Len() < MinAccountLength)
    {
        OutError = FString::Printf(TEXT("O nome precisa de pelo menos %d caracteres."), MinAccountLength);
        return false;
    }
    if (Password.Len() < MinPasswordLength)
    {
        OutError = FString::Printf(TEXT("A senha precisa de pelo menos %d caracteres."), MinPasswordLength);
        return false;
    }
    if (Accounts.Contains(Trimmed))
    {
        OutError = TEXT("Esse nome ja esta em uso.");
        return false;
    }

    Accounts.Add(Trimmed, HashPassword(Password));
    if (!SaveAccounts())
    {
        OutError = TEXT("Nao foi possivel gravar a conta.");
        return false;
    }

    OutError.Reset();
    return true;
}

bool URunnerSession::Login(const FString& Account, const FString& Password, FString& OutError)
{
    LoadAccountsIfNeeded();

    const FString Trimmed = Account.TrimStartAndEnd();
    const FString* Stored = Accounts.Find(Trimmed);
    if (!Stored)
    {
        OutError = TEXT("Conta nao encontrada.");
        return false;
    }
    if (*Stored != HashPassword(Password))
    {
        OutError = TEXT("Senha incorreta.");
        return false;
    }

    CompleteLogin(Trimmed);
    OutError.Reset();
    return true;
}

void URunnerSession::CompleteLogin(const FString& Account)
{
    AccountName = Account;
    bLoggedIn = true;
    OnLoginComplete.Broadcast(true, FString());
}

void URunnerSession::Logout()
{
    LogoutEOS();

    bLoggedIn = false;
    AccountName.Reset();
    ClearCharacter();
}

// ---------------------------------------------------------------------------
// Login de plataforma (Epic Online Services)
// ---------------------------------------------------------------------------

bool URunnerSession::IsEOSAvailable() const
{
    return IOnlineSubsystem::Get(FName(TEXT("EOS"))) != nullptr;
}

bool URunnerSession::LoginWithEOS(const FString& AuthType, const FString& Id, const FString& Token, FString& OutError)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(FName(TEXT("EOS")));
    if (!Subsystem)
    {
        OutError = TEXT("EOS nao esta disponivel (plugins OnlineSubsystemEOS e o Config sao necessarios).");
        OnLoginComplete.Broadcast(false, OutError);
        return false;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity.IsValid())
    {
        OutError = TEXT("A interface de identidade do EOS nao esta disponivel.");
        OnLoginComplete.Broadcast(false, OutError);
        return false;
    }

    FOnlineAccountCredentials Credentials;
    Credentials.Type = AuthType.IsEmpty() ? TEXT("accountportal") : AuthType;
    Credentials.Id = Id;
    Credentials.Token = Token;

    Identity->OnLoginCompleteDelegates->AddUObject(this, &URunnerSession::HandleEOSLoginComplete);

    OutError.Reset();
    if (!Identity->Login(0, Credentials))
    {
        OutError = TEXT("Nao foi possivel iniciar o login no EOS.");
        OnLoginComplete.Broadcast(false, OutError);
        return false;
    }

    // Login assincrono: o resultado chega em HandleEOSLoginComplete.
    return true;
}

void URunnerSession::HandleEOSLoginComplete(const int32 LocalUserNumber, const bool bWasSuccessful,
                                            const FUniqueNetId& UserId, const FString& Error)
{
    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(FName(TEXT("EOS"))))
    {
        if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
        {
            Identity->OnLoginCompleteDelegates->RemoveAll(this);
        }
    }

    if (!bWasSuccessful)
    {
        OnLoginComplete.Broadcast(false, Error.IsEmpty() ? TEXT("O login no EOS falhou.") : Error);
        return;
    }

    // O nome da conta EOS vira a chave dos personagens salvos.
    FString NomeDaConta;
    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(FName(TEXT("EOS"))))
    {
        if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
        {
            NomeDaConta = Identity->GetPlayerNickname(LocalUserNumber);
        }
    }
    if (NomeDaConta.IsEmpty())
    {
        NomeDaConta = UserId.ToString();
    }

    LoadAccountsIfNeeded();
    CompleteLogin(NomeDaConta);
}

void URunnerSession::LogoutEOS()
{
    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(FName(TEXT("EOS"))))
    {
        if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
        {
            Identity->Logout(0);
        }
    }
}

// ---------------------------------------------------------------------------
// Personagens da conta
// ---------------------------------------------------------------------------

TArray<FRunnerSavedCharacter> URunnerSession::GetSavedCharacters() const
{
    TArray<FRunnerSavedCharacter> Meus;
    for (const FRunnerSavedCharacter& Salvo : SavedCharacters)
    {
        if (Salvo.AccountName == AccountName)
        {
            Meus.Add(Salvo);
        }
    }

    Meus.Sort([](const FRunnerSavedCharacter& A, const FRunnerSavedCharacter& B)
    {
        return A.CharacterId < B.CharacterId;
    });
    return Meus;
}

bool URunnerSession::HasSavedCharacters() const
{
    for (const FRunnerSavedCharacter& Salvo : SavedCharacters)
    {
        if (Salvo.AccountName == AccountName)
        {
            return true;
        }
    }
    return false;
}

const FRunnerSavedCharacter* URunnerSession::FindSavedCharacter(const FString& CharacterId) const
{
    for (const FRunnerSavedCharacter& Salvo : SavedCharacters)
    {
        if (Salvo.AccountName == AccountName && Salvo.CharacterId == CharacterId)
        {
            return &Salvo;
        }
    }
    return nullptr;
}

bool URunnerSession::SelectSavedCharacter(const FString& CharacterId, FString& OutError)
{
    if (!bLoggedIn)
    {
        OutError = TEXT("Sem conta ativa.");
        return false;
    }

    const FRunnerSavedCharacter* Salvo = FindSavedCharacter(CharacterId);
    if (!Salvo)
    {
        OutError = FString::Printf(TEXT("Personagem '%s' nao encontrado nesta conta."), *CharacterId);
        return false;
    }

    SelectedChassis = Salvo->ChassisId;
    SelectedClass = Salvo->ClassId;

    FRunnerCharacterProfile Perfil;
    if (!URunnerCharacterFactory::BuildProfile(SelectedChassis, SelectedClass, AccountName,
                                               GetChassisTable(), GetClassTable(), Perfil, OutError))
    {
        return false;
    }

    ActiveProfile = Perfil;
    bHasCharacter = true;
    OutError.Reset();
    return true;
}

// ---------------------------------------------------------------------------
// Dados
// ---------------------------------------------------------------------------

UDataTable* URunnerSession::GetChassisTable() const
{
    const URunnerGameSettings* Settings = GetDefault<URunnerGameSettings>();
    return Settings ? Settings->ChassisTable.LoadSynchronous() : nullptr;
}

UDataTable* URunnerSession::GetClassTable() const
{
    const URunnerGameSettings* Settings = GetDefault<URunnerGameSettings>();
    return Settings ? Settings->ClassTable.LoadSynchronous() : nullptr;
}

TArray<FName> URunnerSession::GetChassisIds(const bool bIncludeExtinct) const
{
    return URunnerCharacterFactory::GetChassisIds(GetChassisTable(), bIncludeExtinct);
}

TArray<FName> URunnerSession::GetClassIds() const
{
    return URunnerCharacterFactory::GetClassIds(GetClassTable());
}

bool URunnerSession::GetChassisData(const FName ChassisId, FRunnerChassisData& OutChassis) const
{
    const UDataTable* Table = GetChassisTable();
    if (!Table)
    {
        return false;
    }
    const FRunnerChassisData* Row = Table->FindRow<FRunnerChassisData>(ChassisId, TEXT("GetChassisData"), false);
    if (!Row)
    {
        return false;
    }
    OutChassis = *Row;
    return true;
}

bool URunnerSession::GetClassData(const FName ClassId, FRunnerClassData& OutClass) const
{
    const UDataTable* Table = GetClassTable();
    if (!Table)
    {
        return false;
    }
    const FRunnerClassData* Row = Table->FindRow<FRunnerClassData>(ClassId, TEXT("GetClassData"), false);
    if (!Row)
    {
        return false;
    }
    OutClass = *Row;
    return true;
}

// ---------------------------------------------------------------------------
// Selecao e ficha
// ---------------------------------------------------------------------------

bool URunnerSession::SelectChassis(const FName ChassisId, FString& OutError)
{
    if (!bLoggedIn)
    {
        OutError = TEXT("Entre na conta antes de escolher o chassi.");
        return false;
    }
    if (!URunnerCharacterFactory::IsChassisSelectable(GetChassisTable(), ChassisId))
    {
        OutError = FString::Printf(TEXT("Chassi '%s' indisponivel (nao existe ou esta extinto)."), *ChassisId.ToString());
        return false;
    }

    SelectedChassis = ChassisId;
    OutError.Reset();
    return true;
}

bool URunnerSession::SelectClass(const FName ClassId, FString& OutError)
{
    if (!bLoggedIn)
    {
        OutError = TEXT("Entre na conta antes de escolher a classe.");
        return false;
    }
    if (!GetClassTable() || !GetClassTable()->GetRowMap().Contains(ClassId))
    {
        OutError = FString::Printf(TEXT("Classe '%s' nao existe."), *ClassId.ToString());
        return false;
    }

    SelectedClass = ClassId;
    OutError.Reset();
    return true;
}

bool URunnerSession::IsSelectionComplete() const
{
    return bLoggedIn && !SelectedChassis.IsNone() && !SelectedClass.IsNone();
}

bool URunnerSession::CreateCharacterProfile(FRunnerCharacterProfile& OutProfile, FString& OutError)
{
    if (!bLoggedIn)
    {
        OutError = TEXT("Sem conta ativa.");
        return false;
    }
    if (!IsSelectionComplete())
    {
        OutError = TEXT("Escolha o chassi e a classe antes de criar o personagem.");
        return false;
    }

    LoadAccountsIfNeeded();
    if (GetSavedCharacters().Num() >= MaxCharacterSlots)
    {
        OutError = FString::Printf(TEXT("A conta ja tem %d personagens (limite)."), MaxCharacterSlots);
        return false;
    }

    if (!URunnerCharacterFactory::BuildProfile(SelectedChassis, SelectedClass, AccountName,
                                               GetChassisTable(), GetClassTable(), ActiveProfile, OutError))
    {
        return false;
    }

    bHasCharacter = true;

    // Guarda na conta como um personagem novo (a lista que aparece no proximo login).
    FRunnerSavedCharacter Novo;
    Novo.AccountName = AccountName;
    Novo.CharacterId = FString::Printf(TEXT("R-%02d"), GetSavedCharacters().Num() + 1);
    Novo.ChassisId = SelectedChassis;
    Novo.ClassId = SelectedClass;
    SavedCharacters.Add(Novo);
    SaveCharacters();

    OutProfile = ActiveProfile;
    OutError.Reset();
    return true;
}

bool URunnerSession::RestoreCharacter(FString& OutError)
{
    if (!bLoggedIn)
    {
        OutError = TEXT("Sem conta ativa.");
        return false;
    }

    LoadAccountsIfNeeded();

    const TArray<FRunnerSavedCharacter> Meus = GetSavedCharacters();
    if (Meus.Num() == 0)
    {
        OutError = TEXT("Essa conta ainda nao tem personagem.");
        return false;
    }

    return SelectSavedCharacter(Meus[0].CharacterId, OutError);
}

bool URunnerSession::GetActiveProfile(FRunnerCharacterProfile& OutProfile) const
{
    if (!bHasCharacter)
    {
        return false;
    }
    OutProfile = ActiveProfile;
    return true;
}

void URunnerSession::ClearCharacter()
{
    ActiveProfile = FRunnerCharacterProfile();
    bHasCharacter = false;
    SelectedChassis = NAME_None;
    SelectedClass = NAME_None;
}
