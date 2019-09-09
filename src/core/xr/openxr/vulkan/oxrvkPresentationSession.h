/*

 Ngine v5.0

 Module      : OpenXR Vulkan Presentation Session
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_VULKAN_PRESENTATION_SESSION
#define ENG_CORE_XR_OPENXR_VULKAN_PRESENTATION_SESSION

#include "core/rendering/vulkan/vkDevice.h"
#include "core/xr/openxr/oxrPresentationSession.h"

namespace en
{
namespace xr
{
class oxrVkPresentationSession : public oxrPresentationSession
{
    public:
    oxrVkPresentationSession(const XrSystemId runtime,
                             const XrViewConfigurationType viewConfigurationType, 
                             const PresentationSessionDescriptor& descriptor,
                             const gpu::GpuDevice* gpu,
                             const gpu::QueueType type = gpu::QueueType::Universal, 
                             const uint32 queue = 0u);

    virtual void createPresentationSessionWithGraphicBinding(
        const XrSystemId runtime,
        const gpu::GpuDevice* gpu,
        const gpu::QueueType type, 
        const uint32 queue);

    virtual uint64 translateTextureFormat(const gpu::Format format);
    virtual void   extendColorSwapChainInfo(XrSwapchainCreateInfo& info);
    virtual XrSwapchainImageBaseHeader* allocateSwapChainImagesArray(uint32 framesCount);
    virtual gpu::Texture* createTextureBackedBySwapChainImage(const gpu::GpuDevice* gpu, gpu::TextureState& textureState, XrSwapchainImageBaseHeader* imageDesc);

    virtual ~oxrVkPresentationSession();
};

} // en::xr
} // en
#endif