#include "Actions.h"
#include "Attributes.h"
#include "Character.h"
#include "CharacterFileLoader.h"
#include "CommandParser.h"
#include "Game.h"
#include "Items.h"
#include "Trade.h"
#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>

bool HasOnlyDigits(const std::string& str)
{
    return std::all_of(str.begin(), str.end(),
                       [](char c)
                       {
                           return std::isdigit(c);
                       });
}

Character::Character(std::string* output, World* world, const CharacterFileLoader& loader, size_t id)
    : _output(output), _world(world), _id(id)
{
    _name = loader._character._name;
    _pwd = loader._character._pwd;
    _attr = loader._character._attr;
    _progress = loader._character._progress;
    _score = loader._character._score;
    _items = loader._character._items;
    _loot = loader._character._loot;
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

    std::vector<std::string> tokens = TokenizeCommand(command);
    for (auto& str : tokens)
        printf("Character::ExecuteCommand got token '%s'\n", str.c_str());
    if (tokens.empty())
        return;

    // First token is always an action
    ActionType type = GetActionType(tokens[0], _inCombat);
    switch (type)
    {
    case ActionType::ADMIN:
        DoAdmin(tokens);
        break;
    case ActionType::DIRECTION:
        DoMove(tokens);
        break;
    case ActionType::ACTIVITY:
        DoActivity(tokens); // Stuff like resting, trading, quest administration
        break;
    case ActionType::SKILL:
        DoSkill(tokens); // Physical abilities
        break;
    case ActionType::SPELL:
        DoSpell(tokens); // Mental abilities: maybe should not separate them from physical?
        break;
    case ActionType::COMBATACTION:
        DoCombatAction(tokens); // Fleeing
        break;
    case ActionType::COMBATSKILL:
        DoCombatSkill(tokens);
        break;
    case ActionType::COMBATSPELL:
        DoCombatSpell(tokens);
        break;
    default:
        // Error
        break;
    }
}

void Character::Regen()
{
    uint16_t hpMult = 1; uint16_t mpMult = 1; uint16_t staMult = 1;
    if (_resting)
    {
        hpMult = 5; mpMult = 5; staMult = 5;
    }
    // xxx Some buffs may give better regen
    if (_items.supplies > 0)
        _score.Regen(hpMult, mpMult, staMult); // else no regen happens
    ConsumeSupplies();
}

void Character::PeriodicEffects()
{
    printf("Character %s processing periodic effect (buff drain, debuff damage)", GetName().c_str());
    // xxx implement: remember to also force the zone to apply periodics onto monsters
}

void Character::InitializeScores()
{
    _score.RecalculateMaxes(_progress.level, _attr.constitution, _attr.willpower);
    _score.Regen(50, 50, 50); // restores to max
}

bool Character::HasConqueredZone(Coordinates zone) const
{
    (void) zone;
    return true; // xxx implement check
}

void Character::DoAdmin(const std::vector<std::string>& tokens)
{
    if (tokens[0] == "quit")
    {
        _world->CharacterExitZone(_location.major, this);
        _output->append("Good-bye, see you again soon in the Age of Wujin!\n");
        _world->GetGame().DisconnectById(_id);
    }
}

void Character::DoMove(const std::vector<std::string>& tokens)
{
    if (_inCombat)
    {
        _output->append("Unable to move during combat!");
        return;
    }

    Direction direction = GetDirection(tokens[0]);
    if (direction == Direction::MOVE)
    {
        // This is "move", the only non-directional movement
        if (tokens.size() >= 3)
        {
            const std::string& first = tokens[1];
            const std::string& second = tokens[2];
            if (!Move(first, second))
            {
                _output->append("Unable to move to coordinates ");
                _output->append(first);
                _output->append(", ");
                _output->append(second);
                _output->append("\n");
            }
        }
        else
        {
            _output->append("You must specify X and Y coordinates to move, e.g. `move 42 24`.\n");
            _output->append("The destination must be within ").append(std::to_string(GetSpeed()));
            _output->append(" units (i.e. your speed)\n");
        }
    }
    else if (direction == Direction::TRAVEL)
    {
        Direction traveldir = GetDirection(tokens[1]);
        if (HasConqueredZone(_location.major))
            DoTravel(traveldir);
        else
            _output->append("Unable to fast travel without conquering this zone.\n");
    }
    else
        Move(direction);
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
            printf("Character has moved to %hhu, %hhu\n", x, y);
            PrintBriefLook();
            SetDelay(DELAY_MOVE);
            return true;
        }
        printf("Character does not have the speed to move to %hhu, %hhu\n", x, y);
        _output->append("That is too far away for a single step. Get closer.\n");
    }
    return false;
}

