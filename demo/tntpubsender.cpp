/*
 * Copyright (C) 2017 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>

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
        cxxtools::Arg<std::string> type(argc, argv, 't');
        cxxtools::Arg<bool> sync(argc, argv, 's');

        if (argc != 2)
        {
            std::cerr << "reads messages in json format from standard input and send to the specified topic\n"
                         "usage: " << argv[0] << " {options} topic\n"
                         "options:\n"
                         "-i <ip>     ip address of pubsub server (default: localhost)\n"
                         "-p <port>   port of pubsub server (default: 9001)\n"
                         "-t <type>   type name of json messages\n"
                         "-s          syncronize after every message\n";
            return 1;
        }

        std::string topic = argv[1];

        tntpub::Client client(ip, port);

        cxxtools::SerializationInfo si;

        try
        {
            // read messages from cin until it fails

            while (std::cin >> cxxtools::Json(si))
            {
                si.setTypeName(type);
                client.sendMessage(topic, si);
                if (sync)
                    client.flush();
            }
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
