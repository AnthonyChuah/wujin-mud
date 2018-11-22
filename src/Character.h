#pragma once

#include "Attributes.h"
#include "Creature.h"
#include "Equipment.h"
#include "Items.h"
#include "Score.h"
#include "World.h"

#include <cstdint>
#include <string>
#include <vector>

class CharacterFileLoader;

class Character
{
public:
    friend class CharacterFileLoader;

    Character(std::string* output, World* world, const std::string& name, size_t id);
    Character(std::string* output, World* world, const CharacterFileLoader& loader, size_t id);
    Character() = default;

    void ExecuteCommand(const std::string& command);
    void Regen();
    void PeriodicEffects();

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name) { _name = name; }
    void SetDelay(uint16_t delay) { _delay = delay; printf("Set delay to %hu\n", _delay); }
    uint16_t GetDelay() const { return _delay; }
    void DecrementDelay() { if (_delay > 0) --_delay; }
    Location GetLocation() const { return _location; }
    void SetLocation(Location loc) { _location = loc; }
    const Zone& GetZone() const { return _world->GetZone(_location.major); }
    uint8_t GetSpeed() const { return _speed; } // Implement logic in the future
    // In case of player re-connection, must re-map new connection's buffer to character.
    void MapToConnection(size_t id, std::string* output) { _id = id; _output = output; }
    bool HasConqueredZone(Coordinates zone) const;

private:
    void DoAdmin(const std::vector<std::string>& tokens);

    void DoMove(const std::vector<std::string>& tokens);
    bool Move(const std::string& first, const std::string& second);
    bool Move(Direction direction);
    bool DoTravel(Direction direction);

    void DoActivity(const std::vector<std::string>& tokens);
    void DoSkill(const std::vector<std::string>& tokens);
    void DoSpell(const std::vector<std::string>& tokens);
    void DoCombatAction(const std::vector<std::string>& tokens);
    void DoCombatSkill(const std::vector<std::string>& tokens);
    void DoCombatSpell(const std::vector<std::string>& tokens);

    void ToggleCreep();
    void SetRest(bool resting);
    void PrintBriefLook();
    void ConsumeSupplies();

    std::string* _output = nullptr;
    World* _world = nullptr;
    // Consider having a Zone ptr instead of indirectly looking up through World

    std::string _name;
    size_t _id;
    Attributes _attr;
    Score _score;
    Items _items;
    Equipment _equipment;
    Location _location = {{196, 128}, {0, 0}};

    // Expand these out into all transient scores
    uint16_t _delay = 0;

    // Expand these out into all non-transient character attributes and skills
    uint8_t _speed = 8; // Speed should be perhaps tied to Level and active buffs/debuffs

    // Expand these out into all character modes
    bool _inching = false;
    bool _resting = false;
    bool _inCombat = false;
    /**
     * Hp/MaxHp, Mp/MaxMp, Sta/MaxSta
     * Delay (cannot do anything while delay positive: caused by your own actions or by a stun)
     * Mental equilibrium, physical balance (required to use mental/physical skills)
     * Positioning (rooted, off-balance, normal, advantageous, flanking)
     * Effects (buffs/debuffs): many, but examples are:
     *   weak, blinded, regenerating, instant reflexes, adrenal strength, burning
     */
};
