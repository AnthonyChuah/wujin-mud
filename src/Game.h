#pragma once

#include "Character.h"
#include "World.h"

#include <unordered_map>

class CommandBuffer;
class TcpConnection;
class TcpServer;

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
    void HandleCharacterPassword(TcpConnection* connection, const std::string& cmd);

    World _world;
    TcpServer* _server;
    std::unordered_map<size_t, TcpConnection*> _connections;
    std::unordered_map<size_t, Character> _characters;
    std::unordered_map<std::string, size_t> _nameToId;
    std::vector<size_t> _disconnects;
    bool _up = true;
};
