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

#include <string>
#include "core/utilities/TintrusivePointer.h" 

#include "core/rendering/inputLayout.h"
#include "core/rendering/blend.h"
#include "core/rendering/display.h"
#include "core/rendering/window.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/raster.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/sampler.h"
#include "core/rendering/texture.h"
#include "core/rendering/surface.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/shader.h"
#include "core/rendering/heap.h"
#include "core/rendering/commandBuffer.h"
#include "core/rendering/layout.h"
#include "core/rendering/synchronization.h"
#include "utilities/Nversion.h"


using namespace std;

namespace en
{
   namespace gpu
   {
   // Graphic API and Shading Language API id's
   #define EN_OpenGL    1
   #define EN_GLSL      2
   #define EN_OpenGL_ES 3
   #define EN_ESSL      4
   #define EN_Metal_OSX 5
   #define EN_Metal_IOS 6
   #define EN_MetalSL   7
   #define EN_Vulkan    8
   #define EN_SPIRV     9
   #define EN_Direct3D  10
   #define EN_HLSL      11
   
   extern const Nversion Vulkan_1_0;                  // Vulkan 1.0
   
   extern const Nversion Metal_OSX_1_0;               // Metal OSX 1.0
   extern const Nversion Metal_OSX_Unsupported;       // For marking unsupported features
   
   extern const Nversion Metal_IOS_1_0;               // Metal 1.0
   extern const Nversion Metal_IOS_Unsupported;       // For marking unsupported features

   extern const Nversion ESSL_3_00;                   // ESSL 3.00
   extern const Nversion ESSL_1_00;                   // ESSL 1.00
   extern const Nversion ESSL_Unsupported;            // For marking unsupported features

   extern const Nversion OpenGL_ES_3_2;               // OpenGL ES 3.2 -
   extern const Nversion OpenGL_ES_3_1;               // OpenGL ES 3.1 - 
   extern const Nversion OpenGL_ES_3_0;               // OpenGL ES 3.0 - 
   extern const Nversion OpenGL_ES_2_0;               // OpenGL ES 2.0 - 
   extern const Nversion OpenGL_ES_1_1;               // OpenGL ES 1.1 - 
   extern const Nversion OpenGL_ES_1_0;               // OpenGL ES 1.0 - 
   extern const Nversion OpenGL_ES_Unsupported;       // For marking unsupported features

   extern const Nversion GLSL_Next;                   // Future GLSL versions
   extern const Nversion GLSL_4_50;                   // GLSL 4.50
   extern const Nversion GLSL_4_40;                   // GLSL 4.40
   extern const Nversion GLSL_4_30;                   // GLSL 4.30
   extern const Nversion GLSL_4_20;                   // GLSL 4.20
   extern const Nversion GLSL_4_10;                   // GLSL 4.10
   extern const Nversion GLSL_4_00;                   // GLSL 4.00
   extern const Nversion GLSL_3_30;                   // GLSL 3.30
   extern const Nversion GLSL_1_50;                   // GLSL 1.50 for OpenGL 3.2
   extern const Nversion GLSL_1_40;                   // GLSL 1.40 for OpenGL 3.1
   extern const Nversion GLSL_1_30;                   // GLSL 1.30 for OpenGL 3.0
   extern const Nversion GLSL_1_20;                   // GLSL 1.20 for OpenGL 2.1
   extern const Nversion GLSL_1_10;                   // GLSL 1.10 for OpenGL 2.0
   extern const Nversion GLSL_Unsupported;            // For marking unsupported features

