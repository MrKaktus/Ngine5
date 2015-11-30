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

#include <string>
#include "core/rendering/metal/metal.h"
#include "core/rendering/common/device.h"

#include "core/rendering/inputAssembler.h"
#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/renderPass.h"

using namespace std;

namespace en
{
   namespace gpu
   {
   class MetalDevice : public CommonDevice
      {
      public:
      id<MTLDevice> device;
      
      MetalDevice();
     ~MetalDevice();

      virtual Ptr<RasterState>     create(const RasterStateInfo& state);
      virtual Ptr<BlendState>      create(const BlendStateInfo& state,
                                          const uint32 attachments,
                                          const BlendAttachmentInfo* color);
      virtual Ptr<ColorAttachment> create(const TextureFormat format = FormatUnsupported,
                                          const uint32 samples = 1);
      virtual Ptr<RenderPass>      create(const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                          const Ptr<DepthStencilAttachment> depthStencil);



   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
   // similarly when binding CubeMap texture, pass it's "face" through "layer".
      virtual Ptr<ColorAttachment> create(const Ptr<Texture> texture,
                                          const TextureFormat format,
                                          const uint32 mipmap = 0,
                                          const uint32 layer = 0,
                                          const uint32 layers = 1);

      virtual Ptr<DepthStencilAttachment> create(const Ptr<Texture> texture,
                                                 const TextureFormat format,
                                                 const uint32 mipmap = 0,
                                                 const uint32 layer = 0,
                                                 const uint32 layers = 1);


      //Ptr<Pipeline> create(const Ptr<InputAssembler> inputAssembler,
      //                     const Ptr<ViewportState>  viewportState,
      //                     const Ptr<RasterState>    rasterState,
      //                     const Ptr<MultisamplingState> multisamplingState,                        
      //                     const Ptr<DepthStencilState> depthStencilState,
      //                     const Ptr<BlendState>     blendState,
      //                     const Ptr<PipelineLayout> pipelineLayout);
      };

   class MetalAPI : public GraphicAPI
      {
      public:
      MetalAPI(string appName);
     ~MetalAPI();
      };
   }
}

#endif