#include "CharacterFileLoader.h"
#include "CommandBuffer.h"
#include "Game.h"
#include "TcpConnection.h"
#include "TcpServer.h"

#include <chrono>
#include <iostream>
#include <thread>

Game::Game() : _world("world.dat")
{
    _disconnects.reserve(2); // Unlikely to get more than 2 disconnects in same 50 ms window
}

void Game::MainLoop()
{
    std::cout << "Game begins MainLoop\n";
    while (_up)
    {
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now());

        ExecuteGameCycle();

        auto end = std::chrono::time_point_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now());
        std::chrono::microseconds elapsed = end - start;
        std::chrono::microseconds wait{CYCLE_US - elapsed.count()};
        std::this_thread::sleep_for(wait);
    }
}

void Game::Shutdown()
{
    _up = false;
}

void Game::AddNewConnection(TcpConnection* connection)
{
    std::cout << "Game adds a new connection ID " << connection->GetId() << "\n";
    _connections.emplace(std::make_pair(connection->GetId(), connection));
}

/**
 * Game should wait until the next cycle before destroying connections, to allow
 * final words to be sent through the socket via async_write.
 */
void Game::ExecuteGameCycle()
{
    for (auto id : _disconnects)
    {
        std::cout << "Clean up a dead user connection of ID " << id << "\n";
        _connections.erase(id);
        _server->DestroyConnection(id);
    }
    _disconnects.clear();

    std::string command;
    for (auto& c : _connections)
    {
        TcpConnection* con = c.second;
        if (!con)
        {
            _disconnects.push_back(c.first);
            continue;
        }

        command = con->GetNextCommand();
        if (con->UserInGame())
            con->GetCharacter()->ExecuteCommand(command);
        else
            HandleCharacterLogin(con, command);
        con->StartWrite();
    }
}

void Game::HandleCharacterLogin(TcpConnection* connection, const std::string& cmd)
{
    if (cmd.empty())
        return;
    // connection->GetOutputBuffer() = "Feature coming soon! You attempted to LOGIN as ";
    // connection->GetOutputBuffer().append(cmd);
    // connection->StartWrite();

    CharacterFileLoader loader(cmd.c_str());
    if (loader)
    {
        Character character(connection->GetOutputBuffer(), _world, loader.GetName());
        if (loader.LoadCharacterData(character))
        {
            // move character into Game's unordered_map of Characters
        }
        else
        {
            std::cout << "Failed to load character file " << loader.GetName() << "\n";
        }
    }
}
