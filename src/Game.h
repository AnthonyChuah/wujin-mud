#pragma once

#include "Character.h"
#include "World.h"

#include <unordered_map>

class CommandBuffer;
class TcpConnection;

/**
 * The smallest unit of time in the game is the Cycle (50 milliseconds)
 * Events that require interaction with the world / other players must happen
 * at a Cycle.
 */
class Game
{
public:
    static constexpr size_t CYCLE_US = 50000; // microseconds
    static constexpr size_t CYCLES_PER_ROUND = 10;
    static constexpr size_t ROUNDS_PER_TICK = 60;

    Game();

    void MainLoop();
    void Shutdown();

    void AddNewConnection(TcpConnection* connection);

private:
    void ExecuteGameCycle();
    void HandleCharacterLogin(TcpConnection* connection, const std::string& cmd);

    size_t _connectionId = 0;
    World _world;
    std::unordered_map<size_t, TcpConnection*> _connections;
    bool _up = true;
};
