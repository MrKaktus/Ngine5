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
#include "core/rendering/device.h"

#include "core/rendering/inputAssembler.h"
#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"

using namespace std;

namespace en
{
   namespace gpu
   {
   class MetalDevice : public GpuDevice
      {
      public:
      MetalDevice();
     ~MetalDevice();

      virtual Ptr<BlendState> create(const BlendStateInfo& state,
                                     const uint32 attachments,
                                     const BlendAttachmentInfo* color);
         
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