   extern const Nversion OpenGL_Next;                 // Future OpenGL versions
   extern const Nversion OpenGL_4_5;                  // OpenGL 4.5    -  
   extern const Nversion OpenGL_4_4;                  // OpenGL 4.4    -   
   extern const Nversion OpenGL_4_3;                  // OpenGL 4.3    -   
   extern const Nversion OpenGL_4_2;                  // OpenGL 4.2    -
   extern const Nversion OpenGL_4_1;                  // OpenGL 4.1    - 
   extern const Nversion OpenGL_4_0;                  // OpenGL 4.0    - 11.03.2010 ok
   extern const Nversion OpenGL_3_3;                  // OpenGL 3.3    - 11.03.2010 ok
   extern const Nversion OpenGL_3_2;                  // OpenGL 3.2    - 24.07.2009 ok
   extern const Nversion OpenGL_3_1;                  // OpenGL 3.1    - 24.03.2009 ok
   extern const Nversion OpenGL_3_0;                  // OpenGL 3.0    - 11.08.2008 ok
   extern const Nversion OpenGL_2_1;                  // OpenGL 2.1    - 30.07.2006 spec
   extern const Nversion OpenGL_2_0;                  // OpenGL 2.0    - 22.10.2004 spec
   extern const Nversion OpenGL_1_5;                  // OpenGL 1.5    - 29.07.2003 / 30.10.2003 spec
   extern const Nversion OpenGL_1_4;                  // OpenGL 1.4    - 24.07.2002 spec
   extern const Nversion OpenGL_1_3;                  // OpenGL 1.3    - 14.08.2001 spec
   extern const Nversion OpenGL_1_2_1;                // OpenGL 1.2.1  - 01.04.1999 spec
   extern const Nversion OpenGL_1_2;                  // OpenGL 1.2    -
   extern const Nversion OpenGL_1_1;                  // OpenGL 1.1    - 29.03.1997
   extern const Nversion OpenGL_1_0;                  // OpenGL 1.0    - 20.06.1992
   extern const Nversion OpenGL_Unsupported;          // For marking unsupported features

   // All queues support transfer operations.
   // If device support Sparse resources, Universal and Compute queues support Sparse Transfer as well. (can we make this assumption ?)
   // Except of Transfer, they sypport:
   enum class QueueType : uint32
      {
      Universal      = 0, // Supports both Rendering and Compute workloads
      Compute           , // Only Compute workloads
      Transfer          , // Only Transfer operations
      SparseTransfer    , // Only Transfer operations with support of Sparse Resource updates
      Count
      };
      
   // Per device context that can be used to perform operations on GPU
   class GpuDevice : public SafeObject<GpuDevice>
      {
      public:
      virtual uint32 displays(void) const = 0;            // Screens the device can render to
      virtual Ptr<Display> display(uint32 id) const = 0;  // Return N'th display handle


      virtual Ptr<Window> createWindow(const WindowSettings& settings,  
                                       const string title) = 0; // Create window
               
      // Create Heap from which GPU resources can be sub-allocated.
      virtual Ptr<Heap> createHeap(const MemoryUsage usage, const uint32 size) = 0;
      
      virtual Ptr<Sampler> createSampler(const SamplerState& state) = 0;
      
      // Buffers and Textures are allocated from the Heaps.
      
      // Create texture that can be shared between processes and API's through
      // shared backing surface. If not supported on given platform, returns nullptr.
      virtual Ptr<Texture> createSharedTexture(Ptr<SharedSurface> backingSurface) = 0;
      
      // TODO:
      // Vulkan - entrypoint is specified at Pipeline creation I guess
      // Metal  - has libraries, from which we pick functions as entry points
      virtual Ptr<Shader> createShader(const ShaderStage stage,
                                       const string& source) = 0;

      virtual Ptr<Shader> createShader(const ShaderStage stage,
                                       const uint8* data,
                                       const uint64 size) = 0;
      
      
      // Returns count of available Command Queues of given type
      virtual uint32 queues(const QueueType type) const = 0;
      
      // Creates Command Buffer from the given Command Queue of given type.
      // When this buffer is commited for execution it will execute on that queue.
      virtual Ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                     const uint32 parentQueue = 0u) = 0;


      // Creates empty input layout for Programmable Vertex Fetch.
      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                 const uint32 controlPoints = 0u) = 0;

