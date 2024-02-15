#ifndef TNTPUB_MESSAGESOURCE_H
#define TNTPUB_MESSAGESOURCE_H

#include <tntpub/datamessage.h>

namespace tntpub
{
/// Interface class for objects, which can send data messages
class MessageSource
{
    virtual void doSendMessage(const DataMessage& msg) = 0;

public:
    virtual ~MessageSource() = default;

    MessageSource& sendMessage(const DataMessage& msg)
    {
        doSendMessage(msg);
        return *this;
    }

    template <typename Obj> MessageSource& sendMessage(const std::string& topic, const Obj& obj)
    {
        auto msg = DataMessage::create(topic, obj);
        doSendMessage(msg);
        return *this;
    }
};

}

#endif
