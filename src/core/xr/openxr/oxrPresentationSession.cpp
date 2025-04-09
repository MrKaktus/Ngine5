
#include "assert.h"

#include "core/parallel/thread.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrPresentationSession.h"
#include "core/xr/openxr/oxrInterface.h"
#include "core/xr/openxr/oxrFrameState.h"
#include "core/xr/openxr/oxrFrame.h"

/// Max count of threads supported by rendering backends (workers and IO threads)
#define MaxSupportedThreads 256 

namespace en
{
namespace xr
{

XrSwapchainUsageFlags oxrTranslateTextureUsage(const gpu::TextureUsage usage)
{
    XrSwapchainUsageFlags usageFlags = 0;

    // Streamed or Static
    if (checkBitmask(underlyingType(usage), underlyingType(gpu::TextureUsage::Streamed)))
    {
        // TODO: Optimize it to specify Source/Destination only.
        usageFlags |= XR_SWAPCHAIN_USAGE_TRANSFER_SRC_BIT; // Copy from Swap-Chain to temp textures
        usageFlags |= XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT; // Copy from temp textures to Swap-Chain
    }

    if (checkBitmask(underlyingType(usage), underlyingType(gpu::TextureUsage::Read)))
    {
        usageFlags |= XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
    } 

    if (checkBitmask(underlyingType(usage), underlyingType(gpu::TextureUsage::Atomic)))
    {
        usageFlags |= XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT;
    }

    return usageFlags;
}

oxrPresentationSession::oxrPresentationSession(
        const XrSystemId runtime,
        const XrViewConfigurationType viewConfigurationType, 
        const PresentationSessionDescriptor& descriptor,
        const gpu::GpuDevice* gpu,
        const gpu::QueueType type, 
        const uint32 queue) :
    session(nullptr),
    presentationSession(XR_NULL_HANDLE),
    supportedFormatsCount(0),
    supportedFormats(nullptr),
    renderedFrameIndex(0),
    currentFrameState(),
    frameTopology(),
    framesCount(0),
    framesPool(nullptr),
    selectedBlendMode(XR_ENVIRONMENT_BLEND_MODE_OPAQUE),
    defaultLayerInfo(),
    defaultLayerViews(nullptr),
    defaultLayerDepth(nullptr)
{
    // Default initialization
    ///////////////////////////

    for(uint32 i=0; i<MaxTexturesPerFrame; ++i)
    {
        colorSwapChain[i]  = XR_NULL_HANDLE; 
        depthSwapChain[i]  = XR_NULL_HANDLE;
        colorFramesPool[i] = nullptr;
        depthFramesPool[i] = nullptr;
    }


    // Create Presentation Session
    ////////////////////////////////

    createPresentationSessionWithGraphicBinding(runtime, gpu, type, queue);


    // Query properties
    /////////////////////

    // Acquire list of supported Texel Formats
    Validate( session, xrEnumerateSwapchainFormats(presentationSession, 0, &supportedFormatsCount, nullptr) )
    if (supportedFormatsCount > 0)
    {
        supportedFormats = new sint64[supportedFormatsCount];
        Validate( session, xrEnumerateSwapchainFormats(presentationSession, supportedFormatsCount, &supportedFormatsCount, supportedFormats) )
    }


    // TODO: Validate descriptor first!


    // Query recommended topology
    ///////////////////////////////

    Validate( session, xrEnumerateViewConfigurationViews(session->instance, runtime, viewConfigurationType, 0, &frameTopology.viewsCount, nullptr) )
    assert(frameTopology.viewsCount );

    XrViewConfigurationView* viewLimits = new XrViewConfigurationView[frameTopology.viewsCount];

    uint32 viewsWritten = 0;
    Validate( session, xrEnumerateViewConfigurationViews(session->instance, runtime, viewConfigurationType, frameTopology.viewsCount, &viewsWritten, viewLimits) )
    assert(frameTopology.viewsCount == viewsWritten );

    // Calculate textures topology
    ////////////////////////////////

    // Calculate textures count
    frameTopology.texturesCount = frameTopology.viewsCount;
    if ( descriptor.surfaceLayout == SurfaceLayout::Shared ||
         descriptor.surfaceLayout == SurfaceLayout::Layered )
    {
        // By default Layered layout keeps max 2 views per texture
        frameTopology.texturesCount /= 2;
    }

    // Calculate layers count
    uint32 surfaceLayers  = 1;
    if (descriptor.surfaceLayout == SurfaceLayout::Layered)
    {
        surfaceLayers = 2;
    }

    // Samples count cannot exceed maximum samples count of any view
    uint32 surfaceSamples = descriptor.samplesCount;
    for(uint32 i=0; i<frameTopology.viewsCount; ++i)
    {
        surfaceSamples = min(surfaceSamples, viewLimits[i].maxSwapchainSampleCount);
    }

    // Determine texture type
    gpu::TextureType textureType = gpu::TextureType::Texture2D;
    if (descriptor.surfaceLayout == SurfaceLayout::Layered)
    {
        if (descriptor.samplesCount > 1)
        {
            textureType = gpu::TextureType::Texture2DMultisampleArray;
        }
        else
        {
            textureType = gpu::TextureType::Texture2DArray;
        }
    }
    else
    if (descriptor.samplesCount > 1)
    {
        textureType = gpu::TextureType::Texture2DMultisample;
    }

    // Populate texture topology
    for(uint32 i=0; i<frameTopology.texturesCount; ++i)
    {
        TextureTopology& textureTopology = frameTopology.texturesTopology[i];

        if (descriptor.surfaceLayout == SurfaceLayout::Dedicated)
        {
            // TODO: What if samplingQuality is smaller than 1.0?

            // samplingQuality scales render target texels count in linear way, thus for e.g.
            // value of 2.0 represents two times more texels in resulting render target. Value
            // of 1.0 represents render target resolution that has 1:1 texel to panel pixel ratio
            // in the center of the view (which means it is still on average 1.4x bigger in
            // each dimmension, than physical panel resolution, to mitigate lens distortion).
            uint32 viewWidth  = uint32(float(viewLimits[i].recommendedImageRectWidth)  * sqrt(descriptor.samplingQuality));
            uint32 viewHeight = uint32(float(viewLimits[i].recommendedImageRectHeight) * sqrt(descriptor.samplingQuality));

            viewWidth  = min(viewWidth,  viewLimits[i].maxImageRectWidth);
            viewHeight = min(viewHeight, viewLimits[i].maxImageRectHeight);

            textureTopology.width  = viewWidth;
            textureTopology.height = viewHeight;
        }
        else
        if (descriptor.surfaceLayout == SurfaceLayout::Shared)
        {
            uint32 viewIndex = i * 2;

            uint32 leftViewWidth  = uint32(float(viewLimits[viewIndex].recommendedImageRectWidth)  * sqrt(descriptor.samplingQuality));
            uint32 leftViewHeight = uint32(float(viewLimits[viewIndex].recommendedImageRectHeight) * sqrt(descriptor.samplingQuality));

            uint32 rightViewWidth  = uint32(float(viewLimits[viewIndex + 1].recommendedImageRectWidth)  * sqrt(descriptor.samplingQuality));
            uint32 rightViewHeight = uint32(float(viewLimits[viewIndex + 1].recommendedImageRectHeight) * sqrt(descriptor.samplingQuality));

            leftViewWidth  = min(leftViewWidth,  viewLimits[viewIndex].maxImageRectWidth);
            leftViewHeight = min(leftViewHeight, viewLimits[viewIndex].maxImageRectHeight);

            rightViewWidth  = min(rightViewWidth,  viewLimits[viewIndex + 1].maxImageRectWidth);
            rightViewHeight = min(rightViewHeight, viewLimits[viewIndex + 1].maxImageRectHeight);

            textureTopology.width  = leftViewWidth + rightViewWidth;
            textureTopology.height = max(leftViewHeight, rightViewHeight);
        }
        else
        if (descriptor.surfaceLayout == SurfaceLayout::Layered)
        {
            uint32 viewIndex = i * 2;

            uint32 leftViewWidth  = uint32(float(viewLimits[viewIndex].recommendedImageRectWidth)  * sqrt(descriptor.samplingQuality));
            uint32 leftViewHeight = uint32(float(viewLimits[viewIndex].recommendedImageRectHeight) * sqrt(descriptor.samplingQuality));

            uint32 rightViewWidth  = uint32(float(viewLimits[viewIndex + 1].recommendedImageRectWidth)  * sqrt(descriptor.samplingQuality));
            uint32 rightViewHeight = uint32(float(viewLimits[viewIndex + 1].recommendedImageRectHeight) * sqrt(descriptor.samplingQuality));

            leftViewWidth = min(leftViewWidth, viewLimits[viewIndex].maxImageRectWidth);
            leftViewHeight = min(leftViewHeight, viewLimits[viewIndex].maxImageRectHeight);

            rightViewWidth = min(rightViewWidth, viewLimits[viewIndex + 1].maxImageRectWidth);
            rightViewHeight = min(rightViewHeight, viewLimits[viewIndex + 1].maxImageRectHeight);

            textureTopology.width  = max(leftViewWidth, rightViewWidth);
            textureTopology.height = max(leftViewHeight, rightViewHeight);
        }

        textureTopology.textureType = textureType;
        textureTopology.layers      = surfaceLayers;
        textureTopology.samples     = surfaceSamples;
        textureTopology.mipmaps     = 1;
    }

    // Calculate views topology

    // This calculation is backend independent, and assumes that views
    // are always put in pairs side by side in case of Shared layout, 
    // and layer after layer in case of Layered one.
    for(uint32 i=0; i<frameTopology.viewsCount; ++i)
    {
        ViewTopology& viewTopology = frameTopology.viewsTopology[i];

        // Position, orientation and Frustum of each View is patched
        // at startFrame() call. When they are provided for current
        // predicted presentation time.
        //
        // TODO: OpenXR API design issue. Why Views topology cannot be queried upfront?
        //
        // viewTopology.position[0]    = 
        // viewTopology.position[1]    = 
        // viewTopology.position[2]    = 
        // viewTopology.orientation[0] = 
        // viewTopology.orientation[1] = 
        // viewTopology.orientation[2] = 
        // viewTopology.orientation[3] = 
        // viewTopology.tangent[0]     = 
        // viewTopology.tangent[1]     = 
        // viewTopology.tangent[2]     = 
        // viewTopology.tangent[3]     = 

        viewTopology.depthRange[0]  = descriptor.depthRange.x;
        viewTopology.depthRange[1]  = descriptor.depthRange.y;

        // Determine backing surface in the frame (universal calculation)
        if (descriptor.surfaceLayout == SurfaceLayout::Shared ||
            descriptor.surfaceLayout == SurfaceLayout::Layered)
        {
            viewTopology.surface = i / 2; // Two views per texture
        }
        else
        {
            viewTopology.surface = i;
        }

        // Determine backing slice in the surface (Layered case only)
        if (descriptor.surfaceLayout == SurfaceLayout::Layered)
        {
            viewTopology.slice   = i % 2; // 2 views interleaved on 2 layers
        }
        else
        {
            viewTopology.slice   = 0;
        }

        uint32 viewportTexelsWidth  = uint32(float(viewLimits[i].recommendedImageRectWidth)  * sqrt(descriptor.samplingQuality));
        uint32 viewportTexelsHeight = uint32(float(viewLimits[i].recommendedImageRectHeight) * sqrt(descriptor.samplingQuality));

        // Calculate viewport width and height in texels
        viewTopology.viewportTexelsOrigin.x = 0;
        viewTopology.viewportTexelsOrigin.y = 0;
        viewTopology.viewportTexelsWidth    = min(viewportTexelsWidth,  viewLimits[i].maxImageRectWidth);
        viewTopology.viewportTexelsHeight   = min(viewportTexelsHeight, viewLimits[i].maxImageRectHeight);

        // Specify destination region for rendering (in normalized coordinates)
        viewTopology.viewportOrigin.x = 0.0f;
        viewTopology.viewportOrigin.y = 0.0f;
        viewTopology.viewportWidth    = 1.0f;
        viewTopology.viewportHeight   = 1.0f;

        if (descriptor.surfaceLayout == SurfaceLayout::Shared)
        {
            // Calculate viewport width and height taking into notice that they may not cover whole backing texture
            viewTopology.viewportWidth  = (float)viewTopology.viewportTexelsWidth  / (float)frameTopology.texturesTopology[viewTopology.surface].width;
            viewTopology.viewportHeight = (float)viewTopology.viewportTexelsHeight / (float)frameTopology.texturesTopology[viewTopology.surface].height;

            if (i % 2 == 1)
            {
                // X Origin of right viewport is equal to width of left viewport
                viewTopology.viewportTexelsOrigin.x = frameTopology.viewsTopology[i-1].viewportTexelsWidth;

                viewTopology.viewportOrigin.x = (float)viewTopology.viewportTexelsOrigin.x / (float)frameTopology.texturesTopology[viewTopology.surface].width;
            }
        }

        if (descriptor.surfaceLayout == SurfaceLayout::Layered)
        {
            // Calculate viewport width and height taking into notice that they may not cover whole backing texture
            viewTopology.viewportWidth  = (float)viewTopology.viewportTexelsWidth  / (float)frameTopology.texturesTopology[viewTopology.surface].width;
            viewTopology.viewportHeight = (float)viewTopology.viewportTexelsHeight / (float)frameTopology.texturesTopology[viewTopology.surface].height;
        }
    }


    // Allocate Swap-Chains
    /////////////////////////

    assert( descriptor.colorFormat != gpu::Format::Unsupported );

    XrSwapchainCreateFlags creationFlags = 0; // XR_SWAPCHAIN_CREATE_PROTECTED_CONTENT_BIT, XR_SWAPCHAIN_CREATE_STATIC_IMAGE_BIT

    for(uint32 i=0; i<frameTopology.texturesCount; ++i)
    {
        XrSwapchainCreateInfo swapChainInfo;
        swapChainInfo.type        = XR_TYPE_SWAPCHAIN_CREATE_INFO;
        swapChainInfo.next        = nullptr;
        swapChainInfo.createFlags = creationFlags;
        swapChainInfo.usageFlags  = oxrTranslateTextureUsage(descriptor.colorUsage) | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainInfo.format      = translateTextureFormat(descriptor.colorFormat); // Translate pixel format to backing graphic API format
        swapChainInfo.sampleCount = frameTopology.texturesTopology[i].samples;
        swapChainInfo.width       = frameTopology.texturesTopology[i].width;
        swapChainInfo.height      = frameTopology.texturesTopology[i].height;
        swapChainInfo.faceCount   = 1;              // or 6 for CubeMaps
        swapChainInfo.arraySize   = frameTopology.texturesTopology[i].layers;
        swapChainInfo.mipCount    = frameTopology.texturesTopology[i].mipmaps;

        // Allow graphic API specific extensions 
        extendColorSwapChainInfo(swapChainInfo);

        Validate( session, xrCreateSwapchain(presentationSession, &swapChainInfo, &colorSwapChain[i]) )

        // Query amount of frames in given SwapChain
        uint32 tempFramesCount = 0;
        Validate( session, xrEnumerateSwapchainImages(colorSwapChain[i], 0, &tempFramesCount, nullptr) )
        assert( tempFramesCount );

        if (i == 0)
        {
            framesCount = tempFramesCount;
        }
        else
        {
            assert( framesCount == tempFramesCount );
        }
    }

    if (descriptor.depthFormat != gpu::Format::Unsupported)
    {
        for(uint32 i=0; i<frameTopology.texturesCount; ++i)
        {
            XrSwapchainCreateInfo swapChainInfo;
            swapChainInfo.type        = XR_TYPE_SWAPCHAIN_CREATE_INFO;
            swapChainInfo.next        = nullptr;
            swapChainInfo.createFlags = creationFlags;
            swapChainInfo.usageFlags  = oxrTranslateTextureUsage(descriptor.depthUsage) | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            swapChainInfo.format      = translateTextureFormat(descriptor.depthFormat); // Translate pixel format to backing graphic API format
            swapChainInfo.sampleCount = frameTopology.texturesTopology[i].samples;
            swapChainInfo.width       = frameTopology.texturesTopology[i].width;
            swapChainInfo.height      = frameTopology.texturesTopology[i].height;
            swapChainInfo.faceCount   = 1;              // or 6 for CubeMaps
            swapChainInfo.arraySize   = frameTopology.texturesTopology[i].layers;
            swapChainInfo.mipCount    = frameTopology.texturesTopology[i].mipmaps;

            Validate( session, xrCreateSwapchain(presentationSession, &swapChainInfo, &depthSwapChain[i]) )

            // Query amount of frames in given SwapChain
            uint32 tempFramesCount = 0;
            Validate( session, xrEnumerateSwapchainImages(depthSwapChain[i], 0, &tempFramesCount, nullptr) )
            assert( framesCount == tempFramesCount );
        }
    }


    // Create frames
    //////////////////

    for(uint32 i=0; i<frameTopology.texturesCount; ++i)
    {
        colorFramesPool[i] = allocateSwapChainImagesArray(framesCount); // Allocates array of graphic API specific descriptors
        assert( colorFramesPool[i] );

        // Query texture handles per frame in SwapChain
        uint32 framesCountWritten = 0;
        Validate( session, xrEnumerateSwapchainImages(colorSwapChain[i], framesCount, &framesCountWritten, colorFramesPool[i]) )
        assert( framesCount == framesCountWritten );
    }

    if (descriptor.depthFormat != gpu::Format::Unsupported)
    {
        for(uint32 i=0; i<frameTopology.texturesCount; ++i)
        {
            depthFramesPool[i] = allocateSwapChainImagesArray(framesCount); // Allocates array of graphic API specific descriptors
            assert( depthFramesPool[i] );
            
            // Query texture handles per frame in SwapChain
            uint32 framesCountWritten = 0;
            Validate( session, xrEnumerateSwapchainImages(depthSwapChain[i], framesCount, &framesCountWritten, depthFramesPool[i]) )
            assert( framesCount == framesCountWritten );
        } 
    }

    framesPool = new oxrFrame[framesCount];

    // Generate Texture objects wrapping Vulkan texture handles
    for(uint32 i=0; i<framesCount; ++i)
    {
        oxrFrame& frame = framesPool[i];

        frame.texturesCount = frameTopology.texturesCount;
        frame.colorTextures = new gpu::Texture*[frameTopology.texturesCount];
        frame.depthTextures = new gpu::Texture*[frameTopology.texturesCount];
        
        for(uint32 j=0; j< frameTopology.texturesCount; ++j)
        {
            gpu::TextureState textureState(
                frameTopology.texturesTopology[j].textureType,
                descriptor.colorFormat,
                descriptor.colorUsage | gpu::TextureUsage::Read,
                frameTopology.texturesTopology[j].width,
                frameTopology.texturesTopology[j].height);

            frame.colorTextures[j] = createTextureBackedBySwapChainImage(gpu, textureState, &colorFramesPool[j][i]);
        }

        if (descriptor.depthFormat != gpu::Format::Unsupported)
        {
            for(uint32 j=0; j<frameTopology.texturesCount; ++j)
            {
                gpu::TextureState textureState(
                    frameTopology.texturesTopology[j].textureType,
                    descriptor.depthFormat,
                    descriptor.depthUsage | gpu::TextureUsage::Read,
                    frameTopology.texturesTopology[j].width,
                    frameTopology.texturesTopology[j].height);

                frame.depthTextures[j] = createTextureBackedBySwapChainImage(gpu, textureState, &depthFramesPool[j][i]);
            }
        }
    } 


    // Create reference spaces
    ////////////////////////////

	// Reference spaces can be created in OpenXR only after Presentation Session is
	// created, but we want to have objects grounded in tracking systems and spaces
	// even before that happens. This will still work, as application shouldn't 
	// have access to device poses, before Presentation Session is created, so 
	// default poses will be returned in such case, and fact of delayed Spaces
	// creation can be hidden.

    session->worldSpace = XR_NULL_HANDLE;
    session->headSpace  = XR_NULL_HANDLE;

    XrPosef defaultPose;
    defaultPose.orientation = { 0.0f, 0.0f, 0.0f, 1.0f };
    defaultPose.position.x = 0.0f;
    defaultPose.position.y = 0.0f;
    defaultPose.position.z = 0.0f;

    // Creates world space, in reference to "View" space which is one of three base spaces
    XrReferenceSpaceCreateInfo spaceInfo;
    spaceInfo.type                 = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
    spaceInfo.next                 = nullptr;
    spaceInfo.referenceSpaceType   = XR_REFERENCE_SPACE_TYPE_STAGE; // XR_REFERENCE_SPACE_TYPE_LOCAL
    spaceInfo.poseInReferenceSpace = defaultPose;

    Validate( session, xrCreateReferenceSpace(presentationSession, &spaceInfo, &session->worldSpace) )

    // Creates head space, representing user head position and used to report views relative to it
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;

    Validate( session, xrCreateReferenceSpace(presentationSession, &spaceInfo, &session->headSpace) )


    // Create compositing layers
    //////////////////////////////

    defaultLayerViews = new XrCompositionLayerProjectionView[frameTopology.viewsCount];
    if (descriptor.depthFormat != gpu::Format::Unsupported)
    {
        defaultLayerDepth = new XrCompositionLayerDepthInfoKHR[frameTopology.viewsCount];
    }

    // Specify default layer configuration
    for(uint32 i=0; i<frameTopology.viewsCount; ++i)
    {
        // Pose is in world space, to indicate that it can be reprojected by Compositor
        defaultLayerViews[i].type                     = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
        defaultLayerViews[i].next                     = nullptr;
      //defaultLayerViews[i].pose                     = Pose is patched on presentWithPose() call when final headset pose for which frame was rendered is known
      //defaultLayerViews[i].fov                      = Fov is patched on startFrame() call when current Headset topology is obtained
        defaultLayerViews[i].subImage.swapchain               = colorSwapChain[frameTopology.viewsTopology[i].surface];
        defaultLayerViews[i].subImage.imageRect.offset.x      = frameTopology.viewsTopology[i].viewportTexelsOrigin.x;
        defaultLayerViews[i].subImage.imageRect.offset.y      = frameTopology.viewsTopology[i].viewportTexelsOrigin.y;
        defaultLayerViews[i].subImage.imageRect.extent.width  = frameTopology.viewsTopology[i].viewportTexelsWidth;
        defaultLayerViews[i].subImage.imageRect.extent.height = frameTopology.viewsTopology[i].viewportTexelsHeight;
        defaultLayerViews[i].subImage.imageArrayIndex         = frameTopology.viewsTopology[i].slice;

        if (descriptor.depthFormat != gpu::Format::Unsupported)
        {
            defaultLayerViews[i].next = &defaultLayerDepth[i];

            defaultLayerDepth[i].type                             = XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR;
            defaultLayerDepth[i].next                             = nullptr;
            defaultLayerDepth[i].subImage.swapchain               = depthSwapChain[frameTopology.viewsTopology[i].surface];
            defaultLayerDepth[i].subImage.imageRect.offset.x      = frameTopology.viewsTopology[i].viewportTexelsOrigin.x;
            defaultLayerDepth[i].subImage.imageRect.offset.y      = frameTopology.viewsTopology[i].viewportTexelsOrigin.y;
            defaultLayerDepth[i].subImage.imageRect.extent.width  = frameTopology.viewsTopology[i].viewportTexelsWidth;
            defaultLayerDepth[i].subImage.imageRect.extent.height = frameTopology.viewsTopology[i].viewportTexelsHeight;
            defaultLayerDepth[i].subImage.imageArrayIndex         = frameTopology.viewsTopology[i].slice;
            defaultLayerDepth[i].minDepth                         = 0.0f;  // 1.0f for Reverse Z 
            defaultLayerDepth[i].maxDepth                         = 1.0f;  // 0.0f
            defaultLayerDepth[i].nearZ                            = descriptor.depthRange.x;
            defaultLayerDepth[i].farZ                             = descriptor.depthRange.y;
        }
    }

    // Specify default layer
    defaultLayerInfo.type       = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
    defaultLayerInfo.next       = nullptr;
    defaultLayerInfo.layerFlags = XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT; // Enforce CAC on standalone mobile VR headsets
                               // XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT
    defaultLayerInfo.space      = session->worldSpace;  // Indicated world space or head space locked layer
    defaultLayerInfo.viewCount  = frameTopology.viewsCount;
    defaultLayerInfo.views      = defaultLayerViews;

    // Other layers support will be added in specialized method call
    // XR_TYPE_COMPOSITION_LAYER_QUAD
    // XR_TYPE_COMPOSITION_LAYER_CUBE_KHR
    // XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR
    // XR_TYPE_COMPOSITION_LAYER_EQUIRECT_KHR


    // Specify blend mode
    if (descriptor.presentationMode == PresentationMode::MergedDigitally)
    {
        selectedBlendMode = XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND; // Pass-Through AR/MR
    }
    else
    if (descriptor.presentationMode == PresentationMode::MergedOptically)
    {
        selectedBlendMode = XR_ENVIRONMENT_BLEND_MODE_ADDITIVE;    // See-through AR/MR
    }


    // Start presentation session
    ///////////////////////////////

    XrSessionBeginInfo beginInfo;
    beginInfo.type                         = XR_TYPE_SESSION_BEGIN_INFO;
    beginInfo.next                         = nullptr;
    beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;  // TODO: Select configuration on presentation session creation

    Validate( session, xrBeginSession(presentationSession, &beginInfo) )
}

void oxrPresentationSession::createPresentationSessionWithGraphicBinding(
    const XrSystemId runtime,
    const gpu::GpuDevice* gpu,
    const gpu::QueueType type, 
    const uint32 queue)
{
    // Specialized version of this method should be called
    assert( 0 );
}

uint64 oxrPresentationSession::translateTextureFormat(const gpu::Format format)
{
    // Specialized version of this method should be called
    assert( 0 );
    return 0;
}

void oxrPresentationSession::extendColorSwapChainInfo(XrSwapchainCreateInfo& info)
{
    // Specialized version of this method should be called
    assert( 0 );
}

XrSwapchainImageBaseHeader* oxrPresentationSession::allocateSwapChainImagesArray(uint32 framesCount)
{
    // Specialized version of this method should be called
    assert( 0 );
    return nullptr;
}

gpu::Texture* oxrPresentationSession::createTextureBackedBySwapChainImage(const gpu::GpuDevice* gpu, gpu::TextureState& textureState, XrSwapchainImageBaseHeader* imageDesc)
{
    // Specialized version of this method should be called
    assert( 0 );
    return nullptr;
}

oxrPresentationSession::~oxrPresentationSession()
{
    // TODO: destroy resources !
}




















// TODO: How events are returned?
Event* oxrPresentationSession::queryNextEvent(void)
{
    XrEventDataBuffer eventBase;
    eventBase.type = XR_TYPE_EVENT_DATA_BUFFER;
    eventBase.next = nullptr;

    Validate( session, xrPollEvent(session->instance, &eventBase) )

    XrResult result = session->lastResult[currentThreadId()];
    if (result == XR_EVENT_UNAVAILABLE)
    {
        return nullptr;
    }

    if (result == XR_SUCCESS)
    {
        switch (eventBase.type)
        {
            case XR_TYPE_EVENT_DATA_BUFFER:

            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
            {
                const XrEventDataInstanceLossPending& xrEvent = *reinterpret_cast<XrEventDataInstanceLossPending*>(&eventBase);
                // ...
                break;
            }

            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: 
            {
                const XrEventDataSessionStateChanged& xrEvent = *reinterpret_cast<XrEventDataSessionStateChanged*>(&eventBase);

                // Actual presentation session state transitions:
                //
                // XR_SESSION_STATE_UNKNOWN = 0,
                // XR_SESSION_STATE_IDLE = 1,
                // XR_SESSION_STATE_READY = 2,
                // XR_SESSION_STATE_RUNNING = 3,
                // XR_SESSION_STATE_VISIBLE = 4,      // ActivePause
                // XR_SESSION_STATE_FOCUSED = 5,      // Active
                // XR_SESSION_STATE_STOPPING = 6,     // transitioning to Paused
                // XR_SESSION_STATE_LOSS_PENDING = 7, // Runtime crashed
                // XR_SESSION_STATE_EXITING = 8,      // transitioning to Shutdown

                // ...
                break;
            }

            // Reference space origin (and possible bounds) changed
            // In most cases this happens when user recenters its pose
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
            {
                const XrEventDataReferenceSpaceChangePending& xrEvent = *reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventBase);

                // TODO:

                break;
            }

            // Events queue overflowed
            case XR_TYPE_EVENT_DATA_EVENTS_LOST:
            {
                const XrEventDataEventsLost& xrEvent = *reinterpret_cast<XrEventDataEventsLost*>(&eventBase);

                // There is nothing that can be done about it so log warning message
                Log << "WARNING!: XR Events queue overflowed! Lost " << xrEvent.lostEventCount << "events!\n";
                break;
            }

            // Input to action binding changed
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
            {
                // TODO: Try to rebind input
                // xrGetCurrentInteractionProfile
                break;
            }

            // Performance degradation notification
            case XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT:
            {
                const XrEventDataPerfSettingsEXT& xrEvent = *reinterpret_cast<XrEventDataPerfSettingsEXT*>(&eventBase);

                // TODO:

                break;
            }

            // Stencil mesh (view visibility mask) has changed
            case XR_TYPE_EVENT_DATA_VISIBILITY_MASK_CHANGED_KHR:
            {
                const XrEventDataVisibilityMaskChangedKHR& xrEvent = *reinterpret_cast<XrEventDataVisibilityMaskChangedKHR*>(&eventBase);

                // TODO: xrGetVisibilityMaskKHR

                break;
            }

            default:
                // TODO:
                break;
        }
    }

