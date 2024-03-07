#include "mymessage.h"

#include <tntpub/client.h>

#include <cxxtools/json.h>
#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>

log_define("tntpub.exampleAsyncSender")

class App : public cxxtools::Connectable
{
    cxxtools::EventLoop _loop;
    tntpub::Client _client;

    tntpub::DataMessage _dataMessage;
    unsigned _count;
    unsigned _flush;

    void onMessagesSent(tntpub::Client& c)
    {
        log_debug("onMessagesSent");
        if (_count == 0)
        {
            _loop.exit();
        }
        else
        {
            for (unsigned n = 0; _count > 0 && n < _flush; ++n, --_count)
            {
                log_debug("send message " << n << '/' << _count << '/' << _flush);
                _dataMessage.touch();
                c.sendMessage(_dataMessage);
            }
        }
    }

    void onClosed(tntpub::Client& c)
    {
        std::cout << "closed" << std::endl;
        _loop.exit();
    }

public:
    App(const std::string& ip, unsigned short port, unsigned flush)
        : _client(&_loop, ip, port),
          _flush(flush)
    {
        cxxtools::connect(_client.messagesSent, *this, &App::onMessagesSent);
        cxxtools::connect(_client.closed, *this, &App::onClosed);
    }

    void sendMessage(const tntpub::DataMessage& msg, unsigned count)
    {
        _dataMessage = msg;
        _count = count;
        onMessagesSent(_client);
        log_debug("run loop");
        _loop.run();
    }
};

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<unsigned> count(argc, argv, 'n', 1);
        cxxtools::Arg<unsigned> flush(argc, argv, 'f', 100); // flush after n messages

        if (argc < 2)
        {
            std::cerr << "usage: " << argv[0] << " [-i ip] [-p port] [-n count] topic\n";
            return 1;
        }

        cxxtools::Arg<std::string> topic(argc, argv);

        App app(ip, port, flush);

        MyMessage msg;

        try
        {
            if (argc > 1)
            {
                for (int a = 1; a < argc; ++a)
                {
                    std::istringstream ss(argv[a]);
                    ss >> cxxtools::Json(msg);
                    auto dm = tntpub::DataMessage::create(topic, msg);
                    app.sendMessage(dm, count);
                }
            }
            else
            {
                while (std::cin >> cxxtools::Json(msg))
                {
                    auto dm = tntpub::DataMessage::create(topic, msg);
                    app.sendMessage(dm, count);
                }
            }
        }
        catch (const cxxtools::SerializationError&)
        {
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
