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
    DataMessage() {}

    template <typename Obj>
    DataMessage(const std::string& topic, const Obj& obj)
        : _topic(topic)
    {
        _data <<= obj;
    }

    template <typename Obj> void get(Obj& obj) const
    { _data >>= obj; }

    const std::string& topic() const
    { return _topic; }

    const std::string& typeName() const
    { return _data.typeName(); }
};

}

#endif
