#include "Data/RunnerRules.h"

namespace
{
    /** Divisao com arredondamento para baixo (o 5e arredonda para baixo em modificadores). */
    int32 FloorDiv(const int32 Numerator, const int32 Denominator)
    {
        return (Numerator >= 0)
            ? (Numerator / Denominator)
            : -(((-Numerator) + Denominator - 1) / Denominator);
    }
}

int32 URunnerRules::GetAttributeModifier(const int32 Score)
{
    return FloorDiv(Score - 10, 2);
}

int32 URunnerRules::GetBaseAttributeScore()
{
    return 8;
}

int32 URunnerRules::GetFinalAttributeScore(const FRunnerChassisData& Chassis, const ERunnerAttribute Attribute)
{
    return GetBaseAttributeScore() + Chassis.GetBonus(Attribute);
}

int32 URunnerRules::GetAttributeCapForChapter(const int32 Chapter)
{
    // Capitulo I (1-20) = 20 · II (21-40) = 24 · III (41-60) = 28
    const int32 Clamped = FMath::Clamp(Chapter, 1, 3);
    return 20 + (Clamped - 1) * 4;
}

int32 URunnerRules::GetProficiencyBonus(const int32 Level)
{
    if (Level >= 17) { return 6; }
    if (Level >= 13) { return 5; }
    if (Level >= 9)  { return 4; }
    if (Level >= 5)  { return 3; }
    return 2;
}

int32 URunnerRules::GetLevelOneHitPoints(const int32 HitDie, const int32 ConstitutionScore)
{
    return 3 * HitDie + GetAttributeModifier(ConstitutionScore);
}

int32 URunnerRules::GetHitPointsPerLevel(const int32 HitDie, const int32 ConstitutionScore)
{
    return HitDie + GetAttributeModifier(ConstitutionScore);
}

int32 URunnerRules::GetArmorClass(const int32 DexterityScore, const int32 ArmorBonus)
{
    return 10 + GetAttributeModifier(DexterityScore) + ArmorBonus;
}

int32 URunnerRules::GetMaxEtherCells()
{
    return 100;
}

int32 URunnerRules::GetAttacksPerAttackAction(const int32 Level)
{
    if (Level >= 17) { return 4; }
    if (Level >= 11) { return 3; }
    if (Level >= 5)  { return 2; }
    return 1;
}
