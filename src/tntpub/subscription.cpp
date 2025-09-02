#include <tntpub/subscription.h>
#include <tntpub/topic.h>
#include <cxxtools/regex.h>

namespace tntpub
{

class Subscription::Impl
{
    Topic _data;
public:
    explicit Impl(const Topic& data)
        : _data(data)
        { }

    virtual ~Impl() = default;
    virtual bool match(const Topic& topic) const = 0;
    virtual bool is(Subscription::Type type) const = 0;

    const Topic& data() const    { return  _data; }
};

namespace
{

class FullTopic : public Subscription::Impl
{
public:
    explicit FullTopic(const Topic& topic)
        : Impl(topic)
        { }
    bool match(const Topic& topic) const override;
    bool is(Subscription::Type type) const;
};

bool FullTopic::match(const Topic& topic) const
{
    return topic.topic() == data().topic()
        && (data().subtopic().empty() || data().subtopic() == topic.subtopic());
}

bool FullTopic::is(Subscription::Type type) const
{
    return type == Subscription::Type::Full;
}

class PrefixTopic : public Subscription::Impl
{
public:
    explicit PrefixTopic(const Topic& prefix)
        : Impl(prefix)
        { }
    bool match(const Topic& topic) const override;
    bool is(Subscription::Type type) const override;
};

bool PrefixTopic::match(const Topic& topic) const
{
    return topic.topic().compare(0, data().topic().size(), data().topic()) == 0
        && (data().subtopic().empty() || data().subtopic() == topic.subtopic());
}

bool PrefixTopic::is(Subscription::Type type) const
{
    return type == Subscription::Type::Prefix;
}

class RegexTopic : public Subscription::Impl
{
    cxxtools::Regex _regex;
public:
    explicit RegexTopic(const Topic& topic)
        : Impl(topic),
          _regex(topic.topic())
        { }
    bool match(const Topic& topic) const override;
    bool is(Subscription::Type type) const override;
};

bool RegexTopic::match(const Topic& topic) const
{
    return _regex.match(topic.topic())
        && (data().subtopic().empty() || data().subtopic() == topic.subtopic());
}

bool RegexTopic::is(Subscription::Type type) const
{
    return type == Subscription::Type::Regex;
}

}

Subscription::Subscription(const Topic& topic, Type type)
    : _impl(nullptr)
{
    switch (type)
    {
        case Type::Null:
        case Type::Full: _impl = new FullTopic(topic); break;
        case Type::Prefix: _impl = new PrefixTopic(topic); break;
        case Type::Regex: _impl = new RegexTopic(topic); break;
    }
}

Subscription::Subscription(const std::string& topic, Type type)
    : Subscription(Topic(topic), type)
{ }

Subscription::Subscription(const std::string& topic, const std::string& subtopic, Type type)
    : Subscription(Topic(topic, subtopic), type)
{ }

Subscription::~Subscription()
{
    delete _impl;
}

bool Subscription::match(const Topic& topic) const
{
    if (!_impl)
        return true;
    return _impl->match(topic);
}

bool Subscription::equals(const Topic& topic, Subscription::Type type) const
{
    if (_impl)
        return _impl->is(type) && _impl->data().topic() == topic.topic();
    else
        return topic.topic().empty();
}

}
