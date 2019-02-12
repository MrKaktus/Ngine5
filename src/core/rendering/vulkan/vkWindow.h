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
#include "core/rendering/vulkan/vkTexture.h"

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
    TextureVK**    swapChainTexture;
    uint32         swapChainImages;
    uint32         swapChainCurrentImageIndex;
    VkQueue        presentQueue;
    VkFence        presentationFence;
    
    WindowVK(VulkanDevice* gpu,
             const std::shared_ptr<CommonDisplay> selectedDisplay,
             const uint32v2 selectedResolution,
             const WindowSettings& settings,
             const std::string title);
    
    virtual void resize(const uint32v2 size);
    virtual Texture* surface(const Semaphore* signalSemaphore = nullptr);
    virtual void present(const Semaphore* waitForSemaphore = nullptr);
    
    void* operator new(size_t size)
    {
        // New and delete are overloaded to make sure that Mutex is always 
        // aligned at proper adress. This also allows application to use 
        // std::unique_ptr() to manage object lifecycle, without growing 
        // unique pointer size (as there is no custom deleter needed).
        return en::allocate<WindowVK>(1, cacheline);
    }

    void operator delete(void* pointer)
    {
        en::deallocate<WindowVK>(static_cast<WindowVK*>(pointer));
    }

    virtual ~WindowVK();
};

} // en::gpu
} // en

#endif

#endif
