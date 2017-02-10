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
    si <<= static_cast<const DataMessage&>(s);
    si.setTypeName("UnsubscribeMessage");
}

}
