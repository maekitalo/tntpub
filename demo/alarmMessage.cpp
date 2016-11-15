/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "alarmMessage.h"
#include <cxxtools/serializationinfo.h>

namespace tntpub
{

const std::string AlarmMessage::typeName = "AlarmMessage";
const std::string AlarmCommitMessage::typeName = "AlarmCommitMessage";

void operator<<= (cxxtools::SerializationInfo& si, const AlarmMessage& msg)
{
    si.setTypeName(AlarmMessage::typeName);
    si.addMember("source") <<= msg._source;
    si.addMember("severity") <<= static_cast<int>(msg._severity);
    si.addMember("time") <<= msg._time;
    si.addMember("message") <<= msg._message;
    si.addMember("state") <<= static_cast<int>(msg._state);
}

void operator>>= (const cxxtools::SerializationInfo& si, AlarmMessage& msg)
{
    int value;

    si.getMember("source") >>= msg._source;
    si.getMember("severity") >>= value;
    msg._severity = static_cast<AlarmMessage::Severity>(value);
    si.getMember("time") >>= msg._time;
    si.getMember("message") >>= msg._message;
    si.getMember("state") >>= value;
    msg._state = static_cast<AlarmMessage::State>(value);
}

void operator<<= (cxxtools::SerializationInfo& si, const AlarmCommitMessage& msg)
{
    si.setTypeName(AlarmCommitMessage::typeName);
    si.addMember("source") <<= msg._source;
    si.addMember("severity") <<= static_cast<int>(msg._severity);
    si.addMember("time") <<= msg._time;
}

void operator>>= (const cxxtools::SerializationInfo& si, AlarmCommitMessage& msg)
{
    int value;
    si.getMember("source") >>= msg._source;
    si.getMember("severity") >>= value;
    msg._severity = static_cast<AlarmMessage::Severity>(value);
    si.getMember("time") >>= msg._time;
}

}
