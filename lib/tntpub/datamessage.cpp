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
    si.addMember("topic") <<= dm.topic;
    si.addMember("data") <<= dm.data;
}

void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm)
{
    si.getMember("topic") >>= dm.topic;
    si.getMember("data") >>= dm.data;
}

}
