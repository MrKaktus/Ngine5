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
#include <sys/kdebug_signpost.h>

#include "core/memory/alignedAllocator.h"

// In plist, kTraceCode is calculated as value << 2 + 0x210A0000
#define EN_SIGNPOST_DISPLAY_LINK 0x00000010

//kdebug_signpost_start(SignPostCode.download.rawValue, UInt(index), 0, 0, SignPostColor.orange.rawValue)
//// perform download
//kdebug_signpost_end(SignPostCode.download.rawValue, UInt(index), 0, 0, SignPostColor.orange.rawValue)
//To mark a single moment in time, we can just use kdebug_signpost:

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
   // Mark the moment when Display Link callback is fired, to relate it to VSync
   kdebug_signpost(EN_SIGNPOST_DISPLAY_LINK, 0, 0, 0, 0);

   // System (mach) times of this callback and next predicted VSync.
   // Predicted VSync may be reported for 2 VSync's in advance (so
   // that it's not the next one that will happen but one after that).
   uint64_t machCallbackTime  = inNow->hostTime;
   uint64_t machNextVSyncTime = inOutputTime->hostTime;

   mach_timebase_info_data_t timebase = { 0, 0 };
   mach_timebase_info(&timebase);

   // Establish destination display, this predicted time will be saved to
   DisplayMTL* display = reinterpret_cast<DisplayMTL*>(displayLinkContext);
   
   // System time is in nanoseconds granularity
   Time callbackTime;
   Time nextVSyncTime;
   callbackTime.nanoseconds( machCallbackTime * timebase.numer / timebase.denom );
   nextVSyncTime.nanoseconds( machNextVSyncTime * timebase.numer / timebase.denom );

   // Cache of two VSync times
   display->nextVSyncTime[0] = display->nextVSyncTime[1];
   display->nextVSyncTime[1] = nextVSyncTime;

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
