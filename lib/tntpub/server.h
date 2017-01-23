/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SERVER_H
#define TNTPUB_SERVER_H

#include <tntpub/datamessage.h>

#include <cxxtools/connectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/net/tcpserver.h>

namespace tntpub
{

class Responder;
class TcpResponder;

////////////////////////////////////////////////////////////////////////
// Server
//
class Server : public cxxtools::Connectable
{
    friend class Responder;
    friend class TcpResponder;

    cxxtools::net::TcpServer _server;

    void onConnectionPending(cxxtools::net::TcpServer&);

    virtual void processMessage(Responder& client, DataMessage& dataMessage);

public:
    Server(cxxtools::SelectorBase& selector);
    Server(cxxtools::SelectorBase& selector, const std::string& ip, unsigned short port);

    cxxtools::SelectorBase* selector()
        { return _server.selector(); }

    void listen(const std::string& ip, unsigned short port);

    Server& sendMessage(const DataMessage& msg)
        { messageReceived(msg); return *this; }

    template <typename Obj> Server& sendMessage(const std::string& topic, const Obj& obj)
    {
        DataMessage dataMessage;
        dataMessage.topic = topic;
        dataMessage.data <<= obj;
        return sendMessage(dataMessage);
    }

    cxxtools::Signal<Responder&> clientConnected;
    cxxtools::Signal<Responder&> clientDisconnected;
    cxxtools::Signal<Responder&, const std::string&> clientSubscribed;
    cxxtools::Signal<const DataMessage&> messageReceived;
};

}

#endif
