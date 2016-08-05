#ifndef SERVER_H
#define SERVER_H

#include "asio.hpp"
#include "protocol.h"

class ServerInterface
{
public:
  static const int BUF_SIZE = 1024;
  ServerInterface(short, Protocol &) {}
  virtual ~ServerInterface() {}
  virtual void start() = 0;
  virtual void stop() = 0;
};

class AsyncServer : public ServerInterface
{
  class Session : public std::enable_shared_from_this<Session>
  {
    asio::ip::tcp::socket _socket;
    Protocol _protocol;

    char _data[BUF_SIZE];

    void read();
    void write(std::size_t length);
  public:
    Session(asio::ip::tcp::socket socket, Protocol protocol);
    void start();
  };
  asio::io_service _io_service;
  asio::ip::tcp::acceptor _acceptor;
  asio::ip::tcp::socket _socket;
  Protocol _protocol;

  void accept();
public:
  AsyncServer(short port, Protocol &protocol);
  void start() override;
  void stop() override;
};

class ThreadedServer : public ServerInterface
{
  short _port;
  Protocol _protocol;
  std::atomic<bool> _stop;
public:
  ThreadedServer(short port, Protocol &protocol);
  void start() override;
  void stop() override;
};

#endif // SERVER_H
