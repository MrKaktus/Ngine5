/*

 Ngine v5.0

 Module      : OpenVR Interface
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENVR_INTERFACE
#define ENG_CORE_XR_OPENVR_INTERFACE

#include "assert.h"

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)
#include "core/rendering/d3d12/dx12.h"
#endif
#if defined(EN_MODULE_RENDERER_VULKAN)
#include "core/rendering/vulkan/vulkan.h"
#endif

#include "core/types.h"
#include "core/xr/interface.h"

#include "core/log/log.h"
#include "core/parallel/thread.h"
#include "platform/system.h"

namespace en
{
namespace xr
{
class ovrInterface : public Interface
{
public:
/*
    XrInstance             instance;
    XrResult               lastResult[MaxSupportedThreads];
    LayerDescriptor*       layer;
    uint32                 layersCount;
    XrExtensionProperties* globalExtension;
    uint32                 globalExtensionsCount;

    XrSpace                worldSpace; // Space in relation to which player and objects are tracked
    XrSpace                headSpace;  // Head space, in relation to which views are tracked

    oxrHeadset*            headset;    // OpenXR supports only one headset
*/

    static bool isPresent(void);

    ovrInterface(std::string appName);
   ~ovrInterface();

    virtual uint32 headsets(void);
    virtual Headset* headset(const uint32 index);
};

} // en::xr
} // en

#endif