    return nullptr;
}











/* OpenXR Spec before v0.9:

   XrWaitFrameDescription waitDesc;
   waitDesc.type        = XR_TYPE_WAIT_FRAME_DESCRIPTION;
   waitDesc.next        = nullptr;
   waitDsc.swapInterval = 1; // Number of display refreshes this wait should consume (1+)
                             // Application may ask to drop frequency (render every second frame, with ASW)

   // Returns prediction for the middle of the time period when photons are emmited.
   // Not coupled with xrEndFrame, may be called on separate thread, but called once per frame.
   result = xrWaitFrame(session,
                       &waitDesc,
                       &waitInfo);
*/

uint64 oxrPresentationSession::startFrame(void)
{
    // In OpenXR there is no ability to start frame before calling xrWaitFrame.

    // TODO: Do coarse estimate of frame target presentation event!
    assert( 0 );
    return 0;
}

FrameState* oxrPresentationSession::waitForOptimalPrediction(void)
{
    // Wait to pace frame
    ///////////////////////

    // For use in the future, if some additional extensions are defined:
    //
    // XrFrameWaitInfo waitInfo;
    // waitInfo type = XR_TYPE_FRAME_WAIT_INFO;
    // waitInfo.next = nullptr;

    // Equivalent of "waitForOptimalPrediction"
    Validate( session, xrWaitFrame(presentationSession, nullptr /* &waitInfo */, &currentFrameState) )
    assert( currentFrameState.type == XR_TYPE_FRAME_STATE );

    // Create frame state
    ///////////////////////

    oxrFrameState* frameState = new oxrFrameState();

    // TODO: Consider how to emulate this behavior over OpenXR, or how to hide and simplify it
    // 
    // frameState->targetPresentationEvent = uint64 
    // frameState->synchronizationValue    = uint64 
    // frameState->shadingRate             = uint64 
    frameState->renderingFrame          = ++renderedFrameIndex;

    frameState->viewsCount = frameTopology.viewsCount;
    frameState->viewsArray = new oxrView[frameTopology.viewsCount];


    // Query views topology for this frame
    ////////////////////////////////////////

    // There is no separation between:
    // View local poses in head space and headset predicted pose in world space!
    // View poses need to be queried in relation to VIEW space to acquire local
    // poses. To acquire headset pose, view 0 local pose need to be subtracted 
    // from view 0 gloal pose.

    XrViewLocateInfo viewPoseQuery;
    viewPoseQuery.type        = XR_TYPE_VIEW_LOCATE_INFO;
    viewPoseQuery.next        = nullptr;
    viewPoseQuery.displayTime = currentFrameState.predictedDisplayTime;
    viewPoseQuery.space       = session->headSpace;

    XrViewState viewState;

    // MaxViewsCount = session->systemInfo.graphicsProperties.maxViewCount
    XrView* viewPose = new XrView[frameTopology.viewsCount];

    // Queries predicted view poses and projection parameters for future moment in time (display time)
    // This prediction gets better, the smaller dT from now to targeted display time
    uint32 viewsWritten = 0;
    Validate( session, xrLocateViews(presentationSession, &viewPoseQuery, &viewState, frameTopology.viewsCount, &viewsWritten, viewPose) )
    assert( viewsWritten == frameTopology.viewsCount );
    if (viewsWritten > frameTopology.viewsCount)
    {
        viewsWritten = frameTopology.viewsCount;
    }

    // Patch frame topology
    for(uint32 i=0; i<viewsWritten; ++i)
    {
        assert( viewPose[i].type == XR_TYPE_VIEW );

        // TODO: Transform position and orientation to Ngine coordinate system!
        frameTopology.viewsTopology[i].position[0]    = viewPose[i].pose.position.x;
        frameTopology.viewsTopology[i].position[1]    = viewPose[i].pose.position.y;
        frameTopology.viewsTopology[i].position[2]    = viewPose[i].pose.position.z; 

        frameTopology.viewsTopology[i].orientation[0] = viewPose[i].pose.orientation.x;
        frameTopology.viewsTopology[i].orientation[1] = viewPose[i].pose.orientation.y;
        frameTopology.viewsTopology[i].orientation[2] = viewPose[i].pose.orientation.z;
        frameTopology.viewsTopology[i].orientation[3] = viewPose[i].pose.orientation.w;

        frameTopology.viewsTopology[i].tangent[0]     = tan(fabs(viewPose[i].fov.angleLeft));
        frameTopology.viewsTopology[i].tangent[1]     = tan(fabs(viewPose[i].fov.angleRight));
        frameTopology.viewsTopology[i].tangent[2]     = tan(fabs(viewPose[i].fov.angleUp));
        frameTopology.viewsTopology[i].tangent[3]     = tan(fabs(viewPose[i].fov.angleDown));

        // TODO: Compare to previous frame topology! Need to spawn events if topology changed!
        //       But we're already at waitForOptimalPrediction() after querying events! 

        // Patch view FOV in default layer description
        defaultLayerViews[i].fov = viewPose[i].fov;
    }

    // TODO: This is dependent on Headset tracking state, and really describes Headset pose prediction state.

    // viewState.viewStateFlags

    // XR_VIEW_STATE_ORIENTATION_VALID_BIT   = 0x00000001;
    // XR_VIEW_STATE_POSITION_VALID_BIT      = 0x00000002;
    // XR_VIEW_STATE_ORIENTATION_TRACKED_BIT = 0x00000004;
    // XR_VIEW_STATE_POSITION_TRACKED_BIT    = 0x00000008;

    return frameState;
}

void oxrPresentationSession::startEncoding(void)
{
    // Begin frame
    ////////////////

    // For use in the future, if some additional extensions are defined:
    //
    // XrFrameBeginInfo beginInfo;
    // beginInfo.type = XR_TYPE_FRAME_BEGIN_INFO;
    // beginInfo.next = nullptr;

    Validate( session, xrBeginFrame(presentationSession, nullptr /* &beginInfo */) )
}

// OpenXR wait for textures is ambiguous:
//
// " xrWaitSwapchainImage will implicitly wait on the oldest acquired swapchain image which has not yet been successfully waited on. 
//   Once a swapchain image has been successfully waited on, it must be released before waiting on the next acquired swapchain image. "
// 
// This allows acquiring images ahead of time on separate 
// Simulation / Update thread together with xrWaitFrame, and
// overlapping of those with previous frame Encode stage,
// enabling 2 frames deep rendering pipeline (2 on CPU, 1 on GPU).
//
Frame* oxrPresentationSession::currentFrame(void)
{
    uint32 frameIndex = 0;

    // TODO: As multiple frames can be acquired by the application (!)
    //       Emulated locking on acquire needs to be implemented to
    //       provide proper behavior of CPU/GPU work interleaving in
    //       case of equivalent of VSync Off mode.

    XrSwapchainImageAcquireInfo acquireInfo;
    acquireInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
    acquireInfo.next = nullptr;

    for(uint32 i=0; i<framesCount; ++i)
    {
        uint32 tempIndex = 0;
        Validate( session, xrAcquireSwapchainImage(colorSwapChain[i], &acquireInfo, &tempIndex) )
        if (i == 0)
        {
            frameIndex = tempIndex;
        }

        // Unfortunately there is no guarantee in which order frames will be pulled
        // from Swap-Chain, nor if they will be pulled in the same order from multiple
        // Swap-Chans (or if those chains even have the same depth/textures count).
        assert( frameIndex == tempIndex );
 
    }
    if (depthSwapChain[0] != XR_NULL_HANDLE)
    {
        for(uint32 i=0; i<frameTopology.texturesCount; ++i)
        {
            uint32 tempIndex = 0;
            Validate( session, xrAcquireSwapchainImage(depthSwapChain[i], &acquireInfo, &tempIndex) )
            
            // Unfortunately there is no guarantee in which order frames will be pulled
            // from Swap-Chain, not if they will be pulled in the same order from multiple
            // Swap-Chans (or if those chains even have the same depth/textures count).
            assert( frameIndex == tempIndex );
        }
    }

    // TODO: Duration time in nanoseconds!
    XrDuration waitTimeout;

    XrSwapchainImageWaitInfo waitInfo;
    waitInfo.type    = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
    waitInfo.next    = nullptr;
    waitInfo.timeout = waitTimeout;

    for(uint32 i=0; i<framesCount; ++i)
    {
        // Waits until Compositor finishes reading oldest acquired frame.
        // In most cases this wait will occur only on first Swap-Chain as
        // they are all used at once to compose final frame.
        // Additionally if this call is performed properly after wait
        // that synchronizes frame start, I suppose to not wait for frames
        // at this point at all, as compositor should be done with them 
        // for a long time now.
        Validate( session, xrWaitSwapchainImage(colorSwapChain[i], &waitInfo) )
    }
    if (depthSwapChain[0] != XR_NULL_HANDLE)
    {
        for(uint32 i=0; i<frameTopology.texturesCount; ++i)
        {
            Validate( session, xrWaitSwapchainImage(depthSwapChain[i], &waitInfo) )
        }
    }

    // Create new Frame object referencing textures from Frame Pool,
    // each time currentFrame() is called. This object can be hold on
    // to by application, and later released on its own pace.
    oxrFrame* frame = new oxrFrame(frameTopology.texturesCount);
    for(uint32 i=0; i<frameTopology.texturesCount; ++i)
    {
        frame->colorTextures[i] = framesPool[frameIndex].colorTextures[i];
        frame->depthTextures[i] = framesPool[frameIndex].depthTextures[i];
    }

    return frame; //&framesPool[frameIndex];
}












