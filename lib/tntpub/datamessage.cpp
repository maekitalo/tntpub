/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "datamessage.h"
#include <sstream>
#include <cxxtools/bin/serializer.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/log.h>
#include <cxxtools/json.h>

log_define("tntpub.datamessage")

namespace tntpub
{
DataMessage DataMessage::subscribe(const std::string& topic, Subscription::Type type, const std::string& data)
{
    return DataMessage(
            topic,
            type == Subscription::Type::Praefix ? Type::SubscribePraefix :
            type == Subscription::Type::Regex   ? Type::SubscribeRegex   :
                                                  Type::SubscribeFull,
            data);
}

DataMessage DataMessage::unsubscribe(const std::string& topic, Subscription::Type type)
{
    std::string data;
    if (type != Subscription::Type::Full)
        data = static_cast<char>(type);
    return DataMessage(
            topic,
            type == Subscription::Type::Praefix ? Type::UnsubscribePraefix :
            type == Subscription::Type::Regex   ? Type::UnsubscribeRegex   :
                                                  Type::UnsubscribeFull,
            std::string());
}

DataMessage::DataMessage(const std::string& topic, Type type, const cxxtools::SerializationInfo& data)
    : _type(type),
      _topic(topic),
      _createDateTime(cxxtools::Clock::getSystemTime()),
      _si(data)
{
    setData(_si);
}

void DataMessage::setData(const cxxtools::SerializationInfo& si)
{
    _data = cxxtools::bin::Serializer::toString(si);
    if (&si != &_si)
        _si.clear();
}

const cxxtools::SerializationInfo& DataMessage::si() const
{
    if (_si.isNull() && !_data.empty())
    {
        std::stringbuf ss(_data);
        cxxtools::bin::Deserializer deserializer(ss);
        _si = std::move(deserializer.si());
    }
    return _si;
}

void DataMessage::appendTo(std::vector<char>& buffer) const
{
    auto offset = buffer.size();
    auto messageLength = sizeof(Header) + _topic.size() + _data.size();
    buffer.resize(offset + messageLength);
    auto ptr = buffer.data() + offset;

    Header& header = reinterpret_cast<Header&>(*ptr);

    header._messageLength = messageLength;
    header._createDateJulian = _createDateTime.date().julian();
    header._createTimeUSecs = _createDateTime.time().totalUSecs();
    header._topicLength = _topic.size();
    header._type = _type;

    log_debug("header created; " << cxxtools::Json(header));

    _topic.copy(ptr + header.topicOffset(), _topic.size());
    _data.copy(ptr + header.dataOffset(), _data.size());
}

bool DataMessageDeserializer::processMessage(std::function<void(DataMessage&)> messageReceived)
{
    log_debug("process message; " << _inputData.size() << " bytes available");
    if (_inputData.size() < sizeof(DataMessage::Header))
    {
        log_debug("message incomplete - size: " << _inputData.size() << " expected: " << sizeof(DataMessage::Header));
        return false;
    }

    const DataMessage::Header& header = reinterpret_cast<const DataMessage::Header&>(*_inputData.data());
    log_debug("header detected; " << cxxtools::Json(header));

    if (_inputData.size() < header.messageLength())
    {
        log_debug("message data incomplete - size: " << _inputData.size() << " expected: " << header.messageLength());
        return false;
    }

    std::string topic(_inputData.data() + header.topicOffset(), header.topicLength());
    log_debug("topic: <" << topic << '>');
    std::string data(_inputData.data() + header.dataOffset(), header.dataLength());

    DataMessage dataMessage(topic, header._type, header.createDateTime(), data);

    _inputData.erase(0, header.messageLength());

    log_debug("message processed " << _inputData.size() << " left; capacity " << _inputData.capacity());

    messageReceived(dataMessage);
    message(dataMessage);

    return true;
}

unsigned DataMessageDeserializer::advance(const char* buffer, unsigned bufsize, std::function<void(DataMessage&)> messageReceived)
{
    _inputData.append(buffer, bufsize);
    unsigned count = 0;
    while (processMessage(messageReceived))
        ++count;
    return count;
}

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm)
{
    si.addMember("type") <<= cxxtools::EnumClass(dm._type);
    si.addMember("topic") <<= dm._topic;
    si.addMember("createDateTime") <<= dm._createDateTime;
    si.addMember("data") <<= dm._data;
}

void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm)
{
    si.getMember("type") >>= cxxtools::EnumClass(dm._type);
    si.getMember("topic") >>= dm._topic;
    si.getMember("createDateTime") >>= dm._createDateTime;
    si.getMember("data") >>= dm._data;
}

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage::Header& header)
{
    si.addMember("messageLength") <<= header._messageLength;
    si.addMember("createDate") <<= header.createDateTime();
    si.addMember("topicLength") <<= header._topicLength;
    si.addMember("dataOffset") <<= header.dataOffset();
    si.addMember("dataLength") <<= header.dataLength();
    si.addMember("type") <<= cxxtools::EnumClass(header._type);
}

}
