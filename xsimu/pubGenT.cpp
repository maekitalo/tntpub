#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <iostream>

log_define("tntpub.pubGenT")

class PubGen : public cxxtools::Connectable
{
    cxxtools::EventLoop _eventLoop;
    tntpub::Client _client;
    cxxtools::Timer _throttleTimer;
    cxxtools::Timer _timer;
    std::string _topic;
    std::string _data;

    unsigned _msgPerSecond;
    unsigned _num;          // number of records to send

    unsigned _count;        // number of entries sent in the current second

    void send();
    void onConnected(tntpub::Client&);
    void onClosed(tntpub::Client&);
    void onThrottleTimeout();
    void onTimeout();

public:
    PubGen(const std::string& ip, unsigned short port, const std::string& topic, unsigned msgPerSecond, unsigned num, unsigned size)
        : _client(&_eventLoop),
          _throttleTimer(&_eventLoop),
          _timer(&_eventLoop),
          _topic(topic),
          _msgPerSecond(msgPerSecond),
          _num(num),
          _count(0)
    {
        _client.beginConnect(ip, port);

        cxxtools::connect(_client.connected, *this, &PubGen::onConnected);
        cxxtools::connect(_client.closed, *this, &PubGen::onClosed);
        cxxtools::connect(_throttleTimer.timeout, *this, &PubGen::onThrottleTimeout);
        cxxtools::connect(_timer.timeout, *this, &PubGen::onTimeout);

        std::string randomData("randdom data ");
        while (_data.size() < size)
            _data.push_back(randomData[_data.size() % randomData.size()]);
    }

    void run()
    {
        _eventLoop.run();
    }
};

void PubGen::onConnected(tntpub::Client&)
{
    _client.endConnect();
    _timer.start(cxxtools::Seconds(1));
    _throttleTimer.start(cxxtools::Seconds(1.0 / _msgPerSecond));
}

void PubGen::onClosed(tntpub::Client&)
{
    onTimeout();
    _eventLoop.exit();
}

void PubGen::onThrottleTimeout()
{
    _client.sendPlainMessage(_topic, _data);
    if (_num == 0)
        _eventLoop.exit();
    --_num;
    ++_count;
}

void PubGen::onTimeout()
{
    log_debug("timeout - count " << _count);

    if (_count > 0)
    {
        std::cout << _count << " msg/s " << std::endl;
        _count = 0;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<unsigned> num(argc, argv, 'n', 1000);
        cxxtools::Arg<unsigned> rsize(argc, argv, 's', 100);
        cxxtools::Arg<unsigned> msgPerSecond(argc, argv, 'T', 100);

        if (argc <= 1)
        {
            std::cerr << "usage: " << argv[0] << " {options} topic\n"
                         " -i       ip address of tntpub server (default: localhost)\n"
                         " -p       port of tntpub server (default: 9001)\n"
                         " -n <num> number of records to write (default: 1000)\n"
                         " -T <num> msg/s to send (default: 100)\n";
            return -1;
        }

        PubGen pubGen(ip, port, argv[1], msgPerSecond, num, rsize);
        pubGen.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
