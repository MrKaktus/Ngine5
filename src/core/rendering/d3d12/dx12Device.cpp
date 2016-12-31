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
#include "threading/scheduler.h"

#include "platform/windows/win_events.h"

namespace en
{
   namespace gpu
   {
   // Checks Vulkan error state
   bool IsError(const HRESULT result)
   {
   if (result == NOERROR)
      return false;
   
   // Compose error message
   string info;
   info += "ERROR: Direct3D12 error: ";
   if (result == D3D11_ERROR_FILE_NOT_FOUND)  // D3D12_ERROR_FILE_NOT_FOUND undeclared ?
      {
      info += "D3D12_ERROR_FILE_NOT_FOUND.\n";
      info += "       The file was not found.\n";
      }
   else
   if (result == D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS) // D3D12_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS undeclared ?
      {
      info += "D3D12_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS.\n";
      info += "       There are too many unique instances of a particular type of state object.\n";
      }
   else
   if (result == D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS) // D3D12_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS undeclared ?
      {
      info += "D3D12_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS.\n";
      info += "       There are too many unique instances of a particular type of view object.\n";
      }
   else
   if (result == DXGI_ERROR_INVALID_CALL)
      {
      info += "DXGI_ERROR_INVALID_CALL (replaced D3DERR_INVALIDCALL).\n";
      info += "       The method call is invalid. For example, a method's parameter may not be a valid pointer.\n";
      }
   else
   if (result == DXGI_ERROR_WAS_STILL_DRAWING)
      {
      info += "DXGI_ERROR_WAS_STILL_DRAWING (replaced D3DERR_WASSTILLDRAWING).\n";
      info += "       The previous blit operation that is transferring information to or from this surface is incomplete.\n";
      }
   else
   if (result == E_FAIL)
      {
      info += "E_FAIL.\n";
      info += "       Attempted to create a device with the debug layer enabled and the layer is not installed.\n";
      }
   else
   if (result == E_INVALIDARG)
      {
      info += "E_INVALIDARG.\n";
      info += "       An invalid parameter was passed to the returning function.\n";
      }
   else
   if (result == E_OUTOFMEMORY)
      {
      info += "E_OUTOFMEMORY.\n";
      info += "       Direct3D could not allocate sufficient memory to complete the call.\n";
      }
   else
   if (result == E_NOTIMPL)
      {
      info += "E_NOTIMPL.\n";
      info += "       The method call isn't implemented with the passed parameter combination.\n";
      }
   else
   if (result == S_FALSE)
      {
      info += "S_FALSE.\n";
      info += "       Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context).\n";
      }

   Log << info.c_str();
   return true; 
   }
   
   // Checks Vulkan warning state
   bool IsWarning(const HRESULT result)
   {
   // Direct3D12 is not providing Warning codes.
   return false;
   }

   Direct3D12Device::Direct3D12Device()
   {
   // TODO: Everything else . . . .



   // All threads starts with Fence values == 0, first Fences will have ID 1
   memset(&fenceCurrentValue[0], 0, sizeof(fenceCurrentValue));

   // QUEUES
   
   // TODO: Is there a way to query amount of available queues of each type?
   uint32 types = underlyingType(QueueType::Count);
   for(uint32 type=0; type<types; ++type)
      {
      D3D12_COMMAND_QUEUE_DESC desc;
      desc.Type     = TranslateQueueType[type];
      desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // In [0..100] range 100 == D3D12_COMMAND_QUEUE_PRIORITY_HIGH
      desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;       // Can use D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
      desc.NodeMask = 0u;                                  // No Multi-GPU support.
     
      Profile( this, CreateCommandQueue(&desc, IID_PPV_ARGS(&queue[type])) )

      queuesCount[type] = 1;


      // TODO: Each thread should create it's own CommandAllocators
      uint32 thread = 0;
      
      // " A given allocator can be associated with no more than one
      //   currently recording command list at a time, . . . "
      //
      Profile( this, CreateCommandAllocator(TranslateQueueType[type],
                                            IID_PPV_ARGS(&commandAllocator[thread][type])) )
      }


   
   
   // RENDER PASS
   // ===========
   //
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
   Profile( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapRTV)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&heapRTV)

   desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
   desc.NumDescriptors = 1;
   
   // Allocate global Depth-Stencil Attachment descriptor heap
   Profile( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapDSV)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&heapDSV)

   // Acquire CPU side handles to all slots in RTV and DSV heaps
   D3D12_CPU_DESCRIPTOR_HANDLE firstHandleRTV = heapRTV->GetCPUDescriptorHandleForHeapStart();
   uint32 rtvDescriptorSize  = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
   for(uint32 i=0; i<8; ++i)
      {
      handleRTV[i] = firstHandleRTV;

      firstHandleRTV.ptr += rtvDescriptorSize;
      }

   handleDSV = heapDSV->GetCPUDescriptorHandleForHeapStart();


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
