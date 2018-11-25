#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "rapidjson/document.h"

struct Coordinates
{
    uint8_t x;
    uint8_t y;

    // Gets the max of horizontal and vertical distances
    friend uint8_t GetDistance(Coordinates from, Coordinates to);
    friend bool operator==(const Coordinates& lhs, const Coordinates& rhs);
};

namespace std
{

template <>
struct hash<Coordinates>
{
    size_t operator()(const Coordinates& coord) const
    {
        return coord.x * 256 + coord.y;
    }
};

}

// World is divided into 256 by 256 zones, each zone has 256 by 256 locations
enum class Direction : uint8_t
{
    INVALID = 0,
    NORTH = 1,
    SOUTH = 2,
    EAST = 3,
    WEST = 4,
    NORTHEAST = 5,
    NORTHWEST = 6,
    SOUTHEAST = 7,
    SOUTHWEST = 8,
    MOVE = 9,
    TRAVEL = 10
};

struct Location
{
    Coordinates major;
    Coordinates minor;

    Direction WhichEdge() const;
    bool TravelZone(Direction direction);
    std::string PrettyPrint() const;
};

class Character;

enum class ZoneType : char
{
    CAPITAL = 'X',
    CITY = 'C',
    FORT = 'F',
    CAMP = 'E',
    ROAD = 'R',
    WILDERNESS = 'W',
    STRONGHOLD = 'S'
};

enum class Terrain : char
{
    CITY = 'C',
    CAMP = 'E',
    ROAD = 'R',
    PLAIN = 'P',
    TUNDRA = 'T',
    DESERT = 'D',
    WET = 'W',
    FOREST = 'F',
    JUNGLE = 'J',
    MOUNTAIN = 'M'
};

uint8_t GetTerrainVisibility(Terrain terrain);

struct Zone
{
    Zone(const std::string& n, const std::string& desc, Coordinates c, ZoneType ty, Terrain ter)
        : name(n), description(desc), coord(c), type(ty), terrain(ter)
    {}

    std::string name;
    std::string description;
    Coordinates coord;
    ZoneType type;
    Terrain terrain;

    // std::vector<Monster> _monsters;
    std::set<Character*> _characters; // World is responsible for managing these pointers

    std::string PrettyPrint() const;
    void SpawnMonsters();
};

std::pair<int16_t, int16_t> GetXYDirections(Direction direction);
Direction GetDirection(const std::string& direction);
std::string PrintDirection(Direction direction);

// May throw exceptions which we will not catch: something's wrong with a JSON file if exception
Coordinates GetCoordinatesFromString(const std::string& str, char delim);

constexpr uint16_t DELAY_ZONETRAVEL = 30;
constexpr uint16_t DELAY_MOVE = 5;

/**
 * World can do book-keeping for entities and zones in the world
 */
class Game;

class World
{
public:
    World(const std::string& file, Game& game);

    const Zone& GetZone(Coordinates coord) const
    {
        return _zones.at(coord);
    }
    bool ExistZone(Coordinates coord) const
    {
        return _zones.find(coord) != _zones.end();
    }

    Game& GetGame() { return _game; }
    void CharacterExitZone(Coordinates coord, Character* self);
    void CharacterEnterZone(Coordinates coord, Character* self);
private:
    bool CheckDomValid(const rapidjson::Document& dom) const;
    void PopulateZones(const rapidjson::Document& dom);

    std::unordered_map<Coordinates, Zone> _zones;
    Game& _game;
};
