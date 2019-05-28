#include <tntpub/messagesink.h>

namespace tntpub
{
MessageSink::~MessageSink()
{
    for (unsigned n = 0; n < _callbacks.size(); ++n)
        delete _callbacks[n].proc;
}

void MessageSink::dispatchMessage(const DataMessage& msg)
{
    messageReceived(msg);
    for (unsigned n = 0; n < _callbacks.size(); ++n)
    {
        if (_callbacks[n].topic.empty()
         || _callbacks[n].topic == msg.topic())
            _callbacks[n].proc->invoke(msg);
    }
}
}
