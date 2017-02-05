/*

 Ngine v5.0
 
 Module      : Windows Display.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/windows/winDisplay.h"

#if defined(EN_PLATFORM_WINDOWS)
namespace en
{
   namespace gpu
   {
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
 
   }
}
#endif