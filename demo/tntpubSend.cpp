#include <tntpub/client.h>
#include <tntpub/datamessage.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

/**
 * Read messages line by line from stdin and writes to a topic
 */
int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<unsigned> count(argc, argv, 'n', 1);

        if (argc != 2)
        {
            std::cerr << "usage: " << argv[0] << " {options} topic\n"
                         " -i <ip>      IP of server (default localhost)\n"
                         " -p <port>    tcp port (default: 9001)\n";
            return -1;
        }

        std::string topic = argv[1];
        tntpub::Client client(ip, port);

        std::string msg;
        while (std::getline(std::cin, msg))
            client.sendPlainMessage(topic, msg);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

