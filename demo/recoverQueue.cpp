/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/server.h>
#include <tntpub/datamessage.h>
#include <tntpub/responder.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <iostream>
#include <deque>

std::string persistantTopic;
std::deque<tntpub::DataMessage> messages;

void onClientSubscribed(tntpub::Responder& client, const std::string& topic)
{
    if (topic == persistantTopic)
    {
        for (const auto& dm: messages)
            client.onDataMessageReceived(dm);
    }
}

void onDataMessageReceived(const tntpub::DataMessage& dm)
{
    if (dm.topic == persistantTopic)
        messages.push_back(dm);
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        cxxtools::EventLoop eventLoop;
        tntpub::Server pubSubServer(eventLoop, ip, port);

        persistantTopic = "foobar";

        cxxtools::connect(pubSubServer.clientSubscribed, &onClientSubscribed);
        cxxtools::connect(pubSubServer.messageReceived, &onDataMessageReceived);

        eventLoop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
