#include <tntpub/client.h>
#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/log.h>
#include <iostream>

static unsigned count = 0;
static unsigned long size = 0;

static void onMessageReceived(const tntpub::DataMessage& dm)
{
    ++count;
    size += dm.data().size();
}

static void onTimer()
{
    if (count > 0)
    {
        unsigned long averageSize = size / count;
        std::cout << count << " msg/s - average size " << averageSize << std::endl;
        count = 0;
        size = 0l;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        if (argc < 2)
        {
            std::cerr << "Usage: " << argv[0] << " {options} [topic]\n"
                         "Options:\n"
                         " -i <ip>      IP of server\n"
                         " -p <port>    tcp port (default: 9001)\n";
            return -1;
        }

        cxxtools::EventLoop loop;

        tntpub::Client client(&loop, ip, port);
        cxxtools::connect(client.messageReceived, onMessageReceived);

        cxxtools::Timer timer(&loop);
        cxxtools::connect(timer.timeout, onTimer);
        timer.start(cxxtools::Seconds(1));

        for (int a = 1; a < argc; ++a)
            client.subscribe(argv[a]);

        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

