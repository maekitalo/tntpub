/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "subscribemessage.h"
#include <cxxtools/serializationinfo.h>

namespace tntpub
{

void operator<<= (cxxtools::SerializationInfo& si, const SubscribeMessage& s)
{
    si.setTypeName("SubscribeMessage");
    si.addMember("topic") <<= s.topic;
}

void operator>>= (const cxxtools::SerializationInfo& si, SubscribeMessage& s)
{
    si.getMember("topic") >>= s.topic;
}

}
