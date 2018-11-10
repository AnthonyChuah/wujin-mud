#pragma once

#include "CommandBuffer.h"

#include <boost/asio.hpp>
#include <memory>
#include <string>

class Character;
class TcpServer;

class TcpConnection
{
public:
    static std::unique_ptr<TcpConnection> Make(boost::asio::io_service& ioService, size_t id, TcpServer* server);

    TcpConnection(boost::asio::io_service& ioService, size_t id, TcpServer* server);
    ~TcpConnection();

    boost::asio::ip::tcp::socket& GetSocket()
    {
        return _socket;
    }

    void AttachCharacter(Character* character);
    void DetachCharacter()
    {
        _character = nullptr;
    }

    void StartWrite();
    void StartRead();

    void Stop();

    std::string& GetOutputBuffer()
    {
        return _message;
    }
    size_t GetId() const
    {
        return _id;
    }
    void SetId(size_t id)
    {
        _id = id;
    }

    bool HasNextCommand() const
    {
        return _commands.GetSize() > 0;
    }

    std::string GetNextCommand();
    Character* GetCharacter()
    {
        return _character;
    }
    bool UserInGame() const
    {
        return _character != nullptr;
    }
    bool UserTryingLogin() const
    {
        return _tryLogin;
    }
    void SetUserTryingLogin()
    {
        _tryLogin = true;
    }
    const std::string& GetLogin() const
    {
        return _login;
    }
    void SetLogin(const std::string& login)
    {
        _login = login;
    }

    friend bool operator==(const TcpConnection& left, const TcpConnection& right)
    {
        return left._id == right._id;
    }
    friend bool operator!=(const TcpConnection& left, const TcpConnection& right)
    {
        return !(left == right);
    }

private:
    void HandleWrite(const boost::system::error_code& error);
    void HandleRead(const boost::system::error_code& error);
    void CheckTimeout();

    size_t _id;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::deadline_timer _timeout;
    boost::asio::streambuf _inputBuffer;
    std::string _message;
    std::string _login;
    CommandBuffer _commands;
    TcpServer* _server;
    Character* _character = nullptr;
    bool _connected = true;
    bool _tryLogin = false;
};
