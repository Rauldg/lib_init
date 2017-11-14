#pragma once

#include <base/samples/Joints.hpp>
#include <base/commands/Joints.hpp>
#include <orocos_cpp_base/ProxyPort.hpp>
#include "Base.hpp"

FW_DECLARE_TYPE(base::samples::Joints)
FW_DECLARE_TYPE(base::commands::Joints)

namespace init
{

class JointDriver : public Base
{
public:
    JointDriver(const std::string& name) : Base(name) {};
    virtual ~JointDriver() {};
    virtual OutputProxyPort<base::samples::Joints> &getStatusPort() = 0;
    virtual InputProxyPort<base::commands::Joints> &getCommandPort() = 0;

};

}