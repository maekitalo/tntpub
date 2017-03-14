#ifndef TNTPUB_MESSAGESOURCE_H
#define TNTPUB_MESSAGESOURCE_H

#include <tntpub/datamessage.h>
#include <cxxtools/signal.h>

namespace tntpub
{
/// Interface class for objects, which receive data messages
class MessageSource
{
public:
    cxxtools::Signal<const DataMessage&> messageReceived;
};
}

#endif
