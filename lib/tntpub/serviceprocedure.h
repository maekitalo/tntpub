#ifndef TNTPUB_SERVICEPROCEDURE_H
#define TNTPUB_SERVICEPROCEDURE_H

#include <cxxtools/serializationerror.h>
#include <cxxtools/typetraits.h>
#include <tntpub/datamessage.h>

namespace tntpub
{

class ServiceProcedure
{
public:
    virtual ~ServiceProcedure() { }
    virtual void invoke(const DataMessage&) = 0;
};

template <typename A>
class ServiceFunction : public ServiceProcedure
{
    void (*_fn)(A);

public:
    explicit ServiceFunction(void (*fn)(A))
        : _fn(fn)
        { }

    virtual void invoke(const DataMessage& dm)
    {
        typename cxxtools::TypeTraits<A>::Value arg;
        try
        {
            dm.get(arg);
        }
        catch (const cxxtools::SerializationError&)
        {
            return;
        }

        _fn(arg);
    }
};

template <typename A, typename C>
class ServiceMethod : public ServiceProcedure
{
    typedef void (C::*MemFuncT)(A);
    C& _object;
    MemFuncT _memFunc;

public:
    explicit ServiceMethod(C& object, MemFuncT memFunc)
        : _object(object),
          _memFunc(memFunc)
        { }

    virtual void invoke(const DataMessage& dm)
    {
        typename cxxtools::TypeTraits<A>::Value arg;
        try
        {
            dm.get(arg);
        }
        catch (const cxxtools::SerializationError&)
        {
            return;
        }

        (_object.*_memFunc)(arg);
    }
};

}

#endif
