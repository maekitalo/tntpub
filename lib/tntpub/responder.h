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

#include <cxxtools/net/tcpsocket.h>

#include <vector>

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
    class DestructionSentry
    {
        bool _deleted;
        Responder* _responder;

    public:
        explicit DestructionSentry(Responder* responder)
            : _deleted(false),
              _responder(responder)
            { _responder->_sentry = this; }

        ~DestructionSentry()
            { if (!_deleted) _responder->_sentry = 0; }

        void detach()         { _deleted = true; }
        bool deleted() const  { return _deleted; }
    };

    DataMessageDeserializer _deserializer;

    std::vector<char> _inputBuffer;
    std::vector<char> _outputBuffer;
    std::vector<char> _outputBufferNext;

    Server& _pubSubServer;
    cxxtools::net::TcpSocket _socket;
    DestructionSentry* _sentry;

    std::vector<Subscription> _subscriptions;

    void onInput(cxxtools::IODevice&);
    void onOutput(cxxtools::IODevice&);

    void closeClient();
    void beginRead();

protected:
    virtual ~Responder();

    virtual void subscribeMessageReceived(const DataMessage& subscribeMessage);

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
