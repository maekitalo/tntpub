/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "mymessage.h"

#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/json.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>
#include <vector>

class AsyncReader : public cxxtools::Connectable
{
    tntpub::Client _client;
    std::vector<std::string> _topics;

    void onConnected(tntpub::Client&);
    void onCloseClient(tntpub::Client&);
    void onMessageReceived(tntpub::DataMessage& message);

public:
    AsyncReader(cxxtools::EventLoop& eventLoop, const std::string& ip, unsigned short port);

    // subscriptions are collected until the connection is set up
    void subscribe(const std::string& topic)
        { _topics.push_back(topic); }

    // we define a signal, which tells, when the connection is closed
    cxxtools::Signal<> exit;
};

AsyncReader::AsyncReader(cxxtools::EventLoop& eventLoop, const std::string& ip, unsigned short port)
    : _client(&eventLoop)
{
    // connect signals to slots
    cxxtools::connect(_client.connected, *this, &AsyncReader::onConnected);
    cxxtools::connect(_client.closed, *this, &AsyncReader::onCloseClient);
    cxxtools::connect(_client.messageReceived, *this, &AsyncReader::onMessageReceived);

    // initiate connection
    _client.beginConnect(ip, port);
}

void AsyncReader::onConnected(tntpub::Client&)
{
    // finalize connection
    _client.endConnect();

    // send subscribe messages
    for (unsigned n = 0; n < _topics.size(); ++n)
        _client.subscribe(_topics[n]);

    std::cout << "connected" << std::endl;
}

// handler, which is called when the connection is closed by the server
void AsyncReader::onCloseClient(tntpub::Client&)
{
    exit();
}

// handler, which is called when a message is received
void AsyncReader::onMessageReceived(tntpub::DataMessage& message)
{
    MyMessage msg;
    message.get(msg);
    std::cout << cxxtools::Json(msg).beautify(true);
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

        // we need a event loop
        cxxtools::EventLoop eventLoop;

        // create reader object
        AsyncReader reader(eventLoop, ip, port);

        cxxtools::connect(reader.exit, eventLoop, &cxxtools::EventLoop::exit);

        // subscribe to topics passed as arguments
        for (int a = 1; a < argc; ++a)
            reader.subscribe(argv[a]);

        // run the event loop
        eventLoop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
