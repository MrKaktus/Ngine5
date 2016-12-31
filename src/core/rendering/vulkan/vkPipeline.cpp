/*

 Ngine v5.0
 
 Module      : Vulkan Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkPipeline.h"

#if defined(EN_MODULE_RENDERER_VULKAN) 

#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkShader.h"
#include "core/rendering/vulkan/vkRenderPass.h"
#include "core/rendering/vulkan/vkLayout.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {








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
   
   // This is wrong as ShaderStage is bitmask now, and not a consecutive list of values!!!
   //const VkShaderStageFlagBits TranslateShaderStage[underlyingType(ShaderStage::Count)] = 
   //   {
   //   VK_SHADER_STAGE_VERTEX_BIT                  ,  // Vertex     
   //   VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT    ,  // Control    
   //   VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ,  // Evaluation 
   //   VK_SHADER_STAGE_GEOMETRY_BIT                ,  // Geometry   
   //   VK_SHADER_STAGE_FRAGMENT_BIT                ,  // Fragment   
   //   VK_SHADER_STAGE_COMPUTE_BIT                    // Compute    
   //   };


   PipelineVK::PipelineVK(VulkanDevice* _gpu, VkPipeline _handle, bool _graphics) :
      gpu(_gpu),
      handle(_handle),
      graphics(_graphics)
   {
   }

   PipelineVK::~PipelineVK()
   {
   ProfileNoRet( gpu, vkDestroyPipeline(gpu->device, handle, nullptr) )
   }

   Ptr<Pipeline> VulkanDevice::createPipeline(const PipelineState& pipelineState)
   {
   Ptr<PipelineVK> result = nullptr;

   // Pipeline object is always created against Render Pass, and app responsibility is to
   // provide missing states (ViewportState, Shaders).
   assert( pipelineState.renderPass );
   assert( pipelineState.viewportState );

   // Cast to Vulkan states
   const RenderPassVK*         renderPass     = raw_reinterpret_cast<RenderPassVK>(&pipelineState.renderPass);

   const InputLayoutVK*        input          = pipelineState.inputLayout ? raw_reinterpret_cast<InputLayoutVK>(&pipelineState.inputLayout)
                                                                          : raw_reinterpret_cast<InputLayoutVK>(&defaultState->inputLayout);

   const ViewportStateVK*      viewport       = raw_reinterpret_cast<ViewportStateVK>(&pipelineState.viewportState);

   const RasterStateVK*        raster         = pipelineState.rasterState ? raw_reinterpret_cast<RasterStateVK>(&pipelineState.rasterState)
                                                                          : raw_reinterpret_cast<RasterStateVK>(&defaultState->rasterState);

   const MultisamplingStateVK* multisampling  = pipelineState.multisamplingState ? raw_reinterpret_cast<MultisamplingStateVK>(&pipelineState.multisamplingState)
                                                                                 : raw_reinterpret_cast<MultisamplingStateVK>(&defaultState->multisamplingState);
      
   const DepthStencilStateVK*  depthStencil   = pipelineState.depthStencilState ? raw_reinterpret_cast<DepthStencilStateVK>(&pipelineState.depthStencilState)
                                                                                : raw_reinterpret_cast<DepthStencilStateVK>(&defaultState->depthStencilState);

   const BlendStateVK*         blend          = pipelineState.blendState ? raw_reinterpret_cast<BlendStateVK>(&pipelineState.blendState)
                                                                         : raw_reinterpret_cast<BlendStateVK>(&defaultState->blendState);

   const PipelineLayoutVK*     layout         = pipelineState.pipelineLayout ? raw_reinterpret_cast<PipelineLayoutVK>(&pipelineState.pipelineLayout) 
                                                                             : raw_reinterpret_cast<PipelineLayoutVK>(&defaultState->pipelineLayout);



   // Patch States

   // Can shader module keep source code of more than one shader stage, and then be bound several times to different stages to reuse it ?
 
   // Count amount of shader stages in use
   uint32 stages = 0;
   for(uint32 i=0; i<5; ++i)
      if (pipelineState.shader[i])
         stages++;

   assert( stages > 0 );

   // Create shader stages descriptions
   VkPipelineShaderStageCreateInfo* shaderInfo = new VkPipelineShaderStageCreateInfo[stages];
   uint32 stage = 0;
   for(uint32 i=0; i<5; ++i)
      if (pipelineState.shader[i])
         {
         const Ptr<ShaderVK> shader = ptr_reinterpret_cast<ShaderVK>(&pipelineState.shader[i]);
         
         shaderInfo[stage].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
         shaderInfo[stage].pNext  = nullptr;
         shaderInfo[stage].flags  = 0u; // Reserved for future use
         shaderInfo[stage].stage  = static_cast<VkShaderStageFlagBits>(underlyingType(shader->stage));
         shaderInfo[stage].module = shader->handle;
         shaderInfo[stage].pName  = pipelineState.function[i].c_str();
         shaderInfo[stage].pSpecializationInfo = nullptr; // Engine is not supporting specialization for now. (const VkSpecializationInfo*)
         stage++;
         }



   // Pipeline state
   VkGraphicsPipelineCreateInfo pipelineInfo;
   pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pipelineInfo.pNext               = nullptr;
   pipelineInfo.flags               = 0;
#ifdef EN_DEBUG
//   pipelineInfo.flags              |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
#endif
   pipelineInfo.stageCount          = stages;
   pipelineInfo.pStages             = shaderInfo;
   pipelineInfo.pVertexInputState   = &input->state;
   pipelineInfo.pInputAssemblyState = &input->statePrimitive;
   pipelineInfo.pTessellationState  = (input->stateTessellator.patchControlPoints > 0) ?  &input->stateTessellator : VK_NULL_HANDLE; // optional - nullptr == Tessellation is Disabled
   pipelineInfo.pViewportState      = viewport      ? &viewport->state         : VK_NULL_HANDLE; // optional - nullptr == Rasterization is Disabled
   pipelineInfo.pRasterizationState = &raster->state;
   pipelineInfo.pMultisampleState   = multisampling ? &multisampling->state    : VK_NULL_HANDLE; // optional - nullptr == Rasterization is Disabled
   pipelineInfo.pDepthStencilState  = depthStencil  ? &depthStencil->state     : VK_NULL_HANDLE; // optional - nullptr == Rasterization is Disabled (or subpass has no Depth-Stencil Attachments)
   pipelineInfo.pColorBlendState    = blend         ? &blend->state            : VK_NULL_HANDLE; // optional - nullptr == Rasterization is Disabled (or subpass has no Color Attachments)
   pipelineInfo.pDynamicState       = nullptr;        // No dynamic state. Use VkPipelineDynamicStateCreateInfo*
   pipelineInfo.layout              = layout->handle;
   pipelineInfo.renderPass          = renderPass->handle;
   pipelineInfo.subpass             = 0u;             // TODO: For now engine is not supporting subpasses except default one.
   pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Pipeline to derive from. (optional)
   pipelineInfo.basePipelineIndex   = -1;

   // Create pipeline state object
   VkPipeline pipeline = VK_NULL_HANDLE;
   Profile( this, vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) )  // pipelineCache
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      result = new PipelineVK(this, pipeline, true);

   delete [] shaderInfo;

   return ptr_reinterpret_cast<Pipeline>(&result);
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