bool Character::Move(Direction direction)
{
    int16_t stride = _inching ? 1 : GetSpeed();
    Direction edge = _location.WhichEdge();

    if (edge == direction)
        return DoTravel(direction);
    else
    {
        int16_t xmul, ymul;
        std::tie(xmul, ymul) = GetXYDirections(direction);
        int xdest = int(_location.minor.x) + xmul * stride;
        int ydest = int(_location.minor.y) + ymul * stride;
        _location.minor.x = Clamp(xdest, 255, 0);
        _location.minor.y = Clamp(ydest, 255, 0);
        printf("Character has moved within the zone to the spot (%hhu, %hhu)\n",
               _location.minor.x, _location.minor.y);
        // Once implemented, give Character basic info in their new square
        PrintBriefLook();
        SetDelay(DELAY_MOVE);
        // Once implemented, trigger Monster Aggro in Character's Zone
    }

    return true;
}

bool Character::DoTravel(Direction direction)
{
    printf("Character is leaving zone from location %s\n", _location.PrettyPrint().c_str());
    Coordinates startZone = _location.major;
    Coordinates startSquare = _location.minor;
    if (_location.TravelZone(direction) && _world->ExistZone(_location.major))
    {
        _world->CharacterExitZone(startZone, this);
        _world->CharacterEnterZone(_location.major, this);
        _output->append("You leave this zone and enter the zone to the ");
        _output->append(PrintDirection(direction));
        _output->append("...\n");
        // Once implemented, trigger MonsterSpawn at new zone
        // Once implemented, give Character basic information at their new zone
        PrintBriefLook();
        // Once implemented, trigger Monster aggro upon Character's new location
        SetDelay(DELAY_ZONETRAVEL);
        return true;
    }

    _location.major = startZone; _location.minor = startSquare;
    _output->append("You were unable to enter the zone to the ");
    _output->append(PrintDirection(direction));
    _output->append(".\n");
    return false;
}

void Character::DoActivity(const std::vector<std::string>& tokens)
{
    if (tokens[0] == "creep")
        ToggleCreep();
    else if (tokens[0] == "r" || tokens[0] == "rest")
        SetRest(true);
    else if (tokens[0] == "stand" || tokens[0] == "st")
        SetRest(false);
    else if (tokens[0] == "buy")
        Buy(tokens);
    else if (tokens[0] == "implements")
        SetImplements(tokens);
    // xxx implement other standard activities
}

void Character::Buy(const std::vector<std::string>& tokens)
{
    // "buy supplies 42", "buy mainhand sword 3", "buy twohand axe 2", "buy offhand shield 1",
    // "buy armour mail 2", "buy ammo 42"
    if (tokens.size() == 1)
    {
        _output->append("You need to buy something! e.g. 'buy supplies 42'.\n");
        return;
    }

    if (tokens[1] == "supplies")
    {
        if (tokens.size() == 2)
        {
            _output->append("Please indicate the quantity of supplies to buy: e.g. 'buy supplies 42'.");
            return;
        }
        uint16_t suppliesQty = std::stoi(tokens[2]);
        printf("Character is trying to buy %u supplies", suppliesQty);
        Trade::BuySupplies(*this, suppliesQty);
    }

    if (tokens[1] == "ammo")
    {
        if (tokens.size() == 2 || !HasOnlyDigits(tokens[2]))
        {
            _output->append("Please indicate the quantity of ammo to buy: e.g. 'buy ammo 42'.");
            return;
        }
        uint16_t ammoQty = std::stoi(tokens[2]);
        printf("Character is trying to buy %u ammo", ammoQty);
        Trade::BuyAmmo(*this, ammoQty);
    }

    // xxx implement other purchases
}

void Character::SetImplements(const std::vector<std::string>& tokens)
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

void Character::ToggleCreep()
{
    _inching = !_inching;
    printf("Player character toggled creep to %d\n", _inching);
    if (_inching)
        _output->append("You begin to creep.\n");
    else
        _output->append("You stop creeping.\n");
}

void Character::SetRest(bool resting)
{
    _resting = resting;
    if (resting)
        _output->append("You rest.\n");
    else
        _output->append("You stop resting and get up.\n");
}

void Character::PrintBriefLook()
{
    const Zone& zone = GetZone();
    _output->append("Zone: ");
    _output->append(zone.name);
    _output->append("\n[Zone] (Location): ");
    _output->append(_location.PrettyPrint());
    // Also need to see monsters nearby
}

void Character::ConsumeSupplies()
{
    uint16_t toConsume = 1;
    // xxx Some buffs may consume more supplies when toggled
    if (_items.supplies < toConsume)
        _items.supplies = 0;
    else
        _items.supplies -= toConsume;

    printf("Character consumed supplies, %u left", _items.supplies);
}
