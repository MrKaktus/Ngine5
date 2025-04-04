/*

 Ngine v5.0
 
 Module      : Metal API Interface.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_API
#define ENG_CORE_RENDERING_METAL_API

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include <string>
#include "core/rendering/common/device.h"
#include "core/rendering/common/display.h"

namespace en
{
namespace gpu
{

// This API need to use generic defines, as this header is shared with CPP code
class MetalAPI : public GraphicAPI
{
    public:
    std::shared_ptr<GpuDevice> _device[8];  // Primary and Supporting GPU (eGPU's as well)
    uint32         devicesCount;
    bool           preferLowPowerGPU; // If set and two GPU's are available, low-power GPU will be choosed over discreete one
    std::shared_ptr<CommonDisplay>* _display;
    std::shared_ptr<CommonDisplay>  virtualDisplay;
    uint32         displaysCount;
  

    MetalAPI();

    virtual RenderingAPI type(void) const;

    virtual uint32                     devices(void) const;
    virtual std::shared_ptr<GpuDevice> primaryDevice(void) const;
    virtual std::shared_ptr<GpuDevice> device(const uint32 index) const;
      
    virtual uint32                     displays(void) const;       // Screens count the device can render to
    virtual std::shared_ptr<Display>   primaryDisplay(void) const;
    virtual std::shared_ptr<Display>   display(const uint32 index) const;   // Return N'th screen handle
      
    virtual ~MetalAPI();
};

} // en::gpu
} // en

#endif
#endif
