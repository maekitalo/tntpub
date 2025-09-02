/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_DATAMESSAGE_H
#define TNTPUB_DATAMESSAGE_H

#include <tntpub/subscription.h>
#include <tntpub/topic.h>
#include <cxxtools/clock.h>
#include <cxxtools/datetime.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/signal.h>
#include <string>
#include <functional>

namespace tntpub
{

/** Data message describes the data sent through pubsub channels.
 *
 *  A data message carries the topic and a serializable object.
 */
class DataMessage
{
    friend class DataMessageDeserializer;
    friend void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm);
    friend void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm);

public:
    enum class Type : char {
        Null = ' ',
        SubscribeFull = 'F',
        SubscribePrefix = 'P',
        SubscribeRegex = 'X',
        UnsubscribeFull = 'f',
        UnsubscribePrefix = 'p',
        UnsubscribeRegex = 'x',
        Data = 'D',
        PlainData = 'R',
        System = 'S'
    };

    struct Header1
    {
        uint32_t _messageLength;
        unsigned _createDateJulian;
        uint64_t _createTimeUSecs;
        uint64_t _serial;
        uint16_t _topicLength;
        Type _type;

        uint32_t messageLength() const      { return _messageLength; }
        uint32_t topicOffset() const        { return sizeof(Header); }
        uint16_t topicLength() const        { return _topicLength; }
        uint32_t dataOffset() const         { return topicOffset() + _topicLength; }
        uint32_t dataLength() const         { return _messageLength - dataOffset(); }
        cxxtools::UtcDateTime createDateTime() const
        {
            cxxtools::Date createDate;
            cxxtools::Time createTime;
            createDate.setJulian(_createDateJulian);
            createTime.setTotalUSecs(_createTimeUSecs);
            return cxxtools::UtcDateTime(createDate, createTime);
        }
    };

    struct Header
    {
        static const uint32_t magic = 0x74707532;   // "tpu2"

        uint32_t _magic = magic;
        uint32_t _messageLength;
        unsigned _createDateJulian;
        uint64_t _createTimeUSecs;
        uint64_t _serial;
        uint16_t _topicLength;
        uint16_t _subtopicLength;
        Type _type;

        uint32_t messageLength() const      { return _messageLength; }
        uint32_t topicOffset() const        { return sizeof(Header); }
        uint16_t topicLength() const        { return _topicLength; }
        uint32_t subtopicOffset() const     { return topicOffset() + _topicLength; }
        uint16_t subtopicLength() const     { return _subtopicLength; }
        uint32_t dataOffset() const         { return subtopicOffset() + _subtopicLength; }
        uint32_t dataLength() const         { return _messageLength - dataOffset(); }

        cxxtools::UtcDateTime createDateTime() const
        {
            cxxtools::Date createDate;
            cxxtools::Time createTime;
            createDate.setJulian(_createDateJulian);
            createTime.setTotalUSecs(_createTimeUSecs);
            return cxxtools::UtcDateTime(createDate, createTime);
        }
    };

private:
    Type _type = Type::Null;
    Topic _topic;
    decltype(Header::_serial) _serial = 0;
    cxxtools::UtcDateTime _createDateTime;
    std::string _data;
    mutable cxxtools::SerializationInfo _si;

    void setData(const cxxtools::SerializationInfo& si);
    static decltype(_serial)  _lastSerial;

    DataMessage(const Topic& topic, Type type, const cxxtools::UtcDateTime& createDateTime, const std::string& data)
        : _type(type),
          _topic(topic),
          _createDateTime(createDateTime),
          _data(data)
        { }

    DataMessage(const Topic& topic, Type type, const std::string& data)
        : _type(type),
          _topic(topic),
          _createDateTime(cxxtools::Clock::getSystemTime()),
          _data(data)
        { }

    DataMessage(const Topic& topic, Type type, cxxtools::SerializationInfo&& data);

public:
    DataMessage() = default;

    DataMessage(const DataMessage& dm)
        : _type(dm._type),
          _topic(dm._topic),
          _serial(dm._serial),
          _createDateTime(dm._createDateTime),
          _data(dm._data),
          _si()
          { }

    DataMessage& operator=(const DataMessage& dm)
    {
        _type = dm._type;
        _topic = dm._topic;
        _serial = dm._serial;
        _createDateTime = dm._createDateTime;
        _data = dm._data;
        _si.clear();
        return *this;
    }

    DataMessage(DataMessage&&) = default;
    DataMessage& operator= (DataMessage&&) = default;

    /// Creates a data message with a object.
    /// The object is serialized using cxxtools serialization.
    /// @deprecated  - use DataMessage::create instead
    template <typename Obj>
