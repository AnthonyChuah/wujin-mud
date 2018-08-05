#pragma once

#include "TcpConnection.h"

#include <boost/asio.hpp>
#include <memory>
#include <unordered_set>

class Game;

class TcpServer
{
public:
    TcpServer(unsigned port, Game* game);

    void Run();
private:
    void StartAccept();
    void HandleAccept(TcpConnection* connection, const boost::system::error_code& error);

    size_t _numConnected = 0;
    boost::asio::io_service _ioService;
    boost::asio::ip::tcp::acceptor _acceptor;
    std::unordered_set<std::unique_ptr<TcpConnection>> _connections;
    Game* _game;
};