      // Creates InputLayout description based on single Vertex buffer.
      // Buffer needs to have specified internal formatting.
      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                 const uint32 controlPoints,
                                                 const Ptr<Buffer> buffer) = 0;

      // Specialized function for creation of any type of InputAssember description.
      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
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
      virtual Ptr<SetLayout> createSetLayout(const uint32 count, 
                                             const ResourceGroup* group,
                                             const ShaderStages stagesMask = ShaderStages::All) = 0;

      virtual Ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                       const Ptr<SetLayout>* set,
                                                       const uint32 immutableSamplers = 0u,
                                                       const Ptr<Sampler>* sampler = nullptr,
                                                       const ShaderStages stagesMask = ShaderStages::All) = 0;

      virtual Ptr<Descriptors> createDescriptorsPool(const uint32 maxSets, 
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
 
      // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
      // similarly when binding CubeMap texture, pass it's "face" through "layer".
      virtual Ptr<ColorAttachment> createColorAttachment(const Format format, 
                                                         const uint32 samples = 1u) = 0;

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Format depthFormat, 
                                                                       const Format stencilFormat = Format::Unsupported,
                                                                       const uint32 samples = 1u) = 0;

      // Creates render pass with Swap-Chain surface as destination.
      // Swap-Chain surface may be destination of MSAA resolve operation.
      virtual Ptr<RenderPass> createRenderPass(const Ptr<ColorAttachment> swapChainSurface,
                                               const Ptr<DepthStencilAttachment> depthStencil) = 0;

      // Creates render pass 
      virtual Ptr<RenderPass> createRenderPass(const uint32 attachments,
                                               const Ptr<ColorAttachment>* color,
                                               const Ptr<DepthStencilAttachment> depthStencil) = 0;

      virtual Ptr<Semaphore> createSemaphore(void) = 0;

      






 
      virtual Ptr<RasterState>        createRasterState(const RasterStateInfo& state) = 0;

      virtual Ptr<MultisamplingState> createMultisamplingState(const uint32 samples,
                                                               const bool enableAlphaToCoverage,
                                                               const bool enableAlphaToOne) = 0;

      virtual Ptr<DepthStencilState>  createDepthStencilState(const DepthStencilStateInfo& desc) = 0;

      virtual Ptr<BlendState>         createBlendState(const BlendStateInfo& state,
                                                       const uint32 attachments,
                                                       const BlendAttachmentInfo* color) = 0;
      
      virtual Ptr<ViewportState>      createViewportState(const uint32 count,
                                                          const ViewportStateInfo* viewports,
                                                          const ScissorStateInfo* scissors) = 0;
 
      // Returns default Pipeline state helper structure, that can be easily
      // modified and passed to Pipeline object creation call. All states are
      // set to their defaults. Input Layout is set to TrnagleStripes, and no
      // input attributes are specified. Pipeline Layout also assumes no 
      // resources are used.
      // App still needs to set Viewport State and assign Shaders.
      virtual PipelineState defaultPipelineState(void) = 0;

      virtual Ptr<Pipeline> createPipeline(const PipelineState& pipelineState) = 0;


      // Capabilities query
      //-------------------
      
      // Size of texel in bytes, based on the given format. For compressed formats, it's texel block size.
      virtual uint32 texelSize(const Format format) = 0;


      virtual ~GpuDevice() {};                            // Polymorphic deletes require a virtual base destructor
      };
      
   // Per graphic API context, initialized depending on API choosed at runtime
   class GraphicAPI : public SafeObject<GraphicAPI>
      {
      public:
      
      static bool create(void);                           // Creates instance of this class (API specific) and assigns it to "Graphics".
      
      virtual uint32 devices(void) const = 0;
      virtual Ptr<GpuDevice> primaryDevice(void) const = 0;

      virtual uint32 displays(void) const = 0;
      virtual Ptr<Display> primaryDisplay(void) const = 0;
      virtual Ptr<Display> display(uint32 index) const = 0;

      virtual ~GraphicAPI() {};                           // Polymorphic deletes require a virtual base destructor
      };



   }

extern Ptr<gpu::GraphicAPI> Graphics;
}


//      // Create Buffer formatted for storing array of structures (AOS). Each element of array is a tightly 
//      // packed structure containing up to MaxInputLayoutAttributesCount of variables. Each such variable 
//      // can be treated as a column and each element as a row in data array. 
//      // Each column has it's specified format, and can be a scalar or vector containing up to 4 channels. 
//      // Elements creating array can be used to store for e.g. Vertices, Control Points, or other data.
//      // When assigned to InputLayout as one of the buffers for processing, optional update rate can be 
//      // specified to describe how often InputLayout should switch to next element. By default it's set  
//      // to 0 which means buffer will be iterated on per vertex rate. If value is greater, it describes 
//      // by how many Draw Instances each structured element is shared, before Input Assembler should 
//      // proceed to next one. 
//      virtual Ptr<Buffer>  create(const BufferState& state, 
//                                  const Formatting& formatting, 
//                                  const uint32 step = 0) = 0;


#endif
