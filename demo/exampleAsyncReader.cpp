/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "mymessage.h"

#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/net/tcpstream.h>
#include <cxxtools/json.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>

cxxtools::EventLoop loop;

// handler, which is called when a message is received
void onMessageReceived(tntpub::DataMessage& message)
{
    MyMessage msg;
    message.get(msg);
    std::cout << cxxtools::Json(msg).beautify(true);
}

// handler, which is called when the connection is closed by the server
void onCloseClient(tntpub::Client&)
{
    loop.exit();
}

int main(int argc, char* argv[])
{
    try
    {
        // initialize logging (read `log.properties` or `log.xml`)
        log_init();

        // accept arguments -i for liste ip and -p for port
        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        // create transport object and client application
        cxxtools::net::TcpStream peer(ip, port);
        peer.attachedDevice()->setSelector(&loop);
        tntpub::Client client(peer);

        // subscribe to topics passed as arguments
        for (int a = 1; a < argc; ++a)
            client.subscribe(argv[a]);

        // we do not flush the output buffer since this is done
        // asyncronously by the event loop

        // connect signals to slots
        cxxtools::connect(client.messageReceived, onMessageReceived);
        cxxtools::connect(client.closed, onCloseClient);

        // run the event loop
        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
