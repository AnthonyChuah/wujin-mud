#include "Progress.h"

#include "Attributes.h"
#include "Score.h"

namespace
{

constexpr uint32_t levelThresholds[Progress::maxLevel] = {
    1000, 3000, 7000, 15000, 31000,
    55000, 91000, 145000, 226000, 348000,
    500000, 690000, 928000, 1225000, 1596000,
    2041000, 2575000, 3216000, 3985000, 4908000,
    5923000, 7040000, 8269000, 9620000, 12000000
};

}

bool Progress::GainExperience(uint32_t gain, Score& score, const Attributes& attr)
{
    if (level == maxLevel)
        return false;
    experience += gain;
    if (experience > levelThresholds[level])
    {
        GainLevel(score, attr);
        return true;
    }
    return false;
}

void Progress::GainLevel(Score& score, const Attributes& attr)
{
    printf("Character has advanced one level up from level %hhu", level);
    score.RecalculateMaxes(++level, attr.constitution, attr.willpower);
    abilityPoints += apPerLevel;
}
