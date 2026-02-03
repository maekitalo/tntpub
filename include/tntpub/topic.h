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

    std::string _main;
    std::string _sub;

public:
    Topic() = default;
    Topic(const std::string& main, const std::string& sub = std::string())
        : _main(main),
          _sub(sub)
    { }
    Topic(const char* main, const char* sub = "")
        : _main(main),
          _sub(sub)
        { }

    std::string str() const             { return _sub.empty() ? _main : _main + '.' + _sub; }
    operator std::string() const        { return str(); }
    const std::string& main() const     { return _main; }
    const std::string& sub() const      { return _sub; }

    bool match(const Topic& other) const
    {
        if (_main != other._main)
            return false;
        return other._sub.empty() || _sub == other._sub;
    }
};

inline bool operator== (const Topic& l, const Topic& r)
{
    return l.main() == r.main()
        && l.sub() == r.sub();
}

std::ostream& operator<< (std::ostream& out, const Topic& topic);

}

#endif
