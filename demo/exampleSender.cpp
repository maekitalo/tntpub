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

        tntpub::TcpClient client(ip, port);

        MyMessage msg;

        try
        {
            // read messages from cin until it fails

            while (std::cin >> cxxtools::Json(msg))
                client.sendMessage(topic, msg);
        }
        catch (const cxxtools::SerializationError&)
        {
        }

        client.flush();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

