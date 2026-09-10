#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/RunnerTypes.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerClassData.h"
#include "RunnerSession.generated.h"

class UDataTable;

/**
 * A maquina de estado da sessao: conta -> chassi -> classe -> ficha.
 * E um UObject simples de proposito: nenhuma dependencia de mundo ou de GameInstance,
 * entao o fluxo inteiro pode ser exercitado por teste de automacao.
 *
 * Login local (dev-grade): contas em Saved/RunnerAccounts.tsv (senha com hash MD5)
 * e fichas em Saved/RunnerCharacters.tsv. Trocar por EOS depois afeta so estas funcoes.
 */
UCLASS()
class PLOIDREKRPG_API URunnerSession : public UObject
{
    GENERATED_BODY()

public:
    // ---------- Conta ----------
    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    bool CreateAccount(const FString& Account, const FString& Password, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    bool Login(const FString& Account, const FString& Password, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    void Logout();

    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    bool IsLoggedIn() const { return bLoggedIn; }

    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    FString GetAccountName() const { return AccountName; }

    // ---------- Criacao ----------
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool SelectChassis(FName ChassisId, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool SelectClass(FName ClassId, FString& OutError);

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    FName GetSelectedChassis() const { return SelectedChassis; }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    FName GetSelectedClass() const { return SelectedClass; }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool IsSelectionComplete() const;

    /** Monta a ficha final, guarda como ficha ativa e grava na conta. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool CreateCharacterProfile(FRunnerCharacterProfile& OutProfile, FString& OutError);

    /** Restaura chassi e classe gravados para a conta logada. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool RestoreCharacter(FString& OutError);

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool HasCharacter() const { return bHasCharacter; }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool GetActiveProfile(FRunnerCharacterProfile& OutProfile) const;

    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    void ClearCharacter();

    // ---------- Dados ----------
    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    UDataTable* GetChassisTable() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    UDataTable* GetClassTable() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    TArray<FName> GetChassisIds(bool bIncludeExtinct) const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    TArray<FName> GetClassIds() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    bool GetChassisData(FName ChassisId, FRunnerChassisData& OutChassis) const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    bool GetClassData(FName ClassId, FRunnerClassData& OutClass) const;

private:
    void LoadAccountsIfNeeded();
    bool SaveAccounts() const;
    bool SaveCharacters() const;
    static FString HashPassword(const FString& Password);

    UPROPERTY() TMap<FString, FString> Accounts;
    /** conta -> "chassi\tclasse" */
    UPROPERTY() TMap<FString, FString> Characters;
    FRunnerCharacterProfile ActiveProfile;
    bool bHasCharacter = false;
    bool bAccountsLoaded = false;
    bool bLoggedIn = false;
    FString AccountName;
    FName SelectedChassis;
    FName SelectedClass;
};
