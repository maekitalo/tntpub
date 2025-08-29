/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <tntpub/datamessage.h>
#include <cxxtools/bin/serializer.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/log.h>
#include <cxxtools/json.h>
#include <cxxtools/hexdump.h>
#include <sstream>
#include <iostream>

log_define("tntpub.datamessage")

namespace tntpub
{
decltype(DataMessage::_serial) DataMessage::_lastSerial = 0;

DataMessage DataMessage::subscribe(const Topic& topic, Subscription::Type type, const std::string& data)
{
    return DataMessage(
            topic,
            type == Subscription::Type::Prefix ? Type::SubscribePrefix :
            type == Subscription::Type::Regex  ? Type::SubscribeRegex   :
                                                 Type::SubscribeFull,
            data);
}

DataMessage DataMessage::unsubscribe(const Topic& topic, Subscription::Type type)
{
    std::string data;
    if (type != Subscription::Type::Full)
        data = static_cast<char>(type);
    return DataMessage(
            topic,
            type == Subscription::Type::Prefix ? Type::UnsubscribePrefix :
            type == Subscription::Type::Regex  ? Type::UnsubscribeRegex   :
                                                 Type::UnsubscribeFull,
            std::string());
}

DataMessage::DataMessage(const Topic& topic, Type type, cxxtools::SerializationInfo&& data)
    : _type(type),
      _topic(topic),
      _createDateTime(cxxtools::Clock::getSystemTime()),
      _si(std::move(data))
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
    auto messageLength = sizeof(Header) + _topic.topic().size() + _topic.subtopic().size() + _data.size();
    buffer.resize(offset + messageLength);
    auto ptr = buffer.data() + offset;

    Header& header = reinterpret_cast<Header&>(*ptr);

    header._magic = Header::magic;
    header._messageLength = messageLength;
    header._createDateJulian = _createDateTime.date().julian();
    header._createTimeUSecs = _createDateTime.time().totalUSecs();
    header._topicLength = _topic.topic().size();
    header._subtopicLength = _topic.subtopic().size();
    header._type = _type;
    header._serial = _serial;

    log_debug("header created; " << cxxtools::Json(header));

    _topic.topic().copy(ptr + header.topicOffset(), _topic.topic().size());
    _topic.subtopic().copy(ptr + header.subtopicOffset(), _topic.subtopic().size());
    _data.copy(ptr + header.dataOffset(), _data.size());
}

DataMessage DataMessage::createFromBuffer(const char* data, unsigned size)
{
    DataMessage result;

    DataMessageDeserializer deserializer;

    if (!deserializer.processMessage(
                data,
                size,
                [&result] (DataMessage& dm) {
                    result = std::move(dm);
                }))
        throw std::runtime_error("failed to create data message from data");

    return result;
}

unsigned DataMessageDeserializer::processMessage(const char* buffer, unsigned bufsize, std::function<void(DataMessage&)> messageReceived)
{
    log_debug("process message; " << bufsize << " bytes available");
    log_finest(cxxtools::hexDump(buffer, bufsize));

    if (bufsize < sizeof(DataMessage::Header1))
    {
        log_debug("message incomplete - size: " << bufsize << " expected: " << sizeof(DataMessage::Header));
        return 0;
    }

    unsigned remaining = bufsize;
    const auto& header = reinterpret_cast<const DataMessage::Header&>(*buffer);

    log_finer("header magic number " << header._magic);
    if (header._magic == DataMessage::Header::magic)
    {
        if (bufsize < sizeof(DataMessage::Header))
        {
            log_debug("message incomplete - size: " << bufsize << " expected: " << sizeof(DataMessage::Header));
            return 0;
        }

        if (bufsize < header.messageLength())
        {
            log_debug("message data incomplete - size: " << bufsize << " expected: " << header.messageLength());
            return 0;
        }

        DataMessage dataMessage(
            Topic(std::string(buffer + header.topicOffset(), header.topicLength()),
                  std::string(buffer + header.subtopicOffset(), header.subtopicLength())),
            header._type,
            header.createDateTime(),
            std::string(buffer + header.dataOffset(), header.dataLength()));

        dataMessage._serial = header._serial;
        buffer += header.messageLength();
        remaining -= header.messageLength();

        log_debug("message to topic <" << dataMessage.topic().topic() << "> processed " << remaining << " left");

        messageReceived(dataMessage);
        message(dataMessage);
    }
    else
    {
        // old format
        log_debug("old format detected");
        const auto& header = reinterpret_cast<const DataMessage::Header1&>(*buffer);

        if (bufsize < header.messageLength())
        {
            log_debug("message data incomplete - size: " << bufsize << " expected: " << header.messageLength());
            return 0;
        }

        DataMessage dataMessage(
            Topic(std::string(buffer + header.topicOffset(), header.topicLength())),
            header._type,
            header.createDateTime(),
            std::string(buffer + header.dataOffset(), header.dataLength()));

        dataMessage._serial = header._serial;
        buffer += header.messageLength();
        remaining -= header.messageLength();

        log_debug("message to topic <" << dataMessage.topic().topic() << "> processed " << remaining << " left");

        messageReceived(dataMessage);
        message(dataMessage);
    }

    return bufsize - remaining;
}

bool DataMessageDeserializer::processMessage(std::function<void(DataMessage&)> messageReceived)
{
    unsigned count = processMessage(_inputData.data(), _inputData.size(), messageReceived);

    if (count == 0)
        return false;

    _inputData.erase(_inputData.begin(), _inputData.begin() + count);

    return true;
}

unsigned DataMessageDeserializer::advance(const char* buffer, unsigned bufsize, std::function<void(DataMessage&)> messageReceived)
{
    unsigned count = 0;

    if (_inputData.empty())
    {
        while (true)
        {
            auto n = processMessage(buffer, bufsize, messageReceived);
            if (n == 0)
                break;
            buffer += n;
            bufsize -= n;
        }
    }

    if (bufsize > 0)
    {
        _inputData.append(buffer, bufsize);
        while (processMessage(messageReceived))
            ++count;
    }

    return count;
}

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm)
{
    si.addMember("type") <<= cxxtools::EnumClass(dm._type);
    si.addMember("topic") <<= dm._topic.topic();
    if (dm._topic.subtopic().size() > 0)
        si.addMember("subtopic") <<= dm._topic.subtopic();
    si.addMember("serial") <<= dm._serial;
    si.addMember("createDateTime") <<= dm._createDateTime;
    si.addMember("data") <<= dm._data;
}

void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm)
{
    si.getMember("type") >>= cxxtools::EnumClass(dm._type);
    si.getMember("topic") >>= dm._topic._topic;
    si.getMember("subtopic", dm._topic._subtopic);
    si.getMember("serial") >>= dm._serial;
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

std::ostream& operator<< (std::ostream& out, const Topic& topic)
{
    out << topic.topic();
    if (!topic.subtopic().empty())
        out << '[' << topic.subtopic() << ']';
    return out;
}

}
