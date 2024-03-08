/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>

#include <cxxtools/serializationinfo.h>
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
        cxxtools::Arg<unsigned> count(argc, argv, 'n', 1);
        unsigned flush = cxxtools::Arg<unsigned>(argc, argv, 'f', 100); // flush after n messages

        if (argc != 2)
        {
            std::cerr << "usage: " << argv[0] << " [-i ip] [-p port] [-n count] topic\n";
            return 1;
        }

        std::string topic = argv[1];

        tntpub::Client client(ip, port);
        client.autoSync(flush == 0);

        cxxtools::SerializationInfo msg;

        try
        {
            // read messages from cin until it fails

            unsigned fcount = 0;
            while (std::cin >> cxxtools::Json(msg))
            {
                auto dm = tntpub::DataMessage::create(topic, msg);
                for (unsigned n = 0; n < count; ++n)
                {
                    dm.touch();
                    client.sendMessage(dm);
                    ++fcount;
                    if (!client.autoSync() && fcount % flush == 0)
                        client.flush();
                }
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
