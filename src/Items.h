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
    uint16_t maxsupplies = 2000;
    uint16_t maxammo = 2000;

    uint16_t supplies;
    uint16_t ammo;
    uint32_t money; // remember to handle under/overflow
    uint32_t banked;
};
