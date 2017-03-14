#include <tntpub/messagesource.h>

namespace tntpub
{
MessageSource::~MessageSource()
{
    for (unsigned n = 0; n < _callbacks.size(); ++n)
        delete _callbacks[n].proc;
}

void MessageSource::dispatchMessage(const DataMessage& msg)
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
