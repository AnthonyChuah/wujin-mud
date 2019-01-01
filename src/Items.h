#pragma once

#include <cstdint>
#include <string>

struct Loot
{
    static constexpr size_t totalTiers = 5;

    uint16_t loot[totalTiers] = {0};
    uint16_t maxloot = 2000; // Upgradeable to 12000 max
    uint32_t GetTotalLootValue() const;
    void GainLoot(unsigned tier, uint16_t qty);
    uint32_t UpgradeLootCapacityCost();

    std::string PrettyPrint() const;
};

struct Items
{
    uint16_t maxsupplies = 1000;
    uint16_t maxammo = 1000;

    uint16_t supplies = 1000;
    uint16_t ammo = 0;
    uint32_t money = 1000; // remember to handle under/overflow
    uint32_t banked = 0;

    std::string PrettyPrint() const;
};
