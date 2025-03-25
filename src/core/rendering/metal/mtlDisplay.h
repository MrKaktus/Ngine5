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
    NSScreen*        handle;           // Pointer to screen in [NSScreen screens] array
    CVDisplayLinkRef displayLink;      // Custom VSync tracking through DisplayLink
    Time             nextVSyncTime[2]; // Predicted time when next two VSync's will happen.
                                       // It's possible that one of them will point to a past
                                       // time from now, in such situation second prediction
                                       // will store next Vsync time.
    
    DisplayMTL(NSScreen* handle);
   ~DisplayMTL();
};

} // en::gpu
} // en
#endif

#endif
