#include "World.h"

#include <cmath>
#include <fstream>

uint8_t GetDistance(Coordinates from, Coordinates to)
{
    uint8_t xdiff = std::abs(to.x - from.x);
    uint8_t ydiff = std::abs(to.y - from.y);
    return xdiff > ydiff ? xdiff : ydiff;
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
    return Direction::INVALID;
}

World::World(const std::string& file)
{
    (void) file;
}

void World::RemoveCharacter(Character* character)
{
    (void) character;
}
