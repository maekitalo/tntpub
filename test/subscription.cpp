#include <tntpub/subscription.h>
#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>

class SubscriptionTest : public cxxtools::unit::TestSuite
{
public:
    SubscriptionTest()
        : cxxtools::unit::TestSuite("subscription")
    {
        registerMethod("full", *this, &SubscriptionTest::full);
        registerMethod("praefix", *this, &SubscriptionTest::praefix);
        registerMethod("regex", *this, &SubscriptionTest::regex);
    }

    void full();
    void praefix();
    void regex();
};

cxxtools::unit::RegisterTest<SubscriptionTest> register_SubscriptionTest;

void SubscriptionTest::full()
{
    tntpub::Subscription subscription("foo");

    CXXTOOLS_UNIT_ASSERT(subscription.match("foo"));
    CXXTOOLS_UNIT_ASSERT(!subscription.match("foo1"));
    CXXTOOLS_UNIT_ASSERT(!subscription.match("1foo"));
}

void SubscriptionTest::praefix()
{
    tntpub::Subscription subscription("foo", tntpub::Subscription::Type::Praefix);

    CXXTOOLS_UNIT_ASSERT(subscription.match("foo"));
    CXXTOOLS_UNIT_ASSERT(subscription.match("foo1"));
    CXXTOOLS_UNIT_ASSERT(!subscription.match("1foo"));
}

void SubscriptionTest::regex()
{
    tntpub::Subscription subscription("o[12]", tntpub::Subscription::Type::Regex);

    CXXTOOLS_UNIT_ASSERT(!subscription.match("foo"));
    CXXTOOLS_UNIT_ASSERT(subscription.match("foo1"));
    CXXTOOLS_UNIT_ASSERT(subscription.match("foo2bar"));
    CXXTOOLS_UNIT_ASSERT(!subscription.match("1foo"));
}

