/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "mymessage.h"

#include <tntpub/client.h>

#include <cxxtools/clock.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>

log_define("tntpub.readerBenchmarker")

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<bool> noparse(argc, argv, 'n');

        tntpub::Client client(ip, port);

        for (int a = 1; a < argc; ++a)
            client.subscribe(argv[a]);

        client.flush();

        cxxtools::Timespan lastT = cxxtools::Clock::getSystemTicks();
        cxxtools::Timespan nextT = lastT + cxxtools::Seconds(1);
        unsigned count = 0;

        while (true)
        {
            if (noparse)
            {
                client.readMessage();
            }
            else
            {
                MyMessage msg;
                client.readMessage().get(msg);
            }
            ++count;
            cxxtools::Timespan t = cxxtools::Clock::getSystemTicks();
            if (t >= nextT)
            {
                std::cout << count << '\t' << count/cxxtools::Seconds(t - lastT) << " msg/s" << std::endl;
                lastT = t;
                nextT = lastT + cxxtools::Seconds(1);
                count = 0;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
