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

    void RecalculateMaxes(uint8_t level, uint8_t constitution, uint8_t willpower);
    void Regen(uint16_t hpMult, uint16_t mpMult, uint16_t staMult);
};
