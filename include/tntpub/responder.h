/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_RESPONDER_H
#define TNTPUB_RESPONDER_H

#include <tntpub/subscription.h>
#include <tntpub/datamessage.h>
#include <cxxtools/connectable.h>
#include <cxxtools/signal.h>

#include <cxxtools/net/bufferedsocket.h>

#include <vector>

namespace cxxtools
{
    class DestructionSentry;
}

namespace tntpub
{

class Server;

////////////////////////////////////////////////////////////////////////
// Responder
//
// A Responder may send messages to this server or subscribe
// for messages
//
class Responder : public cxxtools::Connectable
{
    DataMessageDeserializer _deserializer;

    Server& _pubSubServer;
    cxxtools::net::BufferedSocket _socket;
    cxxtools::DestructionSentry* _sentry = nullptr;

    std::vector<Subscription> _subscriptions;

    void onInput(cxxtools::net::BufferedSocket&);
    void onOutputBufferEmpty(cxxtools::net::BufferedSocket&);
    void onOutputError(cxxtools::net::BufferedSocket&, const std::exception&);

    void closeClient();
    void beginRead();

protected:
    virtual ~Responder();

    virtual void subscribeMessageReceived(const DataMessage& subscribeMessage);
    virtual void unsubscribeMessageReceived(const DataMessage& unsubscribeMessage);
    virtual void systemMessageReceived(const DataMessage& systemMessage);

public:
    explicit Responder(Server& pubSubServer);

    void subscribe(const DataMessage& subscribeMessage);
    void subscribe(const std::string& topic, Subscription::Type type = Subscription::Type::Full);

    bool isSubscribed(const std::string& topic);
    const std::vector<Subscription>& subscriptions() const  { return _subscriptions; }

    void onDataMessageReceived(const DataMessage&);
    void sendMessage(const DataMessage& dataMessage);

    // signals that all messages has been sent to the peer
    cxxtools::Signal<Responder&> outputBufferEmpty;
};

}

#endif
