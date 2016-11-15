/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#ifndef TNTPUB_ALARM_MESSAGE_H
#define TNTPUB_ALARM_MESSAGE_H

#include <string>
#include <cxxtools/datetime.h>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{
const static std::string alarmTopic = "alarm";

class AlarmMessage
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const AlarmMessage& msg);
    friend void operator>>= (const cxxtools::SerializationInfo& si, AlarmMessage& msg);
    friend class AlarmCommitMessage;

public:
    static const std::string typeName;

    enum Severity { INFO, WARNING, ERROR, FATAL };
    enum State { NEW, COMMITTED };

    AlarmMessage()
        : _severity(INFO),
          _time(cxxtools::DateTime::gmtime()),
          _state(NEW)
    { }

    AlarmMessage(const std::string& source_, const std::string& message_, Severity severity_ = INFO)
        : _source(source_),
          _severity(severity_),
          _time(cxxtools::DateTime::gmtime()),
          _message(message_),
          _state(NEW)
    { }

    const std::string& source() const    { return _source; }
    const char* severity() const         { return _severity == INFO ? "INFO" :
                                                  _severity == WARNING ? "WARNING" :
                                                  _severity == ERROR ? "ERROR" : "FATAL"; }

    cxxtools::DateTime time() const      { return _time; }
    const std::string& message() const   { return _message; }
    State state() const                  { return _state; }

private:
    std::string _source;
    Severity _severity;
    cxxtools::DateTime _time;
    std::string _message;
    State _state;

};

class AlarmCommitMessage
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const AlarmCommitMessage& msg);
    friend void operator>>= (const cxxtools::SerializationInfo& si, AlarmCommitMessage& msg);

public:
    static const std::string typeName;

    AlarmCommitMessage()
        : _severity(AlarmMessage::INFO)
    { }

    AlarmCommitMessage(const std::string& source_, AlarmMessage::Severity severity_, const cxxtools::DateTime& time_)
        : _source(source_),
          _severity(severity_),
          _time(time_)
    { }

    bool match(const AlarmMessage& am) const
    { return _source == am._source && _severity == am._severity && _time == am._time; }

    const std::string& source() const    { return _source; }
    const char* severity() const         { return _severity == AlarmMessage::INFO ? "INFO" :
                                                  _severity == AlarmMessage::WARNING ? "WARNING" :
                                                  _severity == AlarmMessage::ERROR ? "ERROR" : "FATAL"; }
    cxxtools::DateTime time() const      { return _time; }

private:
    std::string _source;
    AlarmMessage::Severity _severity;
    cxxtools::DateTime _time;
};

void operator<<= (cxxtools::SerializationInfo& si, const AlarmMessage& msg);
void operator>>= (const cxxtools::SerializationInfo& si, AlarmMessage& msg);

void operator<<= (cxxtools::SerializationInfo& si, const AlarmCommitMessage& msg);
void operator>>= (const cxxtools::SerializationInfo& si, AlarmCommitMessage& msg);

}

#endif
