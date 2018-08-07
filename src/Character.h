#pragma once

#include "Creature.h"
#include "Equipment.h"
#include "World.h"

#include <cstdint>
#include <string>
#include <vector>

struct CharacterTemplate;

class Character : public Creature
{
public:
    Character(std::string& output, World& world, const std::string& name);
    void ExecuteCommand(const std::string& command);

    const std::string& GetName() const
    {
        return _name;
    }
private:
    void DoAdmin(const std::vector<std::string>& tokens);

    void DoMove(const std::vector<std::string>& tokens);
    bool Move(const std::string& first, const std::string& second);
    int32_t GetSpeed() const;

    void DoActivity(const std::vector<std::string>& tokens);
    void DoSkill(const std::vector<std::string>& tokens);
    void DoSpell(const std::vector<std::string>& tokens);
    void DoCombatAction(const std::vector<std::string>& tokens);
    void DoCombatSkill(const std::vector<std::string>& tokens);
    void DoCombatSpell(const std::vector<std::string>& tokens);

    std::string& _output;
    World& _world;

    const std::string _name;
    Items::EquipmentSet _equipment;
    Geo::Coordinates _location;
    uint32_t _speed = 4; // expand this out into all character attributes and skills
    bool _inCombat = false;
};
