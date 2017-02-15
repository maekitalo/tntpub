#ifndef TNTPUB_MESSAGESINK_H
#define TNTPUB_MESSAGESINK_H

#include <tntpub/datamessage.h>

namespace tntpub
{
class MessageSink
{
    virtual void doSendMessage(const DataMessage& msg) = 0;

public:
    MessageSink& sendMessage(const DataMessage& msg)
    {
        doSendMessage(msg);
        return *this;
    }

    template <typename Obj> MessageSink& sendMessage(const std::string& topic, const Obj& obj)
    {
        DataMessage msg(topic, obj);
        doSendMessage(msg);
        return *this;
    }
};

}

#endif
