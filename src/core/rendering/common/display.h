/*

 Ngine v5.0
 
 Module      : Common GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_DISPLAY
#define ENG_CORE_RENDERING_COMMON_DISPLAY

#include "core/rendering/display.h"

namespace en
{
namespace gpu
{

class CommonDisplay : public Display
{
    public:
    uint32v2  _position;          // Upper-Left corner position on virtual desktop
    uint32v2  _resolution;        // Native resolution
    uint32v2  observedResolution; // Display resolution when app started
    uint32v2* modeResolution;     // Resolutions of display modes supported by this display
    uint32    modesCount;         // Count of display modes supported by this display (from the list of modes supported by the driver)
    uint32    frequency;          // Display refresh rate in Hz
      
    CommonDisplay();

    virtual uint32v2 position(void);    // Position on Virtual Desktop
    virtual uint32v2 resolution(void);  // Native resolution
    virtual uint32   refreshRate(void); // Display refresh rate
    
    virtual ~CommonDisplay();
};

} // en::gpu
} // en
#endif
