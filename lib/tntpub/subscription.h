#ifndef TNTPUB_SUBSCRIPTION_H
#define TNTPUB_SUBSCRIPTION_H

#include <string>

namespace tntpub
{
class Subscription
{
public:
    class Impl;

    enum class Type: char
    {
        Null = ' ',
        Full = 'F',
        Praefix = 'P',
        Regex = 'R'
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

    Subscription(const std::string& topic, Type type = Type::Full);
    Subscription(const char* topic, Type type = Type::Full)
        : Subscription(std::string(topic), type)
        { }

    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    ~Subscription();

    bool match(const std::string& topic) const;
    bool equals(const std::string& topic) const;
};

}

#endif
