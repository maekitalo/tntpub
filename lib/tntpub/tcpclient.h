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
    explicit TcpClient(const std::string& ipaddr, unsigned short int port)
        : Client(_peer),
          _peer(ipaddr, port)
          { }

    explicit TcpClient(const cxxtools::net::AddrInfo& addrinfo)
        : Client(_peer),
          _peer(addrinfo)
          { }

    explicit TcpClient(cxxtools::SelectorBase* selector, const std::string& ipaddr, unsigned short int port)
        : Client(_peer),
          _peer(ipaddr, port)
      {
          _peer.attachedDevice()->setSelector(selector);
      }

    explicit TcpClient(cxxtools::SelectorBase* selector, const cxxtools::net::AddrInfo& addrinfo)
        : Client(_peer),
          _peer(addrinfo)
      {
          _peer.attachedDevice()->setSelector(selector);
      }

    cxxtools::net::TcpStream& peer()
        { return _peer; }

    void setSelector(cxxtools::SelectorBase* selector)
        { _peer.attachedDevice()->setSelector(selector); }
};

}

#endif
