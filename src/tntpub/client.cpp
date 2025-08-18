/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/client.h>
#include <tntpub/datamessage.h>

#include <cxxtools/json.h>
#include <cxxtools/log.h>

log_define("tntpub.client")

static const unsigned bufferSize = 8192;

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
        _inputBuffer.resize(bufferSize);
    _peer.beginRead(_inputBuffer.data(), _inputBuffer.size());
}

Client& Client::subscribe(const Topic& topic, Subscription::Type type, const std::string& data)
{
    doSendMessage(DataMessage::subscribe(topic, type, data));
    return *this;
}

void Client::doSendMessage(const DataMessage& dataMessage)
{
    log_debug("sendMessage of type <" << static_cast<std::underlying_type<DataMessage::Type>::type>(dataMessage.type()) << '>');
    if (_autoSync)
    {
        dataMessage.appendTo(_outputBuffer);
        while (!_outputBuffer.empty())
        {
            auto count = _peer.write(_outputBuffer.data(), _outputBuffer.size());
            _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
        }
    }
    else
    {
        if (_peer.writing())
        {
            log_finer("append to next buffer");
            dataMessage.appendTo(_outputBufferNext);

            log_finer("output buffer " << _outputBufferNext.size() << " buffer size " << bufferSize << " wavail=" << _peer.wavail());
        }
        else
        {
            log_finer("append to buffer and begin writing");
            dataMessage.appendTo(_outputBuffer);
            _peer.beginWrite(_outputBuffer.data(), _outputBuffer.size());
        }
    }
}

void Client::flush()
{
    log_debug("flush; writing " << _peer.writing() << " output buffer size " << _outputBuffer.size() << " next size " << _outputBufferNext.size());

    if (_peer.writing())
    {
        auto count = _peer.endWrite();
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
        log_finer(count << " written from pending output " << _outputBuffer.size() << " kept");
    }

    while (!_outputBuffer.empty())
    {
        auto count = _peer.write(_outputBuffer.data(), _outputBuffer.size());
        _outputBuffer.erase(_outputBuffer.begin(), _outputBuffer.begin() + count);
        log_finer(count << " written from output buffer " << _outputBuffer.size() << " kept");
    }

    while (!_outputBufferNext.empty())
    {
        auto count = _peer.write(_outputBufferNext.data(), _outputBufferNext.size());
        _outputBufferNext.erase(_outputBufferNext.begin(), _outputBufferNext.begin() + count);
        log_finer(count << " written from next output buffer " << _outputBufferNext.size() << " kept");
    }
}

DataMessage& Client::readMessage()
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
        log_finer(count << " bytes written, " << _outputBuffer.size() << " left");

        if (_outputBuffer.empty())
        {
            if (_outputBufferNext.empty())
            {
                log_finer("output buffer empty - signal messagesSent");
                messagesSent(*this);
            }
            else
            {
                log_finer("output buffer written, take next buffer " << _outputBufferNext.size());
                _outputBuffer.swap(_outputBufferNext);
                _peer.beginWrite(_outputBuffer.data(), _outputBuffer.size());
            }
        }
        else
        {
            if (!_outputBufferNext.empty())
            {
                log_finer("add " << _outputBufferNext.size() << " bytes to output buffer");
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
            log_debug("got message to topic <" << _dataMessage.topic().topic() << '>');
            log_finer_if(_dataMessage.type() == DataMessage::Type::Data, cxxtools::Json(_dataMessage.si()).beautify(true));
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
