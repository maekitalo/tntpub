/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/server.h>
#include <tntpub/responder.h>

#include <cxxtools/arg.h>
#include <cxxtools/log.h>

log_define("tntpub.server")

namespace tntpub
{

////////////////////////////////////////////////////////////////////////
// Server
//
Server::Server(int argc, char* argv[])
{
    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

    cxxtools::connect(_server.connectionPending, *this, &Server::onConnectionPending);
    _server.setSelector(&_eventLoop);
    log_info("listen on " << ip.getValue() << ':' << port.getValue());
    _server.listen(ip, port);
}

void Server::run()
{
    _eventLoop.run();
}

void Server::onConnectionPending(cxxtools::net::TcpServer&)
{
    new Responder(*this);
}

}
