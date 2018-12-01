#pragma once

#include <cstdint>

class Character;

struct Trade
{
    static void BuySupplies(Character& character, uint16_t qty);
    static void BuyAmmo(Character& character, uint16_t qty);
};
