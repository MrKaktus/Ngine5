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
#include "core/rendering/d3d12/dx12CommandBuffer.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "threading/scheduler.h"
#include "utilities/strings.h"

#include "platform/system.h"
#include "platform/windows/win_events.h"

namespace en
{
   namespace gpu
   {
   // Checks Vulkan error state
   bool IsError(ID3D12Device* device, const HRESULT result)
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
   if (result == DXGI_ERROR_DEVICE_REMOVED)
      {
      HRESULT reason = device->GetDeviceRemovedReason();
      IsError(device, reason);

      info += "Above error caused: DXGI_ERROR_DEVICE_REMOVED.\n";
      info += "       The GPU device instance has been suspended.\n";
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
   else // Error message unknown!
      {
      info += "UNKNOWN: ";
      info += (uint64)(result);
      info += " \n";
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
      device(nullptr),
      initThreads(min(Scheduler.workers(), MaxSupportedWorkerThreads))

   {
   for(uint32 i=0; i<initThreads; ++i)
      lastResult[i] = 0;

   // Way to find HW Adapter from which given D3D12 Device was created (DXGI 1.4+):
   // LUID luid = ID3D12Device::GetAdapterLuid();
   // IDXGIAdapter* adapter = nullptr;
   // HRESULT IDXGIFactory4::EnumAdapterByLuid(luid, IID_PPV_ARGS(adapter)); // __uuidof(IDXGIAdapter), reinterpret_cast<void**>(&adapter)

   // Try to create device with 12.1 feature level, if not supported fallback to 12.0
   if (!SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)))) // __uuidof(ID3D12Device), reinterpret_cast<void**>(&device)
      ProfileCom( D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)) ) // __uuidof(ID3D12Device), reinterpret_cast<void**>(&device)

#if defined(EN_DEBUG)
   // Name Device for debugging
   string name("D3D12 GPU N: ");
   name += stringFrom(index);
   device->SetName((LPCWSTR)name.c_str());
#endif

    // TODO: Debug print of GPU:

    //  DXGI_ADAPTER_DESC1 adapterDescription; 
    //  deviceHandle->GetDesc1(&adapterDescription);
    //WCHAR Description[ 128 ];
    //UINT VendorId;
    //UINT DeviceId;
    //UINT SubSysId;
    //UINT Revision;
    //SIZE_T DedicatedVideoMemory;
    //SIZE_T DedicatedSystemMemory;
    //SIZE_T SharedSystemMemory;
    //LUID AdapterLuid;
    //UINT Flags;


   // COMMAND QUEUES
   // ==============

   // TODO: Is there a way to query amount of available queues of each type?
   uint32 types = underlyingType(QueueType::Count);
   for(uint32 type=0; type<types; ++type)
      {
      // Create single queue for each Queue Family
      D3D12_COMMAND_QUEUE_DESC desc;
      desc.Type     = TranslateQueueType[type];
      desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // In [0..100] range 100 == D3D12_COMMAND_QUEUE_PRIORITY_HIGH
      desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;       // Can use D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
      desc.NodeMask = 0u;                                  // No Multi-GPU support.
     
      Profile( this, CreateCommandQueue(&desc, IID_PPV_ARGS(&queue[type])) ) // __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&queue[type])

#if defined(EN_DEBUG)
      // Name CommandQueue for debugging
      string name("CommandQueue Q: ");
      name += stringFrom(type);
      queue[type]->SetName((LPCWSTR)name.c_str());
#endif

      queuesCount[type] = 1;

      // Each queue has matching fence, to synchronize it's CommandBuffers
      // TODO: Consider sharing across all adapters with D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER
      Profile( this, CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&fence[type])) ) // __uuidof(ID3D12Fence), reinterpret_cast<void**>(&fence)

#if defined(EN_DEBUG)
      // Name Fence for debugging
      name = "Fence Q: ";
      name += stringFrom(type);
      fence[type]->SetName((LPCWSTR)name.c_str());
#endif
      }

   // All queues start with inital fence state
   memset((void*)&fenceValue[0], 0, sizeof(fenceValue));

   // COMMAND ALLOCATORS
   // ==================

   // Each thread has separate pool of allocators for each Queue Type
   for(uint32 thread=0; thread<initThreads; ++thread)
      for(uint32 type=0; type<underlyingType(QueueType::Count); ++type)
         {
         for(uint32 cache=0; cache<AllocatorCacheSize; ++cache)
            {
            // " A given allocator can be associated with no more than one
            //   currently recording command list at a time, . . . "
            //
            commandAllocator[thread][type][cache] = nullptr;
            Profile( this, CreateCommandAllocator(TranslateQueueType[type],
                                                  IID_PPV_ARGS(&(commandAllocator[thread][type][cache]) )) ) // __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&commandAllocator[thread][type][cache])
         
            // TODO: BUG: FIXME: Why it fails to create more allocators than 8 ????? 
            //                   It's not throwing any errors, and while RefCount for
            //                   device grows, pointer is not beeing returned. 
            //                   Looks like internal driver error.
            assert( commandAllocator[thread][type][cache] );
            }
         
         currentAllocator[thread][type] = 0;
         commandBuffersAllocated[thread][type] = 0;
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

#if defined(EN_DEBUG)
   // Name RTV Heap for debugging
   name = "RTV Heap";
   heapRTV->SetName((LPCWSTR)name.c_str());
#endif

   desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
   desc.NumDescriptors = 1;
   
   // Allocate global Depth-Stencil Attachment descriptor heap
   Profile( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heapDSV)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&heapDSV)

