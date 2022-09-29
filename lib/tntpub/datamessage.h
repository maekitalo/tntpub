/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_DATAMESSAGE_H
#define TNTPUB_DATAMESSAGE_H

#include <cxxtools/serializationinfo.h>
#include <cxxtools/clock.h>
#include <cxxtools/datetime.h>
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
    cxxtools::UtcDateTime _createTime;
    std::string _rawdata;
    mutable cxxtools::SerializationInfo _data;

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
        : _topic(topic),
          _createTime(cxxtools::Clock::getSystemTime())
    {
        _data <<= obj;
    }

    /// Returns the topic where the message is sent through.
    const std::string& topic() const
    { return _topic; }

    /// Returns the time, when the data message is created and hence initially received.
    const cxxtools::UtcDateTime& createTime() const
    { return _createTime; }

    /// Returns the data of the message.
    const cxxtools::SerializationInfo& data() const;

    /// Deserializes the object carried by this data message.
    template <typename Obj> void get(Obj& obj) const
    { data() >>= obj; }

    /// Returns the type name of the data object.
    const std::string& typeName() const
    { return data().typeName(); }
};

/** This class helps visualising a data message.
 *
 *  The data message serializes the data as a binary string. This makes
 *  it difficult to see, what is inside e.g. for logging. This class helps
 *  by deserializing the data when serializing and serializing the deserialized
 *  raw data.
 *
 *  Example:
 *  @code
 *      tntpub::DataMessage dm("someTopic", aObject);
 *      log_debug(cxxtools::Json(tntpub::DataMessageView(dm)).beautify(true));
 *  @endcode
 *
 *  In the example above a datamessage is wrapped by the view and the passed
 *  to the json serializer helper, which outputs the message as json.
*/

class DataMessageView
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const DataMessageView& dv);
    const DataMessage& _dm;

public:
    DataMessageView(const DataMessage& dm)
        : _dm(dm)
        { }
};

}

#endif
