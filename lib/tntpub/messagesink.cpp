#include <tntpub/messagesink.h>

namespace tntpub
{
void MessageSink::dispatchMessage(const DataMessage& msg)
{
    messageReceived(msg);

    if (msg.type() != DataMessage::Type::Data)
        return;

    for (unsigned n = 0; n < _callbacks.size(); ++n)
    {
        if (_callbacks[n].topic.match(msg.topic()))
            _callbacks[n].proc->invoke(msg);
    }
}
}
