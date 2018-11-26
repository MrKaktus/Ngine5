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

#ifndef ENG_CORE_RENDERING_D3D12_WINDOW
#define ENG_CORE_RENDERING_D3D12_WINDOW

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/windows/winWindow.h"

namespace en
{
   namespace gpu
   {
   class Direct3D12Device;
   class Texture;

   class WindowD3D12 : public winWindow
      {
      public:
      Direct3D12Device* gpu;
      IDXGISwapChain1*  swapChain;
      IDXGISwapChain4*  swapChain4;

      std::shared_ptr<Texture>* swapChainTexture;
      uint32         swapChainImages;
      uint32         swapChainCurrentImageIndex;

      WindowD3D12(Direct3D12Device* gpu,
                  const std::shared_ptr<CommonDisplay> selectedDisplay,
                  const uint32v2 selectedResolution,
                  const WindowSettings& settings,
                  const std::string title);

      virtual void resize(const uint32v2 size);
      virtual std::shared_ptr<Texture> surface(const Semaphore* signalSemaphore = nullptr);
      virtual void present(const Semaphore* waitForSemaphore = nullptr);
      
      virtual ~WindowD3D12();
      };
   }
}

#endif

#endif
