#ifndef TNTPUB_SUBSCRIPTION_H
#define TNTPUB_SUBSCRIPTION_H

#include <string>

namespace tntpub
{
class Topic;

class Subscription
{
public:
    class Impl;

    enum class Type: char
    {
        Null = ' ',
        Full = 'F',
        Prefix = 'P',
        Regex = 'X'
    };

private:
    Impl* _impl;

public:
    Subscription()
        : _impl(0)
        { }
    Subscription(Subscription&& src)
        : _impl(src._impl)
        { src._impl = nullptr; }

    void operator=(Subscription&& src)
    {
        _impl = src._impl;
        src._impl = nullptr;
    }

    Subscription(const Topic& topic, Type type = Type::Full);
    Subscription(const std::string& topic, Type type = Type::Full);
    Subscription(const std::string& topic, const std::string& subtopic, Type type = Type::Full);
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    ~Subscription();

    bool match(const Topic& topic) const;
    bool equals(const Topic& topic, Type type) const;
};

}

#endif
