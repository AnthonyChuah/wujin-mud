#include "TcpConnection.h"

#include <cstdio>
#include <functional>

TcpConnection::TcpConnection(boost::asio::io_service& ioService, size_t id)
    : _id(id), _socket(ioService), _timeout(ioService)
    , _message("Welcome to Age of Wujin MUD!\n")
{
    _timeout.async_wait(std::bind(&TcpConnection::CheckTimeout, this));
}

std::unique_ptr<TcpConnection> TcpConnection::Make(boost::asio::io_service& ioService,
                                                   size_t id)
{
    return std::make_unique<TcpConnection>(ioService, id);
}

void TcpConnection::Stop()
{
    _message = "Your connection has timed out. Please connect again.\n";
    _connected = false;
    StartWrite();
}

std::string TcpConnection::GetNextCommand()
{
    return _commands.GetCommand();
}

void TcpConnection::StartWrite()
{
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
        printf("boost::asio::async_read returned error %s reading socket",
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
