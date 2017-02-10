/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_UNSUBSCRIBEMESSAGE_H
#define TNTPUB_UNSUBSCRIBEMESSAGE_H

#include <string>
#include <tntpub/datamessage.h>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{

class UnsubscribeMessage : public DataMessage
{
public:
    UnsubscribeMessage()
        { }

    explicit UnsubscribeMessage(const std::string& topic)
        : DataMessage(topic)
        { }

    template <typename Obj>
    UnsubscribeMessage(const std::string& topic, const Obj& obj)
        : DataMessage(topic, obj)
        { }
};

void operator<<= (cxxtools::SerializationInfo& si, const UnsubscribeMessage& s);

}

#endif
