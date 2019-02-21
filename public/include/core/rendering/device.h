/*

 Ngine v5.0
 
 Module      : GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_DEVICE
#define ENG_CORE_RENDERING_DEVICE

#include <memory>
#include <string>

#include "core/rendering/blend.h"
#include "core/rendering/buffer.h"
#include "core/rendering/commandBuffer.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/display.h"
#include "core/rendering/heap.h"
#include "core/rendering/inputLayout.h"
#include "core/rendering/layout.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/raster.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/sampler.h"
#include "core/rendering/shader.h"
#include "core/rendering/state.h"
#include "core/rendering/surface.h"
#include "core/rendering/synchronization.h"
#include "core/rendering/texture.h"
#include "core/rendering/viewport.h"
#include "core/rendering/window.h"

/// Resources life-time management:
/// -------------------------------
///
/// Life-time of enumerated objects, which cannot be created or destroyed by
/// application itself, is managed by rendering abstraction. Such resources can
/// still come and go, following Plug & Play rules:
///
/// - GraphicAPI
/// - GpuDevice
/// - Display
///
/// Rendering abstraction is designed to be as lightweight as possible, thus
/// there is no mechanism tracking life-time of resources that are created by
/// application. The only exception are CommandBuffer objects, which even though
/// created by application, are tracked by given device garbage collector, and
/// automatically destroyed once signaled as processed.
///
/// Application is responsible for ensuring that all resources, referenced by
/// Command Buffers (no matter if still being encoded by CPU, waiting in queue
/// or already being in the middle of processing on GPU) need to be present and
/// alive until encoded and submitted work is done. This refers only to objects
/// directly referenced by Command Buffers:
///
/// - Heap
/// - Buffer
/// - Texture
/// - Sampler
/// - Pipeline
/// - Descriptor
/// - DescriptorSet
/// - Framebuffer
/// - RenderPass
/// - Semaphore
/// - Window
///
/// Below state objects used to describe given pipeline state can be released
/// once Pipeline object is created (or reused for other Pipeline objects
/// creation):
///
/// - BlendState
/// - DepthStencilState
/// - RasterState
/// - MultisamplingState
/// - ViewportState
/// - Shader
///
/// Resources layout objects can be released once Pipeline objects are created
/// and needed DescriptorSets are allocated:
///
/// - InputLayout
/// - SetLayout
/// - PipelineLayout
///
/// The same rule refers to RenderPass helper objects:
///
/// - ColorAttachment
/// - DepthStencilAttachment
///
namespace en
{
namespace gpu
{
/// For use of API specific features (shouldn't be used if not really needed).
enum class RenderingAPI : uint32
{
    Direct3D = 0,
    Metal       ,
    Vulkan      ,
};

/// All queues support transfer operations.If device support Sparse resources,
/// Universal and Compute queues support Sparse Transfer as well. (can we make
/// this assumption ?)
enum class QueueType : uint32
{
    Universal      = 0, ///< Supports both Rendering and Compute workloads
    Compute           , ///< Only Compute workloads
    Transfer          , ///< Only Transfer operations
    SparseTransfer    , ///< Only Transfer and Sparse Resource operations
    Count
};
      
/// Per device context that can be used to perform operations on GPU
class GpuDevice : public std::enable_shared_from_this<GpuDevice>
{
    public:
      
      
    // Presentation:
      
      
    /// List of displays to which this device can render to (may not be the
    /// same as list of displays that are physically connected to it, as system
    /// may underneath transfer final framebuffer between GPU's for presentation
    /// purposes).
    virtual uint32 displays(void) const = 0;
      
    /// N'th display handle
    virtual std::shared_ptr<Display> display(
        const uint32 id) const = 0;

    virtual Window* createWindow(
        const WindowSettings& settings,
        const std::string title) = 0;
         
         
    // Resources creation:
      
      
    /// Create Heap from which GPU resources can be sub-allocated.
    virtual Heap* createHeap(
        const MemoryUsage usage,
        const uint32 size) = 0;
      
    virtual std::shared_ptr<Sampler> createSampler(
        const SamplerState& state) = 0;
      
    /// Buffers and Textures are allocated from the Heaps.
      
    /// Create texture that can be shared between processes and API's through
    /// shared backing surface. If not supported on given platform, returns
    /// nullptr.
    virtual std::shared_ptr<Texture> createSharedTexture(
        std::shared_ptr<SharedSurface> backingSurface) = 0;
    
    // TODO:
    // Vulkan - entrypoint is specified at Pipeline creation I guess
    // Metal  - has libraries, from which we pick functions as entry points
    virtual std::shared_ptr<Shader> createShader(
        const ShaderStage stage,
        const std::string& source) = 0;

    virtual std::shared_ptr<Shader> createShader(
        const ShaderStage stage,
        const uint8* data,
        const uint64 size) = 0;


    /// Returns count of available Command Queues of given type
    virtual uint32 queues(const QueueType type) const = 0;
    
    /// Creates Command Buffer from the given Command Queue of given type. When
    /// this buffer is commited for execution it will execute on that queue.
    virtual std::shared_ptr<CommandBuffer> createCommandBuffer(
        const QueueType type = QueueType::Universal,
        const uint32 parentQueue = 0u) = 0;



    /// Input Layout Primitive Restart feature note:
    ///
    /// If primitiveRestart is enabled, and PrimitiveType is LineStripes or
    /// TriangleStripes, primitives will be restarted on 0xFFFF or 0xFFFFFFFF
    /// element index, depending what data type is used by Index Buffer.
    /// Primitive restart flag is ignored for all other primitive types.
    ///
    /// It is adviced to disable primitive restart, and instead provide index
    /// buffer that distinguishes separate stripes with degenerate primitives
    /// (through repeated indexes). This way some GPU's can process whole
    /// workload in parallel, by dividing it. With special primitive restart
    /// index, workload cannot be easily distributed. Some underlying API's
    /// may still use primitive restart index specified above, so it is adviced
    /// to not use it as standard element index anyway.
    
    /// Creates InputLayout description based on single buffer.
    /// Buffer needs to have specified internal formatting.
    virtual InputLayout* createInputLayout(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const Buffer& buffer) = 0;

    /// Creates empty input layout for Programmable Vertex Fetch.
    virtual InputLayout* createInputLayout(
        const DrawableType primitiveType,
        const uint32 controlPoints = 0u) = 0;

    /// Specialized function for creation of any type of InputLayout.
    virtual InputLayout* createInputLayout(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const uint32 usedAttributes,
        const uint32 usedBuffers,
        const AttributeDesc* attributes,
        const BufferDesc* buffers) = 0;

    // TODO: API differences:
    // - D3D12  - allows picking only one stage (or all) to access this Descriptor Set
    //          - it is specified once per whole Descriptor Set (per Root Parameter)
    // - Vulkan - allows picking several stages to access this Descriptor Set
    //          - it is specified separately for each Resources Group
    //
    // Creates layout of DescriptorSet. Can describe several different
    // groups of resources. Access by specific shader stage, to all
    // this resources can be specified (by default all shader stages
    // can access those resources). If there is a need to specify
    // access for few but not all shader stages, just duplicate this
    // layout with different access, and bind the same DescriptorSet
    // to it.
    //
    // TODO: Could this be done on the D3D12/Metal backend side instead?
    virtual std::shared_ptr<SetLayout> createSetLayout(
        const uint32 count,
        const ResourceGroup* group,
        const ShaderStages stagesMask = ShaderStages::All) = 0;

    virtual std::shared_ptr<PipelineLayout> createPipelineLayout(
        const uint32 sets,
        const std::shared_ptr<SetLayout>* set,
        const uint32 immutableSamplers = 0u,
        const std::shared_ptr<Sampler>* sampler = nullptr,
        const ShaderStages stagesMask = ShaderStages::All) = 0;

    virtual std::shared_ptr<Descriptors> createDescriptorsPool(
        const uint32 maxSets,
        const uint32 (&count)[underlyingType(ResourceType::Count)]) = 0;


    // TODO: Those methods should be reworked to accept TextureView,
    //       and layer selection should be done through it.

    // Metal supports specifying base layer of Render Pass attachment through
    // explicitly setting "slice" or "depthPlane" in MTLRenderPassAttachmentDescriptor,
    // and given mipmap through setting "level" in the same object.
    // iOS can render to any layer and mipmap, on macOS only to 0 layer, so use views anyway to
    // avoid internal copies.
    //
    // Vulkan supports the same feature through providing new ImageView that
    // starts at given mipmap and layer of original texture.
    //
    // Metal is not supporting rendering to more than one layer at the same time.
 
    /// When binding 3D texture, pass it's "depth" plane through "layer"
    /// parameter. Similarly when binding CubeMap texture, pass it's "face"
    /// through "layer".
    virtual ColorAttachment* createColorAttachment(
        const Format format,
        const uint32 samples = 1u) = 0;

    /// By default, Load operation is set to Load, and Store operation to Store
    virtual DepthStencilAttachment* createDepthStencilAttachment(
        const Format depthFormat,
        const Format stencilFormat = Format::Unsupported,
        const uint32 samples = 1u) = 0;

    /// Creates render pass with Swap-Chain surface as destination.
    /// Swap-Chain surface may be destination of MSAA resolve operation.
    virtual RenderPass* createRenderPass(
        const ColorAttachment& swapChainSurface,
        const DepthStencilAttachment* depthStencil = nullptr) = 0;

    /// Creates render pass. Entries in "color" array, match output color
    /// attachment slots in Fragment Shader. Entries in this array may be set
    /// to nullptr, which means that given output color attachment slot has no
    /// bound resource descriptor.
    virtual RenderPass* createRenderPass(
        const uint32 attachments,
        const std::shared_ptr<ColorAttachment> color[] = nullptr,
        const DepthStencilAttachment* depthStencil = nullptr) = 0;

    virtual std::shared_ptr<Semaphore> createSemaphore(void) = 0;

      




    // Pipeline states:


    virtual RasterState* createRasterState(
        const RasterStateInfo& state) = 0;

    virtual MultisamplingState* createMultisamplingState(
        const uint32 samples,
        const bool enableAlphaToCoverage,
        const bool enableAlphaToOne) = 0;

    virtual DepthStencilState* createDepthStencilState(
        const DepthStencilStateInfo& desc) = 0;

    virtual BlendState* createBlendState(
        const BlendStateInfo& state,
        const uint32 attachments,
        const BlendAttachmentInfo* color) = 0;
    
    virtual ViewportState* createViewportState(
        const uint32 count,
        const ViewportStateInfo* viewports,
        const ScissorStateInfo* scissors) = 0;
 
    /// Returns default Pipeline state helper structure, that can be easily
    /// modified and passed to Pipeline object creation call. All states are
    /// set to their defaults. Input Layout is set to TrnagleStripes, and no
    /// input attributes are specified. Pipeline Layout also assumes no 
    /// resources are used.
    /// App still needs to set Viewport State and assign Shaders.
    virtual PipelineState defaultPipelineState(void) = 0;

    virtual std::shared_ptr<Pipeline> createPipeline(
        const PipelineState& pipelineState) = 0;


    // Capabilities query:


    /// Size of memory dedicated to this GPU.
    /// Discreete GPU's VRAM size. Some mobile/integrated GPU's may report 
    /// portion of system memory that was dedicated to them in BIOS.
    virtual uint64 dedicatedMemorySize(void) = 0;

    /// Size of system memory that this GPU can use.
    virtual uint64 systemMemorySize(void) = 0;

    /// Size of texel in bytes, based on the given format.
    /// For compressed formats, it's texel block size.
    virtual uint32 texelSize(
        const Format format,
        const uint8  plane = 0) const = 0;

    /// Provides description of staging buffer alignment and
    /// padding required for given texture layer data upload.
    virtual ImageMemoryAlignment textureMemoryAlignment(
        const TextureState& state,
        const uint32 mipmap,
        const uint32 layer) const = 0;

    virtual ~GpuDevice() {};
};
      
/// Per graphic API context, initialized depending on API choosed at runtime
class GraphicAPI
{
    public:
    
    /// Creates this class instance (API specific) and assigns it to "Graphics"
    static bool create(void);
    
    virtual RenderingAPI type(void) const = 0;

    virtual uint32 devices(void) const = 0;
    virtual std::shared_ptr<GpuDevice> primaryDevice(void) const = 0;
    virtual std::shared_ptr<GpuDevice> device(const uint32 index) const = 0;
    
    virtual uint32 displays(void) const = 0;
    virtual std::shared_ptr<Display> primaryDisplay(void) const = 0;
    virtual std::shared_ptr<Display> display(const uint32 index) const = 0;

    virtual ~GraphicAPI() {};
};

} // en::gpu

extern std::shared_ptr<gpu::GraphicAPI> Graphics;
} // en

#endif
