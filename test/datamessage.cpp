#include <tntpub/datamessage.h>
#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>

class DataMessageTest : public cxxtools::unit::TestSuite
{
public:
    DataMessageTest()
        : cxxtools::unit::TestSuite("datamessage")
    {
        registerMethod("create", *this, &DataMessageTest::create);
        registerMethod("createRaw", *this, &DataMessageTest::createRaw);
        registerMethod("subscribeMessage", *this, &DataMessageTest::subscribeMessage);
        registerMethod("unsubscribeMessage", *this, &DataMessageTest::unsubscribeMessage);
        registerMethod("serialize", *this, &DataMessageTest::serialize);
    }

    void create();
    void createRaw();
    void subscribeMessage();
    void unsubscribeMessage();
    void serialize();
};

cxxtools::unit::RegisterTest<DataMessageTest> register_DataMessageTest;

void DataMessageTest::create()
{
    std::vector<int> v{ 3, 4, 6 };
    auto dm = tntpub::DataMessage::create("foo", v);

    std::vector<int> result;
    dm.get(result);

    CXXTOOLS_UNIT_ASSERT(v == result);
}

void DataMessageTest::createRaw()
{
    std::string data("ABC123");
    auto dm = tntpub::DataMessage::createRaw("foo", data);
    CXXTOOLS_UNIT_ASSERT_EQUALS(dm.data(), data);
}

void DataMessageTest::subscribeMessage()
{
    auto dm = tntpub::DataMessage::subscribe("foo");
    CXXTOOLS_UNIT_ASSERT_EQUALS(dm.topic(), "foo");
    CXXTOOLS_UNIT_ASSERT(dm.isSubscribeMessage());
    CXXTOOLS_UNIT_ASSERT(!dm.isUnsubscribeMessage());
    CXXTOOLS_UNIT_ASSERT(!dm.isDataMessage());
}

void DataMessageTest::unsubscribeMessage()
{
    auto dm = tntpub::DataMessage::unsubscribe("foo");
    CXXTOOLS_UNIT_ASSERT_EQUALS(dm.topic(), "foo");
    CXXTOOLS_UNIT_ASSERT(dm.isUnsubscribeMessage());
    CXXTOOLS_UNIT_ASSERT(!dm.isSubscribeMessage());
    CXXTOOLS_UNIT_ASSERT(!dm.isDataMessage());
}

void DataMessageTest::serialize()
{
    std::vector<int> v{ 3, 4, 6 };
    auto dm = tntpub::DataMessage::create("foo", v);

    std::vector<char> buffer;
    dm.appendTo(buffer);
    CXXTOOLS_UNIT_ASSERT(buffer.size() > sizeof(tntpub::DataMessage::Header));

    tntpub::DataMessageDeserializer deserializer;
    deserializer.addData(buffer.data(), buffer.size());

    std::vector<int> result;
    auto messageProcessed = deserializer.processMessage([&result](tntpub::DataMessage& dm) {
        CXXTOOLS_UNIT_ASSERT_EQUALS(dm.topic(), "foo");
        dm.get(result);
    });

    CXXTOOLS_UNIT_ASSERT(messageProcessed);
    CXXTOOLS_UNIT_ASSERT_EQUALS(deserializer.in_avail(), 0);
    CXXTOOLS_UNIT_ASSERT(v == result);
}
