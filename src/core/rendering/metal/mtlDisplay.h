/*

 Ngine v5.0
 
 Module      : Metal Display.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_DISPLAY
#define ENG_CORE_RENDERING_METAL_DISPLAY

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#import <AppKit/AppKit.h>
#include "core/rendering/common/display.h"
#include "utilities/timer.h"

namespace en
{
   namespace gpu
   {
   class DisplayMTL : public CommonDisplay
      {
      public:
      NSScreen*        handle;        // Pointer to screen in [NSScreen screens] array
      CVDisplayLinkRef displayLink;   // Custom VSync tracking through DisplayLink
      Time             callbackTime;  // Time last callback occured
      Time             nextVSyncTime; // Predicted time next VSync will happen.
                                      // It's possible that it will point to a past time from
                                      // now, in such situation it means that VSync already
                                      // happened, but prediction for the next one didn't yet.

      DisplayMTL(NSScreen* handle);
     ~DisplayMTL();
      };
   }
}
#endif

#endif
