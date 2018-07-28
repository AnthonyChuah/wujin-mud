#include "Actions.h"
#include "Character.h"
#include "CommandParser.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <regex>

bool HasOnlyDigits(const std::string& str)
{
    return std::all_of(str.begin(), str.end(),
                       [](char c)
                       {
                           return std::isdigit(c);
                       });
}

Character::Character(std::ostream& outstream, World& world)
    : _outstream(outstream), _world(world)
{}

void Character::ExecuteCommand(const std::string& command)
{
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
        _world.RemoveCharacter(this);
        _outstream << "Good-bye, see you again soon in the Age of Wujin!\n";
    }
}

void Character::DoMove(const std::vector<std::string>& tokens)
{
    Geo::Direction direction = Geo::GetDirection(tokens[0]);
    switch (direction)
    {
    case Geo::Direction::NORTH:
        break;
    case Geo::Direction::SOUTH:
        break;
    case Geo::Direction::EAST:
        break;
    case Geo::Direction::WEST:
        break;
    case Geo::Direction::NORTHEAST:
        break;
    case Geo::Direction::NORTHWEST:
        break;
    case Geo::Direction::SOUTHEAST:
        break;
    case Geo::Direction::SOUTHWEST:
        break;
    case Geo::Direction::INVALID:
    {
        // This is "move", the only non-directional movement
        if (tokens.size() == 3)
        {
            const std::string& first = tokens[1];
            const std::string& second = tokens[2];
            if (!Move(first, second))
                _outstream << "Unable to move to coordinates " << first
                           << ", " << second << "\n";
        }
        else
            _outstream << "You must specify two arguments to move, "
                "i.e. move [X-coord] [Y-coord], both must be numbers representing "
                "a location within reach in one step, where your step size is "
                       << GetSpeed() << "\n";
        break;
    }
    }
}

bool Character::Move(const std::string& first, const std::string& second)
{
    using namespace Geo;
    if (HasOnlyDigits(first) && HasOnlyDigits(second))
    {
        int32_t x = std::stoi(first);
        int32_t y = std::stoi(second);
        Coordinates target = {x, y};
        int32_t distance = GetDistance(_location, target);
        if (distance <= GetSpeed())
        {
            _location = target;
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
