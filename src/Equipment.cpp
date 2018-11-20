#include "Equipment.h"

#include <cstdio>

namespace
{

uint32_t costMultWeapon = 2;
uint32_t costMultTwohand = 3;
uint32_t buyMultiplier = 5; // repairing is 5 times cheaper

uint32_t GetCostMultiplier(RangedType type)
{
    if (type == RangedType::BOW)
        return 1;
    else if (type == RangedType::CROSSBOW)
        return 4;
    else if (type == RangedType::SHOCKLANCE)
        return 8;
    else
        return 0;
}

uint32_t GetCostMultiplier(ArmourType type)
{
    if (type == ArmourType::PADDED)
        return 1;
    else if (type == ArmourType::MAIL)
        return 3;
    else if (type == ArmourType::PLATE)
        return 6;
    else if (type == ArmourType::FULLPLATE)
        return 9;
}

}

uint8_t CalculateReach(WeaponStyle style, WeaponType weapon)
{
    // xxx implement reach system
    // Longest to shortest: 2h spear, 2h sword/axe/mace, 1h sword/axe/mace, 1h shield
}

uint32_t GetCostScaling(uint32_t tier)
{
    switch (tier)
    {
    case 1:
        return 1;
    case 2:
        return 3;
    case 3:
        return 8;
    case 4:
        return 16;
    case 5:
        return 40;
    }
    return 0;
};

uint16_t Equipment::GetWeight() const
{
    uint16_t armourWeight;
    switch (armourType)
    {
    case ArmourType::PADDED:
        armourWeight = 10;
        break;
    case ArmourType::MAIL:
        armourWeight = 30;
        break;
    case ArmourType::PLATE:
        armourWeight = 45;
        break;
    case ArmourType::FULLPLATE:
        armourWeight = 60;
        break;
    }
    uint16_t rangedWeight = rangedType == RangedType::BOW ? 4 : 10; // includes quiver/arrow weight
    return armourWeight + rangedWeight + implements + weaponSet.GetWeight();
}

uint8_t Equipment::GetRange() const
{
    // xxx implement ranged weapon range
    // bow > crossbow == shocklance, quality tier gives more range, Tier-5 crossbow range == Tier-0 bow range
}

/** 'T' two-hand, 'M' main-hand, 'O' off-hand, 'A' armour, 'R' ranged
 * If return 0, means cannot repair (either invalid slot or tier == 0)
 */
uint32_t Equipment::RepairCost(char slot) const
{
    static constexpr uint16_t maxDurability = 5000;
    uint32_t cost;
    uint16_t toRepair; // durability units
    uint32_t tier;
    switch (slot)
    {
    case 'T':
        if (weaponSet.style == WeaponStyle::TWOHAND)
        {
            tier = weaponSet.twohandTier;
            toRepair = maxDurability - weaponSet.twohandDurability;
            cost = GetCostScaling(tier) * toRepair * costMultTwohand;
            return cost;
        }
        break;
    case 'M':
        if (weaponSet.style != WeaponStyle::TWOHAND)
        {
            tier = weaponSet.mainhandTier;
            toRepair = maxDurability- weaponSet.mainhandDurability;
            cost = GetCostScaling(tier) * toRepair * costMultWeapon;
            return cost;
        }
        break;
    case 'O':
        if (weaponSet.style != WeaponStyle::TWOHAND)
        {
            tier = weaponSet.offhandTier;
            toRepair = maxDurability- weaponSet.offhandDurability;
            cost = GetCostScaling(tier) * toRepair * costMultWeapon;
            return cost;
        }
        break;
    case 'A':
        tier = armourTier;
        toRepair = maxDurability- armourDurability;
        cost = GetCostScaling(tier) * toRepair * costMultArmour;
        return cost;
    case 'R':
        tier = rangedTier;
        toRepair = maxDurability- rangedDurability;
        cost = GetCostScaling(tier) * toRepair * GetCostMultiplier(rangedType);
        return cost;
    default:
        assert(false && "Equipment::Repair got an invalid input");
    }
    return 0;
};

void Equipment::DeathReset()
{
    printf("Character died, reset all equipment to base values!");
    weaponSet.twohandTier = 0;
    weaponSet.mainhandTier = 0;
    weaponSet.offhandTier = 0;
    weaponSet.twohandDurability = 0;
    weaponSet.mainhandDurability = 0;
    weaponSet.offhandDurability = 0;
    mountTier = 0;
    rangedTier = 0;
    armourTier = 0;
    rangedDurability = 0;
    armourDurability = 0;
};

uint32_t Equipment::PKLoot(Loot& loot) const
{
    // xxx implement: when player kills victim player, victim's equipment quality determines loot
    // Equipment tier 0 gives 0 loot, tier 1 gives 1 loot... affected by type multipliers
    // Player-killing also gives you all the money they were holding
}
