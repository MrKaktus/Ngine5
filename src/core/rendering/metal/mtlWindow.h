/*

 Ngine v5.0
 
 Module      : Metal Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_WINDOW
#define ENG_CORE_RENDERING_METAL_WINDOW

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/common/window.h"
#include "core/rendering/metal/mtlTexture.h"

namespace en
{
namespace gpu
{

class MetalDevice;

class WindowMTL : public CommonWindow
{
    public:
    NSWindow*     window;
    NSView*       view;
    CAMetalLayer* layer;
    id <CAMetalDrawable> drawable;
    TextureMTL*   framebuffer;
    
    virtual bool movable(void);
    virtual void move(const uint32v2 position);
    virtual void resize(const uint32v2 size);
    virtual void active(void);
    virtual void transparent(const float opacity);
    virtual void opaque(void);
    virtual Texture* surface(const Semaphore* signalSemaphore = nullptr);
    virtual void present(const Semaphore* waitForSemaphore = nullptr);
    
    WindowMTL(const MetalDevice* gpu, const WindowSettings& settings, const std::string title); //id<MTLDevice> device
    virtual ~WindowMTL();
};

} // en::gpu
} // en
#endif

#endif
