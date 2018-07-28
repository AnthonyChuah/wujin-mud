#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Geo
{

// Although 32-bits, should never go bigger than 24-bit int
struct Coordinates
{
    Coordinates() = default;

    int32_t x;
    int32_t y;

    friend Coordinates operator+(Coordinates left, const Coordinates& right)
    {
        Coordinates result{left.x + right.x, left.y + right.y};
        return result;
    }
    friend Coordinates operator-(Coordinates left, const Coordinates& right)
    {
        Coordinates result{left.x - right.x, left.y - right.y};
        return result;
    }

    // Gets the max of horizontal and vertical distances
    friend int32_t GetDistance(Coordinates from, Coordinates to);
};

struct MajorCoordinates
{
    int32_t X;
    int32_t Y;
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

MajorCoordinates GetMajorCoordinates(Coordinates coordinates);

Direction GetDirection(const std::string& direction);

} // namespace Geo

namespace std
{

template <>
struct hash<Geo::MajorCoordinates>
{
    size_t operator()(const Geo::MajorCoordinates& key) const
    {
        return *reinterpret_cast<const uint64_t*>(&key);
    }
};

} // namespace std

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
    std::unordered_map<Geo::MajorCoordinates, Geo::Zone> _zones;
    Geo::Zone _unknownZone = {"Unknown Zone",
                              "You appear to have wandered into a glitchy unknown zone. "
                              "Report a bug! Also get back to civilization."};
};
