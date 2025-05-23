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

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#include "core/xr/openvr/ovrInterface.h"
#endif

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

    enLog << "Starting module: XR.\n";

    // TODO: Application should receive Hint if it was launched from one of the 
    //       vendor stores, tying it to that vendor headset. For e.g. if it was
    //       launched from Oculus Store, it should initialize Oculus Runtime,
    //       if from Steam, SteamVR, otherwise it should let application select
    //       runtime it wants to use.

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
    XR = std::make_unique<cxrInterface>("Ngine5.0");
#endif
#if defined(EN_PLATFORM_WINDOWS)

    // OpenVR
    if (ovrInterface::isPresent())
    {
        enLog << "OpenVR: Present.";
        if (!XR)
        {
            XR = std::make_unique<ovrInterface>("Ngine5.0");
            if (XR)
            {
                enLog << " Initialized.\n";
            }
            else
            {
                enLog << " Failed to init.\n";
            }
        }
    }
    else
    {
        enLog << "OpenVR: Runtime is missing.\n";
    }

    // OpenXR
    if (oxrInterface::isPresent())
    {
        enLog << "OpenXR: Present.\n";
        if (!XR)
        {
            XR = std::make_unique<oxrInterface>("Ngine5.0");
            if (XR)
            {
                enLog << " Initialized.\n";
            }
            else
            {
                enLog << " Failed to init.\n";
            }
        }
    }
    else
    {
        enLog << "OpenXR: Runtime is missing.\n";
    }

#endif

    return (XR == nullptr) ? false : true;
}

} // en::xr

std::unique_ptr<xr::Interface> XR = nullptr;

} // en