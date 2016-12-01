/*

 Ngine v5.0
 
 Module      : Direct3D12 GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Device.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/log/log.h"
#include "core/utilities/memory.h"
#include "core/rendering/d3d12/dx12Texture.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "platform/windows/win_events.h"
#endif

namespace en
{
   namespace gpu
   {

   Direct3D12Device::Direct3D12Device()
   {
   // TODO: Everything else . . . .
   
   
   // RENDER PASS
   
   // D3D12 has no notion of RenderPass. Instead, it treats Color Attachments and
   // Depth/Stencil attachments, as any other resources. It requires their binding
   // through resource views and descriptors from Descriptor Heaps (RTV's descriptors
   // are alocated from separate Heap than DSV descriptors).
   //
   // Engine using exclusively D3D12 for rendering, would create bigger Heaps for
   // RTV and DSV descriptors, avoiding resources rebinding. As this rendering
   // abstraction layer covers at the same time Direct3D12, Vulkan and Metal, it
   // needs to emulate RenderPass abstraction on top of Direct3D12 Descriptor Heaps.
   //
   // Currently it's done by simply allocating small Descriptor Heaps matching
   // maximum amount of RTV's and DSV's. Thus each change of RenderPass requires
   // rebinding of destination resources views to those Descriptors. It's not
   // perfect solution, but it shouldn't impart performance as RenderPass changes
   // are rare during the frame.
   //
   // Alternative solution would be to allocate bigger Heaps, and cache RTV and DSV
   // descriptors for created Framebuffer objects.
   //
   // In the future, RenderPass API will support building of RenderGraph consisting
   // several RenderPasses. This will allow allocation of bigger Descriptor Heaps
   // and will further reduce consistency of descriptor binds.

   D3D12_DESCRIPTOR_HEAP_DESC desc;
   desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
   desc.NumDescriptors = 8;
   desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
   desc.NodeMask       = 0u;                              // TODO: Set bit to current GPU index

   // Allocate global Color Attachment descriptors heap
   Profile( this, CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), IID_PPV_ARGS(&heapRTV)) )

   desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
   desc.NumDescriptors = 1;
   
   // Allocate global Depth-Stencil Attachment descriptor heap
   Profile( this, CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), IID_PPV_ARGS(&heapDSV)) )


   // TODO: Everything else . . . .

   }

   Direct3D12Device::~Direct3D12Device()
   {
   // TODO: Everything else . . . .
   }

   Ptr<Texture> Direct3D12Device::createSharedTexture(Ptr<SharedSurface> backingSurface)
   {
   // Engine is not supporting cross-API / cross-process surfaces in Direct3D12 currently.
   // Implement it in the future.
   assert( 0 );
   return Ptr<Texture>(nullptr);
   }
   
   }
}
#endif
