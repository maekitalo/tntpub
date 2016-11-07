/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_CLIENT_H
#define TNTPUB_CLIENT_H

#include <tntpub/datamessage.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/signal.h>
#include <cxxtools/iostream.h>
#include <iostream>

namespace tntpub
{
class Client
{
    cxxtools::IOStream& _peer;
    cxxtools::bin::Deserializer _deserializer;
    DataMessage _dataMessage;

public:
    explicit Client(cxxtools::IOStream& peer);

    Client& subscribe(const std::string& topic);
    Client& sendMessage(const DataMessage& msg);

    template <typename Obj> Client& sendMessage(const std::string& topic, const Obj& obj)
    {
        DataMessage dataMessage;
        dataMessage.topic = topic;
        dataMessage.data <<= obj;
        return sendMessage(dataMessage);
    }

    void flush()
    { _peer.flush(); }

    // Processes available input characters and returns true, if the data message is complete.
    // After it returns true, the message can be read using `getMessage`.
    bool advance();

    // returns the last received data message
    const DataMessage& getMessage()
    { return _dataMessage; }

    template <typename Obj> void getMessage(Obj& obj)
    { return _dataMessage.get(obj); }

    // reads next message from stream (blocking)
    const DataMessage& readMessage();

    // reads next message
    template <typename Obj> void readMessage(Obj& obj)
    {
        readMessage().get(obj);
    }

    cxxtools::Signal<DataMessage&> messageReceived;
    cxxtools::Signal<Client&> closed;
};

}

#endif
