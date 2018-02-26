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
   extern bool IsError(ID3D12Device* device, const HRESULT result);
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

   #define Validate( _gpu, command )                                                \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU " << setbase(16) << _gpu << ": " << #command << endl;  \
           _gpu->lastResult[thread] = _gpu->device->command;                        \
           if (en::gpu::IsError(_gpu->lastResult[thread]))                          \
              assert( 0 );                                                          \
           en::gpu::IsWarning(_gpu->lastResult[thread]);                            \
           }

   #define ValidateCom( command )                                                   \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU 0xXXXXXXXX: " << #command << endl;                     \
           HRESULT hr = command;                                                    \
           assert( SUCCEEDED(hr) );                                                 \
           }

   #define ValidateNoRet( _gpu, command )                                           \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU " << setbase(16) << _gpu << ": " << #command << endl;  \
           _gpu->device->command;                                                   \
           }

   #define ValidateComNoRet( command )                                              \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "D3D12 GPU 0xXXXXXXXX: " << #command << endl;                     \
           command;                                                                 \
           }
   #else 

   #define Validate( _gpu, command )                                                \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           _gpu->lastResult[thread] = _gpu->device->command;                        \
           if (en::gpu::IsError(_gpu->device, _gpu->lastResult[thread]))            \
              assert( 0 );                                                          \
           en::gpu::IsWarning(_gpu->lastResult[thread]);                            \
           }

   #define ValidateCom( command )                                      \
           {                                                           \
           HRESULT hr = command;                                       \
           assert( SUCCEEDED(hr) );                                    \
           }

   #define ValidateNoRet( _gpu, command )                              \
           _gpu->device->command;

   #define ValidateComNoRet( command )                                 \
           command;


   #endif
   
