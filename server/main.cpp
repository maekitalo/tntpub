/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/server.h>
#include <cxxtools/log.h>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init();
        tntpub::Server pubSubServer(argc, argv);
        pubSubServer.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
