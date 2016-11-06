/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/responder.h>
#include <tntpub/server.h>
#include <tntpub/subscribemessage.h>
#include <tntpub/datamessage.h>

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
    : _stream(pubSubServer.server()),
      _pubSubServer(pubSubServer)
{
    log_info("new client connected");
    _stream.buffer().device()->setSelector(pubSubServer.server().selector());

    cxxtools::connect(_stream.buffer().inputReady, *this, &Responder::onInput);
    cxxtools::connect(_stream.buffer().outputReady, *this, &Responder::onOutput);
    cxxtools::connect(pubSubServer.messageReceived, *this, &Responder::onDataMessageReceived);

    _stream.buffer().beginRead();
    _deserializer.begin();
}

void Responder::onInput(cxxtools::StreamBuffer& sb)
{
    log_debug("input detected");

    try
    {
        sb.endRead();

        while (sb.in_avail() > 0)
        {
            if (_deserializer.advance(sb.sbumpc()))
            {
                log_debug("message received: " << cxxtools::Json(_deserializer.si()).beautify(true));
                try
                {
                    DataMessage dataMessage;
                    _deserializer.deserialize(dataMessage);
                    log_debug("data message received; topic=\"" << dataMessage.topic << '"');
                    _pubSubServer.messageReceived(dataMessage);
                }
                catch (const cxxtools::SerializationError&)
                {
                    // not a data message
                }

                try
                {
                    SubscribeMessage subscribeMessage;
                    _deserializer.deserialize(subscribeMessage);
                    log_info("subscribe topic \"" << subscribeMessage.topic << '"');
                    _topics.push_back(subscribeMessage.topic);
                }
                catch (const cxxtools::SerializationError&)
                {
                    // not a subscribe message
                }

                _deserializer.begin();
            }
        }

        if (_stream.attachedDevice()->eof())
            closeClient();
        else
            sb.beginRead();
    }
    catch (const std::exception& e)
    {
        log_debug("exception while reading: " << e.what());
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
    }
    catch (const std::exception& e)
    {
        log_debug("exception while reading: " << e.what());
        closeClient();
    }
}

void Responder::onDataMessageReceived(const DataMessage& dataMessage)
{
    log_debug("check topics");

    for (const auto& topic: _topics)
    {
        if (dataMessage.topic == topic)
        {
            log_debug("send message to client");
            _stream << cxxtools::bin::Bin(dataMessage);
            _stream.buffer().beginWrite();
            return;
        }
    }

    log_debug("topic not subscribed");
}

void Responder::closeClient()
{
    log_info("client disconnected");
    delete this;
}

}
