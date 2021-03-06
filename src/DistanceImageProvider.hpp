#pragma once

#include <orocos_cpp_base/ProxyPort.hpp>
#include <base/samples/DistanceImage.hpp>
#include <lib_init/Base.hpp>

FW_DECLARE_TYPE(base::samples::DistanceImage)

namespace init
{

class DistanceImageProvider : public Base
{
public:
    DistanceImageProvider(const std::string& name): Base(name) {};
    virtual ~DistanceImageProvider() {};
    virtual OutputProxyPort<base::samples::DistanceImage> &getDistanceImagePort() = 0;
};
}
