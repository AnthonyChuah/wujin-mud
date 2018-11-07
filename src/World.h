#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct Coordinates
{
    uint8_t x;
    uint8_t y;

    // Gets the max of horizontal and vertical distances
    friend uint8_t GetDistance(Coordinates from, Coordinates to);
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
struct Location
{
    Coordinates major;
    Coordinates minor;
};

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
    SOUTHWEST = 8
};

struct Zone
{
    std::string name;
    std::string description;
};

Direction GetDirection(const std::string& direction);

class Character;

/**
 * World can do book-keeping for entities and zones in the world
 */
class World
{
public:
    World(const std::string& file);

    void RemoveCharacter(Character* character);
private:
    std::unordered_map<Coordinates, Zone> _zones;
    Zone _unknownZone = {"Unknown Zone",
                         "You appear to have wandered into a glitchy unknown zone. "
                         "Report a bug! Also get back to civilization."};
};
