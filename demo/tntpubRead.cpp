#include <tntpub/client.h>
#include <tntpub/datamessage.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

/**
 * subscribe to topic and write received messages to stdout
 */

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        if (argc <= 1)
        {
            std::cerr << "usage: " << argv[0] << " {options} {topics}\n"
                         " -i <ip>      IP of server (default localhost)\n"
                         " -p <port>    tcp port (default: 9001)\n";
            return -1;
        }

        tntpub::Client client(ip, port);

        for (int a = 1; a < argc; ++a)
            client.subscribe(tntpub::Topic(argv[a]));

        while (true)
        {
            auto dm = client.readMessage();
            std::cout << dm.data() << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

