#pragma once

#include <cstdint>

struct Score
{
    uint16_t hp = 500;
    uint16_t mp = 500;
    uint16_t sta = 500;
    uint16_t maxhp = 500;
    uint16_t maxmp = 500;
    uint16_t maxsta = 500;
    uint16_t delay = 0;

    void RecalculateMaxes(uint8_t level, uint8_t constitution, uint8_t willpower);
    void Regen(uint16_t hpMult, uint16_t mpMult, uint16_t staMult);
};
