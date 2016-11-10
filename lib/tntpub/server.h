/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SERVER_H
#define TNTPUB_SERVER_H

#include <cxxtools/connectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/net/tcpserver.h>

namespace tntpub
{

class DataMessage;
class Responder;

////////////////////////////////////////////////////////////////////////
// Server
//
class Server : public cxxtools::Connectable
{
    friend class Responder;

    cxxtools::net::TcpServer _server;

    void onConnectionPending(cxxtools::net::TcpServer&);

public:
    Server(cxxtools::SelectorBase& selector, const std::string& ip, unsigned short port);

    cxxtools::Signal<Responder&> clientConnected;
    cxxtools::Signal<Responder&> clientDisconnected;
    cxxtools::Signal<Responder&, const std::string&> clientSubscribed;
    cxxtools::Signal<const DataMessage&> messageReceived;
};

}

#endif
