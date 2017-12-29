/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/responder.h>
#include <tntpub/server.h>
#include <tntpub/datamessage.h>
#include <tntpub/subscribemessage.h>
#include <tntpub/unsubscribemessage.h>

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
    : _stream(0),
      _pubSubServer(pubSubServer),
      _sentry(0)
{
}

Responder::~Responder()
{
    if (_sentry)
        _sentry->detach();
}

void Responder::init(cxxtools::IOStream& stream)
{
    _stream = &stream;
    _stream->buffer().device()->setSelector(_pubSubServer.selector());
    cxxtools::connect(_stream->buffer().inputReady, *this, &Responder::onInput);
    cxxtools::connect(_stream->buffer().outputReady, *this, &Responder::onOutput);
    cxxtools::connect(_pubSubServer.messageReceived, *this, &Responder::onDataMessageReceived);

    _stream->buffer().beginRead();
    _deserializer.begin();
}

void Responder::onInput(cxxtools::StreamBuffer& sb)
{
    log_debug("input detected " << static_cast<void*>(this));

    DestructionSentry sentry(this);

    try
    {
        sb.endRead();

        while (_deserializer.advance(sb))
        {
            if (_deserializer.si().typeName() == "SubscribeMessage")
            {
                SubscribeMessage subscribeMessage;
                _deserializer.deserialize(subscribeMessage);

                log_info("subscribe topic \"" << subscribeMessage.topic() << '"');

                _topics.push_back(subscribeMessage.topic());
                _pubSubServer.clientSubscribed(*this, subscribeMessage);
            }
            else if (_deserializer.si().typeName() == "UnsubscribeMessage")
            {
                UnsubscribeMessage unsubscribeMessage;
                _deserializer.deserialize(unsubscribeMessage);

                log_info("unsubscribe topic \"" << unsubscribeMessage.topic() << '"');

                for (auto it = _topics.begin(); it != _topics.end(); ++it)
                {
                    if (*it == unsubscribeMessage.topic())
                    {
                        _topics.erase(it);
                        break;
                    }
                }
            }
            else if (_deserializer.si().typeName() == "DataMessage")
            {
                DataMessage dataMessage;
                _deserializer.deserialize(dataMessage);

                log_debug("data message of type <" << dataMessage.typeName() << "> received:\n"
                    << cxxtools::Json(_deserializer.si()).beautify(true));

                _pubSubServer.processMessage(*this, dataMessage);
            }
            else
            {
                log_warn("unknown message type \"" << _deserializer.si().typeName() << '"');
            }

            if (sentry.deleted())
                return;

            _deserializer.begin();
        }

        if (_stream->attachedDevice()->eof())
            closeClient();
        else
            sb.beginRead();
    }
    catch (const std::exception& e)
    {
        log_debug("exception while reading: " << e.what());
        if (!sentry.deleted())
            closeClient();
    }
}

void Responder::onOutput(cxxtools::StreamBuffer& sb)
{
    log_debug("output detected");

    try
    {
        sb.endWrite();
        if (sb.out_avail())
            sb.beginWrite();
        else
            outputBufferEmpty(*this);
    }
    catch (const std::exception& e)
    {
        log_debug("exception while writing: " << e.what());
        closeClient();
    }
}

void Responder::onDataMessageReceived(const DataMessage& dataMessage)
{
    log_debug(static_cast<const void*>(this) << " check topic \"" << dataMessage.topic() << '"');

    for (const auto& topic: _topics)
    {
        if (dataMessage.topic() == topic)
        {
            log_debug("send message to client");
            try
            {
                *_stream << cxxtools::bin::Bin(dataMessage);
                _stream->buffer().beginWrite();
            }
            catch (const std::exception& e)
            {
                log_debug("exception while writing: " << e.what());
                closeClient();
            }
            return;
        }
    }

    log_debug("topic not subscribed");
}

void Responder::closeClient()
{
    log_info("client " << static_cast<void*>(this) << " disconnected");
    _pubSubServer.clientDisconnected(*this);
    delete this;
}

////////////////////////////////////////////////////////////////////////
// TcpResponder
//
TcpResponder::TcpResponder(Server& pubSubServer)
    : Responder(pubSubServer),
      _netstream(pubSubServer._server)
{
    init(_netstream);
    log_info("new client " << static_cast<void*>(this) << " connected");
}

}
