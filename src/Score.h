#pragma once

#include <cstdint>

struct Score
{
    uint16_t hp;
    uint16_t maxhp;
    uint16_t mp;
    uint16_t maxmp;
    uint16_t sta;
    uint16_t maxsta;

    static constexpr uint16_t basehp = 500;
    static constexpr uint16_t basemp = 500;
    static constexpr uint16_t basesta = 500;
    static constexpr uint16_t fixedhpcon = 300;
    static constexpr uint16_t fixedmpwil = 300;
    static constexpr uint16_t fixedstacon = 300;
    void RecalculateMaxes(uint8_t level, uint8_t constitution, uint8_t willpower);
    void Regen(uint16_t hpMult, uint16_t mpMult, uint16_t staMult);
};
