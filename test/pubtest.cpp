#include <tntpub/server.h>
#include <tntpub/client.h>
#include <tntpub/datamessage.h>

#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <cxxtools/selector.h>

class PubTest : public cxxtools::unit::TestSuite
{
public:
    PubTest()
        : cxxtools::unit::TestSuite("pub")
    {
        registerMethod("sendReceive", *this, &PubTest::sendReceive);
        registerMethod("subtopic", *this, &PubTest::subtopic);
        registerMethod("mainAndSub", *this, &PubTest::mainAndSub);
    }

    void sendReceive();
    void subtopic();
    void mainAndSub();
};

cxxtools::unit::RegisterTest<PubTest> register_PubTest;

void PubTest::sendReceive()
{
    cxxtools::Selector selector;

    tntpub::Server server(selector, "", 9001);
    std::string message1 = "Hi";
    std::string message2 = "There";
    std::vector<std::string> results;

    tntpub::Client sender(&selector, "", 9001);
    tntpub::Client receiver(&selector, "", 9001);
    receiver.subscribe("foo");
    cxxtools::connect(receiver.messageReceived,
            [&results](const tntpub::DataMessage& dm) {
                results.emplace_back(dm.data());
            });

    while (selector.wait(0))
        ;

    sender.sendMessage(tntpub::DataMessage::createPlain("foo", message1));
    sender.sendMessage(tntpub::DataMessage::createPlain("foo", message2));
    sender.sendMessage(tntpub::DataMessage::createPlain("foo2", message2));

    while (selector.wait(0))
        ;

    CXXTOOLS_UNIT_ASSERT_EQUALS(results.size(), 2u);
    CXXTOOLS_UNIT_ASSERT_EQUALS(results[0], message1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(results[1], message2);
}

void PubTest::subtopic()
{
    cxxtools::Selector selector;

    tntpub::Server server(selector, "", 9001);
    std::string message1 = "Hi";
    std::string message2 = "There";
    std::vector<std::string> results;

    tntpub::Client sender(&selector, "", 9001);
    tntpub::Client receiver(&selector, "", 9001);
    receiver.subscribe(tntpub::Topic("foo", "sub"));
    cxxtools::connect(receiver.messageReceived,
            [&results](const tntpub::DataMessage& dm) {
                results.emplace_back(dm.data());
            });

    while (selector.wait(0))
        ;

    sender.sendMessage(tntpub::DataMessage::createPlain(tntpub::Topic("foo", "sub"), message1));
    sender.sendMessage(tntpub::DataMessage::createPlain(tntpub::Topic("foo"), message2));
    sender.sendMessage(tntpub::DataMessage::createPlain(tntpub::Topic("foo", "sub2"), message2));

    while (selector.wait(0))
        ;

    CXXTOOLS_UNIT_ASSERT_EQUALS(results.size(), 1u);
    CXXTOOLS_UNIT_ASSERT_EQUALS(results[0], message1);
}

void PubTest::mainAndSub()
{
    cxxtools::Selector selector;

    tntpub::Server server(selector, "", 9001);
    std::string message1 = "Hi";
    std::string message2 = "There";
    std::vector<std::string> results;
    std::vector<std::string> resultssub;

    tntpub::Client sender(&selector, "", 9001);

    tntpub::Client receiver(&selector, "", 9001);
    receiver.subscribe("foo");
    cxxtools::connect(receiver.messageReceived,
            [&results](const tntpub::DataMessage& dm) {
                results.emplace_back(dm.data());
            });

    tntpub::Client receiversub(&selector, "", 9001);
    receiversub.subscribe(tntpub::Topic("foo", "sub"));

    cxxtools::connect(receiversub.messageReceived,
            [&resultssub](const tntpub::DataMessage& dm) {
                resultssub.emplace_back(dm.data());
            });

    while (selector.wait(0))
        ;

    sender.sendMessage(tntpub::DataMessage::createPlain(tntpub::Topic("foo", "sub"), message1));
    sender.sendMessage(tntpub::DataMessage::createPlain(tntpub::Topic("foo"), message2));
    sender.sendMessage(tntpub::DataMessage::createPlain(tntpub::Topic("foo", "sub2"), message2));

    while (selector.wait(0))
        ;

    CXXTOOLS_UNIT_ASSERT_EQUALS(results.size(), 3u);
    CXXTOOLS_UNIT_ASSERT_EQUALS(results[0], message1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(results[1], message2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(results[2], message2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(resultssub.size(), 1u);
    CXXTOOLS_UNIT_ASSERT_EQUALS(resultssub[0], message1);
}

