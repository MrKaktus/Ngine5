/*

 Ngine v5.0
 
 Module      : Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkMultisampling.h"

namespace en
{
   namespace gpu
   { 
   MultisamplingStateVK::MultisamplingStateVK(const uint32 samples,
                                              const bool enableAplhaToCoverage,
                                              const bool enableAlphaToOne)
   {
   // Multisampling state (optional)
   state.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   state.pNext                 = nullptr;
   state.flags                 = 0;        // VkPipelineMultisampleStateCreateFlags
   state.sampleShadingEnable   = VK_FALSE; // optional (GL45) https://www.opengl.org/registry/specs/ARB/sample_shading.txt
   state.minSampleShading      = 0.0f;     // optional (GL45)
   state.pSampleMask           = nullptr;  // Coverage of all samples is enabled (uint32*[]) 
   state.alphaToCoverageEnable = enableAplhaToCoverage ? VK_TRUE : VK_FALSE;
   state.alphaToOneEnable      = enableAlphaToOne      ? VK_TRUE : VK_FALSE;

   switch(nextPowerOfTwo(samples))
      {
      case 2:
         state.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT;
         break;

      case 4:
         state.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
         break;

      case 8:
         state.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT;
         break;

      case 16:
         state.rasterizationSamples = VK_SAMPLE_COUNT_16_BIT;
         break;

      case 32:
         state.rasterizationSamples = VK_SAMPLE_COUNT_32_BIT;
         break;

      case 64:
         state.rasterizationSamples = VK_SAMPLE_COUNT_64_BIT;
         break;

      case 1:
      default:
         state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      };
   }

   }
}