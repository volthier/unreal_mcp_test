#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RunnerCharacterFactory.generated.h"

class UDataTable;
struct FRunnerCharacterProfile;
struct FRunnerChassisData;
struct FRunnerClassData;

/**
 * Monta a ficha a partir do chassi + classe. Tudo estatico e sem mundo:
 * a UI so chama isto e o GameMode so usa o resultado.
 */
UCLASS()
class PLOIDREKRPG_API URunnerCharacterFactory : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Ids de chassi. bIncludeExtinct = true traz tambem os Clyffen (slot EXTINTO, nao selecionavel). */
    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    static TArray<FName> GetChassisIds(const UDataTable* ChassisTable, bool bIncludeExtinct);

    /** Ids de classe. */
    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    static TArray<FName> GetClassIds(const UDataTable* ClassTable);

    /** Um chassi extinto nunca pode ser escolhido, mesmo que o id venha da UI. */
    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    static bool IsChassisSelectable(const UDataTable* ChassisTable, FName ChassisId);

    /**
     * Ficha final. Falha (com motivo em OutError) se o chassi/classe nao existir,
     * se o chassi estiver extinto ou se alguma tabela estiver vazia.
     */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    static bool BuildProfile(FName ChassisId, FName ClassId, const FString& AccountName,
                             const UDataTable* ChassisTable, const UDataTable* ClassTable,
                             FRunnerCharacterProfile& OutProfile, FString& OutError);
};
