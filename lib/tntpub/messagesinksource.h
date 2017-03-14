#ifndef TNTPUB_MESSAGESINKSOURCE_H
#define TNTPUB_MESSAGESINKSOURCE_H

#include <tntpub/messagesink.h>
#include <tntpub/messagesource.h>

namespace tntpub
{
/// Interface class which combines message sink and message source interfaces.
class MessageSinkSource : public MessageSink, public MessageSource
{
};
}

#endif
