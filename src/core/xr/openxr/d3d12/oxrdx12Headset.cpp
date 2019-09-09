
#include "core/xr/openxr/d3d12/oxrdx12Headset.h"

#include "core/rendering/d3d12/dx12Device.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "assert.h"

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D12
#define XR_NO_PROTOTYPES

#include "OpenXR/openxr.h"
#include "OpenXR/openxr_platform.h"
#include "OpenXR/openxr_platform_defines.h"

#include "core/xr/openxr/oxrDLL.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrInterface.h"


// oxrdx12Interface.h

// Already defined in openxr_platform.h (can disable with XR_NO_PROTOTYPES)

// XR_KHR_D3D12_enable
DeclareFunction( xrGetD3D12GraphicsRequirementsKHR )
//*/

namespace en
{
namespace xr
{

oxrHeadsetD3D12::oxrHeadsetD3D12(oxrInterface& _session) :
    oxrHeadset(_session)
{
    loadInterfaceFunctionPointers();

    // Query GPU that should be used to render to given Headset
    XrGraphicsRequirementsD3D12KHR requirements;
    requirements.type        = XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR;
    requirements.next        = nullptr;
    Validate( session, xrGetD3D12GraphicsRequirementsKHR(session->instance, runtime, &requirements) )

    // Use LUID to find Direct3D12Device abstracting given GPU
    for(uint32 i=0; i<en::Graphics->devices(); ++i)
    {
        gpu::Direct3D12Device* d3dDevice = reinterpret_cast<gpu::Direct3D12Device*>(&*en::Graphics->device(i));
        LUID d3dDeviceLuid = d3dDevice->device->GetAdapterLuid();

        // TODO: Verify matching: requirements.minFeatureLevel;

        // TODO: Whats the proper way of comparing two LUID's?
        if (d3dDeviceLuid.HighPart == requirements.adapterLuid.HighPart &&
            d3dDeviceLuid.LowPart  == requirements.adapterLuid.LowPart)
        {
            gpu = d3dDevice;
        }
    }
}

oxrHeadsetD3D12::~oxrHeadsetD3D12()
{
    // TODO: Release resources

    clearInterfaceFunctionPointers();
}

// oxrdx12Interface.cpp

void oxrHeadsetD3D12::loadInterfaceFunctionPointers(void)
{
    // XR_KHR_D3D12_enable
    LoadInstanceFunction( session, xrGetD3D12GraphicsRequirementsKHR )
}

void oxrHeadsetD3D12::clearInterfaceFunctionPointers(void)
{
    // XR_KHR_D3D12_enable
    xrGetD3D12GraphicsRequirementsKHR         = nullptr;
}

} // en::xr
} // en

#endif