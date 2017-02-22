/*
 * Copyright (C) 2017 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_TCPCLIENT_H
#define TNTPUB_TCPCLIENT_H

#include <tntpub/client.h>
#include <cxxtools/net/tcpstream.h>

namespace tntpub
{
class TcpClient : public Client
{
    cxxtools::net::TcpStream _peer;

public:
    TcpClient()
        { }

    explicit TcpClient(const std::string& ipaddr, unsigned short int port)
        : _peer(ipaddr, port)
        { init(_peer); }

    explicit TcpClient(const cxxtools::net::AddrInfo& addrinfo)
        : _peer(addrinfo)
        { init(_peer); }

    explicit TcpClient(cxxtools::SelectorBase* selector, const std::string& ipaddr, unsigned short int port)
        : _peer(ipaddr, port)
    {
        _peer.attachedDevice()->setSelector(selector);
        init(_peer);
    }

    explicit TcpClient(cxxtools::SelectorBase* selector, const cxxtools::net::AddrInfo& addrinfo)
        : _peer(addrinfo)
    {
        _peer.attachedDevice()->setSelector(selector);
        init(_peer);
    }

    cxxtools::net::TcpStream& peer()
        { return _peer; }

    void setSelector(cxxtools::SelectorBase* selector)
        { _peer.attachedDevice()->setSelector(selector); }
};

}

#endif
