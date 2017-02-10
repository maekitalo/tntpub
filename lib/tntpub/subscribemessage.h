/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SUBSCRIBEMESSAGE_H
#define TNTPUB_SUBSCRIBEMESSAGE_H

#include <string>
#include <tntpub/datamessage.h>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{

class SubscribeMessage : public DataMessage
{
public:
    SubscribeMessage()
        { }

    explicit SubscribeMessage(const std::string& topic)
        : DataMessage(topic)
        { }

    template <typename Obj>
    SubscribeMessage(const std::string& topic, const Obj& obj)
        : DataMessage(topic, obj)
        { }
};

void operator<<= (cxxtools::SerializationInfo& si, const SubscribeMessage& s);

}

#endif
