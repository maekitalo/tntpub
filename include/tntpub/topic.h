#ifndef TNTPUB_TOPIC_H
#define TNTPUB_TOPIC_H

#include <string>
#include <iosfwd>

namespace cxxtools
{
    class SerializationInfo;
}

namespace tntpub
{
class DataMessage;

class Topic
{
    friend void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm);

    std::string _topic;
    std::string _subtopic;

public:
    Topic() = default;
    Topic(const std::string& topic, const std::string& subtopic = std::string())
        : _topic(topic),
          _subtopic(subtopic)
    { }
    Topic(const char* topic, const char* subtopic = "")
        : _topic(topic),
          _subtopic(subtopic)
        { }

    const std::string& topic() const        { return _topic; }
    const std::string& subtopic() const     { return _subtopic; }

    bool match(const Topic& other) const
    {
        if (_topic != other._topic)
            return false;
        return other._subtopic.empty() || _subtopic == other._subtopic;
    }
};

inline bool operator== (const Topic& l, const Topic& r)
{
    return l.topic() == r.topic()
        && l.subtopic() == r.subtopic();
}

std::ostream& operator<< (std::ostream& out, const Topic& topic);

}

#endif
