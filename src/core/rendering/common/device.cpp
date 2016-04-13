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
      
   const Nversion    Vulkan_1_0(0, EN_Vulkan, 0, 0, 0);                  // Vulkan 1.0

   const Nversion    Metal_OSX_1_0(0,EN_Metal_OSX,1,0,0);
   const Nversion    Metal_OSX_Unsupported(255,EN_Metal_OSX,255,255,15); // For marking unsupported features
   
   const Nversion    Metal_IOS_1_0(0,EN_Metal_IOS,1,0,0);
   const Nversion    Metal_IOS_Unsupported(255,EN_Metal_IOS,255,255,15); // For marking unsupported features
 
   CommonDevice::CommonDevice()
   {
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


