/*

 Ngine v5.0
 
 Module      : Rendering.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_CONTEXT
#define ENG_CORE_RENDERING_CONTEXT

#if !defined(EN_PLATFORM_OSX)

#include "core/defines.h"
#include "core/types.h"

#if defined(EN_PLATFORM_ANDROID)
#include <android/native_activity.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>                // OpenGL ES 2.0
#include <GLES2/gl2ext.h>             // OpenGL ES Extensions 
#include "GLES3/gl3.h"                // OpenGL ES 3.0
#include "GLES3/gl3platform.h"        // OpenGL ES 3.0 Platform-Dependent Macros
#endif

#if defined(EN_PLATFORM_BLACKBERRY)
#include <screen/screen.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <EGL/egl.h>
#include <GLES/gl.h>                  // OpenGL ES 
#include <GLES/glext.h>               // OpenGL ES Extensions 
#include <GLES2/gl2.h>                // OpenGL ES 2.0
#include <GLES2/gl2ext.h>             // OpenGL ES 2.0 Extensions 
#include "GLES3/gl3.h"                // OpenGL ES 3.0
#include "GLES3/gl3platform.h"        // OpenGL ES 3.0 Platform-Dependent Macros
#endif

#if defined(EN_PLATFORM_IOS)
#import  <QuartzCore/QuartzCore.h>
#import  <OpenGLES/ES2/gl.h>          // OpenGL ES 2.0
#import  <OpenGLES/ES2/glext.h>       // OpenGL ES Extensions 
#import  <OpenGLES/EAGL.h>
#import  <OpenGLES/EAGLDrawable.h>
#endif

#if defined(EN_PLATFORM_OSX)
#include <OpenGL/gl3.h>               // OpenGL 3.0+
#include <OpenGL/gl3ext.h>            // OpenGL 3.0+ Extensions
#endif


#if !defined(EN_PLATFORM_OSX)


#include "core/rendering/opengl/opengl.h"
#include "core/rendering/rendering.hpp"

// Go to:
// https://www.khronos.org/registry/implementers_guide.html
// for headers list per API
//#include "glcorearb.h"                // OpenGL Core and Extensions

#include "core/rendering/opengl/extensions.h" // Include for all OS'es for extension names


#include <set>
#include <map>
using namespace std;

#include "core/configuration.h"              // For EN_MAX_ARRAY_SIZE
#include "core/utilities/TarrayAdvanced.h"
#include "threading.h"
using namespace en::threads;

#define GPU_IMMEDIATE_MODE 

#include "core/rendering/common/sampler.h"

namespace en
{
   namespace gpu
   {


//   // GPU Buffer Column Descriptor
//   struct cachealign BufferColumnDescriptor
//          {
//          Attribute type;                   // Column type
//          char*      name;                   // Local copy of name string
//
//          BufferColumnDescriptor();
//         ~BufferColumnDescriptor();
//          };

//   // GPU Data Block Descriptor
//   struct cachealign DataBlockDescriptor
//          {
//          uint32 size;                       // Size of mapped block in bytes
//          void*  pointer;                    // Pointer to local memory
//                                             
//          DataBlockDescriptor();                 // Constructor
//         ~DataBlockDescriptor();
//          };

//   // GPU Buffer Descriptor
//   class cachealign BufferDescriptor : public ProxyObject
//          {    
//          public:  
//          BufferColumnDescriptor column[16]; // Vector describing each column of buffer
//          uint64     offset[16];             // Offset of each column in row
//          BufferType type;                   // Buffer type (VBO, IBO, ...)
//          uint32     elements;               // Rows/elements in buffer 
//          uint32     columns;                // Active columns
//          uint32     rowSize;                // Size in bytes of one row/element
//          uint32     size;                   // Whole buffer size
//          DataBlockDescriptor* block;            // Pointer to descriptor of mapped block
//          uint32     id;                     // OpenGL handle
//          uint32     vao;                    // OpenGL VAO handle
//          bool       named;                  // Indicates that buffer uses names to match columns
//
//          BufferDescriptor();
//          };

   //// GPU Surface Descriptor
   //struct cachealign SurfaceDescriptor
   //       {
   //       uint32       size;            // Size of mapped surface in bytes
   //       uint16       face;            // CubeMap face number
   //       uint16       mipmap;          // MipMap level
   //       uint16       width;           // MipMap width
   //       uint16       height;          // MipMap height
   //       union {
   //             uint16 depth;           // Depth slice
   //             uint16 layer;           // Layer number
   //             };
   //       void*        pointer;         // Pointer to local memory (if PBO's are not supported)

   //       SurfaceDescriptor();          // Constructor
   //      ~SurfaceDescriptor();
   //       }; 

   //// GPU Texture Descriptor
   //class cachealign TextureDescriptor : public ProxyObject
   //       {
   //       public:
   //       TextureType   type;           // Texture type
   //       TextureFormat format;         // Texture format 
   //       uint16        width;
   //       uint16        height;
   //       uint16        depth;
   //       uint16        layers;
   //       uint16        samples;        // For multisampled textures
   //       uint16        mipmaps;
   //       SurfaceDescriptor* surface;   // Pointer to descriptor of mapped surface 
   //       uint32        id;             // OpenGL handle
   //       uint32        pbo;            // OpenGL PBO handle (OpenGL 2.1+)

   //       TextureDescriptor();          // Constructor
   //      ~TextureDescriptor(); 
   //       };

   //// GPU Sampler Unit Descriptor
   //struct cachealign SamplerUnitDescriptor
   //       {
   //       Texture texture; // Texture attached to sampler
   //       TextureFiltering magFilter; // Magnification filtering to use
   //       TextureFiltering minFilter; // Minification filtering to use
   //       TextureWraping wrapS;       // Wraping method in axis S/U
   //       TextureWraping wrapT;       // Wraping method in axis T/V
   //       TextureWraping wrapR;       // Wraping method in axis R/W
   //       union {
   //             struct {              // Bits indicating state change
   //                    uint32 magFilter : 1;
   //                    uint32 minFilter : 1;
   //                    uint32 wrapS     : 1;
   //                    uint32 wrapT     : 1;
   //                    uint32 wrapR     : 1;
   //                    };
   //             uint32 value;
   //             } dirty;
   //
   //       SamplerUnitDescriptor();
   //      ~SamplerUnitDescriptor();
   //       };

	class cachealign ProgramDescriptor;

   // GPU Input Descriptor
   struct cachealign InputDescriptor
          {
          ProgramDescriptor* program;   // Pointer to parent program
          char*         name;           // Name of input column
          uint32        elements;       // Elements count if parmeter is an array
          uint16        type;           // OpenGL input type
          uint16        location;       // OpenGL input location   

          InputDescriptor();
         ~InputDescriptor();
          };

   // GPU Parameter Descriptor
   class cachealign ParameterDescriptor : public ProxyObject
          {
          public:
          ProgramDescriptor* program;   // Pointer to parent program
          void*         value;          // Pointer to memory that stores value
          char*         name;           // Name of parameter
          uint32        elements;       // Elements count if parmeter is an array
          uint16        type;           // OpenGL parameter type
          sint16        location;       // OpenGL parameter location   
          bool          dirty;          // Update request flag

          ParameterDescriptor();
         ~ParameterDescriptor();
          };

   // GPU Uniform Block Descriptor
   class cachealign BlockDescriptor : public ProxyObject
          {
          public:
          ProgramDescriptor* program;   // Pointer to parent program
          Ptr<Buffer>        buffer;         // Buffer attached to uniform block
          char*         name;           // Name of parameter
          uint32        size;           // Size in bytes
          BufferType    type;           // Type of buffers supported by block (UBO or SSBO)
          uint16        id;             // OpenGL block index

          BlockDescriptor();
         ~BlockDescriptor();
          };

   // GPU Sampler Descriptor
   //class cachealign SamplerDescriptor : public ProxyObject
   //       {
   //       public:
   //       ProgramDescriptor* program;   // Pointer to parent program
   //       SamplerUnitDescriptor* unit;  // Pointer to sampler unit descriptor
   //       char*         name;           // Name of parameter
   //       uint32        elements;       // Elements count if this is array of samplers
   //       sint32        location;       // OpenGL sampler location in shader
   //       uint16        type;           // OpenGL sampler type
   //       bool          dirty;          // Update request flag

   //       SamplerDescriptor();
   //      ~SamplerDescriptor();
   //       };

   // GPU Shader Descriptor
   class cachealign ShaderDescriptor : public ProxyObject
          {
          public:
          PipelineStage stage;          // Stage to which shader refers
          uint32        id;             // OpenGL handle
          };


   struct SamplerHandle
      {
      Ptr<Texture> texture;
      Ptr<Sampler> sampler;
      sint32       location;   // OpenGL index in program parameters array
      uint16       type;       // OpenGL sampler type
#ifdef EN_DEBUG
      uint32       elements;   // Elements count if this is array of samplers (engine doesn't support samplers arrays)
      char*        name;       // Name of parameter    
#endif
      };


   struct TextureSamplerSet
      {
      Ptr<Texture> texture;    // Used texture
      Ptr<Sampler> sampler;    // Used sampler state
      uint16       location;   // Sampler parameter location in program
      uint16       slot;       // Texture Unit sampler parameter is bound to
      char*        name;       // Sampler name
      };

   // GPU Program Descriptor
   class cachealign ProgramDescriptor : public ProxyObject
          {
          public:
          struct Inputs
                 {
                 map<string, InputDescriptor*> names;  // Maps 'name' to input parameter 
                 InputDescriptor*              list;   // Input parameters descriptions
                 uint32                        count;  // Count of parameters
                 } inputs;

          struct Parameters 
                 {
                 map<string, ParameterDescriptor*> names;  // Maps 'name' to parameter 
                 ParameterDescriptor*              list;   // Parameters descriptions
                 ParameterDescriptor**             update; // List of parameters for update
                 uint32                            dirty;  // Count of dirty parameters
                 uint32                            count;  // Count of parameters
                 } parameters;

          struct Blocks 
                 {
                 map<string, BlockDescriptor*> names;  // Maps 'name' to block
                 BlockDescriptor*              list;   // Blocks descriptions
                 uint32                        count;  // Count of blocks
                 } blocks;

          TextureSamplerSet* sampler;
          uint32 samplers;

          //struct Samplers 
          //       {
          //       map<string, sint32> names;  // Maps 'name' to sampler
          ////       SamplerDescriptor*              list;   // Samplers descriptions
          ////       SamplerDescriptor**             update; // List of samplers for update
          ////       uint32                          dirty;  // Count of dirty samplers
          //       uint32              count;  // Count of samplers
          //       } samplers;

          uint32 id;                    // OpenGL handle

          ProgramDescriptor();
         ~ProgramDescriptor();
          };

      namespace egl
      {
      bool CreateDummyWindow(void);
      bool ScreenCreate(const ScreenSettings& settings, const string& title);
      bool ScreenDestroy(void);
      void Display(void);
      }

      namespace wgl
      {
      class WorkerContext : public Task
         { 
         private:
         ScreenSettings settings; 
         uint8          core;  
                          
         public:  
         WorkerContext(uint8 core, ScreenSettings settings);  
         void work(void);     
         }; 

      void BindFunctionsPointers(void);
      void BindWglFunctionsPointers(void);
      void UnbindFunctionsPointers(void);
      void UnbindWglFunctionsPointers(void);
      bool CreateDummyWindow(void);

void InitDisplaysInfo(void);

      bool ScreenCreate(const ScreenSettings& settings, const string& title);
      bool ScreenDestroy(void);
      void Display(void);
      }


      extern const uint16 TransalteCompareMethod[CompareMethodsCount];

      namespace gl
      {
      // Buffer Type
      struct cachealign BufferTypeTranslation
             {
             Nversion opengl;                // First OpenGL version that supports it
             uint16   type;                  // OpenGL buffer type
             };

      // Buffer Column
      struct cachealign BufferColumnTranslation
             {
             Nversion opengl;                // First OpenGL version that supports it
             uint16   size;                  // Column size in bytes
             uint16   channels;              // Channels count
             bool     normalized;            // Should data be normalized
             bool     integer;               // Is data integer
             bool     qword;                 // Is data 64bit per channel
             uint32   type;                  // OpenGL input type
             };

      //// Texture Type 
      //struct cachealign TextureTypeTranslation
      //       {
      //       Nversion opengl;                // First OpenGL version that supports it
      //       uint16   type;                  // OpenGL texture type
      //       };

      //// Texture Format 
      //struct cachealign TextureFormatTranslation
      //       {
      //       Nversion opengl;                // First OpenGL version that supports it
      //       bool     compressed;            // Is compressed
      //       uint16   srcBpp;                // Bits per texel, or bytes per block for compressed textures
      //       uint16   dstFormat;             // OpenGL sized internal format
      //       uint16   srcFormat;             // OpenGL input format
      //       uint16   srcType;               // OpenGL input type
      //       };

      //// Texture Filtering 
      //struct cachealign TextureFilteringTranslation
      //       {
      //       Nversion opengl;                // First OpenGL version that supports it
      //       bool     magnification;         // Can be used as magnification filtering
      //       uint8    anisotropic;           // Anisotropic ratio
      //       uint16   filtering;             // OpenGL filtering method
      //       }; 

      //// Texture Wraping
      //struct cachealign TextureWrapingTranslation
      //       {
      //       Nversion opengl;                // First OpenGL version that supports it
      //       uint16   wraping;               // OpenGL wraping method
      //       };

      // Pipeline Stage
      struct cachealign PipelineStageTranslation
             {
             Nversion opengl;                // First OpenGL version that supports it
             uint16   type;                  // OpenGL stage type
             };

      // Drawable Type
      struct cachealign DrawableTypeTranslation
             {
             Nversion opengl;                // First OpenGL version that supports it
             uint16   type;                  // OpenGL drawable type
             };

      // Blend Function 
      struct BlendFunctionTranslation
             {
             Nversion source;                // First OpenGL version that supports it as source function
             Nversion destination;           // First OpenGL version that supports it as destination function
             uint16   type;                  // OpenGL function type
             };

      // Support tables
      //extern bool BufferColumnSupported[ColumnTypesCount];
      //extern bool TextureTypeSupported[TextureTypesCount];
      //extern bool TextureFormatSupported[TextureFormatsCount];
      extern bool PipelineStageSupported[PipelineStagesCount];
      extern bool BlendSourceFunctionSupported[BlendFunctionsCount];
      extern bool BlendDestinationFunctionSupported[BlendFunctionsCount];

      // Translation tables
      //extern const BufferTypeTranslation       BufferType[BufferTypesCount];
      //extern const BufferColumnTranslation     TranslateAttribute[underlyingType(Attribute::Count)];
      //extern const TextureTypeTranslation      TextureType[TextureTypesCount];
      //extern const TextureFormatTranslation    TextureFormat[TextureFormatsCount];
      //extern const TextureFilteringTranslation TextureFiltering[TextureFilteringMethodsCount];
      //extern const TextureWrapingTranslation   TextureWraping[TextureWrapingMethodsCount];
      //extern const uint16                      TextureFace[TextureFacesCount];
      extern const PipelineStageTranslation    PipelineStage[PipelineStagesCount];
      extern const uint16                      StencilModification[StencilModificationsCount];
      extern const BlendFunctionTranslation    BlendFunction[BlendFunctionsCount];
      extern const uint16                      BlendEquation[BlendEquationsCount];
      extern const uint16                      Face[FaceChoosesCount];
      extern const uint16                      NormalCalculationMethod[NormalCalculationMethodsCount];
      extern const DrawableTypeTranslation     Drawable[DrawableTypesCount];

      bool IsError(const char* function);
#ifdef EN_DEBUG
      #define CheckError( x ) x; en::gpu::gl::IsError( #x );

      #ifdef EN_PROFILER_TRACE_GRAPHICS_API
      #define Profile( x )                        \
              {                                   \
              x;                                  \
              Log << "OpenGL: " << #x << endl;    \
              if (en::gpu::gl::IsError( #x ))     \
                 assert(0);                       \
              }
      #else 
      #define Profile( x )                        \
              {                                   \
              x;                                  \
              if (en::gpu::gl::IsError( #x ))     \
                 assert(0);                       \
              }
      #endif
#else
      #define CheckError( x ) x; /* Nothing in Release */
      #define Profile( x ) x; /* Nothing in Release */
