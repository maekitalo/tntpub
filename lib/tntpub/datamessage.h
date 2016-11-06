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

    template <typename Obj> void get(Obj& obj) const
    { data >>= obj; }
};

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm);
void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm);

}

#endif
