#include "TcpConnection.h"

#include <functional>

TcpConnection::TcpConnection(boost::asio::io_service& ioService, size_t id)
    : _id(id), _socket(ioService)
{
}

std::unique_ptr<TcpConnection> TcpConnection::Make(boost::asio::io_service& ioService,
                                                          size_t id)
{
    return std::make_unique<TcpConnection>(ioService, id);
}

void TcpConnection::Start()
{
    _message = "Intro Message\n";

    boost::asio::async_write(_socket, boost::asio::buffer(_message),
                             std::bind(&TcpConnection::HandleWrite, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}

void TcpConnection::HandleWrite(const boost::system::error_code& error,
                                size_t bytes)
{
    if (!error)
    {
        (void) bytes;
    }
}
