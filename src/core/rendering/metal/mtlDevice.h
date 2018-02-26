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

using namespace std;

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
      shared_ptr<Display> display(const uint32 id) const;  // Return N'th screen handle
      
      virtual shared_ptr<Window>  createWindow(const WindowSettings& settings,
                                               const string title); // Create window
      
      virtual shared_ptr<Heap>    createHeap(const MemoryUsage usage, const uint32 size);

      virtual shared_ptr<Sampler> createSampler(const SamplerState& state);
      
      virtual shared_ptr<Texture> createSharedTexture(shared_ptr<SharedSurface> backingSurface);
      
      virtual shared_ptr<Shader>  createShader(const ShaderStage stage,
                                               const string& source);

      virtual shared_ptr<Shader>  createShader(const ShaderStage stage,
                                               const uint8* data,
                                               const uint64 size);
         
      virtual uint32 queues(const QueueType type) const;
      
      virtual shared_ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                            const uint32 parentQueue = 0u);

      virtual shared_ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                        const bool primitiveRestart,
                                                        const uint32 controlPoints,
                                                        const uint32 usedAttributes,
                                                        const uint32 usedBuffers,
                                                        const AttributeDesc* attributes,
                                                        const BufferDesc* buffers);
 
      virtual shared_ptr<SetLayout> createSetLayout(const uint32 count, 
                                                    const ResourceGroup* group,
                                                    const ShaderStages stagesMask = ShaderStages::All);

      virtual shared_ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                              const shared_ptr<SetLayout>* set,
                                                              const uint32 immutableSamplers = 0u,
                                                              const shared_ptr<Sampler>* sampler = nullptr,
                                                              const ShaderStages stagesMask = ShaderStages::All);

      virtual shared_ptr<Descriptors> createDescriptorsPool(const uint32 maxSets,
                                                            const uint32 (&count)[underlyingType(ResourceType::Count)]);
         


      virtual shared_ptr<ColorAttachment> createColorAttachment(const Format format, 
                                                                const uint32 samples = 1u);

      virtual shared_ptr<DepthStencilAttachment> createDepthStencilAttachment(const Format depthFormat, 
                                                                              const Format stencilFormat = Format::Unsupported,
                                                                              const uint32 samples = 1u);

      virtual shared_ptr<RenderPass> createRenderPass(
         const ColorAttachment& swapChainSurface,
         const DepthStencilAttachment* depthStencil = nullptr);

      virtual shared_ptr<RenderPass> createRenderPass(
         const uint32 attachments,
         const shared_ptr<ColorAttachment> color[] = nullptr,
         const DepthStencilAttachment* depthStencil = nullptr);


      virtual shared_ptr<Semaphore> createSemaphore(void);

      



   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
   // similarly when binding CubeMap texture, pass it's "face" through "layer".





      virtual shared_ptr<RasterState>        createRasterState(const RasterStateInfo& state);

      virtual shared_ptr<MultisamplingState> createMultisamplingState(const uint32 samples,
                                                                      const bool enableAlphaToCoverage,
                                                                      const bool enableAlphaToOne);

      virtual shared_ptr<DepthStencilState>  createDepthStencilState(const DepthStencilStateInfo& desc);
      
      virtual shared_ptr<BlendState>         createBlendState(const BlendStateInfo& state,
                                                              const uint32 attachments,
                                                              const BlendAttachmentInfo* color);
      
      virtual shared_ptr<ViewportState>      createViewportState(const uint32 count,
                                                                 const ViewportStateInfo* viewports,
                                                                 const ScissorStateInfo* scissors);
         
      virtual shared_ptr<Pipeline> createPipeline(const PipelineState& pipelineState);

      virtual LinearAlignment textureLinearAlignment(const Texture& texture, 
                                                     const uint32 mipmap, 
                                                     const uint32 layer);
      };
   }
}
#endif

#endif
