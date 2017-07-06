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

#ifndef ENG_CORE_RENDERING_WINDOWS_WINDOW
#define ENG_CORE_RENDERING_WINDOWS_WINDOW

#include "core/rendering/common/window.h"
#include "core/rendering/windows/winDisplay.h"

#include <string>
using namespace std;

namespace en
{
   namespace gpu
   {
   class winWindow : public CommonWindow
      {
      public:
      static HINSTANCE AppInstance; // Application handle (helper handle)
      HWND hWnd;                    // Window handle

      winWindow(const Ptr<winDisplay> selectedDisplay,
                const uint32v2 selectedResolution,
                const WindowSettings& settings,
                const string title);

      virtual bool movable(void);
      virtual void move(const uint32v2 position);
      virtual void resize(const uint32v2 size);
      virtual void active(void);
      virtual void transparent(const float opacity);
      virtual void opaque(void);

      virtual ~winWindow();
      };
   }
}
#endif