/*

 Ngine v5.0
 
 Module      : Display.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_DISPLAY
#define ENG_CORE_RENDERING_DISPLAY

#include "core/types.h"

namespace en
{
namespace gpu
{
class Display
{
    public:
    virtual uint32v2 position(void) = 0;    ///< Position on Virtual Desktop
    virtual uint32v2 resolution(void) = 0;  ///< Native resolution
    virtual uint32   refreshRate(void) = 0; ///< Display refresh rate
    
    virtual ~Display() {};
};

} // en::gpu
} // en

#endif
