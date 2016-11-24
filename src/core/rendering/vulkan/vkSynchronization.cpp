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
   barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
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

//
//
//   //enum class BufferUsage : uint32
//   //   {
//   //   }
//
//   //// 
//   //enum class Access : uint32
//   //   {
//   //   IndirectCommand    = 0x0001,  // Optimal layout for usage as source of read-only Indirect Commands Buffer
//   //   IndexBuffer        = 0x0002,  // Optimal layout for usage as source of read-only Index Buffer
//   //   VertexBuffer       = 0x0004,  // Optimal layout for usage as source of read-only Vertex Buffer
//   //   UniformBuffer      = 0x0008,  // Optimal layout for usage as source of read-only Uniform Buffer
//
//   //   RenderTargetWrite  = 0x0000   // Optimal layout for usage as destination of Color Attachment texture writes
//   //   DepthWrite         = 0x0000   // Optimal layout for usage as destination of Depth-Stencil texture writes
//   //   DepthRead          = 0x0000   // Optimal layout for usage as source of Depth-Stencil reads
//
//   //   };
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
//   if (checkBits(usageMask, underlyingType(TextureUsage::Atomic)))
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

   //if (checkBits(usageMask, underlyingType(TextureUsage::Read)))
   //   setBitmask(newAccess, VK_ACCESS_SHADER_READ_BIT);

   //if (checkBits(usageMask, underlyingType(TextureUsage::Write)))
   //   setBitmask(newAccess, VK_ACCESS_SHADER_WRITE_BIT);

   //// Depth-Stencil texture specific access
   //if (TextureFormatIsDepth(state.format)   ||
   //    TextureFormatIsStencil(state.format) ||
   //    TextureFormatIsDepthStencil(state.format))
   //   {
   //   if ( checkBits(usageMask, underlyingType(TextureUsage::Read)) ||            // Depth-Stencil read (for e.g. for Shadow-Map test)
   //        checkBits(usageMask, underlyingType(TextureUsage::RenderTargetRead)) ) // Depth-Stencil read (for Depth/Stencil tests)
   //      setBitmask(newAccess, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);

   //   if (checkBits(usageMask, underlyingType(TextureUsage::RenderTargetWrite)))  // Depth-Stencil write during Depth/Stencil testing
   //      setBitmask(newAccess, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT); 
   //   }
   //else // Color Attachments access
   //   {
   //   if (checkBits(usageMask, underlyingType(TextureUsage::RenderTargetWrite)))
   //      setBitmask(newAccess, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
   //   }

   //return newAccess;
   //}











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
//
//
//
//      // This can be deduced during bind to Indirect draw command. (but then we will have bubble caused by late transition).
//      //
//      // IndirectCommand    0x0001   (R buffer)   VK_ACCESS_INDIRECT_COMMAND_READ_BIT           D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
//
//      // Those can be deduced from buffer creation:
//      //
//      // IndexBuffer        0x0002   (R buffer)   VK_ACCESS_INDEX_READ_BIT                      D3D12_RESOURCE_STATE_INDEX_BUFFER
//      // VertexBuffer       0x0004   (R buffer)   VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT           D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
//      // UniformBuffer      0x0008   (R buffer)   VK_ACCESS_UNIFORM_READ_BIT                    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
//
//      // Should this need to be explicitly state? Or should this pixel/non-pixel flags always declared?
//      // Texture            0x0000                VK_ACCESS_SHADER_READ_BIT                     D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
//      //                                                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
//
//      // Image              0x0000   (RW texture) VK_ACCESS_SHADER_READ_BIT                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS
//      //                                          VK_ACCESS_SHADER_WRITE_BIT
//
//      // RenderTargetWrite  0x0000   (W  texture) VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT          D3D12_RESOURCE_STATE_RENDER_TARGET
//
//      // Those two could be figured out from RenderTargetRead/Write and Format
//      // DepthWrite         0x0000   (W  texture) VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT  D3D12_RESOURCE_STATE_DEPTH_WRITE
//      // DepthRead          0x0000   (R  texture) VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT   D3D12_RESOURCE_STATE_DEPTH_READ
//
//      // D3D12 distinguishes Copy & Blit from Resolve operations while Vulkan treats all of them as Transfer operations.
//
//
//      // UploadSource       0x0000   (R)          VK_ACCESS_HOST_READ_BIT                       D3D12_RESOURCE_STATE_COPY_SOURCE
//      // DownloadDestination 0x0000               VK_ACCESS_HOST_WRITE_BIT                      D3D12_RESOURCE_STATE_COPY_DEST
//
//      // CopySource         0x0000   (R)          VK_ACCESS_TRANSFER_READ_BIT                   D3D12_RESOURCE_STATE_COPY_SOURCE
//      // CopyDestination    0x0000   (W)          VK_ACCESS_TRANSFER_WRITE_BIT                  D3D12_RESOURCE_STATE_COPY_DEST
//
//      // ResolveSource      0x0000   (R)          VK_ACCESS_TRANSFER_READ_BIT                   D3D12_RESOURCE_STATE_RESOLVE_SOURCE
//      // ResolveDestination 0x0000   (W)          VK_ACCESS_TRANSFER_WRITE_BIT                  D3D12_RESOURCE_STATE_RESOLVE_DEST
//
//
//      // Present            0x0000   (R)          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT           D3D12_RESOURCE_STATE_PRESENT
//
//
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
//        D3D12_RESOURCE_STATE_GENERIC_READ	= ( ( ( ( ( 0x1 | 0x2 )  | 0x40 )  | 0x80 )  | 0x200 )  | 0x800 ) ,
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
//   // Vulkan tries to hide barrier latency by specifying after completion of which pipeline stage it should start, and before which it should end.
//   // At the same time D3D12 is trying to do the same, by allowing split-barriers, where app manuall places begining and ending barrrier events on CommandBuffer.
//   // (see: http://www.gamedev.net/topic/676655-how-could-we-benefit-from-using-split-barriers/ )
//
//   CommandBufferD3D12::transitionTexture(const Ptr<Texture> _texture)
//   {
//   assert( _texture );
//
//   TextureD3D12* texture = raw_reinterpret_cast<TextureD3D12>(&_texture);
//
//   // Transition Swap-Chain surface from rendering destination to presentation layout.
//   D3D12_RESOURCE_BARRIER barrier;
//   barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//   barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//          // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
//          // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends
//   barrier.Transition.pResource   = texture->handle;
//   barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//   barrier.Transition.StateBefore = texture->currentState;
//   barrier.Transition.StateAfter  = 0; // TODO: D3D12_RESOURCE_STATES
//
//   ProfileNoRet( handle->ResourceBarrier(1, barrier) )
//   }
//
//   CommandBufferD3D12::transitionTexture(const Ptr<Texture> _texture, 
//                                         const uint32v2 mipmaps, 
//                                         const uint32v2 layers)
//   {
//   assert( _texture );
//
//   TextureD3D12* texture = raw_reinterpret_cast<TextureD3D12>(&_texture);
//
//   // Each surface needs to be described separately if whole resource is not transitioning
//   UINT barriers = mipmaps.count * layers.count;
//
//   // Transition Swap-Chain surface from rendering destination to presentation layout.
//   D3D12_RESOURCE_BARRIER* barrier = new D3D12_RESOURCE_BARRIER[barriers];
//   for(uint32 layer=0; layer<layers.count; ++layer)
//      for(uint32 mipmap=0; mipmap<mipmaps.count; ++mipmap)
//         {
//         uint32 index = layer * mipmaps.count + mipmap;
//
//         UINT subresource = D3D12CalcSubresource(mipmaps.base + mipmap,
//                                                 layers.base  + layer,
//                                                 0u,                      // Plane Slice - DepthStencil formats have two slices
//                                                 texture->state.mipmaps,
//                                                 texture->state.layers);  // D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES for whole texture
//
//         barrier[index].Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//         barrier[index].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//                // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
//                // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends
//         barrier[index].Transition.pResource   = texture->handle;
//         barrier[index].Transition.Subresource = subresource;
//         barrier[index].Transition.StateBefore = texture->currentState;
//         barrier[index].Transition.StateAfter  = 0; // TODO: D3D12_RESOURCE_STATES
//         }
//
//   ProfileNoRet( handle->ResourceBarrier(barriers, barrier) )
//
//   delete [] barrier;
//   }
//
//   // Ensures that operations on "before" resource are finished before operations of "after" resource starts.
//   // (as both resources are sharing the same memory, so it prevens concurrent work and data corruption).
//   //
//   // barrier.Type                     = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
//   // barrier.Aliasing.pResourceBefore = ; // TODO: ID3D12Resource *
//   // barrier.Aliasing.pResourceAfter  = ; // TODO: ID3D12Resource *
//
//   // UAV (r/w) accessess barrier, previous operations need to finish before next ones
//   // (prevents concurrent execution, and data corruption)
//   //
//   // barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
//   // barrier.UAV.pResource = texture->handle;
//
//

   }
}
#endif
