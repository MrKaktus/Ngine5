/*

 Ngine v5.0
 
 Module      : Vulkan Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkBlend.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/state.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   {
   // Optimisation: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkBlendFactor TranslateBlend[BlendFunctionsCount] =
      {
      VK_BLEND_FACTOR_ZERO,                     // Zero
      VK_BLEND_FACTOR_ONE,                      // One
      VK_BLEND_FACTOR_SRC_COLOR,                // Source
      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,      // OneMinusSource
      VK_BLEND_FACTOR_DST_COLOR,                // Destination
      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,      // OneMinusDestination
      VK_BLEND_FACTOR_SRC_ALPHA,                // SourceAlpha
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,      // OneMinusSourceAlpha
      VK_BLEND_FACTOR_DST_ALPHA,                // DestinationAlpha
      VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,      // OneMinusDestinationAlpha
      VK_BLEND_FACTOR_CONSTANT_COLOR,           // ConstantColor
      VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR, // OneMinusConstantColor
      VK_BLEND_FACTOR_CONSTANT_ALPHA,           // ConstantAlpha
      VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA, // OneMinusConstantAlpha
      VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,       // SourceAlphaSaturate
      VK_BLEND_FACTOR_SRC1_COLOR,               // SecondSource
      VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,     // OneMinusSecondSource
      VK_BLEND_FACTOR_SRC1_ALPHA,               // SecondSourceAlpha
      VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,     // OneMinusSecondSourceAlpha
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkBlendOp TranslateBlendFunc[BlendEquationsCount] =
      {
      VK_BLEND_OP_ADD,                          // Add
      VK_BLEND_OP_SUBTRACT,                     // Subtract
      VK_BLEND_OP_REVERSE_SUBTRACT,             // DestinationMinusSource
      VK_BLEND_OP_MIN,                          // Min
      VK_BLEND_OP_MAX                           // Max
      };

   // Logical Operation is alternative to blending (currently not supported)
   static const VkLogicOp TranslateLogicOperation[LogicOperationsCount] = 
      {
      VK_LOGIC_OP_CLEAR,                        // Clear
      VK_LOGIC_OP_SET,                          // Set
      VK_LOGIC_OP_COPY,                         // Copy
      VK_LOGIC_OP_COPY_INVERTED,                // CopyInverted
      VK_LOGIC_OP_NO_OP,                        // NoOperation
      VK_LOGIC_OP_INVERT,                       // Invert
      VK_LOGIC_OP_AND,                          // And
      VK_LOGIC_OP_NAND,                         // NAnd
      VK_LOGIC_OP_OR,                           // Or
      VK_LOGIC_OP_NOR,                          // Nor
      VK_LOGIC_OP_XOR,                          // Xor
      VK_LOGIC_OP_EQUIVALENT,                   // Equiv
      VK_LOGIC_OP_AND_REVERSE,                  // AndReverse
      VK_LOGIC_OP_AND_INVERTED,                 // AndInverted
      VK_LOGIC_OP_OR_REVERSE,                   // OrReverse
      VK_LOGIC_OP_OR_INVERTED                   // OrInverted
      };

   BlendStateVK::BlendStateVK(const BlendStateInfo& desc,
                              const uint32 attachments, 
                              const BlendAttachmentInfo* color)
   {
   state.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   state.pNext             = nullptr;
   state.logicOpEnable     = VK_FALSE;          // Logic Operations are not supported
   state.logicOp           = VK_LOGIC_OP_NO_OP; // TranslateLogicOperation[color[0].logic];
   state.attachmentCount   = min(attachments, MaxColorAttachmentsCount);
   state.pAttachments      = attachments ? &blendInfo[0] : nullptr;
   state.blendConstants[0] = desc.blendColor.r;
   state.blendConstants[1] = desc.blendColor.g;
   state.blendConstants[2] = desc.blendColor.b;
   state.blendConstants[3] = desc.blendColor.a;

   for(uint32 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
      {
      //assert( !(color[0].logicOperation && color[i].blending) );
      blendInfo[i].blendEnable         = color[i].blending;
      blendInfo[i].srcColorBlendFactor = static_cast<VkBlendFactor>(underlyingType(color[i].srcRGB));   // Optimisation: TranslateBlend[color[i].srcRGB];
      blendInfo[i].dstColorBlendFactor = static_cast<VkBlendFactor>(underlyingType(color[i].dstRGB));   // Optimisation: TranslateBlend[color[i].dstRGB];
      blendInfo[i].colorBlendOp        = static_cast<VkBlendOp>(underlyingType(color[i].rgbFunc));      // Optimisation: TranslateBlendFunc[color[i].rgbFunc];
      blendInfo[i].srcAlphaBlendFactor = static_cast<VkBlendFactor>(underlyingType(color[i].srcAlpha)); // Optimisation: TranslateBlend[color[i].srcAlpha];
      blendInfo[i].dstAlphaBlendFactor = static_cast<VkBlendFactor>(underlyingType(color[i].dstAlpha)); // Optimisation: TranslateBlend[color[i].dstAlpha];
      blendInfo[i].alphaBlendOp        = static_cast<VkBlendOp>(underlyingType(color[i].alphaFunc));    // Optimisation: TranslateBlendFunc[color[i].alphaFunc];
      // Translate Color Write Mask
      blendInfo[i].colorWriteMask      = color[i].writeMask;
      // Optimisation:
      //if (color[i].writeMask & ColorMaskRed)           
      //   blendInfo[i].colorWriteMask  |= VK_COLOR_COMPONENT_R_BIT;
      //if (color[i].writeMask & ColorMaskGreen)
      //   blendInfo[i].colorWriteMask  |= VK_COLOR_COMPONENT_G_BIT;
      //if (color[i].writeMask & ColorMaskBlue)
      //   blendInfo[i].colorWriteMask  |= VK_COLOR_COMPONENT_B_BIT;
      //if (color[i].writeMask & ColorMaskAlpha)
      //   blendInfo[i].colorWriteMask  |= VK_COLOR_COMPONENT_A_BIT;
      }
   }

   Ptr<BlendState> VulkanDevice::createBlendState(const BlendStateInfo& state,
                                                  const uint32 attachments,
                                                  const BlendAttachmentInfo* color)
   {
   return ptr_reinterpret_cast<BlendState>(&Ptr<BlendStateVK>(new BlendStateVK(state, attachments, color)));
   }

   }
}
#endif
