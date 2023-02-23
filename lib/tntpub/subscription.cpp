#include <tntpub/subscription.h>
#include <cxxtools/regex.h>

namespace tntpub
{

class Subscription::Impl
{
    std::string _data;
public:
    explicit Impl(const std::string& data)
        : _data(data)
        { }

    virtual ~Impl() = default;
    virtual bool match(const std::string& topic) const = 0;

    const std::string& data() const    { return  _data; }
};

class FullTopic : public Subscription::Impl
{
public:
    explicit FullTopic(const std::string& topic)
        : Impl(topic)
        { }
    bool match(const std::string& topic) const override;
};

bool FullTopic::match(const std::string& topic) const
{
    return topic == data();
}

class PraefixTopic : public Subscription::Impl
{
public:
    explicit PraefixTopic(const std::string& praefix)
        : Impl(praefix)
        { }
    bool match(const std::string& topic) const override;
};

bool PraefixTopic::match(const std::string& topic) const
{
    return topic.compare(0, data().size(), data()) == 0;
}

class RegexTopic : public Subscription::Impl
{
    cxxtools::Regex _regex;
public:
    explicit RegexTopic(const std::string& topic)
        : Impl(topic),
          _regex(topic)
        { }
    bool match(const std::string& topic) const override;
};

bool RegexTopic::match(const std::string& topic) const
{
    return _regex.match(topic);
}

Subscription::Subscription(const std::string& topic, Type type)
    : _impl(nullptr)
{
    switch (type)
    {
        case Type::Full: _impl = new FullTopic(topic); break;
        case Type::Praefix: _impl = new PraefixTopic(topic); break;
        case Type::Regex: _impl = new RegexTopic(topic); break;
    }
}

Subscription::~Subscription()
{
    delete _impl;
}

bool Subscription::match(const std::string& topic) const
{
    if (!_impl)
        return true;
    return _impl->match(topic);
}

bool Subscription::equals(const std::string& topic) const
{
    if (_impl)
        return _impl->data() == topic;
    else
        return topic.empty();
}

}
