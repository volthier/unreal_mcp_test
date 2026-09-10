#include "Session/RunnerSession.h"

#include "Data/RunnerCharacterFactory.h"
#include "Data/RunnerGameSettings.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"

namespace
{
    constexpr int32 MinAccountLength = 3;
    constexpr int32 MinPasswordLength = 4;
    const TCHAR* AccountsFileName = TEXT("RunnerAccounts.tsv");
    const TCHAR* CharactersFileName = TEXT("RunnerCharacters.tsv");
}

void URunnerSession::LoadAccountsIfNeeded()
{
    if (bAccountsLoaded)
    {
        return;
    }
    bAccountsLoaded = true;

    TArray<FString> Lines;
    const FString Path = FPaths::ProjectSavedDir() / AccountsFileName;
    if (!FFileHelper::LoadFileToStringArray(Lines, *Path))
    {
        return;
    }

    for (const FString& Line : Lines)
    {
        FString Account;
        FString Hash;
        if (Line.Split(TEXT("\t"), &Account, &Hash) && !Account.IsEmpty() && !Hash.IsEmpty())
        {
            Accounts.Add(Account, Hash);
        }
    }

    // Fichas ja criadas (uma por conta)
    TArray<FString> CharacterLines;
    if (FFileHelper::LoadFileToStringArray(CharacterLines, *(FPaths::ProjectSavedDir() / CharactersFileName)))
    {
        for (const FString& Line : CharacterLines)
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT("\t"), true);
            if (Parts.Num() == 3 && !Parts[0].IsEmpty())
            {
                Characters.Add(Parts[0], Parts[1] + TEXT("\t") + Parts[2]);
            }
        }
    }
}

bool URunnerSession::SaveCharacters() const
{
    TArray<FString> Lines;
    for (const TPair<FString, FString>& Pair : Characters)
    {
        Lines.Add(Pair.Key + TEXT("\t") + Pair.Value);
    }
    return FFileHelper::SaveStringArrayToFile(Lines, *(FPaths::ProjectSavedDir() / CharactersFileName));
}

bool URunnerSession::SaveAccounts() const
{
    TArray<FString> Lines;
    for (const TPair<FString, FString>& Pair : Accounts)
    {
        Lines.Add(Pair.Key + TEXT("\t") + Pair.Value);
    }
    const FString Path = FPaths::ProjectSavedDir() / AccountsFileName;
    return FFileHelper::SaveStringArrayToFile(Lines, *Path);
}

FString URunnerSession::HashPassword(const FString& Password)
{
    // Dev-grade: hash simples so para nao guardar a senha em texto. Producao usa servico de identidade.
    return FMD5::HashAnsiString(*Password);
}

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

    AccountName = Trimmed;
    bLoggedIn = true;
    OutError.Reset();
    return true;
}

void URunnerSession::Logout()
{
    bLoggedIn = false;
    AccountName.Reset();
    ClearCharacter();
}

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

    if (!URunnerCharacterFactory::BuildProfile(SelectedChassis, SelectedClass, AccountName,
                                               GetChassisTable(), GetClassTable(), ActiveProfile, OutError))
    {
        return false;
    }

    bHasCharacter = true;
    Characters.Add(AccountName, SelectedChassis.ToString() + TEXT("\t") + SelectedClass.ToString());
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

    const FString* Saved = Characters.Find(AccountName);
    if (!Saved)
    {
        OutError = TEXT("Essa conta ainda nao tem personagem.");
        return false;
    }

    FString Chassi;
    FString Classe;
    if (!Saved->Split(TEXT("\t"), &Chassi, &Classe) || Chassi.IsEmpty() || Classe.IsEmpty())
    {
        OutError = TEXT("Ficha gravada em formato invalido.");
        return false;
    }

    SelectedChassis = FName(*Chassi);
    SelectedClass = FName(*Classe);

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