#if __cplusplus >= 201402L
    [[deprecated("use DataMessage::create instead")]]
#endif
    DataMessage(const Topic& topic, const Obj& obj)
        : _type(Type::Data),
          _serial(0),
          _topic(topic),
          _createDateTime(cxxtools::Clock::getSystemTime())
    {
        _si <<= obj;
        setData(_si);
    }

    static DataMessage subscribe(const Topic& topic, Subscription::Type type = Subscription::Type::Full, const std::string& data = std::string());

    template <typename T>
    static DataMessage subscribeWithObject(const Topic& topic, const T& obj, Subscription::Type type = Subscription::Type::Full)
    {
        cxxtools::SerializationInfo si;
        si <<= obj;
        return DataMessage(topic,
            type == Subscription::Type::Prefix ? Type::SubscribePrefix :
            type == Subscription::Type::Regex  ? Type::SubscribeRegex   :
                                                 Type::SubscribeFull,
            std::move(si));
    }

    static DataMessage unsubscribe(const Topic& topic, Subscription::Type type = Subscription::Type::Null);

    static DataMessage createPlain(const Topic& topic, const std::string& data)
    { return DataMessage(topic, Type::PlainData, data); }

    template <typename T>
    static DataMessage create(const Topic& topic, const T& obj)
    {
        cxxtools::SerializationInfo si;
        si <<= obj;
        return DataMessage(topic, Type::Data, std::move(si));
    }

    template <typename T>
    static DataMessage createSystem(const Topic& topic, const T& obj)
    {
        cxxtools::SerializationInfo si;
        si <<= obj;
        return DataMessage(topic, Type::System, std::move(si));
    }

    /// Returns the topic where the message is sent through.
    const Topic& topic() const
        { return _topic; }
    /// Sets the topic of the message.
    void topic(const Topic& topic)
        { _topic = topic; }

    /// Returns the type
    Type type() const 
        { return _type; }
    decltype(_serial) serial() const
        { return _serial; }
    static decltype(_serial) lastSerial()
        { return _lastSerial; }
    bool isDataMessage() const
        { return _type == Type::Data || _type == Type::PlainData; }
    bool isSubscribeMessage() const
        { return _type == Type::SubscribeFull || _type == Type::SubscribePrefix || _type == Type::SubscribeRegex; }
    bool isUnsubscribeMessage() const
        { return _type == Type::UnsubscribeFull || _type == Type::UnsubscribePrefix || _type == Type::UnsubscribeRegex; }
    bool isSystemMessage() const
        { return _type == Type::System; }

    /// Returns the time, when the data message is created and hence initially received.
    const cxxtools::UtcDateTime& createDateTime() const
        { return _createDateTime; }

    /// Returns the data of the message.
    const std::string& data() const
        { return _data; }

    const cxxtools::SerializationInfo& si() const;

    const std::string& typeName() const
        { return si().typeName(); }

    /// Deserializes the object carried by this data message.
    template <typename Obj> void get(Obj& obj) const
    { si() >>= obj; }

    void touch()
        { _createDateTime = cxxtools::Clock::getSystemTime(); }

    void setNextSerial()
        { _serial = ++_lastSerial; }
    void serial(uint32_t v)
        { _serial = v; }

    static Subscription::Type subscriptionType(Type messageType);

    void appendTo(std::vector<char>& buffer) const;
    static DataMessage createFromBuffer(const char* data, unsigned size);
    static DataMessage createFromBuffer(const std::vector<char>& buffer)
        { return createFromBuffer(buffer.data(), buffer.size()); }
};

class DataMessageDeserializer
{
    std::string _inputData;

public:
    void addData(const char* buffer, unsigned bufsize)
        { _inputData.append(buffer, bufsize); }

    // process message - returns number of bytes consumed
    unsigned processMessage(const char* buffer, unsigned bufsize, std::function<void(DataMessage&)> messageReceived);
    bool processMessage(std::function<void(DataMessage&)> messageReceived);
    unsigned advance(const char* buffer, unsigned bufsize, std::function<void(DataMessage&)> messageReceived);
    unsigned in_avail() const   { return _inputData.size(); }

    cxxtools::Signal<DataMessage&> message;
};

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm);
void operator<<= (cxxtools::SerializationInfo& si, const DataMessage::Header& header);

}

#endif
