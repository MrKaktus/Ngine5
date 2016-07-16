/*

 Ngine v5.0
 
 Module      : Vulkan GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkDevice.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/log/log.h"
#include "core/utilities/memory.h"

namespace en
{
   namespace gpu
   { 
   // Checks Vulkan error state
   bool IsError(const VkResult result)
   {
   sint32 value = static_cast<sint32>(result);
   if (value >= 0)
      return false;
   
   // Compose error message
   string info;
   info += "ERROR: Vulkan error: ";
   if (result == VK_ERROR_OUT_OF_HOST_MEMORY)
      info += "VK_ERROR_OUT_OF_HOST_MEMORY.\n";
   else
   if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY)
      info += "VK_ERROR_OUT_OF_DEVICE_MEMORY.\n";
   else
   if (result == VK_ERROR_INITIALIZATION_FAILED)
      info += "VK_ERROR_INITIALIZATION_FAILED.\n";
   else
   if (result == VK_ERROR_DEVICE_LOST)
      info += "VK_ERROR_DEVICE_LOST.\n";
   else
   if (result == VK_ERROR_MEMORY_MAP_FAILED)
      info += "VK_ERROR_MEMORY_MAP_FAILED.\n";
   else
   if (result == VK_ERROR_LAYER_NOT_PRESENT)
      info += "VK_ERROR_LAYER_NOT_PRESENT.\n";
   else
   if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
      {
      info += "VK_ERROR_EXTENSION_NOT_PRESENT.\n";
      info += "       Cannot find a specified extension library.\n";
      info += "       Make sure your layers path is set appropriately.\n";
      }
   else
   if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
      {
      info += "VK_ERROR_INCOMPATIBLE_DRIVER.\n";
      info += "       Cannot find a compatible Vulkan installable client driver (ICD).\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      }

   Log << info.c_str();
   return true; 
   }
   
   // Checks Vulkan warning state
   bool IsWarning(const VkResult result)
   {
   sint32 value = static_cast<sint32>(result);
   if (value == 0)
      return false;
   
   // Compose error message
   string info;
   info += "WARNING: Vulkan error: ";
   if (result == VK_NOT_READY)
      info += "VK_NOT_READY.\n";
   else
   if (result == VK_TIMEOUT)
      info += "VK_TIMEOUT.\n";
   else
   if (result == VK_EVENT_SET)
      info += "VK_EVENT_SET.\n";
   else
   if (result == VK_EVENT_RESET)
      info += "VK_EVENT_RESET.\n";
   else
   if (result == VK_INCOMPLETE)
      info += "VK_INCOMPLETE.\n";
   
   Log << info.c_str();
   return true; 
   }
      
   void unbindedVulkanFunctionHandler(...)
   {
   Log << "ERROR: Called unbinded Vulkan function.\n";
   assert(0);
   }

   // MACROS: Safe Vulkan function binding with fallback
   //         Both Macros should be used only inside VulkanAPI and VulkanDevice class methods.
   #define bindInstanceFunction(name)                                    \
   {                                                                     \
   vk##name = (PFN_vk##name) vkGetInstanceProcAddr(instance, "vk"#name); \
   if (!vk##name)                                                        \
      {                                                                  \
      vk##name = (PFN_vk##name) &unbindedVulkanFunctionHandler;          \
      Log << "Error: Cannot bind function vk" << #name << endl;          \
      };                                                                 \
   }

   #define bindDeviceFunction(name)                                      \
   {                                                                     \
   vk##name = (PFN_vk##name) vkGetDeviceProcAddr(device, "vk"#name);     \
   if (!vk##name)                                                        \
      {                                                                  \
      vk##name = (PFN_vk##name) &unbindedVulkanFunctionHandler;          \
      Log << "Error: Cannot bind function vk" << #name << endl;          \
      };                                                                 \
   }




#if defined(EN_PLATFORM_WINDOWS)
   winDisplay::winDisplay()
      observedResolution(),
      modes(0),
      index(0),
      resolutionChanged(false),
      CommonDisplay()
   {
   }

   winDisplay::~winDisplay()
   {
   // Restore original resolution
   if (resolutionChanged)
      {
      // TODO: Check if that's not happening by default when window is destroyed
      }
   }
#endif

#if defined(EN_PLATFORM_WINDOWS)
   winWindow::winWindow(const Ptr<winDisplay> selectedDisplay,
                        const uint32v2 selectedResolution,
                        const WindowSettings& settings,
                        const string title) :
      CommonWindow()
   {
   HINSTANCE AppInstance; // Application handle
   WNDCLASS  Window;      // Window class
   DWORD     Style;       // Window style
   DWORD     ExStyle;     // Window extended style
   RECT      WindowRect;  // Window rectangle
      
   // Get this application instance
   AppInstance          = (HINSTANCE)GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE);  // GWL_HINSTANCE is deprecated in x64 environment
     
   // Window settings
   Window.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Have its own Device Context and also cannot be resized. Oculus uses CLASSDC which is not thread safe!    
   Window.lpfnWndProc   = (WNDPROC) WinEvents;                // Procedure that handles OS evets for this window
   Window.cbClsExtra    = 0;                                  // No extra window data
   Window.cbWndExtra    = 0;                                  //
   Window.hInstance     = AppInstance;                        // Handle to instance of program that this window belongs to
   Window.hIcon         = LoadIcon(NULL, IDI_WINLOGO);        // Load default icon
   Window.hCursor       = LoadCursor(NULL, IDC_ARROW);        // Load arrow pointer
   Window.hbrBackground = NULL;                               // No background (Direct3D/OpenGL/Vulkan will handle this)
   Window.lpszMenuName  = NULL;                               // No menu
   Window.lpszClassName = L"WindowClass";                     // Class name
   
   // Window size      
   WindowRect.left      = (long)0;      
   WindowRect.right     = (long)selectedResolution.width;
   WindowRect.top       = (long)0;    
   WindowRect.bottom    = (long)selectedResolution.height;
     
   // Registering Window Class
   if (!RegisterClass(&Window))	
      {					
      Log << "Error! Cannot register window class." << endl;
      assert( 0 );
      }
      
   // Preparing for displays native fullscreen mode
   if (settings.mode == Fullscreen)
      {
      // Display Device settings
      DEVMODE DispDevice;
      memset(&DispDevice, 0, sizeof(DispDevice));       // Clearing structure
      DispDevice.dmSize       = sizeof(DispDevice);     // Size of this structure
      DispDevice.dmBitsPerPel = 32;                     // bpp
      DispDevice.dmPelsWidth  = selectedResolution.width;
      DispDevice.dmPelsHeight = selectedResolution.height;
      DispDevice.dmFields     = DM_BITSPERPEL |
                                DM_PELSWIDTH  | 
                                DM_PELSHEIGHT;          // Sets which fields were filled

      // Specifies which display to use
      DISPLAY_DEVICE Device;
      memset(&Device, 0, sizeof(Device));
      Device.cb = sizeof(Device);
      assert( EnumDisplayDevices(nullptr, selectedDisplay->index, &Device, EDD_GET_DEVICE_INTERFACE_NAME) );
      memcpy(&DispDevice.dmDeviceName, &Device.DeviceName, sizeof(DispDevice.dmDeviceName));

      // Changing Display settings
      if (ChangeDisplaySettingsEx(Device.DeviceName, &DispDevice, nullptr, CDS_FULLSCREEN, nullptr) != DISP_CHANGE_SUCCESSFUL)
         {					
         Log << "Error! Cannot change display settings for fullscreen." << endl;
         assert( 0 );
         }
 
      // Setting additional data
      Style   = WS_POPUP;                               // Window style
      ExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;        // Window extended style
      ShowCursor(FALSE);                                // Hide mouse 
      }
   else
      {
      // Preparing for desktop window (adding size of borders and bar)
      if (settings.mode == Windowed)
         {
         Style   = WS_CLIPSIBLINGS |                       // \_Prevents from overdrawing
                   WS_CLIPCHILDREN |                       // / by other windows
                   WS_POPUP        | 
                   WS_OVERLAPPEDWINDOW;                    // Window style
         ExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;     // Window extended style  
      
         AdjustWindowRectEx(&WindowRect, Style, FALSE, ExStyle);  // Calculates true size of window with bars and borders
         }
      else // Borderless window
         {
         Style   = WS_POPUP;                               // Window style                // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
         ExStyle = 0;                                      // Window extended style  
         }
      }

   // Creating Window in modern way
   wstring windowTitle = wstring(title.begin(),title.end());
   hWnd = CreateWindowEx(
      ExStyle,                             // Extended style 
      L"WindowClass",                      // Window class name
      (LPCWSTR)windowTitle.c_str(),        // Title of window            
      Style,                               // Additional styles
      settings.position.x, settings.position.y, // Position on desktop    <-- TODO: Shouldn't be display.pos + settings.pos? Is this on Desktop or on Window ?
      WindowRect.right  - WindowRect.left, // True window widht
      WindowRect.bottom - WindowRect.top,  // True window height
      nullptr, //GetDesktopWindow(),       // Desktop is parent window
      nullptr,                             // No menu
      AppInstance,                         // Handle to this instance of program
      nullptr );                           // Won't pass anything
   
   // Check if window was created
   if (hWnd == nullptr)
      {
      Log << "Error! Cannot create window.\n";
      return false;
      }
   
   _display  = ptr_dynamic_cast<CommonDisplay, winDisplay>(selectedDisplay);
   _position = settings.position;
   _size     = selectedResolution;
 //_resolution will be set by child class implementing given Graphics API Swap-Chain
   }
#endif

   bool winWindow::movable(void)
   {
   return false;
   }
   
   void winWindow::move(const uint32v2 position)
   {
   }
   
   void winWindow::resize(const uint32v2 size)
   {
   }
   
   void winWindow::active(void)
   {
   }


   // Create Window
   //---------------
   

//   HDC       hDC;         // Application Device Context
//
//   // Acquiring Device Context
//   hDC = GetDC(hWnd);
//   if (hDC == nullptr)
//      {
//      Log << "Error! Cannot create device context.\n";
//      return false;
//      }






   WindowVK::WindowVK(const VulkanDevice* gpu,
                      const Ptr<CommonDisplay> selectedDisplay,
                      const uint32v2 selectedResolution,
                      const WindowSettings& settings,
                      const string title) :
#if defined(EN_PLATFORM_WINDOWS)
      winWindow(ptr_dynamic_cast<winDisplay, CommonDisplay>(selectedDisplay), selectedResolution, settings, title)
#endif
   {
   // Window > Surface > Swap-Chain > Device - connection

   // Be sure device is idle before creating Swap-Chain
   vkDeviceWaitIdle(gpu->device);

   // Create Swap-Chain surface attached to Window
   //----------------------------------------------

   VkSurfaceKHR             swapChainSurface;
   VkSurfaceCapabilitiesKHR swapChainCapabilities;
      
   VkWin32SurfaceCreateInfoKHR createInfo;
   createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
   createInfo.pNext     = nullptr;
   createInfo.flags     = 0;           // VkWin32SurfaceCreateFlagsKHR - Reserved.
   createInfo.hinstance = AppInstance; // HINSTANCE
   createInfo.hwnd      = hWnd;        // HWND

   Profile( vkCreateWin32SurfaceKHR(ptr_dynamic_cast<VulkanAPI, GraphicsAPI>(en::Graphics)->instance,
                                    &createInfo, nullptr, &swapChainSurface) )

   // Query capabilities of Swap-Chain surface for this device
   Profile( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, swapChainSurface, &swapChainCapabilities) )
   if (lastResult != VK_SUCCESS)
      {
      string info;
      info += "ERROR: Vulkan error:\n";
      info += "       Cannot query Swap-Chain surface capabilities.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Calculate amount of backing images in Swap-Chain
   //--------------------------------------------------
   
   // Typical Swap-Chain consist of 3 surfaces, one is presented on display, next is
   // queued to be presented, and the third one is used by application for rendering.
   uint32 swapChainImages = max(3, swapChainCapabilities.minImageCount);
   if (swapChainCapabilities.maxImageCount)
      swapChainImages = min(swapChainImages, swapChainCapabilities.maxImageCount);
   
   // TODO: In future check what's the most optimal count and what should be used for VR.

   // Determine PixelFormat and Color Space
   //---------------------------------------
   
   VkFormat        swapChainFormat     = TranslateTextureFormat[underlyingType(settings.format)];
   VkColorSpaceKHR swapChainColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      
   // Query count of available Pixel Formats supported by this device, and matching destination Window
   uint32 formats = 0;
   Profile( vkGetPhysicalDeviceSurfaceFormatsKHR(handle, swapChainSurface, &formats, nullptr) )
   if ( (lastResult != VK_SUCCESS) ||
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
   Profile( vkGetPhysicalDeviceSurfaceFormatsKHR(handle, swapChainSurface, &formats, &deviceFormats[0]) )
   if (lastResult != VK_SUCCESS)
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
   if (!bitsCheck(swapChainCapabilities.supportedUsageFlags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))
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
   if (!bitsCheck(swapChainCapabilities.supportedTransforms, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
      {
      // If we need to apply a transform, use current one in the system
      swapChainTransform = swapChainCapabilities.currentTransform;
      }

   // Determine Presenting Mode
   //---------------------------

   uint32 modes = 0; // Presentation Modes
   VkPresentModeKHR* presentationMode = nullptr;
   
   // Query device Presentation Modes count
   Profile( vkGetPhysicalDeviceSurfacePresentModesKHR(handle, swapChainSurface, &modes, nullptr) )
   if ( (lastResult != VK_SUCCESS) ||
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
   Profile( vkGetPhysicalDeviceSurfacePresentModesKHR(handle, swapChainSurface, &modes, &presentationMode[0]) )
   if (lastResult != VK_SUCCESS)
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







   // Screen
   // - resolution
   // - color space
   
   // Window
   // - resolution
   
   // GraphicsDevice
   // - Color Space
   // - Render Target resolution (so it can be smaller than Window, or Fullscreen Window)




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
   createInfo.imageExtent           = swapChainResolution;
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
 
   Profile( vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) )

   _resolution = swapChainResolution;
   }
   
   
   
   Ptr<Window> VulkanDevice::create(const WindowSettings& settings, const string title)
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
      selectedResolution = display->resolution;

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
   
   



   LayerDescriptor::LayerDescriptor() :
      extension(nullptr),
      extensionsCount(0)
   {
   }

   // CPU memory allocation for given GPU device control
   void* VKAPI_PTR defaultAlloc(
       void*                                       pUserData,
       size_t                                      size,
       size_t                                      alignment,
       VkSystemAllocationScope                     allocationScope)
   {
   void* ptr = reinterpret_cast<void*>(allocate<uint8>(alignment, size));
   if (ptr)
      {
      VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
      deviceVK->memoryRAM += size;
      }

   return ptr;
   }
   
   void* VKAPI_PTR defaultRealloc(
       void*                                       pUserData,
       void*                                       pOriginal,
       size_t                                      size,
       size_t                                      alignment,
       VkSystemAllocationScope                     allocationScope)
   {
   #ifdef EN_PLATFORM_WINDOWS
   return reinterpret_cast<void*>(reallocate<uint8>(reinterpret_cast<uint8*>(pOriginal), alignment, size));
   #else
   // THERE IS NO MEM ALIGNED REALLOC ON UNIX SYSTEMS !
   void* ptr = reinterpret_cast<void*>(allocate<uint8>(alignment, size));
   if (ptr)
      {
      // TODO: Needs to know size of original allocation !
      //if (pOriginal)
      //   memcpy(ptr, pOriginal, size);
      deallocate<uint8>(reinterpret_cast<uint8*>(pOriginal));

      VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);

      // TODO: Needs to know size of original allocation !
      //deviceVK->memoryRAM -= oldSize;
      //deviceVK->memoryRAM += size;
      }

   return ptr;
   #endif
   }
   
   void VKAPI_PTR defaultFree(
       void*                                       pUserData,
       void*                                       pMemory)
   {
   deallocate<uint8>(reinterpret_cast<uint8*>(pMemory));

   VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
   // TODO: Needs to know size of original allocation !
   //deviceVK->memoryRAM -= oldSize;
   }
   
   void VKAPI_PTR defaultIntAlloc(
       void*                                       pUserData,
       size_t                                      size,
       VkInternalAllocationType                    allocationType,
       VkSystemAllocationScope                     allocationScope)
   {
   VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
   deviceVK->memoryDriver += size;
   }
   
   void VKAPI_PTR defaultIntFree(
       void*                                       pUserData,
       size_t                                      size,
       VkInternalAllocationType                    allocationType,
       VkSystemAllocationScope                     allocationScope)
   {
   VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
   deviceVK->memoryDriver -= size;
   }



   VulkanDevice::VulkanDevice(const VkPhysicalDevice _handle) :
      lastResult(VK_SUCCESS),
      handle(_handle),
      queueFamily(nullptr),
      queueFamiliesCount(0),
      queueFamilyIndices(nullptr),
      layer(nullptr),
      layersCount(0),
      globalExtension(nullptr),
      globalExtensionsCount(0),
      memoryRAM(0),
      memoryDriver(0)
   {
   // Unbind all function pointers first
   unbindDeviceFunctionPointers();

   // Init default memory allocation callbacks
   defaultAllocCallbacks.pUserData             = this;
   defaultAllocCallbacks.pfnAllocation         = defaultAlloc;
   defaultAllocCallbacks.pfnReallocation       = defaultRealloc;
   defaultAllocCallbacks.pfnFree               = defaultFree;
   defaultAllocCallbacks.pfnInternalAllocation = defaultIntAlloc;
   defaultAllocCallbacks.pfnInternalFree       = defaultIntFree;

   // Gather Device Capabilities
   ProfileNoRet( vkGetPhysicalDeviceFeatures(handle, &features) )
   ProfileNoRet( vkGetPhysicalDeviceProperties(handle, &properties) )
   ProfileNoRet( vkGetPhysicalDeviceMemoryProperties(handle, &memory) )

 // TODO: Gather even more information
 //Profile( vkGetPhysicalDeviceFormatProperties(handle, VkFormat format, VkFormatProperties* pFormatProperties) )
 //Profile( vkGetPhysicalDeviceImageFormatProperties(handle, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) )

   // TODO: Populate "Support" section of CommonDevice
   // Texture
 //support.maxTextureSize1D     = properties.limits.maxImageDimension1D;
   support.maxTextureSize       = properties.limits.maxImageDimension2D;
   support.maxTextureRectSize   = properties.limits.maxImageDimension2D;     // TODO: There is no such thing in Vulkan
   support.maxTextureCubeSize   = properties.limits.maxImageDimensionCube; 
   support.maxTexture3DSize     = properties.limits.maxImageDimension3D;
   support.maxTextureLayers     = properties.limits.maxImageArrayLayers;          
   support.maxTextureBufferSize = properties.limits.maxImageDimension1D;     // TODO: Looks like there is no such thing in Vulkan
   support.maxTextureLodBias    = properties.limits.maxSamplerLodBias;            
   // Sampler
   support.maxAnisotropy        = properties.limits.maxSamplerAnisotropy;


   // Queue Families, Queues
   //------------------------

   // Gather information about Queue Families supported by this device
   ProfileNoRet( vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr) )
   queueFamily = new VkQueueFamilyProperties[queueFamiliesCount];
   ProfileNoRet( vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamily) )

   // Generate list of all Queue Family indices.
   // This list will be passed during resource creation time for resources that need to be populated first.
   // This way we can use separate Transfer Queues for faster data transfer, and driver knows to use
   // proper synchronization mechanisms to prevent hazards and race conditions.
   queueFamilyIndices = new uint32[queueFamiliesCount];
   for (uint32 ii=0; i<queueFamiliesCount; ++i)
       queueFamilyIndices[i] = i;

   // Map Queue Families and their Queue Count to QueueType and Index.
   memset(&queuesCount[0], 0, sizeof(queuesCount));
   memset(&queueTypeToFamily[0], 0, sizeof(queueTypeToFamily));
   for(uint32_t family=0; family<queueCount; ++family)
      {
      uint32 flags = queueFamily[family].queueFlags;
      
      // For now, we assume that there will be always only one Queue Family matching one Queue Type.
      // If that's not true in the future, then below, each range of Queues in given Family, will need
      // to be mapped to range of Queues in given Type. If such situation will occur, assertions below
      // will fire at runtime.
      if (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Universal)] == 0 );
         queuesCount[underlyingType(QueueType::Universal)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Universal)] = family;
         }
      else
      if (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Universal)] == 0 );
         queuesCount[underlyingType(QueueType::Universal)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Universal)] = family;
         }
      else
      if (flags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Compute)] == 0 );
         queuesCount[underlyingType(QueueType::Compute)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Compute)] = family;
         }
      else
      if (flags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Compute)] == 0 );
         queuesCount[underlyingType(QueueType::Compute)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Compute)] = family;
         }
      else
      if (flags & (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::SparseTransfer)] == 0 );
         queuesCount[underlyingType(QueueType::SparseTransfer)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::SparseTransfer)] = family;
         }
      else
      if (flags & (VK_QUEUE_TRANSFER_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Transfer)] == 0 );
         queuesCount[underlyingType(QueueType::Transfer)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Transfer)] = family;
         }
      else
         {
         // It's another combination of Queue Family flags that we don't support.
         // This shouldn't happen but if it will we will report it without asserting.
         
         Log << "Unsupported type of Queue Family" << endl;
         Log << "    Queues in Family: %i" << queueFamily[family].queueCount << endl;
         Log << "    Queue Min Transfer Width : %i" << queueFamily[family].minImageTransferGranularity.width << endl;
         Log << "    Queue Min Transfer Height: %i" << queueFamily[family].minImageTransferGranularity.height << endl;
         Log << "    Queue Min Transfer Depth : %i" << queueFamily[family].minImageTransferGranularity.depth << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            Log << "    Queue supports: GRAPHICS" << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
            Log << "    Queue supports: COMPUTE" << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
            Log << "    Queue supports: TRANSFER" << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            Log << "    Queue supports: SPARSE_BINDING" << endl;
         Log << "    Queues Time Stamp: %i" << queueFamily[family].timestampValidBits << endl;
         }
      }
   
   // Debug Print of Queue Families
   
    //for(uint32_t family=0; family<queueCount; ++family)
    //   {
    //   printf("Queue Family %i:\n\n", family);

    //   printf("    Queues in Family: %i\n", queueFamily[family].queueCount);
    //   printf("    Queue Min Transfer W: %i\n", queueFamily[family].minImageTransferGranularity.width);
    //   printf("    Queue Min Transfer H: %i\n", queueFamily[family].minImageTransferGranularity.height);
    //   printf("    Queue Min Transfer D: %i\n", queueFamily[family].minImageTransferGranularity.depth);
    //   if (queueFamily[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    //      printf("    Queue supports: GRAPHICS\n");
    //   if (queueFamily[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
    //      printf("    Queue supports: COMPUTE\n");
    //   if (queueFamily[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
    //      printf("    Queue supports: TRANSFER\n");
    //   if (queueFamily[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
    //      printf("    Queue supports: SPARSE_BINDING\n\n\n"); 
    //   printf("    Queues Time Stamp: %i\n", queueFamily[family].timestampValidBits);
    //   }


   // Layouts
   //---------

   VkLayerProperties* layerProperties = nullptr;

   // Acquire list of Vulkan Layers supported by this Device 
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateDeviceLayerProperties(handle, &layersCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);

      if (layersCount == 0)
         break;

      // Allocate array of Layer Properties descriptors
      if (layerProperties)
         delete [] layerProperties;
      layerProperties = new VkLayerProperties[layersCount];
      Profile( vkEnumerateDeviceLayerProperties(handle, &layersCount, layerProperties) )
      }

   // Acquire information about each Layer and supported extensions
   layer = new LayerDescriptor[layersCount];
   for(uint32 i=0; i<layersCount; ++i)
      {
      layer[i].properties = layerProperties[i];

      // Acquire list of all extensions supported by this layer
      lastResult = VK_INCOMPLETE;
      while(lastResult == VK_INCOMPLETE)
         {
         Profile( vkEnumerateDeviceExtensionProperties(handle, layer[i].properties.layerName, &layer[i].extensionsCount, nullptr) )
         if (IsWarning(lastResult))
            assert(0);
      
         if (layer[i].extensionsCount == 0)
            break;
      
         // Allocate array of Layer Extension descriptors
         if (layer[i].extension)
            delete [] layer[i].extension;
         layer[i].extension = new VkExtensionProperties[layer[i].extensionsCount];
         Profile( vkEnumerateDeviceExtensionProperties(handle, layer[i].properties.layerName, &layer[i].extensionsCount, layer[i].extension) )
         }
      }

   delete [] layerProperties;

   // Acquire list of all global extensions not being part of any layer
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);
   
      if (globalExtensionsCount == 0)
         break;

      if (globalExtension)
         delete [] globalExtension;
      globalExtension = new VkExtensionProperties[globalExtensionsCount];
      Profile( vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, globalExtension) )
      }

   // While creating device, we can choose to init as many Queue Families as we want (but each only once).
   // In each Queue Family we can specify how many Queues we want to create and init.
   // For now lets just init everything.
   VkDeviceQueueCreateInfo* queueFamilyInfo = new VkDeviceQueueCreateInfo[queueFamiliesCount];
   for(uint32 i=0; i<queueFamiliesCount; ++i)
      {
      uint32 queues = queueFamily[i].queueCount;

      queueFamilyInfo[i].sType              = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueFamilyInfo[i].pNext              = nullptr; 
      queueFamilyInfo[i].flags              = 0; // Reserved
      queueFamilyInfo[i].queueFamilyIndex   = i;
      queueFamilyInfo[i].queueCount         = queues;

      // Mark all queues from the same family to have the same priority during workload distribution
      float* priorities = new float[queues];
      for(uint32 priority=0; priority<queues; ++priority)
         priorities[priority] = 1.0f;

      queueFamilyInfo[i].pQueuePriorities   = priorities;
      }

   // Similarly to Queue Families, for now on, lets init all Layers and all Extensions available

   // Create array of pointers to all layer names
   char** layersPtrs = new char*[layersCount];
   uint32 index = 0;
   for(uint32 i=0; i<layersCount; ++i)  
      layersPtrs[index] = &layer[i].properties.layerName[0];

   // Calculate total amount of extensions supported by all Layers
   uint32 totalExtensionsCount = globalExtensionsCount;
   for(uint32 i=0; i<layersCount; ++i)
      totalExtensionsCount += layer[i].extensionsCount;  

   // Create array of pointers to this extension names
   char** extensionPtrs = new char*[totalExtensionsCount];
   index = 0;
   for(uint32 i=0; i<globalExtensionsCount; ++i, ++index)
      extensionPtrs[index] = &globalExtension[i].extensionName[0];
   for(uint32 i=0; i<layersCount; ++i)
      for(uint32 j=0; j<layer[i].extensionsCount; ++j, ++index)   
         extensionPtrs[index] = &layer[i].extension[j].extensionName[0];

   // Create Device Interface
   VkDeviceCreateInfo deviceInfo;
   deviceInfo.sType                     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   deviceInfo.pNext                     = nullptr;
   deviceInfo.flags                     = 0;                     // Reserved
   deviceInfo.queueCreateInfoCount      = queueFamiliesCount;
   deviceInfo.pQueueCreateInfos         = queueInfo;
   deviceInfo.enabledLayerCount         = layersCount;
   deviceInfo.ppEnabledLayerNames       = layersCount          ? reinterpret_cast<const char*const*>(layersPtrs) : nullptr;
   deviceInfo.enabledExtensionCount     = totalExtensionsCount;
   deviceInfo.ppEnabledExtensionNames   = totalExtensionsCount ? reinterpret_cast<const char*const*>(extensionPtrs) : nullptr;
   deviceInfo.pEnabledFeatures          = nullptr;

   Profile( vkCreateDevice(handle, &deviceInfo, &defaultAllocCallbacks, &device) )

   // Bind Device Functions
   bindDeviceFunctionPointers();


   // Command Pools
   //---------------
  
   // <<<< Per Thread Section (TODO: Execute on each Worker Thread)
   uint32 thread = Scheduler.core();
   
   // Memory pool used to allocate CommandBuffers.
   // Once device is created, on each Worker Thread, we create Command Pool for each Queue Family
   // that was associated to each of our Queue Types. Then each time Command Buffer creation will
   // be requested for Queue from given Queue Type, apropriate Command Pool for matching Queue
   // Family will be used (taking into notice parent thread).
   for(uint32 i=0; i<underlyingType(QueueType::Count); ++i)
      if (queuesCount[i] > 0)
         {
         uint32 family = queueTypeToFamily[i];

         // It's abstract object as it has no size.
         // It's not tied to Queueu, but to Queue Family, so it can be shared by all Queues in this family.
         // It's single threaded, and so should be Command Buffers allocated from it
         // (so each Thread gets it's own Pool per each Family, and have it's own Command Buffers).
         VkCommandPoolCreateInfo poolInfo;
         poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
         poolInfo.pNext            = nullptr;
         poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // To reuse CB's use VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
         poolInfo.queueFamilyIndex = queueFamilyId;
         
         Profile( vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool[thread][i]) )
         }

   // <<<< Per Thread Section


   // TODO: Do we want to support reset of Command Pools? Should it go to API?
   //
   // // Resets Command Pool, frees all resources encoded on all CB's created from this pool. CB's are rest to initial state.
   // uint32 thread = 0;
   // uint32 type = 0;
   // Profile( vkResetCommandPool(VkDevice device, commandPool[thread][type], VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) )






   // Create Pipeline Cache

   // TODO: VkPipelineCache  pipelineCache;


   // Free temporary resources
   delete [] extensionPtrs;
   delete [] layersPtrs;
   }

   VulkanDevice::~VulkanDevice()
   {
   // <<<< Per Thread Section (TODO: Execute on each Worker Thread)
   uint32 thread = Scheduler.core();

   // Release all Command Pools used by this thread for Commands submission
   for(uint32 i=0; i<underlyingType(QueueType::Count); ++i)
      if (queuesCount[i] > 0)
         Profile( vkDestroyCommandPool(device, commandPool[thread][i], nullptr) )

   // <<<< Per Thread Section

   delete [] queueFamily;
   delete [] queueFamilyIndices;
   for(uint32 i=0; i<layersCount; ++i)
      delete [] layer[i].extension;
   delete [] layer;
   delete [] globalExtension;
   }
   
   uint32 VulkanDevice::queues(const QueueType type) const
   {
   return queuesCount[underlyingType(type)];  //Need translation table from (Type, N) -> (Family, Index)
   }

   void VulkanDevice::bindDeviceFunctionPointers(void)
   {
   // TODO: Order it by extensions etc.

   // Windows WA - Undefine Windows API defines
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif
#ifdef CreateEvent
#undef CreateEvent
#endif

   // Vulkan 1.0
   //
   bindDeviceFunction( DestroyInstance );
   bindDeviceFunction( GetPhysicalDeviceFeatures );
   bindDeviceFunction( GetPhysicalDeviceFormatProperties );
   bindDeviceFunction( GetPhysicalDeviceImageFormatProperties );
   bindDeviceFunction( GetPhysicalDeviceProperties );
   bindDeviceFunction( GetPhysicalDeviceQueueFamilyProperties );
   bindDeviceFunction( GetPhysicalDeviceMemoryProperties );
   bindDeviceFunction( GetDeviceProcAddr );
   bindDeviceFunction( CreateDevice );
   bindDeviceFunction( DestroyDevice );
   bindDeviceFunction( EnumerateDeviceExtensionProperties );
   bindDeviceFunction( EnumerateDeviceLayerProperties );
   bindDeviceFunction( GetDeviceQueue );
   bindDeviceFunction( QueueSubmit );
   bindDeviceFunction( QueueWaitIdle );
   bindDeviceFunction( DeviceWaitIdle );
   bindDeviceFunction( AllocateMemory );
   bindDeviceFunction( FreeMemory );
   bindDeviceFunction( MapMemory );
   bindDeviceFunction( UnmapMemory );
   bindDeviceFunction( FlushMappedMemoryRanges );
   bindDeviceFunction( InvalidateMappedMemoryRanges );
   bindDeviceFunction( GetDeviceMemoryCommitment );
   bindDeviceFunction( BindBufferMemory );
   bindDeviceFunction( BindImageMemory );
   bindDeviceFunction( GetBufferMemoryRequirements );
   bindDeviceFunction( GetImageMemoryRequirements );
   bindDeviceFunction( GetImageSparseMemoryRequirements );
   bindDeviceFunction( GetPhysicalDeviceSparseImageFormatProperties );
   bindDeviceFunction( QueueBindSparse );
   bindDeviceFunction( CreateFence );
   bindDeviceFunction( DestroyFence );
   bindDeviceFunction( ResetFences );
   bindDeviceFunction( GetFenceStatus );
   bindDeviceFunction( WaitForFences );
   bindDeviceFunction( CreateSemaphore );
   bindDeviceFunction( DestroySemaphore );
   bindDeviceFunction( CreateEvent );
   bindDeviceFunction( DestroyEvent );
   bindDeviceFunction( GetEventStatus );
   bindDeviceFunction( SetEvent );
   bindDeviceFunction( ResetEvent );
   bindDeviceFunction( CreateQueryPool );
   bindDeviceFunction( DestroyQueryPool );
   bindDeviceFunction( GetQueryPoolResults );
   bindDeviceFunction( CreateBuffer );
   bindDeviceFunction( DestroyBuffer );
   bindDeviceFunction( CreateBufferView );
   bindDeviceFunction( DestroyBufferView );
   bindDeviceFunction( CreateImage );
   bindDeviceFunction( DestroyImage );
   bindDeviceFunction( GetImageSubresourceLayout );
   bindDeviceFunction( CreateImageView );
   bindDeviceFunction( DestroyImageView );
   bindDeviceFunction( CreateShaderModule );
   bindDeviceFunction( DestroyShaderModule );

   bindDeviceFunction( CreatePipelineCache );
   bindDeviceFunction( DestroyPipelineCache );
   bindDeviceFunction( GetPipelineCacheData );
   bindDeviceFunction( MergePipelineCaches );
   bindDeviceFunction( CreateGraphicsPipelines );
   bindDeviceFunction( CreateComputePipelines );
   bindDeviceFunction( DestroyPipeline );
   bindDeviceFunction( CreatePipelineLayout );
   bindDeviceFunction( DestroyPipelineLayout );
   bindDeviceFunction( CreateSampler );
   bindDeviceFunction( DestroySampler );
   bindDeviceFunction( CreateDescriptorSetLayout );
   bindDeviceFunction( DestroyDescriptorSetLayout );
   bindDeviceFunction( CreateDescriptorPool );
   bindDeviceFunction( DestroyDescriptorPool );
   bindDeviceFunction( ResetDescriptorPool );
   bindDeviceFunction( AllocateDescriptorSets );
   bindDeviceFunction( FreeDescriptorSets );
   bindDeviceFunction( UpdateDescriptorSets );
   bindDeviceFunction( CreateFramebuffer );
   bindDeviceFunction( DestroyFramebuffer );
   bindDeviceFunction( CreateRenderPass );
   bindDeviceFunction( DestroyRenderPass );
   bindDeviceFunction( GetRenderAreaGranularity );
   bindDeviceFunction( CreateCommandPool );
   bindDeviceFunction( DestroyCommandPool );
   bindDeviceFunction( ResetCommandPool );
   bindDeviceFunction( AllocateCommandBuffers );
   bindDeviceFunction( FreeCommandBuffers );
   bindDeviceFunction( BeginCommandBuffer );
   bindDeviceFunction( EndCommandBuffer );
   bindDeviceFunction( ResetCommandBuffer );
   bindDeviceFunction( CmdBindPipeline );
   bindDeviceFunction( CmdSetViewport );
   bindDeviceFunction( CmdSetScissor );
   bindDeviceFunction( CmdSetLineWidth );
   bindDeviceFunction( CmdSetDepthBias );
   bindDeviceFunction( CmdSetBlendConstants );
   bindDeviceFunction( CmdSetDepthBounds );
   bindDeviceFunction( CmdSetStencilCompareMask );
   bindDeviceFunction( CmdSetStencilWriteMask );
   bindDeviceFunction( CmdSetStencilReference );
   bindDeviceFunction( CmdBindDescriptorSets );
   bindDeviceFunction( CmdBindIndexBuffer );
   bindDeviceFunction( CmdBindVertexBuffers );
   bindDeviceFunction( CmdDraw );
   bindDeviceFunction( CmdDrawIndexed );
   bindDeviceFunction( CmdDrawIndirect );
   bindDeviceFunction( CmdDrawIndexedIndirect );
   bindDeviceFunction( CmdDispatch );
   bindDeviceFunction( CmdDispatchIndirect );
   bindDeviceFunction( CmdCopyBuffer );
   bindDeviceFunction( CmdCopyImage );
   bindDeviceFunction( CmdBlitImage );
   bindDeviceFunction( CmdCopyBufferToImage );
   bindDeviceFunction( CmdCopyImageToBuffer );
   bindDeviceFunction( CmdUpdateBuffer );
   bindDeviceFunction( CmdFillBuffer );
   bindDeviceFunction( CmdClearColorImage );
   bindDeviceFunction( CmdClearDepthStencilImage );
   bindDeviceFunction( CmdClearAttachments );
   bindDeviceFunction( CmdResolveImage );
   bindDeviceFunction( CmdSetEvent );
   bindDeviceFunction( CmdResetEvent );
   bindDeviceFunction( CmdWaitEvents );
   bindDeviceFunction( CmdPipelineBarrier );
   bindDeviceFunction( CmdBeginQuery );
   bindDeviceFunction( CmdEndQuery );
   bindDeviceFunction( CmdResetQueryPool );
   bindDeviceFunction( CmdWriteTimestamp );
   bindDeviceFunction( CmdCopyQueryPoolResults );
   bindDeviceFunction( CmdPushConstants );
   bindDeviceFunction( CmdBeginRenderPass );
   bindDeviceFunction( CmdNextSubpass );
   bindDeviceFunction( CmdEndRenderPass );
   bindDeviceFunction( CmdExecuteCommands );

   bindDeviceFunction( CreateSwapchainKHR );
   bindDeviceFunction( DestroySwapchainKHR );
   bindDeviceFunction( GetSwapchainImagesKHR );
   bindDeviceFunction( AcquireNextImageKHR );
   bindDeviceFunction( QueuePresentKHR );
   }

   void VulkanDevice::unbindDeviceFunctionPointers(void)
   {
   // Vulkan 1.0
   //
   vkCreateInstance                               = nullptr;
   vkDestroyInstance                              = nullptr;
   vkEnumeratePhysicalDevices                     = nullptr;
   vkGetPhysicalDeviceFeatures                    = nullptr;
   vkGetPhysicalDeviceFormatProperties            = nullptr;
   vkGetPhysicalDeviceImageFormatProperties       = nullptr;
   vkGetPhysicalDeviceProperties                  = nullptr;
   vkGetPhysicalDeviceQueueFamilyProperties       = nullptr;
   vkGetPhysicalDeviceMemoryProperties            = nullptr;
   vkGetDeviceProcAddr                            = nullptr;
   vkCreateDevice                                 = nullptr;
   vkDestroyDevice                                = nullptr;
   vkEnumerateInstanceExtensionProperties         = nullptr;
   vkEnumerateDeviceExtensionProperties           = nullptr;
   vkEnumerateInstanceLayerProperties             = nullptr;
   vkEnumerateDeviceLayerProperties               = nullptr;
   vkGetDeviceQueue                               = nullptr;
   vkQueueSubmit                                  = nullptr;
   vkQueueWaitIdle                                = nullptr;
   vkDeviceWaitIdle                               = nullptr;
   vkAllocateMemory                               = nullptr;
   vkFreeMemory                                   = nullptr;
   vkMapMemory                                    = nullptr;
   vkUnmapMemory                                  = nullptr;
   vkFlushMappedMemoryRanges                      = nullptr;
   vkInvalidateMappedMemoryRanges                 = nullptr;
   vkGetDeviceMemoryCommitment                    = nullptr;
   vkBindBufferMemory                             = nullptr;
   vkBindImageMemory                              = nullptr;
   vkGetBufferMemoryRequirements                  = nullptr;
   vkGetImageMemoryRequirements                   = nullptr;
   vkGetImageSparseMemoryRequirements             = nullptr;
   vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
   vkQueueBindSparse                              = nullptr;
   vkCreateFence                                  = nullptr;
   vkDestroyFence                                 = nullptr;
   vkResetFences                                  = nullptr;
   vkGetFenceStatus                               = nullptr;
   vkWaitForFences                                = nullptr;
   vkCreateSemaphore                              = nullptr;
   vkDestroySemaphore                             = nullptr;
   vkCreateEvent                                  = nullptr;
   vkDestroyEvent                                 = nullptr;
   vkGetEventStatus                               = nullptr;
   vkSetEvent                                     = nullptr;
   vkResetEvent                                   = nullptr;
   vkCreateQueryPool                              = nullptr;
   vkDestroyQueryPool                             = nullptr;
   vkGetQueryPoolResults                          = nullptr;
   vkCreateBuffer                                 = nullptr;
   vkDestroyBuffer                                = nullptr;
   vkCreateBufferView                             = nullptr;
   vkDestroyBufferView                            = nullptr;
   vkCreateImage                                  = nullptr;
   vkDestroyImage                                 = nullptr;
   vkGetImageSubresourceLayout                    = nullptr;
   vkCreateImageView                              = nullptr;
   vkDestroyImageView                             = nullptr;
   vkCreateShaderModule                           = nullptr;
   vkDestroyShaderModule                          = nullptr;
   vkCreatePipelineCache                          = nullptr;
   vkDestroyPipelineCache                         = nullptr;
   vkGetPipelineCacheData                         = nullptr;
   vkMergePipelineCaches                          = nullptr;
   vkCreateGraphicsPipelines                      = nullptr;
   vkCreateComputePipelines                       = nullptr;
   vkDestroyPipeline                              = nullptr;
   vkCreatePipelineLayout                         = nullptr;
   vkDestroyPipelineLayout                        = nullptr;
   vkCreateSampler                                = nullptr;
   vkDestroySampler                               = nullptr;
   vkCreateDescriptorSetLayout                    = nullptr;
   vkDestroyDescriptorSetLayout                   = nullptr;
   vkCreateDescriptorPool                         = nullptr;
   vkDestroyDescriptorPool                        = nullptr;
   vkResetDescriptorPool                          = nullptr;
   vkAllocateDescriptorSets                       = nullptr;
   vkFreeDescriptorSets                           = nullptr;
   vkUpdateDescriptorSets                         = nullptr;
   vkCreateFramebuffer                            = nullptr;
   vkDestroyFramebuffer                           = nullptr;
   vkCreateRenderPass                             = nullptr;
   vkDestroyRenderPass                            = nullptr;
   vkGetRenderAreaGranularity                     = nullptr;
   vkCreateCommandPool                            = nullptr;
   vkDestroyCommandPool                           = nullptr;
   vkResetCommandPool                             = nullptr;
   vkAllocateCommandBuffers                       = nullptr;
   vkFreeCommandBuffers                           = nullptr;
   vkBeginCommandBuffer                           = nullptr;
   vkEndCommandBuffer                             = nullptr;
   vkResetCommandBuffer                           = nullptr;
   vkCmdBindPipeline                              = nullptr;
   vkCmdSetViewport                               = nullptr;
   vkCmdSetScissor                                = nullptr;
   vkCmdSetLineWidth                              = nullptr;
   vkCmdSetDepthBias                              = nullptr;
   vkCmdSetBlendConstants                         = nullptr;
   vkCmdSetDepthBounds                            = nullptr;
   vkCmdSetStencilCompareMask                     = nullptr;
   vkCmdSetStencilWriteMask                       = nullptr;
   vkCmdSetStencilReference                       = nullptr;
   vkCmdBindDescriptorSets                        = nullptr;
   vkCmdBindIndexBuffer                           = nullptr;
   vkCmdBindVertexBuffers                         = nullptr;
   vkCmdDraw                                      = nullptr;
   vkCmdDrawIndexed                               = nullptr;
   vkCmdDrawIndirect                              = nullptr;
   vkCmdDrawIndexedIndirect                       = nullptr;
   vkCmdDispatch                                  = nullptr;
   vkCmdDispatchIndirect                          = nullptr;
   vkCmdCopyBuffer                                = nullptr;
   vkCmdCopyImage                                 = nullptr;
   vkCmdBlitImage                                 = nullptr;
   vkCmdCopyBufferToImage                         = nullptr;
   vkCmdCopyImageToBuffer                         = nullptr;
   vkCmdUpdateBuffer                              = nullptr;
   vkCmdFillBuffer                                = nullptr;
   vkCmdClearColorImage                           = nullptr;
   vkCmdClearDepthStencilImage                    = nullptr;
   vkCmdClearAttachments                          = nullptr;
   vkCmdResolveImage                              = nullptr;
   vkCmdSetEvent                                  = nullptr;
   vkCmdResetEvent                                = nullptr;
   vkCmdWaitEvents                                = nullptr;
   vkCmdPipelineBarrier                           = nullptr;
   vkCmdBeginQuery                                = nullptr;
   vkCmdEndQuery                                  = nullptr;
   vkCmdResetQueryPool                            = nullptr;
   vkCmdWriteTimestamp                            = nullptr;
   vkCmdCopyQueryPoolResults                      = nullptr;
   vkCmdPushConstants                             = nullptr;
   vkCmdBeginRenderPass                           = nullptr;
   vkCmdNextSubpass                               = nullptr;
   vkCmdEndRenderPass                             = nullptr;
   vkCmdExecuteCommands                           = nullptr;
   }





   VkDeviceMemory VulkanDevice::allocMemory(VkMemoryRequirements requirements, VkFlags properties)
   {
   VkDeviceMemory handle;

   // Find Memory Type that best suits required allocation
   uint32 index = VK_MAX_MEMORY_TYPES;
   for(uint32 i=0; i<memory.memoryTypeCount; ++i)
      {
      // Memory Type needs to be able to support requested allocation
      if (checkBit(requirements.memoryTypeBits, (i+1)))
         // Memory Type needs to support at least requested sub-set of memory properties
         if ((memory.memoryTypes[i].propertyFlags & properties) == properties)
            index = i;

      // If this memory type cannot be used to allocate requested resource, continue search
      if (index == VK_MAX_MEMORY_TYPES)
         continue;

      // Try to allocate memory on Heap supporting this memory type
      VkMemoryAllocateInfo allocInfo;
      allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.pNext           = nullptr;
      allocInfo.allocationSize  = requirements.size;
      allocInfo.memoryTypeIndex = index;
      
      Profile( vkAllocateMemory(device, &allocInfo, &defaultAllocCallbacks, &handle) )
	  if (lastResult == VK_SUCCESS)
         return handle;
      }

   // FAIL
   return handle;
   }


// typedef struct {
//     VkDeviceSize                                size;
//     VkDeviceSize                                alignment;
//     uint32_t                                    memoryTypeBits;
// } VkMemoryRequirements;
// 
// 
// - Heaps are fixed up front.
// - Their amount and types is vendor / gpu / driver dependent.
// - We alloc memory on given cheap by referring to Memory Type ID -> which points at given heap. (see below).
// 
// Acquire GPU heaps count an types available :
// 
// 	void vkGetPhysicalDeviceMemoryProperties(
// 	    VkPhysicalDevice physicalDevice, 
// 	    VkPhysicalDeviceMemoryProperties* pMemoryProperties);
// 
// 	#define VK_MAX_MEMORY_TYPES               32
// 	#define VK_MAX_MEMORY_HEAPS               16
// 
// 	typedef struct {
// 	    uint32_t                   memoryTypeCount;
// 	    VkMemoryType               memoryTypes[VK_MAX_MEMORY_TYPES];
// 	    uint32_t                   memoryHeapCount;
// 	    VkMemoryHeap               memoryHeaps[VK_MAX_MEMORY_HEAPS];
// 	} VkPhysicalDeviceMemoryProperties;
// 
// 
// 
// 
// 
// Memory Heap:
// 
// 	// size is uint64
// 	typedef struct {
// 	    VkDeviceSize               size;
// 	    VkMemoryHeapFlags          flags;
// 	} VkMemoryHeap;
// 
// 	typedef enum {
// 	    VK_MEMORY_HEAP_HOST_LOCAL_BIT = 0x00000001,
// 	} VkMemoryHeapFlagBits;
// 	typedef VkFlags VkMemoryHeapFlags;
// 
// 
// Memory Type:
// 
// 	// heapIndex - refers to memoryHeaps[] array above
// 	typedef struct {
// 	    VkMemoryPropertyFlags                       propertyFlags;
// 	    uint32_t                                    heapIndex;   
// 	} VkMemoryType;
// 
// 	typedef enum {
// 	    VK_MEMORY_PROPERTY_DEVICE_ONLY = 0,
// 	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000001,
// 	    VK_MEMORY_PROPERTY_HOST_NON_COHERENT_BIT = 0x00000002,
// 	    VK_MEMORY_PROPERTY_HOST_UNCACHED_BIT = 0x00000004,
// 	    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000008,
// 	} VkMemoryPropertyFlagBits;
// 	typedef VkFlags VkMemoryPropertyFlags;
// 
// When allocating resource, you pass MemoryType ID in the array, and through it, Driver picks Heap to use.
// 
// 
// Query memory layout for given resource:
// 
// 	void VKAPI vkGetBufferMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkBuffer                                    buffer,
// 	    VkMemoryRequirements*                       pMemoryRequirements);
// 	
// 	void VKAPI vkGetImageMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    VkMemoryRequirements*                       pMemoryRequirements);
// 	
// 	void VKAPI vkGetImageSparseMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    uint32_t*                                   pNumRequirements,
// 	    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements);
// 
// Returns simple size/offset + bits :
// 
// 	typedef struct {
// 	    VkDeviceSize                                size;
// 	    VkDeviceSize                                alignment;
// 	    uint32_t                                    memoryTypeBits;
// 	} VkMemoryRequirements;
// 	
// 	typedef struct {
// 	    VkSparseImageFormatProperties               formatProps;
// 	    uint32_t                                    imageMipTailStartLOD;
// 	    VkDeviceSize                                imageMipTailSize;
// 	    VkDeviceSize                                imageMipTailOffset;
// 	    VkDeviceSize                                imageMipTailStride;
// 	} VkSparseImageMemoryRequirements;
// 
// 
// So we alloc memory :
// 
// 	VkResult VKAPI vkAllocMemory(
// 	    VkDevice                                    device,
// 	    const VkMemoryAllocInfo*                    pAllocInfo,
// 	    VkDeviceMemory*                             pMem);
// 	
// 	// memoryTypeIndex <—— alloc by pointing on MEMORY TYPE ID 
// 	typedef struct {
// 	    VkStructureType                             sType;
// 	    const void*                                 pNext;
// 	    VkDeviceSize                                allocationSize;
// 	    uint32_t                                    memoryTypeIndex;
// 	} VkMemoryAllocInfo;
// 
// Then we can bind it to resource description/handle to given memory block range:
// 
// 	VkResult VKAPI vkBindBufferMemory(
// 	    VkDevice                                    device,
// 	    VkBuffer                                    buffer,
// 	    VkDeviceMemory                              mem,
// 	    VkDeviceSize                                memOffset);
// 	
// 	VkResult VKAPI vkBindImageMemory(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    VkDeviceMemory                              mem,
// 	    VkDeviceSize                                memOffset);
// 
// 
// So it looks like in Vulkan (VkDeviceMemory == Heap in D3D12/Metal) while Vulkan Heaps are even bigger concept.
// We do allocations on the Heap, but then we can freely bind Resource Handles to different Memory regions.
// There are also functions for mapping/unmapping/invalidating memory etc.




   VulkanAPI::VulkanAPI(string appName) :
#if defined(EN_PLATFORM_WINDOWS)
      library(LoadLibrary("vulkan-1.dll")),
#endif
#if defined(EN_PLATFORM_LINUX)
      library(dlopen("libvulkan.so", RTLD_NOW)),
#endif
      lastResult(VK_SUCCESS),
      layer(nullptr),
      layersCount(0),
      globalExtension(nullptr),
      globalExtensionsCount(0),
      devicesCount(0),
      display(nullptr),
      virtualDisplay(nullptr),
      displaysCount(0),
      GraphicAPI() // or SafeObject()
   {
   // API INDEPENDENT - WINDOWS DEPENDENT SECTION - BEGIN
   //-----------------------------------------------------

   // Display Device settings
   DISPLAY_DEVICE Device;
   memset(&Device, 0, sizeof(Device));
   Device.cb = sizeof(Device);

   // Calculate amount of available displays
   while(EnumDisplayDevices(nullptr, displaysCount, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
      if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
         displaysCount++;

   display = new Ptr<Screen>[displaysCount];
   virtualDisplay = new Screen();
  
   // Clear structure for next display (to ensure there is no old data)
   memset(&Device, 0, sizeof(Device));
   Device.cb = sizeof(Device);
      
   // Gather information about available displays
   uint32 displayId = 0;
   uint32 activeId = 0;
   while(EnumDisplayDevices(nullptr, displayId, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
      {
      // Only displays that are part of Virtual Desktop are used by engine
      // (so HMD displays won't be queried until they work in legacy mode as part of desktop)
      if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
         {
         uint32v2 desktopPosition;     // Display position on Virtual Desktop
         uint32v2 currentResolution;   // Displays current resolution
         uint32v2 nativeResolution;    // Displays native resolution (assumed largest possible)
         
         DEVMODE DispMode;
         memset(&DispMode, 0, sizeof(DispMode));
         DispMode.dmSize = sizeof(DispMode);

         // Query displays current resolution before game started
         assert( EnumDisplaySettingsEx(Device.DeviceName, ENUM_CURRENT_SETTINGS, &DispMode, 0u) )
 
         // Verify that proper values were returned by query
         assert( checkBits(DispMode.dmFields, DM_POSITION & DM_PELSWIDTH & DM_PELSHEIGHT) )

         desktopPosition.x   = DispMode.dmPosition.x;
         desktopPosition.y   = DispMode.dmPosition.y;
         currentResolution.x = DispMode.dmPelsWidth;
         currentResolution.y = DispMode.dmPelsHeight;
            
         // Calculate amount of available display modes (count only modes supported by display and no rotation ones)
         uint32 modesCount = 0;
         while(EnumDisplaySettingsEx(Device.DeviceName, ENUM_REGISTRY_SETTINGS, &DispMode, 0u))
            modesCount++;

         Ptr<winDisplay> currentDisplay = new winDisplay();
         
         currentDisplay->modeResolution = new uint32v2[modesCount];
     
         // Gather information about all supported display modes
         uint32 modeId = 0;
         while(EnumDisplaySettingsEx(Device.DeviceName, modeId, &DispMode, 0u))
            {
            // Verify that proper values were returned by query
            assert( checkBits(DispMode.dmFields, DM_PELSWIDTH & DM_PELSHEIGHT) );
            
            currentDisplay->modeResolution[modeId].x = DispMode.dmPelsWidth;
            currentDisplay->modeResolution[modeId].y = DispMode.dmPelsHeight;

            // Find largest supported resolution and assume it's display native
            if ( (DispMode.dmPelsWidth  > nativeResolution.x) ||
                 (DispMode.dmPelsHeight > nativeResolution.y) )
               {
               nativeResolution.x = DispMode.dmPelsWidth;
               nativeResolution.y = DispMode.dmPelsHeight;
               }
               
            modeId++;
            }
        
         currentDisplay->position           = desktopPosition;
         currentDisplay->resolution         = nativeResolution;
         currentDisplay->observedResolution = currentResolution;
         currentDisplay->modesCount         = modesCount;
         currentDisplay->index              = displayId;
       
         // Select Primary Display
         if (Device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
            {
            displayPrimary = activeId;
            }
       
         // Calculate upper-left corner position, and size of virtual display.
         // It's assumed that X axis increases right, and Y axis increases down.
         // Virtual Display is a bounding box for all available displays.
         if (activeId == 0)
            {
            virtualDisplay->position   = currentDisplay->position;
            virtualDisplay->resolution = currentDisplay->resolution;
            }
         else
            {
            if (currentDisplay->position.x < virtualDisplay->position.x)
               {
               virtualDisplay->resolution.width += (virtualDisplay->position.x - currentDisplay->position.x);
               virtualDisplay->position.x = currentDisplay->position.x;
               }
            if (currentDisplay->position.y < virtualDisplay->position.y)
               {
               virtualDisplay->resolution.height += (virtualDisplay->position.y - currentDisplay->position.y);
               virtualDisplay->position.y = currentDisplay->position.y;
               }
            uint32 virtualRightBorder = virtualDisplay->position.x + virtualDisplay->resolution.width;
            uint32 currentRightBorder = currentDisplay->position.x + currentDisplay->resolution.width;
            if (virtualRightBorder < currentRightBorder)
               virtualDisplay->resolution.width = currentRightBorder - virtualDisplay->position.x;
            uint32 virtualBottomBorder = virtualDisplay->position.y + virtualDisplay->resolution.height;
            uint32 currentBottomBorder = currentDisplay->position.y + currentDisplay->resolution.height;
            if (virtualBottomBorder < currentBottomBorder)
               virtualDisplay->resolution.height = currentBottomBorder - virtualDisplay->position.y;
            }
       
         // Add active display to the list
         display[activeId] = ptr_dynamic_cast<Screen, winDisplay>(currentDisplay);
         activeId++;
         }
         
      // Clear structure for next display (to ensure there is no old data)
      memset(&Device, 0, sizeof(Device));
      Device.cb = sizeof(Device);
      displayId++;
      }
   
   // API INDEPENDENT - WINDOWS DEPENDENT SECTION - END
   //-----------------------------------------------------

   // Load Vulkan dynamic library
   if (library == nullptr)
      {
      string info;
      info += "ERROR: Vulkan error: ";
      info += "       Cannot find Vulkan dynamic library.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // TODO: Get pointer to fuction needed to acquire other function pointers
   // vkGetInstanceProcAddr = 
   bindInstanceFunction( EnumerateInstanceExtensionProperties );
   bindInstanceFunction( EnumerateInstanceLayerProperties );
   bindInstanceFunction( CreateInstance );
   bindInstanceFunction( EnumeratePhysicalDevices );

   VkLayerProperties* layerProperties = nullptr;

   // Acquire list of supported Vulkan Layers
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateInstanceLayerProperties(&layersCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);

      if (layersCount == 0)
         break;

      // Allocate array of Layer Properties descriptors
      if (layerProperties)
         delete [] layerProperties;
      layerProperties = new VkLayerProperties[layersCount];
      Profile( vkEnumerateInstanceLayerProperties(&layersCount, layerProperties) )
      }
   layer = new LayerDescriptor[layersCount];

   // Acquire information about each Layer and supported extensions
   for(uint32 i=0; i<layersCount; ++i)
      {
      layer[i].properties = layerProperties[i];

      // Acquire list of all extensions supported by this layer
      lastResult = VK_INCOMPLETE;
      while(lastResult == VK_INCOMPLETE)
         {
         Profile( vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, nullptr) )
         if (IsWarning(lastResult))
            assert(0);
      
         if (layer[i].extensionsCount == 0)
            break;
      
         // Allocate array of Layer Extension descriptors
         if (layer[i].extension)
            delete [] layer[i].extension;
         layer[i].extension = new VkExtensionProperties[layer[i].extensionsCount];
         Profile( vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, layer[i].extension) )
         }
      }

   delete [] layerProperties;

   // Acquire list of all global extensions not being part of any layer
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);
   
      if (globalExtensionsCount == 0)
         break;

      if (globalExtension)
         delete [] globalExtension;
      globalExtension = new VkExtensionProperties[globalExtensionsCount];
      Profile( vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, globalExtension) )
      }

   // Choose Layers and extensions that will be enabled for Vulkan interface.
   // For now on lets enable all Layers and only needed extensions.

   // Create array of pointers to all layer names
   uint32 enabledLayersCount = layersCount;
   char** layersPtrs = new char*[layersCount];
   for(uint32 i=0; i<layersCount; ++i)  
      layersPtrs[i] = &layer[i].properties.layerName[0];

   // Enable WSI SwapChain extension
   uint32 enabledExtensionsCount = 0;
   char** extensionPtrs = new char*[globalExtensionsCount];
   for(uint32 i=0; i<globalExtensionsCount; ++i)
      if (strcmp("VK_EXT_KHR_swapchain", globalExtension[i].extensionName) == 0 )
         extensionPtrs[enabledExtensionsCount++] = &globalExtension[i].extensionName[0];

   // We need at least SwapChain working to display anything
   if (!enabledExtensionsCount)
      {
      string info;
      info += "ERROR: Vulkan error: ";
      info += "       Cannot find a compatible Vulkan installable client driver (ICD).\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert(0);
      }

   // This code would enable all Layers and all Extensions (lots of stuff)
   //
   //  // Create array of pointers to all layer names
   //  uint32 enabledLayersCount = layersCount;
   //  char* layersPtrs = char*[layersCount];
   //  uint32 index = 0;
   //  for(uint32 i=0; i<layersCount; ++i, ++index)  
   //     layersPtrs[index] = &layer[i].properties.layerName[0];
   //  
   //  // Calculate total amount of extensions supported by all Layers
   //  uint32 enabledExtensionsCount = globalExtensionsCount;
   //  for(uint32 i=0; i<layersCount; ++i)
   //     enabledExtensionsCount += layer[i].extensionsCount;  
   //  
   //  // Create array of pointers to this extension names
   //  char* extensionPtrs = char*[enabledExtensionsCount];
   //  uint32 index = 0;
   //  for(uint32 i=0; i<globalExtensionsCount; ++i, ++index)
   //     extensionPtrs[index] = &globalExtension[i].extName[0];
   //  for(uint32 i=0; i<layersCount; ++i)
   //     for(uint32 j=0; j<layer[i].extensionsCount; ++j, ++index)   
   //        extensionPtrs[index] = &layer[i].extension[j].extName[0];

   // Create Application API Instance
   VkApplicationInfo appInfo;
   appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   appInfo.pNext              = nullptr;
   appInfo.pApplicationName   = appName.c_str();
   appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
   appInfo.pEngineName        = "Ngine";
   appInfo.engineVersion      = VK_MAKE_VERSION(5, 0, 0);
   appInfo.apiVersion         = VK_API_VERSION;
   
   VkInstanceCreateInfo instInfo;
   instInfo.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   instInfo.pNext                     = nullptr;
   instInfo.flags                     = 0;                     // TODO: VkInstanceCreateFlags
   instInfo.pApplicationInfo          = &appInfo;
   instInfo.enabledLayerNameCount     = enabledLayersCount;
   instInfo.ppEnabledLayerNames       = enabledLayersCount     ? reinterpret_cast<const char*const*>(layersPtrs) : nullptr;
   instInfo.enabledExtensionNameCount = enabledExtensionsCount;
   instInfo.ppEnabledExtensionNames   = enabledExtensionsCount ? reinterpret_cast<const char*const*>(extensionPtrs) : nullptr;

   // TODO: Needs to have separate aloc callbacks for API
   //Profile( vkCreateInstance(&instInfo, &defaultAllocCallbacks, &instance) )

   // Enumerate available physical devices
   Profile( vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr) )
   VkPhysicalDevice* tempHandle = new VkPhysicalDevice[devicesCount];
   Profile( vkEnumeratePhysicalDevices(instance, &devicesCount, tempHandle) )

   // Create interfaces for all available physical devices
   device = new Ptr<VulkanDevice>[devicesCount];
   for(uint32 i=0; i<devicesCount; ++i)
      device[i] = new VulkanDevice(*tempHandle);
 
   // Bind Interface functions
   bindInterfaceFunctionPointers();

   delete [] tempHandle;
   }

   VulkanAPI::~VulkanAPI()
   {
   vkGetInstanceProcAddr                          = nullptr;
   vkEnumerateInstanceExtensionProperties         = nullptr;
   vkEnumerateInstanceLayerProperties             = nullptr;
   vkCreateInstance                               = nullptr;
   vkEnumeratePhysicalDevices                     = nullptr;

   vkGetPhysicalDeviceSurfaceSupportKHR           = nullptr;

   for(uint32 i=0; i<layersCount; ++i)
      delete [] layer[i].extension;
   delete [] layer;
   delete [] globalExtension;
   }



   void VulkanAPI::bindInterfaceFunctionPointers(void)
   {
   bindInstanceFunction( GetPhysicalDeviceSurfaceSupportKHR );
   }



// TODO: Window creation and bind !
//
//
//#ifdef EN_PLATFORM_WINDOWS
//    HINSTANCE connection;      // hInstance - Windows Instance
//    HWND      window;          // hWnd      - window handle
//#else
//    xcb_connection_t*        connection;
//    xcb_screen_t*            screen;
//    xcb_window_t             window;
//    xcb_intern_atom_reply_t* atom_wm_delete_window;
//    VkPlatformHandleXcbKHR   platformHandle;
//#endif
//
//   // Init connection
//#ifdef EN_PLATFORM_WINDOWS
//   connection = hInstance;
//#else
//   const xcb_setup_t*    setup;
//   xcb_screen_iterator_t iter;
//   int                   scr;
//   
//   connection = xcb_connect(nullptr, &scr);
//   if (demo->connection == nullptr) 
//      {
//      string info;
//      info += "ERROR: Vulkan error: ";
//      info += "       Cannot find a compatible Vulkan installable client driver (ICD).\n";
//      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
//      Log << info.c_str();
//      exit(0);
//      }
//   
//   setup = xcb_get_setup(connection);
//   iter = xcb_setup_roots_iterator(setup);
//   while (scr-- > 0)
//       xcb_screen_next(&iter);
//   
//   screen = iter.data;
//#endif
//
//   // Create VulkanAPI object here
//   VulkanAPI context = new VulkanAPI();
//
//   // Create Window
//    WNDCLASSEX  win_class;
//
//    // Initialize the window class structure:
//    win_class.cbSize = sizeof(WNDCLASSEX);
//    win_class.style = CS_HREDRAW | CS_VREDRAW;
//    win_class.lpfnWndProc = WndProc;
//    win_class.cbClsExtra = 0;
//    win_class.cbWndExtra = 0;
//    win_class.hInstance = demo->connection; // hInstance
//    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
//    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//    win_class.lpszMenuName = NULL;
//    win_class.lpszClassName = demo->name;
//    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
//    // Register window class:
//    if (!RegisterClassEx(&win_class)) {
//        // It didn't work, so try to give a useful error:
//        printf("Unexpected error trying to start the application!\n");
//        fflush(stdout);
//        exit(1);
//    }
//    // Create window with the registered class:
//    RECT wr = { 0, 0, demo->width, demo->height };
//    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
//    demo->window = CreateWindowEx(0,
//                                  demo->name,           // class name
//                                  demo->name,           // app name
//                                  WS_OVERLAPPEDWINDOW | // window style
//                                  WS_VISIBLE |
//                                  WS_SYSMENU,
//                                  100,100,              // x/y coords
//                                  wr.right-wr.left,     // width
//                                  wr.bottom-wr.top,     // height
//                                  NULL,                 // handle to parent
//                                  NULL,                 // handle to menu
//                                  demo->connection,     // hInstance
//                                  NULL);                // no extra parameters
//    if (!demo->window) {
//        // It didn't work, so try to give a useful error:
//        printf("Cannot create a window in which to draw!\n");
//        fflush(stdout);
//        exit(1);
//    }
//
//
//   // Attach Vulkan to Window (WSI)
//    VkResult err;
//    uint32_t i;
//
//
//
//   // Construct the WSI surface description:
//   VkSurfaceDescriptionWindowKHR surfaceInfo;
//   surfaceInfo.sType           = VK_STRUCTURE_TYPE_SURFACE_DESCRIPTION_WINDOW_KHR;
//   surfaceInfo.pNext           = nullptr;
//#ifdef _WIN32
//   surfaceInfo.platform        = VK_PLATFORM_WIN32_KHR;
//   surfaceInfo.pPlatformHandle = demo->connection;
//   surfaceInfo.pPlatformWindow = demo->window;
//#else  // _WIN32
//   platformHandle.connection = connection;
//   platformHandle.root       = screen->root;
//
//   surfaceInfo.platform        = VK_PLATFORM_XCB_KHR;
//   surfaceInfo.pPlatformHandle = &platformHandle;
//   surfaceInfo.pPlatformWindow = &window;
//#endif // _WIN32
//
//    // Iterate over each queue to learn whether it supports presenting to WSI:
//    VkBool32* supportsPresent = (VkBool32 *)malloc(demo->queue_count * sizeof(VkBool32));
//    for (i = 0; i < demo->queue_count; i++) {
//        demo->fpGetPhysicalDeviceSurfaceSupportKHR(demo->gpu, i, (VkSurfaceDescriptionKHR*) &surfaceInfo, &supportsPresent[i]);
//    }
//
//    // Search for a graphics and a present queue in the array of queue
//    // families, try to find one that supports both
//    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
//    uint32_t presentQueueNodeIndex  = UINT32_MAX;
//    for (i = 0; i < demo->queue_count; i++) {
//        if ((demo->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
//            if (graphicsQueueNodeIndex == UINT32_MAX) {
//                graphicsQueueNodeIndex = i;
//            }
//            
//            if (supportsPresent[i] == VK_TRUE) {
//                graphicsQueueNodeIndex = i;
//                presentQueueNodeIndex = i;
//                break;
//            }
//        }
//    }
//    if (presentQueueNodeIndex == UINT32_MAX) {
//        // If didn't find a queue that supports both graphics and present, then
//        // find a separate present queue.
//        for (uint32_t i = 0; i < demo->queue_count; ++i) {
//            if (supportsPresent[i] == VK_TRUE) {
//                presentQueueNodeIndex = i;
//                break;
//            }
//        }
//    }
//    free(supportsPresent);
//
//    // Generate error if could not find both a graphics and a present queue
//    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
//        ERR_EXIT("Could not find a graphics and a present queue\n",
//                 "WSI Initialization Failure");
//    }
//
//    // TODO: Add support for separate queues, including presentation,
//    //       synchronization, and appropriate tracking for QueueSubmit
//    // While it is possible for an application to use a separate graphics and a
//    // present queues, this demo program assumes it is only using one:
//    if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
//        ERR_EXIT("Could not find a common graphics and a present queue\n",
//                 "WSI Initialization Failure");
//    }
//
//    demo->graphics_queue_node_index = graphicsQueueNodeIndex;
//
//    err = vkGetDeviceQueue(demo->device, demo->graphics_queue_node_index,
//            0, &demo->queue);
//    assert(!err);
//
//    // Get the list of VkFormat's that are supported:
//    uint32_t formatCount;
//    err = demo->fpGetSurfaceFormatsKHR(demo->device,
//                                    (VkSurfaceDescriptionKHR *) &demo->surface_description,
//                                    &formatCount, NULL);
//    assert(!err);
//    VkSurfaceFormatKHR *surfFormats =
//        (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
//    err = demo->fpGetSurfaceFormatsKHR(demo->device,
//                                    (VkSurfaceDescriptionKHR *) &demo->surface_description,
//                                    &formatCount, surfFormats);
//    assert(!err);
//    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
//    // the surface has no preferred format.  Otherwise, at least one
//    // supported format will be returned.
//    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
//    {
//        demo->format = VK_FORMAT_B8G8R8A8_UNORM;
//    }
//    else
//    {
//        assert(formatCount >= 1);
//        demo->format = surfFormats[0].format;
//    }
//    demo->color_space = surfFormats[0].colorSpace;
//
//    demo->quit = false;
//    demo->curFrame = 0;
//
//    // Get Memory information and properties
//    err = vkGetPhysicalDeviceMemoryProperties(demo->gpu, &demo->memory_properties);
//    assert(!err);
//
//
//
//   // Here Window is created and Vulkan context is bound to it.
//   // We can render.


   }
}
#endif
