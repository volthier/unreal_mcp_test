#include "Data/RunnerGameSettings.h"

URunnerGameSettings::URunnerGameSettings()
{
    ChassisTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/Data/DT_Chassis.DT_Chassis")));
    ClassTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/Data/DT_Classes.DT_Classes")));
}
