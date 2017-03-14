#ifndef TNTPUB_MESSAGESOURCE_H
#define TNTPUB_MESSAGESOURCE_H

#include <tntpub/datamessage.h>
#include <tntpub/serviceprocedure.h>

#include <cxxtools/signal.h>

#include <vector>

namespace tntpub
{
/// Interface class for objects, which receive data messages
class MessageSource
{
    struct Slot
    {
        std::string topic;
        ServiceProcedure* proc;
        Slot() : proc(0) { }
        Slot(const std::string& topic_, ServiceProcedure* proc_)
            : topic(topic_),
              proc(proc_)
              { }
        explicit Slot(ServiceProcedure* proc_)
            : proc(proc_)
              { }
    };

    std::vector<Slot> _callbacks;

protected:
    void dispatchMessage(const DataMessage& msg);

public:
    ~MessageSource();

    template <typename A> void registerFunction(const std::string& topic, void (*fn)(A))
    {
        _callbacks.push_back(Slot(topic, new ServiceFunction<A>(fn)));
    }

    template <typename A, class C> void registerMethod(const std::string& topic, C& obj, void (C::*method)(A))
    {
        _callbacks.push_back(Slot(topic, new ServiceMethod<A, C>(obj, method)));
    }

    template <typename A> void registerFunction(void (*fn)(A))
    {
        _callbacks.push_back(Slot(new ServiceFunction<A>(fn)));
    }

    template <typename A, class C> void registerMethod(C& obj, void (C::*method)(A))
    {
        _callbacks.push_back(Slot(new ServiceMethod<A, C>(obj, method)));
    }

    cxxtools::Signal<const DataMessage&> messageReceived;
};
}

#endif