#endif
      void GatherDeviceInformation(void);

      void GatherContextCapabilities(void);
      void StateInit(void);

      // Deffered state
      void CullingUpdate(void);
      void CullingFunctionUpdate(void);
      void CullingFaceUpdate(void);
      void ScissorRectUpdate(void);
      void ScissorUpdate(void);
      void DepthWritingUpdate(void);
      void DepthTestingUpdate(void);
      void DepthFunctionUpdate(void);
      void StencilWritingUpdate(void);
      void StencilTestingUpdate(void);
      void StencilOpsUpdate(void);
      void StencilFuncUpdate(void);
      void ColorWritingUpdate(void);
      void ClearUpdate(void);

      void BlendUpdate(void);
      void BlendColorUpdate(void);
      void BlendFuncUpdate(void);
      void BlendEquationUpdate(void);
      }

      namespace gl20
      {
      void BuffersInit(void);
      void BuffersClear(void);
      //bool SupportColumnType(const ColumnType type);
      //Ptr<Buffer> BufferCreate(const BufferSettings& bufferSettings, void* data);
      //extern void*  BufferMap(BufferDescriptor* buffer);
      //bool   BufferUnmap(BufferDescriptor* buffer);
      //bool   BufferDestroy(BufferDescriptor* const buffer);

      //void TexturesInit(void);
      //void TexturesClear(void);
      //bool SupportTextureType(const TextureType type);
      //bool SupportTextureFormat(const TextureFormat type);
      //Texture TextureCreate(const TextureState& textureState);
      //uint32  TextureSize(const TextureDescriptor* texture);
      //uint32  TextureSize(const TextureDescriptor* texture, const uint8 mipmap);
      //void*   TextureMap(TextureDescriptor* texture, const uint8 mipmap, const uint16 layer);
      //void*   TextureMap(TextureDescriptor* texture, const TextureFace face, const uint8 mipmap, const uint16 layer);
      //bool    TextureUnmap(TextureDescriptor* texture);
      //void    TextureRead(const TextureDescriptor* texture, uint8* buffer, const uint8 mipmap);
      //bool    TextureDestroy(TextureDescriptor* const texture);
      //bool    SamplerUnitUpdate(SamplerUnitDescriptor* sampler);
      //bool    SamplerDestroy(SamplerDescriptor* const sampler);
      
      void ProgramsInit(void);
      void ProgramsClear(void);
      Program ProgramCreate(const vector<Shader>& shaders);
      bool    ProgramDraw(ProgramDescriptor* program, const Ptr<Buffer> buffer, const Ptr<Buffer> indexBuffer, const DrawableType type, const uint32 patchSize, const uint32 inst);
      uint32  ProgramParameterSize(uint16 type);
      bool    ProgramParameterIsSampler(uint16 type);
      bool    ProgramParameterUpdate(ParameterDescriptor* parameter);
      bool    ProgramDestroy(ProgramDescriptor* const program);

      Shader  ShaderCreate(const PipelineStage stage, const string& code);
      Shader  ShaderCreateComposed(const PipelineStage stage, const vector<string>& code);
      bool    ShaderDestroy(ShaderDescriptor* const shader);
      bool    ParameterDestroy(ParameterDescriptor* const parameter);
      bool    BlockDestroy(BlockDescriptor* const block);
      }

      namespace gl30 // Karol Gasinski
      {
      //class Framebuffer : public en::gpu::Framebuffer
      //      {
      //      public:
      //                                               // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
      //      bool color(                              //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D (for all layers)
      //         const DataAccess access,              // Type of action that can be performed on this texture 
      //         const uint8 index,                    // Color attachment
      //         const Ptr<Texture>& texture,          // Texture
      //         const uint8 mipmap = 0);              // Mipmap
      //      
      //                                               // Use: TextureCubeMap
      //      bool color(                              //      TextureCubeMapArray (all layers)
      //         const DataAccess access,              // Type of action that can be performed on this texture 
      //         const uint8 index,                    // Color attachment
      //         const Ptr<Texture>& texture,          // CubeMap Texture
      //         const TextureFace face,               // CubeMap Face
      //         const uint8 mipmap = 0);              // Mipmap
      //      
      //      bool color(                              // Use: Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D (for specific layer)
      //         const DataAccess access,              // Type of action that can be performed on this texture 
      //         const uint8 index,                    // Color attachment
      //         const Ptr<Texture>& texture,          // Texture
      //         const uint16 layer,                   // Array layer or depth of 3D texture
      //         const uint8 mipmap = 0);              // Mipmap
      //      
      //      bool color(                              // Use: TextureCubeMapArray (for specific layer)
      //         const DataAccess access,              // Type of action that can be performed on this texture 
      //         const uint8 index,                    // Color attachment
      //         const Ptr<Texture>& texture,          // CubeMap Texture
      //         const TextureFace face,               // CubeMap Face
      //         const uint16 layer,                   // Array layer 
      //         const uint8 mipmap = 0);              // Mipmap
      //      
      //      bool depth(
      //         const DataAccess access,              // Type of action that can be performed on this texture 
      //         const Ptr<Texture>& texture);         // Depth Texture
      //      
      //      //virtual bool setStencil(
      //      //virtual bool setDepthStencil(
      //      
      //      void defaultColor(const uint8 index);    // Detach texture/renderbuffer from given color attachment
      //      void defaultDepth(void);                 // Detach texture/renderbuffer from depth attachment
      //      void defaultStencil(void);               // Detach texture/renderbuffer from stencil attachment
      //      void defaultDepthStencil(void);          // Detach texture/renderbuffer from deth/stencil attachment
      //      
      //      public:                                  // Engine internal
      //      uint32 id;                               // OpenGL framebuffer Id
   
      //      Framebuffer();
      //      ~Framebuffer();                          // Polymorphic deletes require a virtual base destructor
      //      };
      }

      namespace es20
      {
      //Texture TextureCreate(const TextureState& textureState);
      //bool    TextureUnmap(TextureDescriptor* texture);
      //bool    SamplerUnitUpdate(SamplerUnitDescriptor* sampler);
      }

   struct Context
          {
          struct Device
                 {
#ifdef EN_PLATFORM_ANDROID
                 ANativeWindow* window;  // Native Activity window
#endif
#ifdef EN_PLATFORM_BLACKBERRY
                 struct Screen
                        {
                        screen_context_t context;  // Screen Context
                        screen_window_t  window;   // Screen Window
                        screen_display_t display;  // Screen Display              
                        static const char* windowGroupName; // Screen Window Group Name
                        } screen;
#endif
#ifdef EN_PLATFORM_WINDOWS
                 bool        registered; // Indicates if window class was registered
                 HWND        hWnd;       // Window handle
                 HDC         hDC;        // Application Device Context
                 HGLRC*      hRC;        // OpenGL Rendering Contexts per worker thread
#endif
#ifdef EN_OPENGL_MOBILE
                 EGLDisplay  display;    // Display device
                 EGLSurface  surface;    // Drawing surface
                 EGLContext  context;    // OpenGL Rendering Contexts
                 Nversion    egl;        // Egl Version
#endif
                 uint8       contexts;   // OpenGL Rendering Contexts count
                 string      vendor;     // Vendor string
                 string      renderer;   // Renderer string
                 uint16      width;      // Display native resolution width
                 uint16      height;     // Display native resolution height
                 Nversion    api;        // Best supported Graphic API version
                 Nversion    sl;         // Best supported Shading Language version

                 void destroy(void);
                 void defaults(void);
                 } device;

          struct Support
                 {
                 // Basic information
                 set<string> extensions;                   // Table of supported extensions
                                                          
                 // Implementation dependent  

                 uint8       maxInputAssemblerAttributesCount;


                 // Texture
                 uint32      maxTextureSize;                 // Maximum 2D/1D texture image dimension
                 uint32      maxTextureRectSize;             // Maximum size of rectangle texture dimmension
                 uint32      maxTextureCubeSize;             // Maximum cube map texture image dimension 
                 uint32      maxTexture3DSize;               // Maximum 3D texture image dimension
                 uint32      maxTextureLayers;               // Maximum number of layers for texture arrays 
                 uint32      maxTextureBufferSize;           // No. of addressable texels for buffer textures
                 float       maxTextureLodBias;              // Maximum absolute texture level of detail bias


                 uint32      maxFramebufferColorAttachments; // Number of Framebuffer Color attachments
                 uint32      maxRenderTargets;               // Maximum supported Render Targets count            
                 uint32      maxClipDistances;               // Maximum number of user clipping planes 
                 uint32      subpixelBits;                   // Number of bits of subpixel precision in screen xw and yw

                 uint32      maxTextureUnits;                // Maximum number of Texture Units
                 uint32      maxRenderbufferSize;            // Maximum width and height of renderbuffers
                 uint32      maxViewportWidth;               // Maximum viewport dimensions 
                 uint32      maxViewportHeight;              // 
                 uint32      maxViewports;                   // Maximum number of active viewports
                 uint32      viewportSubpixelBits;           // No. of bits of subpixel precision for viewport bounds
                 float       viewportBoundsRange;            // Viewport bounds range [min; max] (at least [-32768; 32767])
                 uint32      layerProvokingVertex;           // Vertex convention followed by gl_Layer
                 uint32      viewportProvokingVertex;        // Vertex convention followed by gl_ViewportIndex
                 float       pointSizeMin;                   // Range (lo to hi) of point sprite sizes
                 float       pointSizeMax;                   //
                 float       pointSizeGranularity;           // Point sprite size granularity
                 float       aliasedLineWidthMin;            // Range (lo to hi) of aliased line widths
                 float       aliasedLineWidthMax;            //
                 float       antialiasedLineWidthMin;        // Range (lo to hi) of antialiased line widths 
                 float       antialiasedLineWidthMax;        //
                 float       antialiasedLineGranularity;     // Antialiased line width granularity 
                 uint32      maxElementIndices;              // Recommended max. number of DrawRangeElements indices
                 uint32      maxElementVerices;              // Recommended max. number of DrawRangeElements vertices
                 uint32      compressedTextureFormats;       // Enumerated compressed texture formats                   !!!!! TODO !!!!!!!
                 uint32      numCompressedTextureFormats;    // Number of compressed texture formats 

                 uint32      maxRectTextureSize;             // Max. width & height of rectangular textures 
                 uint32      programBinaryFormats;           // Enumerated program binary formats                       !!!!! TODO !!!!!!!
                 uint32      numProgramBinaryFormats;        // Number of program binary formats
                 uint32      shaderBinaryFormats;            // Enumerated shader binary formats                        !!!!! TODO !!!!!!!
                 uint32      numShaderBinaryFormats;         // Number of shader binary formats 
                 bool        shaderCompiler;                 // Shader compiler supported
                 uint32      minMapBufferAligment;           // Min. byte alignment of pointers returned by Map*Buffer
                 uint32      maxPatchSize;                   // Maximum number of Control Points in input tessellation patch
                 float       maxAnisotropy;                  // Maximum anisotropic filtering samples count

                 // Device state
                 uint32      postTransformVertexCacheSize; // Post Vertex Transform Cache Size

                 bool extension(string& name);
                 void defaults(void);
                 } support;

          struct Screen
                 {
                 Nversion    api;        // Graphic API version used for rendering
                 Nversion    sl;         // Shading Language version used for rendering    
                 uint16      width;
                 uint16      height;
                 uint8       samples;
                 bool        fullscreen;
                 bool        created;
                 Nmutex      lock;       // Screen creation need to be thread-safe
                 sint32      display;

                 bool support(Nversion version);
                 void defaults(void);
                 } screen;


// <<<<<< NEW GPU CONTEXT IMPLEMENTATION

          Nmutex textureUpload;    // <--- for OpenGL this locks needs to be global, as only one PBO can be updated at the same time (as OGL is state machine)

          // Samplers cache
          struct Sampler
             {
             struct SamplerCache
                {
                Ptr<gpu::Sampler> sampler;
                SamplerHash  hash;
                };
          
             SamplerCache* cache;
             uint32 cacheSize;
             } sampler;

          // Texture Units state
          struct TextureUnits
             {
             Ptr<gpu::Texture>* texture;
             Ptr<gpu::Sampler>* sampler;
             uint32*       metric;  // Age of each binding (reset each time it is set)
             } textureUnit;

// <<<<<< NEW GPU CONTEXT IMPLEMENTATION


          // State (deffered)
          // Changes of state are deffered till draw call
          struct State
                 {
                 struct Culling
                        {
                        uint16 function; // OpenGL Normal vector calculation method enum
                        uint16 face;     // OpenGL Face to be culled enum
                        bool   enabled;
                        } culling;

                 struct Depth
                        {
                        double clear;
                        uint16 function; // OpenGL depth function enum
                        bool   writing;
                        bool   testing;
                        } depth;
                 
                 struct Scissor
                        {
                        uint32 left;
                        uint32 bottom;
                        uint32 width;
                        uint32 height;
                        bool   enabled;
                        } scissor;
                 
                 struct Stencil
                        {
                        uint8  clear;
                        bool   writing;
                        bool   testing;
                        struct Front
                               {
                               sint32 reference; 
                               uint16 function;     // OpenGL Stencil test function enum
                               uint8  mask;
                               uint16 operation[3]; // OpenGL Stencil operation enums
                               } front;
                        struct Back
                               {
                               sint32 reference; 
                               uint16 function;     // OpenGL Stencil test function enum
                               uint8  mask;
                               uint16 operation[3]; // OpenGL Stencil operation enums
                               } back;
                        } stencil;
                 
                 struct Output
                        {
                        struct Color
                               {
                               struct Blend
                                      {
                                      bool   on;          // Blending 
                                      uint16 srcFuncRGB;  // Separate blending function for source RGB
                                      uint16 srcFuncA;    // Separate blending function for source A
                                      uint16 dstFuncRGB;  // Separate blending function for destination RGB
                                      uint16 dstFuncA;    // Separate blending function for destination A
                                      uint16 equationRGB; // Blend equation for RGB
                                      uint16 equationA;   // Blend equation for A    
                                      } blend;
                               } color[16];
                        float4 blendColor;
                        struct Framebuffer
                               {
                               uint32 read;               // Current Read Framebuffer ID
                               uint32 write;              // Current Write Framebuffer ID
                               } framebuffer;
                        } output;

                 struct Color
                        {
                        float4 clear;
                        bool   enabled[4];
                        } color;

                 // Deffered State Update bitmask
                 struct {
                        // Out-of-order deffered state changes
                        // (updated before draw call or swap buffers)
                        uint32 blend                : 16; // Blend on/off
                        uint32 blendFunction        : 16; // Blend function
                        uint32 blendEquation        : 16; // Blend equation
                        uint32 blendColor           : 1;  // Blending default color
                        uint32 color                : 1;  // Color buffer on/off mask
                        uint32 colorClearValue      : 1;  // Color clear value (updated before clearing color buffer)
                        uint32 colorClear           : 1;  // Color buffer clearing
                        uint32 depthTesting         : 1;  // Depth testing on/off
                        uint32 depthFunction        : 1;  // Depth test function
                        uint32 depthWriting         : 1;  // Depth writing on/off
                        uint32 depthClearValue      : 1;  // Depth clear value (updated before clearing depth buffer)
                        uint32 depthClear           : 1;  // Depth buffer clearing
                        uint32 stencilTesting       : 1;  // Stencil test on/off
                        uint32 stencilFrontFunc     : 1;  // Stencil front function, reference and mask
                        uint32 stencilBackFunc      : 1;  // Stencil back function, reference and mask
                        uint32 stencilFrontOps      : 1;  // Stencil front operations
                        uint32 stencilBackOps       : 1;  // Stencil back operation
                        uint32 stencilWriting       : 1;  // Stencil writing on/off
                        uint32 stencilClearValue    : 1;  // Stencil clear value ( update before clearing stencil buffer)
                        uint32 stencilClear         : 1;  // Stencil buffer clearing
                        uint32 culling              : 1;  // Backface culling on/off
                        uint32 cullingFunction      : 1;  // Culling function
                        uint32 cullingFace          : 1;  // Culling face
                        uint32 scissor              : 1;  // Scissor test on/off
                        uint32 scissorRect          : 1;  // Scissor rect changed
                        } dirtyBits;

                 // GPU Deffered State Update function type
                 typedef void(*gpuStateUpdateFunction)(void);

                 gpuStateUpdateFunction update[18]; // Table of pointers to functions that will update
                                                    // gpu state in driver if it has changed since last
                                                    // draw call.
                 uint32 dirty;                      // Count of update calls that need to be done.
                 
                 void defaults(void);
                 } state;

          // GPU Resuorces management
          //TarrayAdvanced<BufferDescriptor,  EN_MAX_ARRAY_SIZE> buffers;
          //TarrayAdvanced<TextureDescriptor, EN_MAX_ARRAY_SIZE> textures;
          TarrayAdvanced<ShaderDescriptor,  EN_MAX_ARRAY_SIZE> shaders;
          TarrayAdvanced<ProgramDescriptor, EN_MAX_ARRAY_SIZE> programs;

          uint32 emptyVAO;

          Context();
         ~Context();

#ifdef EN_PLATFORM_ANDROID
          bool create(ANativeWindow* window);
#else
          bool create(void);
#endif
          void destroy(void);
          void defaults(void);
          void linkToInterface(void);
          };
   }

extern gpu::Context GpuContext;
}

#endif

#endif

#endif
