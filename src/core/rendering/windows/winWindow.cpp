/*

 Ngine v5.0
 
 Module      : Windows Windowing System.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)
// Need to compile with Windows 10 SDK to prevent:
// c:\program files (x86)\windows kits\10\include\10.0.10240.0\um\ole2.h(39): fatal error C1083: Cannot open include file: 'coml2api.h': No such file or directory
// See more at:
// https://naughter.wordpress.com/2015/05/24/changes-in-the-windows-10-sdk-compared-to-windows-8-1-part-two/
//
// It needs to be included before winWindow.h, as TIntrusivePointer is including atomics.inl which uses windows.h
#include <windows.h>
#endif

#include "core/rendering/windows/winWindow.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "platform/windows/win_events.h"
#include "core/log/log.h"
#include "parallel/scheduler.h"

namespace en
{
   namespace gpu
   {
   HINSTANCE winWindow::AppInstance = nullptr;  // Application handle (helper handle)
   static uint32    windows = 0;      // Count of currently active windows

   winWindow::winWindow(const std::shared_ptr<winDisplay> selectedDisplay,
                        const uint32v2 selectedResolution,
                        const WindowSettings& settings,
                        const std::string title) :
      hWnd(nullptr),
      CommonWindow()
   {
   DWORD     Style;       // Window style
   DWORD     ExStyle;     // Window extended style
   RECT      WindowRect;  // Window rectangle
      
   // First time, init shared helper resources
   if (windows == 0)
      {
      // Get this application instance
      AppInstance          = (HINSTANCE)GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE);  // GWL_HINSTANCE is deprecated in x64 environment
        
      // Window settings
      WNDCLASS WindowClass;
      WindowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Have its own Device Context and also cannot be resized. Oculus uses CLASSDC which is not thread safe!    
      WindowClass.lpfnWndProc   = (WNDPROC) WinEvents;                // Procedure that handles OS evets for this window
      WindowClass.cbClsExtra    = 0;                                  // No extra window data
      WindowClass.cbWndExtra    = 0;                                  //
      WindowClass.hInstance     = AppInstance;                        // Handle to instance of program that this window belongs to
      WindowClass.hIcon         = LoadIcon(NULL, IDI_WINLOGO);        // Load default icon
      WindowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);        // Load arrow pointer
      WindowClass.hbrBackground = NULL;                               // No background (Direct3D/OpenGL/Vulkan will handle this)
      WindowClass.lpszMenuName  = NULL;                               // No menu
      WindowClass.lpszClassName = L"WindowClass";                     // Class name
      
      // Registering Window Class
      if (!RegisterClass(&WindowClass))	
         {					
         Log << "Error! Cannot register window class.\n";
         assert( 0 );
         }
      }

   // Window size      
   WindowRect.left      = (long)0;      
   WindowRect.right     = (long)selectedResolution.width;
   WindowRect.top       = (long)0;    
   WindowRect.bottom    = (long)selectedResolution.height;

   // Window position
   sint16v2 windowPosition;
   windowPosition.x = settings.position.x;
   windowPosition.y = settings.position.y;

   // Preparing for displays native fullscreen mode
   if (settings.mode == Fullscreen)
      {
      windowPosition = sint16v2(0, 0);

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
         Log << "Error! Cannot change display settings for fullscreen.\n";
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

         // Subtract border and bar sizes
         windowPosition.x += WindowRect.left;
         windowPosition.y += WindowRect.top;
         }
      else // Borderless window
         {
         Style   = WS_POPUP;                               // Window style                // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
         ExStyle = 0;                                      // Window extended style  
         }
      }

   // Creating Window in modern way
   std::wstring windowTitle = std::wstring(title.begin(), title.end());
   hWnd = CreateWindowEx(
      ExStyle,                             // Extended style 
      L"WindowClass",                      // Window class name
      (LPCWSTR)windowTitle.c_str(),        // Title of window            
      Style,                               // Additional styles
      windowPosition.x, windowPosition.y,  // Position in screen coordinates    <-- TODO: Shouldn't be display.pos + settings.pos? Is this on Desktop or on Window ?
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
   
   _display     = selectedDisplay;
   _position    = settings.position;
   _size        = selectedResolution;
 //_resolution will be set by child class implementing given Graphics API Swap-Chain
   _mode        = settings.mode;
   verticalSync = settings.verticalSync;

   windows++;
   }

   winWindow::~winWindow()
   {
   // Reset to old display settings
   if (_mode == WindowMode::Fullscreen)
      {
      ChangeDisplaySettings(nullptr, 0);
      ShowCursor(TRUE);
      }

   // Delete window
   DestroyWindow(hWnd);

   // Unregister window class
   if (windows == 1)
      {
      UnregisterClass(L"WindowClass", AppInstance);
      }

   windows--;
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
   
   // TODO: This method should be called by specialization class implementation that will change Swap-Chain !
   void winWindow::resize(const uint32v2 size)
   {
   if (_mode != WindowMode::Windowed)
      return;
     
   DWORD Style;       // Window style
   DWORD ExStyle;     // Window extended style
   RECT  WindowRect;  // Window rectangle
 
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
   
void taskMakeWindowActive(void* data)
{
    HWND& hWnd = *(HWND*)(data);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    SetActiveWindow(hWnd);
}

void winWindow::active(void)
{
    TaskState taskState;

    // Window needs to be activated on main thread
    en::Scheduler->runOnMainThread(taskMakeWindowActive, (void*)&hWnd, &taskState, true);
    en::Scheduler->wait(&taskState);
}

   void winWindow::transparent(const float opacity)
   {
   // TODO: Finish !
   }
   
   void winWindow::opaque(void)
   {
   // TODO: Finish !
   }

   }
}
#endif
