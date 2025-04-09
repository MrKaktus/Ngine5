


#include "core/xr/openvr/ovrInterface.h"

#include "utilities/utilities.h"

namespace en
{
namespace xr
{

bool ovrInterface::isPresent(void)
{
    return false;
}

ovrInterface::ovrInterface(std::string appName)
{
    // TODO: Refactor vive.cpp and move OpenVR code here
}

ovrInterface::~ovrInterface()
{
    // TODO: Refactor vive.cpp and move OpenVR code here
}

uint32 ovrInterface::headsets(void)
{
    // TODO: Refactor vive.cpp and move OpenVR code here
    return 0;
}

Headset* ovrInterface::headset(const uint32 index)
{
    // TODO: Refactor vive.cpp and move OpenVR code here
    return nullptr;
}

} // en::xr
} // en