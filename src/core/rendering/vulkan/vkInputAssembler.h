/*

 Ngine v5.0
 
 Module      : Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_INPUT_ASSEMBLER
#define ENG_CORE_RENDERING_VULKAN_INPUT_ASSEMBLER

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/common/inputAssembler.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   class InputAssemblerVK : public InputAssembler
      {
      public:
      VkPipelineVertexInputStateCreateInfo   state;
      VkPipelineInputAssemblyStateCreateInfo statePrimitive;
      VkPipelineTessellationStateCreateInfo  stateTessellator;

      InputAssemblerVK(const DrawableType primitiveType,
                       const uint32 controlPoints, 
                       const uint32 usedAttributes, 
                       const uint32 usedBuffers, 
                       const AttributeDesc* attributes,  
                       const BufferDesc* buffers);

      virtual ~InputAssemblerVK();
      };

   }
}
#endif

#endif