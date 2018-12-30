#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

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

    // xxx add learned abilities: std::vector<AbilityName> abilities;

    void DeathExpLoss()
    {
        printf("Experience lost due to death, 10 percent taken from %u\n", experience);
        experience = (experience * 9) / 10;
    }
    bool GainExperience(uint32_t gain, Score& score, const Attributes& attr);
    void GainLevel(Score& score, const Attributes& attr);
    std::string PrettyPrint() const;
};

inline uint8_t CalculateBaseMoveSpeed(Progress progress)
{
    if (progress.level == Progress::maxLevel)
        return 14;

    return 8 + progress.level / 5;
}