    // Time when current frame may be displayed (if won't miss it's deadline)
    // "for the middle of the time period during which the new swapchains will be displayed."
    //currentFrameState.predictedDisplayTime;   // XrTime
    //currentFrameState.predictedDisplayPeriod; // XrDuration













/*

// Set the primary view configuration for the session.
XrSessionBeginInfo beginInfo = {XR_TYPE_SESSION_BEGIN_INFO, nullptr, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
CHK_XR(xrBeginSession(session, &beginInfo));

XrCompositionLayerFlags layerFlags; // previously initialized

// Allocate a buffer according to viewCount.
std::vector<XrView> views(viewCount, {XR_TYPE_VIEW});

// Get views
XrTime displayTime; // previously initialized
XrSpace space; // previously initialized

// Run a per-frame loop.
while (!quit)
{
    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.displayTime = displayTime;
    viewLocateInfo.space = space;

    XrViewState viewState{};
    uint32_t viewCountOutput;
    CHK_XR(xrLocateViews(session, &viewLocateInfo, &viewState, configViews.size(), &viewCountOutput, views.data()));

    // Wait for beginning the next frame.
    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    CHK_XR(xrWaitFrame(session, &frameWaitInfo, &frameState));

    // Begin frame
    XrFrameBeginInfo frameBeginInfo { XR_TYPE_FRAME_BEGIN_INFO };
    CHK_XR(xrBeginFrame(session, &frameBeginInfo));

    // ...
    // Use view and frameState for scene render.

    // End frame

}


*/








/* OpenXR Spec before v0.9:

   // Is capabe of handling worst case scenation of CAVE.
   // Each frame only sub-set is presented.
   XrCompositorLayerMultiProjectionElement projections[6];

   uint32 projectionsCount = headPose.projectionCountOutput;
   for(uint32 i=0; i<projectionsCount; ++i)
      {
      projections[i].viewTransform    = headPose.projections[i].viewTransform;    // XrRigidTransformf
      projections[i].projectionMatrix = headPose.projections[i].projectionMatrix; // XrMatrix4x4f
      projections[i].swapchain        = swapChain; // Specifies for which SwapChain this view is presented.
                                                   // Allows to finish frame that renders for several different HMD's?
      projections[i].arrayLayer       = i; // Index in 2DArray
      projections[i].imageRect        = ;  // XrRect2Df - View mapping to area of backing texture
      }

   // XR_KHR_extra_layer_info_depth
   XrExtraLayerInfoDepthKHR layerDepth;
   layerDepth.type                = XR_TYPE_EXTRA_LAYER_INFO_DEPTH_KHR;
   layerDepth.next                = nullptr;
   layerDepth.depthSwapchainCount = 1; // Count of swapChains
   layerDepth.depthSwapchains     = ;  // XrSwapchain*
   layerDepth.depthSwapchainRects = ;  // XrRect2Df* - Depth resolution may differ from color resolution!!
   layerDepth.minDepth = 0.01f;
   layerDepth.maxDepth = 1000.0f; // maxDepth >= minDepth so Reverse Z is not supported?

   XrCompositorLayerMultiProjection layer;
   // XrCompositorLayerHeader header:
   layer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   layer.header.next             = &layerDepth;
   layer.header.flags            = ; // XrCompositorLayerFlags
   layer.header.frameIndex       = ; // uint64_t
   layer.header.displayTime      = ; // XrNanoseconds
   layer.header.synthesisCpuTime = ; // XrNanoseconds
   layer.header.synthesisGpuTime = ; // XrNanoseconds
   layer.header.eye              = ; // XrEye
   layer.header.colorScale       = ; // XrVector4f
   layer.header.coordinateSystem = ; //XrCoordinateSystem
   layer.projectionCount = projectionsCount;
   layer.projections     = &projections[0];

   // Extra overlays: Quad
   XrCompositorLayerQuad quadLayer;
   // XrCompositorLayerHeader header:
   quadLayer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   quadLayer.header.next             = &layerEx;
   quadLayer.header.flags            = ; // XrCompositorLayerFlags
   quadLayer.header.frameIndex       = ; // uint64_t
   quadLayer.header.displayTime      = ; // XrNanoseconds
   quadLayer.header.synthesisCpuTime = ; // XrNanoseconds
   quadLayer.header.synthesisGpuTime = ; // XrNanoseconds
   quadLayer.header.eye              = ; // XrEye
   quadLayer.header.colorScale       = ; // XrVector4f
   quadLayer.header.coordinateSystem = ; //XrCoordinateSystem
   quadLayer.swapchain = swapChain;
   quadLayer.arrayLayer = ; //int32_t
   quadLayer.imageRect  = ; // XrRect2Df
   quadLayer.transform  = ; // XrRigidTransformf
   quadLayer.size       = ; // XrVector2f

   // XR_KHR_compositor_layer_cylinder

   // Extra overlays: Cylinder
   XrCompositorLayerCylinderKHR cylinderLayer;
   // XrCompositorLayerHeader header:
   cylinderLayer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   cylinderLayer.header.next             = &layerEx;
   cylinderLayer.header.flags            = ; // XrCompositorLayerFlags
   cylinderLayer.header.frameIndex       = ; // uint64_t
   cylinderLayer.header.displayTime      = ; // XrNanoseconds
   cylinderLayer.header.synthesisCpuTime = ; // XrNanoseconds
   cylinderLayer.header.synthesisGpuTime = ; // XrNanoseconds
   cylinderLayer.header.eye              = ; // XrEye
   cylinderLayer.header.colorScale       = ; // XrVector4f
   cylinderLayer.header.coordinateSystem = ; //XrCoordinateSystem
   cylinderLayer.swapchain = swapChain;
   cylinderLayer.arrayLayer   = ; //int32_t
   cylinderLayer.imageRect    = ; // XrRect2Df
   cylinderLayer.transform    = ; // XrRigidTransformf
   cylinderLayer.radius       = ; // float
   cylinderLayer.centralAngle = ; // float
   cylinderLayer.aspectRatio  = ; // float

   // XR_KHR_compositor_layer_equirect

   // Extra overlays: Euirect
   XrCompositorLayerEquirectKHR equirectLayer;
   // XrCompositorLayerHeader header:
   equirectLayer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   equirectLayer.header.next             = &layerEx;
   equirectLayer.header.flags            = ; // XrCompositorLayerFlags
   equirectLayer.header.frameIndex       = ; // uint64_t
   equirectLayer.header.displayTime      = ; // XrNanoseconds
   equirectLayer.header.synthesisCpuTime = ; // XrNanoseconds
   equirectLayer.header.synthesisGpuTime = ; // XrNanoseconds
   equirectLayer.header.eye              = ; // XrEye
   equirectLayer.header.colorScale       = ; // XrVector4f
   equirectLayer.header.coordinateSystem = ; //XrCoordinateSystem
   equirectLayer.swapchain = swapChain;
   equirectLayer.arrayLayer   = ; //int32_t
   equirectLayer.imageRect    = ; // XrRect2Df
   equirectLayer.transform    = ; // XrRigidTransformf
   equirectLayer.offset       = ; // XrOffset2Df
   equirectLayer.scale        = ; // XrVector2f
   equirectLayer.bias         = ; // XrVector2f

   // XR_KHR_fixed_dual_projection_layer

   // This seems crippled :/
   XrFixedDualProjectionParamsKHR dualProjInfo;
   //dualProjInfo.viewTransform;         // XrRigidTransformf
   //dualProjInfo.viewTransforOuter;     // XrRigidTransformf
   //dualProjInfo.projectionMatrix;      // XrMatrix4x4f
   //dualProjInfo.projectionMatrixOuter; // XrMatrix4x4f

   result = xrQueryFixedDualProjectionParamsKHR(system,
                                               &dualProjInfo);



   // Has time for which frame was predicted
   XrEndFrameDescription endDesc;
   endDesc.type        = XR_TYPE_END_FRAME_DESCRIPTION;
   endDesc.next        = nullptr;
   endDesc.flags       = 0; // XrEndFrameDescriptionFlags reserved for future.
   endDesc.displayTime = ;  // XrNanoseconds - time for which frame was rendered
   endDesc.layerCount  = 1;
   endDesc.layers      = &layer; // + quadLayer + cylinderLayer

   // Submits current set of layers, but also set of layers for future frames,
   // queued ahead is possible. Mostly for video decoding.
   result = xrEndFrame(session,
                      &endDesc);

*/

void oxrPresentationSession::presentWithPose(const Pose& _headPose)
{
    // Release currently used textures as application is done with them on CPU side
    XrSwapchainImageReleaseInfo releaseInfo;
    releaseInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
    releaseInfo.next = nullptr;

    for(uint32 i=0; i<framesCount; ++i)
    {
        Validate( session, xrReleaseSwapchainImage(colorSwapChain[i], &releaseInfo) )
    }

    if (depthSwapChain[0] != XR_NULL_HANDLE)
    {
        for(uint32 i=0; i<frameTopology.texturesCount; ++i)
        {
            Validate( session, xrReleaseSwapchainImage(depthSwapChain[i], &releaseInfo) )
        }
    }

    // Patch default layer configuration with pose for which this frame was rendered
    for(uint32 i=0; i<frameTopology.viewsCount; ++i)
    {
        // TODO: Reconstruct each View pose in World space, based on provided Head pose and Frame Topology.
        XrPosef viewPose; // XrPosef headPose * headsetTopology.viewLocalPose; (frameTopology.viewsTopology[i].position * frameTopology.viewsTopology[i].orientation)

        // Pose is in world space, to indicate that it should be reprojected by Compositor
        defaultLayerViews[i].pose = viewPose; 
    }

    // Array of pointers to layers that will be composed
    XrCompositionLayerBaseHeader* layersArray[1] = { reinterpret_cast<XrCompositionLayerBaseHeader*>(&defaultLayerInfo) };

    XrFrameEndInfo endInfo;
    endInfo.type                 = XR_TYPE_FRAME_END_INFO;
    endInfo.next                 = nullptr;
    endInfo.displayTime          = currentFrameState.predictedDisplayTime;
    endInfo.environmentBlendMode = selectedBlendMode;
    endInfo.layerCount           = 1; // In future expose ability to merge multiple layers
                                      // Cannot exceed XrSystemGraphicsProperties::maxLayerCount
    endInfo.layers               = layersArray;

    Validate( session, xrEndFrame(presentationSession, &endInfo) )
}

void oxrPresentationSession::endEncoding(void)
{
    // TODO:
}

} // en::xr
} // en