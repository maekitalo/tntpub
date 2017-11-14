/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_RESPONDER_H
#define TNTPUB_RESPONDER_H

#include <cxxtools/connectable.h>
#include <cxxtools/signal.h>

#include <cxxtools/bin/deserializer.h>

#include <cxxtools/net/tcpstream.h>

#include <vector>

namespace tntpub
{

class DataMessage;
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

    cxxtools::IOStream* _stream;
    Server& _pubSubServer;
    cxxtools::bin::Deserializer _deserializer;
    DestructionSentry* _sentry;

    std::vector<std::string> _topics;

    void onInput(cxxtools::StreamBuffer&);
    void onOutput(cxxtools::StreamBuffer&);

    void closeClient();

protected:
    void init(cxxtools::IOStream& stream);
    ~Responder();

public:
    explicit Responder(Server& pubSubServer);

    void onDataMessageReceived(const DataMessage&);

    cxxtools::StreamBuffer& buffer()
    { return _stream->buffer(); }

    // signals that all messages has been sent to the peer
    cxxtools::Signal<Responder&> outputBufferEmpty;
};

class TcpResponder : public Responder
{
    cxxtools::net::TcpStream _netstream;

public:
    explicit TcpResponder(Server& pubSubServer);
};

}

#endif
