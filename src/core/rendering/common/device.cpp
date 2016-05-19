/*
 
 Ngine v5.0
 
 Module      : Common Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#include "core/log/log.h"
#include "core/rendering/common/device.h"
#if defined(EN_PLATFORM_OSX)
#include "core/rendering/metal/mtlAPI.h"
//extern class en::gpu::MetalAPI;
#endif

namespace en
{
   namespace gpu
   {
   const Nversion    Vulkan_1_0(0, EN_Vulkan, 0, 0, 0);                  // Vulkan 1.0

   const Nversion    Metal_OSX_1_0(0,EN_Metal_OSX,1,0,0);                // Metal on OSX 10.11
   const Nversion    Metal_OSX_Unsupported(255,EN_Metal_OSX,255,255,15); // For marking unsupported features
   
   const Nversion    Metal_IOS_1_0(0,EN_Metal_IOS,1,0,0);
   const Nversion    Metal_IOS_Unsupported(255,EN_Metal_IOS,255,255,15); // For marking unsupported features


#ifdef EN_OPENGL_DESKTOP
   #define SLVersions  12
   #define APIVersions 19
#endif
#ifdef EN_OPENGL_MOBILE
   #define SLVersions  2
   #define APIVersions 6
#endif
  
   const Nversion    ESSL_3_00(1,EN_ESSL,3,0,0);                         // ESSL 3.00  
   const Nversion    ESSL_1_00(0,EN_ESSL,1,0,0);                         // ESSL 1.00  
   const Nversion    ESSL_Unsupported(255,EN_ESSL,255,255,15);           // For marking unsupported features

   const Nversion    OpenGL_ES_3_2(5,EN_OpenGL_ES,3,2,0);                // OpenGL ES 3.2 -
   const Nversion    OpenGL_ES_3_1(4,EN_OpenGL_ES,3,1,0);                // OpenGL ES 3.1 - 
   const Nversion    OpenGL_ES_3_0(3,EN_OpenGL_ES,3,0,0);                // OpenGL ES 3.0 - 
   const Nversion    OpenGL_ES_2_0(2,EN_OpenGL_ES,2,0,0);                // OpenGL ES 2.0 - 
   const Nversion    OpenGL_ES_1_1(1,EN_OpenGL_ES,1,1,0);                // OpenGL ES 1.1 - 
   const Nversion    OpenGL_ES_1_0(0,EN_OpenGL_ES,1,0,0);                // OpenGL ES 1.0 - 
   const Nversion    OpenGL_ES_Unsupported(255,EN_OpenGL_ES,255,255,15); // For marking unsupported features

   const Nversion    GLSL_Next(255,EN_GLSL,9,90,0);                      // Future GLSL versions
   const Nversion    GLSL_4_50(11,EN_GLSL,4,50,0);                       // GLSL 4.50
   const Nversion    GLSL_4_40(10,EN_GLSL,4,40,0);                       // GLSL 4.40
   const Nversion    GLSL_4_30(9,EN_GLSL,4,30,0);                        // GLSL 4.30
   const Nversion    GLSL_4_20(8,EN_GLSL,4,20,0);                        // GLSL 4.20
   const Nversion    GLSL_4_10(7,EN_GLSL,4,10,0);                        // GLSL 4.10
   const Nversion    GLSL_4_00(6,EN_GLSL,4,0,0);                         // GLSL 4.00
   const Nversion    GLSL_3_30(5,EN_GLSL,3,30,0);                        // GLSL 3.30
   const Nversion    GLSL_1_50(4,EN_GLSL,1,50,0);                        // GLSL 1.50 for OpenGL 3.2
   const Nversion    GLSL_1_40(3,EN_GLSL,1,40,0);                        // GLSL 1.40 for OpenGL 3.1
   const Nversion    GLSL_1_30(2,EN_GLSL,1,30,0);                        // GLSL 1.30 for OpenGL 3.0
   const Nversion    GLSL_1_20(1,EN_GLSL,1,20,0);                        // GLSL 1.20 for OpenGL 2.1
   const Nversion    GLSL_1_10(0,EN_GLSL,1,10,0);                        // GLSL 1.10 for OpenGL 2.0
   const Nversion    GLSL_Unsupported(255,EN_GLSL,255,255,15);           // For marking unsupported features

   const Nversion    OpenGL_Next(255,EN_OpenGL,9,9,0);                   // Future OpenGL versions
   const Nversion    OpenGL_4_5(18,EN_OpenGL,4,5,0);                     // OpenGL 4.5    -  
   const Nversion    OpenGL_4_4(17,EN_OpenGL,4,4,0);                     // OpenGL 4.4    -   
   const Nversion    OpenGL_4_3(16,EN_OpenGL,4,3,0);                     // OpenGL 4.3    -   
   const Nversion    OpenGL_4_2(15,EN_OpenGL,4,2,0);                     // OpenGL 4.2    -
   const Nversion    OpenGL_4_1(14,EN_OpenGL,4,1,0);                     // OpenGL 4.1    - 
   const Nversion    OpenGL_4_0(13,EN_OpenGL,4,0,0);                     // OpenGL 4.0    - 11.03.2010 ok
   const Nversion    OpenGL_3_3(12,EN_OpenGL,3,3,0);                     // OpenGL 3.3    - 11.03.2010 ok
   const Nversion    OpenGL_3_2(11,EN_OpenGL,3,2,0);                     // OpenGL 3.2    - 24.07.2009 ok
   const Nversion    OpenGL_3_1(10,EN_OpenGL,3,1,0);                     // OpenGL 3.1    - 24.03.2009 ok
   const Nversion    OpenGL_3_0(9,EN_OpenGL,3,0,0);                      // OpenGL 3.0    - 11.08.2008 ok
   const Nversion    OpenGL_2_1(8,EN_OpenGL,2,1,0);                      // OpenGL 2.1    - 30.07.2006 spec
   const Nversion    OpenGL_2_0(7,EN_OpenGL,2,0,0);                      // OpenGL 2.0    - 22.10.2004 spec
   const Nversion    OpenGL_1_5(6,EN_OpenGL,1,5,0);                      // OpenGL 1.5    - 29.07.2003 / 30.10.2003 spec
   const Nversion    OpenGL_1_4(5,EN_OpenGL,1,4,0);                      // OpenGL 1.4    - 24.07.2002 spec
   const Nversion    OpenGL_1_3(4,EN_OpenGL,1,3,0);                      // OpenGL 1.3    - 14.08.2001 spec
   const Nversion    OpenGL_1_2_1(3,EN_OpenGL,1,2,1);                    // OpenGL 1.2.1  - 01.04.1999 spec
   const Nversion    OpenGL_1_2(2,EN_OpenGL,1,2,0);                      // OpenGL 1.2    -
   const Nversion    OpenGL_1_1(1,EN_OpenGL,1,1,0);                      // OpenGL 1.1    - 29.03.1997
   const Nversion    OpenGL_1_0(0,EN_OpenGL,1,0,0);                      // OpenGL 1.0    - 20.06.1992
   const Nversion    OpenGL_Unsupported(255,EN_OpenGL,255,255,15);       // For marking unsupported features

   CommonDevice::CommonDevice()
   {
   support.attribute.reset();
   support.format.reset();
   
   // Input Assembler
   support.maxInputAssemblerAttributesCount = 0;

   // Texture
   support.maxTextureSize                   = 0;
   support.maxTextureRectSize               = 0;
   support.maxTextureCubeSize               = 0;
   support.maxTexture3DSize                 = 0;
   support.maxTextureLayers                 = 0;
   support.maxTextureBufferSize             = 0;
   support.maxTextureLodBias                = 0.0f;

   // Sampler
   support.maxAnisotropy                    = 0.0f;

//   maxRenderTargets               = 0;
//   maxClipDistances               = 0;
//   subpixelBits                   = 0;
//   maxTextureUnits                = 0;
//   maxRenderbufferSize            = 0;
//   maxViewportWidth               = 0;
//   maxViewportHeight              = 0;
//   maxViewports                   = 0;
//   viewportSubpixelBits           = 0;
//   viewportBoundsRange            = 0.0f;
//   layerProvokingVertex           = 0;
//   viewportProvokingVertex        = 0;
//   pointSizeMin                   = 0.0f;
//   pointSizeMax                   = 0.0f;
//   pointSizeGranularity           = 0.0f;
//   aliasedLineWidthMin            = 0.0f;
//   aliasedLineWidthMax            = 0.0f;
//   antialiasedLineWidthMin        = 0.0f;  
//   antialiasedLineWidthMax        = 0.0f;
//   antialiasedLineGranularity     = 0.0f;  
//   maxElementIndices              = 0;
//   maxElementVerices              = 0;
//   compressedTextureFormats       = 0; // TODO: This should be vector of formats!
//   numCompressedTextureFormats    = 0;
//   maxTextureBufferSize           = 0;
//   maxRectTextureSize             = 0;
//   programBinaryFormats           = 0; // TODO: This should be vector of formats!
//   numProgramBinaryFormats        = 0;
//   shaderBinaryFormats            = 0; // TODO: This should be vector of formats!
//   numShaderBinaryFormats         = 0;
//   shaderCompiler                 = false;
//   minMapBufferAligment           = 0;
//   maxFramebufferColorAttachments = 0;
//   postTransformVertexCacheSize   = 32;
//   maxPatchSize                   = 0;
   }

   CommonDevice::~CommonDevice()
   {
   }
   
   void CommonDevice::init(void)
   {
   // Should be implemented by API
   assert(0);
   }
   
   Ptr<InputAssembler> CommonDevice::create(const DrawableType primitiveType,
                                            const uint32 controlPoints,
                                            const uint32 usedAttributes,
                                            const uint32 usedBuffers,
                                            const AttributeDesc* attributes,
                                            const BufferDesc* buffers)
   {
   // Should be implemented by API
   assert(0);
   return Ptr<InputAssembler>(nullptr);
   }
   
   
   // This static function should be in .mm file if we include Metal headers !!!
   bool GraphicAPI::create(void)
   {
   Log << "Starting module: Rendering.\n";

   // Load from config file desired Rendering API and Shading Language Version
   // Load choosed API for Android & Windows
   
#if defined(EN_PLATFORM_ANDROID)
   // Graphics = ptr_dynamic_cast<GraphicAPI, OpenGLESAPI>(Ptr<OpenGLESAPI>(new OpenGLESAPI()));
   // Graphics = ptr_dynamic_cast<GraphicAPI, VulkanAPI>  (Ptr<VulkanAPI>(new VulkanAPI()));
#endif
#if defined(EN_PLATFORM_BLACKBERRY)
   Graphics = ptr_dynamic_cast<GraphicAPI, OpenGLESAPI>(Ptr<OpenGLESAPI>(new OpenGLESAPI()));
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   Graphics = ptr_dynamic_cast<GraphicAPI, MetalAPI>(Ptr<MetalAPI>(new MetalAPI()));
#endif
#if defined(EN_PLATFORM_WINDOWS)
   // Graphics = ptr_dynamic_cast<GraphicAPI, Direct3DAPI>(Ptr<Direct3DAPI>(new Direct3DAPI()));
   // Graphics = ptr_dynamic_cast<GraphicAPI, OpenGLAPI>(Ptr<OpenGLAPI>(new OpenGLESAPI()));
   // Graphics = ptr_dynamic_cast<GraphicAPI, VulkanAPI>(Ptr<VulkanAPI>(new VulkanAPI()));
#endif

   return (Graphics == nullptr) ? false : true;
   }
   
   }
   
Ptr<gpu::GraphicAPI> Graphics;
}


