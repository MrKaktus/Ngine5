/*
 
 Ngine v5.0
 
 Module      : Common XR Interface.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#include "core/defines.h"
#include "core/log/log.h"
#include "core/xr/interface.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "core/xr/openxr/oxrInterface.h"
#endif

namespace en
{
namespace xr
{

// This static function should be in .mm file if we include Metal headers !!!
bool Interface::create(void)
{
    if (XR)
    {
        return true;
    }

    Log << "Starting module: XR.\n";

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
    XR = std::make_shared<cxrInterface>("Ngine5.0");
#endif
#if defined(EN_PLATFORM_WINDOWS)
    if (oxrInterface::isPresent())
    {
        Log << "OpenXR: Present.\n";
        XR = std::make_shared<oxrInterface>("Ngine5.0");
    }
    else
    {
        Log << "OpenXR: Runtime is missing.\n";
    }
#endif

    return (XR == nullptr) ? false : true;
}

} // en::xr

std::shared_ptr<xr::Interface> XR;

} // en