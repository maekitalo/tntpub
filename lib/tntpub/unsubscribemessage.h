/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_UNSUBSCRIBEMESSAGE_H
#define TNTPUB_UNSUBSCRIBEMESSAGE_H

#include <string>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{

struct UnsubscribeMessage
{
    std::string topic;
};

void operator<<= (cxxtools::SerializationInfo& si, const UnsubscribeMessage& s);
void operator>>= (const cxxtools::SerializationInfo& si, UnsubscribeMessage& s);

}

#endif
