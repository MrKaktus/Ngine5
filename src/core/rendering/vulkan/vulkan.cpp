/*

 Ngine v5.0
 
 Module      : Vulkan API.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/
#include "core/defines.h"
#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_WINDOWS)
PFN_vkCreateInstance                               vkCreateInstance                               = nullptr;
PFN_vkDestroyInstance                              vkDestroyInstance                              = nullptr;
PFN_vkEnumeratePhysicalDevices                     vkEnumeratePhysicalDevices                     = nullptr;
PFN_vkGetPhysicalDeviceFeatures                    vkGetPhysicalDeviceFeatures                    = nullptr;
PFN_vkGetPhysicalDeviceFormatProperties            vkGetPhysicalDeviceFormatProperties            = nullptr;
PFN_vkGetPhysicalDeviceImageFormatProperties       vkGetPhysicalDeviceImageFormatProperties       = nullptr;
PFN_vkGetPhysicalDeviceProperties                  vkGetPhysicalDeviceProperties                  = nullptr;
PFN_vkGetPhysicalDeviceQueueFamilyProperties       vkGetPhysicalDeviceQueueFamilyProperties       = nullptr;
PFN_vkGetPhysicalDeviceMemoryProperties            vkGetPhysicalDeviceMemoryProperties            = nullptr;
PFN_vkGetInstanceProcAddr                          vkGetInstanceProcAddr                          = nullptr;
PFN_vkGetDeviceProcAddr                            vkGetDeviceProcAddr                            = nullptr;
PFN_vkCreateDevice                                 vkCreateDevice                                 = nullptr;
PFN_vkDestroyDevice                                vkDestroyDevice                                = nullptr;
PFN_vkEnumerateInstanceExtensionProperties         vkEnumerateInstanceExtensionProperties         = nullptr;
PFN_vkEnumerateDeviceExtensionProperties           vkEnumerateDeviceExtensionProperties           = nullptr;
PFN_vkEnumerateInstanceLayerProperties             vkEnumerateInstanceLayerProperties             = nullptr;
PFN_vkEnumerateDeviceLayerProperties               vkEnumerateDeviceLayerProperties               = nullptr;
PFN_vkGetDeviceQueue                               vkGetDeviceQueue                               = nullptr;
PFN_vkQueueSubmit                                  vkQueueSubmit                                  = nullptr;
PFN_vkQueueWaitIdle                                vkQueueWaitIdle                                = nullptr;
PFN_vkDeviceWaitIdle                               vkDeviceWaitIdle                               = nullptr;
PFN_vkAllocateMemory                               vkAllocateMemory                               = nullptr;
PFN_vkFreeMemory                                   vkFreeMemory                                   = nullptr;
PFN_vkMapMemory                                    vkMapMemory                                    = nullptr;
PFN_vkUnmapMemory                                  vkUnmapMemory                                  = nullptr;
PFN_vkFlushMappedMemoryRanges                      vkFlushMappedMemoryRanges                      = nullptr;
PFN_vkInvalidateMappedMemoryRanges                 vkInvalidateMappedMemoryRanges                 = nullptr;
PFN_vkGetDeviceMemoryCommitment                    vkGetDeviceMemoryCommitment                    = nullptr;
PFN_vkBindBufferMemory                             vkBindBufferMemory                             = nullptr;
PFN_vkBindImageMemory                              vkBindImageMemory                              = nullptr;
PFN_vkGetBufferMemoryRequirements                  vkGetBufferMemoryRequirements                  = nullptr;
PFN_vkGetImageMemoryRequirements                   vkGetImageMemoryRequirements                   = nullptr;
PFN_vkGetImageSparseMemoryRequirements             vkGetImageSparseMemoryRequirements             = nullptr;
PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
PFN_vkQueueBindSparse                              vkQueueBindSparse                              = nullptr;
PFN_vkCreateFence                                  vkCreateFence                                  = nullptr;
PFN_vkDestroyFence                                 vkDestroyFence                                 = nullptr;
PFN_vkResetFences                                  vkResetFences                                  = nullptr;
PFN_vkGetFenceStatus                               vkGetFenceStatus                               = nullptr;
PFN_vkWaitForFences                                vkWaitForFences                                = nullptr;
PFN_vkCreateSemaphore                              vkCreateSemaphore                              = nullptr;
PFN_vkDestroySemaphore                             vkDestroySemaphore                             = nullptr;
PFN_vkCreateEvent                                  vkCreateEvent                                  = nullptr;
PFN_vkDestroyEvent                                 vkDestroyEvent                                 = nullptr;
PFN_vkGetEventStatus                               vkGetEventStatus                               = nullptr;
PFN_vkSetEvent                                     vkSetEvent                                     = nullptr;
PFN_vkResetEvent                                   vkResetEvent                                   = nullptr;
PFN_vkCreateQueryPool                              vkCreateQueryPool                              = nullptr;
PFN_vkDestroyQueryPool                             vkDestroyQueryPool                             = nullptr;
PFN_vkGetQueryPoolResults                          vkGetQueryPoolResults                          = nullptr;
PFN_vkCreateBuffer                                 vkCreateBuffer                                 = nullptr;
PFN_vkDestroyBuffer                                vkDestroyBuffer                                = nullptr;
PFN_vkCreateBufferView                             vkCreateBufferView                             = nullptr;
PFN_vkDestroyBufferView                            vkDestroyBufferView                            = nullptr;
PFN_vkCreateImage                                  vkCreateImage                                  = nullptr;
PFN_vkDestroyImage                                 vkDestroyImage                                 = nullptr;
PFN_vkGetImageSubresourceLayout                    vkGetImageSubresourceLayout                    = nullptr;
PFN_vkCreateImageView                              vkCreateImageView                              = nullptr;
PFN_vkDestroyImageView                             vkDestroyImageView                             = nullptr;
PFN_vkCreateShaderModule                           vkCreateShaderModule                           = nullptr;
PFN_vkDestroyShaderModule                          vkDestroyShaderModule                          = nullptr;
PFN_vkCreatePipelineCache                          vkCreatePipelineCache                          = nullptr;
PFN_vkDestroyPipelineCache                         vkDestroyPipelineCache                         = nullptr;
PFN_vkGetPipelineCacheData                         vkGetPipelineCacheData                         = nullptr;
PFN_vkMergePipelineCaches                          vkMergePipelineCaches                          = nullptr;
PFN_vkCreateGraphicsPipelines                      vkCreateGraphicsPipelines                      = nullptr;
PFN_vkCreateComputePipelines                       vkCreateComputePipelines                       = nullptr;
PFN_vkDestroyPipeline                              vkDestroyPipeline                              = nullptr;
PFN_vkCreatePipelineLayout                         vkCreatePipelineLayout                         = nullptr;
PFN_vkDestroyPipelineLayout                        vkDestroyPipelineLayout                        = nullptr;
PFN_vkCreateSampler                                vkCreateSampler                                = nullptr;
PFN_vkDestroySampler                               vkDestroySampler                               = nullptr;
PFN_vkCreateDescriptorSetLayout                    vkCreateDescriptorSetLayout                    = nullptr;
PFN_vkDestroyDescriptorSetLayout                   vkDestroyDescriptorSetLayout                   = nullptr;
PFN_vkCreateDescriptorPool                         vkCreateDescriptorPool                         = nullptr;
PFN_vkDestroyDescriptorPool                        vkDestroyDescriptorPool                        = nullptr;
PFN_vkResetDescriptorPool                          vkResetDescriptorPool                          = nullptr;
PFN_vkAllocateDescriptorSets                       vkAllocateDescriptorSets                       = nullptr;
PFN_vkFreeDescriptorSets                           vkFreeDescriptorSets                           = nullptr;
PFN_vkUpdateDescriptorSets                         vkUpdateDescriptorSets                         = nullptr;
PFN_vkCreateFramebuffer                            vkCreateFramebuffer                            = nullptr;
PFN_vkDestroyFramebuffer                           vkDestroyFramebuffer                           = nullptr;
PFN_vkCreateRenderPass                             vkCreateRenderPass                             = nullptr;
PFN_vkDestroyRenderPass                            vkDestroyRenderPass                            = nullptr;
PFN_vkGetRenderAreaGranularity                     vkGetRenderAreaGranularity                     = nullptr;
PFN_vkCreateCommandPool                            vkCreateCommandPool                            = nullptr;
PFN_vkDestroyCommandPool                           vkDestroyCommandPool                           = nullptr;
PFN_vkResetCommandPool                             vkResetCommandPool                             = nullptr;
PFN_vkAllocateCommandBuffers                       vkAllocateCommandBuffers                       = nullptr;
PFN_vkFreeCommandBuffers                           vkFreeCommandBuffers                           = nullptr;
PFN_vkBeginCommandBuffer                           vkBeginCommandBuffer                           = nullptr;
PFN_vkEndCommandBuffer                             vkEndCommandBuffer                             = nullptr;
PFN_vkResetCommandBuffer                           vkResetCommandBuffer                           = nullptr;
PFN_vkCmdBindPipeline                              vkCmdBindPipeline                              = nullptr;
PFN_vkCmdSetViewport                               vkCmdSetViewport                               = nullptr;
PFN_vkCmdSetScissor                                vkCmdSetScissor                                = nullptr;
PFN_vkCmdSetLineWidth                              vkCmdSetLineWidth                              = nullptr;
PFN_vkCmdSetDepthBias                              vkCmdSetDepthBias                              = nullptr;
PFN_vkCmdSetBlendConstants                         vkCmdSetBlendConstants                         = nullptr;
PFN_vkCmdSetDepthBounds                            vkCmdSetDepthBounds                            = nullptr;
PFN_vkCmdSetStencilCompareMask                     vkCmdSetStencilCompareMask                     = nullptr;
PFN_vkCmdSetStencilWriteMask                       vkCmdSetStencilWriteMask                       = nullptr;
PFN_vkCmdSetStencilReference                       vkCmdSetStencilReference                       = nullptr;
PFN_vkCmdBindDescriptorSets                        vkCmdBindDescriptorSets                        = nullptr;
PFN_vkCmdBindIndexBuffer                           vkCmdBindIndexBuffer                           = nullptr;
PFN_vkCmdBindVertexBuffers                         vkCmdBindVertexBuffers                         = nullptr;
PFN_vkCmdDraw                                      vkCmdDraw                                      = nullptr;
PFN_vkCmdDrawIndexed                               vkCmdDrawIndexed                               = nullptr;
PFN_vkCmdDrawIndirect                              vkCmdDrawIndirect                              = nullptr;
PFN_vkCmdDrawIndexedIndirect                       vkCmdDrawIndexedIndirect                       = nullptr;
PFN_vkCmdDispatch                                  vkCmdDispatch                                  = nullptr;
PFN_vkCmdDispatchIndirect                          vkCmdDispatchIndirect                          = nullptr;
PFN_vkCmdCopyBuffer                                vkCmdCopyBuffer                                = nullptr;
PFN_vkCmdCopyImage                                 vkCmdCopyImage                                 = nullptr;
PFN_vkCmdBlitImage                                 vkCmdBlitImage                                 = nullptr;
PFN_vkCmdCopyBufferToImage                         vkCmdCopyBufferToImage                         = nullptr;
PFN_vkCmdCopyImageToBuffer                         vkCmdCopyImageToBuffer                         = nullptr;
PFN_vkCmdUpdateBuffer                              vkCmdUpdateBuffer                              = nullptr;
PFN_vkCmdFillBuffer                                vkCmdFillBuffer                                = nullptr;
PFN_vkCmdClearColorImage                           vkCmdClearColorImage                           = nullptr;
PFN_vkCmdClearDepthStencilImage                    vkCmdClearDepthStencilImage                    = nullptr;
PFN_vkCmdClearAttachments                          vkCmdClearAttachments                          = nullptr;
PFN_vkCmdResolveImage                              vkCmdResolveImage                              = nullptr;
PFN_vkCmdSetEvent                                  vkCmdSetEvent                                  = nullptr;
PFN_vkCmdResetEvent                                vkCmdResetEvent                                = nullptr;
PFN_vkCmdWaitEvents                                vkCmdWaitEvents                                = nullptr;
PFN_vkCmdPipelineBarrier                           vkCmdPipelineBarrier                           = nullptr;
PFN_vkCmdBeginQuery                                vkCmdBeginQuery                                = nullptr;
PFN_vkCmdEndQuery                                  vkCmdEndQuery                                  = nullptr;
PFN_vkCmdResetQueryPool                            vkCmdResetQueryPool                            = nullptr;
PFN_vkCmdWriteTimestamp                            vkCmdWriteTimestamp                            = nullptr;
PFN_vkCmdCopyQueryPoolResults                      vkCmdCopyQueryPoolResults                      = nullptr;
PFN_vkCmdPushConstants                             vkCmdPushConstants                             = nullptr;
PFN_vkCmdBeginRenderPass                           vkCmdBeginRenderPass                           = nullptr;
PFN_vkCmdNextSubpass                               vkCmdNextSubpass                               = nullptr;
PFN_vkCmdEndRenderPass                             vkCmdEndRenderPass                             = nullptr;
PFN_vkCmdExecuteCommands                           vkCmdExecuteCommands                           = nullptr;
#endif