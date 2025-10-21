#include <tntpub/server.h>

#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/selector.h>
#include <cxxtools/log.h>

log_define("tntpub.test.InvalidMessageTest")

class InvalidMessageTest : public cxxtools::unit::TestSuite
{
public:
    InvalidMessageTest()
        : cxxtools::unit::TestSuite("invalidMessage")
    {
        registerMethod("sendInvalidMessage", *this, &InvalidMessageTest::sendInvalidMessage);
    }

    void sendInvalidMessage();
};

cxxtools::unit::RegisterTest<InvalidMessageTest> register_InvalidMessageTest;

void InvalidMessageTest::sendInvalidMessage()
{
    cxxtools::Selector selector;

    tntpub::Server server(selector, "", 9001);
    std::string message1 = "Hi";
    std::string message2 = "There";
    std::vector<std::string> results;

    cxxtools::net::TcpSocket client;
    client.setSelector(&selector);
    client.beginConnect("", 9001);

    cxxtools::connect(client.connected, [](cxxtools::net::TcpSocket& client) {
            client.endConnect();
            const std::string message(200, 'A');
            client.beginWrite(message.data(), message.size());
    });

    char buffer[200];
    bool replyReceived = false;
    bool connectionClosed = false;

    cxxtools::connect(client.outputReady, [&buffer](cxxtools::IODevice& client) {
            log_debug("outputReady");
            client.endWrite();
            client.beginRead(buffer, sizeof(buffer));
    });

    cxxtools::connect(client.inputReady, [&replyReceived](cxxtools::IODevice& client) {
            log_debug("inputReady");
            auto n = client.endRead();
            CXXTOOLS_UNIT_ASSERT_EQUALS(n, 0);
            replyReceived = true;
    });

    while (selector.wait(0))
        ;

    CXXTOOLS_UNIT_ASSERT(replyReceived);
}

