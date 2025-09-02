#include <tntpub/subscription.h>
#include <tntpub/topic.h>
#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>

class SubscriptionTest : public cxxtools::unit::TestSuite
{
public:
    SubscriptionTest()
        : cxxtools::unit::TestSuite("subscription")
    {
        registerMethod("full", *this, &SubscriptionTest::full);
        registerMethod("prefix", *this, &SubscriptionTest::prefix);
        registerMethod("regex", *this, &SubscriptionTest::regex);
        registerMethod("equals", *this, &SubscriptionTest::equals);
        registerMethod("subtopic", *this, &SubscriptionTest::subtopic);
    }

    void full();
    void prefix();
    void regex();
    void equals();
    void subtopic();
};

cxxtools::unit::RegisterTest<SubscriptionTest> register_SubscriptionTest;

void SubscriptionTest::full()
{
    tntpub::Subscription subscription(tntpub::Topic("foo"));

    CXXTOOLS_UNIT_ASSERT(subscription.match(tntpub::Topic("foo")));
    CXXTOOLS_UNIT_ASSERT(!subscription.match(tntpub::Topic("foo1")));
    CXXTOOLS_UNIT_ASSERT(!subscription.match(tntpub::Topic("1foo")));
}

void SubscriptionTest::prefix()
{
    tntpub::Subscription subscription(tntpub::Topic("foo"), tntpub::Subscription::Type::Prefix);

    CXXTOOLS_UNIT_ASSERT(subscription.match(tntpub::Topic("foo")));
    CXXTOOLS_UNIT_ASSERT(subscription.match(tntpub::Topic("foo1")));
    CXXTOOLS_UNIT_ASSERT(!subscription.match(tntpub::Topic("1foo")));
}

void SubscriptionTest::regex()
{
    tntpub::Subscription subscription(tntpub::Topic("o[12]"), tntpub::Subscription::Type::Regex);

    CXXTOOLS_UNIT_ASSERT(!subscription.match(tntpub::Topic("foo")));
    CXXTOOLS_UNIT_ASSERT(subscription.match(tntpub::Topic("foo1")));
    CXXTOOLS_UNIT_ASSERT(subscription.match(tntpub::Topic("foo2bar")));
    CXXTOOLS_UNIT_ASSERT(!subscription.match(tntpub::Topic("1foo")));
}

void SubscriptionTest::equals()
{
    tntpub::Subscription full(tntpub::Topic("foo"), tntpub::Subscription::Type::Full);
    tntpub::Subscription prefix(tntpub::Topic("foo"), tntpub::Subscription::Type::Prefix);
    tntpub::Subscription regex(tntpub::Topic("foo"), tntpub::Subscription::Type::Regex);

    CXXTOOLS_UNIT_ASSERT(full.equals("foo", tntpub::Subscription::Type::Full));
    CXXTOOLS_UNIT_ASSERT(!full.equals("bar", tntpub::Subscription::Type::Full));
    CXXTOOLS_UNIT_ASSERT(!full.equals("foo", tntpub::Subscription::Type::Prefix));
    CXXTOOLS_UNIT_ASSERT(!full.equals("foo", tntpub::Subscription::Type::Regex));

    CXXTOOLS_UNIT_ASSERT(!prefix.equals("foo", tntpub::Subscription::Type::Full));
    CXXTOOLS_UNIT_ASSERT(prefix.equals("foo", tntpub::Subscription::Type::Prefix));
    CXXTOOLS_UNIT_ASSERT(!prefix.equals("bar", tntpub::Subscription::Type::Prefix));
    CXXTOOLS_UNIT_ASSERT(!prefix.equals("foo", tntpub::Subscription::Type::Regex));

    CXXTOOLS_UNIT_ASSERT(!regex.equals("foo", tntpub::Subscription::Type::Full));
    CXXTOOLS_UNIT_ASSERT(!regex.equals("foo", tntpub::Subscription::Type::Prefix));
    CXXTOOLS_UNIT_ASSERT(regex.equals("foo", tntpub::Subscription::Type::Regex));
    CXXTOOLS_UNIT_ASSERT(!regex.equals("bar", tntpub::Subscription::Type::Regex));

}

void SubscriptionTest::subtopic()
{
    tntpub::Subscription foo("foo", "sub1");
    tntpub::Subscription allfoo(tntpub::Topic("foo"));

    CXXTOOLS_UNIT_ASSERT(!foo.match(tntpub::Topic("foo")));
    CXXTOOLS_UNIT_ASSERT(foo.match(tntpub::Topic("foo", "sub1")));
    CXXTOOLS_UNIT_ASSERT(!foo.match(tntpub::Topic("foo1", "sub1")));
    CXXTOOLS_UNIT_ASSERT(!foo.match(tntpub::Topic("foo", "sub2")));

    CXXTOOLS_UNIT_ASSERT(allfoo.match(tntpub::Topic("foo")));
    CXXTOOLS_UNIT_ASSERT(allfoo.match(tntpub::Topic("foo", "sub1")));
    CXXTOOLS_UNIT_ASSERT(!allfoo.match(tntpub::Topic("foo1", "sub1")));
    CXXTOOLS_UNIT_ASSERT(allfoo.match(tntpub::Topic("foo", "sub2")));
}
