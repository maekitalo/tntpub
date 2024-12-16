/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/responder.h>
#include <tntpub/server.h>
#include <tntpub/datamessage.h>

#include <cxxtools/destructionsentry.h>
#include <cxxtools/bin/bin.h>
#include <cxxtools/json.h>

#include <cxxtools/log.h>

log_define("tntpub.responder")

namespace tntpub
{
////////////////////////////////////////////////////////////////////////
// Responder
//
Responder::Responder(Server& pubSubServer)
    : _pubSubServer(pubSubServer),
      _socket(*pubSubServer.selector(), pubSubServer._server)
{
    log_info("new client " << static_cast<void*>(this) << " connected");

    cxxtools::connect(_socket.inputReady, *this, &Responder::onInput);
    cxxtools::connect(_socket.outputBufferEmpty, *this, &Responder::onOutputBufferEmpty);
    cxxtools::connect(_socket.outputFailed, *this, &Responder::onOutputError);
    cxxtools::connect(_pubSubServer.messageReceived, *this, &Responder::onDataMessageReceived);
    _socket.beginRead();
}

Responder::~Responder()
{
    if (_sentry)
        _sentry->detach();
}

void Responder::subscribeMessageReceived(const DataMessage& subscribeMessage)
{
    log_info("subscribe message received");
    subscribe(subscribeMessage);
}

void Responder::unsubscribeMessageReceived(const DataMessage& unsubscribeMessage)
{
    log_info("unsubscribe topic \"" << unsubscribeMessage.topic() << '"');

    for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it)
    {
        if (it->equals(unsubscribeMessage.topic()))
        {
            _subscriptions.erase(it);
            break;
        }
    }
}

void Responder::systemMessageReceived(const DataMessage& systemMessage)
{
    return;
}

void Responder::subscribe(const DataMessage& subscribeMessage)
{
    log_info("subscribe topic \"" << subscribeMessage.topic() << '"');

    _subscriptions.emplace_back(subscribeMessage.topic(), static_cast<Subscription::Type>(subscribeMessage.type()));
    _pubSubServer.clientSubscribed(*this, subscribeMessage);
}

void Responder::subscribe(const std::string& topic, Subscription::Type type)
{
    subscribe(DataMessage::subscribe(topic, type));
}

void Responder::onInput(cxxtools::net::BufferedSocket&)
{
    log_debug("input detected " << static_cast<void*>(this));

    cxxtools::DestructionSentry sentry(_sentry);

    try
    {
        _socket.endRead();

        auto& input = _socket.inputBuffer();
        log_debug(input.size() << " bytes available");

        _deserializer.advance(input.data(), input.size(), [this, &sentry] (DataMessage& dataMessage) {
            log_debug("process data message " << cxxtools::Json(dataMessage));
            if (dataMessage.isDataMessage())
            {
                log_debug("data message to topic <" << dataMessage.topic() << "> received");
                dataMessage.setNextSerial();
                _pubSubServer.processMessage(*this, dataMessage);
            }
            else if (dataMessage.isSubscribeMessage())
            {
                subscribeMessageReceived(dataMessage);
            }
            else if (dataMessage.isUnsubscribeMessage())
            {
                unsubscribeMessageReceived(dataMessage);
            }
            else if (dataMessage.isSystemMessage())
            {
                systemMessageReceived(dataMessage);
            }
            else
            {
                log_warn("unknown message type \"" << static_cast<unsigned>(dataMessage.type()) << '"');
            }

            if (sentry.deleted())
                return;
        });

        input.clear();

        if (_socket.eof())
            closeClient();
        else
            _socket.beginRead();
    }
    catch (const std::exception& e)
    {
        log_warn("failed to read message: " << e.what());
        if (!sentry.deleted())
            closeClient();
    }
}

bool Responder::isSubscribed(const std::string& topic)
{
    log_debug(static_cast<const void*>(this) << " check topic \"" << topic << '"');

    for (const auto& subscription: _subscriptions)
        if (subscription.match(topic))
            return true;

    log_debug("topic not subscribed");
    return false;
}

void Responder::onDataMessageReceived(const DataMessage& dataMessage)
{
    if (isSubscribed(dataMessage.topic()))
        sendMessage(dataMessage);
}

void Responder::sendMessage(const DataMessage& dataMessage)
{
    log_debug("send message to client");
    dataMessage.appendTo(_socket.outputBuffer());
    _socket.beginWrite();
}

void Responder::onOutputBufferEmpty(cxxtools::net::BufferedSocket&)
{
    log_debug("output buffer empty");
    outputBufferEmpty(*this);
}

void Responder::onOutputError(cxxtools::net::BufferedSocket&, const std::exception& e)
{
    log_debug("exception while writing: " << e.what());
    closeClient();
}

void Responder::closeClient()
{
    log_info("client " << static_cast<void*>(this) << " disconnected");
    _pubSubServer.clientDisconnected(*this);
    delete this;
}

}
