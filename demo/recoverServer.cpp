#include <tntpub/server.h>
#include <tntpub/responder.h>
#include <tntpub/subscribemessage.h>
#include <tntpub/datamessage.h>

#include <cxxtools/eventloop.h>
#include <cxxtools/regex.h>
#include <cxxtools/convert.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <vector>
#include <deque>
#include <map>

log_define("recoverServer")

/*

    This implements a tntpub server with recovery feature.

    It works like the tntpub server but when a subscription is received
    with a number extension, this is taken as the position, from where
    to receive all missed messages so far.

    E.g. subscribing to "foo.5" sends messages previously sent to
    topic "foo" starting from the 5th before activating the actual
    subscription.

    The received messages are held in memory. For own applications the
    messages may also be persisted to a filesystem or database.

*/

////////////////////////////////////////////////////////////////////////
// RecoverServer

// A custom server class is needed for receiving the messages and creating
// a custom responder.
class RecoverServer : public tntpub::Server
{
    typedef std::map<std::string, std::vector<tntpub::DataMessage>> Messages;
    Messages _messages;

    void doSendMessage(const tntpub::DataMessage& msg) override;

protected:
    tntpub::Responder* createResponder() override;

public:
    RecoverServer(cxxtools::SelectorBase& selector, const std::string& ip, unsigned short port)
        : tntpub::Server(selector, ip, port)
        { }

    const Messages& messages() const { return _messages; }
};

////////////////////////////////////////////////////////////////////////
// RecoverResponder

// A responder is responsible for a single client. Here it stores the position
// from where to send the next message per topic.

class RecoverResponder : public tntpub::TcpResponder
{
    RecoverServer& _server;

    struct RecoverChannel
    {
        std::string _topic;
        unsigned _pos;

        explicit RecoverChannel(const std::string& topic, unsigned pos)
            : _topic(topic), _pos(pos)
            { }
    };

    std::deque<RecoverChannel> _recoverPositions;

    void onOutputBufferEmpty(tntpub::Responder&);

protected:
    void subscribeMessageReceived(const tntpub::SubscribeMessage& subscribeMessage) override;

public:
    explicit RecoverResponder(RecoverServer& server);
};

////////////////////////////////////////////////////////////////////////
// Impl Server

tntpub::Responder* RecoverServer::createResponder()
{
    // instantiate a custom responder
    return new RecoverResponder(*this);
}

void RecoverServer::doSendMessage(const tntpub::DataMessage& msg)
{
    Server::doSendMessage(msg);
    _messages[msg.topic()].emplace_back(msg);
}

////////////////////////////////////////////////////////////////////////
// Impl Responder

RecoverResponder::RecoverResponder(RecoverServer& server)
    : tntpub::TcpResponder(server),
      _server(server)
{
    // when the output buffer gets empty, we are notified and we can
    // send the next messages.
    cxxtools::connect(outputBufferEmpty, *this, &RecoverResponder::onOutputBufferEmpty);
}

void RecoverResponder::subscribeMessageReceived(const tntpub::SubscribeMessage& subscribeMessage)
{
    log_info("subscribe message received for topic " << subscribeMessage.topic());

    auto topic = subscribeMessage.topic();
    static cxxtools::Regex re("^(.+)\\.([0-9]+)$");
    cxxtools::RegexSMatch sm;

    // check, if the subscription has a starting position in the topic, e.g. foo.5
    if (re.match(topic, sm))
    {
        topic = sm[1];
        auto pos = cxxtools::convert<unsigned>(sm[2]);

        log_debug("topic=" << topic << " recovery position=" << pos);

        auto it = _server.messages().find(topic);

        if (it == _server.messages().end())
        {
            // when there are no messages sent to that topic, we can bypass recovery
            subscribe(subscribeMessage);
        }
        else
        {
            // Store the position, from where to recover in our list and send
            // the next messages.
            // Note that the output buffer need not be really empty, but we just
            // claim, that it is. The streambuffer is automatically extended
            // when needed.
            _recoverPositions.emplace_back(topic, pos);
            onOutputBufferEmpty(*this);
        }
    }
    else
    {
        log_debug("no match");
        subscribe(subscribeMessage);
    }
}

void RecoverResponder::onOutputBufferEmpty(tntpub::Responder&)
{
    // we send the next 10 messages to the buffer

    unsigned count = 0;
    while (!_recoverPositions.empty() && count < 10)
    {
        auto& r = _recoverPositions[0];
        auto it = _server.messages().find(r._topic);

        if (it == _server.messages().end() || r._pos >= it->second.size())
        {
            // when that topic is recovered, start the actual subscription
            // and remove the recovery item
            subscribe(r._topic);
            _recoverPositions.pop_front();
        }
        else
        {
            // send and increment the position
            sendMessage(it->second[r._pos++]);
            ++count;
        }
    }
}

////////////////////////////////////////////////////////////////////////
// main
int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 9001);

        cxxtools::EventLoop eventLoop;
        RecoverServer pubSubServer(eventLoop, ip, port);

        eventLoop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
