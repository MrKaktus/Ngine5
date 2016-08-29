/*

 Ngine v5.0
 
 Module      : Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkRaster.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/state.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   { 
   static const VkPolygonMode TranslateFillMode[FillModesCount] =
      {
      VK_POLYGON_MODE_POINT,       // Vertices
      VK_POLYGON_MODE_LINE,        // Wireframe
      VK_POLYGON_MODE_FILL         // Solid
      };

   static const VkCullModeFlagBits TranslateCullingMethod[FaceChoosesCount] = 
      {
      VK_CULL_MODE_FRONT_BIT,      // FrontFace
      VK_CULL_MODE_BACK_BIT        // BackFace
      };
   // VK_CULL_MODE_FRONT_AND_BACK_BIT  // BothFaces - Unsupported by D3D12

   static const VkFrontFace TranslateNormalCalculationMethod[NormalCalculationMethodsCount] =
      {
      VK_FRONT_FACE_CLOCKWISE,        // ClockWise
      VK_FRONT_FACE_COUNTER_CLOCKWISE // CounterClockWise
      };

   RasterStateVK::RasterStateVK(const RasterStateInfo& desc)
   {
   // Raster State
   state.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   state.pNext                   = nullptr;
   state.flags                   = 0; // VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT
                                      // VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT
                                      // VK_PIPELINE_CREATE_DERIVATIVE_BIT
   state.depthClampEnable        = desc.enableDepthClamp  ? VK_TRUE : VK_FALSE;
   state.rasterizerDiscardEnable = desc.disableRasterizer ? VK_TRUE : VK_FALSE;  // Disables Rasterizer (for Geometry passes only)
   state.polygonMode             = TranslateFillMode[desc.fillMode];             // optional (GL45)
   state.cullMode                = desc.enableCulling     ? TranslateCullingMethod[desc.cullFace] : VK_CULL_MODE_NONE;
   state.frontFace               = TranslateNormalCalculationMethod[desc.frontFace];
   state.depthBiasEnable         = desc.enableDepthBias   ? VK_TRUE : VK_FALSE;
   state.depthBiasConstantFactor = desc.depthBiasConstantFactor;             
   state.depthBiasClamp          = desc.depthBiasClamp;        
   state.depthBiasSlopeFactor    = desc.depthBiasSlopeFactor;  
   state.lineWidth               = desc.lineWidth;

   // Pass through state
   conservativeRaster = desc.enableConservativeRasterization;

   //rasterStateInfo.pointSize            = raster.pointSize;            // Point Fade Threshold - max value to which point size is clamped (deprecated in 4.2?)
   //rasterStateInfo.pointFadeThreshold   = raster.pointFadeThreshold;   // https://www.opengl.org/registry/specs/ARB/point_sprite.txt
   //                                                                    // https://www.opengl.org/registry/specs/ARB/point_parameters.txt
   }

   Ptr<RasterState> VulkanDevice::create(const RasterStateInfo& state)
   {
   return ptr_dynamic_cast<RasterState, RasterStateVK>(Ptr<RasterStateVK>(new RasterStateVK(state)));
   }

   }
}
#endif
