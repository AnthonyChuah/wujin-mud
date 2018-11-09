#include "World.h"

#include "Character.h"

#include <cmath>
#include <fstream>

uint8_t GetDistance(Coordinates from, Coordinates to)
{
    uint8_t xdiff = std::abs(to.x - from.x);
    uint8_t ydiff = std::abs(to.y - from.y);
    return xdiff > ydiff ? xdiff : ydiff;
}

bool operator==(const Coordinates& lhs, const Coordinates& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

Direction Location::WhichEdge() const
{
    uint8_t x = minor.x;
    uint8_t y = minor.y;

    if (x == 0)
    {
        if (y < 64)
            return Direction::NORTHWEST;
        else if (y >= 192)
            return Direction::SOUTHWEST;
        return Direction::WEST;
    }
    else if (x == 255)
    {
        if (y < 64)
            return Direction::NORTHEAST;
        else if (y >= 192)
            return Direction::SOUTHEAST;
        return Direction::EAST;
    }
    if (y == 0)
    {
        if (x < 64)
            return Direction::NORTHWEST;
        else if (x >= 192)
            return Direction::NORTHEAST;
        return Direction:: NORTH;
    }
    else if (y == 255)
    {
        if (x < 64)
            return Direction::SOUTHWEST;
        else if (x >= 192)
            return Direction::SOUTHEAST;
        return Direction:: SOUTH;
    }

    return Direction::INVALID;
}

bool Location::TravelZone(Direction direction)
{
    int16_t xdest, ydest;
    int16_t xdir, ydir;
    std::tie(xdir, ydir) = GetXYDirections(direction);
    xdest = int16_t(major.x) + xdir;
    ydest = int16_t(major.y) + ydir;
    if (xdest < 0 || xdest > 255 || ydest < 0 || ydest > 255)
        return false;

    major.x = xdest; major.y = ydest;
    if (xdir == 0)
    {
        // Must be going north or south: just flip minor.y
        minor.y = 255 - minor.y; // 255 -> 0, 0 -> 255
        minor.x = 128;
    }
    else if (ydir == 0)
    {
        // Must be going east or west: just flip minor.x
        minor.x = 255 - minor.x;
        minor.y = 128;
    }
    else
    {
        // Must be going diagonally. Do not flip: they must start in the corner
        // xdir -1, ydir +1 -> going southwest -> northeast corner -> 255, 0
        // xdir +1, ydir -1 -> going northeast -> southwest corner -> 0, 255
        // xdir +1, ydir +1 -> going southeast -> northwest corner -> 0, 0
        // xdir -1, ydir -1 -> going northwest -> southeast corner -> 255, 255
        if (ydir == 1)
            minor.y = 0;
        else if (ydir == -1)
            minor.y = 255;
        if (xdir == 1)
            minor.x = 0;
        else if (xdir == -1)
            minor.x = 255;
    }

    return true;
}

std::string Location::PrettyPrint() const
{
    std::string print = "[";
    print += major.x;
    print += ", ";
    print += major.y;
    print += "] (";
    print += minor.x;
    print += ", ";
    print += minor.y;
    print += ")";
    return print;
}

std::string Zone::PrettyPrint() const
{
    std::string print = name;
    print += "[";
    print += coord.x;
    print += ", ";
    print += coord.y;
    print += "]";
    return print;
}

void Zone::SpawnMonsters()
{
    // Should lookup zone table for which monsters to spawn where
    // Monster ctor will lookup monster table to populate monster stats
}

std::pair<int16_t, int16_t> GetXYDirections(Direction direction)
{
    switch (direction)
    {
    case Direction::NORTH:
        return {0, -1};
    case Direction::SOUTH:
        return {0, 1};
    case Direction::EAST:
        return {1, 0};
    case Direction::WEST:
        return {-1, 0};
    case Direction::NORTHEAST:
        return {1, -1};
    case Direction::NORTHWEST:
        return {-1, -1};
    case Direction::SOUTHEAST:
        return {1, 1};
    case Direction::SOUTHWEST:
        return {-1, 1};
    default:
        return {0, 0};
    }
}

Direction GetDirection(const std::string& direction)
{
    if (direction == "n" || direction == "north")
        return Direction::NORTH;
    else if (direction == "s" || direction == "south")
        return Direction::SOUTH;
    else if (direction == "e" || direction == "east")
        return Direction::EAST;
    else if (direction == "w" || direction == "west")
        return Direction::WEST;
    else if (direction == "ne" || direction == "northeast")
        return Direction::NORTHEAST;
    else if (direction == "nw" || direction == "northwest")
        return Direction::NORTHWEST;
    else if (direction == "se" || direction == "southeast")
        return Direction::SOUTHEAST;
    else if (direction == "sw" || direction == "southwest")
        return Direction::SOUTHWEST;
    else if (direction == "m" || direction == "move")
        return Direction::MOVE;
    return Direction::INVALID;
}

std::string PrintDirection(Direction direction)
{
    switch (direction)
    {
    case Direction::NORTH:
        return "north";
    case Direction::SOUTH:
        return "south";
    case Direction::EAST:
        return "east";
    case Direction::WEST:
        return "west";
    case Direction::NORTHEAST:
        return "northeast";
    case Direction::NORTHWEST:
        return "northwest";
    case Direction::SOUTHEAST:
        return "southeast";
    case Direction::SOUTHWEST:
        return "southwest";
    default:
        return "invalid";
    }
}

World::World(const std::string& file)
{
    (void) file;
}

void World::CharacterExitZone(Coordinates coord, Character* self)
{
    Zone& zone = _zones.at(coord);
    zone._characters.erase(self);
}

void World::CharacterEnterZone(Coordinates coord, Character* self)
{
    Zone& zone = _zones.at(coord);
    zone._characters.insert(self);
}
