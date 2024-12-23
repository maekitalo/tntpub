#ifndef TNTPUB_MESSAGESOURCESOURCE_H
#define TNTPUB_MESSAGESOURCESOURCE_H

#include <tntpub/messagesource.h>
#include <tntpub/messagesink.h>

namespace tntpub
{
/// Interface class which combines message sink and message source interfaces.
class MessageSinkSource : public MessageSource, public MessageSink
{
};
}

#endif
