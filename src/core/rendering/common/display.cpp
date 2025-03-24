/*
 
 Ngine v5.0
 
 Module      : Common Display.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#include "core/rendering/common/display.h"

namespace en
{
namespace gpu
{

//Display::Display() :
//    SafeObject()
//{
//}
   
CommonDisplay::CommonDisplay() :
    _position(0, 0),
    _resolution(0, 0),
    observedResolution(),
    modeResolution(nullptr),
    modesCount(0),
    frequency(0)
{
}

CommonDisplay::~CommonDisplay()
{
    delete [] modeResolution;
}

uint32v2 CommonDisplay::position(void)
{
    return _position;
}

uint32v2 CommonDisplay::resolution(void)
{
    return _resolution;
}

uint32 CommonDisplay::refreshRate(void)
{
    return frequency;
}
   
} // en::gpu
} // en
