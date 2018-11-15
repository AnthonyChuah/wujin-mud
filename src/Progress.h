#pragma once

#include <cstdint>
#include <cstdio>

/**
 * Experience table:
 * 1000, 3000, 7000, 15000, 31000 (diffs double)
 * 55000, 91000, 145000, 226000, 348000 (diffs 1.5x, round up to thousand)
 * 500000, 690000, 928000, 1225000, 1596000 (diffs 1.25x, round up to thousand)
 * 2041000, 2575000, 3216000, 3985000, 4908000 (diffs 1.2x, round up to thousand)
 * 5923000, 7040000, 8269000, 9620000, 12000000
 * (diffs 1.1x, round up to thousand, final level round up to million)
 *
 * I expect exp gains to range from 100 (weakest monster kill) to 10000 (strongest monster kill)
 * and to 100000 (hardest quest complete).
 *
 * Ability points: base level-0 points is 100.
 * Every level grants 40 ability points. So at level 3, you start with 220 points.
 * By level 25, you have 1100 points.
 *
 * Abilities cost between 10 to 50 points each (intelligence/dexterity = 5).
 * Assuming 30 points per ability, that gives a character of average int/dex
 * the ability to learn 36 abilities. Suppose that 8 of them are prerequisites,
 * 5 are passives, that gives us 23 different abilities to use.
 * For 10 int/dex types, we learn 46 abilities. Since magic-types tend to have more
 * prerequisites, let us say 20 of them are. 36 different abilities to use.
 */
struct Attributes;
struct Score;

struct Progress
{
    static constexpr uint8_t maxLevel = 25;
    static constexpr uint16_t apPerLevel = 40;

    uint32_t experience = 7000;
    uint16_t abilityPoints = 220;
    uint16_t usedPoints = 0;
    uint8_t level = 3;

    void DeathExpLoss()
    {
        printf("Experience lost due to death, 10 percent taken from %u", experience);
        experience = (experience * 9) / 10;
    }
    bool GainExperience(uint32_t gain, Score& score, const Attributes& attr);
    void GainLevel(Score& score, const Attributes& attr);
};

inline uint8_t CalculateBaseMoveSpeed(Progress progress)
{
    if (progress.level == Progress::maxLevel)
        return 14;

    return 8 + progress.level / 5;
}
