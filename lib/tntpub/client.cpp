/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>
#include <tntpub/subscribemessage.h>
#include <tntpub/unsubscribemessage.h>

#include <cxxtools/bin/bin.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>

log_define("tntpub.client")

namespace tntpub
{

Client::Client(cxxtools::IOStream& peer)
    : _peer(peer)
{
    cxxtools::connect(_peer.buffer().outputReady, *this, &Client::onOutput);
    cxxtools::connect(_peer.buffer().inputReady, *this, &Client::onInput);
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

Client& Client::unsubscribe(const std::string& topic)
{
    UnsubscribeMessage unsubscribeMessage;
    unsubscribeMessage.topic = topic;
    _peer << cxxtools::bin::Bin(unsubscribeMessage);
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
            messageReceived(*this);
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
            log_debug("got message");
            _deserializer.deserialize(_dataMessage);
            messageReceived(*this);
            _deserializer.begin();
            return true;
        }
    }

    return false;
}

void Client::onOutput(cxxtools::StreamBuffer& sb)
{
    log_debug("onOutput");
    sb.endWrite();
    if (sb.out_avail())
        sb.beginWrite();
}

void Client::onInput(cxxtools::StreamBuffer& sb)
{
    log_debug("onInput");
    sb.endRead();
    while (sb.in_avail())
        advance();

    if (sb.device()->eof())
        closed(*this);
    else
        sb.beginRead();
}

void Client::beginRead()
{
    log_debug("beginRead");
    _peer.buffer().beginRead();
}

const DataMessage& Client::endRead()
{
    log_debug("endRead");
    return _dataMessage;
}

}
