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

#include "core/rendering/inputAssembler.h"
#include "core/rendering/blend.h"
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

   class Display : public SafeObject<Display>
      {
      public:

      virtual uint32v2 position(void) = 0;   // Position on Virtual Desktop
      virtual uint32v2 resolution(void) = 0; // Native resolution

      virtual ~Display() {};   // Polymorphic deletes require a virtual base destructor
      };

   enum WindowMode
      {
      Windowed          = 0,   // Create Window
      BorderlessWindow     ,   // Create borderless Window
      Fullscreen               // Switch given display to full screen mode
      };
      
   struct WindowSettings
      {
      WindowMode   mode;       // Mode in which we create surface (BorderlessWindow by default).
      Ptr<Display> display;    // Display on which window will be created, if not specified, primary display is selected.
      uint32v2     position;   // Position on the display in pixels from Upper-Left corner. Ignored in Fullscreen mode.
      uint32v2     size;       // Window size in pixels of the screen native resolution (if zeros are set, native
                               // resolution will be assumed). In Fullscreen mode, if size is set, it need to match
                               // one of resolutions supported by the display (if it's smaller than native resolution,
                               // Display will change resolution and use native HW scaler).
                               // Swap-Chain properties:
      Format       format;     // Pixel Format for backing surfaces. (Default Format::RGBA_8)
      uint32v2     resolution; // Destination surface resolution. If set to zeros, resolution will match window size
                               // (default state). Can be set to smaller resolution than window size if application
                               // wants to benefit from Windowing System scaler that will upsample the image (useful
                               // on high DPI displays, allows saving of memory needed for allocation of Swap-Chain
                               // surfaces, and GPU power needed to rasterize in high resolution).
                               // In Fullscreen mode, this field should be set to zeroes (default).
                               // You should use size instead to obtain the same results.

      WindowSettings();
      };

   class Window : public SafeObject<Window>
      {
      public:
      virtual Ptr<Display> display(void) const = 0;   // Display on which window's center point is currently located
      virtual uint32v2 position(void) const = 0;
      virtual uint32v2 size(void) const = 0;          // Size in displays native resolution pixels
      virtual uint32v2 resolution(void) const = 0;    // Resolution of backing image
      
      virtual bool movable(void) = 0;
      virtual void move(const uint32v2 position) = 0;
      virtual void resize(const uint32v2 size) = 0;
      virtual void active(void) = 0;
      virtual void transparent(const float opacity) = 0;
      virtual void opaque(void) = 0;
      virtual Ptr<Texture> surface(const Ptr<Semaphore> signalSemaphore = nullptr) = 0; // Signal when Swap-Chain surface is presented, and can be reused
      virtual void present(const Ptr<Semaphore> waitForSemaphore = nullptr) = 0;        // Wait for rendering to Swap-Chain surface being done
                                                                                        
      //virtual void present(void) = 0;                 // Presenting is always performed from first queue of type QueueType::Universal (queue 0).
      
      virtual ~Window() {};                               // Polymorphic deletes require a virtual base destructor
      };



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


      virtual Ptr<Window> create(const WindowSettings& settings,  
                                 const string title) = 0; // Create window
               
      // Create Heap from which GPU resources can be sub-allocated.
      virtual Ptr<Heap> createHeap(const MemoryUsage usage, const uint32 size) = 0;
      
      // Buffers and Textures are allocated from the Heaps.
      
      // Create texture that can be shared between processes and API's through
      // shared backing surface.
      virtual Ptr<Texture> createSharedTexture(Ptr<SharedSurface> backingSurface) = 0;
      
      // TODO:
      // Vulkan - entrypoint is specified at Pipeline creation I guess
      // Metal  - has libraries, from which we pick functions as entry points
      virtual Ptr<Shader> createShader(const ShaderStage stage,
                                       const string& source) = 0;
      
      
      // Returns count of available Command Queues of given type
      virtual uint32 queues(const QueueType type) const = 0;
      
      // Creates Command Buffer from the given Command Queue of given type.
      // When this buffer is commited for execution it will execute on that queue.
      virtual Ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                     const uint32 parentQueue = 0u) = 0;

      // Creates InputAssembler description based on single Vertex buffer.
      // Buffer needs to have specified internal formatting.
      virtual Ptr<InputAssembler> create(const DrawableType primitiveType,
                                         const uint32 controlPoints,
                                         const Ptr<Buffer> buffer) = 0;

      // Specialized function for creation of any type of InputAssember description.
      virtual Ptr<InputAssembler>  create(const DrawableType primitiveType,
                                          const uint32 controlPoints,
                                          const uint32 usedAttributes,
                                          const uint32 usedBuffers,
                                          const AttributeDesc* attributes,
                                          const BufferDesc* buffers) = 0;

      virtual Ptr<SetLayout> createSetLayout(const uint32 count, 
                                             const ResourceGroup* group,
                                             const ShaderStage stageMask) = 0;

      virtual Ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                       const Ptr<SetLayout>* set,
                                                       const uint32 immutableSamplers = 0u,
                                                       const Ptr<Sampler>* sampler = nullptr) = 0;

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
      
      virtual Ptr<ViewportState>      create(const uint32 count,
                                             const ViewportStateInfo* viewports,
                                             const ScissorStateInfo* scissors) = 0;

      // Returns default Pipeline state helper structure, that can be easily
      // modified and passed to Pipeline object creation call.
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
//      // packed structure containing up to MaxInputAssemblerAttributesCount of variables. Each such variable 
//      // can be treated as a column and each element as a row in data array. 
//      // Each column has it's specified format, and can be a scalar or vector containing up to 4 channels. 
//      // Elements creating array can be used to store for e.g. Vertices, Control Points, or other data.
//      // When assigned to InputAssembler as one of the buffers for processing, optional update rate can be 
//      // specified to describe how often InputAssembler should switch to next element. By default it's set  
//      // to 0 which means buffer will be iterated on per vertex rate. If value is greater, it describes 
//      // by how many Draw Instances each structured element is shared, before Input Assembler should 
//      // proceed to next one. 
//      virtual Ptr<Buffer>  create(const BufferState& state, 
//                                  const Formatting& formatting, 
//                                  const uint32 step = 0) = 0;


#endif
