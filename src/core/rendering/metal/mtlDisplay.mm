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

#include "core/rendering/metal/mtlDisplay.h"

#if defined(EN_MODULE_RENDERER_METAL)

#if defined(EN_PLATFORM_OSX)
#ifdef aligned
#undef aligned
#endif
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#include "core/utilities/memory.h"

namespace en
{
   namespace gpu
   {
   // Function called when CoreVideo DisplayLink is fired
   CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                const CVTimeStamp *inNow,
                                const CVTimeStamp *inOutputTime,
                                CVOptionFlags flagsIn,
                                CVOptionFlags *flagsOut,
                                void *displayLinkContext)
   {
   // System (mach) times of this callback and next V-Sync
   uint64_t machCallbackTime  = inNow->hostTime;
   uint64_t machNextVSyncTime = inOutputTime->hostTime;

   mach_timebase_info_data_t timebase = { 0, 0 };
   mach_timebase_info(&timebase);

   // Establish destination display, this predicted time will be saved to
   DisplayMTL* display = reinterpret_cast<DisplayMTL*>(displayLinkContext);
   
   // System time is in nanoseconds granularity
   display->callbackTime.nanoseconds( machCallbackTime * timebase.numer / timebase.denom );
   display->nextVSyncTime.nanoseconds( machNextVSyncTime * timebase.numer / timebase.denom );

   // TODO: Registered VSync time, should be used by async threads
   //       to calculate how many frames ago that thread started it's
   //       execution, from next closest VSync that will happen.
   return kCVReturnSuccess;
   }

   DisplayMTL::DisplayMTL(NSScreen* _handle) :
      handle(_handle),
      CommonDisplay()
   {
   assert( handle );
   
   // Acquire display ID from NSScreen
   NSDictionary* screenDictionary = [handle deviceDescription];
   NSNumber* screenID = [screenDictionary objectForKey:@"NSScreenNumber"];
   assert( screenID );
   CGDirectDisplayID displayID = [screenID unsignedIntValue];

   // Alternative for primary display:
   // CGDirectDisplayID displayID = CGMainDisplayID();

   // Query display Refresh Rate
#if defined(EN_PLATFORM_OSX)
   CGDisplayModeRef mode = CGDisplayCopyDisplayMode(displayID);
   frequency = uint32(CGDisplayModeGetRefreshRate(mode));
#endif
#if defined(EN_PLATFORM_IOS)
   frequency = [UIScreen maximumFramesPerSecond];
#endif

   // Configure Display Link
   CVReturn result = CVDisplayLinkCreateWithCGDisplay(displayID, &displayLink);
   assert( result == kCVReturnSuccess );

   // Alternative is to have single callback for all displays,
   // but this may be less useful:
   // CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
   
   // Register callback function to be executed whenever
   // next VSync prediction for this display is ready
   result = CVDisplayLinkSetOutputCallback(displayLink,
                                           displayLinkCallback,
                                           this); // Pointer to data passed on each callback.

   assert( result == kCVReturnSuccess );
   
   // Start DisplayLink
   result = CVDisplayLinkStart(displayLink);
   assert( result == kCVReturnSuccess );
   }
   
   DisplayMTL::~DisplayMTL()
   {
   // Stop DisplayLink
   CVReturn result = CVDisplayLinkStop(displayLink);
   assert( result == kCVReturnSuccess );
   assert( CVDisplayLinkIsRunning(displayLink) == false );

   // Destroy CoreVideo DisplayLink
   CVDisplayLinkRelease(displayLink);
   displayLink = nullptr;
   
   handle = nullptr;
   }
   
   }
}
#endif
