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

void Client::init(cxxtools::IOStream& peer)
{
    _peer = &peer;
    cxxtools::connect(_peer->buffer().outputReady, *this, &Client::onOutput);
    cxxtools::connect(_peer->buffer().inputReady, *this, &Client::onInput);
    _deserializer.begin();
    _peer->buffer().beginRead();
}

Client& Client::subscribe(const std::string& topic)
{
    SubscribeMessage subscribeMessage(topic);
    *_peer << cxxtools::bin::Bin(subscribeMessage);
    _peer->buffer().beginWrite();
    return *this;
}

Client& Client::unsubscribe(const std::string& topic)
{
    UnsubscribeMessage unsubscribeMessage(topic);
    *_peer << cxxtools::bin::Bin(unsubscribeMessage);
    _peer->buffer().beginWrite();
    return *this;
}

void Client::doSendMessage(const DataMessage& msg)
{
    log_debug("sendMessage " << cxxtools::Json(msg).beautify(true));
    *_peer << cxxtools::bin::Bin(msg);
    _peer->buffer().beginWrite();
}

const DataMessage& Client::readMessage()
{
    char ch;
    while (_peer->get(ch))
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
    log_debug("advance");
    while (_peer->rdbuf()->in_avail())
    {
        char ch = _peer->rdbuf()->sbumpc();

        if (_deserializer.advance(ch))
        {
            log_debug("got message");
            _deserializer.deserialize(_dataMessage);
            messageReceived(_dataMessage);
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
    while (true)
    {
        auto n = sb.in_avail();
        log_debug("in_avail=" << n);
        if (n <= 0)
            break;
        advance();
    }

    if (sb.device()->eof())
        closed(*this);
    else
        sb.beginRead();
}

}
