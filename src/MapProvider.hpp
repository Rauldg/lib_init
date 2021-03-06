#pragma once

#include "Base.hpp"
#include <orocos_cpp_base/ProxyPort.hpp>
#include <envire/core/EventTypes.hpp>
#include <rtt/extras/ReadOnlyPointer.hpp>

FW_DECLARE_TYPE(RTT::extras::ReadOnlyPointer< envire::BinaryEvents >)

namespace init
{

class MapProvider : public virtual Base
{
public:
    MapProvider(const std::string& name);
    virtual ~MapProvider();
    
    virtual OutputProxyPort< RTT::extras::ReadOnlyPointer< envire::BinaryEvents > >& getMapPort() = 0;    
};

}
