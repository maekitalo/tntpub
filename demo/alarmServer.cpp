/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/server.h>
#include <tntpub/responder.h>
#include <tntpub/datamessage.h>
#include <tntpub/subscribemessage.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/arg.h>
#include <cxxtools/connectable.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>

#include <iostream>

#include "alarmMessage.h"

log_define("tntpub.alarmServer")

class AlarmDaemon : public cxxtools::Connectable
{
    tntpub::Server _pubSubServer;
    std::vector<tntpub::AlarmMessage> _alarmMessages;

    void onClientSubscribed(tntpub::Responder& client, const tntpub::SubscribeMessage& message);
    void onDataMessageReceived(const tntpub::DataMessage& msg);

public:
    AlarmDaemon(cxxtools::EventLoop& eventLoop, const std::string& ip, unsigned short port);

};

AlarmDaemon::AlarmDaemon(cxxtools::EventLoop& eventLoop, const std::string& ip, unsigned short port)
    : _pubSubServer(eventLoop, ip, port)
{
    cxxtools::connect(_pubSubServer.clientSubscribed, *this, &AlarmDaemon::onClientSubscribed);
    cxxtools::connect(_pubSubServer.messageReceived, *this, &AlarmDaemon::onDataMessageReceived);
}

void AlarmDaemon::onClientSubscribed(tntpub::Responder& client, const tntpub::SubscribeMessage& message)
{
    if (message.topic() == tntpub::alarmTopic)
    {
        for (const auto&m: _alarmMessages)
            client.onDataMessageReceived(tntpub::DataMessage(tntpub::alarmTopic, m));
    }
}

void AlarmDaemon::onDataMessageReceived(const tntpub::DataMessage& msg)
{
    log_debug("message received: " << cxxtools::Json(msg));

    if (msg.topic() != tntpub::alarmTopic)
        return;

    if (msg.typeName() == tntpub::AlarmMessage::typeName)
    {
        log_debug("alarm message received");

        tntpub::AlarmMessage am;
        msg.get(am);
        log_info("new alarm message: " << cxxtools::Json(am).beautify(true));
        _alarmMessages.push_back(am);
    }
    else if (msg.typeName() == tntpub::AlarmCommitMessage::typeName)
    {
        log_debug("commit message received");

        tntpub::AlarmCommitMessage cm;
        msg.get(cm);

        for (auto it = _alarmMessages.begin(); it != _alarmMessages.end(); ++it)
        {
            if (cm.match(*it))
            {
                log_info("commit alarm message: " << cxxtools::Json(*it).beautify(true));
                _alarmMessages.erase(it);
                break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        cxxtools::EventLoop eventLoop;
        AlarmDaemon alarmDaemon(eventLoop, ip, port);
        eventLoop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
