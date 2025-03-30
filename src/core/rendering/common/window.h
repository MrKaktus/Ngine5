/*

 Ngine v5.0
 
 Module      : Common GPU Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_WINDOW
#define ENG_CORE_RENDERING_COMMON_WINDOW

#include "core/rendering/window.h"
#include "core/rendering/common/display.h"
#include "core/parallel/mutex.h"

namespace en
{
namespace gpu
{

class CommonWindow : public Window
{
    public:
    Mutex    surfaceAcquire;   ///< Window instance mutex.
    std::shared_ptr<CommonDisplay> _display;
    uint32v2 _position;
    uint32v2 _size;
    uint32v2 _resolution;
    WindowMode _mode;          ///< Windowed / Borderless / Fullscreen
    uint32   _frame;           ///< Frame counter, increased after each present
    bool     needNewSurface;
    bool     verticalSync;
    
    CommonWindow();
    
    virtual std::shared_ptr<Display> display(void) const;   ///< Display on which window's center point is currently located
    virtual uint32v2 position(void) const;
    virtual uint32v2 size(void) const;
    virtual uint32v2 resolution(void) const;
    virtual uint32   frame(void) const;
    
    virtual void transparent(const float opacity); // TODO: Do we really want that here? (Transp. should be enabled on window creation time, and queried later)
    virtual void opaque(void);
    
    virtual ~CommonWindow();
};

} // en::Gpu
} // en
#endif
