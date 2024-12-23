#include <tntpub/client.h>
#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/clock.h>

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<unsigned> msgsize(argc, argv, 's', 100);
        cxxtools::Arg<bool> sync(argc, argv, 'S');
        cxxtools::Arg<unsigned> obufsize(argc, argv, 'O', 8192);

        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " {options} topic\n"
                         "Options:\n"
                         " -i <ip>      IP of server\n"
                         " -p <port>    tcp port (default: 9001)\n"
                         " -s <number>  size of message (default: 100)\n"
                         " -S           sync (disable auto sync)\n"
                         " -O <number>  sync after <number> of bytes\n";
            return -1;
        }

        std::string topic = argv[1];

        tntpub::Client client(ip, port);
        client.autoSync(sync);

        auto msg = tntpub::DataMessage::createPlain(topic, std::string(msgsize, 'X'));

        cxxtools::Clock cl;
        auto t0 = cl.start();
        unsigned count = 0;
        unsigned seconds = 0;

        while (true)
        {
            client.sendMessage(msg);
            if (!sync && client.wavail() >= obufsize)
                client.flush();
            ++count;

            auto current = cl.stop();
            auto elapsed = current - t0;
            auto elapsedSeconds = static_cast<unsigned>(cxxtools::Seconds(elapsed));
            if (elapsedSeconds > seconds)
            {
                seconds = elapsedSeconds;
                std::cout << count << " msg/s" << std::endl;
                count = 0;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
