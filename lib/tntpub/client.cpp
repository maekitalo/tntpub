/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>
#include <tntpub/subscribemessage.h>

#include <cxxtools/bin/bin.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>

log_define("tntpub.client")

namespace tntpub
{

Client::Client(cxxtools::IOStream& peer)
    : _peer(peer)
{
    _deserializer.begin();
}

Client& Client::subscribe(const std::string& topic)
{
    SubscribeMessage subscribeMessage;
    subscribeMessage.topic = topic;
    _peer << cxxtools::bin::Bin(subscribeMessage);
    _peer.buffer().beginWrite();
    return *this;
}

Client& Client::sendMessage(const DataMessage& msg)
{
    log_debug("sendMessage " << cxxtools::Json(msg).beautify(true));
    _peer << cxxtools::bin::Bin(msg);
    _peer.buffer().beginWrite();
    return *this;
}

const DataMessage& Client::readMessage()
{
    char ch;
    while (_peer.get(ch))
    {
        if (_deserializer.advance(ch))
        {
            _deserializer.deserialize(_dataMessage);
            messageReceived(_dataMessage);
            _deserializer.begin();
            return _dataMessage;
        }
    }

    throw std::runtime_error("input stream failed in pubsub client");
}

bool Client::advance()
{
    char ch;
    while (_peer.rdbuf()->in_avail())
    {
        _peer.get(ch);

        if (_deserializer.advance(ch))
        {
            _deserializer.deserialize(_dataMessage);
            messageReceived(_dataMessage);
            return true;
        }
    }

    return false;
}

}
