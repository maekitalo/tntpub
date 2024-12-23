#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/clock.h>

#include <iostream>

log_define("tntpub.pubDelay")

class PubCopy : public cxxtools::Connectable
{
    cxxtools::EventLoop _eventLoop;
    tntpub::Client _client;
    cxxtools::Timer _timer;
    std::string _topicIn;
    std::string _topicOut;

    cxxtools::Timespan _sumDelay;
    unsigned _count;        // number of messages received the current second

    void onConnected(tntpub::Client&);
    void onClosed(tntpub::Client&);
    void onMessageReceived(const tntpub::DataMessage&);
    void onTimeout();

public:
    PubCopy(const std::string& ip, unsigned short port, const std::string& topicIn, const std::string& topicOut)
        : _client(&_eventLoop),
          _timer(&_eventLoop),
          _topicIn(topicIn),
          _topicOut(topicOut),
          _count(0)
    {
        _client.beginConnect(ip, port);

        cxxtools::connect(_client.connected, *this, &PubCopy::onConnected);
        cxxtools::connect(_client.closed, *this, &PubCopy::onClosed);
        cxxtools::connect(_client.messageReceived, *this, &PubCopy::onMessageReceived);
        cxxtools::connect(_timer.timeout, *this, &PubCopy::onTimeout);
    }

    void run()
    {
        _timer.start(cxxtools::Seconds(1));
        _eventLoop.run();
    }
};

void PubCopy::onConnected(tntpub::Client&)
{
    _client.endConnect();
    _client.subscribe(_topicIn);
}

void PubCopy::onClosed(tntpub::Client&)
{
    onTimeout();
    _eventLoop.exit();
}

void PubCopy::onMessageReceived(const tntpub::DataMessage& dm)
{
    auto createDateTime = dm.createDateTime();
    _sumDelay += cxxtools::Clock::getSystemTime() - createDateTime;
    auto dmOut = dm;
    dmOut.topic(_topicOut);
    _client.sendMessage(dmOut);
    ++_count;
}

void PubCopy::onTimeout()
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

        if (argc != 3)
        {
            std::cerr << "usage: " << argv[0] << " {options} topicIn topicOut\n"
                         " -i       ip address of tntpub server (default: localhost)\n"
                         " -p       port of tntpub server (default: 9001)\n";
            return -1;
        }

        PubCopy pubDelay(ip, port, argv[1], argv[2]);
        pubDelay.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
