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
#include "core/rendering/texture.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/multisampling.h"

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

   class Screen : public SafeObject<Screen>
      {
      public:
      uint32v2 resolution;  // Native resolution
      uint32v2 position;    // Upper-Left corner position on virtual desktop
      
      Screen();
      virtual ~Screen() {}; // Polymorphic deletes require a virtual base destructor
      };
      
   enum WindowMode
      {
      Windowed          = 0,   // Create Window
      BorderlessWindow     ,   // Create borderless Window
      Fullscreen               // Create full screen surface
      };
      
   struct WindowSettings
      {
      Ptr<Screen> screen;    // Screen on which window will be created, if not specified, primary screen is choosed.
      uint32v2    position;  // Position on the screen, by default Upper-Left corner.
      uint32v2    size;      // Window size in pixels of the screen (if zeroes set, native resolution will be set)
      WindowMode  mode;      // Mode in which we create surface (BorderlessWindow by default).
      
      WindowSettings();
      };

   class Window : public SafeObject<class Window>
      {
      public:
      virtual Ptr<Screen> screen(void) = 0;   // Screen on which window's center point is currently located
      virtual uint32v2 position(void) = 0;
      virtual bool movable(void) = 0;
      virtual void move(const uint32v2 position) = 0;
      virtual void resize(const uint32v2 size) = 0;
      virtual void active(void) = 0;
      virtual void transparent(const float opacity) = 0;
      virtual void opaque(void) = 0;
      virtual Ptr<Texture> surface(void) = 0; // App should query for current surface each time it wants to reference it
      virtual void display(void) = 0;
      
      virtual ~Window() {};                               // Polymorphic deletes require a virtual base destructor
      };

   class CommandBuffer : public SafeObject<CommandBuffer>
      {
      public:
      virtual void bind(const Ptr<RasterState> raster) = 0;
      //virtual void bind(const Ptr<ViewportScissorState> viewportScissor) = 0;
      virtual void bind(const Ptr<DepthStencilState> depthStencil) = 0;
      virtual void bind(const Ptr<BlendState> blend) = 0;

      virtual bool startRenderPass(const Ptr<RenderPass> pass) = 0;
      virtual void set(const Ptr<Pipeline> pipeline) = 0;
      virtual bool endRenderPass(void) = 0;
      virtual void commit(void) = 0;
      virtual void waitUntilCompleted(void) = 0;
      
      virtual ~CommandBuffer() {};                        // Polymorphic deletes require a virtual base destructor
      };
      
   // Per device context that can be used to perform operations on GPU
   class GpuDevice : public SafeObject<GpuDevice>
      {
      public:
      virtual uint32 screens(void) = 0;                 // Screens the device can render to
      virtual Ptr<Screen> screen(uint32 id) const = 0;  // Return N'th screen handle
      virtual Ptr<class Window> create(const WindowSettings& settings,  // TODO: remove "class" when old Rendering API Abstraction is removed
                                 const string title) = 0; // Create window
         
         


      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Formatting& formatting,
                                 const uint32 step = 0,
                                 const void* data = nullptr) = 0;
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Attribute format,
                                 const void* data = nullptr) = 0;

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> create(const BufferType type,
                                 const uint32 size,
                                 const void* data = nullptr) = 0;
      
      
      
      
      virtual Ptr<Texture> create(const TextureState state) = 0;
      
      virtual Ptr<CommandBuffer> createCommandBuffer(void) = 0; // Create command buffer

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
                                          const BufferDesc* buffers);


      // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
      // similarly when binding CubeMap texture, pass it's "face" through "layer".
      virtual Ptr<ColorAttachment> createColorAttachment(const Ptr<Texture> texture,
                                          const uint32 mipmap = 0,
                                          const uint32 layer = 0,
                                          const uint32 layers = 1) = 0;

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Ptr<Texture> depth,
                                                 const Ptr<Texture> stencil,
                                                 const uint32 mipmap = 0,
                                                 const uint32 layer = 0,
                                                 const uint32 layers = 1) = 0;

      // Creates simple render pass with one color destination
      virtual Ptr<RenderPass> create(const Ptr<ColorAttachment> color,
                                     const Ptr<DepthStencilAttachment> depthStencil) = 0;
      
      // Creates render pass with Multiple Render Targets
      virtual Ptr<RenderPass> create(const uint32 _attachments,
                                     const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                     const Ptr<DepthStencilAttachment> depthStencil) = 0;
        
      // Creates render pass which's output goes to window framebuffer
      virtual Ptr<RenderPass> create(const Ptr<Texture> framebuffer,
                                     const Ptr<DepthStencilAttachment> depthStencil) = 0;
      
      // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
      virtual Ptr<RenderPass> create(const Ptr<Texture> temporaryMSAA,
                                     const Ptr<Texture> framebuffer,
                                     const Ptr<DepthStencilAttachment> depthStencil) = 0;

      virtual Ptr<DepthStencilState>  create(const DepthStencilStateInfo& desc) = 0;
      virtual Ptr<MultisamplingState> create(const uint32 samples,
                                             const bool enableAlphaToCoverage,
                                             const bool enableAlphaToOne) = 0;

      virtual ~GpuDevice() {};                            // Polymorphic deletes require a virtual base destructor
      };
      
   // Per graphic API context, initialized depending on API choosed at runtime
   class GraphicAPI : public SafeObject<GraphicAPI>
      {
      public:
      
      static bool create(void);                           // Creates instance of this class (API specific) and assigns it to "Graphics".
      
      virtual uint32 devices(void) = 0;
      virtual Ptr<GpuDevice> primaryDevice(void) = 0;
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
