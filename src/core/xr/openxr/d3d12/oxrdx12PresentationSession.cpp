
#include "core/xr/openxr/d3d12/oxrdx12PresentationSession.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D12
#define XR_NO_PROTOTYPES

#include "OpenXR/openxr.h"
#include "OpenXR/openxr_platform.h"
#include "OpenXR/openxr_platform_defines.h"

#include "core/rendering/d3d12/dx12Texture.h"

#include "core/xr/openxr/oxrDLL.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrInterface.h"

namespace en
{
namespace xr
{

oxrPresentationSessionD3D12::oxrPresentationSessionD3D12(
        const XrSystemId runtime,
        const XrViewConfigurationType viewConfigurationType, 
        const PresentationSessionDescriptor& descriptor,
        const gpu::GpuDevice* gpu,
        const gpu::QueueType type, 
        const uint32 queue) :
    oxrPresentationSession(
        runtime,
        viewConfigurationType, 
        descriptor,
        gpu,
        type, 
        queue)
{
}

void oxrPresentationSessionD3D12::createPresentationSessionWithGraphicBinding(
    const XrSystemId runtime,
    const gpu::GpuDevice* gpu,
    const gpu::QueueType type, 
    const uint32 queue)
{
    const gpu::Direct3D12Device* d3dDevice = reinterpret_cast<const gpu::Direct3D12Device*>(gpu);

    // Create presentation session using specified GPU, Queue Family and Index
    XrGraphicsBindingD3D12KHR graphicsInfo;
    graphicsInfo.type   = XR_TYPE_GRAPHICS_BINDING_D3D12_KHR;
    graphicsInfo.next   = nullptr;
    graphicsInfo.device = d3dDevice->device;
    graphicsInfo.queue  = d3dDevice->queue[underlyingType(type)];

    XrSessionCreateInfo info;
    info.type        = XR_TYPE_SESSION_CREATE_INFO;
    info.next        = &graphicsInfo;
    info.createFlags = 0;       // Reserved
    info.systemId    = runtime; 

    Validate( session, xrCreateSession(session->instance, &info, &presentationSession) )
}

uint64 oxrPresentationSessionD3D12::translateTextureFormat(const gpu::Format format)
{
    return gpu::TranslateTextureFormat[underlyingType(format)];
}

void oxrPresentationSessionD3D12::extendColorSwapChainInfo(XrSwapchainCreateInfo& info)
{
}

XrSwapchainImageBaseHeader* oxrPresentationSessionD3D12::allocateSwapChainImagesArray(uint32 framesCount)
{
    XrSwapchainImageBaseHeader* result = (XrSwapchainImageBaseHeader*) new XrSwapchainImageD3D12KHR[framesCount];

#if defined(EN_DEBUG)
    for(uint32 i=0; i<framesCount; ++i)
    {
        assert( result[i].type == XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR );
        assert( result[i].next == nullptr );
    }
#endif

    return result;
}

gpu::Texture* oxrPresentationSessionD3D12::createTextureBackedBySwapChainImage(const gpu::GpuDevice* gpu, gpu::TextureState& textureState, XrSwapchainImageBaseHeader* imageDesc)
{
    // Create textures backed by image from Swap-Chain
    gpu::TextureD3D12* texture = new gpu::TextureD3D12((gpu::Direct3D12Device*)gpu, textureState);
    
    XrSwapchainImageD3D12KHR& desc = *((XrSwapchainImageD3D12KHR*)imageDesc);
    
    texture->handle = desc.texture;

    return texture;
}

oxrPresentationSessionD3D12::~oxrPresentationSessionD3D12()
{
    Validate( session, xrEndSession(presentationSession) )

    delete [] framesPool;

    for(uint32 i=0; i<frameTopology.texturesCount; ++i)
    {
        delete [] (XrSwapchainImageD3D12KHR*)colorFramesPool[i];

        Validate( session, xrDestroySwapchain(colorSwapChain[i]) )

        if (depthFramesPool[i])
        {
            delete [] (XrSwapchainImageD3D12KHR*)depthFramesPool[i];

            Validate( session, xrDestroySwapchain(depthSwapChain[i]) )
        }
    }

    delete [] supportedFormats;

    Validate( session, xrDestroySession(presentationSession) )
}

} // en::xr
} // en

#endif