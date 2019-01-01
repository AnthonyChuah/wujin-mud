#include "Equipment.h"
#include "Utils.h"

#include <cassert>
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
        return 3;
    else if (type == RangedType::SHOCKLANCE)
        return 6;
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
    return 9; // FULLPLATE
}

}

uint8_t CalculateReach(WeaponStyle style, WeaponType weapon)
{
    if (style == WeaponStyle::TWOHAND)
    {
        if (weapon == WeaponType::SPEAR)
            return 7;
        else if (weapon == WeaponType::MACE)
            return 5;
        return 6;
    }
    else
    {
        if (weapon != WeaponType::SHIELD)
            return 4;
        return 3;
    }
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

uint8_t Equipment::GetEncumbrance() const
{
    uint8_t armourWeight;
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
    uint8_t rangedWeight = rangedType == RangedType::BOW ? 5 : 10;
    uint8_t encumbrance = armourWeight + rangedWeight + implements + weaponSet.GetWeight();
    printf("Equipment::RecalcEncumbrance set encumbrance to %u\n", encumbrance);
    return encumbrance;
}

uint8_t Equipment::GetRange() const
{
    constexpr uint8_t baseRange = 15;
    return baseRange + rangedTier; // apply strength bonus from Combat module
}

/** 'T' two-hand, 'M' main-hand, 'O' off-hand, 'A' armour, 'R' ranged
 * If return 0, means cannot repair (either invalid slot or tier == 0)
 */
uint32_t Equipment::RepairCost(char slot) const
{
    uint16_t toRepair; // durability units
    uint32_t cost;
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
            toRepair = maxDurability - weaponSet.mainhandDurability;
            cost = GetCostScaling(tier) * toRepair * costMultWeapon;
            return cost;
        }
        break;
    case 'O':
        if (weaponSet.style != WeaponStyle::TWOHAND)
        {
            tier = weaponSet.offhandTier;
            toRepair = maxDurability - weaponSet.offhandDurability;
            cost = GetCostScaling(tier) * toRepair * costMultWeapon;
            return cost;
        }
        break;
    case 'A':
        tier = armourTier;
        toRepair = maxDurability - armourDurability;
        cost = GetCostScaling(tier) * toRepair * GetCostMultiplier(armourType);
        return cost;
    case 'R':
        tier = rangedTier;
        toRepair = maxDurability - rangedDurability;
        cost = GetCostScaling(tier) * toRepair * GetCostMultiplier(rangedType);
        return cost;
    default:
        assert(false && "Equipment::Repair got an invalid input");
    }
    return 0;
};

bool Equipment::Repair(char slot)
{
    switch (slot)
    {
    case 'T':
        if (weaponSet.style == WeaponStyle::TWOHAND)
        {
            weaponSet.twohandDurability = maxDurability;
            return true;
        }
        break;
    case 'M':
        if (weaponSet.style != WeaponStyle::TWOHAND)
        {
            weaponSet.mainhandDurability = maxDurability;
            return true;
        }
        break;
    case 'O':
        if (weaponSet.style != WeaponStyle::TWOHAND)
        {
            weaponSet.offhandDurability = maxDurability;
            return true;
        }
        break;
    case 'A':
        armourDurability = maxDurability;
        return true;
    case 'R':
        rangedDurability = maxDurability;
        return true;
    default:
        assert(false && "Repair got an invalid input");
    }
    return false;
}

void Equipment::DeathReset()
{
    printf("Character died, reset all equipment to base values!");
    weaponSet.twohandTier = 0;
    weaponSet.mainhandTier = 0;
    weaponSet.offhandTier = 0;
    weaponSet.twohandDurability = 0;
    weaponSet.mainhandDurability = 0;
    weaponSet.offhandDurability = 0;
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
    (void) loot;
    return 0;
}

uint32_t Equipment::SwitchWeaponStyle(WeaponStyle style)
{
    WeaponStyle currentStyle = weaponSet.style;
    printf("Equipment::SwitchWeaponStyle from %c to %c",
           CastToUnderlying(currentStyle), CastToUnderlying(style));

    if (style == currentStyle)
        return 0; // change nothing, and return 0 money for the non-existent change
    if (style == WeaponStyle::TWOWEAPON && currentStyle == WeaponStyle::SHIELD)
    {
        uint32_t sale = Valuation(weaponSet.offhandTier, 'O') / buyMultiplier;
        weaponSet.offhandTier = 0;
        weaponSet.offhandDurability = 0;
        weaponSet.style = style;
        return sale;
    } // Shield --> TwoWeapon
    else if (style == WeaponStyle::SHIELD && currentStyle == WeaponStyle::TWOWEAPON)
    {
        uint32_t sale = Valuation(weaponSet.offhandTier, 'O') / buyMultiplier;
        weaponSet.offhandTier = 0;
        weaponSet.offhandDurability = 0;
        weaponSet.style = style;
        return sale;
    } // TwoWeapon --> Shield
    else if (style == WeaponStyle::TWOHAND)
    {
        uint32_t saleM = Valuation(weaponSet.mainhandTier, 'M') / buyMultiplier;
        uint32_t saleO = Valuation(weaponSet.offhandTier, 'O') / buyMultiplier;
        weaponSet.mainhandTier = 0; weaponSet.offhandTier = 0;
        weaponSet.mainhandDurability = 0; weaponSet.offhandDurability = 0;
        weaponSet.style = style;
        return saleM + saleO;
    } // TwoWeapon or Shield --> TwoHander
    else
    {
        uint32_t saleT = Valuation(weaponSet.twohandTier, 'T') / buyMultiplier;
        weaponSet.twohandDurability = 0;
        weaponSet.twohandTier = 0;
        weaponSet.style = style;
        return saleT;
    } // TwoHander --> TwoWeapon or Shield
    // All possible state transitions covered

    assert(false && "Equipment::SwitchWeaponStyle reached an impossible case");
    return 0;
}

