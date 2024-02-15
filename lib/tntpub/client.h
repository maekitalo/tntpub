/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_CLIENT_H
#define TNTPUB_CLIENT_H

#include <tntpub/datamessage.h>
#include <tntpub/datamessage.h>
#include <tntpub/subscription.h>
#include <tntpub/messagesinksource.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/signal.h>
#include <cxxtools/net/tcpstream.h>
#include <vector>

namespace tntpub
{
class Client : public MessageSinkSource, public cxxtools::Connectable
{
    cxxtools::net::TcpSocket _peer;

    std::vector<char> _inputBuffer;
    std::vector<char> _outputBuffer;
    std::vector<char> _outputBufferNext;

    DataMessageDeserializer _deserializer;
    DataMessage _dataMessage;

    void onConnected(cxxtools::net::TcpSocket&);
    void onClosed(cxxtools::net::TcpSocket&);
    void onOutput(cxxtools::IODevice&);
    void onInput(cxxtools::IODevice&);
    void doSendMessage(const DataMessage& msg);

    void init();
    void beginRead();

    Client(Client&) = delete;
    Client& operator=(Client&) = delete;

public:
    explicit Client(cxxtools::SelectorBase* selector = 0)
    {
        setSelector(selector);
        init();
    }

    Client(const std::string& ipaddr, unsigned short int port)
        : _peer(ipaddr, port)
        { init(); beginRead(); }

    explicit Client(const cxxtools::net::AddrInfo& addrinfo)
        : _peer(addrinfo)
        { init(); beginRead(); }

    explicit Client(cxxtools::SelectorBase* selector, const std::string& ipaddr, unsigned short int port)
        : _peer(ipaddr, port)
    {
        setSelector(selector);
        init();
        beginRead();
    }

    explicit Client(cxxtools::SelectorBase* selector, const cxxtools::net::AddrInfo& addrinfo)
        : _peer(addrinfo)
    {
        setSelector(selector);
        init();
        beginRead();
    }

    void setSelector(cxxtools::SelectorBase* selector)
        { _peer.setSelector(selector); }

    bool beginConnect(const cxxtools::net::AddrInfo& addrinfo)
        { return _peer.beginConnect(addrinfo); }

    bool beginConnect(const std::string& ipaddr, unsigned short int port)
        { return _peer.beginConnect(ipaddr, port); }

    void endConnect()
        { _peer.endConnect(); beginRead(); }

    void connect(const cxxtools::net::AddrInfo& addrinfo)
        { _peer.connect(addrinfo); beginRead(); }

    void connect(const std::string& ipaddr, unsigned short int port)
        { _peer.connect(ipaddr, port); beginRead(); }

    void close()
        { _peer.close(); }

    bool isConnected() const
        { return _peer.isConnected(); }

    // subscribe to topic
    Client& subscribe(const std::string& topic, Subscription::Type type = Subscription::Type::Full);

    // subscribe to topic with additional information
    template <typename Obj>
    Client& subscribe(const std::string& topic, const Obj& obj, Subscription::Type type = Subscription::Type::Full)
        { doSendMessage(DataMessage::subscribe(topic, obj, type)); return *this; }

    Client& unsubscribe(const std::string& topic)
        { doSendMessage(DataMessage::unsubscribe(topic)); return *this; }

    void flush();

    // returns the last received data message
    const DataMessage& getMessage()
    { return _dataMessage; }

    template <typename Obj> void getMessage(Obj& obj)
    { return _dataMessage.si() >>= obj; }

    // reads next message from stream (blocking)
    const DataMessage& readMessage();

    // reads next message
    template <typename Obj> void readMessage(Obj& obj)
    {
        readMessage().si() >>= obj;
    }

    cxxtools::Signal<Client&> connected;
    cxxtools::Signal<Client&> closed;
    cxxtools::Signal<Client&> messagesSent;
};

}

#endif