#else // Release

   #define Validate( _gpu, command )                                  \
           _gpu->lastResult[Scheduler.core()] = _gpu->device->command;

   #define ValidateCom( command )                                     \
           command;

   #define ValidateNoRet( _gpu, command )                             \
           _gpu->device->command;

   #define ValidateComNoRet( command )                                \
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
      IDXGIAdapter3*          adapter;  // HW Adapter, physical GPU from which this D3D12 GPU was created
      ID3D12Device*           device;
      uint32                  queuesCount[underlyingType(QueueType::Count)];
      ID3D12CommandQueue*     queue[underlyingType(QueueType::Count)];
      ID3D12Fence*            fence[underlyingType(QueueType::Count)];        // One Fence per Queue, synchronizing CommandBuffers execution
      volatile uint32         fenceValue[underlyingType(QueueType::Count)];   // Each queue fence increasing value for proper ordering of events
      Mutex                  queueAcquire[underlyingType(QueueType::Count)]; // Ensures CommandBuffer commit and fence signal is atomic operation.
      uint32                  currentAllocator[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)]; // Specifies currently used Allocator on given thread for given queue
      uint32                  commandBuffersAllocated[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)];  // Specifies current amount of CommandBuffers allocated from current allocator
      uint32                  commandBuffersExecuting[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)][AllocatorCacheSize]; // Count of CommandBuffers still executing per allocator
      shared_ptr<CommandBuffer> commandBuffers[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)][AllocatorCacheSize][MaxCommandBuffersExecuting];
      ID3D12CommandAllocator* commandAllocator[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)][AllocatorCacheSize]; // Each thread can encode separate CommandBuffer,
                                                                              // using pool of allocators. This allows them to be reset without CPU-GPU synchronization.
      uint32                  initThreads;                                    // Amount of threads handled by this device

      // Command Buffers Management
      //----------------------------

      void addCommandBufferToQueue(shared_ptr<CommandBuffer> command);
      void clearCommandBuffersQueue(void);




      // We treat rendering destinations as fixed state that is rebinded with every RenderPass change.
      ID3D12DescriptorHeap* heapRTV; // Global heaps for current RenderPass (there can be only one)
      ID3D12DescriptorHeap* heapDSV;
      D3D12_CPU_DESCRIPTOR_HANDLE handleRTV[8];
      D3D12_CPU_DESCRIPTOR_HANDLE handleDSV;
      
      Direct3D12Device(Direct3DAPI* api, const uint32 index, IDXGIAdapter3* adapter);
     ~Direct3D12Device();



      virtual void init(void);

      virtual shared_ptr<Window> createWindow(const WindowSettings& settings,
                                              const string title);

      virtual uint32 queues(const QueueType type) const;

      virtual shared_ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                            const uint32 parentQueue = 0u);

      virtual shared_ptr<Heap>    createHeap(const MemoryUsage usage, const uint32 size);

      virtual shared_ptr<Sampler> createSampler(const SamplerState& state);
      
      virtual shared_ptr<Texture> createSharedTexture(shared_ptr<SharedSurface> backingSurface);

      virtual shared_ptr<Pipeline> createPipeline(const PipelineState& pipelineState);

      virtual shared_ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                        const bool primitiveRestart,
                                                        const uint32 controlPoints,
                                                        const uint32 usedAttributes,
                                                        const uint32 usedBuffers,
                                                        const AttributeDesc* attributes,
                                                        const BufferDesc* buffers);

      // TODO:
      virtual shared_ptr<Shader>  createShader(const ShaderStage stage,
                                               const string& source);

      virtual shared_ptr<Shader> createShader(const ShaderStage stage,
                                              const uint8* data,
                                              const uint64 size);

      virtual shared_ptr<ColorAttachment> createColorAttachment(const Format format, 
                                                                const uint32 samples = 1u);

      virtual shared_ptr<DepthStencilAttachment> createDepthStencilAttachment(const Format depthFormat, 
                                                                              const Format stencilFormat = Format::Unsupported,
                                                                              const uint32 samples = 1u);

      virtual shared_ptr<RenderPass> createRenderPass(
         const ColorAttachment& swapChainSurface,
         const DepthStencilAttachment* depthStencil = nullptr);

      virtual shared_ptr<RenderPass> createRenderPass(
         const uint32 attachments,
         const shared_ptr<ColorAttachment> color[] = nullptr,
         const DepthStencilAttachment* depthStencil = nullptr);

      virtual shared_ptr<Semaphore> createSemaphore(void);


         

      virtual shared_ptr<SetLayout> createSetLayout(const uint32 count, 
                                                    const ResourceGroup* group,
                                                    const ShaderStages stagesMask = ShaderStages::All);

      virtual shared_ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                              const shared_ptr<SetLayout>* set,
                                                              const uint32 immutableSamplers = 0u,
                                                              const shared_ptr<Sampler>* sampler = nullptr,
                                                              const ShaderStages stagesMask = ShaderStages::All);

      virtual shared_ptr<Descriptors> createDescriptorsPool(const uint32 maxSets, 
                                                            const uint32 (&count)[underlyingType(ResourceType::Count)]);


      virtual shared_ptr<RasterState>        createRasterState(const RasterStateInfo& state);

      virtual shared_ptr<MultisamplingState> createMultisamplingState(const uint32 samples,
                                                                      const bool enableAlphaToCoverage,
                                                                      const bool enableAlphaToOne);

      virtual shared_ptr<DepthStencilState>  createDepthStencilState(const DepthStencilStateInfo& desc);
      
      virtual shared_ptr<BlendState>         createBlendState(const BlendStateInfo& state,
                                                              const uint32 attachments,
                                                              const BlendAttachmentInfo* color);

      virtual shared_ptr<ViewportState> createViewportState(const uint32 count,
                                                            const ViewportStateInfo* viewports,
                                                            const ScissorStateInfo* scissors);

      virtual LinearAlignment textureLinearAlignment(const Texture& texture, 
                                                     const uint32 mipmap, 
                                                     const uint32 layer);
      };


   // Direct3D API Interface
   class Direct3DAPI : public CommonGraphicAPI
      {
      public:
      HMODULE                library;
      HRESULT                lastResult[MaxSupportedWorkerThreads];

      ID3D12Debug*           debugController;
      IDXGIFactory5*         factory;         // Application Direct3D API Factory
      shared_ptr<Direct3D12Device>* _device;  // Physical Device Interfaces
      uint32                 devicesCount;

      public:
      Direct3DAPI(string appName);
      virtual ~Direct3DAPI();

      virtual RenderingAPI type(void) const;

      // TODO: Those could be moved to CommonGraphicAPI
      virtual uint32 devices(void) const;
      virtual shared_ptr<GpuDevice> primaryDevice(void) const;
      virtual shared_ptr<GpuDevice> device(const uint32 index) const;
      
      virtual uint32 displays(void) const;
      virtual shared_ptr<Display> primaryDisplay(void) const;
      virtual shared_ptr<Display> display(const uint32 index) const;
      };
   }
}
#endif

#endif
