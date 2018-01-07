/*

 Ngine v5.0
 
 Module      : Vulkan Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkRaster.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

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

   static const VkCullModeFlagBits TranslateCullingMethod[underlyingType(Face::Count)] = 
      {
      VK_CULL_MODE_FRONT_BIT,      // Front
      VK_CULL_MODE_BACK_BIT        // Back
      };
   // VK_CULL_MODE_FRONT_AND_BACK_BIT  // BothFaces - Unsupported by D3D12 & Metal

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
   state.cullMode                = desc.enableCulling     ? TranslateCullingMethod[underlyingType(desc.cullFace)] : VK_CULL_MODE_NONE;
   state.frontFace               = TranslateNormalCalculationMethod[desc.frontFace];
   state.depthBiasEnable         = desc.enableDepthBias   ? VK_TRUE : VK_FALSE;
   state.depthBiasConstantFactor = desc.depthBiasConstantFactor;             
   state.depthBiasClamp          = desc.depthBiasClamp;        
   state.depthBiasSlopeFactor    = desc.depthBiasSlopeFactor;  
   state.lineWidth               = 1.0f; //desc.lineWidth; (unsupported)

   // Pass through state (unsupported)
   // conservativeRaster = desc.enableConservativeRasterization;
   }

   shared_ptr<RasterState> VulkanDevice::createRasterState(const RasterStateInfo& state)
   {
   return make_shared<RasterStateVK>(state);
   }

   }
}
#endif
