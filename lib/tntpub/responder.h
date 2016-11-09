/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_RESPONDER_H
#define TNTPUB_RESPONDER_H

#include <cxxtools/connectable.h>

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
    cxxtools::net::TcpStream _stream;
    Server& _pubSubServer;
    cxxtools::bin::Deserializer _deserializer;

    std::vector<std::string> _topics;

    void onInput(cxxtools::StreamBuffer&);
    void onOutput(cxxtools::StreamBuffer&);

    void closeClient();

public:
    explicit Responder(Server& pubSubServer);

    void onDataMessageReceived(const DataMessage&);
};

}

#endif
