/*

 Ngine v5.0
 
 Module      : Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkPipeline.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {





 
   typedef enum VkShaderStageFlagBits {
    VK_SHADER_STAGE_VERTEX_BIT = 0x00000001,
    VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
    VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
    VK_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
    VK_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
    VK_SHADER_STAGE_COMPUTE_BIT = 0x00000020,
    VK_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
    VK_SHADER_STAGE_ALL = 0x7FFFFFFF,


   {
   Ptr<SetLayout> result = nullptr;
   
   VK_DESCRIPTOR_TYPE_SAMPLER = 0,
   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
   VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
   VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
   VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
   VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
   VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
   
   VkDescriptorSetLayoutBinding setBinding;
   setBinding.binding = i; // Binding the same as in Shader
   setBinding.descriptorType = ;// VkDescriptorType
   setBinding.descriptorCount = ; // uint32_t
VkShaderStageFlags stageFlags;
const VkSampler* pImmutableSamplers;
   
   VkDescriptorSetLayoutCreateInfo setInfo;
   setInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   setInfo.pNext        = nullptr;
   setInfo.flags        = 0; // Reserved for future
   setInfo.bindingCount = 0;       // uint32
   setInfo.pBindings    = nullptr; // const VkDescriptorSetLayoutBinding*

   VkDescriptorSetLayout setLayout;
      
   Profile( gpu, vkCreateDescriptorSetLayout(gpu->device, &setInfo, nullptr, &setLayout) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = ptr_dynamic_cast<SetLayout, SetLayoutVK>(new SetLayoutVK(gpu, setLayout));
      }
      
   return result;
   }

   
   {
   Ptr<Layout> result = nullptr;
   
   uint32 descriptorSets = 0; // <= VkPhysicalDeviceLimits::maxPerStageDescriptorSamplers
   
   
	typedef struct {
	    VkShaderStageFlags                          stageFlags;
	    uint32_t                                    offset;
	    uint32_t                                    size; // <= VkPhysicalDeviceLimits::maxPushConstantsSize - offset
	} VkPushConstantRange;
   
   // total accessible to any given shader stage across all elements of pSetLayouts:
   
   // VK_DESCRIPTOR_TYPE_SAMPLER
   // VK_DESCRIPTOR_ TYPE_COMBINED_IMAGE_SAMPLER
   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorSamplers
   
   // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
   // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorUniformBuffers
   
   // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
   // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorStorageBuffers
   
   // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
   // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
   // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorSampledImages
   
   // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
   // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorStorageImages
  
   VkPipelineLayoutCreateInfo layoutInfo;
   layoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   layoutInfo.pNext                  = nullptr;
   layoutInfo.flags                  = 0;
   layoutInfo.setLayoutCount         = descriptorSets;
   layoutInfo.pSetLayouts            = nullptr; // const VkDescriptorSetLayout*
   layoutInfo.pushConstantRangeCount = 0;       // uint32_t
   layoutInfo.pPushConstantRanges    = nullptr; // const VkPushConstantRange*

   VkPipelineLayout layout;
   Profile( gpu, vkCreatePipelineLayout(gpu->device, &layoutInfo, nullptr, &layout) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = ptr_dynamic_cast<Layout, LayoutVK>(new LayoutVK(gpu, layout));
      }
      
   return result;
   }




   //enum ProvokingVertex
   //   {
   //   ProvokingVertexFirst     = 0,
   //   ProvokingVertexLast         ,
   //   ProvokingVertexesCount
   //   };
   //
   //enum CoordinateOrigin
   //   {
   //   OriginUpperLeft          = 0,
   //   OriginLowerLeft             ,
   //   CoordinateOriginsCount
   //   };
   


   //static const VkProvokingVertex TranslateProvokingVertex[ProvokingVertexesCount] = 
   //   {
   //   VK_PROVOKING_VERTEX_FIRST,
   //   VK_PROVOKING_VERTEX_LAST  
   //   };
   //
   //static const VkCoordinateOrigin TranslateCoordinateOrigin[CoordinateOriginsCount] = 
   //   {
   //   VK_COORDINATE_ORIGIN_UPPER_LEFT,
   //   VK_COORDINATE_ORIGIN_LOWER_LEFT
   //   };



// Each feature has assigned bit:
// SupportMultisampling = 1   <- bit 1
//
// Supported features create a bitfield , iof feature is supported bit is 1, else its 0
//
// We check given feature support by pasing bit number in vector to API.
// This way in future we can add and rearrange enums and they are still valid!
// Also future apps are backward compatible ! -> they request some bit number, it's bigger than size of bitfield on given implementation -> means its not supported.
// We can also now group features together creating bitfields that will be compared to support bitfield to give result. Not only T/F but also concrete list of unsupported features.




// Metal:

// CommandQueue
// -> CommandBuffer
//    -> CommnadEncoder - One per "Render Pass"   <- Render Pass Descriptor - Describes whole FBO (color/depth/stencil attachments)
//       repeatedely N times:
//       -> bind Pipeline
//       -> bind Resources
//       -> bind Dynamic States
//       -> draw
//       end encoding
//    submit buffer
//  flush queue
// display



   /*
   
   DYNAMIC RENDER PASS STATE
   
   enum VkDynamicState
   
   VkPipelineDynamicStateCreateInfo dynamicInfo;
   dynamicInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamicInfo.pNext             = nullptr;
   dynamicInfo.flags             = 0; // Reserved for future
   dynamicInfo.dynamicStateCount = //elements in array;
   dynamicInfo.pDynamicStates    = //ptr to array of enums


   VK_DYNAMIC_STATE_VIEWPORT = 0,
   VK_DYNAMIC_STATE_SCISSOR = 1,
   VK_DYNAMIC_STATE_LINE_WIDTH = 2,
   VK_DYNAMIC_STATE_DEPTH_BIAS = 3,
   VK_DYNAMIC_STATE_BLEND_CONSTANTS = 4,
   VK_DYNAMIC_STATE_DEPTH_BOUNDS = 5,
   VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK = 6,
   VK_DYNAMIC_STATE_STENCIL_WRITE_MASK = 7,
   VK_DYNAMIC_STATE_STENCIL_REFERENCE
   
   vkCmdSetStencilReference();
   
   */
   

   Ptr<Pipeline> VulkanDevice::create(const Ptr<InputAssembler> inputAssembler,
                        const Ptr<ViewportState>  viewportState,
                        const Ptr<RasterState>    rasterState,
                        const Ptr<MultisamplingState> multisamplingState,                        
                        const Ptr<DepthStencilState> depthStencilState,
                        const Ptr<BlendState>     blendState,
                        const Ptr<PipelineLayout> pipelineLayout)
   {
   Ptr<Pipeline> result = nullptr;

   // Input Assembler State is Required
   assert( inputAssembler );
 
   // Cast to Vulkan states
   const Ptr<InputAssemblerVK>     input          = ptr_dynamic_cast<InputAssemblerVK,     InputAssembler>(inputAssembler);
   const Ptr<ViewportStateVK>      viewport       = ptr_dynamic_cast<ViewportStateVK,      ViewportState>(viewportState);
   const Ptr<RasterStateVK>        raster         = ptr_dynamic_cast<RasterStateVK,        RasterState>(rasterState);
   const Ptr<MultisamplingStateVK> multisampling  = ptr_dynamic_cast<MultisamplingStateVK, MultisamplingState>(multisamplingState); 
   const Ptr<DepthStencilStateVK>  depthStencil   = ptr_dynamic_cast<DepthStencilStateVK,  DepthStencilState>(depthStencilState);
   const Ptr<BlendStateVK>         blend          = ptr_dynamic_cast<BlendStateVK,         BlendState>(blendState);

   const Ptr<PipelineLayoutVK>     layout         = ptr_dynamic_cast<PipelineLayoutVK,     PipelineLayout>(pipelineLayout);

   // Patch States



   // Pipeline state
   VkGraphicsPipelineCreateInfo pipelineInfo;
   pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pipelineInfo.pNext               = nullptr;
//    uint32_t                                    stageCount          = ;
//    const VkPipelineShaderStageCreateInfo*      pStages             = ;
   pipelineInfo.pVertexInputState   = (input->state.vertexAttributeDescriptionCount > 0) ? &input->state : VK_NULL_HANDLE; // optional - nullptr == Use Programmable Vertex Fetch
   pipelineInfo.pInputAssemblyState = &input->statePrimitive;
   pipelineInfo.pTessellationState  = (input->stateTessellator.patchControlPoints > 0) ?  &input->stateTessellator : VK_NULL_HANDLE; // optional - nullptr == Tessellation Disabled
   pipelineInfo.pViewportState      = viewport      ? &viewport->state         : VK_NULL_HANDLE;
   pipelineInfo.pRasterizationState = raster        ? &raster->state           : VK_NULL_HANDLE;
   pipelineInfo.pMultisampleState   = multisampling ? &multisampling->state    : VK_NULL_HANDLE; // optional - nullptr == Multisampling Disabled
   pipelineInfo.pDepthStencilState  = depthStencil  ? &depthStencil->state     : VK_NULL_HANDLE; // optional - nullptr == disabled
   pipelineInfo.pColorBlendState    = blend         ? &blend->state            : VK_NULL_HANDLE; // optional - nullptr == Blending Disabled
   pipelineInfo.pDynamicState       = nullptr; // No dynamic state. Use VkPipelineDynamicStateCreateInfo*
   pipelineInfo.flags               = 0;
#ifdef EN_DEBUG
   pipelineInfo.flags              |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
#endif
   pipelineInfo.layout              = layout->state;
//    VkRenderPass                                renderPass          = ;
//    uint32_t                                    subpass             = ;
   pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Pipeline to derive from. (optional)
//    int32_t                                     basePipelineIndex   = ;

   // Create pipeline state object
   VkPipeline pipeline;
   Profile( this, vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &pipeline) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      Ptr<PipelineVK> pso = new PipelineVK();
      pso->id = pipeline;
      result = ptr_dynamic_cast<Pipeline, PipelineVK>(pso);
      }

   return result;
   }

   }
}
#endif

