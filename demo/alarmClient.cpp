/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "alarmMessage.h"

#include <tntpub/client.h>

#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/join.h>

#include <exception>
#include <iostream>

const std::string alarmTopic = "alarm";

struct Usage { };

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<bool> help(argc, argv, '?');

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);
        cxxtools::Arg<std::string> source(argc, argv, 's');
        cxxtools::Arg<bool> infoMessage(argc, argv, 'I');
        cxxtools::Arg<bool> warningMessage(argc, argv, 'W');
        cxxtools::Arg<bool> errorMessage(argc, argv, 'E');
        cxxtools::Arg<bool> fatalMessage(argc, argv, 'F');
        cxxtools::Arg<bool> commitMessage(argc, argv, 'C');

        if (help)
            throw Usage();

        tntpub::Client client(ip, port);

        if (commitMessage)
        {
            if (argc != 2)
                throw Usage();

            cxxtools::DateTime time(argv[1]);

            tntpub::AlarmMessage::Severity severity = infoMessage    ? tntpub::AlarmMessage::INFO
                                                    : warningMessage ? tntpub::AlarmMessage::WARNING
                                                    : errorMessage   ? tntpub::AlarmMessage::ERROR
                                                    :                  tntpub::AlarmMessage::FATAL;

            client.sendMessage(tntpub::alarmTopic, tntpub::AlarmCommitMessage(source, severity, time));
        }
        else if (infoMessage)
        {
            if (argc <= 1)
                throw Usage();

            std::string msg = cxxtools::join(argv + 1, argv + argc, ' ');
            client.sendMessage(tntpub::alarmTopic, tntpub::AlarmMessage(source, msg, tntpub::AlarmMessage::INFO));
        }
        else if (warningMessage)
        {
            if (argc <= 1)
                throw Usage();

            std::string msg = cxxtools::join(argv + 1, argv + argc, ' ');
            client.sendMessage(tntpub::alarmTopic, tntpub::AlarmMessage(source, msg, tntpub::AlarmMessage::WARNING));
        }
        else if (errorMessage)
        {
            if (argc <= 1)
                throw Usage();

            std::string msg = cxxtools::join(argv + 1, argv + argc, ' ');
            client.sendMessage(tntpub::alarmTopic, tntpub::AlarmMessage(source, msg, tntpub::AlarmMessage::ERROR));
        }
        else if (fatalMessage)
        {
            if (argc <= 1)
                throw Usage();

            std::string msg = cxxtools::join(argv + 1, argv + argc, ' ');
            client.sendMessage(tntpub::alarmTopic, tntpub::AlarmMessage(source, msg, tntpub::AlarmMessage::FATAL));
        }
        else
        {
            client.subscribe(tntpub::alarmTopic)
                  .flush();
            while (true)
            {
                const tntpub::DataMessage& dataMessage = client.readMessage();
                if (dataMessage.typeName() == tntpub::AlarmMessage::typeName)
                {
                    tntpub::AlarmMessage msg;
                    dataMessage.get(msg);
                    std::cout << msg.severity() << " time " << msg.time().toString() << ": " << msg.message() << std::endl;
                }
                else if (dataMessage.typeName() == tntpub::AlarmCommitMessage::typeName)
                {
                    tntpub::AlarmCommitMessage msg;
                    dataMessage.get(msg);
                    std::cout << "commit message " << msg.severity() << " time " << msg.time().toString() << std::endl;
                }
            }

        }
    }
    catch (const Usage&)
    {
        std::cerr << "usage: " << argv[0] << " [-i ip] [-p port]\n"
                     "       " << argv[0] << " [-i ip] [-p port] [-s source] [-IWEF] -C datetime\n"
                     "       " << argv[0] << " [-i ip] [-p port] [-s source] -I infomessage\n"
                     "       " << argv[0] << " [-i ip] [-p port] [-s source] -W warningmessage\n"
                     "       " << argv[0] << " [-i ip] [-p port] [-s source] -E errormessage\n"
                     "       " << argv[0] << " [-i ip] [-p port] [-s source] -F fatalmessage\n";
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

