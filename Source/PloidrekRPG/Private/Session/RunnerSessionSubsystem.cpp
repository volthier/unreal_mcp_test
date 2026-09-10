#include "Session/RunnerSessionSubsystem.h"

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
}

void URunnerSessionSubsystem::LoadAccountsIfNeeded()
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
}

bool URunnerSessionSubsystem::SaveAccounts() const
{
    TArray<FString> Lines;
    for (const TPair<FString, FString>& Pair : Accounts)
    {
        Lines.Add(Pair.Key + TEXT("\t") + Pair.Value);
    }
    const FString Path = FPaths::ProjectSavedDir() / AccountsFileName;
    return FFileHelper::SaveStringArrayToFile(Lines, *Path);
}

FString URunnerSessionSubsystem::HashPassword(const FString& Password)
{
    // Dev-grade: hash simples so para nao guardar a senha em texto. Producao usa servico de identidade.
    return FMD5::HashAnsiString(*Password);
}

bool URunnerSessionSubsystem::CreateAccount(const FString& Account, const FString& Password, FString& OutError)
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

bool URunnerSessionSubsystem::Login(const FString& Account, const FString& Password, FString& OutError)
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

void URunnerSessionSubsystem::Logout()
{
    bLoggedIn = false;
    AccountName.Reset();
    SelectedChassis = NAME_None;
    SelectedClass = NAME_None;
}

UDataTable* URunnerSessionSubsystem::GetChassisTable() const
{
    const URunnerGameSettings* Settings = GetDefault<URunnerGameSettings>();
    return Settings ? Settings->ChassisTable.LoadSynchronous() : nullptr;
}

UDataTable* URunnerSessionSubsystem::GetClassTable() const
{
    const URunnerGameSettings* Settings = GetDefault<URunnerGameSettings>();
    return Settings ? Settings->ClassTable.LoadSynchronous() : nullptr;
}

TArray<FName> URunnerSessionSubsystem::GetChassisIds(const bool bIncludeExtinct) const
{
    return URunnerCharacterFactory::GetChassisIds(GetChassisTable(), bIncludeExtinct);
}

TArray<FName> URunnerSessionSubsystem::GetClassIds() const
{
    return URunnerCharacterFactory::GetClassIds(GetClassTable());
}

bool URunnerSessionSubsystem::GetChassisData(const FName ChassisId, FRunnerChassisData& OutChassis) const
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

bool URunnerSessionSubsystem::GetClassData(const FName ClassId, FRunnerClassData& OutClass) const
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

bool URunnerSessionSubsystem::SelectChassis(const FName ChassisId, FString& OutError)
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

bool URunnerSessionSubsystem::SelectClass(const FName ClassId, FString& OutError)
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

bool URunnerSessionSubsystem::IsSelectionComplete() const
{
    return bLoggedIn && !SelectedChassis.IsNone() && !SelectedClass.IsNone();
}

bool URunnerSessionSubsystem::CreateCharacterProfile(FRunnerCharacterProfile& OutProfile, FString& OutError) const
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

    return URunnerCharacterFactory::BuildProfile(SelectedChassis, SelectedClass, AccountName,
                                                 GetChassisTable(), GetClassTable(), OutProfile, OutError);
}
