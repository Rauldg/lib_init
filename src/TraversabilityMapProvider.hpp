#pragma once

#include "Base.hpp"
#include <orocos_cpp_base/ProxyPort.hpp>
#include <envire/core/EventTypes.hpp>
#include <rtt/extras/ReadOnlyPointer.hpp>

FW_DECLARE_TYPE(RTT::extras::ReadOnlyPointer< envire::BinaryEvents >)

namespace init {

class TraversabilityMapProvider : public Base
{
public:
    TraversabilityMapProvider(const std::string& name) : Base(name) {};

    virtual OutputProxyPort< RTT::extras::ReadOnlyPointer< envire::BinaryEvents > >& getTraversabilityMapPort() = 0;
};

    
}
