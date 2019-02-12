/*

 Ngine v5.0
 
 Module      : Vulkan Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkDepthStencil.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/state.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   {
   // Optimization: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkCompareOp TranslateCompareFunction[CompareMethodsCount] = 
      {
      VK_COMPARE_OP_NEVER,                 // Never
      VK_COMPARE_OP_LESS,                  // Less
      VK_COMPARE_OP_EQUAL,                 // Equal
      VK_COMPARE_OP_LESS_OR_EQUAL,         // LessOrEqual
      VK_COMPARE_OP_GREATER,               // Greater
      VK_COMPARE_OP_NOT_EQUAL,             // NotEqual
      VK_COMPARE_OP_GREATER_OR_EQUAL,      // GreaterOrEqual
      VK_COMPARE_OP_ALWAYS,                // Always
      };
   
   // Optimization: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkStencilOp TranslateStencilOperation[underlyingType(StencilOperation::Count)] =
      {
      VK_STENCIL_OP_KEEP,                  // Keep
      VK_STENCIL_OP_ZERO,                  // Clear
      VK_STENCIL_OP_REPLACE,               // Reference
      VK_STENCIL_OP_INCREMENT_AND_CLAMP,   // Increase
      VK_STENCIL_OP_DECREMENT_AND_CLAMP,   // Decrease
      VK_STENCIL_OP_INVERT,                // InvertBits
      VK_STENCIL_OP_INCREMENT_AND_WRAP,    // IncreaseWrap
      VK_STENCIL_OP_DECREMENT_AND_WRAP,    // DecreaseWrap
      };

   DepthStencilStateVK::DepthStencilStateVK(const DepthStencilStateInfo& desc)
   {
   state.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   state.pNext                 = nullptr; 
   state.flags                 = 0u;
   state.depthTestEnable       = desc.enableDepthTest   ? VK_TRUE : VK_FALSE;
   state.depthWriteEnable      = desc.enableDepthWrite  ? VK_TRUE : VK_FALSE;
   state.depthCompareOp        = TranslateCompareFunction[desc.depthFunction];
   state.depthBoundsTestEnable = desc.enableDepthBounds ? VK_TRUE : VK_FALSE;
   state.stencilTestEnable     = desc.enableStencilTest ? VK_TRUE : VK_FALSE;
   state.minDepthBounds        = desc.depthRange.x; // optional (depth_bounds_test)
   state.maxDepthBounds        = desc.depthRange.y; // optional (depth_bounds_test)
   for(uint8 i=0; i<2; ++i)
      {
      VkStencilOpState& stencil = (i == 0) ? state.front : state.back;

      stencil.failOp           = static_cast<VkStencilOp>(underlyingType(desc.stencil[i].whenStencilFails));  // Optimisation: TranslateStencilOperation[desc.stencil[i].whenStencilFails];
      stencil.depthFailOp      = static_cast<VkStencilOp>(underlyingType(desc.stencil[i].whenDepthFails));    // Optimisation: TranslateStencilOperation[desc.stencil[i].whenDepthFails];
      stencil.passOp           = static_cast<VkStencilOp>(underlyingType(desc.stencil[i].whenBothPass));      // Optimisation: TranslateStencilOperation[desc.stencil[i].whenBothPass];
      stencil.compareOp        = static_cast<VkCompareOp>(underlyingType(desc.stencil[i].function));          // Optimisation: TranslateCompareFunction[desc.stencil[i].function];
      stencil.compareMask      = desc.stencil[i].readMask;
      stencil.writeMask        = desc.stencil[i].writeMask;
      stencil.reference        = desc.stencil[i].reference;
      }
   }

   DepthStencilState* VulkanDevice::createDepthStencilState(const DepthStencilStateInfo& desc)
   {
   return new DepthStencilStateVK(desc);
   }

   }
}
#endif
