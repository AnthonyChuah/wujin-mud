#include "CharacterFileLoader.h"
#include "CommandBuffer.h"
#include "Game.h"
#include "TcpConnection.h"
#include "TcpServer.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <thread>

namespace
{

void ToProperCase(std::string& str)
{
    str[0] = toupper(str[0]);
    for (size_t i = 1; i < str.length(); ++i)
        str[i] = tolower(str[i]);
}

}

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
        std::chrono::microseconds wait{CYCLE_USEC - elapsed.count()};
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

        auto iter = _characters.find(id);
        if (iter != _characters.end())
        {
            std::string nameToDelete = _characters.at(id).GetName();
            _characters.erase(id);
            _nameToId.erase(nameToDelete);
        }

        _server->DestroyConnection(id);
    }
    _disconnects.clear();

    // Make the world trigger any non-player-initiated events
    // e.g. damage over time, debuff fading

    std::string command;
    for (auto& c : _connections)
    {
        TcpConnection* con = c.second;
        if (!con)
        {
            _disconnects.push_back(c.first);
            continue;
        }

        if (!con->HasNextCommand())
            continue;

        command = con->GetNextCommand();
        command = command.substr(0, command.find_first_of("\n\r"));

        if (command.empty())
            return;

        if (con->UserInGame())
            con->GetCharacter()->ExecuteCommand(command);
        else if (con->UserTryingLogin())
            HandleCharacterPassword(con, command);
        else
            HandleCharacterLogin(con, command);
    }

    for (auto& c : _connections)
        c.second->StartWrite(); // Flush all buffered messages to all connected clients
}

void Game::HandleCharacterLogin(TcpConnection* connection, const std::string& cmd)
{
    // connection->GetOutputBuffer() = "Feature coming soon! You attempted to LOGIN as ";
    // connection->GetOutputBuffer().append(cmd);
    // connection->StartWrite();

    // cmd contains a string which is the name of the character he wants to login as
    // Validate name (all letters no spaces)
    auto it = std::find_if(cmd.begin(), cmd.end(), [](char c)
                           {
                               return !isalpha(c);
                           });
    if (it != cmd.end())
    {
        std::string& output = connection->GetOutputBuffer();
        output.append("Invalid login name. Only letters allowed, no spaces. Try again.\n");
        return;
    }

    std::string name = cmd;
    ToProperCase(name);

    // Then, ask for password and set user state to "trying login"
    connection->SetUserTryingLogin();
    connection->SetLogin(name);
    connection->GetOutputBuffer().append("Trying to login as player character ");
    connection->GetOutputBuffer().append(name);
    connection->GetOutputBuffer().append(". Password:");
}

void Game::HandleCharacterPassword(TcpConnection* connection, const std::string& cmd)
{
    const std::string& name = connection->GetLogin();

    auto iter = _nameToId.find(name);
    if (iter != _nameToId.end())
    {
        size_t oldId = iter->second;
        Character existing = _characters.find(oldId)->second;
        auto pairiter = _characters.emplace(std::make_pair(connection->GetId(), std::move(existing)));
        _nameToId.emplace(std::make_pair(name, connection->GetId()));

        _disconnects.push_back(oldId);

        connection->GetOutputBuffer().append("Character is already logged on. Reconnecting.\n");
        connection->AttachCharacter(&pairiter.first->second);
        return;
    }

    CharacterFileLoader loader(name.c_str(), cmd);
    if (loader)
    {
        auto pairiter = _characters.emplace(std::piecewise_construct,
                                            std::forward_as_tuple(connection->GetId()),
                                            std::forward_as_tuple(&connection->GetOutputBuffer(),
                                                                  &_world, loader));
        _nameToId.emplace(std::make_pair(name, connection->GetId()));

        std::cout << "Loaded Character whose ID is " << connection->GetId() << "\n";

        connection->GetOutputBuffer().append("Welcome to AgeOfWujin MUD, ");
        connection->GetOutputBuffer().append(name);
        connection->GetOutputBuffer().append("!");
        connection->AttachCharacter(&pairiter.first->second);
    }
    else
    {
        std::cout << "Failed to load character file " << loader.GetName() << "\n";
        std::cout << "Attempt to do character creation!\n";
        connection->GetOutputBuffer().append("Character does not exist. Character creation not yet implemented.\n");
        connection->GetOutputBuffer().append("Please reconnect and try again.");

        _disconnects.push_back(connection->GetId());
        // Implement later
    }
}
