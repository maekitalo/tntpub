/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_DATAMESSAGE_H
#define TNTPUB_DATAMESSAGE_H

#include <cxxtools/serializationinfo.h>
#include <string>

namespace tntpub
{

/** Data message describes the data sent through pubsub channels.
 *
 *  A data message carries the topic and a serializable object.
 */
class DataMessage
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm);
    friend void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm);

    std::string _topic;
    cxxtools::SerializationInfo _data;

protected:
    explicit DataMessage(const std::string& topic)
        : _topic(topic)
        { }

public:
    /// A data message is default constructable
    DataMessage() {}

    /// Creates a data message with a object.
    /// The object is serialized using cxxtools serialization.
    template <typename Obj>
    DataMessage(const std::string& topic, const Obj& obj)
        : _topic(topic)
    {
        _data <<= obj;
    }

    /// Returns the topic where the message is sent through.
    const std::string& topic() const
    { return _topic; }

    /// Returns the data of the message.
    const cxxtools::SerializationInfo& data() const
    { return _data; }

    /// Deserializes the object carried by this data message.
    template <typename Obj> void get(Obj& obj) const
    { _data >>= obj; }

    /// Returns the type name of the data object.
    const std::string& typeName() const
    { return _data.typeName(); }
};

}

#endif
