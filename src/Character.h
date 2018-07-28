#pragma once

#include "Creature.h"
#include "Equipment.h"
#include "World.h"

#include <cstdint>
#include <ostream>
#include <vector>

class Character : public Creature
{
public:
    Character(std::ostream& outstream, World& world);
    void ExecuteCommand(const std::string& command);

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

    std::ostream& _outstream;
    World& _world;

    Items::EquipmentSet _equipment;
    Geo::Coordinates _location;
    uint32_t _speed = 4;
    bool _inCombat = false;
};
