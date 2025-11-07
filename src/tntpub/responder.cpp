/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/responder.h>
#include <tntpub/server.h>
#include <tntpub/datamessage.h>
#include <tntpub/topic.h>

#include <cxxtools/destructionsentry.h>
#include <cxxtools/bin/bin.h>
#include <cxxtools/json.h>
#include <cxxtools/hexdump.h>

#include <cxxtools/log.h>

log_define("tntpub.responder")

namespace tntpub
{
////////////////////////////////////////////////////////////////////////
// Responder
//

unsigned Responder::_maxOBuf = 0;

Responder::Responder(Server& pubSubServer)
    : _pubSubServer(pubSubServer),
      _socket(*pubSubServer.selector(), pubSubServer._server),
      systemMessageReceived(pubSubServer.systemMessageReceived)
{
    log_info("new client " << static_cast<void*>(this) << " connected");
    log_debug("connection from " << _socket.getPeerAddr() << " fd " << _socket.getFd());

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
    log_info(static_cast<void*>(this) << " subscribe message received");
    subscribe(subscribeMessage);
}

void Responder::unsubscribeMessageReceived(const DataMessage& unsubscribeMessage)
{
    log_info(static_cast<void*>(this) << " unsubscribe topic \"" << unsubscribeMessage.topic() << '"');
    log_debug("unsubscription message " << cxxtools::Json(unsubscribeMessage));

    for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it)
    {
        if (it->equals(unsubscribeMessage.topic().topic(), DataMessage::subscriptionType(unsubscribeMessage.type())))
        {
            _subscriptions.erase(it);
            break;
        }
    }
}

void Responder::subscribe(const DataMessage& subscribeMessage)
{
    log_info(static_cast<void*>(this) << " subscribe topic \"" << subscribeMessage.topic() << '"');
    log_debug("subscription message " << cxxtools::Json(subscribeMessage));

    _subscriptions.emplace_back(subscribeMessage.topic(), DataMessage::subscriptionType(subscribeMessage.type()));
    _pubSubServer.clientSubscribed(*this, subscribeMessage);
}

void Responder::subscribe(const Topic& topic, Subscription::Type type)
{
    subscribe(DataMessage::subscribe(topic, type));
}

void Responder::onInput(cxxtools::net::BufferedSocket&)
{
    log_finest("input detected " << static_cast<void*>(this));

    cxxtools::DestructionSentry sentry(_sentry);

    try
    {
        _socket.endRead();

        auto& input = _socket.inputBuffer();
        log_finest(input.size() << " Bytes available");
        log_finest(cxxtools::hexDump(input));

        _deserializer.advance(input.data(), input.size(), [this, &sentry] (DataMessage& message) {
            log_debug(static_cast<void*>(this) << " process message " << cxxtools::Json(message));
            if (message.isDataMessage())
            {
                log_debug(static_cast<void*>(this) << " data message to topic <" << message.topic() << "> received");
                message.setNextSerial();
                _pubSubServer.processMessage(*this, message);
            }
            else if (message.isSubscribeMessage())
            {
                subscribeMessageReceived(message);
            }
            else if (message.isUnsubscribeMessage())
            {
                unsubscribeMessageReceived(message);
            }
            else if (message.isSystemMessage())
            {
                systemMessageReceived(message);
            }
            else
            {
                log_warn("unknown message type \"" << static_cast<unsigned>(message.type()) << '"');
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
        log_warn(static_cast<void*>(this) << " failed to read message: " << e.what());
        if (!sentry.deleted())
            closeClient();
    }
}

bool Responder::isSubscribed(const Topic& topic)
{
    for (const auto& subscription: _subscriptions)
    {
        if (subscription.match(topic))
        {
            log_finer(static_cast<const void*>(this) << " topic \"" << topic << "\" subscribed");
            return true;
        }
    }

    log_finer(static_cast<const void*>(this) << " topic \"" << topic << "\" not subscribed");
    return false;
}

void Responder::onDataMessageReceived(const DataMessage& dataMessage)
{
    if (isSubscribed(dataMessage.topic()))
        sendMessage(dataMessage);
}

void Responder::sendMessage(const DataMessage& dataMessage)
{
    log_debug(static_cast<void*>(this) << " send message " << cxxtools::Json(dataMessage));
    dataMessage.appendTo(_socket.outputBuffer());

    if (_maxOBuf != 0)
    {
        if (_socket.outputSize() > _maxOBuf)
        {
            log_warn("output buffer size " << _socket.outputSize() << " > max (" << _maxOBuf << ')');
            outputBufferFull(*this);
        }

        if (_socket.outputSize() > _maxOBuf)
        {
            log_warn("closing client");
            closeClient();
            return;
        }
    }

    _socket.beginWrite();
}

void Responder::onOutputBufferEmpty(cxxtools::net::BufferedSocket&)
{
    log_finer("output buffer empty");
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
