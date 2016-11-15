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

struct DataMessage
{
    std::string topic;
    cxxtools::SerializationInfo data;

    DataMessage() {}

    template <typename Obj>
    DataMessage(const std::string& topic_, const Obj& obj)
        : topic(topic_)
    {
        data <<= obj;
    }

    template <typename Obj> void get(Obj& obj) const
    { data >>= obj; }

    const std::string& typeName() const
    { return data.typeName(); }
};

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm);
void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm);

}

#endif