uint32_t Equipment::Valuation(uint8_t tier, char slot)
{
    if (slot == 'T')
        return costMultTwohand * buyMultiplier * maxDurability * GetCostScaling(tier);
    return costMultWeapon * buyMultiplier * maxDurability * GetCostScaling(tier);
    // else off-hand 'O' or main-hand 'M', both same prices
}

uint32_t Equipment::Valuation(uint8_t tier, ArmourType type)
{
    return GetCostMultiplier(type) * GetCostScaling(tier) * maxDurability * buyMultiplier;
}

uint32_t Equipment::Valuation(uint8_t tier, RangedType type)
{
    return GetCostMultiplier(type) * GetCostScaling(tier) * maxDurability * buyMultiplier;
}

bool Equipment::ExistArmour(uint8_t tier, ArmourType type)
{
    if (type == ArmourType::PADDED)
        return tier <= 3 ? true : false;
    else if (type == ArmourType::PLATE)
        return tier >= 1 ? true : false;
    else if (type == ArmourType::FULLPLATE)
        return tier >= 2 ? true : false;
    return true;
}

void PrettyPrintEnum(std::string& output, WeaponType type)
{
    switch (type)
    {
    case WeaponType::SWORD:
        output.append("sword");
        break;
    case WeaponType::AXE:
        output.append("axe");
        break;
    case WeaponType::MACE:
        output.append("mace");
        break;
    case WeaponType::SPEAR:
        output.append("spear");
        break;
    case WeaponType::SHIELD:
        output.append("shield");
        break;
    }
}

void PrettyPrintEnum(std::string& output, RangedType type)
{
    switch (type)
    {
    case RangedType::BOW:
        output.append("bow");
        break;
    case RangedType::CROSSBOW:
        output.append("crossbow");
        break;
    case RangedType::SHOCKLANCE:
        output.append("shocklance");
        break;
    case RangedType::NONE:
        output.append("no ranged weapon");
        break;
    }
}

void PrettyPrintEnum(std::string& output, ArmourType type)
{
    switch (type)
    {
    case ArmourType::PADDED:
        output.append("padded armour");
        break;
    case ArmourType::MAIL:
        output.append("mail armour");
        break;
    case ArmourType::PLATE:
        output.append("plate armour");
        break;
    case ArmourType::FULLPLATE:
        output.append("full plate armour");
        break;
    }
}

void PrettyPrintEnum(std::string& output, WeaponStyle type)
{
    switch (type)
    {
    case WeaponStyle::TWOHAND:
        output.append("two-handed-weapon style");
        break;
    case WeaponStyle::TWOWEAPON:
        output.append("two-weapon style");
        break;
    case WeaponStyle::SHIELD:
        output.append("weapon-and-shield style");
        break;
    }
}

std::string Equipment::PrettyPrint() const
{
    WeaponStyle style = weaponSet.style;
    std::string output("Your weapon style is: ");
    PrettyPrintEnum(output, style);
    output.append("\n[");
    if (style == WeaponStyle::TWOHAND)
    {
        output.append("2-hand]      ");
        PrettyPrintEnum(output, weaponSet.twohand);
        output.append(" +");
        output.append(std::to_string(weaponSet.twohandTier));
        output.append(", durability: ");
        output.append(std::to_string(weaponSet.twohandDurability));
        output.append("/");
        output.append(std::to_string(maxDurability));
    }
    else
    {
        output.append("main-hand]      ");
        PrettyPrintEnum(output, weaponSet.mainhand);
        output.append(" +");
        output.append(std::to_string(weaponSet.mainhandTier));
        output.append(", durability: ");
        output.append(std::to_string(weaponSet.mainhandDurability));
        output.append("/");
        output.append(std::to_string(maxDurability));
        output.append("\n[off-hand]      ");
        PrettyPrintEnum(output, weaponSet.offhand);
        output.append(" +");
        output.append(std::to_string(weaponSet.offhandTier));
        output.append(", durability: ");
        output.append(std::to_string(weaponSet.offhandDurability));
        output.append("/");
        output.append(std::to_string(maxDurability));
    }

    output.append("\n[armour]      ");
    PrettyPrintEnum(output, armourType);
    output.append(" +");
    output.append(std::to_string(armourTier));
    output.append(", durability: ");
    output.append(std::to_string(armourDurability));
    output.append("/");
    output.append(std::to_string(maxDurability));

    output.append("\n[ranged]      ");
    PrettyPrintEnum(output, rangedType);
    output.append(" +");
    output.append(std::to_string(rangedTier));
    output.append(", durability: ");
    output.append(std::to_string(rangedDurability));
    output.append("/");
    output.append(std::to_string(maxDurability));

    output.append("\n[implements]  ");
    output.append(std::to_string(implements));
    output.append(" weight units\n[encumbrance] ");
    output.append(std::to_string(GetEncumbrance()));
    output.append(" weight units\n");

    return output;
}
