/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "subscribemessage.h"
#include <cxxtools/serializationinfo.h>
#include <cxxtools/regex.h>

namespace tntpub
{
void SubscribeMessage::validate(const std::string& topic, Subscription::Type type)
{
    if (type == Subscription::Type::Regex)
    {
        cxxtools::Regex re(topic);
    }
}

void operator<<= (cxxtools::SerializationInfo& si, const SubscribeMessage& s)
{
    si <<= static_cast<const DataMessage&>(s);
    if (s._type != Subscription::Type::Full)
        si.addMember("type") <<= cxxtools::EnumClass(s._type);
    si.setTypeName("SubscribeMessage");
}

void operator>>= (const cxxtools::SerializationInfo& si, SubscribeMessage& s)
{
    si >>= static_cast<DataMessage&>(s);
    auto pi = si.findMember("type");
    if (pi)
        *pi >>= cxxtools::EnumClass(s._type);
    else
        s._type = Subscription::Type::Full;
}

}
