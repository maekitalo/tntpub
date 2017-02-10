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
class Client : public cxxtools::Connectable
{
    cxxtools::IOStream* _peer;
    cxxtools::bin::Deserializer _deserializer;
    DataMessage _dataMessage;

    void onOutput(cxxtools::StreamBuffer& sb);
    void onInput(cxxtools::StreamBuffer& sb);

protected:
    Client()
        : _peer(0)
        { }

    void init(cxxtools::IOStream& peer);

public:
    explicit Client(cxxtools::IOStream& peer)
        { init(peer); }

    Client& subscribe(const std::string& topic);
    Client& unsubscribe(const std::string& topic);
    Client& sendMessage(const DataMessage& msg);

    template <typename Obj> Client& sendMessage(const std::string& topic, const Obj& obj)
    {
        DataMessage dataMessage(topic, obj);
        return sendMessage(dataMessage);
    }

    void flush()
    { _peer->flush(); }

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
