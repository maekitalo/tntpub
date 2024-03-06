/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_DATAMESSAGE_H
#define TNTPUB_DATAMESSAGE_H

#include <tntpub/subscription.h>
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
        SubscribePraefix = 'P',
        SubscribeRegex = 'X',
        UnsubscribeFull = 'f',
        UnsubscribePraefix = 'p',
        UnsubscribeRegex = 'x',
        Data = 'D',
        PlainData = 'R'
    };

    struct Header
    {
        uint32_t _messageLength;
        uint64_t _createDateJulian;
        uint64_t _createTimeUSecs;
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

private:
    Type _type = Type::Null;
    std::string _topic;
    cxxtools::UtcDateTime _createDateTime;
    std::string _data;
    mutable cxxtools::SerializationInfo _si;

    void setData(const cxxtools::SerializationInfo& si);

    DataMessage(const std::string& topic, Type type, const cxxtools::UtcDateTime& createDateTime, const std::string& data)
        : _type(type),
          _topic(topic),
          _createDateTime(createDateTime),
          _data(data)
        { }

    DataMessage(const std::string& topic, Type type, const std::string& data)
        : _type(type),
          _topic(topic),
          _createDateTime(cxxtools::Clock::getSystemTime()),
          _data(data)
        { }

    DataMessage(const std::string& topic, Type type, const cxxtools::SerializationInfo& data);

public:
    DataMessage() = default;

    DataMessage(const DataMessage& dm)
        : _type(dm._type),
          _topic(dm._topic),
          _createDateTime(dm._createDateTime),
          _data(dm._data),
          _si()
          { }

    DataMessage& operator=(const DataMessage& dm)
    {
        _type = dm._type;
        _topic = dm._topic;
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
    DataMessage(const std::string& topic, const Obj& obj)
        : _type(Type::Data),
          _topic(topic),
          _createDateTime(cxxtools::Clock::getSystemTime())
    {
        _si <<= obj;
        setData(_si);
    }

    static DataMessage subscribe(const std::string& topic, Subscription::Type type = Subscription::Type::Full, const std::string& data = std::string());

    static DataMessage unsubscribe(const std::string& topic, Subscription::Type type = Subscription::Type::Null);

    static DataMessage createPlain(const std::string& topic, const std::string& data)
    { return DataMessage(topic, Type::PlainData, data); }

    template <typename T>
    static DataMessage create(const std::string& topic, const T& obj)
    {
        cxxtools::SerializationInfo si;
        si <<= obj;
        return DataMessage(topic, Type::Data, si);
    }

    /// Returns the topic where the message is sent through.
    const std::string& topic() const
        { return _topic; }

    /// Returns the type
    Type type() const 
        { return _type; }
    bool isDataMessage() const
        { return _type == Type::Data || _type == Type::PlainData; }
    bool isSubscribeMessage() const
        { return _type == Type::SubscribeFull || _type == Type::SubscribePraefix || _type == Type::SubscribeRegex; }
    bool isUnsubscribeMessage() const
        { return _type == Type::UnsubscribeFull || _type == Type::UnsubscribePraefix || _type == Type::UnsubscribeRegex; }

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

    void appendTo(std::vector<char>& buffer) const;
};

class DataMessageDeserializer
{
    std::string _inputData;

public:
    void addData(const char* buffer, unsigned bufsize)
        { _inputData.append(buffer, bufsize); }
    bool processMessage(std::function<void(DataMessage&)> messageReceived);
    unsigned advance(const char* buffer, unsigned bufsize, std::function<void(DataMessage&)> messageReceived);
    unsigned in_avail() const   { return _inputData.size(); }

    cxxtools::Signal<DataMessage&> message;
};

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm);
void operator<<= (cxxtools::SerializationInfo& si, const DataMessage::Header& header);

}

#endif
