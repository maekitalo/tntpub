/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SERVER_H
#define TNTPUB_SERVER_H

#include <cxxtools/connectable.h>

#include <cxxtools/signal.h>
#include <cxxtools/eventloop.h>

#include <cxxtools/net/tcpserver.h>

namespace tntpub
{

class DataMessage;
class Server;

////////////////////////////////////////////////////////////////////////
// Server
//
class Server : public cxxtools::Connectable
{
    cxxtools::net::TcpServer _server;
    cxxtools::EventLoop _eventLoop;

    void onConnectionPending(cxxtools::net::TcpServer&);

public:
    Server(int argc, char* argv[]);
    void run();

    cxxtools::net::TcpServer& server()   { return _server; }

    cxxtools::Signal<const DataMessage&> messageReceived;
};

}

#endif
