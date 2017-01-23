
/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "mymessage.h"

#include <tntpub/tcpclient.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/clock.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>

log_define("tntpub.readerBenchmarker")

cxxtools::EventLoop loop;
cxxtools::Timespan lastT;
cxxtools::Timespan nextT;
unsigned count;

void onMessageReceived(tntpub::DataMessage& message)
{
    MyMessage msg;
    message.get(msg);

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

void onCloseClient(tntpub::Client&)
{
    loop.exit();
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        tntpub::TcpClient client(&loop, ip, port);

        for (int a = 1; a < argc; ++a)
            client.subscribe(argv[a]);

        cxxtools::connect(client.messageReceived, onMessageReceived);
        cxxtools::connect(client.closed, onCloseClient);

        lastT = cxxtools::Clock::getSystemTicks();
        nextT = lastT + cxxtools::Seconds(1);
        count = 0;

        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
