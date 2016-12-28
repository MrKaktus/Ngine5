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

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/log/log.h"

#include "core/storage.h"   // For Pipeline Cache handling
#include "utilities/strings.h"

#include "core/utilities/memory.h"
#include "core/rendering/vulkan/vkTexture.h"
#include "core/rendering/vulkan/vkSynchronization.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "platform/windows/win_events.h"
#endif

namespace en
{
   namespace gpu
   {
   void unbindedVulkanFunctionHandler(...)
   {
   Log << "ERROR: Called unbinded Vulkan function.\n";
   assert( 0 );
   }

   // MACROS: Safe Vulkan function binding with fallback
   //         Both Macros should be used only inside VulkanAPI and VulkanDevice class methods.

   
   
#if defined(EN_PLATFORM_LINUX)
   #define LoadProcAddress dlsym
#endif

#if defined(EN_PLATFORM_WINDOWS)
   #define LoadProcAddress GetProcAddress
#endif

   #define DeclareFunction(function)                                             \
   PFN_##function function;

   #define LoadFunction(function)                                                \
   {                                                                             \
   function = (PFN_##function)LoadProcAddress(library, #function);               \
   if (function == nullptr)                                                      \
      {                                                                          \
      function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
      Log << "Error: Cannot bind function " << #function << endl;                \
      }                                                                          \
   }

   #define LoadGlobalFunction(function)                                          \
   {                                                                             \
   function = (PFN_##function) vkGetInstanceProcAddr(nullptr, #function);        \
   if (function == nullptr)                                                      \
      {                                                                          \
      function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
      Log << "Error: Cannot bind global function " << #function << endl;         \
      }                                                                          \
   }
   
   #define LoadInstanceFunction(function)                                        \
   {                                                                             \
   function = (PFN_##function) vkGetInstanceProcAddr(instance, #function);       \
   if (function == nullptr)                                                      \
      {                                                                          \
      function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
      Log << "Error: Cannot bind instance function " << #function << endl;       \
      }                                                                          \
   }

   #define LoadDeviceFunction(function)                                          \
   {                                                                             \
   /* We don't need to do this. api is already declared in Device.               \ 
   VulkanAPI* api = raw_reinterpret_cast<VulkanAPI>(&Graphics); */               \
   function = (PFN_##function) api->vkGetDeviceProcAddr(device, #function);      \
   if (function == nullptr)                                                      \
      {                                                                          \
      function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
      Log << "Error: Cannot bind device function " << #function << endl;         \
      }                                                                          \
   }
   
   
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
      





#if defined(EN_PLATFORM_WINDOWS)
   winDisplay::winDisplay() :
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
      AppInstance(nullptr),
      hWnd(nullptr),
      CommonWindow()
   {
   _mode = settings.mode;
   
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
      assert( 0 );
      }
   
   _display  = ptr_reinterpret_cast<CommonDisplay>(&selectedDisplay);
   _position = settings.position;
   _size     = selectedResolution;
 //_resolution will be set by child class implementing given Graphics API Swap-Chain
   }

   winWindow::~winWindow()
   {
   // Reset to old display settings
   if (_fullscreen)
      {
      ChangeDisplaySettings(nullptr, 0);
      ShowCursor(TRUE);
      }

   // Delete window
   DestroyWindow(hWnd);

   // Unregister window class
   UnregisterClass(L"WindowClass", AppInstance);
   }

   bool winWindow::movable(void)
   {
   if (_mode == WindowMode::Windowed)
      return true;
      
   return false;
   }
   
   void winWindow::move(const uint32v2 position)
   {
   if (_mode != WindowMode::Windowed)
      return;
      
   SetWindowPos(hWnd, HWND_TOP, position.x, position.y, _size.x, _size.y, SWP_NOSIZE | SWP_NOZORDER);
   // or MoveWindow();
   }
   
   // TODO: This method should be called by specialization class implementation that will chage Swap-Chain !
   void winWindow::resize(const uint32v2 size)
   {
   if (_mode != WindowMode::Windowed)
      return;
      
   // New window size
   WindowRect.left      = (long)0;      
   WindowRect.right     = (long)size.width;
   WindowRect.top       = (long)0;    
   WindowRect.bottom    = (long)size.height;
     
   // Calculate window final size after taking into notice borders, etc.
   Style   = WS_CLIPSIBLINGS |                       // \_Prevents from overdrawing
             WS_CLIPCHILDREN |                       // / by other windows
             WS_POPUP        |
             WS_OVERLAPPEDWINDOW;                    // Window style
   ExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;     // Window extended style
      
   AdjustWindowRectEx(&WindowRect, Style, FALSE, ExStyle);  // Calculates true size of window with bars and borders
   
   SetWindowPos(hWnd, HWND_TOP, _position.x, _position.y,
                WindowRect.right, WindowRect.bottom,
                SWP_NOMOVE | SWP_NOZORDER);
   }
   
   void winWindow::active(void)
   {
   ShowWindow(hWnd, SW_SHOW);
   SetForegroundWindow(hWnd);
   SetFocus(hWnd);
   SetActiveWindow(hWnd);
   }

   void winWindow::transparent(const float opacity)
   {
   // TODO: Finish !
   }
   
   void winWindow::opaque(void)
   {
   // TODO: Finish !
   }
#endif

   WindowVK::WindowVK(VulkanDevice* _gpu,
                      const Ptr<CommonDisplay> selectedDisplay,
                      const uint32v2 selectedResolution,
                      const WindowSettings& settings,
                      const string title) :
#if defined(EN_PLATFORM_WINDOWS)
      gpu(_gpu),
      // Create native OS window or assert.
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
      info.pNext              = _fullscreen ? &displayInfo : nullptr;     // If using VK_KHR_display_swapchain, pass additional information.
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

   // TODO: Currently max is 16MB. This should be configurable through config file.
   #define PipelineCacheMaximumSize 16*1024*1024


   VulkanDevice::VulkanDevice(VulkanAPI* _api, const uint32 _index, const VkPhysicalDevice _handle) :
      api(_api),
      index(_index),
      handle(_handle),
      queueFamily(nullptr),
      queueFamiliesCount(0),
      queueFamilyIndices(nullptr),
      globalExtension(nullptr),
      globalExtensionsCount(0),
      pipelineCache(VK_NULL_HANDLE),
      rebuildCache(true),
      memoryRAM(0),
      memoryDriver(0),
      CommonDevice()
   {
   for(uint32 i=0; i<MaxSupportedWorkerThreads; ++i)
      lastResult[i] = VK_SUCCESS;
      
   // Clear Device function pointers
   clearDeviceFunctionPointers();

   // Init default memory allocation callbacks
   defaultAllocCallbacks.pUserData             = this;
   defaultAllocCallbacks.pfnAllocation         = defaultAlloc;
   defaultAllocCallbacks.pfnReallocation       = defaultRealloc;
   defaultAllocCallbacks.pfnFree               = defaultFree;
   defaultAllocCallbacks.pfnInternalAllocation = defaultIntAlloc;
   defaultAllocCallbacks.pfnInternalFree       = defaultIntFree;

   // Gather Device Capabilities
   ProfileNoRet( api, vkGetPhysicalDeviceFeatures(handle, &features) )
   ProfileNoRet( api, vkGetPhysicalDeviceProperties(handle, &properties) )
   ProfileNoRet( api, vkGetPhysicalDeviceMemoryProperties(handle, &memory) )

    for(uint32_t i=0; i<memory.memoryHeapCount; ++i)
       {
       printf("Memory range %i:\n\n", i);
       printf("    Size          : %llu\n", memory.memoryHeaps[i].size);
       printf("    Backing memory: %s\n", memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "VRAM" : "RAM");
       printf("    Memory types  :\n");
       for(uint32_t j=0; j<memory.memoryTypeCount; ++j)
          if (memory.memoryTypes[j].heapIndex == i)
             {
             printf("    Type: %i\n", j);
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n");
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n");
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n");
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_HOST_CACHED_BIT\n\n\n"); 
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n\n\n"); 
             }

       }


 // TODO: Gather even more information
 //ProfileNoRet( api, vkGetPhysicalDeviceFormatProperties(handle, VkFormat format, VkFormatProperties* pFormatProperties) )
 //ProfileNoRet( api, vkGetPhysicalDeviceImageFormatProperties(handle, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) )

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
   ProfileNoRet( api, vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr) )
   queueFamily = new VkQueueFamilyProperties[queueFamiliesCount];
   ProfileNoRet( api, vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamily) )

   // Generate list of all Queue Family indices.
   // This list will be passed during resource creation time for resources that need to be populated first.
   // This way we can use separate Transfer Queues for faster data transfer, and driver knows to use
   // proper synchronization mechanisms to prevent hazards and race conditions.
   queueFamilyIndices = new uint32[queueFamiliesCount];
   for (uint32 i=0; i<queueFamiliesCount; ++i)
       queueFamilyIndices[i] = i;

   // Map Queue Families and their Queue Count to QueueType and Index.
   memset(&queuesCount[0], 0, sizeof(queuesCount));
   memset(&queueTypeToFamily[0], 0, sizeof(queueTypeToFamily));
   for(uint32_t family=0; family<queueFamiliesCount; ++family)
      {
      uint32 flags = queueFamily[family].queueFlags;
      
      // For now, we assume that there will be always only one Queue Family matching one Queue Type.
      // If that's not true in the future, then below, each range of Queues in given Family, will need
      // to be mapped to range of Queues in given Type. If such situation will occur, assertions below
      // will fire at runtime.
      if (flags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Universal)] == 0 );
         queuesCount[underlyingType(QueueType::Universal)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Universal)] = family;
         }
      else
      if (flags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Universal)] == 0 );
         queuesCount[underlyingType(QueueType::Universal)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Universal)] = family;
         }
      else
      if (flags == (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Compute)] == 0 );
         queuesCount[underlyingType(QueueType::Compute)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Compute)] = family;
         }
      else
      if (flags == (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::Compute)] == 0 );
         queuesCount[underlyingType(QueueType::Compute)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::Compute)] = family;
         }
      else
      if (flags == (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
         {
         assert( queuesCount[underlyingType(QueueType::SparseTransfer)] == 0 );
         queuesCount[underlyingType(QueueType::SparseTransfer)] = queueFamily[family].queueCount;
         queueTypeToFamily[underlyingType(QueueType::SparseTransfer)] = family;
         }
      else
      if (flags == (VK_QUEUE_TRANSFER_BIT))
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
         Log << "    Queues in Family: " << queueFamily[family].queueCount << endl;
         Log << "    Queue Min Transfer Width : " << queueFamily[family].minImageTransferGranularity.width << endl;
         Log << "    Queue Min Transfer Height: " << queueFamily[family].minImageTransferGranularity.height << endl;
         Log << "    Queue Min Transfer Depth : " << queueFamily[family].minImageTransferGranularity.depth << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            Log << "    Queue supports: GRAPHICS" << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
            Log << "    Queue supports: COMPUTE" << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
            Log << "    Queue supports: TRANSFER" << endl;
         if (queueFamily[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            Log << "    Queue supports: SPARSE_BINDING" << endl;
         Log << "    Queues Time Stamp: " << queueFamily[family].timestampValidBits << endl << endl;
         }
      }
   
#if defined(EN_DEBUG)
   // Debug log Queue Families
   for(uint32_t family=0; family<queueFamiliesCount; ++family)
      {
      Log << "Queue Family %i:" << family << endl;

      Log << "    Queues: " << queueFamily[family].queueCount << endl;
      Log << "    Queue Min Transfer W: " << queueFamily[family].minImageTransferGranularity.width << endl;
      Log << "    Queue Min Transfer H: " << queueFamily[family].minImageTransferGranularity.height << endl;
      Log << "    Queue Min Transfer D: " << queueFamily[family].minImageTransferGranularity.depth << endl;
      if (queueFamily[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
         Log << "    Family supports: GRAPHICS\n";
      if (queueFamily[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
         Log << "    Family supports: COMPUTE\n";
      if (queueFamily[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
         Log << "    Family supports: TRANSFER\n";
      if (queueFamily[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
         Log << "    Family supports: SPARSE_BINDING\n";
      Log << "    Queues Time Stamp: " << queueFamily[family].timestampValidBits << endl << endl;
      }
#endif

   // Device Extensions
   //-------------------

   // Acquire list of Device extensions
   Profile( api, vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, nullptr) )
   if (IsWarning(api->lastResult[Scheduler.core()]))
      assert( 0 );

   if (globalExtensionsCount > 0)
      {
      globalExtension = new VkExtensionProperties[globalExtensionsCount];
      Profile( api, vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, globalExtension) )
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

   // Specify Vulkan Extensions to initialize
   //-----------------------------------------

   uint32 enabledExtensionsCount = 0;
   char** extensionPtrs = nullptr;
   extensionPtrs = new char*[globalExtensionsCount];
   
   // Adding Windowing System Interface extensions to the list
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
#if defined(EN_PLATFORM_ANDROID)
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_LINUX)
   // TODO: Pick one on Linux (as usual it's complete mess)
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_MIR_SURFACE_EXTENSION_NAME;
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_WINDOWS)
   //extensionPtrs[enabledExtensionsCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif

   // TODO: Add here loading list of needed extensions from some config file.
   //       (generated automatically by the engine/editor based on features
   //        used by the app).

   // Verify selected extensions are available
   for(uint32 i=0; i<enabledExtensionsCount; ++i)
      {
      bool found = false;
      for(uint32 j=0; j<globalExtensionsCount; ++j)
         if (strcmp(extensionPtrs[i], globalExtension[j].extensionName) == 0 )
            found = true;
         
      if (!found)
         {
         Log << "ERROR: Requested Vulkan extension " << extensionPtrs[i] << " is not supported on this system!" << endl;
         assert( 0 );
         }
      }

   // Create Vulkan Device
   //----------------------

   // Create Device Interface
   VkDeviceCreateInfo deviceInfo;
   deviceInfo.sType                     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   deviceInfo.pNext                     = nullptr;
   deviceInfo.flags                     = 0;                     // Reserved
   deviceInfo.queueCreateInfoCount      = queueFamiliesCount;
   deviceInfo.pQueueCreateInfos         = queueFamilyInfo;
   deviceInfo.enabledLayerCount         = 0;                     // Deprecated, ignored.
   deviceInfo.ppEnabledLayerNames       = nullptr;               // Deprecated, ignored.
   deviceInfo.enabledExtensionCount     = enabledExtensionsCount;
   deviceInfo.ppEnabledExtensionNames   = reinterpret_cast<const char*const*>(extensionPtrs);
   deviceInfo.pEnabledFeatures          = nullptr;

   // TODO: In the future provide our own allocation callbacks to track
   //       and analyze drivers system memory usage (&defaultAllocCallbacks).

   Profile( api, vkCreateDevice(handle, &deviceInfo, nullptr, &device) )

   // Bind Device Functions
   loadDeviceFunctionPointers();

   // Free temporary resources
   delete [] extensionPtrs;

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
         uint32 queueFamilyId = queueTypeToFamily[i];

         // It's abstract object as it has no size.
         // It's not tied to Queueu, but to Queue Family, so it can be shared by all Queues in this family.
         // It's single threaded, and so should be Command Buffers allocated from it
         // (so each Thread gets it's own Pool per each Family, and have it's own Command Buffers).
         VkCommandPoolCreateInfo poolInfo;
         poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
         poolInfo.pNext            = nullptr;
         poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // To reuse CB's use VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
         poolInfo.queueFamilyIndex = queueFamilyId;
         
         Profile( this, vkCreateCommandPool(device, &poolInfo, nullptr, (VkCommandPool*)(&commandPool[thread][i])) )
         }

   // <<<< Per Thread Section


   // TODO: Do we want to support reset of Command Pools? Should it go to API?
   //
   // // Resets Command Pool, frees all resources encoded on all CB's created from this pool. CB's are reset to initial state.
   // uint32 thread = 0; 
   // uint32 type = 0;
   // Profile( vkResetCommandPool(VkDevice device, commandPool[thread][type], VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) )


   // Pipeline Cache
   //---------------

   // Try to load cache from disk
   uint64 cacheSize = 0u;
   void*  cacheData = loadPipelineCache(cacheSize);

   // Create runtime Pipeline cache, and reuse previous data if possible
   VkPipelineCacheCreateInfo cacheInfo;
   cacheInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
   cacheInfo.pNext           = nullptr;
   cacheInfo.flags           = 0u;
   cacheInfo.initialDataSize = cacheSize; // Cache size will never be greater than 4GB
   cacheInfo.pInitialData    = cacheData;

   Profile( this, vkCreatePipelineCache(device, &cacheInfo, nullptr, &pipelineCache) )

   // Release temporary buffer
   delete [] cacheData;

   init();
   }


   void* VulkanDevice::loadPipelineCache(uint64& size)
   {
   using namespace en::storage;

   // Pipeline cache header
   aligned(1) 
   struct PipelineCacheHeader
      {
      uint32 headerSize;
      VkPipelineCacheHeaderVersion version;
      uint32 vendorID;
      uint32 deviceID;
      uint32 cacheUUID[4];
      };
   aligndefault

   // Try to reuse pipeline cache from disk. 
   // It is assumed that devices are always enumerated in the same order.
   Nfile* file = nullptr;
   string filename = string("gpu") + stringFrom(index) + string("pipelineCache.data");
   if (!Storage.open(filename, &file))
      return nullptr;

   // Verify that cache file is not corrupted
   uint64 diskCacheSize = file->size();
   if (diskCacheSize < sizeof(PipelineCacheHeader))
      {
      delete file;
      return nullptr;
      }

   // Size of cache copy on disk shouldn't exceed the limit
   if (diskCacheSize > PipelineCacheMaximumSize)
      {
      delete file;
      return nullptr;
      }

   // Read header of cache stored on disk
   PipelineCacheHeader diskHeader;
   file->read(0u, sizeof(PipelineCacheHeader), &diskHeader);

   // Read size of driver cache in RAM (should be at least size of a header)
   uint64 cacheSize = 0u;
   Profile( this, vkGetPipelineCacheData(device, pipelineCache, (size_t*)(&cacheSize), nullptr) )
   if (cacheSize < sizeof(PipelineCacheHeader))
      {
      delete file;
      return nullptr;
      }

   // Read driver cache header
   PipelineCacheHeader driverHeader;
   uint64 headerSize = sizeof(PipelineCacheHeader);
   Profile( this, vkGetPipelineCacheData(device, pipelineCache, (size_t*)(&headerSize), &driverHeader) )

   // Ensure that both headers match. This means that cache stored on 
   // disk is still valid (headers won't match after drivers update, GPU change, etc.).
   if (memcmp(&diskHeader, &driverHeader, sizeof(PipelineCacheHeader)) != 0u)
      {
      delete file;
      return nullptr;
      }

   // Load previously cached pipeline state objects from disk
   uint8* cacheData = new uint8[diskCacheSize];
   file->read(&cacheData);
   delete file;

   size = diskCacheSize;
   rebuildCache = false;
   return cacheData;
   }

   void VulkanDevice::init()
   {
   // TODO: Populate API capabilities


   for(uint32 i=0; i<MaxSupportedWorkerThreads; i++)
      {
      commandBuffersExecuting[i] = 0u;
      for(uint32 j=0; j<MaxCommandBuffersExecuting; j++)      
         commandBuffers[i][j] = nullptr;
      }

   initMemoryManager();
   CommonDevice::init();
   }

   VulkanDevice::~VulkanDevice()
   {
   Profile( this, vkDeviceWaitIdle(device) )
   
   // If requested recreate pipeline cache on disk
   if (rebuildCache)
      {
      using namespace en::storage;

      // Save cache to disk (at app end):
      // - if marked for recreation of HDD cache
      //   - retrieve current cache size
      //   - retrieve cache dat
      //   - delete cache on HDD
      //   - store it as new cache on HDD

      // Read size of driver cache
      uint64 cacheSize = 0u;
      Profile( this, vkGetPipelineCacheData(device, pipelineCache, (size_t*)(&cacheSize), nullptr) )
      
      // Try to reuse Pipeline objects from the previous application run (read from drivers cache on the disk).
      if (cacheSize > 0u)
         {
         // Size of cache copy on disk shouldn't exceed the limit
         if (cacheSize > PipelineCacheMaximumSize)
            cacheSize = PipelineCacheMaximumSize;

         uint8* cacheData = new uint8[cacheSize];
         Profile( this, vkGetPipelineCacheData(device, pipelineCache, (size_t*)(&cacheSize), cacheData) )

         Nfile* file = nullptr;
         string filename = string("gpu") + stringFrom(index) + string("pipelineCache.data");
         if (Storage.open(filename, &file, FileAccess::Write))
            {
            file->write(cacheSize, cacheData);
            delete file;
            }

         delete [] cacheData;
         }
      }

   ProfileNoRet( this, vkDestroyPipelineCache(device, pipelineCache, nullptr) )

   // <<<< Per Thread Section (TODO: Execute on each Worker Thread)
   uint32 thread = Scheduler.core();

   // Release all Command Pools used by this thread for Commands submission
   for(uint32 i=0; i<underlyingType(QueueType::Count); ++i)
      if (queuesCount[i] > 0)
         ProfileNoRet( this, vkDestroyCommandPool(device, commandPool[thread][i], nullptr) )

   // <<<< Per Thread Section

   if (device != VK_NULL_HANDLE)
      ProfileNoRet( this, vkDestroyDevice(device, nullptr) )  // Instance or Device function ?

   delete [] queueFamily;
   delete [] queueFamilyIndices;
   delete [] globalExtension;
   }
   
   void VulkanDevice::loadDeviceFunctionPointers(void)
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
   LoadDeviceFunction( vkDestroyInstance )
   LoadDeviceFunction( vkGetPhysicalDeviceFeatures )
   LoadDeviceFunction( vkGetPhysicalDeviceFormatProperties )
   LoadDeviceFunction( vkGetPhysicalDeviceProperties )
   LoadDeviceFunction( vkGetPhysicalDeviceQueueFamilyProperties )
   LoadDeviceFunction( vkGetPhysicalDeviceMemoryProperties )
   LoadDeviceFunction( vkCreateDevice )
   LoadDeviceFunction( vkDestroyDevice )
   LoadDeviceFunction( vkEnumerateDeviceExtensionProperties )
   LoadDeviceFunction( vkEnumerateDeviceLayerProperties )
   LoadDeviceFunction( vkGetDeviceQueue )
   LoadDeviceFunction( vkQueueSubmit )
   LoadDeviceFunction( vkQueueWaitIdle )
   LoadDeviceFunction( vkDeviceWaitIdle )
   LoadDeviceFunction( vkAllocateMemory )
   LoadDeviceFunction( vkFreeMemory )
   LoadDeviceFunction( vkMapMemory )
   LoadDeviceFunction( vkUnmapMemory )
   LoadDeviceFunction( vkFlushMappedMemoryRanges )
   LoadDeviceFunction( vkInvalidateMappedMemoryRanges )
   LoadDeviceFunction( vkGetDeviceMemoryCommitment )
   LoadDeviceFunction( vkBindBufferMemory )
   LoadDeviceFunction( vkBindImageMemory )
   LoadDeviceFunction( vkGetBufferMemoryRequirements )
   LoadDeviceFunction( vkGetImageMemoryRequirements )
   LoadDeviceFunction( vkGetImageSparseMemoryRequirements )
   LoadDeviceFunction( vkGetPhysicalDeviceSparseImageFormatProperties )
   LoadDeviceFunction( vkQueueBindSparse )
   LoadDeviceFunction( vkCreateFence )
   LoadDeviceFunction( vkDestroyFence )
   LoadDeviceFunction( vkResetFences )
   LoadDeviceFunction( vkGetFenceStatus )
   LoadDeviceFunction( vkWaitForFences )
   LoadDeviceFunction( vkCreateSemaphore )
   LoadDeviceFunction( vkDestroySemaphore )
   LoadDeviceFunction( vkCreateEvent )
   LoadDeviceFunction( vkDestroyEvent )
   LoadDeviceFunction( vkGetEventStatus )
   LoadDeviceFunction( vkSetEvent )
   LoadDeviceFunction( vkResetEvent )
   LoadDeviceFunction( vkCreateQueryPool )
   LoadDeviceFunction( vkDestroyQueryPool )
   LoadDeviceFunction( vkGetQueryPoolResults )
   LoadDeviceFunction( vkCreateBuffer )
   LoadDeviceFunction( vkDestroyBuffer )
   LoadDeviceFunction( vkCreateBufferView )
   LoadDeviceFunction( vkDestroyBufferView )
   LoadDeviceFunction( vkCreateImage )
   LoadDeviceFunction( vkDestroyImage )
   LoadDeviceFunction( vkGetImageSubresourceLayout )
   LoadDeviceFunction( vkCreateImageView )
   LoadDeviceFunction( vkDestroyImageView )
   LoadDeviceFunction( vkCreateShaderModule )
   LoadDeviceFunction( vkDestroyShaderModule )
   LoadDeviceFunction( vkCreatePipelineCache )
   LoadDeviceFunction( vkDestroyPipelineCache )
   LoadDeviceFunction( vkGetPipelineCacheData )
   LoadDeviceFunction( vkMergePipelineCaches )
   LoadDeviceFunction( vkCreateGraphicsPipelines )
   LoadDeviceFunction( vkCreateComputePipelines )
   LoadDeviceFunction( vkDestroyPipeline )
   LoadDeviceFunction( vkCreatePipelineLayout )
   LoadDeviceFunction( vkDestroyPipelineLayout )
   LoadDeviceFunction( vkCreateSampler )
   LoadDeviceFunction( vkDestroySampler )
   LoadDeviceFunction( vkCreateDescriptorSetLayout )
   LoadDeviceFunction( vkDestroyDescriptorSetLayout )
   LoadDeviceFunction( vkCreateDescriptorPool )
   LoadDeviceFunction( vkDestroyDescriptorPool )
   LoadDeviceFunction( vkResetDescriptorPool )
   LoadDeviceFunction( vkAllocateDescriptorSets )
   LoadDeviceFunction( vkFreeDescriptorSets )
   LoadDeviceFunction( vkUpdateDescriptorSets )
   LoadDeviceFunction( vkCreateFramebuffer )
   LoadDeviceFunction( vkDestroyFramebuffer )
   LoadDeviceFunction( vkCreateRenderPass )
   LoadDeviceFunction( vkDestroyRenderPass )
   LoadDeviceFunction( vkGetRenderAreaGranularity )
   LoadDeviceFunction( vkCreateCommandPool )
   LoadDeviceFunction( vkDestroyCommandPool )
   LoadDeviceFunction( vkResetCommandPool )
   LoadDeviceFunction( vkAllocateCommandBuffers )
   LoadDeviceFunction( vkFreeCommandBuffers )
   LoadDeviceFunction( vkBeginCommandBuffer )
   LoadDeviceFunction( vkEndCommandBuffer )
   LoadDeviceFunction( vkResetCommandBuffer )
   LoadDeviceFunction( vkCmdBindPipeline )
   LoadDeviceFunction( vkCmdSetViewport )
   LoadDeviceFunction( vkCmdSetScissor )
   LoadDeviceFunction( vkCmdSetLineWidth )
   LoadDeviceFunction( vkCmdSetDepthBias )
   LoadDeviceFunction( vkCmdSetBlendConstants )
   LoadDeviceFunction( vkCmdSetDepthBounds )
   LoadDeviceFunction( vkCmdSetStencilCompareMask )
   LoadDeviceFunction( vkCmdSetStencilWriteMask )
   LoadDeviceFunction( vkCmdSetStencilReference )
   LoadDeviceFunction( vkCmdBindDescriptorSets )
   LoadDeviceFunction( vkCmdBindIndexBuffer )
   LoadDeviceFunction( vkCmdBindVertexBuffers )
   LoadDeviceFunction( vkCmdDraw )
   LoadDeviceFunction( vkCmdDrawIndexed )
   LoadDeviceFunction( vkCmdDrawIndirect )
   LoadDeviceFunction( vkCmdDrawIndexedIndirect )
   LoadDeviceFunction( vkCmdDispatch )
   LoadDeviceFunction( vkCmdDispatchIndirect )
   LoadDeviceFunction( vkCmdCopyBuffer )
   LoadDeviceFunction( vkCmdCopyImage )
   LoadDeviceFunction( vkCmdBlitImage )
   LoadDeviceFunction( vkCmdCopyBufferToImage )
   LoadDeviceFunction( vkCmdCopyImageToBuffer )
   LoadDeviceFunction( vkCmdUpdateBuffer )
   LoadDeviceFunction( vkCmdFillBuffer )
   LoadDeviceFunction( vkCmdClearColorImage )
   LoadDeviceFunction( vkCmdClearDepthStencilImage )
   LoadDeviceFunction( vkCmdClearAttachments )
   LoadDeviceFunction( vkCmdResolveImage )
   LoadDeviceFunction( vkCmdSetEvent )
   LoadDeviceFunction( vkCmdResetEvent )
   LoadDeviceFunction( vkCmdWaitEvents )
   LoadDeviceFunction( vkCmdPipelineBarrier )
   LoadDeviceFunction( vkCmdBeginQuery )
   LoadDeviceFunction( vkCmdEndQuery )
   LoadDeviceFunction( vkCmdResetQueryPool )
   LoadDeviceFunction( vkCmdWriteTimestamp )
   LoadDeviceFunction( vkCmdCopyQueryPoolResults )
   LoadDeviceFunction( vkCmdPushConstants )
   LoadDeviceFunction( vkCmdBeginRenderPass )
   LoadDeviceFunction( vkCmdNextSubpass )
   LoadDeviceFunction( vkCmdEndRenderPass )
   LoadDeviceFunction( vkCmdExecuteCommands )

   // VK_KHR_surface - Interface extension

   // VK_KHR_swapchain
   LoadDeviceFunction( vkCreateSwapchainKHR )
   LoadDeviceFunction( vkDestroySwapchainKHR )
   LoadDeviceFunction( vkGetSwapchainImagesKHR )
   LoadDeviceFunction( vkAcquireNextImageKHR )
   LoadDeviceFunction( vkQueuePresentKHR )

   // VK_KHR_swapchain 
   LoadDeviceFunction( vkCreateSwapchainKHR )
   LoadDeviceFunction( vkDestroySwapchainKHR )
   LoadDeviceFunction( vkGetSwapchainImagesKHR )
   LoadDeviceFunction( vkAcquireNextImageKHR )
   LoadDeviceFunction( vkQueuePresentKHR )
      
   // VK_KHR_display
   LoadDeviceFunction( vkGetPhysicalDeviceDisplayPropertiesKHR )
   LoadDeviceFunction( vkGetPhysicalDeviceDisplayPlanePropertiesKHR )
   LoadDeviceFunction( vkGetDisplayPlaneSupportedDisplaysKHR )
   LoadDeviceFunction( vkGetDisplayModePropertiesKHR )
   LoadDeviceFunction( vkCreateDisplayModeKHR )
   LoadDeviceFunction( vkGetDisplayPlaneCapabilitiesKHR )
   LoadDeviceFunction( vkCreateDisplayPlaneSurfaceKHR )
      
   // VK_KHR_display_swapchain
   LoadDeviceFunction( vkCreateSharedSwapchainsKHR )
      
   // Windowing System Interface - OS Specyific extension
   
   // VK_KHR_xlib_surface
   // VK_KHR_xcb_surface
   // VK_KHR_wayland_surface
   // VK_KHR_mir_surface
   // VK_KHR_android_surface
      
   // VK_KHR_win32_surface - Interface extension
   }

   void VulkanDevice::clearDeviceFunctionPointers(void)
   {
   // Vulkan 1.0
   //
   vkCreateInstance                               = nullptr;
   vkDestroyInstance                              = nullptr;
   vkEnumeratePhysicalDevices                     = nullptr;
   vkGetPhysicalDeviceFeatures                    = nullptr;
   vkGetPhysicalDeviceFormatProperties            = nullptr;
   vkGetPhysicalDeviceProperties                  = nullptr;
   vkGetPhysicalDeviceQueueFamilyProperties       = nullptr;
   vkGetPhysicalDeviceMemoryProperties            = nullptr;
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










   uint32 VulkanDevice::displays(void) const
   {
   // Currently all Vulkan devices share all available displays
   Ptr<VulkanAPI> api = ptr_reinterpret_cast<VulkanAPI>(&en::Graphics);
   return api->displaysCount;
   }
   
   Ptr<Display> VulkanDevice::display(uint32 index) const
   {
   // Currently all Vulkan devices share all available displays
   Ptr<VulkanAPI> api = ptr_reinterpret_cast<VulkanAPI>(&en::Graphics);
     
   assert( api->displaysCount > index );
   
   return ptr_reinterpret_cast<Display>(&api->displayArray[index]);
   }

   uint32 VulkanDevice::queues(const QueueType type) const
   {
   return queuesCount[underlyingType(type)];  //Need translation table from (Type, N) -> (Family, Index)
   }

   Ptr<Texture> VulkanDevice::createSharedTexture(Ptr<SharedSurface> backingSurface)
   {
   // Vulkan is not supporting cross-API / cross-process surfaces for now.
   // Implement it in the future.
   assert( 0 );
   return Ptr<Texture>(nullptr);
   }





   //VkDeviceMemory VulkanDevice::allocMemory(VkMemoryRequirements requirements, VkFlags properties)
   //{
   //VkDeviceMemory handle;

   //// Find Memory Type that best suits required allocation
   //uint32 index = VK_MAX_MEMORY_TYPES;
   //for(uint32 i=0; i<memory.memoryTypeCount; ++i)
   //   {
   //   // Memory Type needs to be able to support requested allocation
   //   if (checkBit(requirements.memoryTypeBits, (i+1)))
   //      // Memory Type needs to support at least requested sub-set of memory properties
   //      if ((memory.memoryTypes[i].propertyFlags & properties) == properties)
   //         index = i;

   //   // If this memory type cannot be used to allocate requested resource, continue search
   //   if (index == VK_MAX_MEMORY_TYPES)
   //      continue;

   //   // Try to allocate memory on Heap supporting this memory type
   //   VkMemoryAllocateInfo allocInfo;
   //   allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   //   allocInfo.pNext           = nullptr;
   //   allocInfo.allocationSize  = requirements.size;
   //   allocInfo.memoryTypeIndex = index;
   //   
   //   // Profile will block here !!! FIXME !!!
   //   Profile( this, vkAllocateMemory(device, &allocInfo, &defaultAllocCallbacks, &handle) )
   //   if (lastResult[Scheduler.core()] == VK_SUCCESS)
   //      return handle;
   //   }

   //// FAIL
   //return handle;
   //}


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
      library(LoadLibrary(L"vulkan-1.dll")),
#endif
#if defined(EN_PLATFORM_LINUX)
      library(dlopen("libvulkan.so", RTLD_NOW)),
#endif
      layer(nullptr),
      layersCount(0),
      globalExtension(nullptr),
      globalExtensionsCount(0),
      devicesCount(0),
      displayArray(nullptr),
      virtualDisplay(nullptr),
      displaysCount(0),
      GraphicAPI() // or SafeObject()
   {
   for(uint32 i=0; i<MaxSupportedWorkerThreads; ++i)
      lastResult[i] = VK_SUCCESS;
      
   // Windows OS - Windowing System Query (API Independent)
   //-------------------------------------------------------

#if defined(EN_PLATFORM_WINDOWS)
   // Display Device settings
   DISPLAY_DEVICE Device;
   memset(&Device, 0, sizeof(Device));
   Device.cb = sizeof(Device);

   // Calculate amount of available displays
   uint32 index = 0;
   while(EnumDisplayDevices(nullptr, index++, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
      if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
         displaysCount++;

   displayArray = new Ptr<CommonDisplay>[displaysCount];
   virtualDisplay = new winDisplay();
  
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
         assert( EnumDisplaySettingsEx(Device.DeviceName, ENUM_CURRENT_SETTINGS, &DispMode, 0u) );
 
         // Verify that proper values were returned by query
         assert( checkBitmask(DispMode.dmFields, (DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT)) );

         desktopPosition.x   = DispMode.dmPosition.x;
         desktopPosition.y   = DispMode.dmPosition.y;
         currentResolution.x = DispMode.dmPelsWidth;
         currentResolution.y = DispMode.dmPelsHeight;
            
         // Calculate amount of available display modes (count only modes supported by display and no rotation ones)
         uint32 modesCount = 0;
         while(EnumDisplaySettingsEx(Device.DeviceName, modesCount, &DispMode, 0u))
            modesCount++;

         Ptr<winDisplay> currentDisplay = new winDisplay();
         
         currentDisplay->modeResolution = new uint32v2[modesCount];
     
         // Gather information about all supported display modes
         uint32 modeId = 0;
         while(EnumDisplaySettingsEx(Device.DeviceName, modeId, &DispMode, 0u))
            {
            // Verify that proper values were returned by query
            assert( checkBitmask(DispMode.dmFields, DM_PELSWIDTH | DM_PELSHEIGHT) );
            
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
        
         currentDisplay->_position          = desktopPosition;
         currentDisplay->_resolution        = nativeResolution;
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
            virtualDisplay->_position   = currentDisplay->_position;
            virtualDisplay->_resolution = currentDisplay->_resolution;
            }
         else
            {
            if (currentDisplay->_position.x < virtualDisplay->_position.x)
               {
               virtualDisplay->_resolution.width += (virtualDisplay->_position.x - currentDisplay->_position.x);
               virtualDisplay->_position.x = currentDisplay->_position.x;
               }
            if (currentDisplay->_position.y < virtualDisplay->_position.y)
               {
               virtualDisplay->_resolution.height += (virtualDisplay->_position.y - currentDisplay->_position.y);
               virtualDisplay->_position.y = currentDisplay->_position.y;
               }
            uint32 virtualRightBorder = virtualDisplay->_position.x + virtualDisplay->_resolution.width;
            uint32 currentRightBorder = currentDisplay->_position.x + currentDisplay->_resolution.width;
            if (virtualRightBorder < currentRightBorder)
               virtualDisplay->_resolution.width = currentRightBorder - virtualDisplay->_position.x;
            uint32 virtualBottomBorder = virtualDisplay->_position.y + virtualDisplay->_resolution.height;
            uint32 currentBottomBorder = currentDisplay->_position.y + currentDisplay->_resolution.height;
            if (virtualBottomBorder < currentBottomBorder)
               virtualDisplay->_resolution.height = currentBottomBorder - virtualDisplay->_position.y;
            }
       
         // Add active display to the list
         displayArray[activeId] = ptr_dynamic_cast<CommonDisplay, winDisplay>(currentDisplay);
         activeId++;
         }
         
      // Clear structure for next display (to ensure there is no old data)
      memset(&Device, 0, sizeof(Device));
      Device.cb = sizeof(Device);
      displayId++;
      }
#endif

   // Verify load of Vulkan dynamic library
   if (library == nullptr)
      {
      string info;
      info += "ERROR: Vulkan error: ";
      info += "       Cannot find Vulkan dynamic library.\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert( 0 );
      }

   // Load using OS, main function pointer used to acquire other Vulkan function pointers
   LoadFunction( vkGetInstanceProcAddr )

   // Load Global Vulkan function pointers (GPU device independent)
   LoadGlobalFunction( vkEnumerateInstanceExtensionProperties )
   LoadGlobalFunction( vkEnumerateInstanceLayerProperties )
   LoadGlobalFunction( vkCreateInstance )
  
   // Gather available Vulkan Extensions
   //------------------------------------

   // Acquire list of Vulkan extensions 
   Profile( this, vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, nullptr) )
   if (IsWarning(lastResult[0]))
      assert( 0 );

   if (globalExtensionsCount > 0)
      {
      globalExtension = new VkExtensionProperties[globalExtensionsCount];
      Profile( this, vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, globalExtension) )
      }
      
   // Gather available Vulkan Instance Layers & Layers Extensions
   //-------------------------------------------------------------

   Profile( this, vkEnumerateInstanceLayerProperties(&layersCount, nullptr) )
   if (IsWarning(lastResult[0]))
      assert( 0 );

   // Acquire list of supported Vulkan Layers
   if (layersCount > 0)
      {
      VkLayerProperties* layerProperties = new VkLayerProperties[layersCount];
      Profile( this, vkEnumerateInstanceLayerProperties(&layersCount, layerProperties) )
      
      layer = new LayerDescriptor[layersCount];

      // Acquire information about each Layer and supported extensions
      for(uint32 i=0; i<layersCount; ++i)
         {
         layer[i].properties = layerProperties[i];

         Profile( this, vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, nullptr) )
         if (IsWarning(lastResult[0]))
            assert( 0 );

         if (layer[i].extensionsCount > 0)
            {
            layer[i].extension = new VkExtensionProperties[layer[i].extensionsCount];
            Profile( this, vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, layer[i].extension) )
            }
         }

      delete [] layerProperties;
      }

   // Specify Vulkan Extensions to initialize
   //-----------------------------------------

   uint32 enabledExtensionsCount = 0;
   char** extensionPtrs = nullptr;
   extensionPtrs = new char*[globalExtensionsCount];
   
   // Adding Windowing System Interface extensions to the list
#if defined(EN_DEBUG)
   extensionPtrs[enabledExtensionsCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME; 
#endif
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
#if defined(EN_PLATFORM_ANDROID)
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_LINUX)
   // TODO: Pick one on Linux (as usual it's complete mess)
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_MIR_SURFACE_EXTENSION_NAME;
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_WINDOWS)
   extensionPtrs[enabledExtensionsCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif

   // TODO: Add here loading list of needed extensions from some config file.
   //       (generated automatically by the engine/editor based on features
   //        used by the app).

   // Verify selected extensions are available
   for(uint32 i=0; i<enabledExtensionsCount; ++i)
      {
      bool found = false;
      for(uint32 j=0; j<globalExtensionsCount; ++j)
         if (strcmp(extensionPtrs[i], globalExtension[j].extensionName) == 0 )
            found = true;
         
      if (!found)
         {
         Log << "ERROR: Requested Vulkan extension " << extensionPtrs[i] << " is not supported on this system!" << endl;
         assert( 0 );
         }
      }
     
   // Specify Vulkan Layers and their Extensions to initialize
   //----------------------------------------------------------

   uint32 enabledLayersCount = 0;
   char** layersPtrs = nullptr;
   
#if defined(EN_DEBUG)
   if (layersCount > 0)
      {
      Log << "Available Vulkan Layers:\n";
      for(uint32 i=0; i<layersCount; ++i)  
         Log << "  " << layer[i].properties.layerName << endl;
      }

   // TODO: Read list of requested layers to enable from config file

   // Cannot enable more layers than all currently available
   layersPtrs = new char*[layersCount];

   // Temporary WA to manually select layers to load
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_api_dump";
   layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_core_validation";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_image";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_object_tracker";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_parameter_validation";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_screenshot";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_swapchain";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_GOOGLE_threading";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_GOOGLE_unique_objects";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_vktrace";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_NV_optimus";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_RENDERDOC_Capture";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_VALVE_steam_overlay";
   //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_standard_validation";


   //// In debug mode enable additional layers for debugging,
   //// profiling and other purposes (currently all available).
   //enabledLayersCount = layersCount;
   //layersPtrs = new char*[layersCount];
   //for(uint32 i=0; i<layersCount; ++i)  
   //   layersPtrs[i] = &layer[i].properties.layerName[0];

   // Debug: Use below to activate all available layers and their extensions:
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
#endif

   // Create Application Vulkan API Instance
   //----------------------------------------

   VkApplicationInfo appInfo;
   appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   appInfo.pNext              = nullptr;
   appInfo.pApplicationName   = appName.c_str();
   appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
   appInfo.pEngineName        = "Ngine";
   appInfo.engineVersion      = VK_MAKE_VERSION(5, 0, 0);
   appInfo.apiVersion         = VK_API_VERSION_1_0;
   
   VkInstanceCreateInfo instInfo;
   instInfo.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   instInfo.pNext                     = nullptr;
   instInfo.flags                     = 0;        // VkInstanceCreateFlags - Reserved.
   instInfo.pApplicationInfo          = &appInfo;
   instInfo.enabledLayerCount         = enabledLayersCount;
   instInfo.ppEnabledLayerNames       = reinterpret_cast<const char*const*>(layersPtrs);
   instInfo.enabledExtensionCount     = enabledExtensionsCount;
   instInfo.ppEnabledExtensionNames   = reinterpret_cast<const char*const*>(extensionPtrs);

   // TODO: In the future provide our own allocation callbacks to track
   //       and analyze drivers system memory usage (&defaultAllocCallbacks).

   Profile( this, vkCreateInstance(&instInfo, nullptr, &instance) )

   // Bind Interface functions
   loadInterfaceFunctionPointers();
   
   delete [] extensionPtrs;
   delete [] layersPtrs;

   // Create Vulkan Devices
   //-----------------------

   // Enumerate available physical devices
   Profile( this, vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr) )
   VkPhysicalDevice* deviceHandle = new VkPhysicalDevice[devicesCount];
   Profile( this, vkEnumeratePhysicalDevices(instance, &devicesCount, deviceHandle) )

   // Create interfaces for all available physical devices
   device = new Ptr<VulkanDevice>[devicesCount];
   for(uint32 i=0; i<devicesCount; ++i)
      device[i] = new VulkanDevice(this, i, deviceHandle[i]);
 
   // Register debug callbacks
   //--------------------------

   // TODO: !!!!

#if defined(EN_DEBUG)

//typedef enum VkDebugReportObjectTypeEXT {
//VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT = 0,
//VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT = 1,
//VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT = 2,
//VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT = 3,
//VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT = 4,
//VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT = 5,
//VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT = 6,
//VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT = 7,
//VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT = 8,
//VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT = 9,
//VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT = 10,
//VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT = 11,
//VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT = 12,
//VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT = 13,
//VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT = 14,
//VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT = 15,
//VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT = 16,
//VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT = 17,
//VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT = 18,
//VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT = 19,
//VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT = 20,
//VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT = 21,
//VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT = 22,
//VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT = 23,
//VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT = 24,
//VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT = 25,
//VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT = 26,
//VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT = 27,
//VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT = 28,
//} VkDebugReportObjectTypeEXT;
//
//typedef VkBool32 (VKAPI_PTR *PFN_vkDebugReportCallbackEXT)(
//VkDebugReportFlagsEXT flags,
//VkDebugReportObjectTypeEXT objectType,
//uint64_t object,
//size_t location,
//int32_t messageCode,
//const char* pLayerPrefix,
//const char* pMessage,
//void* pUserData);


   //// Can register separate callbacks for each report type
   //VkDebugReportCallbackCreateInfoEXT debugInfo;
   //debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
   //debugInfo.pNext = nullptr;
   //debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT   |
   //                  VK_DEBUG_REPORT_WARNING_BIT_EXT |
   //                  VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

//pedef enum VkDebugReportFlagBitsEXT {
//VK_DEBUG_REPORT_INFORMATION_BIT_EXT = 0x00000001,
//VK_DEBUG_REPORT_DEBUG_BIT_EXT =
//PFN_vkDebugReportCallbackEXT pfnCallback;
//void* pUserData;

   //Profile( this, vkCreateDebugReportCallbackEXT(instance, &debugInfo, nullptr, VkDebugReportCallbackEXT* pCallback) )
#endif


   delete [] deviceHandle;
   }

   VulkanAPI::~VulkanAPI()
   {
   // Release Layers and Extensions information
   for(uint32 i=0; i<layersCount; ++i)
      delete [] layer[i].extension;
   delete [] layer;
   delete [] globalExtension;
   
   // Release Vulkan instance
   if (instance != VK_NULL_HANDLE)
      ProfileNoRet( this, vkDestroyInstance(instance, nullptr) )
 
   // Clear Vulkan function pointers
   clearInterfaceFunctionPointers();
   
   // Release Dynamically Linked Vulkan Library
   if (library)
      {
#if defined(EN_PLATFORM_WINDOWS)
      FreeLibrary(library);
#endif
#if defined(EN_PLATFORM_LINUX)
      dlclose(library);
#endif
      }
      
   // Windows OS - Windowing System (API Independent)
   virtualDisplay = nullptr;
   for(uint32 i=0; i<displaysCount; ++i)
      displayArray[i] = nullptr;
   delete [] displayArray;
   }

   void VulkanAPI::loadInterfaceFunctionPointers(void)
   {
   LoadInstanceFunction( vkEnumeratePhysicalDevices )
   LoadInstanceFunction( vkGetPhysicalDeviceFeatures )
   LoadInstanceFunction( vkGetPhysicalDeviceProperties )
   LoadInstanceFunction( vkGetPhysicalDeviceMemoryProperties )
   LoadInstanceFunction( vkGetPhysicalDeviceFormatProperties )
   LoadInstanceFunction( vkGetPhysicalDeviceImageFormatProperties )
   LoadInstanceFunction( vkGetPhysicalDeviceQueueFamilyProperties )
   LoadInstanceFunction( vkEnumerateDeviceExtensionProperties )
   LoadInstanceFunction( vkCreateDevice )
   LoadInstanceFunction( vkGetDeviceProcAddr )
   LoadInstanceFunction( vkDestroyInstance )
   
   // VK_KHR_surface
   LoadInstanceFunction( vkGetPhysicalDeviceSurfaceSupportKHR )
   LoadInstanceFunction( vkGetPhysicalDeviceSurfaceCapabilitiesKHR )
   LoadInstanceFunction( vkGetPhysicalDeviceSurfaceFormatsKHR )
   LoadInstanceFunction( vkGetPhysicalDeviceSurfacePresentModesKHR )
   LoadInstanceFunction( vkDestroySurfaceKHR )

   // VK_KHR_win32_surface
   LoadInstanceFunction( vkCreateWin32SurfaceKHR )
   LoadInstanceFunction( vkGetPhysicalDeviceWin32PresentationSupportKHR )

   // VK_EXT_debug_report
   LoadInstanceFunction( vkCreateDebugReportCallbackEXT )
   LoadInstanceFunction( vkDestroyDebugReportCallbackEXT )
   LoadInstanceFunction( vkDebugReportMessageEXT )
   }

   void VulkanAPI::clearInterfaceFunctionPointers(void)
   {
   // OS Function Pointer
   vkGetInstanceProcAddr                          = nullptr;
   
   // Vulkan Function Pointers
   vkEnumerateInstanceExtensionProperties         = nullptr;
   vkEnumerateInstanceLayerProperties             = nullptr;
   vkCreateInstance                               = nullptr;
   
   // Vulkan Instance Function Pointers
   vkEnumeratePhysicalDevices                     = nullptr;
   vkGetPhysicalDeviceFeatures                    = nullptr;
   vkGetPhysicalDeviceProperties                  = nullptr;
   vkGetPhysicalDeviceMemoryProperties            = nullptr;
   vkGetPhysicalDeviceFormatProperties            = nullptr;
   vkGetPhysicalDeviceImageFormatProperties       = nullptr;
   vkGetPhysicalDeviceQueueFamilyProperties       = nullptr;
   vkEnumerateDeviceExtensionProperties           = nullptr;
   vkCreateDevice                                 = nullptr;
   vkGetDeviceProcAddr                            = nullptr;
   vkDestroyInstance                              = nullptr;
   vkGetPhysicalDeviceSurfaceSupportKHR           = nullptr;
   }
   
   uint32 VulkanAPI::devices(void) const
   {
   return devicesCount;
   }
         
   Ptr<GpuDevice> VulkanAPI::primaryDevice(void) const
   {
   return ptr_reinterpret_cast<GpuDevice>(&device[0]);
   }

   uint32 VulkanAPI::displays(void) const
   {
   return displaysCount;
   }
   
   Ptr<Display> VulkanAPI::primaryDisplay(void) const
   {
   return ptr_reinterpret_cast<Display>(&displayArray[0]);
   }
   
   Ptr<Display> VulkanAPI::display(uint32 index) const
   {
   assert( index < displaysCount );
   return ptr_reinterpret_cast<Display>(&displayArray[index]);   
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

//   // Construct the WSI surface description:
//   VkSurfaceDescriptionWindowKHR surfaceInfo;

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
