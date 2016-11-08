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

log_define("tntpub.exampleReader")

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        cxxtools::net::TcpStream peer(ip, port);
        tntpub::Client client(peer);

        for (int a = 1; a < argc; ++a)
            client.subscribe(argv[a]);

        peer.flush();

        while (true)
        {
            MyMessage msg;
            client.readMessage().get(msg);

            log_info("got message");

            std::cout << cxxtools::Json(msg).beautify(true);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
