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
   extern const Nversion Vulkan_1_0;                  // Vulkan 1.0
   
   extern const Nversion Metal_OSX_1_0;               // Metal OSX 1.0
   extern const Nversion Metal_OSX_Unsupported;       // For marking unsupported features
   
   extern const Nversion Metal_IOS_1_0;               // Metal 1.0
   extern const Nversion Metal_IOS_Unsupported;       // For marking unsupported features



   class Screen : public SafeObject
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

   class Window : public SafeObject
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

   class CommandBuffer : public SafeObject
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
   class GpuDevice : public SafeObject
      {
      public:
      virtual uint32 screens(void) = 0;                 // Screens the device can render to
      virtual Ptr<Screen> screen(uint32 id) const = 0;  // Return N'th screen handle
      virtual Ptr<class Window> create(const WindowSettings& settings,  // TODO: remove "class" when old Rendering API Abstraction is removed
                                 const string title) = 0; // Create window
         
      virtual Ptr<Texture> create(const TextureState state) = 0;
      
      virtual Ptr<CommandBuffer> createCommandBuffer(void) = 0; // Create command buffer

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

      // Creates InputAssembler description based on single buffer attributes
      virtual Ptr<InputAssembler> create(const Ptr<BufferView> buffer) = 0;

      // Creates InputAssembler description combining attributes from several buffers
      virtual Ptr<InputAssembler> create(const InputAssemblerSettings& attributes) = 0;

      virtual Ptr<DepthStencilState>  create(const DepthStencilStateInfo& desc) = 0;
      virtual Ptr<MultisamplingState> create(const uint32 samples,
                                             const bool enableAlphaToCoverage,
                                             const bool enableAlphaToOne) = 0;

      virtual ~GpuDevice() {};                            // Polymorphic deletes require a virtual base destructor
      };
      
   // Per graphic API context, initialized depending on API choosed at runtime
   class GraphicAPI : public SafeObject
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

#endif