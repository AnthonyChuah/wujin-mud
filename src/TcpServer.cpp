#include "Game.h"
#include "TcpServer.h"

#include <functional>

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

void TcpServer::StartAccept()
{
    auto pairit = _connections.emplace(TcpConnection::Make(_acceptor.get_io_service(),
                                                           _numConnected++));
    TcpConnection* connection = pairit.first->get();
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
