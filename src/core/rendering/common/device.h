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

#include "core/rendering/device.h"
#include "core/rendering/common/inputLayout.h"
#include "core/rendering/common/display.h"
#include "utilities/Nversion.h"
#include "platform/system.h"
#include "threading/mutex.h"

// TODO: Move it to Thread Pool Scheduler
#define MaxSupportedWorkerThreads  64
#define MaxCommandBuffersExecuting 32
#define MaxCommandBuffersAllocated 32
#define AllocatorCacheSize         2

namespace en
{
namespace gpu
{      
class CommonDevice : public GpuDevice
{
    public:
    CommonDevice();
   ~CommonDevice();
   
    /// Graphic API version supported by this device
    Nversion       api;
    PipelineState* defaultState;

    /// GPU HW and API dependent capabilities
    struct Support
    {
        // Formats
        std::bitset<underlyingType(Attribute::Count)> attribute; ///< Input Assembler Attribute Formats
        std::bitset<underlyingType(Format::Count)> sampling;     ///< Texel Formats - Sampling support
        std::bitset<underlyingType(Format::Count)> rendering;    ///< Texel Formats - Rendering support
           
        // Memory
        uint64      videoMemorySize;                   ///< Dedicated VRAM on NUMA
        uint64      systemMemorySize;                  ///< GPU accessible system memory
        
        // Input Assembler
        uint8       maxInputLayoutBuffersCount;        ///< Maximum number of Vertex buffers that can be bound
        uint8       maxInputLayoutAttributesCount;     ///< Maximum number of input attributes
        
        // Texture
        uint32      maxTextureSize;                    ///< Maximum 2D/1D texture image dimension
        uint32      maxTextureCubeSize;                ///< Maximum cube map texture image dimension
        uint32      maxTexture3DSize;                  ///< Maximum 3D texture image dimension
        uint32      maxTextureLayers;                  ///< Maximum number of layers for texture arrays
        uint32      maxTextureBufferSize;              ///< No. of addressable texels for buffer textures
        float       maxTextureLodBias;                 ///< Maximum absolute texture level of detail bias
        
        // Sampler
        float       maxAnisotropy;                     ///< Maximum anisotropic filtering factor
        
        // Rasterizer
        uint32      maxColorAttachments;               ///< Maximum number of color renderable textures
       
       
    // TODO: Add support.maxViewports , support.maxScissors

//      uint32      maxFramebufferColorAttachments; // Number of Framebuffer Color attachments
//      uint32      maxRenderTargets;               // Maximum supported Render Targets count            
//      uint32      maxClipDistances;               // Maximum number of user clipping planes 
//      uint32      subpixelBits;                   // Number of bits of subpixel precision in screen xw and yw
//
//      uint32      maxTextureUnits;                // Maximum number of Texture Units
//      uint32      maxRenderbufferSize;            // Maximum width and height of renderbuffers
//      uint32      maxViewportWidth;               // Maximum viewport dimensions 
//      uint32      maxViewportHeight;              // 
//      uint32      maxViewports;                   // Maximum number of active viewports
//      uint32      viewportSubpixelBits;           // No. of bits of subpixel precision for viewport bounds
//      float       viewportBoundsRange;            // Viewport bounds range [min; max] (at least [-32768; 32767])
//      uint32      layerProvokingVertex;           // Vertex convention followed by gl_Layer
//      uint32      viewportProvokingVertex;        // Vertex convention followed by gl_ViewportIndex
//      float       pointSizeMin;                   // Range (lo to hi) of point sprite sizes
//      float       pointSizeMax;                   //
//      float       pointSizeGranularity;           // Point sprite size granularity
//      float       aliasedLineWidthMin;            // Range (lo to hi) of aliased line widths
//      float       aliasedLineWidthMax;            //
//      float       antialiasedLineWidthMin;        // Range (lo to hi) of antialiased line widths 
//      float       antialiasedLineWidthMax;        //
//      float       antialiasedLineGranularity;     // Antialiased line width granularity 
//      uint32      maxElementIndices;              // Recommended max. number of DrawRangeElements indices
//      uint32      maxElementVerices;              // Recommended max. number of DrawRangeElements vertices
//      uint32      compressedTextureFormats;       // Enumerated compressed texture formats                   !!!!! TODO !!!!!!!
//      uint32      numCompressedTextureFormats;    // Number of compressed texture formats 
//
//      uint32      maxRectTextureSize;             // Max. width & height of rectangular textures 
//      uint32      programBinaryFormats;           // Enumerated program binary formats                       !!!!! TODO !!!!!!!
//      uint32      numProgramBinaryFormats;        // Number of program binary formats
//      uint32      shaderBinaryFormats;            // Enumerated shader binary formats                        !!!!! TODO !!!!!!!
//      uint32      numShaderBinaryFormats;         // Number of shader binary formats 
//      bool        shaderCompiler;                 // Shader compiler supported
//      uint32      minMapBufferAligment;           // Min. byte alignment of pointers returned by Map*Buffer
//      uint32      maxPatchSize;                   // Maximum number of Control Points in input tessellation patch


    } support;
         
    virtual void init(void);

    virtual void cleanupCommonResources(void);

    virtual uint32 displays(void) const;

    virtual std::shared_ptr<Display> display(const uint32 index) const;

    virtual InputLayout* createInputLayout(
        const DrawableType primitiveType,
        const uint32 controlPoints = 0u);

    virtual InputLayout* createInputLayout(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const Buffer& buffer);
       
  // Needs to be Declared and Defined to allow calls to it from itself
  //virtual InputLayout* createInputLayout(
  //    const DrawableType primitiveType,
  //    const uint32 controlPoints,
  //    const uint32 usedAttributes,
  //    const uint32 usedBuffers,
  //    const AttributeDesc* attributes,
  //    const BufferDesc* buffers);
       
    virtual PipelineState defaultPipelineState(void);

    virtual uint64 dedicatedMemorySize(void);
    virtual uint64 systemMemorySize(void);
    virtual uint32 texelSize(
        const Format format,
        const uint8  plane = 0) const;
};

// CompileTimeSizeReporting( CommonDevice );
static_assert(sizeof(CommonDevice) == 152, "en::gpu::CommonDevice size mismatch!"); // 144 + padding


class CommonGraphicAPI : public GraphicAPI
{
    public:
    /// API Independent, OS Dependent - Windowing System
    std::shared_ptr<CommonDisplay>* displayArray;
    std::shared_ptr<CommonDisplay>  virtualDisplay;
    uint32                     displaysCount;
    uint32                     displayPrimary;

    public:
    CommonGraphicAPI();
    virtual ~CommonGraphicAPI();
};

} // en::gpu
} // en

#endif
