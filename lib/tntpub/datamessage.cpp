/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "datamessage.h"

namespace tntpub
{

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm)
{
    si.setTypeName("DataMessage");
    si.addMember("topic") <<= dm._topic;
    si.addMember("data") <<= dm._data;
}

void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm)
{
    si.getMember("topic") >>= dm._topic;
    si.getMember("data") >>= dm._data;
}

}
