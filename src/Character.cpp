#include "Actions.h"
#include "Character.h"
#include "CharacterFileLoader.h"
#include "CommandParser.h"
#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <regex>

bool HasOnlyDigits(const std::string& str)
{
    return std::all_of(str.begin(), str.end(),
                       [](char c)
                       {
                           return std::isdigit(c);
                       });
}

Character::Character(std::string* output, World* world, const std::string& name)
    : _output(output), _world(world), _name(name)
{}

Character::Character(std::string* output, World* world, const CharacterFileLoader& loader)
    : _output(output), _world(world)
{
    _name = loader._character._name;
    _equipment = loader._character._equipment;
    _location = loader._character._location;
    _speed = loader._character._speed;
    printf("Character with name %s has location loaded: (%hhu, %hhu) (%hhu, %hhu)\n",
           _name.c_str(), _location.major.x, _location.major.y, _location.minor.x, _location.minor.y);
}

void Character::ExecuteCommand(const std::string& command)
{
    if (command.empty())
        return;

    std::vector<std::string> tokens = Parse::TokenizeCommand(command);
    if (tokens.empty())
        return;

    // First token is always an action
    Actions::ActionType type = Actions::GetActionType(tokens[0], _inCombat);
    switch (type)
    {
    case Actions::ActionType::ADMIN:
        DoAdmin(tokens);
        break;
    case Actions::ActionType::MOVEMENT:
        DoMove(tokens);
        break;
    case Actions::ActionType::ACTIVITY:
        DoActivity(tokens);
        break;
    case Actions::ActionType::SKILL:
        DoSkill(tokens);
        break;
    case Actions::ActionType::SPELL:
        DoSpell(tokens);
        break;
    case Actions::ActionType::COMBATACTION:
        DoCombatAction(tokens);
        break;
    case Actions::ActionType::COMBATSKILL:
        DoCombatSkill(tokens);
        break;
    case Actions::ActionType::COMBATSPELL:
        DoCombatSpell(tokens);
        break;
    default:
        // Error
        break;
    }
}

void Character::DoAdmin(const std::vector<std::string>& tokens)
{
    if (tokens[0] == "quit")
    {
        _world->RemoveCharacter(this);
        _output->append("Good-bye, see you again soon in the Age of Wujin!\n");
    }
}

void Character::DoMove(const std::vector<std::string>& tokens)
{
    Direction direction = GetDirection(tokens[0]);
    switch (direction)
    {
    case Direction::NORTH:
        break;
    case Direction::SOUTH:
        break;
    case Direction::EAST:
        break;
    case Direction::WEST:
        break;
    case Direction::NORTHEAST:
        break;
    case Direction::NORTHWEST:
        break;
    case Direction::SOUTHEAST:
        break;
    case Direction::SOUTHWEST:
        break;
    case Direction::INVALID:
    {
        // This is "move", the only non-directional movement
        if (tokens.size() == 3)
        {
            const std::string& first = tokens[1];
            const std::string& second = tokens[2];
            if (!Move(first, second))
            {
                _output->append("Unable to move to coordinates ");
                _output->append(first).append(", ").append(second).append("\n");
            }
        }
        else
        {
            _output->append("You must specify X and Y coordinates to move, e.g. `move 42 24`.\n");
            _output->append("The destination must be within ").append(std::to_string(GetSpeed()));
            _output->append(" units (i.e. your speed)\n");
        }
        break;
    }
    }
}

bool Character::Move(const std::string& first, const std::string& second)
{
    if (HasOnlyDigits(first) && HasOnlyDigits(second))
    {
        uint8_t x = Clamp(std::stoi(first), 255, 0);
        uint8_t y = Clamp(std::stoi(second), 255, 0);
        Coordinates target = {x, y};
        uint8_t distance = GetDistance(_location.minor, target);
        if (distance <= GetSpeed())
        {
            _location.minor = target;
            return true;
        }
    }
    return false;
}

int32_t Character::GetSpeed() const
{
    if (_equipment.GetBonuses().speed < _speed)
        return _speed;
    return _equipment.GetBonuses().speed;
}

void Character::DoActivity(const std::vector<std::string>& tokens)
{
    (void) tokens;
}

void Character::DoSkill(const std::vector<std::string>& tokens)
{
    (void) tokens;
}

void Character::DoSpell(const std::vector<std::string>& tokens)
{
    (void) tokens;
}

void Character::DoCombatAction(const std::vector<std::string>& tokens)
{
    (void) tokens;
}

void Character::DoCombatSkill(const std::vector<std::string>& tokens)
{
    (void) tokens;
}

void Character::DoCombatSpell(const std::vector<std::string>& tokens)
{
    (void) tokens;
}
