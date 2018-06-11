/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include "datamessage.h"
#include <cxxtools/bin/bin.h>
#include <sstream>

namespace tntpub
{

const cxxtools::SerializationInfo& DataMessage::data() const
{
    if (_data.isNull() && !_rawdata.empty())
    {
        std::istringstream in(_rawdata);
        in >> cxxtools::bin::Bin(_data);
    }

    return _data;
}

void operator<<= (cxxtools::SerializationInfo& si, const DataMessage& dm)
{
    si.setTypeName("DataMessage");
    si.addMember("topic") <<= dm._topic;
    if (dm._rawdata.empty())
    {
        std::ostringstream s;
        s << cxxtools::bin::Bin(dm._data);
        si.addMember("rawdata") <<= s.str();
    }
    else
    {
        si.addMember("rawdata") <<= dm._rawdata;
    }
}

void operator>>= (const cxxtools::SerializationInfo& si, DataMessage& dm)
{
    si.getMember("topic") >>= dm._topic;
    const cxxtools::SerializationInfo* pi = si.findMember("rawdata");
    if (pi)
    {
        *pi >>= dm._rawdata;
        dm._data = cxxtools::SerializationInfo();
    }
    else
    {
        si.getMember("data") >>= dm._data;
    }
}

void operator<<= (cxxtools::SerializationInfo& si, const DataMessageView& dv)
{
    si.addMember("topic") <<= dv._dm.topic();
    si.addMember("data") <<= dv._dm.data();
    si.setTypeName("DataMessage");
}

}
