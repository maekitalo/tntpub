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
    si <<= static_cast<const DataMessage&>(s);
    si.setTypeName("SubscribeMessage");
}

}
