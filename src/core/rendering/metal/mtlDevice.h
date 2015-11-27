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

// TODO: Here platform specific bindings

#include "core/rendering/metal/mtlInputAssembler.h"
#include "core/rendering/metal/mtlBlend.h"
//#include "core/rendering/metal/mtlRaster.h"
//#include "core/rendering/metal/mtlMultisampling.h"
#include "core/rendering/metal/mtlViewport.h"
#include "core/rendering/metal/mtlDepthStencil.h"
//#include "core/rendering/metal/mtlPipeline.h"

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