#pragma once

#include "Character.h"
#include "World.h"

#include <cstdint>
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
    static constexpr uint32_t CYCLE_USEC = 50000; // microseconds
    static constexpr uint32_t CYCLES_PER_ROUND = 10;
    static constexpr uint8_t CYCLES_PER_TICK = 255;
    static constexpr uint8_t CYCLES_PER_SAVE = 251; // coprime with 255

    Game();

    void MainLoop();
    void Shutdown();

    void AttachServer(TcpServer* server)
    {
        _server = server;
    }
    void AddNewConnection(TcpConnection* connection);
    void DisconnectById(size_t id);

private:
    void ExecuteGameCycle();
    void Tick();
    void SaveCharacters() const;
    void HandleCharacterLogin(TcpConnection* connection, const std::string& cmd);
    void HandleCharacterPassword(TcpConnection* connection, const std::string& cmd);
    void HandleCharacterCreation(TcpConnection* connection, const std::string& cmd);

    World _world;
    TcpServer* _server;
    std::unordered_map<size_t, TcpConnection*> _connections;
    std::unordered_map<size_t, Character> _characters;
    std::unordered_map<std::string, size_t> _nameToId;
    std::vector<size_t> _disconnects;
    uint32_t _elapsed = 0; // elapsed cycles since server start: won't overflow
    uint8_t _toTick = CYCLES_PER_TICK; // cycles until next tick
    uint8_t _toSave = CYCLES_PER_SAVE;
    bool _up = true;
};
