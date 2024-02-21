#include <tntpub/client.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init();
        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<std::string> topic(argc, argv, 't', "plainText");

        tntpub::Client client(ip, port);

        for (int a = 1; a < argc; ++a)
            client.sendPlainMessage(topic, argv[a]);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
