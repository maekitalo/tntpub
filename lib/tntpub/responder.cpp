/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/responder.h>
#include <tntpub/server.h>
#include <tntpub/datamessage.h>

#include <cxxtools/ioerror.h>
#include <cxxtools/bin/bin.h>
#include <cxxtools/json.h>

#include <cxxtools/log.h>

log_define("tntpub.responder")

static const unsigned inputBufferSize = 8192;

namespace tntpub
{
////////////////////////////////////////////////////////////////////////
// Responder
//
Responder::Responder(Server& pubSubServer)
    : _inputBuffer(inputBufferSize),
      _pubSubServer(pubSubServer),
      _socket(pubSubServer._server),
      _sentry(0)
{
    log_info("new client " << static_cast<void*>(this) << " connected");

    _socket.setSelector(_pubSubServer.selector());
    cxxtools::connect(_socket.inputReady, *this, &Responder::onInput);
    cxxtools::connect(_socket.outputReady, *this, &Responder::onOutput);
    cxxtools::connect(_pubSubServer.messageReceived, *this, &Responder::onDataMessageReceived);
    _socket.beginRead(_inputBuffer.data(), _inputBuffer.size());
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

void Responder::onInput(cxxtools::IODevice&)
{
    log_debug("input detected " << static_cast<void*>(this));

    DestructionSentry sentry(this);

    try
    {
        auto count = _socket.endRead();
        _deserializer.advance(_inputBuffer.data(), count, [this, &sentry] (DataMessage& dataMessage) {
            log_debug("process data message " << cxxtools::Json(dataMessage));
            if (dataMessage.isDataMessage())
            {
                log_debug("data message to topic <" << dataMessage.topic() << "> received");
                _pubSubServer.processMessage(*this, dataMessage);
            }
            else if (dataMessage.isSubscribeMessage())
            {
                subscribeMessageReceived(dataMessage);
            }
            else if (dataMessage.isUnsubscribeMessage())
            {
                log_info("unsubscribe topic \"" << dataMessage.topic() << '"');

                for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it)
                {
                    if (it->equals(dataMessage.topic()))
                    {
                        _subscriptions.erase(it);
                        break;
                    }
                }
            }
            else
            {
                log_warn("unknown message type \"" << static_cast<unsigned>(dataMessage.type()) << '"');
            }

            if (sentry.deleted())
                return;
        });

        if (_socket.eof())
            closeClient();
        else
            _socket.beginRead(_inputBuffer.data(), _inputBuffer.size());
    }
    catch (const std::exception& e)
    {
        log_warn("failed to read message: " << e.what());
        if (!sentry.deleted())
            closeClient();
    }
}

void Responder::onOutput(cxxtools::IODevice&)
{
    log_debug("onOutput");

    try
    {
        auto count = _socket.endWrite();
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
        if (_outputBuffer.empty())
        {
            if (_outputBufferNext.empty())
            {
                log_finer("output buffer empty");
                outputBufferEmpty(*this);
            }
            else
            {
                log_finer("fetch next output buffer " << _outputBufferNext.size());
                _outputBuffer.swap(_outputBufferNext);
                log_finer("begin write " << _outputBuffer.size());
                _socket.beginWrite(_outputBuffer.data(), _outputBuffer.size());
            }
        }
        else
        {
            log_finer("partial write " << _outputBuffer.size() << " left");
            if (!_outputBufferNext.empty())
            {
                log_finer("append next " << _outputBufferNext.size());
                _outputBuffer.insert(_outputBuffer.end(), _outputBufferNext.begin(), _outputBufferNext.end());
                _outputBufferNext.clear();
            }
            log_finer("begin write " << _outputBuffer.size());
            _socket.beginWrite(_outputBuffer.data(), _outputBuffer.size());
        }
    }
    catch (const std::exception& e)
    {
        log_debug("exception while writing: " << e.what());
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
    try
    {
        if (_socket.writing())
        {
            dataMessage.appendTo(_outputBufferNext);
            log_finer("next buffer " << _outputBufferNext.size());
        }
        else
        {
            dataMessage.appendTo(_outputBuffer);
            log_finer("begin write " << _outputBuffer.size());
            _socket.beginWrite(_outputBuffer.data(), _outputBuffer.size());
        }
    }
    catch (const std::exception& e)
    {
        log_debug("exception while writing: " << e.what());
        closeClient();
    }
}

void Responder::closeClient()
{
    log_info("client " << static_cast<void*>(this) << " disconnected");
    _pubSubServer.clientDisconnected(*this);
    delete this;
}

}
