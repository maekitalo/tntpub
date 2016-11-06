/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "mymessage.h"

#include <tntpub/client.h>

#include <cxxtools/net/tcpstream.h>
#include <cxxtools/json.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        if (argc != 2)
        {
            std::cerr << "usage: " << argv[0] << " [-i ip] [-p port] topic\n";
            return 1;
        }

        std::string topic = argv[1];

        cxxtools::net::TcpStream peer(ip, port);
        tntpub::Client client(peer);

        MyMessage msg;
        while (std::cin >> cxxtools::Json(msg))
        {
            client.sendMessage(topic, msg);
            peer.flush();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

