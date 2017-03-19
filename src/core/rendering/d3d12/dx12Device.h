/*

 Ngine v5.0
 
 Module      : Direct3D 12 GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_DEVICE
#define ENG_CORE_RENDERING_D3D12_DEVICE

#include "core/rendering/d3d12/dx12.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/common/device.h"
#include "core/rendering/common/display.h"

#include "core/rendering/d3d12/dx12InputLayout.h"
#include "core/rendering/d3d12/dx12Blend.h"
#include "core/rendering/d3d12/dx12Raster.h"
#include "core/rendering/d3d12/dx12Viewport.h"
#include "core/rendering/d3d12/dx12DepthStencil.h"

#include "threading/scheduler.h" // For Profile

using namespace std;

#ifdef EN_DEBUG
namespace en
{
   namespace gpu
   {
   extern bool IsError(const HRESULT result);
   extern bool IsWarning(const HRESULT result);
   }
}
#endif
//
// gpu     - pointer to class storing pointer to called function.
//           Should be VulkanDevice*, or VulkanGPU*.
// command - Vulkan API function call to execute.
//
// Result of function call is stored per GPU, per Thread.
// threading/scheduler.h needs to be included.
//
#ifdef EN_DEBUG
   #ifdef EN_PROFILER_TRACE_GRAPHICS_API

   #define Profile( _gpu, command )                                                 \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU " << setbase(16) << _gpu << ": " << #command << endl;  \
           _gpu->lastResult[thread] = _gpu->device->command;                        \
           if (en::gpu::IsError(_gpu->lastResult[thread]))                          \
              assert( 0 );                                                          \
           en::gpu::IsWarning(_gpu->lastResult[thread]);                            \
           }

   #define ProfileCom( command )                                                       \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU 0xXXXXXXXX: " << #command << endl;                     \
           HRESULT hr = command;                                                    \
           assert( SUCCEEDED(hr) );                                                 \
           }

   #define ProfileNoRet( _gpu, command )                                            \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU " << setbase(16) << _gpu << ": " << #command << endl;  \
           _gpu->device->command;                                                   \
           }

   #define ProfileComNoRet( command )                                                  \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU 0xXXXXXXXX: " << #command << endl;                     \
           command;                                                                 \
           }
   #else 

   #define Profile( _gpu, command )                                    \
           {                                                           \
           uint32 thread = Scheduler.core();                           \
           _gpu->lastResult[thread] = _gpu->device->command;           \
           if (en::gpu::IsError(_gpu->lastResult[thread]))             \
              assert( 0 );                                             \
           en::gpu::IsWarning(_gpu->lastResult[thread]);               \
           }

   #define ProfileCom( command )                                       \
           {                                                           \
           HRESULT hr = command;                                       \
           assert( SUCCEEDED(hr) );                                    \
           }

   #define ProfileNoRet( _gpu, command )                               \
           _gpu->device->command;

   #define ProfileComNoRet( command )                                  \
           command;


   #endif
   
#else // Release

   #define Profile( _gpu, command )                                   \
           _gpu->lastResult[Scheduler.core()] = _gpu->device->command;

   #define ProfileCom( command )                                      \
           command;

   #define ProfileNoRet( _gpu, command )                              \
           _gpu->device->command;

   #define ProfileComNoRet( command )                                 \
           command;

#endif

namespace en
{
   namespace gpu
   {
   // TODO: Direct3D12 Compute queue is not supporting transfer operations!
   //       Correct the description in public headers, and asserts in all 3 API's!
   
   const D3D12_COMMAND_LIST_TYPE TranslateQueueType[underlyingType(QueueType::Count)]
      {
      D3D12_COMMAND_LIST_TYPE_DIRECT ,  // Universal
      D3D12_COMMAND_LIST_TYPE_COMPUTE,  // Compute
      D3D12_COMMAND_LIST_TYPE_COPY   ,  // Transfer
      D3D12_COMMAND_LIST_TYPE_COPY   ,  // SparseTransfer
      };
      
   // Bundles/Secondary Command Buffers are not supported currently.
   // - D3D12_COMMAND_LIST_TYPE_BUNDLE

   class Direct3DAPI;

   class Direct3D12Device : public CommonDevice
      {
      public:
      HRESULT                 lastResult[MaxSupportedWorkerThreads];
      Direct3DAPI*            api;      // Direct3D API 
      uint32                  index;    // This device number on the list
      IDXGIAdapter1*          adapter;  // HW Adapter, physical GPU from which this D3D12 GPU was created
      ID3D12Device*           device;
      uint32                  queuesCount[underlyingType(QueueType::Count)];
      ID3D12CommandQueue*     queue[underlyingType(QueueType::Count)];
      ID3D12CommandAllocator* commandAllocator[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)];
      uint32                  fenceCurrentValue[MaxSupportedWorkerThreads]; // Pool of values signaled by Fences
      HANDLE                  fenceSignalingEvent[MaxSupportedWorkerThreads]; // Event used to signal Fence completion on CPU side
      
	//// Synchronization objects.
	//UINT m_frameIndex;
	//HANDLE m_fenceEvent;
	//ComPtr<ID3D12Fence> m_fence;
	//UINT64 m_fenceValue;

      // We treat rendering destinations as fixed state that is rebinded with every RenderPass change.
      ID3D12DescriptorHeap* heapRTV; // Global heaps for current RenderPass (there can be only one)
      ID3D12DescriptorHeap* heapDSV;
      D3D12_CPU_DESCRIPTOR_HANDLE handleRTV[8];
      D3D12_CPU_DESCRIPTOR_HANDLE handleDSV;
      
      Direct3D12Device(Direct3DAPI* api, const uint32 index, IDXGIAdapter1* adapter);
     ~Direct3D12Device();



      virtual void init(void);

      virtual uint32 displays(void) const;

      virtual Ptr<Display> display(uint32 index) const;

      virtual Ptr<Window> createWindow(const WindowSettings& settings, 
                                       const string title);

      virtual uint32 queues(const QueueType type) const;

      // Creates Command Buffer from the given Command Queue of given type.
      // When this buffer is commited for execution it will execute on that queue.
      virtual Ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                     const uint32 parentQueue = 0u);

      virtual Ptr<Heap>    createHeap(const MemoryUsage usage, const uint32 size);

      virtual Ptr<Sampler> createSampler(const SamplerState& state);
      
      virtual Ptr<Texture> createSharedTexture(Ptr<SharedSurface> backingSurface);

      virtual Ptr<Pipeline> createPipeline(const PipelineState& pipelineState);

      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                 const uint32 controlPoints,
                                                 const uint32 usedAttributes,
                                                 const uint32 usedBuffers,
                                                 const AttributeDesc* attributes,
                                                 const BufferDesc* buffers);

      // TODO:
      virtual Ptr<Shader>  createShader(const ShaderStage stage,
                                        const string& source);

      virtual Ptr<ColorAttachment> createColorAttachment(const Format format, 
                                                         const uint32 samples = 1u);

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Format depthFormat, 
                                                                       const Format stencilFormat = Format::Unsupported,
                                                                       const uint32 samples = 1u);

      virtual Ptr<RenderPass> createRenderPass(const Ptr<ColorAttachment> swapChainSurface,
                                               const Ptr<DepthStencilAttachment> depthStencil);

      virtual Ptr<RenderPass> createRenderPass(const uint32 attachments,
                                               const Ptr<ColorAttachment>* color,
                                               const Ptr<DepthStencilAttachment> depthStencil);

      virtual Ptr<Semaphore> createSemaphore(void);


         

      virtual Ptr<SetLayout> createSetLayout(const uint32 count, 
                                             const ResourceGroup* group,
                                             const ShaderStages stagesMask = ShaderStages::All);

      virtual Ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                       const Ptr<SetLayout>* set,
                                                       const uint32 immutableSamplers = 0u,
                                                       const Ptr<Sampler>* sampler = nullptr,
                                                       const ShaderStages stagesMask = ShaderStages::All);




      virtual Ptr<RasterState>        createRasterState(const RasterStateInfo& state);

      virtual Ptr<MultisamplingState> createMultisamplingState(const uint32 samples,
                                                               const bool enableAlphaToCoverage,
                                                               const bool enableAlphaToOne);

      virtual Ptr<DepthStencilState>  createDepthStencilState(const DepthStencilStateInfo& desc);
      
      virtual Ptr<BlendState>         createBlendState(const BlendStateInfo& state,
                                                       const uint32 attachments,
                                                       const BlendAttachmentInfo* color);

      virtual Ptr<ViewportState> createViewportState(const uint32 count,
                                                     const ViewportStateInfo* viewports,
                                                     const ScissorStateInfo* scissors);

      };


   // Direct3D API Interface
   class Direct3DAPI : public GraphicAPI
      {
      public:
      HRESULT                lastResult[MaxSupportedWorkerThreads];

      ID3D12Debug*           debugController;
      IDXGIFactory5*         factory;         // Application Direct3D API Factory
      Ptr<Direct3D12Device>* device;          // Physical Device Interfaces
      uint32                 devicesCount;

      // API Independent, OS Dependent - Windowing System
      Ptr<CommonDisplay>*    displayArray;
      Ptr<CommonDisplay>     virtualDisplay;
      uint32                 displaysCount;
      uint32                 displayPrimary;

      public:
      Direct3DAPI(string appName);
      virtual ~Direct3DAPI();

      // TODO: Those could be moved to CommonGraphicAPI
      virtual uint32 devices(void) const;
      virtual Ptr<GpuDevice> primaryDevice(void) const;

      virtual uint32 displays(void) const;
      virtual Ptr<Display> primaryDisplay(void) const;
      virtual Ptr<Display> display(uint32 index) const;
      };
   }
}
#endif

#endif
