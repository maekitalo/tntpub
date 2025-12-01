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
    return topic.main() == data().main()
        && (data().sub().empty() || data().sub() == topic.sub());
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
    return topic.main().compare(0, data().main().size(), data().main()) == 0
        && (data().sub().empty() || data().sub() == topic.sub());
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
          _regex(topic.sub())
        { }
    bool match(const Topic& topic) const override;
    bool is(Subscription::Type type) const override;
};

bool RegexTopic::match(const Topic& topic) const
{
    return data().main() == topic.main()
        && _regex.match(topic.sub());
}

bool RegexTopic::is(Subscription::Type type) const
{
    return type == Subscription::Type::Regex;
}

class RegexTopicReversed : public Subscription::Impl
{
    cxxtools::Regex _regex;
public:
    explicit RegexTopicReversed(const Topic& topic)
        : Impl(topic),
          _regex(topic.sub())
        { }
    bool match(const Topic& topic) const override;
    bool is(Subscription::Type type) const override;
};

bool RegexTopicReversed::match(const Topic& topic) const
{
    return data().main() == topic.main()
        && !_regex.match(topic.sub());
}

bool RegexTopicReversed::is(Subscription::Type type) const
{
    return type == Subscription::Type::RegexReversed;
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
        case Type::RegexReversed: _impl = new RegexTopicReversed(topic); break;
    }
}

Subscription::Subscription(const std::string& topic, Type type)
    : Subscription(Topic(topic), type)
{ }

Subscription::Subscription(const std::string& topic, const std::string& sub, Type type)
    : Subscription(Topic(topic, sub), type)
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
        return _impl->is(type) && _impl->data().main() == topic.main();
    else
        return topic.main().empty();
}

}
