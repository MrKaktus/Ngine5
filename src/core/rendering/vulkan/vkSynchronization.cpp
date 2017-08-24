/*

 Ngine v5.0
 
 Module      : Vulkan Synchronization.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkCommandBuffer.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/log/log.h"
#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkBuffer.h"
#include "core/rendering/vulkan/vkTexture.h"
#include "core/rendering/vulkan/vkRenderPass.h"

namespace en
{
   namespace gpu
   {


   // BARRIERS
   //////////////////////////////////////////////////////////////////////////


   void TranslateBufferAccess(BufferAccess usage, BufferType type, VkAccessFlags& access)
   {
   access = static_cast<VkAccessFlags>(0u);

   bool canBeWritten = false;

   uint32 usageMask = underlyingType(usage);

   if (checkBitmask(usageMask, underlyingType(BufferAccess::Read)))
      {
      // Allows buffer to be read as Vertex Buffer during Draw
      if (type == BufferType::Vertex)
         setBitmask(access, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
      else
      // Allows buffer to be read as Index Buffer during Indexed Draw
      if (type == BufferType::Index)
         setBitmask(access, VK_ACCESS_INDEX_READ_BIT);
      else
      // Allows buffer to be read as Uniform Buffer
      if (type == BufferType::Uniform)
         setBitmask(access, VK_ACCESS_UNIFORM_READ_BIT);
      else
      // Allows buffer to be used as Indirect Command Buffer during Draw and Dispatch Indirect commands
      if (type == BufferType::Indirect)
         setBitmask(access, VK_ACCESS_INDIRECT_COMMAND_READ_BIT);
      else
      // Allows general Buffer read access by Shaders (Storage)
         setBitmask(access, VK_ACCESS_SHADER_READ_BIT);
      }

   // Allows Writing to Buffers (by Compute Shaders)
   if (checkBitmask(usageMask, underlyingType(BufferAccess::Write)))
      {
      setBitmask(access, VK_ACCESS_SHADER_WRITE_BIT);
      canBeWritten = true;
      }

   // Transfer between System Memory and Dedicated Memory
   if (checkBitmask(usageMask, underlyingType(BufferAccess::SystemSource)))
      setBitmask(access, VK_ACCESS_HOST_READ_BIT);

   if (checkBitmask(usageMask, underlyingType(BufferAccess::SystemDestination)))
      setBitmask(access, VK_ACCESS_HOST_WRITE_BIT);

   // Copy and Blit operations
   if (checkBitmask(usageMask, underlyingType(BufferAccess::CopySource)))
      setBitmask(access, VK_ACCESS_TRANSFER_READ_BIT);

   if (checkBitmask(usageMask, underlyingType(BufferAccess::CopyDestination)))
      setBitmask(access, VK_ACCESS_TRANSFER_WRITE_BIT);
   }

   void TranslateTextureAccess(TextureAccess usage, Format format, VkAccessFlags& access, VkImageLayout& layout)
   {
   access = static_cast<VkAccessFlags>(0u);
   layout = static_cast<VkImageLayout>(0u);

   bool canBeWritten = false;

   uint32 usageMask = underlyingType(usage);

   // Allows Reading and Sampling Textures by Shaders (if format is Depth-Stencil, allows Depth comparison test read)
   if (checkBitmask(usageMask, underlyingType(TextureAccess::Read)))
      {
      setBitmask(access, VK_ACCESS_SHADER_READ_BIT);
      layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      if ( TextureFormatIsDepth(format) ||
           TextureFormatIsStencil(format) ||
           TextureFormatIsDepthStencil(format) )
         layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
      }

   // Allows Writing to Textures (by Compute Shaders)
   if (checkBitmask(usageMask, underlyingType(TextureAccess::Write)))
      {
      setBitmask(access, VK_ACCESS_SHADER_WRITE_BIT);
      layout = VK_IMAGE_LAYOUT_GENERAL;
      canBeWritten = true;
      }

   // Texture can be one of Color, Depth, Stencil read sources during rendering operations
   if (checkBitmask(usageMask, underlyingType(TextureAccess::RenderTargetRead)))
      {
      if ( TextureFormatIsDepth(format) ||
           TextureFormatIsStencil(format) ||
           TextureFormatIsDepthStencil(format) )                  // Read via HiZ, Early-Z and Depth Test
         {
         setBitmask(access, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
         if (canBeWritten)
            layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 
         else
            layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
         }
      else
         {
         setBitmask(access, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT); // Read via Blening / LogicOp or Subpass load
         layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
         }
      }

   // Texture can be one of Color, Depth, Stencil destination for Rendering operations
   if (checkBitmask(usageMask, underlyingType(TextureAccess::RenderTargetWrite)))
      {
      if ( TextureFormatIsDepth(format) ||
           TextureFormatIsStencil(format) ||
           TextureFormatIsDepthStencil(format) )                   // Written via Depth Write
         {
         setBitmask(access, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
         layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;         // Overrides Read-Only Depth-Stencil mode
         }
      else
         {
         setBitmask(access, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT); // Written via Output write or Blening / LogicOp
         layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
         }
      }

   // Any kind of transfer or present operation overrides previously selected layout

   // Resolving MSAA surfaces
   if (checkBitmask(usageMask, underlyingType(TextureAccess::ResolveSource)))
      {
      setBitmask(access, VK_ACCESS_TRANSFER_READ_BIT);
      layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      }
   if (checkBitmask(usageMask, underlyingType(TextureAccess::ResolveDestination)))
      {
      setBitmask(access, VK_ACCESS_TRANSFER_WRITE_BIT);
      layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      }

   // Transfer between System Memory and Dedicated Memory
   if (checkBitmask(usageMask, underlyingType(TextureAccess::SystemSource)))
      {
      setBitmask(access, VK_ACCESS_HOST_READ_BIT);
      layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      }
   if (checkBitmask(usageMask, underlyingType(TextureAccess::SystemDestination)))
      {
      setBitmask(access, VK_ACCESS_HOST_WRITE_BIT);
      layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      }

   // Copy and Blit operations
   if (checkBitmask(usageMask, underlyingType(TextureAccess::CopySource)))
      {
      setBitmask(access, VK_ACCESS_TRANSFER_READ_BIT);
      layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      }
   if (checkBitmask(usageMask, underlyingType(TextureAccess::CopyDestination)))
      {
      setBitmask(access, VK_ACCESS_TRANSFER_WRITE_BIT);
      layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      }

   // Present
   if (checkBitmask(usageMask, underlyingType(TextureAccess::Present)))
      {
      access = 0u;
      layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      }
   }

   // TODO: Analyze below access types:
   //
   //  VK_ACCESS_MEMORY_READ_BIT
   //  VK_ACCESS_MEMORY_WRITE_BIT
   //  VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX
   //  VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX

   // TODO: Figure out how to expose barrier place in pipeline
   //
   //  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001,
   //  VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT = 0x00000002,
   //  VK_PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00000004,
   //  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT = 0x00000008,
   //  VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
   //  VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
   //  VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT = 0x00000040,
   //  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00000080,
   //  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
   //  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = 0x00000200,
   //  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
   //  VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT = 0x00000800,
   //  VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00001000,
   //  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00002000,
   //  VK_PIPELINE_STAGE_HOST_BIT = 0x00004000,
   //  VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT = 0x00008000,
   //  VK_PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00010000,



   // Buffer barrier
   void CommandBufferVK::barrier(const Ptr<Buffer> buffer, 
                                 const uint64 offset,
                                 const uint64 size,
                                 const BufferAccess currentAccess,
                                 const BufferAccess newAccess)
   {
   assert( buffer );

   BufferVK* ptr = raw_reinterpret_cast<BufferVK>(&buffer);

   // Determine current buffer access bitmask
   VkAccessFlags vOldAccess;
   TranslateBufferAccess(currentAccess, ptr->apiType, vOldAccess);

   // Determine buffer new access
   VkAccessFlags vNewAccess;
   TranslateBufferAccess(newAccess, ptr->apiType, vNewAccess);

   barrier(buffer, 
           offset, 
           size,
           vOldAccess,
           vNewAccess,
           VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,  
           VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
   }

   // Texture barrier
   void CommandBufferVK::barrier(const Ptr<Texture>  _texture, 
                                 const TextureAccess currentAccess,
                                 const TextureAccess newAccess)
   {
   // Vulkan is not gaining anything from this specialized call,
   // as it's already transitioning all subresources in one call.
   barrier(_texture, 
           uint32v2(0, _texture->mipmaps()), 
           uint32v2(0, _texture->layers()),
           currentAccess,
           newAccess);
   }

   void CommandBufferVK::barrier(const Ptr<Texture>  texture, 
                                 const uint32v2      mipmaps, 
                                 const uint32v2      layers,
                                 const TextureAccess currentAccess,
                                 const TextureAccess newAccess) 
   {
   assert( texture );

   TextureVK* ptr = raw_reinterpret_cast<TextureVK>(&texture);

   // Determine current texture access bitmask and layout type
   VkAccessFlags vOldAccess;
   VkImageLayout vOldLayout;
   TranslateTextureAccess(currentAccess, ptr->state.format, vOldAccess, vOldLayout);

   // Determine texture new access and layout
   VkAccessFlags vNewAccess;
   VkImageLayout vNewLayout;
   TranslateTextureAccess(newAccess, ptr->state.format, vNewAccess, vNewLayout);

   barrier(texture, 
           mipmaps, 
           layers,
           vOldAccess,
           vNewAccess,
           vOldLayout,
           vNewLayout,
           VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,  
           VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
   }


   // Buffer barrier
   void CommandBufferVK::barrier(const Ptr<Buffer> _buffer, 
                                 const uint64 offset,
                                 const uint64 size,
                                 const VkAccessFlags currentAccess,
                                 const VkAccessFlags newAccess,
                                 const VkPipelineStageFlags afterStage,  // Transition after this stage
                                 const VkPipelineStageFlags beforeStage) // Transition before this stage
   {
   assert( _buffer );

   BufferVK* buffer = raw_reinterpret_cast<BufferVK>(&_buffer);

   VkBufferMemoryBarrier barrier;
   barrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
   barrier.pNext               = nullptr;
   barrier.srcAccessMask       = currentAccess;
   barrier.dstAccessMask       = newAccess;
   barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;  // No transition of ownership between Queue Families is allowed.
   barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;  // TODO: Fix this for Transfer Queue Families !!
   barrier.buffer              = buffer->handle;
   barrier.offset              = offset;
   barrier.size                = size;

   ProfileNoRet( gpu, vkCmdPipelineBarrier(handle,
                                           afterStage,
                                           beforeStage,    
                                           0u,            // 0 or VK_DEPENDENCY_BY_REGION_BIT ??? 
                                           0u, nullptr,   // Memory barriers
                                           1u, &barrier,  // Buffer memory barriers
                                           0u, nullptr) ) // Image memory barriers
   }

   // Texture barrier
   void CommandBufferVK::barrier(const Ptr<Texture> _texture, 
                                 const uint32v2 mipmaps, 
                                 const uint32v2 layers,
                                 const VkAccessFlags currentAccess,
                                 const VkAccessFlags newAccess,
                                 const VkImageLayout currentLayout,
                                 const VkImageLayout newLayout,
                                 const VkPipelineStageFlags afterStage,  // Transition after this stage
                                 const VkPipelineStageFlags beforeStage) // Transition before this stage
   {
   assert( _texture );

   TextureVK* texture = raw_reinterpret_cast<TextureVK>(&_texture);

   // Transition Swap-Chain surface from rendering destination to presentation layout.
   VkImageMemoryBarrier barrier;
   barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   barrier.pNext               = nullptr;
   barrier.srcAccessMask       = currentAccess;
   barrier.dstAccessMask       = newAccess;
   barrier.oldLayout           = currentLayout;
   barrier.newLayout           = newLayout;
   barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;  // No transition of ownership between Queue Families is allowed.
   barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;  // TODO: Fix this for Transfer Queue Families !!
   barrier.image               = texture->handle; 
   barrier.subresourceRange.aspectMask     = TranslateImageAspect(texture->state.format);
   barrier.subresourceRange.baseMipLevel   = mipmaps.base;
   barrier.subresourceRange.levelCount     = mipmaps.count;
   barrier.subresourceRange.baseArrayLayer = layers.base;
   barrier.subresourceRange.layerCount     = layers.count;

   ProfileNoRet( gpu, vkCmdPipelineBarrier(handle,
                                           afterStage,
                                           beforeStage,    
                                           0u,             // 0 or VK_DEPENDENCY_BY_REGION_BIT ??? 
                                           0u, nullptr,    // Memory barriers
                                           0u, nullptr,    // Buffer memory barriers
                                           1u, &barrier) ) // Image memory barriers
   }


   // SEMAPHORES
   //////////////////////////////////////////////////////////////////////////


   SemaphoreVK::SemaphoreVK(VulkanDevice* _gpu) :
      gpu(_gpu),
      handle(VK_NULL_HANDLE)
   {
   VkSemaphoreCreateInfo info;
   info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
   info.pNext = nullptr;
   info.flags = 0u;       // VkSemaphoreCreateFlags - reserved.

   Profile( gpu, vkCreateSemaphore(gpu->device, &info, nullptr, &handle) )
   }

   SemaphoreVK::~SemaphoreVK()
   {
   ProfileNoRet( gpu, vkDestroySemaphore(gpu->device, handle, nullptr) )
   }

   Ptr<Semaphore> VulkanDevice::createSemaphore(void)
   {
   return Ptr<Semaphore>(new SemaphoreVK(this));
   }



   // Vulkan:
   // 
   // Fences     - can be used to communicate to the host that execution of some task on the device has completed.
   //              (GPU -> CPU notification, CPU - GPU synchronization)
   //
   // Semaphores - can be used to control resource access across multiple queues.
   //              Command Buffers synchronization on the queue.
   // 
   //              D3D12 Fences - used to synchronize queues.
   //                             An object used for synchronization of the CPU and one or more GPUs.
   //              CommandQueue::Signal(ID3D12Fence *pFence, UINT64 Value) - signals execution reaching given fence
   //              CommandQueue::Wait(ID3D12Fence *pFence, UINT64 Value) - waits for given fence to be signaled
   //
   // Events     - provide a fine-grained synchronization primitive which can be signaled either within a command 
   //              buffer or by the host, and can be waited upon within a command buffer or queried on the host.
   //
   //              Metal Fences - are equivalent, but have no CPU access.
   //
   // Barriers   - Pipeline barriers also provide synchronization control within a command buffer, but at a single 
   //              point, rather than with separate signal and wait operations.
   //
   //              D3D12 Barriers - have ability to signal beginning and end of transition.





   // EVENTS
   //////////////////////////////////////////////////////////////////////////


   EventVK::EventVK(VulkanDevice* _gpu) :
      gpu(_gpu),
      handle(VK_NULL_HANDLE)
   {
   VkEventCreateInfo info;
   info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
   info.pNext = nullptr;
   info.flags = 0; // Reserved for future

   Profile( gpu, vkCreateEvent(gpu->device, &info, nullptr, &handle) )
   }

   EventVK::~EventVK()
   {
   ProfileNoRet( gpu, vkDestroyEvent(gpu->device, handle, nullptr) )
   }
   
   bool EventVK::signaled(void)
   {
   uint32 thread = Scheduler.core();

   Profile( gpu, vkGetEventStatus(gpu->device, handle) )
   if (gpu->lastResult[thread] == VK_EVENT_SET)
      return true;

   return false;
   }

   void EventVK::signal(void)
   {
   Profile( gpu, vkSetEvent(gpu->device, handle) )
   }

   void EventVK::unsignal(void)
   {
   Profile( gpu, vkResetEvent(gpu->device, handle) )
   }

   Ptr<Event> CommandBufferVK::signal(void)
   {
   Ptr<EventVK> result = new EventVK(gpu);

   // All stages work, before this moment in time, needs to be finished.
   // TODO: Expose way to specify only sub-set of pipeline stages.
   VkPipelineStageFlags flags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
   ProfileNoRet( gpu, vkCmdSetEvent(handle, result->handle, flags) )

   return ptr_reinterpret_cast<Event>(&result);
   }

   void CommandBufferVK::wait(Ptr<Event> eventToWaitFor)
   {
   EventVK* _event = raw_reinterpret_cast<EventVK>(&eventToWaitFor);

   ProfileNoRet( gpu, vkCmdWaitEvents(handle,
                                      1,       // events count
                                      &_event->handle,
                                      VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, // TODO: Expose event sync place in currently executed Pipeline Stage
                                      VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, //       This will avoid unnecessary Cache flushes, etc.
                                      0,       // Memory barriers
                                      nullptr,
                                      0,       // Buffer memory barriers
                                      nullptr,
                                      0,       // Image memory barriers
                                      nullptr) )
   }


   //enum class PipelineStage : uint32
   //   {
   //   // TODO: Do we mimic Vulkan Pipeline Stages granularity ?
   //   Count
   //   };
   //
   //static const VkPipelineStageFlagBits TranslatePipelineStage[underlyingType(PipelineStage::Count)] =
   //   {
   //   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT                    ,
   //   VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT                  ,
   //   VK_PIPELINE_STAGE_VERTEX_INPUT_BIT                   ,
   //   VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  ,
   //   VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    ,
   //   VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT ,
   //   VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                ,
   //   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                ,
   //   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT           ,
   //   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT            ,
   //   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT        ,
   //   VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 ,
   //   VK_PIPELINE_STAGE_TRANSFER_BIT                       ,
   //   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT                 ,
   //   VK_PIPELINE_STAGE_HOST_BIT                           ,
   //   VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT                   ,
   //   VK_PIPELINE_STAGE_ALL_COMMANDS_BIT                   ,
   //   };
   







   //enum class BufferUsage : uint32
   //   {
   //   }







//
//
//
//
//
//   // Buffer Barrier after populating it with data
//   VkAccessFlags newAccess = 0u; 
//
//   BufferVK* buffer = nullptr;
//   switch(buffer->apiType)
//      {
//      case BufferType::Vertex:
//         setBitmask(newAccess, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
//         break;
//
//      case BufferType::Index:
//         setBitmask(newAccess, VK_ACCESS_INDEX_READ_BIT);
//         break;
//         
//      case BufferType::Uniform:
//         setBitmask(newAccess, VK_ACCESS_UNIFORM_READ_BIT);
//         break;
//
//      case BufferType::Storage:
//         setBitmask(newAccess, VK_ACCESS_SHADER_READ_BIT);
//         break;
//         
//      case BufferType::Indirect:
//         setBitmask(newAccess, VK_ACCESS_INDIRECT_COMMAND_READ_BIT);
//         break;
//
//      case BufferType::Transfer:
//         if (buffer->heap->_usage = MemoryUsage::Streamed
//         _usage = MemoryUsage::Immediate
//
//         setBitmask(newAccess, );
//         break;
//            
//      default:
//         assert( 0 );
//         break;
//      };
//
//

//
//
//
//
//
//   uint32 usageMask = underlyingType(usage);
//
//
//
//
//
//   if (checkBitmask(usageMask, underlyingType(TextureUsage::Atomic)))
//      {
//      textureInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
//      canBeMemoryless = false;
//      }
//
//
//
//
//
//
// 
//
//
//
//
//    VK_ACCESS_INPUT_ATTACHMENT_READ_BIT = 0x00000010,
//    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00000080,
//    // Pre-operation transfer
//    VK_ACCESS_TRANSFER_READ_BIT = 0x00000800,
//    VK_ACCESS_TRANSFER_WRITE_BIT = 0x00001000,
//    VK_ACCESS_HOST_READ_BIT = 0x00002000,
//    VK_ACCESS_HOST_WRITE_BIT = 0x00004000,
//
//
//


   //VkAccessFlags postBarrierAccess(const TextureUsage usage)
   //{
   //uint32 usageMask = underlyingType(usage);

   //// Post operation, restore texture to it's optimal state, by
   //// specifying new access mask based on declared usage and format.
   //VkAccessFlags newAccess = 0u; 

   //if (checkBitmask(usageMask, underlyingType(TextureUsage::Read)))
   //   setBitmask(newAccess, VK_ACCESS_SHADER_READ_BIT);

   //if (checkBitmask(usageMask, underlyingType(TextureUsage::Write)))
   //   setBitmask(newAccess, VK_ACCESS_SHADER_WRITE_BIT);

   //// Depth-Stencil texture specific access
   //if (TextureFormatIsDepth(state.format)   ||
   //    TextureFormatIsStencil(state.format) ||
   //    TextureFormatIsDepthStencil(state.format))
   //   {
   //   if ( checkBitmask(usageMask, underlyingType(TextureUsage::Read)) ||            // Depth-Stencil read (for e.g. for Shadow-Map test)
   //        checkBitmask(usageMask, underlyingType(TextureUsage::RenderTargetRead)) ) // Depth-Stencil read (for Depth/Stencil tests)
   //      setBitmask(newAccess, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);

   //   if (checkBitmask(usageMask, underlyingType(TextureUsage::RenderTargetWrite)))  // Depth-Stencil write during Depth/Stencil testing
   //      setBitmask(newAccess, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT); 
   //   }
   //else // Color Attachments access
   //   {
   //   if (checkBitmask(usageMask, underlyingType(TextureUsage::RenderTargetWrite)))
   //      setBitmask(newAccess, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
   //   }

   //return newAccess;
   //}




//   //// Examples:
//   //// - Static        - TextureUsage::Read
//   //// - ShadowMap     - TextureUsage::RenderTargetWrite | TextureUsage::Read
//   //// - DepthBuffer   - TextureUsage::RenderTargetWrite
//   //// - GBuffer Depth - TextureUsage::RenderTargetWrite | TextureUsage::Read
//   ////
//   //enum class TextureUsage : uint32
//   //   {
//   //   Read                = 0x0001,  // Allows Reading and Sampling Textures by Shaders
//   //   Write               = 0x0002,  // Allows Writing to Textures (for eg. by Compute Shaders)
//   //   Atomic              = 0x0004,  // Allows Atomic operations on Texture
//   //   RenderTargetRead    = 0x0008,  // Texture can be one of Color, Depth, Stencil read sources during rendering operations
//   //   RenderTargetWrite   = 0x0010,  // Texture can be one of Color, Depth, Stencil destination for Rendering operations
//   //   MultipleViews       = 0x0020,  // Allows creation of multiple Texture Views from one Texture
//   //   Streamed            = 0x0040,  // Optimal for fast streaming of data between CPU and GPU
//   //   Sparse              = 0x0100,  // Texture is partially backed with memory
//   //   };


   // 
   //enum class Access : uint32
   //   {
   //   IndirectCommand    = 0x0001,  // Optimal layout for usage as source of read-only Indirect Commands Buffer
   //   IndexBuffer        = 0x0002,  // Optimal layout for usage as source of read-only Index Buffer
   //   VertexBuffer       = 0x0004,  // Optimal layout for usage as source of read-only Vertex Buffer
   //   UniformBuffer      = 0x0008,  // Optimal layout for usage as source of read-only Uniform Buffer

   //   DepthRead          = 0x0000,  // Optimal layout for usage as source of Depth-Stencil reads
   //   DepthWrite         = 0x0000,  // Optimal layout for usage as destination of Depth-Stencil texture writes
   //   RenderTargetWrite  = 0x0000,  // Optimal layout for usage as destination of Color Attachment texture writes



   //   };

   //inline Access operator| (Access a, Access b)
   //{
   //return static_cast<Access>(underlyingType(a) | underlyingType(b));
   //}



   //VkAccessFlags function(Access newAccess)
   //{
   //VkAccessFlags access = 0u;

   //uint32 accessMask = underlyingType(newAccess);

   //if (checkBitmask(accessMask, underlyingType(Access::IndirectCommand)))
   //   setBitmask(access, VK_ACCESS_INDIRECT_COMMAND_READ_BIT);

   //// TODO: Finish Vulkan translation

   //}




   // Vulkan VkAccessFlagBits:
   //
   // - 0                                           - If none is set as source, current resource content may be discarded.
   // - VK_ACCESS_INDIRECT_COMMAND_READ_BIT
   // - VK_ACCESS_INDEX_READ_BIT
   // - VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
   // - VK_ACCESS_UNIFORM_READ_BIT
   // - VK_ACCESS_INPUT_ATTACHMENT_READ_BIT
   // - VK_ACCESS_SHADER_READ_BIT
   // - VK_ACCESS_SHADER_WRITE_BIT
   // - VK_ACCESS_COLOR_ATTACHMENT_READ_BIT          - Read via Blening / LogicOp or Subpass load
   // - VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
   // - VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
   // - VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
   // - VK_ACCESS_TRANSFER_READ_BIT
   // - VK_ACCESS_TRANSFER_WRITE_BIT
   // - VK_ACCESS_HOST_READ_BIT                      - Read access by a Host operation.
   // - VK_ACCESS_HOST_WRITE_BIT                     - Write access by a Host operation.
   // - VK_ACCESS_MEMORY_READ_BIT
   // - VK_ACCESS_MEMORY_WRITE_BIT
   //
   // D3D12 D3D12_RESOURCE_STATES:
   // 
   // - D3D12_RESOURCE_STATE_COMMON                    
   // - D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
   // - D3D12_RESOURCE_STATE_INDEX_BUFFER              
   // - D3D12_RESOURCE_STATE_RENDER_TARGET             
   // - D3D12_RESOURCE_STATE_UNORDERED_ACCESS          
   // - D3D12_RESOURCE_STATE_DEPTH_WRITE               
   // - D3D12_RESOURCE_STATE_DEPTH_READ                
   // - D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE 
   // - D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE     
   // - D3D12_RESOURCE_STATE_STREAM_OUT                
   // - D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT         
   // - D3D12_RESOURCE_STATE_COPY_DEST                 
   // - D3D12_RESOURCE_STATE_COPY_SOURCE               
   // - D3D12_RESOURCE_STATE_RESOLVE_DEST              
   // - D3D12_RESOURCE_STATE_RESOLVE_SOURCE            
   // - D3D12_RESOURCE_STATE_GENERIC_READ             
   // - D3D12_RESOURCE_STATE_PRESENT                  
   // - D3D12_RESOURCE_STATE_PREDICATION
   //
   // Metal:
   //
   // - MTLTextureUsageUnknown
   // - MTLTextureUsageShaderRead       - Allows Reads and Sampling from all shader stages
   // - MTLTextureUsageShaderWrite      - Allows writing from Compute Shaders
   // - MTLTextureUsageRenderTarget     - Color, Depth or Stencil render target
   // - MTLTextureUsagePixelFormatView  - Allows Views
   //

   //enum class BufferUsage : uint32
   //   {
   //   IndirectCommand    = 0x0001,  // Optimal layout for usage as source of read-only Indirect Commands Buffer
   //   IndexBuffer        = 0x0002,  // Optimal layout for usage as source of read-only Index Buffer
   //   VertexBuffer       = 0x0004,  // Optimal layout for usage as source of read-only Vertex Buffer
   //   UniformBuffer      = 0x0008,  // Optimal layout for usage as source of read-only Uniform Buffer
   //   };

   //enum class TextureUsage : uint32
   //   {
   //   Read                = 0x0001,  // Allows Reading and Sampling Textures by Shaders (if format is Depth-Stencil, allows Depth comparison test read)
   //   Write               = 0x0002,  // Allows Writing to Textures (by Compute Shaders)
   //   Atomic              = 0x0004,  // Allows Atomic operations on Texture
   //   RenderTargetRead    = 0x0008,  // Texture can be one of Color, Depth, Stencil read sources during rendering operations
   //   RenderTargetWrite   = 0x0010,  // Texture can be one of Color, Depth, Stencil destination for Rendering operations
   //   MultipleViews       = 0x0020,  // Allows creation of multiple Texture Views from one Texture
   //   Streamed            = 0x0040,  // Optimal for fast streaming of data between CPU and GPU
   //   Sparse              = 0x0100,  // Texture is partially backed with memory

   //   ResolveSource       = 0x0200,  // Can be MSAA source of Resolve operation.
   //   ResolveDestination  = 0x0400,  // Can be destination of MSAA resolve operation.
   //   };

   //   SystemSource      // System memory backed resource, used as source of data transfer to dedicated memory.
   //   SystemDestination // System memory backed resource, used as destination of data transfer from dedicated memory.
   //   CopySource        // Resource backed by dedicated memory. Used as source for Copy or Blit operation.
   //   CopyDestination   // Resource backed by dedicated memory. Used as destination of Copy or Blit operation.


   // Mapping:
   //
   // This can be deduced during bind to Indirect draw command. (but then we will have bubble caused by late transition).
   //
   // IndirectCommand    (R buffer)   VK_ACCESS_INDIRECT_COMMAND_READ_BIT           D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
   //
   // Those can be deduced on buffer creation:
   //
   // IndexBuffer        (R buffer)   VK_ACCESS_INDEX_READ_BIT                      D3D12_RESOURCE_STATE_INDEX_BUFFER
   // VertexBuffer       (R buffer)   VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT           D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
   // UniformBuffer      (R buffer)   VK_ACCESS_UNIFORM_READ_BIT                    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
   //
   // Read               (R Texture)  VK_ACCESS_SHADER_READ_BIT
   // Write              (W Texture)  VK_ACCESS_SHADER_WRITE_BIT
   // RenderTargetRead   (R Texture)  VK_ACCESS_INPUT_ATTACHMENT_READ_BIT
   //
   // D3D12 distinguishes difference between Copy/Blit operations and Resolve ones, 
   // while Vulkan treats all of them as Transfer operations. On the other hand,
   // Vulkan distinguishes difference between copy operations inside VRAM and transfers
   // between RAM and VRAM, while Direct3D12 treats them as the same.
   //
   // SystemSource       (R)          VK_ACCESS_HOST_READ_BIT                       D3D12_RESOURCE_STATE_COPY_SOURCE
   // SystemDestination  (W)          VK_ACCESS_HOST_WRITE_BIT                      D3D12_RESOURCE_STATE_COPY_DEST
   //
   // CopySource         (R)          VK_ACCESS_TRANSFER_READ_BIT                   D3D12_RESOURCE_STATE_COPY_SOURCE
   // CopyDestination    (W)          VK_ACCESS_TRANSFER_WRITE_BIT                  D3D12_RESOURCE_STATE_COPY_DEST
   //
   // ResolveSource      (R)          VK_ACCESS_TRANSFER_READ_BIT                   D3D12_RESOURCE_STATE_RESOLVE_SOURCE
   // ResolveDestination (W)          VK_ACCESS_TRANSFER_WRITE_BIT                  D3D12_RESOURCE_STATE_RESOLVE_DEST
   //
   // Should this need to be explicitly state? Or should this pixel/non-pixel flags always declared?
   // Texture                         VK_ACCESS_SHADER_READ_BIT                     D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
   //                                                                               D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
   //
   // Image              (RW texture) VK_ACCESS_SHADER_READ_BIT                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS
   //                                 VK_ACCESS_SHADER_WRITE_BIT
   //
   // RenderTargetWrite  (W  texture) VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT          D3D12_RESOURCE_STATE_RENDER_TARGET
   //
   // Those two could be figured out from RenderTargetRead/Write, Format and RenderPass Load/Store operations
   // DepthWrite         (W  texture) VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT  D3D12_RESOURCE_STATE_DEPTH_WRITE
   // DepthRead          (R  texture) VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT   D3D12_RESOURCE_STATE_DEPTH_READ
   //
   // Present            (R)          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT           D3D12_RESOURCE_STATE_PRESENT




















//    // TODO:   VK_ACCESS_INPUT_ATTACHMENT_READ_BIT   vs  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT ????
//
//    //VK_ACCESS_INDIRECT_COMMAND_READ_BIT = 0x00000001,
//    //VK_ACCESS_INDEX_READ_BIT = 0x00000002,
//    //VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT = 0x00000004,
//    //VK_ACCESS_UNIFORM_READ_BIT = 0x00000008,
//    VK_ACCESS_INPUT_ATTACHMENT_READ_BIT = 0x00000010,      // RenderTargetRead (in Render Passes graph)
//    VK_ACCESS_SHADER_READ_BIT = 0x00000020,
//    VK_ACCESS_SHADER_WRITE_BIT = 0x00000040,
//    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00000080,      // Can be read by presentation engine? (uncompressed?)
//    //VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
//    //VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00000200,
//    //VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00000400,
//    //VK_ACCESS_TRANSFER_READ_BIT = 0x00000800,
//    //VK_ACCESS_TRANSFER_WRITE_BIT = 0x00001000,
//    VK_ACCESS_HOST_READ_BIT = 0x00002000,
//    VK_ACCESS_HOST_WRITE_BIT = 0x00004000,
//    VK_ACCESS_MEMORY_READ_BIT = 0x00008000,
//    VK_ACCESS_MEMORY_WRITE_BIT = 0x00010000,
//
//
//
//// TextureD3D12:
//// D3D12_RESOURCE_STATES currentState; <- This won't work when transitioning the same texture on several command buffers at the same time (concurrent threads)
//
//       D3D12_RESOURCE_STATE_COMMON	= 0,
//        //D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER	= 0x1,
//        //D3D12_RESOURCE_STATE_INDEX_BUFFER	= 0x2,
//        //D3D12_RESOURCE_STATE_RENDER_TARGET	= 0x4,
//        //D3D12_RESOURCE_STATE_UNORDERED_ACCESS	= 0x8,
//        //D3D12_RESOURCE_STATE_DEPTH_WRITE	= 0x10,
//        //D3D12_RESOURCE_STATE_DEPTH_READ	= 0x20,
//        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE	= 0x40, // VS, HS, DS, GS, CS - RO access
//        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	= 0x80,     //                 PS - RO access
//        D3D12_RESOURCE_STATE_STREAM_OUT	= 0x100,
//        //D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT	= 0x200,
//        //D3D12_RESOURCE_STATE_COPY_DEST	= 0x400,
//        //D3D12_RESOURCE_STATE_COPY_SOURCE	= 0x800,
//        //D3D12_RESOURCE_STATE_RESOLVE_DEST	= 0x1000,
//        //D3D12_RESOURCE_STATE_RESOLVE_SOURCE	= 0x2000,
//        D3D12_RESOURCE_STATE_GENERIC_READ	= ( ( ( ( ( 0x1 | 0x2 )  | 0x40 )  | 0x80 )  | 0x200 )  | 0x800 ) , // <--- Initial Stage for Upload Heaps
//        D3D12_RESOURCE_STATE_PRESENT	= 0,
//        D3D12_RESOURCE_STATE_PREDICATION	= 0x
//
//
//    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001,
//    VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT = 0x00000002,
//    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00000004,
//    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT = 0x00000008,
//    VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
//    VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
//    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT = 0x00000040,
//    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00000080,
//    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
//    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = 0x00000200,
//    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
//    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT = 0x00000800,
//    VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00001000,
//    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00002000,
//    VK_PIPELINE_STAGE_HOST_BIT = 0x00004000,
//    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT = 0x00008000,
//    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00010000,
//
//   VkAccessFlags newAccess; // TODO: Specify !
//   VkImageLayout newLayout;
//   VkPipelineStageFlags afterStage;  // Transition after this stage
//   VkPipelineStageFlags beforeStage; // Transition before this stage
//
//
//


   }
}
#endif
