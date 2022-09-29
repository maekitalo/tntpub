/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_SUBSCRIBEMESSAGE_H
#define TNTPUB_SUBSCRIBEMESSAGE_H

#include <string>
#include <tntpub/datamessage.h>
#include <tntpub/subscription.h>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{

class SubscribeMessage : public DataMessage
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const SubscribeMessage& s);
    friend void operator>>= (const cxxtools::SerializationInfo& si, SubscribeMessage& s);

    Subscription::Type _type;
    static void validate(const std::string& topic, Subscription::Type type);

public:
    SubscribeMessage()
        { }

    explicit SubscribeMessage(const std::string& topic, Subscription::Type type = Subscription::Type::Full)
        : DataMessage(topic),
          _type(type)
        { validate(topic, type); }

    template <typename Obj>
    SubscribeMessage(const std::string& topic, const Obj& obj, Subscription::Type type = Subscription::Type::Full)
        : DataMessage(topic, obj),
          _type(type)
        { validate(topic, type); }

    Subscription::Type type() const   { return _type; }
};

}

#endif
