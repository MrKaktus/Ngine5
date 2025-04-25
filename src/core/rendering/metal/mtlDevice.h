/*

 Ngine v5.0
 
 Module      : Metal GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_DEVICE
#define ENG_CORE_RENDERING_METAL_DEVICE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include <string>
#include "core/rendering/metal/metal.h"

#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/metal/mtlTexture.h"

#include "core/rendering/common/device.h"
#include "core/rendering/common/inputLayout.h"


#import <AppKit/AppKit.h>

namespace en
{
namespace gpu
{

class MetalDevice : public CommonDevice
{
    public:
    id<MTLDevice> device;
    id<MTLCommandQueue> queue;
    
    MetalDevice(id<MTLDevice> handle);
   ~MetalDevice();

    // Internal
    
    void init(void);
    
    // Interface

    uint32 displays(void) const;            // Screens count the device can render to
    std::shared_ptr<Display> display(const uint32 id) const;  // Return N'th screen handle
    
    virtual Window* createWindow(
        const WindowSettings& settings,
        const std::string title); // Create window
    
    virtual Heap* createHeap(
        const MemoryUsage usage, 
        const uint32 size);

    virtual Sampler* createSampler(
        const SamplerState& state);
    
    virtual std::shared_ptr<Texture> createSharedTexture(
        std::shared_ptr<SharedSurface> backingSurface);
    
    virtual std::shared_ptr<Shader>  createShader(
        const ShaderStage stage,
        const std::string& source);

    virtual std::shared_ptr<Shader>  createShader(
        const ShaderStage stage,
        const uint8* data,
        const uint32 size);
        
    virtual uint32 queues(const QueueType type) const;
    
    virtual std::shared_ptr<CommandBuffer> createCommandBuffer(
        const QueueType type = QueueType::Universal,
        const uint32 parentQueue = 0u);

    virtual InputLayout* createInputLayout(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const uint32 usedAttributes,
        const uint32 usedBuffers,
        const AttributeDesc* attributes,
        const BufferDesc* buffers);

    virtual SetLayout* createSetLayout(
        const uint32 count, 
        const ResourceGroup* group,
        const ShaderStages stagesMask = ShaderStages::All);

    virtual PipelineLayout* createPipelineLayout(
        const uint32      setsCount,
        const SetLayout** sets,
        const uint32      immutableSamplersCount = 0u,
        const Sampler**   immutableSamplers = nullptr,
        const ShaderStages stagesMask = ShaderStages::All);

    virtual Descriptors* createDescriptorsPool(
        const uint32 maxSets,
        const uint32 (&count)[underlyingType(ResourceType::Count)]);
        


    virtual ColorAttachment* createColorAttachment(
        const Format format, 
        const uint32 samples = 1u);

    virtual DepthStencilAttachment* createDepthStencilAttachment(
        const Format depthFormat, 
        const Format stencilFormat = Format::Unsupported,
        const uint32 samples = 1u);

    virtual RenderPass* createRenderPass(
        const ColorAttachment& swapChainSurface,
        const DepthStencilAttachment* depthStencil = nullptr);

    virtual RenderPass* createRenderPass(
        const uint32 attachments,
        const ColorAttachment* color[] = nullptr,
        const DepthStencilAttachment* depthStencil = nullptr);


    virtual std::shared_ptr<Semaphore> createSemaphore(void);

     



// When binding 3D texture, pass it's plane "depth" through "layer" parameter,
// similarly when binding CubeMap texture, pass it's "face" through "layer".





    virtual RasterState* createRasterState(
        const RasterStateInfo& state);

    virtual MultisamplingState* createMultisamplingState(
        const uint32 samples,
        const bool enableAlphaToCoverage,
        const bool enableAlphaToOne);

    virtual DepthStencilState* createDepthStencilState(
        const DepthStencilStateInfo& desc);
     
    virtual BlendState* createBlendState(
        const BlendStateInfo& state,
        const uint32 attachments,
        const BlendAttachmentInfo* color);
     
    virtual ViewportState* createViewportState(
        const uint32 count,
        const ViewportStateInfo* viewports,
        const ScissorStateInfo* scissors);
        
    virtual Pipeline* createPipeline(
        const PipelineState& pipelineState);

    virtual ImageMemoryAlignment textureMemoryAlignment(
        const TextureState& state,
        const uint32 mipmap,
        const uint32 layer) const;
};

} // en::gpu
} // en

#endif
#endif