#if defined(EN_DEBUG)
   // Name DSV Heap for debugging
   name = "DSV Heap";
   heapDSV->SetName((LPCWSTR)name.c_str());
#endif

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

   // Release CommandBuffers in flight once they are done
   bool stillExecuting;
   do
   {
   stillExecuting = false;
   for(uint32 thread=0; thread<initThreads; ++thread)
      for(uint32 queueType=0; queueType<underlyingType(QueueType::Count); ++queueType)
         for(uint32 cacheId=0; cacheId<AllocatorCacheSize; ++cacheId)
            {
            uint32 executing = commandBuffersExecuting[thread][queueType][cacheId];
            for(uint32 i=0; i<executing; ++i)
               {
               CommandBufferD3D12* command = raw_reinterpret_cast<CommandBufferD3D12>(&commandBuffers[thread][queueType][cacheId][i]);
               if (command->isCompleted())
                  {
                  // Safely release Command Buffer object
                  commandBuffers[thread][queueType][cacheId][i] = nullptr;
                  if (i < (executing - 1))
                     {
                     commandBuffers[thread][queueType][cacheId][i] = commandBuffers[thread][queueType][cacheId][executing - 1];
                     commandBuffers[thread][queueType][cacheId][executing - 1] = nullptr;
                     }
            
                  executing--;
                  commandBuffersExecuting[thread][queueType][cacheId]--;
                  }
               else
                  stillExecuting = true;
               }
            }
   }
   while(stillExecuting);

   // Destroy Command Queues, their Fences and Allocators
   for(uint32 queueType=0; queueType<underlyingType(QueueType::Count); ++queueType)
      {
      for(uint32 thread=0; thread<initThreads; ++thread)
         for(uint32 cache=0; cache<AllocatorCacheSize; ++cache)
            {
            commandAllocator[thread][queueType][cache]->Reset();
            commandAllocator[thread][queueType][cache]->Release();
            commandAllocator[thread][queueType][cache] = nullptr;
            }

      fence[queueType]->Release();
      fence[queueType] = nullptr;

      queue[queueType]->Release();
      queue[queueType] = nullptr;
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
   // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dn788653(v=vs.85).aspx

   support.maxColorAttachments = 8;

   for(uint32 thread=0; thread<initThreads; ++thread)
      for(uint32 queueType=0; queueType<underlyingType(QueueType::Count); ++queueType)
         for(uint32 cacheId=0; cacheId<AllocatorCacheSize; ++cacheId)
            {
            commandBuffersExecuting[thread][queueType][cacheId] = 0u;
            for(uint32 i=0; i<MaxCommandBuffersExecuting; i++)      
               commandBuffers[thread][queueType][cacheId][i] = nullptr;
            }

   CommonDevice::init();
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
      library(LoadLibrary(L"d3d12.dll")),
      debugController(nullptr),
      factory(nullptr),
      device(nullptr),
      devicesCount(0),
      CommonGraphicAPI()
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

#if defined(EN_DEBUG)
   // Name Factory for debugging
   string name("Factory");
   factory->SetPrivateData( WKPDID_D3DDebugObjectName, name.length(), name.c_str() );
#endif

   // Create Direct3D12 Devices
   //---------------------------

   // Enumerate available physical devices (with and without video output) that support D3D12
   IDXGIAdapter1* deviceHandle = nullptr;
   uint32 deviceIndex = 0;
   while(factory->EnumAdapters1(deviceIndex, &deviceHandle) != DXGI_ERROR_NOT_FOUND) 
      {
      if (SUCCEEDED(D3D12CreateDevice(deviceHandle, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
         ++devicesCount;
      deviceIndex++;

      // Release acquired handle after query
      deviceHandle->Release();
      deviceHandle = nullptr;
      }

   device = new Ptr<Direct3D12Device>[devicesCount];

   // Create interfaces for all physical devices supporting D3D12
   uint32 index = 0;
   deviceIndex = 0;
   while(factory->EnumAdapters1(index, &deviceHandle) != DXGI_ERROR_NOT_FOUND) 
      {
      if (SUCCEEDED(D3D12CreateDevice(deviceHandle, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
         {
         device[deviceIndex] = new Direct3D12Device(this, deviceIndex, deviceHandle);
         ++deviceIndex;
         }
      else
         {
         // Release acquired handle if Device Creation fails
         deviceHandle->Release();
         deviceHandle = nullptr; 
         }

      index++;
      }

   //for(uint32 i=0; i<devicesCount; ++i)
   //   {
   //   deviceHandle = nullptr;
   //   assert( factory->EnumAdapters1(i, &deviceHandle) != DXGI_ERROR_NOT_FOUND );

   //   // Verify that device supports at least 12.0 feature level

   //      {
   //      device[i] = new Direct3D12Device(this, i, deviceHandle);
   //      }
   //   }

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
   }

   RenderingAPI Direct3DAPI::type(void) const
   {
   return RenderingAPI::Direct3D;
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
