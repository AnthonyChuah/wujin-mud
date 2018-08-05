#include "CommandBuffer.h"
#include "Game.h"
#include "TcpConnection.h"

#include <chrono>
#include <iostream>
#include <thread>

Game::Game() : _world("world.dat")
{}

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
    std::cout << "Game adds a new connection ID " << _connectionId << "\n";
    _connections.emplace(std::make_pair(_connectionId++, connection));
}

void Game::ExecuteGameCycle()
{
    std::string command;
    std::vector<size_t> disconnected;
    for (auto& c : _connections)
    {
        TcpConnection* con = c.second;
        if (!con)
        {
            disconnected.push_back(c.first);
            continue;
        }

        command = con->GetNextCommand();
        if (con->UserInGame())
            con->GetCharacter()->ExecuteCommand(command);
        else
            HandleCharacterLogin(con, command);
        con->StartWrite();
    }

    for (auto id : disconnected)
    {
        std::cout << "Clean up a dead user connection of ID " << id << "\n";
        _connections.erase(id);
    }
}

void Game::HandleCharacterLogin(TcpConnection* connection, const std::string& cmd)
{
    if (cmd.empty())
        return;
    connection->GetOutputBuffer() = "Feature coming soon! You attempted to LOGIN as ";
    connection->GetOutputBuffer().append(cmd);
    connection->StartWrite();
}
