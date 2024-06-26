#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/clock.h>

#include <iostream>

log_define("tntpub.pubDelay")

class PubDelay : public cxxtools::Connectable
{
    cxxtools::EventLoop _eventLoop;
    tntpub::Client _client;
    cxxtools::Timer _timer;
    std::string _topic;

    cxxtools::Timespan _sumDelay;

    unsigned _count;        // number of messages received the current second

    void onConnected(tntpub::Client&);
    void onClosed(tntpub::Client&);
    void onMessageReceived(const tntpub::DataMessage&);
    void onTimeout();

public:
    PubDelay(const std::string& ip, unsigned short port, const std::string& topic)
        : _client(&_eventLoop),
          _timer(&_eventLoop),
          _topic(topic),
          _count(0)
    {
        _client.beginConnect(ip, port);

        cxxtools::connect(_client.connected, *this, &PubDelay::onConnected);
        cxxtools::connect(_client.closed, *this, &PubDelay::onClosed);
        cxxtools::connect(_client.messageReceived, *this, &PubDelay::onMessageReceived);
        cxxtools::connect(_timer.timeout, *this, &PubDelay::onTimeout);
    }

    void run()
    {
        _timer.start(cxxtools::Seconds(1));
        _eventLoop.run();
    }
};

void PubDelay::onConnected(tntpub::Client&)
{
    _client.endConnect();
    _client.subscribe(_topic);
}

void PubDelay::onClosed(tntpub::Client&)
{
    onTimeout();
    _eventLoop.exit();
}

void PubDelay::onMessageReceived(const tntpub::DataMessage& dm)
{
    _sumDelay += cxxtools::Clock::getSystemTime() - dm.createDateTime();
    ++_count;
}

void PubDelay::onTimeout()
{
    log_debug("timeout - count " << _count);

    if (_count > 0)
    {
        int64_t delay = _sumDelay.totalUSecs() / _count;
        std::cout << _count << " msg/s " << delay/1000.0 << " msecs delay" << std::endl;
        _count = 0;
        _sumDelay = cxxtools::Timespan();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        if (argc <= 1)
        {
            std::cerr << "usage: " << argv[0] << " {options} topic\n"
                         " -i       ip address of tntpub server (default: localhost)\n"
                         " -p       port of tntpub server (default: 9001)\n";
            return -1;
        }

        PubDelay pubDelay(ip, port, argv[1]);
        pubDelay.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
