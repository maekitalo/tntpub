/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SUBSCRIBEMESSAGE_H
#define TNTPUB_SUBSCRIBEMESSAGE_H

#include <string>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{

struct SubscribeMessage
{
    std::string topic;
};

void operator<<= (cxxtools::SerializationInfo& si, const SubscribeMessage& s);
void operator>>= (const cxxtools::SerializationInfo& si, SubscribeMessage& s);

}

#endif
