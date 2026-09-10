#include "Data/RunnerCharacterFactory.h"

#include "Data/RunnerChassisData.h"
#include "Data/RunnerClassData.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerRules.h"
#include "Engine/DataTable.h"

TArray<FName> URunnerCharacterFactory::GetChassisIds(const UDataTable* ChassisTable, const bool bIncludeExtinct)
{
    TArray<FName> Ids;
    if (!ChassisTable)
    {
        return Ids;
    }

    for (const TPair<FName, uint8*>& Row : ChassisTable->GetRowMap())
    {
        const FRunnerChassisData* Chassis = reinterpret_cast<const FRunnerChassisData*>(Row.Value);
        if (!Chassis)
        {
            continue;
        }
        if (Chassis->bExtinct && !bIncludeExtinct)
        {
            continue;
        }
        Ids.Add(Row.Key);
    }

    Ids.Sort(FNameLexicalLess());
    return Ids;
}

TArray<FName> URunnerCharacterFactory::GetClassIds(const UDataTable* ClassTable)
{
    TArray<FName> Ids;
    if (!ClassTable)
    {
        return Ids;
    }

    for (const TPair<FName, uint8*>& Row : ClassTable->GetRowMap())
    {
        Ids.Add(Row.Key);
    }

    Ids.Sort(FNameLexicalLess());
    return Ids;
}

bool URunnerCharacterFactory::IsChassisSelectable(const UDataTable* ChassisTable, const FName ChassisId)
{
    if (!ChassisTable)
    {
        return false;
    }
    const FRunnerChassisData* Chassis = ChassisTable->FindRow<FRunnerChassisData>(ChassisId, TEXT("IsChassisSelectable"), false);
    return Chassis != nullptr && !Chassis->bExtinct;
}

bool URunnerCharacterFactory::BuildProfile(const FName ChassisId, const FName ClassId, const FString& AccountName,
                                           const UDataTable* ChassisTable, const UDataTable* ClassTable,
                                           FRunnerCharacterProfile& OutProfile, FString& OutError)
{
    OutProfile = FRunnerCharacterProfile();

    if (!ChassisTable || !ClassTable)
    {
        OutError = TEXT("Tabela de chassi ou de classe nao configurada.");
        return false;
    }

    const FRunnerChassisData* Chassis = ChassisTable->FindRow<FRunnerChassisData>(ChassisId, TEXT("BuildProfile"), false);
    if (!Chassis)
    {
        OutError = FString::Printf(TEXT("Chassi '%s' nao existe."), *ChassisId.ToString());
        return false;
    }
    if (Chassis->bExtinct)
    {
        OutError = FString::Printf(TEXT("Chassi '%s' esta EXTINTO e nao pode ser escolhido."), *ChassisId.ToString());
        return false;
    }

    const FRunnerClassData* Class = ClassTable->FindRow<FRunnerClassData>(ClassId, TEXT("BuildProfile"), false);
    if (!Class)
    {
        OutError = FString::Printf(TEXT("Classe '%s' nao existe."), *ClassId.ToString());
        return false;
    }

    OutProfile.AccountName = AccountName;
    OutProfile.ChassisId = ChassisId;
    OutProfile.ClassId = ClassId;

    // Atributos: base 8 em tudo, chassi aplica o desvio (GDD 3.1 e 4).
    static const ERunnerAttribute Todas[] = {
        ERunnerAttribute::Strength, ERunnerAttribute::Dexterity, ERunnerAttribute::Constitution,
        ERunnerAttribute::Intelligence, ERunnerAttribute::Wisdom, ERunnerAttribute::Charisma
    };
    for (const ERunnerAttribute Atributo : Todas)
    {
        OutProfile.Attributes.Add(Atributo, URunnerRules::GetFinalAttributeScore(*Chassis, Atributo));
    }

    // Ficha derivada: HP, CA, Eter e ataques por Acao de Ataque.
    const int32 Constitution = OutProfile.GetAttribute(ERunnerAttribute::Constitution);
    const int32 Dexterity = OutProfile.GetAttribute(ERunnerAttribute::Dexterity);

    OutProfile.MaxHitPoints = URunnerRules::GetLevelOneHitPoints(Class->HitDie, Constitution);
    OutProfile.HitPointsPerLevel = URunnerRules::GetHitPointsPerLevel(Class->HitDie, Constitution);
    OutProfile.ArmorClass = URunnerRules::GetArmorClass(Dexterity, 0);
    OutProfile.EtherCells = URunnerRules::GetMaxEtherCells();
    OutProfile.AttacksPerAttackAction = URunnerRules::GetAttacksPerAttackAction(1);
    OutProfile.BodyMesh = Chassis->Mesh;
    OutProfile.AccentColor = Chassis->AccentColor;
    OutProfile.bValid = true;
    OutError.Reset();
    return true;
}
