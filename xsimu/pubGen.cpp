#include <tntpub/client.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <iostream>

log_define("tntpub.pubGen")

class PubGen : public cxxtools::Connectable
{
    cxxtools::EventLoop _eventLoop;
    tntpub::Client _client;
    cxxtools::Timer _timer;
    std::string _topic;
    std::string _data;

    unsigned _num;          // number of records to send
    unsigned _bulk;         // number of entries to send at once

    cxxtools::Timespan _startClock;

    unsigned _totalCount;   // number of entries sent
    unsigned _count;        // number of entries sent in the current second
    unsigned _countBulk;    // number of sends
    unsigned _countPending; // number of sent messages

    void send();
    void onConnected(tntpub::Client&);
    void onClosed(tntpub::Client&);
    void onMessagesSent(tntpub::Client&);
    void onTimeout();

public:
    PubGen(const std::string& ip, unsigned short port, const std::string& topic, unsigned num, unsigned bulk, unsigned size)
        : _client(&_eventLoop),
          _timer(&_eventLoop),
          _topic(topic),
          _num(num),
          _bulk(bulk),
          _totalCount(0),
          _count(0),
          _countBulk(0),
          _countPending(0)
    {
        _client.beginConnect(ip, port);

        cxxtools::connect(_client.connected, *this, &PubGen::onConnected);
        cxxtools::connect(_client.closed, *this, &PubGen::onClosed);
        cxxtools::connect(_client.messagesSent, *this, &PubGen::onMessagesSent);
        cxxtools::connect(_timer.timeout, *this, &PubGen::onTimeout);

        std::string randomData("randdom data ");
        while (_data.size() < size)
            _data.push_back(randomData[_data.size() % randomData.size()]);
    }

    void run()
    {
        _timer.start(cxxtools::Seconds(1));
        _eventLoop.run();
    }
};

void PubGen::send()
{
    unsigned count = std::min(_num, _bulk);

    if (count == 0)
    {
        _eventLoop.exit();

        auto usecs = cxxtools::Clock::getSystemTicks().totalUSecs() - _startClock.totalUSecs();
        auto secs = usecs / 1e6;

        std::cout << _totalCount << " msg " << secs << " secs " << static_cast<unsigned>(_totalCount / secs) << " msg/s " << _countBulk << " writes/s" << std::endl;
        return;
    }

    if (_num > 0)
        _num -= count;

    for (unsigned n = 0; n < count; ++n)
        _client.sendPlainMessage(_topic, _data);

    ++_countBulk;
    _countPending = count;
}

void PubGen::onConnected(tntpub::Client&)
{
    _client.endConnect();
    _startClock = cxxtools::Clock::getSystemTicks();
    send();
}

void PubGen::onClosed(tntpub::Client&)
{
    onTimeout();
    _eventLoop.exit();
}

void PubGen::onMessagesSent(tntpub::Client&)
{
    log_debug(_countPending << " messages sent");
    _count += _countPending;
    _totalCount += _countPending;
    _countPending = 0;
    send();
}

void PubGen::onTimeout()
{
    log_debug("timeout - count " << _count << " bulks " << _countBulk);

    if (_count > 0)
    {
        std::cout << _totalCount << " msg " << _count << " msg/s " << _countBulk << " writes/s" << std::endl;
        _countBulk = 0;
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
        cxxtools::Arg<unsigned> bulk(argc, argv, 'b', 100);
        cxxtools::Arg<unsigned> rsize(argc, argv, 's', 100);

        if (argc <= 1)
        {
            std::cerr << "usage: " << argv[0] << " {options} topic\n"
                         " -i       ip address of tntpub server (default: localhost)\n"
                         " -p       port of tntpub server (default: 9001)\n"
                         " -n <num> number of records to write (default: 1000)\n"
                         " -b <num> number of records to write at once (default: 100)\n"
                         " -s <num> record size of generated data (default: 100)\n";
            return -1;
        }

        PubGen pubGen(ip, port, argv[1], num, bulk, rsize);
        pubGen.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
