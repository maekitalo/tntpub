/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>
#include <tntpub/datamessage.h>

#include <cxxtools/json.h>
#include <cxxtools/log.h>

log_define("tntpub.client")

static const unsigned inputBufferSize = 8192;

namespace tntpub
{

void Client::init()
{
    cxxtools::connect(_peer.outputReady, *this, &Client::onOutput);
    cxxtools::connect(_peer.inputReady, *this, &Client::onInput);
    cxxtools::connect(_peer.connected, *this, &Client::onConnected);
    cxxtools::connect(_peer.closed, *this, &Client::onClosed);
}

void Client::beginRead()
{
    if (_inputBuffer.empty())
        _inputBuffer.resize(inputBufferSize);
    _peer.beginRead(_inputBuffer.data(), _inputBuffer.size());
}

Client& Client::subscribe(const std::string& topic, Subscription::Type type)
{
    doSendMessage(DataMessage::subscribe(topic, type));
    return *this;
}

void Client::doSendMessage(const DataMessage& dataMessage)
{
    log_debug("sendMessage of type <" << dataMessage.typeName() << '>');
    if (_peer.writing())
    {
        dataMessage.appendTo(_outputBufferNext);
    }
    else
    {
        dataMessage.appendTo(_outputBuffer);
        _peer.beginWrite(_outputBuffer.data(), _outputBuffer.size());
    }
}

void Client::flush()
{
    if (_peer.writing())
    {
        auto count = _peer.endWrite();
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
    }

    while (!_outputBuffer.empty())
    {
        auto count = _peer.write(_outputBuffer.data(), _outputBuffer.size());
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
    }

    while (!_outputBufferNext.empty())
    {
        auto count = _peer.write(_outputBufferNext.data(), _outputBufferNext.size());
        _outputBufferNext.erase(_outputBufferNext.begin(), _outputBufferNext.begin() + count);
    }
}

const DataMessage& Client::readMessage()
{
    do
    {
        if (_deserializer.processMessage([this](DataMessage& dataMessage) {
            _dataMessage = std::move(dataMessage);
        }))
        {
            return _dataMessage;
        }

        auto count = _peer.endRead();
        _deserializer.addData(_inputBuffer.data(), count);
        beginRead();
    }
    while (!_peer.eof());

    throw std::runtime_error("input stream failed in pubsub client");
}

void Client::onConnected(cxxtools::net::TcpSocket&)
{
    connected(*this);
}

void Client::onClosed(cxxtools::net::TcpSocket&)
{
    closed(*this);
}

void Client::onOutput(cxxtools::IODevice&)
{
    log_debug("onOutput");
    try
    {
        auto count = _peer.endWrite();
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);

        if (_outputBuffer.empty())
        {
            if (_outputBufferNext.empty())
            {
                messagesSent(*this);
            }
            else
            {
                _outputBuffer.swap(_outputBufferNext);
                _peer.beginWrite(_outputBuffer.data(), _outputBuffer.size());
            }
        }
        else
        {
            if (!_outputBufferNext.empty())
            {
                _outputBuffer.insert(_outputBuffer.end(), _outputBufferNext.begin(), _outputBufferNext.end());
                _outputBufferNext.clear();
            }
            _peer.beginWrite(_outputBuffer.data(), _outputBuffer.size());
        }
    }
    catch (const std::exception& e)
    {
        log_warn("output failed: " << e.what());
        _peer.close();
        closed(*this);
        return;
    }

    if (_outputBuffer.empty())
    {
        if (_outputBufferNext.empty())
        {
            log_debug("all messages sent");
            messagesSent(*this);
        }
        else
        {
            _outputBuffer.swap(_outputBufferNext);
        }

    }

    if (!_outputBuffer.empty())
    {
        log_debug("continue writing");
        _peer.beginWrite(_outputBuffer.data(), _outputBuffer.size());
    }
}

void Client::onInput(cxxtools::IODevice&)
{
    log_debug("onInput");

    try
    {
        auto count = _peer.endRead();
        _deserializer.addData(_inputBuffer.data(), count);

        while (_deserializer.processMessage([this](DataMessage& dataMessage) {
            _dataMessage = std::move(dataMessage);
        }))
        {
            log_debug("got message to topic <" << _dataMessage.topic() << '>');
            log_finer(cxxtools::Json(_dataMessage.si()).beautify(true));
            dispatchMessage(_dataMessage);
        }
    }
    catch (const std::exception& e)
    {
        log_warn("read failed: " << e.what());
        _peer.close();
        closed(*this);
        return;
    }

    if (_peer.eof())
        closed(*this);
    else
        beginRead();
}

}
