/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/server.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<unsigned> maxOBuf(argc, argv, 'B', 0);

        std::cout << "listening on " << *ip << ':' << *port << std::endl;

        cxxtools::EventLoop eventLoop;
        tntpub::Server::maxOBuf(maxOBuf);
        tntpub::Server pubSubServer(eventLoop, ip, port);
        eventLoop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
