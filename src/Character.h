#pragma once

#include "Creature.h"
#include "Equipment.h"
#include "World.h"

#include <cstdint>
#include <string>
#include <vector>

class CharacterFileLoader;

class Character : public Creature
{
public:
    Character(std::string* output, World* world, const std::string& name);
    Character(std::string* output, World* world, const CharacterFileLoader& loader);
    Character() = default;

    void ExecuteCommand(const std::string& command);

    const std::string& GetName() const
    {
        return _name;
    }
    void SetName(const std::string& name)
    {
        _name = name;
    }

    Location GetLocation() const
    {
        return _location;
    }
    void SetLocation(Location loc)
    {
        _location = loc;
    }

    // In case of player re-connection, must re-map new connection's buffer to character.
    void MapOutputBuffer(std::string* output)
    {
        _output = output;
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

    std::string* _output = nullptr;
    World* _world = nullptr;

    std::string _name;
    EquipmentSet _equipment;
    Location _location;
    uint32_t _speed = 4; // expand this out into all character attributes and skills
    bool _inCombat = false;
};
