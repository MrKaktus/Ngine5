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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include <string>
#include "core/rendering/common/device.h"

using namespace std;

namespace en
{
   namespace gpu
   {
   class MetalAPI : public GraphicAPI
      {
      public:
      Ptr<GpuDevice> device[2];  // Primary and Supporting GPU
      uint32         devicesCount;
      bool           preferLowPowerGPU; // If set and two GPU's are available, low-power GPU will be choosed over discreete one
      Ptr<Screen>*   display;
      Ptr<Screen>    virtualDisplay;
      uint32         displaysCount;
  

      MetalAPI();

      virtual uint32 devices(void);
      virtual Ptr<GpuDevice> primaryDevice(void);

      virtual ~MetalAPI();
      };
   }
}
#endif

#endif
