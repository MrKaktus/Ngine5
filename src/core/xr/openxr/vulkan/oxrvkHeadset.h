/*

 Ngine v5.0

 Module      : OpenXR Vulkan Headset
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_VULKAN_HEADSET
#define ENG_CORE_XR_OPENXR_VULKAN_HEADSET

#include "core/xr/openxr/oxrHeadset.h"

namespace en
{
namespace xr
{

class oxrHeadsetVK : public oxrHeadset
{
public:
    void loadInterfaceFunctionPointers(void);
    void clearInterfaceFunctionPointers(void);

    oxrHeadsetVK(oxrInterface& session);
   ~oxrHeadsetVK();
};

} // en::xr
} // en

#endif