#include "mymessage.h"
#include <tntpub/client.h>
#include <tntpub/datamessage.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

/**
 * Sends a serializiable object
 */
int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<std::string> text(argc, argv, 'T', "Hello World!");
        cxxtools::Arg<unsigned> number(argc, argv, 'N', 42);

        if (argc != 2)
        {
            std::cerr << "usage: " << argv[0] << " {options} topic\n"
                         " -i <ip>      IP of server (default localhost)\n"
                         " -p <port>    tcp port (default: 9001)\n"
                         " -T <text>    text to send (default: \"Hello World!\")\n"
                         " -N <number>  number to send (default: 42)\n";
            return -1;
        }

        std::string topic = argv[1];
        tntpub::Client client(ip, port);

        MyMessage msg;
        msg.text = text;
        msg.number = number;

        client.sendMessage(topic, msg);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

