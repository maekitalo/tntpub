/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SERVER_H
#define TNTPUB_SERVER_H

#include <tntpub/messagesinksource.h>

#include <cxxtools/connectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/net/tcpserver.h>

namespace tntpub
{

class Responder;

////////////////////////////////////////////////////////////////////////
// Server
//
class Server : public MessageSinkSource, public cxxtools::Connectable
{
    friend class Responder;

    cxxtools::net::TcpServer _server;

    void onConnectionPending(cxxtools::net::TcpServer&);

    virtual void processMessage(Responder& client, DataMessage& dataMessage);

protected:
    virtual Responder* createResponder();

public:
    Server(cxxtools::SelectorBase& selector);
    Server(cxxtools::SelectorBase& selector, const std::string& ip, unsigned short port);

    virtual ~Server() = default;

    cxxtools::SelectorBase* selector()
        { return _server.selector(); }

    void listen(const std::string& ip, unsigned short port);

    void doSendMessage(const DataMessage& msg);

    cxxtools::Signal<Responder&> clientConnected;
    cxxtools::Signal<Responder&> clientDisconnected;
    cxxtools::Signal<Responder&, const DataMessage&> clientSubscribed;
};

}

#endif
