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
    cxxtools::IOStream* _stream;
    Server& _pubSubServer;
    cxxtools::bin::Deserializer _deserializer;

    std::vector<std::string> _topics;

    void onInput(cxxtools::StreamBuffer&);
    void onOutput(cxxtools::StreamBuffer&);

    void closeClient();

protected:
    void init(cxxtools::IOStream& stream);

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
