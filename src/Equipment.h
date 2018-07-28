#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Items
{

enum class EquipSlot : uint8_t
{
    INVALID = 0,
    MAINHAND = 1,
    OFFHAND = 2,
    HEAD = 3,
    NECK = 4,
    TORSO = 5,
    ARMS = 6,
    HANDS = 7,
    WAIST = 8,
    LEGS = 9,
    FEET = 10,
    MOUNT = 11
};

struct Item
{
    std::string name;
    std::string description;
    uint32_t value;
    uint32_t weight;
};

struct Equipment : Item
{
    EquipSlot slot;
};

struct Weapon : Equipment
{
    uint32_t deflect;
    uint32_t damage;
    bool twohanded;
    uint8_t reach;
};

struct Armour : Equipment
{
    uint32_t deflect;
    uint32_t reduction;
};

struct Mount : Equipment
{
    uint32_t speed;
};

struct EquipmentBonuses
{
    uint32_t damage;
    uint32_t deflect;
    uint32_t reduction;
    uint32_t speed;
    uint32_t weight;
    uint8_t reach;
};

/**
 * At some point, implement lookup table of equipment and characters' equipment sets
 * only need to index into the table.
 */
class EquipmentSet
{
public:
    void CalculateBonuses();
    const EquipmentBonuses& GetBonuses() const
    {
        return _bonuses;
    }

private:
    EquipmentBonuses _bonuses;
    std::pair<bool, Weapon> _mainhand;
    std::pair<bool, Weapon> _offhand;
    std::pair<bool, Armour> _head;
    std::pair<bool, Armour> _neck;
    std::pair<bool, Armour> _torso;
    std::pair<bool, Armour> _arms;
    std::pair<bool, Armour> _hands;
    std::pair<bool, Armour> _waist;
    std::pair<bool, Armour> _legs;
    std::pair<bool, Armour> _feet;
    std::pair<bool, Mount> _mount;
};

} // namespace Items
