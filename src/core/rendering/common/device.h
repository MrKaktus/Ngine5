/*

 Ngine v5.0
 
 Module      : Common GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_DEVICE
#define ENG_CORE_RENDERING_COMMON_DEVICE

#include <bitset>
using namespace std;

#include "core/rendering/device.h"
#include "core/rendering/common/inputLayout.h"
#include "utilities/Nversion.h"
#include "threading/mutex.h"

// TODO: Move it to Thread Pool Scheduler
#define MaxSupportedWorkerThreads 64
#define MaxCommandBuffersExecuting 32

namespace en
{
   namespace gpu
   {
   class CommonDisplay : public Display
      {
      public:
      uint32v2  _position;          // Upper-Left corner position on virtual desktop
      uint32v2  _resolution;        // Native resolution
      uint32v2  observedResolution; // Display resolution when app started
      uint32v2* modeResolution;     // Resolutions of display modes supported by this display
      uint32    modesCount;         // Count of display modes supported by this display (from the list of modes supported by the driver)

      CommonDisplay();

      virtual uint32v2 position(void);    // Position on Virtual Desktop
      virtual uint32v2 resolution(void);  // Native resolution

      virtual ~CommonDisplay();
      };
      
   class CommonWindow : public Window
      {
      public:
      Ptr<CommonDisplay> _display;
      uint32v2 _position;
      uint32v2 _size;
      uint32v2 _resolution;
      Nmutex   surfaceAcquire;   // Window instance mutex.
      WindowMode _mode;          // Windowed / Borderless / Fullscreen
      bool     needNewSurface;

      CommonWindow();
      
      virtual Ptr<Display> display(void) const;   // Display on which window's center point is currently located
      virtual uint32v2 position(void) const;
      virtual uint32v2 size(void) const;
      virtual uint32v2 resolution(void) const;
      
      virtual void transparent(const float opacity); // TODO: Do we really want that here? (Transp. should be enabled on window creation time, and queried later)
      virtual void opaque(void);
      //virtual Ptr<Texture> surface(const Ptr<Semaphore> signalSemaphore = nullptr); // App should query for current surface each time it wants to reference it
      //virtual void present(const Ptr<Semaphore> waitForSemaphore = nullptr);
      
      virtual ~CommonWindow();
      };

   class CommonDevice : public GpuDevice
      {
      public:
      CommonDevice();
     ~CommonDevice();
     
      // Graphic API version supported by this device
      Nversion       api;
      PipelineState* defaultState;

      // GPU HW and API dependent capabilities
      struct Support
         {
         // Formats
         bitset<underlyingType(Attribute::Count)> attribute; // Input Assembler Attribute Formats
         bitset<underlyingType(Format::Count)> sampling;     // Texel Formats - Sampling support
         bitset<underlyingType(Format::Count)> rendering;    // Texel Formats - Rendering support
            
         // Input Assembler
         uint8       maxInputLayoutAttributesCount;  // Maximum number of input attributes

         // Texture
         uint32      maxTextureSize;                    // Maximum 2D/1D texture image dimension
         uint32      maxTextureRectSize;                // Maximum size of rectangle texture dimmension
         uint32      maxTextureCubeSize;                // Maximum cube map texture image dimension
         uint32      maxTexture3DSize;                  // Maximum 3D texture image dimension
         uint32      maxTextureLayers;                  // Maximum number of layers for texture arrays
         uint32      maxTextureBufferSize;              // No. of addressable texels for buffer textures
         float       maxTextureLodBias;                 // Maximum absolute texture level of detail bias

         // Sampler
         float       maxAnisotropy;                     // Maximum anisotropic filtering factor
         
         // Rasterizer
         uint32      maxColorAttachments;               // Maximum number of color renderable textures
         
         
      // TODO: Add support.maxViewports , support.maxScissors

//         uint32      maxFramebufferColorAttachments; // Number of Framebuffer Color attachments
//         uint32      maxRenderTargets;               // Maximum supported Render Targets count            
//         uint32      maxClipDistances;               // Maximum number of user clipping planes 
//         uint32      subpixelBits;                   // Number of bits of subpixel precision in screen xw and yw
//
//         uint32      maxTextureUnits;                // Maximum number of Texture Units
//         uint32      maxRenderbufferSize;            // Maximum width and height of renderbuffers
//         uint32      maxViewportWidth;               // Maximum viewport dimensions 
//         uint32      maxViewportHeight;              // 
//         uint32      maxViewports;                   // Maximum number of active viewports
//         uint32      viewportSubpixelBits;           // No. of bits of subpixel precision for viewport bounds
//         float       viewportBoundsRange;            // Viewport bounds range [min; max] (at least [-32768; 32767])
//         uint32      layerProvokingVertex;           // Vertex convention followed by gl_Layer
//         uint32      viewportProvokingVertex;        // Vertex convention followed by gl_ViewportIndex
//         float       pointSizeMin;                   // Range (lo to hi) of point sprite sizes
//         float       pointSizeMax;                   //
//         float       pointSizeGranularity;           // Point sprite size granularity
//         float       aliasedLineWidthMin;            // Range (lo to hi) of aliased line widths
//         float       aliasedLineWidthMax;            //
//         float       antialiasedLineWidthMin;        // Range (lo to hi) of antialiased line widths 
//         float       antialiasedLineWidthMax;        //
//         float       antialiasedLineGranularity;     // Antialiased line width granularity 
//         uint32      maxElementIndices;              // Recommended max. number of DrawRangeElements indices
//         uint32      maxElementVerices;              // Recommended max. number of DrawRangeElements vertices
//         uint32      compressedTextureFormats;       // Enumerated compressed texture formats                   !!!!! TODO !!!!!!!
//         uint32      numCompressedTextureFormats;    // Number of compressed texture formats 
//
//         uint32      maxRectTextureSize;             // Max. width & height of rectangular textures 
//         uint32      programBinaryFormats;           // Enumerated program binary formats                       !!!!! TODO !!!!!!!
//         uint32      numProgramBinaryFormats;        // Number of program binary formats
//         uint32      shaderBinaryFormats;            // Enumerated shader binary formats                        !!!!! TODO !!!!!!!
//         uint32      numShaderBinaryFormats;         // Number of shader binary formats 
//         bool        shaderCompiler;                 // Shader compiler supported
//         uint32      minMapBufferAligment;           // Min. byte alignment of pointers returned by Map*Buffer
//         uint32      maxPatchSize;                   // Maximum number of Control Points in input tessellation patch


         } support;
         
      virtual void init(void);

      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                 const uint32 controlPoints = 0u);

      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                 const uint32 controlPoints,
                                                 const Ptr<Buffer> buffer);
         
      // Needs to be Declared and Defined to allow calls to it from itself
      //virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
      //                                              const uint32 controlPoints,
      //                                              const uint32 usedAttributes,
      //                                              const uint32 usedBuffers,
      //                                              const AttributeDesc* attributes,
      //                                              const BufferDesc* buffers);
         
      virtual PipelineState defaultPipelineState(void);

      virtual uint32 texelSize(const Format format);
      };
   }
}

#endif
