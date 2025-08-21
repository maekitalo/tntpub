#ifndef TNTPUB_MESSAGESINK_H
#define TNTPUB_MESSAGESINK_H

#include <tntpub/datamessage.h>
#include <tntpub/serviceprocedure.h>
#include <tntpub/subscription.h>

#include <cxxtools/signal.h>

#include <vector>
#include <memory>

namespace tntpub
{
/// Interface class for objects, which receive data messages
class MessageSink
{
    struct Slot
    {
        Subscription topic;
        std::unique_ptr<ServiceProcedure> proc;
        Slot(Subscription&& topic_, ServiceProcedure* proc_)
            : topic(std::move(topic_)),
              proc(std::unique_ptr<ServiceProcedure>(proc_))
              { }
        explicit Slot(ServiceProcedure* proc_)
            : proc(std::unique_ptr<ServiceProcedure>(proc_))
              { }
    };

    std::vector<Slot> _callbacks;

protected:
    void dispatchMessage(const DataMessage& msg);

public:
    template <typename A> void registerFunction(Subscription&& topic, void (*fn)(A))
    {
        _callbacks.emplace_back(std::move(topic), new ServiceFunction<A>(fn));
    }

    template <typename A> void registerFunction(const char* topic, void (*fn)(A))
    {
        _callbacks.emplace_back(Topic(topic), new ServiceFunction<A>(fn));
    }

    template <typename A, class C> void registerMethod(Subscription&& topic, C& obj, void (C::*method)(A))
    {
        _callbacks.emplace_back(std::move(topic), new ServiceMethod<A, C>(obj, method));
    }

    template <typename A, class C> void registerMethod(const char* topic, C& obj, void (C::*method)(A))
    {
        _callbacks.emplace_back(Topic(topic), new ServiceMethod<A, C>(obj, method));
    }

    template <typename A> void registerFunction(void (*fn)(A))
    {
        _callbacks.emplace_back(new ServiceFunction<A>(fn));
    }

    template <typename A, class C> void registerMethod(C& obj, void (C::*method)(A))
    {
        _callbacks.emplace_back(new ServiceMethod<A, C>(obj, method));
    }

    cxxtools::Signal<const DataMessage&> messageReceived;
};
}

#endif
