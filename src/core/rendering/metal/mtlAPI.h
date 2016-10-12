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

using namespace std;

namespace en
{
   namespace gpu
   {
   // This API need to use generic defines, as this header is shared with CPP code
   class MetalAPI : public GraphicAPI
      {
      public:
      Ptr<GpuDevice> device[2];  // Primary and Supporting GPU
      uint32         devicesCount;
      bool           preferLowPowerGPU; // If set and two GPU's are available, low-power GPU will be choosed over discreete one
      Ptr<CommonDisplay>* _display;
      Ptr<CommonDisplay>  virtualDisplay;
      uint32         displaysCount;
  

      MetalAPI();

      virtual uint32         devices(void) const;
      virtual Ptr<GpuDevice> primaryDevice(void) const;

      virtual uint32         displays(void) const;       // Screens count the device can render to
      virtual Ptr<Display>   primaryDisplay(void) const;
      virtual Ptr<Display>   display(uint32 index) const;   // Return N'th screen handle
      
      virtual ~MetalAPI();
      };
   }
}
#endif

#endif
