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

void Client::init()
{
    cxxtools::connect(_peer.buffer().outputReady, *this, &Client::onOutput);
    cxxtools::connect(_peer.buffer().inputReady, *this, &Client::onInput);
    cxxtools::connect(_peer.connected, *this, &Client::onConnected);
    cxxtools::connect(_peer.closed, *this, &Client::onClosed);
}

void Client::begin()
{
    _deserializer.begin();
    _peer.buffer().beginRead();
}

Client& Client::subscribe(const std::string& topic, Subscription::Type type)
{
    doSubscribe(SubscribeMessage(topic, type));
    return *this;
}

void Client::doSubscribe(const SubscribeMessage& subscribeMessage)
{
    _peer << cxxtools::bin::Bin(subscribeMessage);
    _peer.buffer().beginWrite();
}

Client& Client::unsubscribe(const std::string& topic)
{
    UnsubscribeMessage unsubscribeMessage(topic);
    _peer << cxxtools::bin::Bin(unsubscribeMessage);
    _peer.buffer().beginWrite();
    return *this;
}

void Client::doSendMessage(const DataMessage& msg)
{
    log_debug("sendMessage of type <" << msg.typeName() << '>');
    log_finer(cxxtools::Json(msg).beautify(true));
    _peer << cxxtools::bin::Bin(msg);
    _peer.buffer().beginWrite();
}

const DataMessage& Client::readMessage()
{
    _dataMessage = DataMessage();
    while (_peer.peek() != std::ios::traits_type::eof())
    {
        if (_deserializer.advance(_peer.buffer()))
        {
            _deserializer.deserialize(_dataMessage);
            dispatchMessage(_dataMessage);
            _deserializer.begin();
            return _dataMessage;
        }
    }

    throw std::runtime_error("input stream failed in pubsub client");
}

bool Client::advance()
{
    log_debug("advance");
    if (_deserializer.advance(_peer.buffer()))
    {
        _deserializer.deserialize(_dataMessage);

        log_debug("got message of type <" << _dataMessage.typeName() << '>');
        log_finer(cxxtools::Json(_dataMessage).beautify(true));

        dispatchMessage(_dataMessage);
        _dataMessage = DataMessage();
        _deserializer.begin();
        return true;
    }

    return false;
}

void Client::onConnected(cxxtools::net::TcpStream&)
{
    connected(*this);
}

void Client::onClosed(cxxtools::net::TcpStream&)
{
    closed(*this);
}

void Client::onOutput(cxxtools::StreamBuffer& sb)
{
    log_debug("onOutput");
    try
    {
        sb.endWrite();
    }
    catch (const std::exception& e)
    {
        log_warn("output failed: " << e.what());
        sb.device()->cancel();
        sb.discard();
        _peer.close();
        closed(*this);
        return;
    }

    if (sb.out_avail())
    {
        log_debug("continue writing");
        sb.beginWrite();
    }
    else
    {
        log_debug("all messages sent");
        messagesSent(*this);
    }
}

void Client::onInput(cxxtools::StreamBuffer& sb)
{
    log_debug("onInput");

    try
    {
        sb.endRead();
    }
    catch (const std::exception& e)
    {
        log_warn("read failed: " << e.what());
        sb.device()->cancel();
        sb.discard();
        _peer.close();
        closed(*this);
        return;
    }

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
