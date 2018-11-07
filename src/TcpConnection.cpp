#include "TcpConnection.h"
#include "TcpServer.h"

#include <cstdio>
#include <functional>

TcpConnection::TcpConnection(boost::asio::io_service& ioService, size_t id, TcpServer* server)
    : _id(id), _socket(ioService), _timeout(ioService)
    , _message("Welcome to Age of Wujin MUD! What is the name of our hero?\n\nName:")
    , _server(server)
{
    _timeout.async_wait(std::bind(&TcpConnection::CheckTimeout, this));
}

TcpConnection::~TcpConnection()
{
    printf("~TcpConnection()\n");
    _socket.close();
    _timeout.cancel();
    _connected = false;
}

std::unique_ptr<TcpConnection> TcpConnection::Make(boost::asio::io_service& ioService,
                                                   size_t id, TcpServer* server)
{
    return std::make_unique<TcpConnection>(ioService, id, server);
}

void TcpConnection::Stop()
{
    _connected = false;
}

std::string TcpConnection::GetNextCommand()
{
    return _commands.GetCommand();
}

void TcpConnection::StartWrite()
{
    if (_message.empty())
        return;

    _message.append("\n");
    boost::asio::async_write(_socket, boost::asio::buffer(_message),
                             std::bind(&TcpConnection::HandleWrite, this,
                                       std::placeholders::_1));
}

void TcpConnection::HandleWrite(const boost::system::error_code& error)
{
    if (error)
    {
        printf("boost::asio::async_write returned error %s after sending message %s",
               error.message().c_str(), _message.c_str());
    }

    _message.clear();

    if (!_connected)
    {
        _timeout.cancel();
        _socket.close();
    }
}

void TcpConnection::StartRead()
{
    _timeout.expires_from_now(boost::posix_time::seconds(30));
    boost::asio::async_read_until(_socket, _inputBuffer, '\n',
                                  std::bind(&TcpConnection::HandleRead, this,
                                            std::placeholders::_1));
}

void TcpConnection::HandleRead(const boost::system::error_code& error)
{
    if (!_connected)
        return;

    if (error == boost::asio::error::eof ||
        error == boost::asio::error::connection_reset)
    {
        _server->DestroyConnection(_id);
    }

    if (!error)
    {
        std::string line;
        std::istream instream(&_inputBuffer);
        std::getline(instream, line);

        if (!line.empty())
        {
            printf("Received message: %s\n", line.c_str());
            if (!_commands.AddCommand(line.c_str()))
            {
                printf("User spammed too many commands: this command dropped: %s", line.c_str());
            }
        }

        StartRead();
    }
    else
    {
        printf("boost::asio::async_read returned error %s reading socket\n",
               error.message().c_str());
    }
}

void TcpConnection::CheckTimeout()
{
    if (!_connected)
        return;

    if (_timeout.expires_at() <= boost::asio::deadline_timer::traits_type::now())
        Stop();
    else
        _timeout.async_wait(std::bind(&TcpConnection::CheckTimeout, this));
}
