/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "mymessage.h"

#include <tntpub/tcpclient.h>

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
        // initialize logging (read `log.properties` or `log.xml`)
        log_init();

        // accept arguments -i for liste ip and -p for port
        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        // create tcp client application
        tntpub::TcpClient client(ip, port);

        // subscribe to topics passed as arguments
        for (int a = 1; a < argc; ++a)
            client.subscribe(argv[a]);

        // since output is buffered we need to force sending
        // the subscribe message
        client.flush();

        while (true)
        {
            // wait for a message and deserialize it
            MyMessage msg;
            client.readMessage().get(msg);

            log_info("got message");

            // output in json format
            std::cout << cxxtools::Json(msg).beautify(true);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
