/*

 Ngine v5.0
 
 Module      : Common Raster.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/raster.h"

namespace en
{
   namespace gpu
   {
   // Default Rasterizer state:
   //
   // (D) - state is dynamic, set on Command Buffer
   // (S) - state is present only in Shading Language
   //  X  - doesn't exist in given API
   //
   //                                   Direct3D12:            |     Vulkan:        |   Metal:                   | OpenGL:
   //                               ---------------------------+--------------------+----------------------------+------------------------
   // enableCulling                   -    true                |     no defaults    |     false                  | (D) false
   // enableDepthBias                 -    false               |                    |     false                  | (D) false
   // enableDepthClamp                -    false (clip - true) |                    | (D)[1] false (clip - true) | (D) false (clip - true)
   // enableConservativeRasterization -    false               |                    |  X                         | (D) false (NV extension)
   // disableRasterizer               - ?                      |                    |     false                  | (D) false
   // fillMode                        -    Solid               |                    | (D) Solid                  | (D) Solid
   // cullFace                        -    BackFace            |                    | (D) disabled               | (D) BackFace
   // frontFace                       -    ClockWise           |                    | (D) ClockWise              | (D) CounterClockWise
   // depthBiasConstantFactor         -    0                   |                    | (D) 0.0f                   | (D) 0
   // depthBiasClamp                  -    0.0f                |                    | (D) 0.0f                   |  
   // depthBiasSlopeFactor            -    0.0f                |                    | (D) 0.0f                   |     0
   // pointSize                       - X?                     |                    | (S) [[point_size]]         |
   //                                                          |                    |     (0.125 to 255.0)       |
   // pointFadeThreshold              - X?                     |                    |  X?                        | Deprecated in 4.2 ?
   // lineWidth                       - X                      |                    |  X                         | Deprecated 
   //                                                          |                    |                            |
   // Direct3D12 specyfic:                                     |                    |                            |
   //                                                          |                    |                            |
   // BOOL MultisampleEnable          -    FALSE               |                    |                            |
   // BOOL AntialiasedLineEnable      -    FALSE               |                    |                            |
   // UINT ForcedSampleCount          -    0                   |                    |                            |
   //
   // [1] Metal depth clip mode is available on iOS 9.0+







   // D3D12 Defaults:
   //
   // FillMode                  Solid
   // CullMode                  Back
   // FrontCounterClockwise     FALSE   (D3D is ClockWise by default)
   // DepthBias                 0
   // SlopeScaledDepthBias      0.0f
   // DepthBiasClamp            0.0f
   // DepthClipEnable           TRUE
   // MultisampleEnable         FALSE
   // AntialiasedLineEnable     FALSE
   // ForcedSampleCount         0
   // ConservativeRaster        D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF

   // Metal Defaults:
   //
   // MTLTriangleFillMode       MTLTriangleFillModeFill
   // MTLCullMode               MTLCullModeNone         (Culling is Disabled)
   // MTLWinding                MTLWindingClockWise     (ClockWise by default)
   // depthBiasConstantFactor   0.0f
   // depthBiasSlopeFactor      0.0f
   // depthBiasClamp            0.0f
   // MTLDepthClipMode          MTLDepthClipModeClip

   // OpenGL defaults:
   //
   // glDisable(GL_CULL_FACE);
   // glDisable(GL_POLYGON_OFFSET_FILL); - depth Bias / polygon offset
   // glDisable(GL_DEPTH_CLAMP);
   // glDisable(GL_RASTERIZER_DISCARD);
   // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   // glCullFace(GL_BACK);
   // glFrontFace(GL_CCW);
   //    glPolygonOffset(float scale - 0,float units - 0); 
   //       scale - depthBiasSlopeFactor  
   //       units - depthBiasConstantFactor ?

   RasterStateInfo::RasterStateInfo() :
      enableCulling(false),
      enableDepthBias(false),
      enableDepthClamp(false), // Depth Clip by default
      enableConservativeRasterization(false),
      disableRasterizer(false),
      fillMode(Solid),
      cullFace(BackFace),
      frontFace(CounterClockWise),   // TODO: Change to CW
      depthBiasConstantFactor(0.0f),
      depthBiasClamp(0.0f),
      depthBiasSlopeFactor(0.0f),
    //pointSize(1.0f),
    //pointFadeThreshold(1.0f),
      lineWidth(1.0f)
   {
   }

   }
}
