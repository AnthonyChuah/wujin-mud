#include "Game.h"
#include "TcpServer.h"

#include <functional>
#include <iostream>

TcpServer::TcpServer(unsigned port, Game* game)
    : _acceptor(_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , _game(game)
{
    StartAccept();
}

void TcpServer::Run()
{
    _ioService.run();
}

bool TcpServer::DestroyConnection(size_t id)
{
    if (!_connections.erase(id))
    {
        std::cout << "Attempted to erase non-existent connection ID " << id << "\n";
        return false;
    }
    return true;
}

void TcpServer::StartAccept()
{
    auto pairit = _connections.emplace(
        std::make_pair(_numConnected, TcpConnection::Make(_acceptor.get_io_service(),
                                                          _numConnected, this)));
    ++_numConnected;
    TcpConnection* connection = pairit.first->second.get();
    _acceptor.async_accept(connection->GetSocket(),
                           std::bind(&TcpServer::HandleAccept, this, connection,
                                     std::placeholders::_1));
}

void TcpServer::HandleAccept(TcpConnection* connection, const boost::system::error_code& error)
{
    if (!error)
    {
        connection->StartRead();
        connection->StartWrite();
        _game->AddNewConnection(connection);
    }
    StartAccept();
}
