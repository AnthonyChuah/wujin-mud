#pragma once

#include "Items.h"

#include <cstdint>

/**
 * xxx implementation notes
 * Quality tiers improve:
 * Melee Weapons: damage, deflect
 * Ranged Weapons: damage, range
 * Armour: absorb, deflect
 * Combat-related bonuses should be calculated in a separate module (e.g. "Combat")
 * Armour quality tiers:
 * Padded: from 0 to 3
 * Mail: from 0 to 5
 * Plate: from 1 to 5
 * Fullplate: from 2 to 5
 */
enum class WeaponType : char
{
    SWORD = 'J',
    AXE = 'A',
    MACE = 'M',
    SPEAR = 'S',
    SHIELD = 'D'
};

enum class RangedType : char
{
    BOW = 'B',
    CROSSBOW = 'C',
    SHOCKLANCE = 'L',
    NONE = 'N'
};

enum class ArmourType : char
{
    PADDED = 'G',
    MAIL = 'M',
    PLATE = 'P',
    FULLPLATE = 'F'
};

enum class WeaponStyle : char
{
    TWOHAND = 'T',
    TWOWEAPON = '2',
    SHIELD = 'S'
};

void PrettyPrintEnum(std::string& output, WeaponType type);
void PrettyPrintEnum(std::string& output, RangedType type);
void PrettyPrintEnum(std::string& output, ArmourType type);
void PrettyPrintEnum(std::string& output, WeaponStyle type);

uint8_t CalculateReach(WeaponStyle style, WeaponType weapon);

struct WeaponSet
{
    WeaponStyle style = WeaponStyle::TWOHAND;
    WeaponType twohand = WeaponType::SPEAR;
    uint8_t twohandTier = 0;
    WeaponType mainhand;
    uint8_t mainhandTier = 0;
    WeaponType offhand;
    uint8_t offhandTier = 0;
    uint16_t twohandDurability = 0; // Durability maximum of 5000, drain 10 per tick in combat
    uint16_t mainhandDurability = 0; // Tier 0 weapons have 0 durability but never degrade
    uint16_t offhandDurability = 0;

    uint16_t GetWeight() const { return style == WeaponStyle::SHIELD ? 10 : 5; }
};

uint32_t GetCostScaling(uint32_t tier);

struct Equipment
{
    static constexpr uint16_t maxDurability = 5000;

    WeaponSet weaponSet;
    RangedType rangedType = RangedType::NONE;
    uint8_t rangedTier = 0;
    ArmourType armourType = ArmourType::MAIL;
    uint8_t armourTier = 0;
    uint16_t rangedDurability = 0;
    uint16_t armourDurability = 0;

    // xxx some thaumaturgy spells require at least some weight carried here
    // Stuff like capacitors, volatiles, water, which power some of those spells
    // I could see up to weight of 20 here, but no higher
    // Players free to change this level as they want
    uint8_t implements = 0;

    uint8_t GetRange() const;
    uint32_t RepairCost(char slot) const;
    bool Repair(char slot);
    void DeathReset();
    uint32_t PKLoot(Loot& loot) const;
    uint8_t GetEncumbrance() const;
    std::string PrettyPrint() const;

    // xxx players can switch weapon styles in camps
    uint32_t SwitchWeaponStyle(WeaponStyle style); // return sale price of obsolete weapons
    // The Buy functions return the money that you need to buy the item, if you cannot afford it
    uint32_t BuyWeapon(uint32_t& money, char slot, WeaponType type, uint8_t tier);
    bool ValidateWeaponTypeAndSlot(char slot, WeaponType type) const;
    uint32_t BuyArmour(uint32_t& money, ArmourType type, uint8_t tier);
    uint32_t BuyRanged(uint32_t& money, RangedType type, uint8_t tier);

    static uint32_t Valuation(uint8_t tier, char slot); // for weapons
    static uint32_t Valuation(uint8_t tier, RangedType type); // ranged weapons
    static uint32_t Valuation(uint8_t tier, ArmourType type); // armour
    static bool ExistArmour(uint8_t tier, ArmourType type);
};
