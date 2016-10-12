/*

 Ngine v5.0
 
 Module      : Metal Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlRaster.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   static const MTLTriangleFillMode TranslateFillMode[FillModesCount] =
      {
      MTLTriangleFillModeLines,    // Vertices (unsupported)
      MTLTriangleFillModeLines,    // Wireframe
      MTLTriangleFillModeFill      // Solid
      };

   static const MTLWinding TranslateNormalCalculationMethod[NormalCalculationMethodsCount] =
      {
      MTLWindingClockwise,         // ClockWise
      MTLWindingCounterClockwise   // CounterClockWise
      };
        
   static const MTLCullMode TranslateCullingMethod[FaceChoosesCount] =
      {      
      MTLCullModeFront,            // FrontFace
      MTLCullModeBack,             // BackFace
      MTLCullModeFront             // BothFaces (unsupported)
      };
      
   RasterStateMTL::RasterStateMTL(const RasterStateInfo& desc) :
      culling(desc.enableCulling ? TranslateCullingMethod[desc.cullFace] : MTLCullModeNone),
      frontFace(TranslateNormalCalculationMethod[desc.frontFace]),
      fillMode(TranslateFillMode[desc.fillMode]),
      depthClamp(desc.enableDepthClamp ? MTLDepthClipModeClamp : MTLDepthClipModeClip), // Default is Clipping, Clamping needs to be enabled.
      depthBiasConstantFactor(desc.depthBiasConstantFactor),
      depthBiasClamp(desc.depthBiasClamp),
      depthBiasSlopeFactor(desc.depthBiasSlopeFactor),
      enableRasterization(!desc.disableRasterizer)
   {
   // Metal is not supporting:
   // - Conservative Rasterization
   // - lineWidth
   // - pointFadeThreshold
   //
   // Metal supports in Shading Language:
   // - pointSize
   }
   
   RasterStateMTL::~RasterStateMTL()
   {
   }
   
   Ptr<RasterState> MetalDevice::createRasterState(const RasterStateInfo& state)
   {
   return ptr_dynamic_cast<RasterState, RasterStateMTL>(new RasterStateMTL(state));
   }
   
   }
}
#endif