// clipOrigin          = VK_COORDINATE_ORIGIN_LOWER_LEFT;   // optional (GL45) - Can specify Direct3D way: VK_COORDINATE_ORIGIN_UPPER_LEFT 
// depthMode           = VK_DEPTH_MODE_ZERO_TO_ONE;         // optional (GL45) - Can specify Direct3D way: VK_DEPTH_MODE_NEGATIVE_ONE_TO_ONE

// // PIPELINE CACHES
// 
// VkPipelineCacheCreateInfo cacheInfo;
//     VkStructureType                             sType;
//     const void*                                 pNext;
//     size_t                                      initialSize;
//     const void*                                 initialData;
//     size_t                                      maxSize;
// 
// VkPipelineCache cache;
// 
// typedef VkResult (VKAPI *PFN_vkCreatePipelineCache)(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, VkPipelineCache* pPipelineCache);
// typedef void (VKAPI *PFN_vkDestroyPipelineCache)(VkDevice device, VkPipelineCache pipelineCache);
// typedef size_t (VKAPI *PFN_vkGetPipelineCacheSize)(VkDevice device, VkPipelineCache pipelineCache);
// typedef VkResult (VKAPI *PFN_vkGetPipelineCacheData)(VkDevice device, VkPipelineCache pipelineCache, void* pData);
// typedef VkResult (VKAPI *PFN_vkMergePipelineCaches)(VkDevice device, VkPipelineCache destCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches);
