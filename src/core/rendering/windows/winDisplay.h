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

#ifndef ENG_CORE_RENDERING_WINDOWS_DISPLAY
#define ENG_CORE_RENDERING_WINDOWS_DISPLAY

#include "core/rendering/common/display.h"

namespace en
{
namespace gpu
{

class winDisplay : public CommonDisplay
{
    public:
    uint32    index;              // Index of this display on Windows displays list
    bool      resolutionChanged;  // Flag if app changed display resolution (allows restoration of original resolution on exit)
   
    winDisplay();
   ~winDisplay();
};

} // en::gpu
} // en

#endif
