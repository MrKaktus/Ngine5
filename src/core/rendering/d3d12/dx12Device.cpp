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

   Direct3D12Device::Direct3D12Device(Direct3DAPI* _api, const uint32 _index, IDXGIAdapter1* _adapter) :
      api(_api),
      index(_index),
      adapter(_adapter),
      device(nullptr)
   {
   for(uint32 i=0; i<MaxSupportedWorkerThreads; ++i)
      lastResult[i] = 0;

   // Way to find HW Adapter from which given D3D12 Device was created (DXGI 1.4+):
   // LUID luid = ID3D12Device::GetAdapterLuid();
   // IDXGIAdapter* adapter = nullptr;
   // HRESULT IDXGIFactory4::EnumAdapterByLuid(luid, IID_PPV_ARGS(adapter)); // __uuidof(IDXGIAdapter), reinterpret_cast<void**>(&adapter)

   // Try to create device with 12.1 feature level, if not supported fallback to 12.0
   if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
      ProfileCom( D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)) ) // __uuidof(ID3D12Device), reinterpret_cast<void**>(&device)
   else
      ProfileCom( D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)) ) // __uuidof(ID3D12Device), reinterpret_cast<void**>(&device)

   // All threads starts with Fence values == 0, first Fences will have ID 1
   memset(&fenceCurrentValue[0], 0, sizeof(fenceCurrentValue));

   // COMMAND QUEUES
   // ==============

   // TODO: Is there a way to query amount of available queues of each type?
   uint32 types = underlyingType(QueueType::Count);
   for(uint32 type=0; type<types; ++type)
      {
      D3D12_COMMAND_QUEUE_DESC desc;
      desc.Type     = TranslateQueueType[type];
      desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // In [0..100] range 100 == D3D12_COMMAND_QUEUE_PRIORITY_HIGH
      desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;       // Can use D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
      desc.NodeMask = 0u;                                  // No Multi-GPU support.
     
      Profile( this, CreateCommandQueue(&desc, IID_PPV_ARGS(&queue[type])) ) // __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&queue[type])

      queuesCount[type] = 1;


      // TODO: Each thread should create it's own CommandAllocators
      uint32 thread = 0;
      
      // " A given allocator can be associated with no more than one
      //   currently recording command list at a time, . . . "
      //
      Profile( this, CreateCommandAllocator(TranslateQueueType[type],
                                            IID_PPV_ARGS(&commandAllocator[thread][type])) ) // __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&commandAllocator[thread][type])
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
   // perfect solution, but it shouldn't impact performance as RenderPass changes
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


   init();
   }

   Direct3D12Device::~Direct3D12Device()
   {
   // TODO: Everything else . . . .

   // Destroy Command Queues and Allocators
   for(uint32 type=0; type<underlyingType(QueueType::Count); ++type)
      {
      // TODO: Each thread should destroy it's own CommandAllocators
      uint32 thread = 0;

      commandAllocator[thread][type]->Release();
      commandAllocator[thread][type] = nullptr;

      queue[type]->Release();
      queue[type] = nullptr;
      }

   // Destroy device
   device->Release();
   device = nullptr;

   // Destroy adapter
   adapter->Release();
   adapter = nullptr;
   }

   void Direct3D12Device::init()
   {
   // TODO: Populate API capabilities

   CommonDevice::init();
   }

   uint32 Direct3D12Device::displays(void) const
   {
   // TODO: Currently all Direct3D12 devices share all available displays
   return api->displaysCount;
   }
   
   Ptr<Display> Direct3D12Device::display(uint32 index) const
   {
   // TODO: Currently all Direct3D12 devices share all available displays
   assert( api->displaysCount > index );
   
   return ptr_reinterpret_cast<Display>(&api->displayArray[index]);
   }

   uint32 Direct3D12Device::queues(const QueueType type) const
   {
   return queuesCount[underlyingType(type)];
   }

   Ptr<Texture> Direct3D12Device::createSharedTexture(Ptr<SharedSurface> backingSurface)
   {
   // Engine is not supporting cross-API / cross-process surfaces in Direct3D12 currently.
   // Implement it in the future.
   assert( 0 );
   return Ptr<Texture>(nullptr);
   }
   



   // DIRECT3D API 
   //////////////////////////////////////////////////////////////////////////


   Direct3DAPI::Direct3DAPI(string appName) :
      debugController(nullptr),
      factory(nullptr),
      device(nullptr),
      devicesCount(0),
      displayArray(nullptr),
      virtualDisplay(nullptr),
      displaysCount(0),
      displayPrimary(0)
   {
   for(uint32 i=0; i<MaxSupportedWorkerThreads; ++i)
      lastResult[i] = 0;

   UINT dxgiFactoryFlags = 0;

#if defined(EN_DEBUG)
   // Enable the debug layer (requires the Graphics Tools "optional feature").
   // NOTE: Enabling the debug layer after device creation will invalidate the active device.
   if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) // __uuidof(ID3D12Debug), reinterpret_cast<void**>(&debugController)
      {
      debugController->EnableDebugLayer();

      // Enable additional debug layers.
      dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
      }
#endif

   // Factory handles all physical GPU's (adapters)
   ProfileCom( CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)) ) // __uuidof(IDXGIFactory5), reinterpret_cast<void**>(&factory)

   // Create Direct3D12 Devices
   //---------------------------

   // Enumerate available physical devices (with and without video output)
   IDXGIAdapter1* deviceHandle = nullptr;
   while(factory->EnumAdapters1(devicesCount, &deviceHandle) != DXGI_ERROR_NOT_FOUND) 
      ++devicesCount;

   // Create interfaces for all available physical devices
   device = new Ptr<Direct3D12Device>[devicesCount];
   for(uint32 i=0; i<devicesCount; ++i)
      {
      deviceHandle = nullptr;
      assert( factory->EnumAdapters1(i, &deviceHandle) != DXGI_ERROR_NOT_FOUND );

      device[i] = new Direct3D12Device(this, i, deviceHandle);
      }
   }

   Direct3DAPI::~Direct3DAPI()
   {
   // Destroy Direct3D Devices
   for(uint32 i=0; i<devicesCount; ++i)
      device[i] = nullptr;
   delete [] device;

   // Release Direct3D factory
   ProfileCom( factory->Release() )
   factory = nullptr;

   // Release Debug Controller
   if (debugController)
      {
      ProfileCom( debugController->Release() )
      debugController = nullptr;
      }

   // Windows OS - Windowing System (API Independent)
   virtualDisplay = nullptr;
   for(uint32 i=0; i<displaysCount; ++i)
      displayArray[i] = nullptr;
   delete [] displayArray;
   }

   uint32 Direct3DAPI::devices(void) const
   {
   return devicesCount;
   }

   Ptr<GpuDevice> Direct3DAPI::primaryDevice(void) const
   {
   return ptr_reinterpret_cast<GpuDevice>(&device[0]);
   }

   uint32 Direct3DAPI::displays(void) const
   {
   return displaysCount;
   }

   Ptr<Display> Direct3DAPI::primaryDisplay(void) const
   {
   return ptr_reinterpret_cast<Display>(&displayArray[0]);
   }

   Ptr<Display> Direct3DAPI::display(uint32 index) const
   {
   assert( index < displaysCount );
   return ptr_reinterpret_cast<Display>(&displayArray[index]);  
   }

   }
}
#endif
