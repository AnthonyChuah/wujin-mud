#pragma once

#include <cstdint>

struct Loot
{
    uint16_t loot[5] = {0};
    uint16_t maxloot = 2000; // Upgradeable to 12000 max
    uint32_t GetTotalLootValue() const;
    void GainLoot(unsigned tier, uint16_t qty);
    uint32_t UpgradeLootCapacityCost();
};

struct Items
{
    uint16_t maxsupplies = 1000;
    uint16_t maxammo = 1000;

    uint16_t supplies = 1000;
    uint16_t ammo = 0;
    uint32_t money = 1000; // remember to handle under/overflow
    uint32_t banked = 0;
};
