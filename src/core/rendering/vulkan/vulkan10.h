/*

 Ngine v5.0
 
 Module      : Vulkan API
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               This module implements OpenGL backend.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_1_0
#define ENG_CORE_RENDERING_VULKAN_1_0

// class VulkanDevice : public GpuDevice
//    {
//    private:

      DeclareFunction( vkCreateInstance )
      DeclareFunction( vkDestroyInstance )
      DeclareFunction( vkEnumeratePhysicalDevices )
      DeclareFunction( vkGetPhysicalDeviceFeatures )
      DeclareFunction( vkGetPhysicalDeviceFormatProperties )
      DeclareFunction( vkGetPhysicalDeviceProperties )
      DeclareFunction( vkGetPhysicalDeviceQueueFamilyProperties )
      DeclareFunction( vkGetPhysicalDeviceMemoryProperties )                
      DeclareFunction( vkCreateDevice )               
      DeclareFunction( vkDestroyDevice )
      DeclareFunction( vkEnumerateInstanceExtensionProperties )
      DeclareFunction( vkEnumerateDeviceExtensionProperties )
      DeclareFunction( vkEnumerateInstanceLayerProperties )
      DeclareFunction( vkEnumerateDeviceLayerProperties )   
      DeclareFunction( vkGetDeviceQueue )                     
      DeclareFunction( vkQueueSubmit )                      
      DeclareFunction( vkQueueWaitIdle )                     
      DeclareFunction( vkDeviceWaitIdle )
      DeclareFunction( vkAllocateMemory )
      DeclareFunction( vkFreeMemory )
      DeclareFunction( vkMapMemory )
      DeclareFunction( vkUnmapMemory )
      DeclareFunction( vkFlushMappedMemoryRanges )
      DeclareFunction( vkInvalidateMappedMemoryRanges )
      DeclareFunction( vkGetDeviceMemoryCommitment )
      DeclareFunction( vkBindBufferMemory )
      DeclareFunction( vkBindImageMemory )
      DeclareFunction( vkGetBufferMemoryRequirements )
      DeclareFunction( vkGetImageMemoryRequirements )
      DeclareFunction( vkGetImageSparseMemoryRequirements )
      DeclareFunction( vkGetPhysicalDeviceSparseImageFormatProperties )
      DeclareFunction( vkQueueBindSparse )
      DeclareFunction( vkCreateFence )
      DeclareFunction( vkDestroyFence )
      DeclareFunction( vkResetFences )
      DeclareFunction( vkGetFenceStatus )
      DeclareFunction( vkWaitForFences )
      DeclareFunction( vkCreateSemaphore )
      DeclareFunction( vkDestroySemaphore )
      DeclareFunction( vkCreateEvent )
      DeclareFunction( vkDestroyEvent )
      DeclareFunction( vkGetEventStatus )
      DeclareFunction( vkSetEvent )
      DeclareFunction( vkResetEvent )
      DeclareFunction( vkCreateQueryPool )
      DeclareFunction( vkDestroyQueryPool )
      DeclareFunction( vkGetQueryPoolResults )
      DeclareFunction( vkCreateBuffer )
      DeclareFunction( vkDestroyBuffer )
      DeclareFunction( vkCreateBufferView )
      DeclareFunction( vkDestroyBufferView )
      DeclareFunction( vkCreateImage )
      DeclareFunction( vkDestroyImage )
      DeclareFunction( vkGetImageSubresourceLayout )
      DeclareFunction( vkCreateImageView )
      DeclareFunction( vkDestroyImageView )
      DeclareFunction( vkCreateShaderModule )
      DeclareFunction( vkDestroyShaderModule )
      DeclareFunction( vkCreatePipelineCache )
      DeclareFunction( vkDestroyPipelineCache )
      DeclareFunction( vkGetPipelineCacheData )
      DeclareFunction( vkMergePipelineCaches )
      DeclareFunction( vkCreateGraphicsPipelines )
      DeclareFunction( vkCreateComputePipelines )
      DeclareFunction( vkDestroyPipeline )
      DeclareFunction( vkCreatePipelineLayout )
      DeclareFunction( vkDestroyPipelineLayout )
      DeclareFunction( vkCreateSampler )
      DeclareFunction( vkDestroySampler )
      DeclareFunction( vkCreateDescriptorSetLayout )
      DeclareFunction( vkDestroyDescriptorSetLayout )
      DeclareFunction( vkCreateDescriptorPool )
      DeclareFunction( vkDestroyDescriptorPool )
      DeclareFunction( vkResetDescriptorPool )
      DeclareFunction( vkAllocateDescriptorSets )
      DeclareFunction( vkFreeDescriptorSets )
      DeclareFunction( vkUpdateDescriptorSets )
      DeclareFunction( vkCreateFramebuffer )
      DeclareFunction( vkDestroyFramebuffer )
      DeclareFunction( vkCreateRenderPass )
      DeclareFunction( vkDestroyRenderPass )
      DeclareFunction( vkGetRenderAreaGranularity )
      DeclareFunction( vkCreateCommandPool )
      DeclareFunction( vkDestroyCommandPool )
      DeclareFunction( vkResetCommandPool )
      DeclareFunction( vkAllocateCommandBuffers )
      DeclareFunction( vkFreeCommandBuffers )
      DeclareFunction( vkBeginCommandBuffer )
      DeclareFunction( vkEndCommandBuffer )
      DeclareFunction( vkResetCommandBuffer )
      DeclareFunction( vkCmdBindPipeline )
      DeclareFunction( vkCmdSetViewport )
      DeclareFunction( vkCmdSetScissor )
      DeclareFunction( vkCmdSetLineWidth )
      DeclareFunction( vkCmdSetDepthBias )
      DeclareFunction( vkCmdSetBlendConstants )
      DeclareFunction( vkCmdSetDepthBounds )
      DeclareFunction( vkCmdSetStencilCompareMask )
      DeclareFunction( vkCmdSetStencilWriteMask )
      DeclareFunction( vkCmdSetStencilReference )
      DeclareFunction( vkCmdBindDescriptorSets )
      DeclareFunction( vkCmdBindIndexBuffer )
      DeclareFunction( vkCmdBindVertexBuffers )
      DeclareFunction( vkCmdDraw )
      DeclareFunction( vkCmdDrawIndexed )
      DeclareFunction( vkCmdDrawIndirect )
      DeclareFunction( vkCmdDrawIndexedIndirect )
      DeclareFunction( vkCmdDispatch )
      DeclareFunction( vkCmdDispatchIndirect )
      DeclareFunction( vkCmdCopyBuffer )
      DeclareFunction( vkCmdCopyImage )
      DeclareFunction( vkCmdBlitImage )
      DeclareFunction( vkCmdCopyBufferToImage )
      DeclareFunction( vkCmdCopyImageToBuffer )
      DeclareFunction( vkCmdUpdateBuffer )
      DeclareFunction( vkCmdFillBuffer )
      DeclareFunction( vkCmdClearColorImage )
      DeclareFunction( vkCmdClearDepthStencilImage )
      DeclareFunction( vkCmdClearAttachments )
      DeclareFunction( vkCmdResolveImage )
      DeclareFunction( vkCmdSetEvent )
      DeclareFunction( vkCmdResetEvent )
      DeclareFunction( vkCmdWaitEvents )
      DeclareFunction( vkCmdPipelineBarrier )
      DeclareFunction( vkCmdBeginQuery )
      DeclareFunction( vkCmdEndQuery )
      DeclareFunction( vkCmdResetQueryPool )
      DeclareFunction( vkCmdWriteTimestamp )
      DeclareFunction( vkCmdCopyQueryPoolResults )
      DeclareFunction( vkCmdPushConstants )
      DeclareFunction( vkCmdBeginRenderPass )
      DeclareFunction( vkCmdNextSubpass )
      DeclareFunction( vkCmdEndRenderPass )
      DeclareFunction( vkCmdExecuteCommands )

      // Windowing System Interface - KHRONOS extension
      
      // VK_KHR_surface - Interface extension
      
      // VK_KHR_swapchain 
      DeclareFunction( vkCreateSwapchainKHR )
      DeclareFunction( vkDestroySwapchainKHR )
      DeclareFunction( vkGetSwapchainImagesKHR )
      DeclareFunction( vkAcquireNextImageKHR )
      DeclareFunction( vkQueuePresentKHR )
      
      // VK_KHR_display
      DeclareFunction( vkGetPhysicalDeviceDisplayPropertiesKHR )
      DeclareFunction( vkGetPhysicalDeviceDisplayPlanePropertiesKHR )
      DeclareFunction( vkGetDisplayPlaneSupportedDisplaysKHR )
      DeclareFunction( vkGetDisplayModePropertiesKHR )
      DeclareFunction( vkCreateDisplayModeKHR )
      DeclareFunction( vkGetDisplayPlaneCapabilitiesKHR )
      DeclareFunction( vkCreateDisplayPlaneSurfaceKHR )
      
      // VK_KHR_display_swapchain
      DeclareFunction( vkCreateSharedSwapchainsKHR )
      
      // VK_KHR_maintenance1
      DeclareFunction( vkTrimCommandPoolKHR )

      // Windowing System Interface - OS Specyific extension
      
      // VK_KHR_xlib_surface
      // VK_KHR_xcb_surface
      // VK_KHR_wayland_surface
      // VK_KHR_mir_surface
      // VK_KHR_android_surface
      
      // VK_KHR_win32_surface - Interface extension

      // Extensions
      
      // VK_EXT_debug_report
      // VK_NV_glsl_shader
      // VK_IMG_filter_cubic
      // VK_EXT_debug_marker
      // VK_AMD_rasterization_order
      // VK_AMD_shader_trinary_minmax
      // VK_AMD_shader_explicit_vertex_parameter
      // VK_AMD_gcn_shader
      // VK_NV_dedicated_allocation

#endif
