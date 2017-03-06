/*

 Ngine v5.0
 
 Module      : Vulkan Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkWindow.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/vulkan/vkSynchronization.h"
#include "core/rendering/vulkan/vkTexture.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   { 
   WindowVK::WindowVK(VulkanDevice* _gpu,
                      const Ptr<CommonDisplay> selectedDisplay,
                      const uint32v2 selectedResolution,
                      const WindowSettings& settings,
                      const string title) :
      gpu(_gpu),
      // Create native OS window or assert.
#if defined(EN_PLATFORM_ANDROID)
      andWindow(ptr_dynamic_cast<andDisplay, CommonDisplay>(selectedDisplay), selectedResolution, settings, title)
#endif
#if defined(EN_PLATFORM_LINUX)
      linWindow(ptr_dynamic_cast<linDisplay, CommonDisplay>(selectedDisplay), selectedResolution, settings, title)
#endif
#if defined(EN_PLATFORM_WINDOWS)
      winWindow(ptr_dynamic_cast<winDisplay, CommonDisplay>(selectedDisplay), selectedResolution, settings, title)
#endif
   {
   swapChainTexture           = nullptr;
   swapChainImages            = 0;
   swapChainCurrentImageIndex = 0;

   // Window > Surface > Swap-Chain > Device - connection

   // Be sure device is idle before creating Swap-Chain
   Profile( gpu, vkDeviceWaitIdle(gpu->device) )

   // Create Swap-Chain surface attached to Window
   //----------------------------------------------

   VkSurfaceCapabilitiesKHR swapChainCapabilities;
     
#if defined(EN_PLATFORM_WINDOWS)
   VkWin32SurfaceCreateInfoKHR winCreateInfo;
   winCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
   winCreateInfo.pNext     = nullptr;
   winCreateInfo.flags     = 0;           // VkWin32SurfaceCreateFlagsKHR - Reserved.
   winCreateInfo.hinstance = AppInstance; // HINSTANCE
   winCreateInfo.hwnd      = hWnd;        // HWND

   VulkanAPI* api = raw_reinterpret_cast<VulkanAPI>(&en::Graphics);
   Profile( api, vkCreateWin32SurfaceKHR(api->instance,
                                         &winCreateInfo, nullptr, &swapChainSurface) )
#else
   // TODO: Implement OS Specific part for other platforms.
   assert( 0 );
#endif

   // Query capabilities of Swap-Chain surface for this device
   Profile( api, vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->handle, swapChainSurface, &swapChainCapabilities) )
   if (api->lastResult[0] != VK_SUCCESS)  // TODO FIXME! Assumes Thread 0 !
      {
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Cannot query Swap-Chain surface capabilities.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Verify that queue family picked to handle QueueType::Universal is supporting present calls on this Window.
   VkBool32 supportPresent = VK_FALSE;
   Profile( api, vkGetPhysicalDeviceSurfaceSupportKHR(gpu->handle, gpu->queueTypeToFamily[underlyingType(QueueType::Universal)], swapChainSurface, &supportPresent) )
   if (supportPresent == VK_FALSE)
      {
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Queue Family supporting Present is different than queue family handling Universal queue type!\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Engine assumes that queue family handling QueueType::Universal is supporting Present.
   // Presenting is always performed from first queue of type QueueType::Universal (queue 0).
   ProfileNoRet( gpu, vkGetDeviceQueue(gpu->device, gpu->queueTypeToFamily[underlyingType(QueueType::Universal)], 0u, &presentQueue) )

   // Calculate amount of backing images in Swap-Chain
   //--------------------------------------------------
   
   // Typical Swap-Chain consist of 3 surfaces, one is presented on display, next is
   // queued to be presented, and the third one is used by application for rendering.
   // We use 4, to be able to use Mailbox mode rendering (where we can replace last
   // waiting frame with more recent one at any time, and still have one frame to query).
   swapChainImages = max(4, swapChainCapabilities.minImageCount);
   if (swapChainCapabilities.maxImageCount)
      swapChainImages = min(swapChainImages, swapChainCapabilities.maxImageCount);
   
   // TODO: In future check what's the most optimal count and what should be used for VR.

   // Determine PixelFormat and Color Space
   //---------------------------------------
   
   VkFormat        swapChainFormat     = TranslateTextureFormat[underlyingType(settings.format)];
   VkColorSpaceKHR swapChainColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
      
   // Query count of available Pixel Formats supported by this device, and matching destination Window
   uint32 formats = 0;
   Profile( api, vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->handle, swapChainSurface, &formats, nullptr) )
   if ( (api->lastResult[0] != VK_SUCCESS) ||   // TODO FIXME! Assumes Thread 0 !
        (formats == 0) )
      {
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Cannot query Swap-Chain surface Pixel Formats count.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Query types of all available device Pixel Formats for that surface
   VkSurfaceFormatKHR* deviceFormats = new VkSurfaceFormatKHR[formats];
   Profile( api, vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->handle, swapChainSurface, &formats, &deviceFormats[0]) )
   if (api->lastResult[0] != VK_SUCCESS)   // TODO FIXME! Assumes Thread 0 !
      {
      delete [] deviceFormats;
      
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Cannot query list of Swap-Chain surface supported Pixel Formats.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }
   
   // If device specifies only one format VK_FORMAT_UNDEFINED it means
   // there is no preffered Pixel Formats for this window. In such case
   // we can use Pixel Format and Color Space requested by the app.
   // Otherwise we need to check if requested properties are on the
   // list of available ones, and fail if that's not true.
   bool requestedFormatSupported = true;
   if ( (formats > 1) ||
        (deviceFormats[0].format != VK_FORMAT_UNDEFINED) )
      {
      requestedFormatSupported = false;
      
      for(uint32 i=0; i<formats; ++i)
         if ( (deviceFormats[i].format     == swapChainFormat) &&
              (deviceFormats[i].colorSpace == swapChainColorSpace) )
            {
            requestedFormatSupported = true;
            break;
            }
      }

   delete [] deviceFormats;
   
   if (!requestedFormatSupported)
      {
      string info;
      info += "ERROR: Requested Pixel Format is not supported by this device.\n";
      Log << info.c_str();
      return;
      }

   // Verify requested Swap-Chain resolution
   //----------------------------------------

   // Final determined resolution of backing surface (may be smaller than window resolution)
   uint32v2 swapChainResolution = selectedResolution;

   // In windowed mode Swap-Chain resolution can differ from Window size (be smaller)
   if ( (settings.mode != Fullscreen) &&
        (settings.resolution.width  > 0) &&
        (settings.resolution.width  < selectedResolution.width) &&
        (settings.resolution.height > 0) &&
        (settings.resolution.height < selectedResolution.height) )
      {
      // If Windowing System allows scaling of Swap-Chain surfaces to window size
      if ( (swapChainCapabilities.currentExtent.width  == 0xFFFFFFFF) &&
           (swapChainCapabilities.currentExtent.height == 0xFFFFFFFF) )
         {
         // If requested resolution is fitting in allowed range, it's accepted.
         if ( (settings.resolution.width  >= swapChainCapabilities.minImageExtent.width)  ||
              (settings.resolution.width  <= swapChainCapabilities.maxImageExtent.width)  ||
              (settings.resolution.height >= swapChainCapabilities.minImageExtent.height) ||
              (settings.resolution.height <= swapChainCapabilities.maxImageExtent.height) )
            swapChainResolution = settings.resolution;
         }
         
      // Report warning if couldn't set requested Swap-Chain resolution
      if (swapChainResolution == selectedResolution)
         Log << "Warning: Requested Swap-Chain resolution is unsupported. Using window resolution instead.\n";
      }

   // Verify supported usage
   //------------------------
   
   // Swap-Chain images need to support usage as Color Attachment.
   VkImageUsageFlags swapChainUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
   if (!checkBitmask(swapChainCapabilities.supportedUsageFlags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))
      {
      Log << "ERROR: Swap-Chain is not supporting mandatory color attachment usage!\n";
      assert( 0 );
      }
      
   // TODO: Consider requesting additional usages:
   // VK_IMAGE_USAGE_TRANSFER_SRC_BIT - For screenshots of final frame
   // VK_IMAGE_USAGE_TRANSFER_DST_BIT - For quick blit of splash screens

   // Determine behaviour when device orientation is changed
   //--------------------------------------------------------

   // By default we don't want any transformation to take place
   VkSurfaceTransformFlagBitsKHR swapChainTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
   if (!checkBitmask(swapChainCapabilities.supportedTransforms, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
      {
      // If we need to apply a transform, use current one in the system
      swapChainTransform = swapChainCapabilities.currentTransform;
      }

   // Determine Presenting Mode
   //---------------------------

   uint32 modes = 0; // Presentation Modes
   VkPresentModeKHR* presentationMode = nullptr;
   
   // Query device Presentation Modes count
   Profile( api, vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->handle, swapChainSurface, &modes, nullptr) )
   if ( (api->lastResult[0] != VK_SUCCESS) ||    // TODO FIXME! Assumes Thread 0 !
        (modes == 0) )
      {
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Cannot query device Presentation Modes count.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Query device Presentation Modes details
   presentationMode = new VkPresentModeKHR[modes];
   Profile( api, vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->handle, swapChainSurface, &modes, &presentationMode[0]) )
   if (api->lastResult[0] != VK_SUCCESS)   // TODO FIXME! Assumes Thread 0 !
      {
      delete [] presentationMode;
      
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Cannot query list of device Presentation Modes.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Select presentation mode. Prefer immediate dispatch to display with VSync.
   // If that's not possible, we will deal with internal queue of Swap-Chain images.
   bool selectedMode = false;
   VkPresentModeKHR swapChainPresentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
   for(uint32 i=0; i<modes; ++i)
      if (presentationMode[i] == VK_PRESENT_MODE_MAILBOX_KHR)
         {
         selectedMode = true;
         break;
         }

   if (!selectedMode)
      {
      for(uint32 i=0; i<modes; ++i)
         if (presentationMode[i] == VK_PRESENT_MODE_FIFO_KHR)
            {
            swapChainPresentationMode = VK_PRESENT_MODE_FIFO_KHR;
            selectedMode = true;
            break;
            }

      if (!selectedMode)
         {
         Log << "ERROR: Cannot specify Swap-Chain presentation method!\n";
         assert( 0 );
         }
      }
      
   delete [] presentationMode;

   // TODO: This should be synchronized with calculation of amount of backing images count ??
   //
   // TODO: Consider also:
   //       VK_PRESENT_MODE_IMMEDIATE_KHR    - No VSync
   //       VK_PRESENT_MODE_FIFO_RELAXED_KHR - Prevent Stuttering if frame was late ( for VR ? )

   // Create Swap-Chain connecting Window to Device
   //-----------------------------------------------
   
   VkSwapchainCreateInfoKHR createInfo;
   createInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
   createInfo.pNext                 = nullptr;
   createInfo.flags                 = 0; // Reserved - VkSwapchainCreateFlagsKHR
   createInfo.surface               = swapChainSurface;
   createInfo.minImageCount         = swapChainImages;
   createInfo.imageFormat           = swapChainFormat;
   createInfo.imageColorSpace       = swapChainColorSpace;
   createInfo.imageExtent           = { swapChainResolution.width, swapChainResolution.height };
   createInfo.imageArrayLayers      = 1;                    // TODO: This may change for VR support
   createInfo.imageUsage            = swapChainUsage;
   createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE; // Only single Queue Family can access Swap-Chain buffers
   createInfo.queueFamilyIndexCount = 0;
   createInfo.pQueueFamilyIndices   = nullptr;
   createInfo.preTransform          = swapChainTransform;
   createInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Doesn't support transparent windows
   createInfo.presentMode           = swapChainPresentationMode;
   createInfo.clipped               = VK_TRUE;
   createInfo.oldSwapchain          = VK_NULL_HANDLE;
 
   Profile( gpu, vkCreateSwapchainKHR(gpu->device, &createInfo, nullptr, &swapChain) )

   // Create array of textures backed by Swap-Chain images
   //------------------------------------------------------
 
   // Ensure that amount of images in swap-chain is equal to requested one
   uint32 swapChainActualImagesCount = 0;
   Profile( gpu, vkGetSwapchainImagesKHR(gpu->device, swapChain, &swapChainActualImagesCount, nullptr) )
   assert( swapChainActualImagesCount == swapChainImages );

   // Request handles to swap-chain images
   VkImage* swapChainImageHandles = new VkImage[swapChainImages];
   Profile( gpu, vkGetSwapchainImagesKHR(gpu->device, swapChain, &swapChainImages, swapChainImageHandles) )

   // Create textures backed with handles
   TextureState textureState = TextureState(TextureType::Texture2D, 
                                            settings.format, 
                                            TextureUsage::RenderTargetWrite,
                                            swapChainResolution.width,
                                            swapChainResolution.height);

   swapChainTexture = new Ptr<Texture>[swapChainImages];
   for(uint32 i=0; i<swapChainImages; ++i)
      {
      Ptr<TextureVK> texture = new TextureVK(gpu, textureState);
      texture->handle = swapChainImageHandles[i];
      swapChainTexture[i] = ptr_reinterpret_cast<Texture>(&texture);
      }

   delete [] swapChainImageHandles;

   // Create Fence that will be signaled when acquired surface is safe to use
   //------------------------------------------------------------------------- 
         
   VkFenceCreateInfo fenceInfo;
   fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
   fenceInfo.pNext = nullptr;
   fenceInfo.flags = 0u; 
         
   Profile( gpu, vkCreateFence(gpu->device, &fenceInfo, nullptr, &presentationFence) )

   _resolution = swapChainResolution;
   }
   
   WindowVK::~WindowVK()
   {
   // Be sure device is idle before destroying the Window connection
   Profile( gpu, vkDeviceWaitIdle(gpu->device) )

   // Release presentation fence
   ProfileNoRet( gpu, vkDestroyFence(gpu->device, presentationFence, nullptr) )

   // Detach swap-chain surfaces from texture containers
   // TODO: Do we need to present/release currently acquired surface first ?
   //for(uint32 i=0; i<swapChainImages; ++i)
   //   ptr_reinterpret_cast<TextureVK>(&swapChainTexture[i])->handle = VK_NULL_HANDLE;
 
   // TODO: Do we release swap-chain surfaces in any particular way?

   // Release Swap-Chain surfaces attached to Swap-Chain textures in their descriptors.
   delete [] swapChainTexture;

   VulkanAPI* api = raw_reinterpret_cast<VulkanAPI>(&en::Graphics);

   ProfileNoRet( api, vkDestroySurfaceKHR(api->instance,
                                          swapChainSurface, nullptr) )
   }
   
   void WindowVK::resize(const uint32v2 size)
   {
   // TODO: Finish !
   assert( 0 );
   }

   Ptr<Texture> WindowVK::surface(const Ptr<Semaphore> signalSemaphore)
   {
   if (needNewSurface)
      {
      surfaceAcquire.lock();

      if (needNewSurface)
         {
         uint32 thread = Scheduler.core();

         // v1.0.38 p663
         //
         // " The vkCmdWaitEvents or vkCmdPipelineBarrier used to transition the image away from VK_IMAGE_
         //   LAYOUT_PRESENT_SRC_KHR layout must have dstStageMask and dstAccessMask parameters set based on the
         //   next use of the image. The application must use implicit ordering guarantees and execution dependencies to prevent the
         //   image transition from occurring before the semaphore passed to vkAcquireNextImageKHR has signaled. "
         //
         // v1.0.36 p648
         //
         // " After acquiring a
         //   presentable image and before modifying it, the application must use a synchronization primitive to ensure that the
         //   presentation engine has finished reading from the image. The application can then transition the image’s layout, 
         //   ... "
         //
         // " Note
         //   This allows the platform to handle situations which require out-of-order return of images after presentation. At the
         //   same time, it allows the application to generate command buffers referencing all of the images in the swapchain
         //   at initialization time, rather than in its main loop. "
         //
         // " . . . must be transitioned away from this layout after calling vkAcquireNextImageKHR.  "
         //
         // Pseudocode:
         //
         // window->surface(postSemaphore);
         // command->start(postSemaphore);
         //
         // // Transition Swap-Chain surface from presentation layout to rendering destination
         // command->barrier(swapChainTexture[swapChainCurrentImageIndex], 
         //                  uint32v2(0,1),  // First mipmap
         //                  uint32v2(0,1)); // First layer
         //                  0u, // Current Access
         //                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // New Access
         //                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // Current Layout
         //                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // New Layout
         //                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,     // Transition after this stage
         //                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);    // Transition before this stage
         //
         SemaphoreVK* signal = raw_reinterpret_cast<SemaphoreVK>(&signalSemaphore);

         // Acquire one of the Swap-Chain surfaces for rendering
         Profile( gpu, vkAcquireNextImageKHR(gpu->device, 
                                             swapChain, 
                                             UINT64_MAX,     // wait time in nanoseconds
                                             signal->handle, // semaphore to signal when presentation engine finishes reading from this surface, command buffer will wait on it
                                             VK_NULL_HANDLE, // presentationFence, <- Don't actively wait for now.
                                             &swapChainCurrentImageIndex) )
         
         // Ensure that engine is recreating Swap-Chain on window resize
         assert( gpu->lastResult[thread] != VK_ERROR_OUT_OF_DATE_KHR );

         //// Active wait for presentation engine to finish reading from Swap-Chain surface to display panel
         //gpu->lastResult[thread] = VK_NOT_READY;
         //while(gpu->lastResult[thread] != VK_SUCCESS)
         //   {
         //   Profile( gpu, vkGetFenceStatus(gpu->device, presentationFence) )
         //   }

         //// Reset fence for reuse
         //Profile( gpu, vkResetFences(gpu->device, 1u, &presentationFence) )

         needNewSurface = false;
         }

      surfaceAcquire.unlock();
      }

   return swapChainTexture[swapChainCurrentImageIndex];
   }

   // Data needed to transition to present :
   //
   // command buffer handle
   // const Ptr<Texture> _texture, 
   // barrier.srcAccessMask 
   // barrier.oldLayout     
   // VkPipelineStageFlags afterStage;  // Transition after this stage
   // VkPipelineStageFlags beforeStage; // Transition before this stage


   // Presents current surface, after all work encoded on given Commnad Buffer is done
   void WindowVK::present(const Ptr<Semaphore> waitForSemaphore) // const Ptr<CommandBuffer> command ? <- pass command buffer in ??
   {
   surfaceAcquire.lock();
   if (!needNewSurface)
      {
      // v1.0.36 p662
      //
      // " When transitioning the image to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, there is no need to delay subsequent
      //   processing, or perform any visibility operations (as vkQueuePresentKHR performs automatic visibility
      //   operations). To achieve this, the dstAccessMask member of the VkImageMemoryBarrier should be set
      //   to 0, and the dstStageMask parameter should be set to VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_
      //   BIT "
      // 
      // Pseudocode:
      //
      // command->barrier(swapChainTexture[swapChainCurrentImageIndex], 
      //                  uint32v2(0,1),  // First mipmap
      //                  uint32v2(0,1)); // First layer
      //                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // Current Access
      //                  0u,
      //                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // Current Layout
      //                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // New Layout
      //                  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,     // Transition after this stage
      //                  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);    // Transition before this stage
      // 
      // Ptr<Semaphore> waitForSemaphore = gpu->createSemaphore();
      // command->commit(waitForSemaphore);
      // window->present(preSemaphore); // Wait on semaphore

      VkDisplayPresentInfoKHR displayInfo;
      displayInfo.sType      = VK_STRUCTURE_TYPE_DISPLAY_PRESENT_INFO_KHR;
      displayInfo.pNext      = nullptr;
      displayInfo.srcRect    = { (sint32)_resolution.width, (sint32)_resolution.height }; // Region of surface to present. Smaller or equal to Swap-Chain surface size.
      displayInfo.dstRect    = { (sint32)_size.width, (sint32)_size.height };             // Region of display area to render to. 
      displayInfo.persistent = VK_TRUE;                                   // If display has it's own copy of image, it will keep displaying it until 
                                                                          // new image arrives from presentation engine.
      VkPresentInfoKHR info;
      info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      info.pNext              = _mode == WindowMode::Fullscreen ? &displayInfo : nullptr; // If using VK_KHR_display_swapchain, pass additional information.
      info.waitSemaphoreCount = 0u;      
      info.pWaitSemaphores    = nullptr;         // At least one Semaphore, indicating that rendering to presentation surface is done (if not used waitUntilComplete).
      info.swapchainCount     = 1u;              // Can present Swap-Chains of multiple windows at the same time (then we pass array of those and their surface id's).
      info.pSwapchains        = &swapChain;      
      info.pImageIndices      = &swapChainCurrentImageIndex;
      info.pResults           = nullptr;         // Results per Swap-Chain. We present only one so general function call result is enough.

      if (waitForSemaphore)
         {
         SemaphoreVK* wait = raw_reinterpret_cast<SemaphoreVK>(&waitForSemaphore);
         
         info.waitSemaphoreCount = 1u;
         info.pWaitSemaphores    = &wait->handle;
         }

      Profile( gpu, vkQueuePresentKHR(presentQueue, &info) )
      needNewSurface = true;
      _frame++;

      // TODO: Measure here Window rendering time (average between present calls) ?
      }
   surfaceAcquire.unlock();
   }


   Ptr<Window> VulkanDevice::createWindow(const WindowSettings& settings, const string title)
   {
   // Select destination display
   Ptr<CommonDisplay> display;
   if (settings.display)
      display = ptr_dynamic_cast<CommonDisplay, Display>(settings.display);
   else
      display = ptr_dynamic_cast<CommonDisplay, Display>(Graphics->primaryDisplay());
      
   // Checking if app wants to use default resolution
   bool useNativeResolution = false;
   if (settings.size.width  == 0 ||
       settings.size.height == 0)
      useNativeResolution = true;

   // Select resolution to use (to which display should switch in Fullscreen mode)
   uint32v2 selectedResolution = settings.size;
   if (useNativeResolution)
      selectedResolution = display->_resolution;

   // Verify apps custom size in Fullscreen mode is supported, and that app is not using
   // size and resolution at once in this mode.
   if (settings.mode == Fullscreen)
      {
      if (!useNativeResolution)
         {
         // Verify that requested resolution is supported by display
         bool validResolution = false;
         for(uint32 i=0; i<display->modesCount; ++i)
            if (selectedResolution == display->modeResolution[i])
               validResolution = true;

         if (!validResolution)
            {
            Log << "Error! Requested window size for Fullscreen mode is not supported by selected display." << endl;
            return Ptr<Window>(nullptr);
            }
         }
         
      // Cannot use Display HW Scaler to emulate smaller resolution and Swap-Chain upsampling at the same time.
      if (settings.resolution.x != 0 ||
          settings.resolution.y != 0)
         {
         Log << "Error! In Fullscreen mode resolution shouldn't be used, use size setting instead." << endl;
         return Ptr<Window>(nullptr);
         }
      }

   Ptr<WindowVK> ptr = new WindowVK(this, display, selectedResolution, settings, title);
   return ptr_dynamic_cast<Window>(ptr);
   }

   }
}
#endif
