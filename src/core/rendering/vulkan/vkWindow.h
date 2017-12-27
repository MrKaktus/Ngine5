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

#ifndef ENG_CORE_RENDERING_VULKAN_WINDOW
#define ENG_CORE_RENDERING_VULKAN_WINDOW

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#if defined(EN_PLATFORM_ANDROID)
#include "core/rendering/android/andWindow.h"
#endif
#if defined(EN_PLATFORM_LINUX)
#include "core/rendering/linux/linWindow.h"
#endif
#if defined(EN_PLATFORM_WINDOWS)
#include "core/rendering/windows/winWindow.h"
#endif

namespace en
{
   namespace gpu
   {
   class VulkanDevice;
   class Texture;

#if defined(EN_PLATFORM_ANDROID)
   class WindowVK : public andWindow
#endif
#if defined(EN_PLATFORM_LINUX)
   class WindowVK : public linWindow
#endif
#if defined(EN_PLATFORM_WINDOWS)
   class WindowVK : public winWindow
#endif
      {
      public:
      VulkanDevice*  gpu;
      VkSurfaceKHR   swapChainSurface; 
      VkSwapchainKHR swapChain;
      shared_ptr<Texture>*  swapChainTexture;
      uint32         swapChainImages;
      uint32         swapChainCurrentImageIndex;
      VkQueue        presentQueue;
      VkFence        presentationFence;

      WindowVK(VulkanDevice* gpu,
               const shared_ptr<CommonDisplay> selectedDisplay,
               const uint32v2 selectedResolution,
               const WindowSettings& settings,
               const string title);

      virtual void resize(const uint32v2 size);
      virtual shared_ptr<Texture> surface(const Semaphore* signalSemaphore = nullptr);
      virtual void present(const Semaphore* waitForSemaphore = nullptr);
      
      virtual ~WindowVK();
      };
   }
}

#endif

#endif
