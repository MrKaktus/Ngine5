
#include "core/xr/openxr/vulkan/oxrvkPresentationSession.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#if defined(EN_PLATFORM_ANDROID)
#define XR_USE_PLATFORM_ANDROID
#endif
#if defined(EN_PLATFORM_WINDOWS)
#define XR_USE_PLATFORM_WIN32
#endif
#define XR_USE_GRAPHICS_API_VULKAN

#include "OpenXR/openxr.h"
#include "OpenXR/openxr_platform.h"
#include "OpenXR/openxr_platform_defines.h"

#include "core/rendering/vulkan/vkTexture.h"

#include "core/xr/openxr/oxrDLL.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrInterface.h"





namespace en
{
namespace xr
{






typedef XrResult (XRAPI_PTR *PFN_xrResultToString)(XrInstance instance, XrResult value, char buffer[XR_MAX_RESULT_STRING_SIZE]);
typedef XrResult (XRAPI_PTR *PFN_xrStructureTypeToString)(XrInstance instance, XrStructureType value, char buffer[XR_MAX_STRUCTURE_NAME_SIZE]);


typedef XrResult (XRAPI_PTR *PFN_xrEnumerateReferenceSpaces)(XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, XrReferenceSpaceType* spaces);
typedef XrResult (XRAPI_PTR *PFN_xrCreateReferenceSpace)(XrSession session, const XrReferenceSpaceCreateInfo* createInfo, XrSpace* space);
typedef XrResult (XRAPI_PTR *PFN_xrGetReferenceSpaceBoundsRect)(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df* bounds);
typedef XrResult (XRAPI_PTR *PFN_xrCreateActionSpace)(XrAction action, const XrActionSpaceCreateInfo* createInfo, XrSpace* space);
typedef XrResult (XRAPI_PTR *PFN_xrLocateSpace)(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation* location);
typedef XrResult (XRAPI_PTR *PFN_xrDestroySpace)(XrSpace space);




typedef XrResult (XRAPI_PTR *PFN_xrLocateViews)(XrSession session, const XrViewLocateInfo* viewLocateInfo, XrViewState* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrView* views);
typedef XrResult (XRAPI_PTR *PFN_xrStringToPath)(XrInstance instance, const char* pathString, XrPath* path);
typedef XrResult (XRAPI_PTR *PFN_xrPathToString)(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
typedef XrResult (XRAPI_PTR *PFN_xrCreateActionSet)(XrSession session, const XrActionSetCreateInfo* createInfo, XrActionSet* actionSet);
typedef XrResult (XRAPI_PTR *PFN_xrDestroyActionSet)(XrActionSet actionSet);
typedef XrResult (XRAPI_PTR *PFN_xrCreateAction)(XrActionSet actionSet, const XrActionCreateInfo* createInfo, XrAction* action);
typedef XrResult (XRAPI_PTR *PFN_xrDestroyAction)(XrAction action);
typedef XrResult (XRAPI_PTR *PFN_xrSetInteractionProfileSuggestedBindings)(XrSession session, const XrInteractionProfileSuggestedBinding* suggestedBindings);
typedef XrResult (XRAPI_PTR *PFN_xrGetCurrentInteractionProfile)(XrSession session, XrPath topLevelUserPath, XrInteractionProfileState* interactionProfile);
typedef XrResult (XRAPI_PTR *PFN_xrGetActionStateBoolean)(XrAction action, uint32_t countSubactionPaths, const XrPath* subactionPaths, XrActionStateBoolean* data);
//typedef XrResult (XRAPI_PTR *PFN_xrGetActionStateVector1f)(XrAction action, uint32_t countSubactionPaths, const XrPath* subactionPaths, XrActionStateVector1f* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetActionStateVector2f)(XrAction action, uint32_t countSubactionPaths, const XrPath* subactionPaths, XrActionStateVector2f* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetActionStatePose)(XrAction action, XrPath subactionPath, XrActionStatePose* data);
typedef XrResult (XRAPI_PTR *PFN_xrSyncActionData)(XrSession session, uint32_t countActionSets, const XrActiveActionSet* actionSets);
typedef XrResult (XRAPI_PTR *PFN_xrGetBoundSourcesForAction)(XrAction action, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, XrPath* sources);
typedef XrResult (XRAPI_PTR *PFN_xrGetInputSourceLocalizedName)(XrSession session, XrPath source, XrInputSourceLocalizedNameFlags whichComponents, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
typedef XrResult (XRAPI_PTR *PFN_xrApplyHapticFeedback)(XrAction hapticAction, uint32_t countSubactionPaths, const XrPath* subactionPaths, const XrHapticBaseHeader* hapticEvent);
typedef XrResult (XRAPI_PTR *PFN_xrStopHapticFeedback)(XrAction hapticAction, uint32_t countSubactionPaths, const XrPath* subactionPaths);



void oxrVkInterfaceInit(void)
{
}




oxrPresentationSessionVK::oxrPresentationSessionVK(
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

void oxrPresentationSessionVK::createPresentationSessionWithGraphicBinding(
    const XrSystemId runtime,
    const gpu::GpuDevice* gpu,
    const gpu::QueueType type, 
    const uint32 queue)
{
    gpu::VulkanAPI* vulkanRuntime = (gpu::VulkanAPI*)&(*en::Graphics);
    const gpu::VulkanDevice* vulkanGPU = reinterpret_cast<const gpu::VulkanDevice*>(gpu);

    // Create presentation session using specified GPU, Queue Family and Index
    XrGraphicsBindingVulkanKHR graphicsInfo;
    graphicsInfo.type             = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
    graphicsInfo.next             = nullptr;
    graphicsInfo.instance         = vulkanRuntime->instance;
    graphicsInfo.physicalDevice   = vulkanGPU->handle;
    graphicsInfo.device           = vulkanGPU->device;
    graphicsInfo.queueFamilyIndex = vulkanGPU->queueTypeToFamily[underlyingType(type)];
    graphicsInfo.queueIndex       = queue;

    XrSessionCreateInfo info;
    info.type        = XR_TYPE_SESSION_CREATE_INFO;
    info.next        = &graphicsInfo;
    info.createFlags = 0;       // Reserved
    info.systemId    = runtime;

    Validate( session, xrCreateSession(session->instance, &info, &presentationSession) )
}

uint64 oxrPresentationSessionVK::translateTextureFormat(const gpu::Format format)
{
    return gpu::TranslateTextureFormat[underlyingType(format)];
}

void oxrPresentationSessionVK::extendColorSwapChainInfo(XrSwapchainCreateInfo& info)
{
    // Below structure would be used to pass list of Vulkan texel formats that application 
    // wants to overlay onto each other when using swap chain textures (to create texture
    // views). This structure would be passed in "next" pointer of XrSwapchainCreateInfo
    // once XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT flag would be used.
    // Currently this functionality is not supported by the engine. 
    //
    // // This structure needs to be pre-declared in Vulkan presentation session object to 
    // // prevent it's deallocation on exit from this function scope. Otherwise it needs to
    // // be dynamically allocated and later released by anothjer specialized method.
    // XrVulkanSwapchainFormatListCreateInfoKHR swapChainFormatInfo;
    // swapChainFormatInfo.type = XR_TYPE_VULKAN_SWAPCHAIN_FORMAT_LIST_CREATE_INFO_KHR;
    // swapChainFormatInfo.next = nullptr;
    // uint32_t                    viewFormatCount;
    // const VkFormat*             viewFormats;
    //
    // info.next        = &swapChainFormatInfo;
    // info.usageFlags |= XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT;
}

XrSwapchainImageBaseHeader* oxrPresentationSessionVK::allocateSwapChainImagesArray(uint32 framesCount)
{
    XrSwapchainImageBaseHeader* result = (XrSwapchainImageBaseHeader*) new XrSwapchainImageVulkanKHR[framesCount];

#if defined(EN_DEBUG)
    for(uint32 i=0; i<framesCount; ++i)
    {
        assert( result[i].type == XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR );
        assert( result[i].next == nullptr );
    }
#endif

    return result;
}

gpu::Texture* oxrPresentationSessionVK::createTextureBackedBySwapChainImage(const gpu::GpuDevice* gpu, gpu::TextureState& textureState, XrSwapchainImageBaseHeader* imageDesc)
{
    // Create textures backed by image from Swap-Chain
    gpu::TextureVK* texture = new gpu::TextureVK((gpu::VulkanDevice*)gpu, textureState);

    XrSwapchainImageVulkanKHR& desc = *((XrSwapchainImageVulkanKHR*)imageDesc);
    
    texture->handle = desc.image;

    return texture;
}

oxrPresentationSessionVK::~oxrPresentationSessionVK()
{
    Validate( session, xrEndSession(presentationSession) )

    delete [] framesPool;

    for(uint32 i=0; i<frameTopology.texturesCount; ++i)
    {
        delete [] (XrSwapchainImageVulkanKHR*)colorFramesPool[i];

        Validate( session, xrDestroySwapchain(colorSwapChain[i]) )

        if (depthFramesPool[i])
        {
            delete [] (XrSwapchainImageVulkanKHR*)depthFramesPool[i];

            Validate( session, xrDestroySwapchain(depthSwapChain[i]) )
        }
    }

    delete [] supportedFormats;

    Validate( session, xrDestroySession(presentationSession) )
}

} // en::xr
} // en

#endif