/*

 Ngine v5.0

 Module      : OpenXR Presentation Session
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_PRESENTATION_SESSION
#define ENG_CORE_XR_OPENXR_PRESENTATION_SESSION

#include "core/xr/presentationSession.h"
#include "core/xr/openxr/oxrFrame.h"
#include "core/xr/openxr/oxrHeadset.h"

#include "OpenXR/openxr.h"

// Maximum count of textures needed to compose single frame (2 for Dedicated layout)
#define MaxTexturesPerFrame 2

namespace en
{
namespace xr
{



struct TextureTopology
{
    gpu::TextureType textureType;
	uint16 width;
	uint16 height;
	uint32 layers  : 12;
    uint32 samples : 8;
	uint32 mipmaps : 5;
	uint32         : 7;
};

struct ViewTopology
{
    double position[3];
    float  orientation[4];
    float  tangent[4];
    float  depthRange[2];
    uint32 surface;
    uint32 slice;
    uint32v2 viewportTexelsOrigin; // Viewport origin in texels
    uint32 viewportTexelsWidth;  // Viewport width in texels
    uint32 viewportTexelsHeight; // Viewport height in texels
    float2 viewportOrigin;       // Viewport origin in normalized coordinates
    float  viewportWidth;        // Viewport width in normalized coordinates
    float  viewportHeight;       // Viewport height in normalized coordinates 
};

#define MaxFrameTextures 4
#define MaxFrameViews    4

struct FrameTopology
{
    uint32          viewsCount;     // Count of separate views that need to be rendered for each frame
    uint32          texturesCount;  // Count of textures of given type in frame (<= MaxFrameTextures)
    TextureTopology texturesTopology[MaxFrameTextures];
    ViewTopology    viewsTopology[MaxFrameViews];
};

XrSwapchainUsageFlags oxrTranslateTextureUsage(const gpu::TextureUsage usage);

class oxrInterface;

class oxrPresentationSession : public PresentationSession
{
    public:
    oxrInterface* session; // XR Session handle
//oxrHeadset*   headset;

    XrSession                   presentationSession; // Presentation Session handle

    uint32                      supportedFormatsCount; // List of supported texel formats (of backing graphics API)
    sint64*                     supportedFormats;

    uint64                      renderedFrameIndex;
    XrFrameState                currentFrameState;

    // Frames Pool
    FrameTopology               frameTopology;
    uint32                      framesCount;    // Should be the same across all Swap-Chains
    XrSwapchain                 colorSwapChain[MaxTexturesPerFrame]; 
    XrSwapchain                 depthSwapChain[MaxTexturesPerFrame];
    XrSwapchainImageBaseHeader* colorFramesPool[MaxTexturesPerFrame];
    XrSwapchainImageBaseHeader* depthFramesPool[MaxTexturesPerFrame];
    oxrFrame*                   framesPool;

    XrEnvironmentBlendMode            selectedBlendMode;

    // Default Layer
    XrCompositionLayerProjection      defaultLayerInfo;
    XrCompositionLayerProjectionView* defaultLayerViews;
    XrCompositionLayerDepthInfoKHR*   defaultLayerDepth; 



    oxrPresentationSession(
        const XrSystemId runtime,
        const XrViewConfigurationType viewConfigurationType, 
        const PresentationSessionDescriptor& descriptor,
        const gpu::GpuDevice* gpu,
        const gpu::QueueType type, 
        const uint32 queue);

    virtual ~oxrPresentationSession();

    // Implemented by graphic API specialization classes

    virtual void createPresentationSessionWithGraphicBinding(
        const XrSystemId runtime,
        const gpu::GpuDevice* gpu,
        const gpu::QueueType type, 
        const uint32 queue);

    virtual uint64 translateTextureFormat(const gpu::Format format);
    virtual void   extendColorSwapChainInfo(XrSwapchainCreateInfo& info);
    virtual XrSwapchainImageBaseHeader* allocateSwapChainImagesArray(uint32 framesCount);
    virtual gpu::Texture* createTextureBackedBySwapChainImage(const gpu::GpuDevice* gpu, gpu::TextureState& textureState, XrSwapchainImageBaseHeader* imageDesc);



    // Public API's

    virtual void   queryNextEvent(void);

    virtual uint64 startFrame(void);
    virtual FrameState* waitForOptimalPrediction(void);
    virtual void   startEncoding(void);
    virtual Frame* currentFrame(void);
    virtual void   presentWithPose(const Pose& pose);
    virtual void   endEncoding(void);
};



} // en::xr
} // en
#endif