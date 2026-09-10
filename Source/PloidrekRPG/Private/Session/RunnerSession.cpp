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
    constexpr int32 MinUserNameLength = 3;
    constexpr int32 MinPasswordLength = 8;
    constexpr int32 MinCharacterNameLength = 3;
    constexpr int32 MaxCharacterNameLength = 16;
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

    // Contas: "email<TAB>hash<TAB>nome de usuario". Contas antigas tinham so duas colunas
    // (o nome fazia as vezes de e-mail) e continuam sendo lidas.
    TArray<FString> Lines;
    if (FFileHelper::LoadFileToStringArray(Lines, *(FPaths::ProjectSavedDir() / AccountsFileName)))
    {
        for (const FString& Line : Lines)
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT("\t"), true);
            if (Parts.Num() >= 2 && !Parts[0].IsEmpty() && !Parts[1].IsEmpty())
            {
                FRunnerLocalAccount Conta;
                Conta.Email = Parts[0];
                Conta.PasswordHash = Parts[1];
                Conta.UserName = Parts.IsValidIndex(2) && !Parts[2].IsEmpty() ? Parts[2] : Parts[0];
                Accounts.Add(Conta.Email.ToLower(), Conta);
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
            if (Parts.Num() >= 4 && !Parts[0].IsEmpty() && !Parts[1].IsEmpty())
            {
                FRunnerSavedCharacter Salvo;
                Salvo.AccountName = Parts[0];
                Salvo.CharacterId = Parts[1];
                Salvo.ChassisId = FName(*Parts[2]);
                Salvo.ClassId = FName(*Parts[3]);
                // Personagens gravados antes do campo de nome entram sem nome (a lista mostra chassi + classe).
                Salvo.CharacterName = Parts.IsValidIndex(4) ? Parts[4] : FString();
                SavedCharacters.Add(Salvo);
            }
        }
    }
}

bool URunnerSession::SaveAccounts() const
{
    TArray<FString> Lines;
    for (const TPair<FString, FRunnerLocalAccount>& Pair : Accounts)
    {
        Lines.Add(Pair.Value.Email + TEXT("\t") + Pair.Value.PasswordHash + TEXT("\t") + Pair.Value.UserName);
    }
    return FFileHelper::SaveStringArrayToFile(Lines, *(FPaths::ProjectSavedDir() / AccountsFileName));
}

