/*

 Ngine v5.0
 
 Module      : D3D12 Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Window.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   { 


   Ptr<Window> Direct3D12Device::createWindow(const WindowSettings& settings, const string title)
   {
   // TODO: !!!

   return Ptr<Window>(nullptr);
   }

   }
}
#endif