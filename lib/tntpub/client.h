/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_CLIENT_H
#define TNTPUB_CLIENT_H

#include <tntpub/datamessage.h>
#include <tntpub/messagesink.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/signal.h>
#include <cxxtools/net/tcpstream.h>

namespace tntpub
{
class Client : public MessageSink, public cxxtools::Connectable
{
    cxxtools::net::TcpStream _peer;
    cxxtools::bin::Deserializer _deserializer;
    DataMessage _dataMessage;

    void onConnected(cxxtools::net::TcpStream&);
    void onClosed(cxxtools::net::TcpStream&);
    void onOutput(cxxtools::StreamBuffer& sb);
    void onInput(cxxtools::StreamBuffer& sb);
    void doSendMessage(const DataMessage& msg);

    void init();
    void begin();

public:
    explicit Client(cxxtools::SelectorBase* selector = 0)
    {
        if (selector)
            setSelector(selector);
        init();
    }

    Client(const std::string& ipaddr, unsigned short int port)
        : _peer(ipaddr, port)
        { init(); begin(); }

    explicit Client(const cxxtools::net::AddrInfo& addrinfo)
        : _peer(addrinfo)
        { init(); begin(); }

    explicit Client(cxxtools::SelectorBase* selector, const std::string& ipaddr, unsigned short int port)
        : _peer(ipaddr, port)
    {
        setSelector(selector);
        init();
        begin();
    }

    explicit Client(cxxtools::SelectorBase* selector, const cxxtools::net::AddrInfo& addrinfo)
        : _peer(addrinfo)
    {
        setSelector(selector);
        init();
        begin();
    }

    void setSelector(cxxtools::SelectorBase* selector)
        { _peer.attachedDevice()->setSelector(selector); }

    bool beginConnect(const cxxtools::net::AddrInfo& addrinfo)
    { return _peer.beginConnect(addrinfo); }

    bool beginConnect(const std::string& ipaddr, unsigned short int port)
    { return _peer.beginConnect(ipaddr, port); }

    void endConnect()
    { _peer.endConnect(); begin(); }

    void connect(const cxxtools::net::AddrInfo& addrinfo)
    { _peer.connect(addrinfo); begin(); }

    void connect(const std::string& ipaddr, unsigned short int port)
    { _peer.connect(ipaddr, port); begin(); }

    void close()
    { _peer.close(); }

    Client& subscribe(const std::string& topic);
    Client& unsubscribe(const std::string& topic);

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

    cxxtools::Signal<Client&> connected;
    cxxtools::Signal<DataMessage&> messageReceived;
    cxxtools::Signal<Client&> closed;
};

}

#endif