bool URunnerSession::SaveCharacters() const
{
    TArray<FString> Lines;
    for (const FRunnerSavedCharacter& Salvo : SavedCharacters)
    {
        Lines.Add(Salvo.AccountName + TEXT("\t") + Salvo.CharacterId + TEXT("\t")
            + Salvo.ChassisId.ToString() + TEXT("\t") + Salvo.ClassId.ToString()
            + TEXT("\t") + Salvo.CharacterName);
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

bool URunnerSession::ValidateNewAccount(const FString& Email, const FString& UserName,
                                        const FString& Password, const FString& ConfirmPassword,
                                        FString& OutError)
{
    const FString Mail = Email.TrimStartAndEnd();
    const FString Nome = UserName.TrimStartAndEnd();

    // E-mail
    if (Mail.IsEmpty())
    {
        OutError = TEXT("Informe o e-mail.");
        return false;
    }
    if (Mail.Contains(TEXT(" ")))
    {
        OutError = TEXT("O e-mail nao pode ter espacos.");
        return false;
    }
    FString AntesDoArroba;
    FString DepoisDoArroba;
    if (!Mail.Split(TEXT("@"), &AntesDoArroba, &DepoisDoArroba) || AntesDoArroba.IsEmpty() || DepoisDoArroba.IsEmpty())
    {
        OutError = TEXT("Informe um e-mail valido (ex.: nome@dominio.com).");
        return false;
    }
    if (!DepoisDoArroba.Contains(TEXT(".")) || DepoisDoArroba.StartsWith(TEXT(".")) || DepoisDoArroba.EndsWith(TEXT(".")))
    {
        OutError = TEXT("O e-mail precisa de um dominio com ponto (ex.: nome@dominio.com).");
        return false;
    }

    // Nome de usuario
    if (Nome.Len() < MinUserNameLength)
    {
        OutError = FString::Printf(TEXT("O nome de usuario precisa de pelo menos %d caracteres."), MinUserNameLength);
        return false;
    }
    for (const TCHAR Caractere : Nome)
    {
        if (!FChar::IsAlnum(Caractere) && Caractere != TEXT('_') && Caractere != TEXT('-'))
        {
            OutError = TEXT("No nome de usuario use apenas letras, numeros, _ ou -.");
            return false;
        }
    }

    // Senha: 8+ com maiuscula, minuscula e caractere especial.
    if (Password.Len() < MinPasswordLength)
    {
        OutError = FString::Printf(TEXT("A senha precisa de pelo menos %d caracteres."), MinPasswordLength);
        return false;
    }
    bool bTemMaiuscula = false;
    bool bTemMinuscula = false;
    bool bTemEspecial = false;
    for (const TCHAR Caractere : Password)
    {
        if (FChar::IsUpper(Caractere))      { bTemMaiuscula = true; }
        else if (FChar::IsLower(Caractere)) { bTemMinuscula = true; }
        else if (!FChar::IsAlnum(Caractere)) { bTemEspecial = true; }
    }
    if (!bTemMaiuscula)
    {
        OutError = TEXT("A senha precisa de pelo menos uma letra maiuscula.");
        return false;
    }
    if (!bTemMinuscula)
    {
        OutError = TEXT("A senha precisa de pelo menos uma letra minuscula.");
        return false;
    }
    if (!bTemEspecial)
    {
        OutError = TEXT("A senha precisa de pelo menos um caractere especial (ex.: ! @ # $ %).");
        return false;
    }

    // Confirmacao
    if (Password != ConfirmPassword)
    {
        OutError = TEXT("A confirmacao precisa ser igual a senha.");
        return false;
    }

    OutError.Reset();
    return true;
}

bool URunnerSession::CreateAccount(const FString& Email, const FString& UserName, const FString& Password, FString& OutError)
{
    LoadAccountsIfNeeded();

    // A tela ja valida com o campo de confirmacao; aqui a confirmacao e a propria senha.
    if (!ValidateNewAccount(Email, UserName, Password, Password, OutError))
    {
        return false;
    }

    const FString Chave = Email.TrimStartAndEnd().ToLower();
    const FString Nome = UserName.TrimStartAndEnd();

    if (Accounts.Contains(Chave))
    {
        OutError = TEXT("Esse e-mail ja tem conta.");
        return false;
    }
    for (const TPair<FString, FRunnerLocalAccount>& Par : Accounts)
    {
        if (Par.Value.UserName.Equals(Nome, ESearchCase::IgnoreCase))
        {
            OutError = TEXT("Esse nome de usuario ja esta em uso.");
            return false;
        }
    }

    FRunnerLocalAccount Conta;
    Conta.Email = Email.TrimStartAndEnd();
    Conta.UserName = Nome;
    Conta.PasswordHash = HashPassword(Password);
    Accounts.Add(Chave, Conta);

    if (!SaveAccounts())
    {
        OutError = TEXT("Nao foi possivel gravar a conta.");
        return false;
    }

    OutError.Reset();
    return true;
}

bool URunnerSession::Login(const FString& Email, const FString& Password, FString& OutError)
{
    LoadAccountsIfNeeded();

    const FString Chave = Email.TrimStartAndEnd().ToLower();
    const FRunnerLocalAccount* Conta = Accounts.Find(Chave);
    if (!Conta)
    {
        OutError = TEXT("Conta nao encontrada (use o e-mail cadastrado).");
        return false;
    }
    if (Conta->PasswordHash != HashPassword(Password))
    {
        OutError = TEXT("Senha incorreta.");
        return false;
    }

    CompleteLogin(Chave, Conta->UserName);
    OutError.Reset();
    return true;
}

void URunnerSession::CompleteLogin(const FString& Key, const FString& DisplayName)
{
    AccountKey = Key;
    AccountName = DisplayName;
    AccountEmail = Accounts.Contains(Key) ? Accounts[Key].Email : FString();
    bLoggedIn = true;
    OnLoginComplete.Broadcast(true, FString());
}

void URunnerSession::Logout()
{
    LogoutEOS();

    bLoggedIn = false;
    AccountName.Reset();
    AccountEmail.Reset();
    AccountKey.Reset();
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
    const FString Tipo = AuthType.IsEmpty() ? TEXT("accountportal") : AuthType;

    // O Dev Auth Tool recebe o host:porta em "Id" e o NOME da credencial em "Token". Faltando
    // qualquer um dos dois o SDK responde EOS_InvalidParameters com
    // "EOS_Auth_Credentials.Id reason: must not be null or empty" — que nao diz nada ao jogador.
    // Entao a checagem mora aqui, com o texto que explica o que digitar.
    if (Tipo.Equals(TEXT("developer"), ESearchCase::IgnoreCase)
        && (Id.TrimStartAndEnd().IsEmpty() || Token.TrimStartAndEnd().IsEmpty()))
    {
        OutError = TEXT("Dev Auth: campo 1 = host:porta do tool (ex.: localhost:8081); campo 2 = o nome da credencial.");
        OnLoginComplete.Broadcast(false, OutError);
        return false;
    }

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
    Credentials.Type = Tipo;
    Credentials.Id = Id.TrimStartAndEnd();
    Credentials.Token = Token.TrimStartAndEnd();

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
    CompleteLogin(NomeDaConta, NomeDaConta);
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
        if (Salvo.AccountName == AccountKey)
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
        if (Salvo.AccountName == AccountKey)
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
        if (Salvo.AccountName == AccountKey && Salvo.CharacterId == CharacterId)
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

    Perfil.CharacterName = Salvo->GetDisplayName();
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

bool URunnerSession::ValidateCharacterName(const FString& Name, FString& OutError)
{
    const FString Nome = Name.TrimStartAndEnd();

    if (Nome.IsEmpty())
    {
        OutError = TEXT("De um nome ao seu Runner.");
        return false;
    }
    if (Nome.Len() < MinCharacterNameLength)
    {
        OutError = FString::Printf(TEXT("O nome do Runner precisa de pelo menos %d caracteres."), MinCharacterNameLength);
        return false;
    }
    if (Nome.Len() > MaxCharacterNameLength)
    {
        OutError = FString::Printf(TEXT("O nome do Runner pode ter no maximo %d caracteres."), MaxCharacterNameLength);
        return false;
    }
    for (const TCHAR Caractere : Nome)
    {
        if (!FChar::IsAlnum(Caractere) && Caractere != TEXT(' ') && Caractere != TEXT('_') && Caractere != TEXT('-'))
        {
            OutError = TEXT("No nome do Runner use letras, numeros, espaco, _ ou -.");
            return false;
        }
    }

    OutError.Reset();
    return true;
}

bool URunnerSession::IsCharacterNameTaken(const FString& Name) const
{
    const FString Nome = Name.TrimStartAndEnd();
    for (const FRunnerSavedCharacter& Salvo : SavedCharacters)
    {
        if (Salvo.AccountName == AccountKey && Salvo.CharacterName.Equals(Nome, ESearchCase::IgnoreCase))
        {
            return true;
        }
    }
    return false;
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

    // O nome e obrigatorio: e por ele que o jogador reconhece o personagem na lista.
    if (!ValidateCharacterName(PendingCharacterName, OutError))
    {
        return false;
    }

    LoadAccountsIfNeeded();
    if (GetSavedCharacters().Num() >= MaxCharacterSlots)
    {
        OutError = FString::Printf(TEXT("A conta ja tem %d personagens (limite)."), MaxCharacterSlots);
        return false;
    }
    if (IsCharacterNameTaken(PendingCharacterName))
    {
        OutError = FString::Printf(TEXT("Ja existe um Runner chamado '%s' nesta conta."), *PendingCharacterName.TrimStartAndEnd());
        return false;
    }

    if (!URunnerCharacterFactory::BuildProfile(SelectedChassis, SelectedClass, AccountName,
                                               GetChassisTable(), GetClassTable(), ActiveProfile, OutError))
    {
        return false;
    }

    ActiveProfile.CharacterName = PendingCharacterName.TrimStartAndEnd();

    bHasCharacter = true;

    // Guarda na conta como um personagem novo (a lista que aparece no proximo login).
    FRunnerSavedCharacter Novo;
    Novo.AccountName = AccountKey;
    Novo.CharacterId = FString::Printf(TEXT("R-%02d"), GetSavedCharacters().Num() + 1);
    Novo.ChassisId = SelectedChassis;
    Novo.ClassId = SelectedClass;
    Novo.CharacterName = PendingCharacterName.TrimStartAndEnd();
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
    PendingCharacterName.Reset();
}
