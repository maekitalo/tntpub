/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef MYMESSAGE_H
#define MYMESSAGE_H

#include <cxxtools/serializationinfo.h>
#include <string>

struct MyMessage
{
    std::string text;
    unsigned number;
};

inline void operator<<= (cxxtools::SerializationInfo& si, const MyMessage& msg)
{
    si.setTypeName("MyMessage");
    si.addMember("text") <<= msg.text;
    si.addMember("number") <<= msg.number;
}

inline void operator>>= (const cxxtools::SerializationInfo& si, MyMessage& msg)
{
    si.getMember("text") >>= msg.text;
    si.getMember("number") >>= msg.number;
}

#endif
