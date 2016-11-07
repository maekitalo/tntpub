/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "unsubscribemessage.h"
#include <cxxtools/serializationinfo.h>

namespace tntpub
{

void operator<<= (cxxtools::SerializationInfo& si, const UnsubscribeMessage& s)
{
    si.setTypeName("UnsubscribeMessage");
    si.addMember("topic") <<= s.topic;
}

void operator>>= (const cxxtools::SerializationInfo& si, UnsubscribeMessage& s)
{
    si.getMember("topic") >>= s.topic;
}

}
