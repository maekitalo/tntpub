/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <iostream>

class AsyncReader : public cxxtools::Connectable
{
    tntpub::Client _client;
    std::string _topic;

    void onConnected(tntpub::Client&);
    void onCloseClient(tntpub::Client&);
    void onMessageReceived(const tntpub::DataMessage& message);

public:
    AsyncReader(cxxtools::EventLoop& eventLoop, const std::string& ip, unsigned short port, const std::string& topic);

    // subscriptions are collected until the connection is set up
    // we define a signal, which tells, when the connection is closed
    cxxtools::Signal<> exit;
};

AsyncReader::AsyncReader(cxxtools::EventLoop& eventLoop, const std::string& ip, unsigned short port, const std::string& topic)
    : _client(&eventLoop),
      _topic(topic)
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

    // send subscribe message
    _client.subscribe(_topic);

    std::cout << "connected" << std::endl;
}

// handler, which is called when the connection is closed by the server
void AsyncReader::onCloseClient(tntpub::Client&)
{
    exit();
}

// handler, which is called when a message is received
void AsyncReader::onMessageReceived(const tntpub::DataMessage& message)
{
    std::cout << message.data() << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        // initialize logging (read `log.properties` or `log.xml`)
        log_init(argc, argv);

        // accept arguments -i for liste ip and -p for port
        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        if (argc != 2)
        {
            std::cerr << "usage: " << argv[0] << " {options} topic\n"
                         " -i <ip>      IP of server (default localhost)\n"
                         " -p <port>    tcp port (default: 9001)\n";
            return -1;
        }

        std::string topic = argv[1];

        // we need a event loop
        cxxtools::EventLoop eventLoop;

        // create reader object
        AsyncReader reader(eventLoop, ip, port, topic);

        cxxtools::connect(reader.exit, eventLoop, &cxxtools::EventLoop::exit);

        // run the event loop
        eventLoop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
