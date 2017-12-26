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

#include "core/defines.h"
#include "core/configuration.h"
#include "core/types.h"
#include "core/log/log.h"


#include "core/rendering/context.h"
#include "core/rendering/opengl/glState.h"
#include "core/rendering/opengl/glTexture.h"

#include "utilities/strings.h"

//#include "core/rendering/api.cpp"

#if 0 //!defined(EN_PLATFORM_OSX)

namespace en
{
   namespace gpu
   {

   Context::Context() :
      emptyVAO(0)
   {
   //buffers.create(4096);
   //textures.create(4096);
   shaders.create(4096);
   programs.create(4096);

   defaults();
   }

   Context::~Context()
   {
   }

#ifdef EN_PLATFORM_ANDROID
   bool Context::create(ANativeWindow* window)
   {
   Log << "Starting module: Rendering.\n";

   defaults();

   // Wait until onNativeWindowResized callback is complete.
   while(device.window == NULL);

   // Create window with default OpenGL context
   if (!egl::CreateDummyWindow())
      return false;

   // Gather information about device capabilities
   gl::GatherDeviceInformation();
   return true;
   }
#else
   bool Context::create(void)
   {
   Log << "Starting module: Rendering.\n";

   defaults();

#ifdef EN_PLATFORM_WINDOWS  
   // Create window with default OpenGL context
   if (!wgl::CreateDummyWindow())
      return false;

   // Gather information about device capabilities
   gl::GatherDeviceInformation();
#endif

   return true;
   }
#endif

   void Context::destroy(void)
   {
   Log << "Closing module: Rendering." << endl;
   Gpu.screen.destroy();
   }

   // Sets all variables to default values
   void Context::defaults(void)
   {
   device.defaults();
   support.defaults();
   screen.defaults();
   state.defaults();

   // Clear all settings
   //gl20::BuffersClear();
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   ClearTextureSupport();
#endif
   gl20::ProgramsClear();
   }

   void Context::linkToInterface(void)
   {
   // TODO: Here rebind dynamic interface to best available implementation
   //Interface::Support::Input::type     = gl20::SupportColumnType;
   //Interface::Support::Texture::type   = gl20::SupportTextureType;
   //Interface::Support::Texture::format = gl20::SupportTextureFormat;

#ifdef EN_OPENGL_MOBILE
   Interface::display                  = egl::Display;
#endif
#ifdef EN_PLATFORM_WINDOWS  
   Interface::display                  = wgl::Display;
#endif
   }

   void Context::Device::destroy(void)
   {
#ifdef EN_PLATFORM_BLACKBERRY
   //Stop requesting events from libscreen
   screen_stop_events(GpuContext.device.screen.context);

   //Shut down BPS library for this process
   bps_shutdown();
#endif

#ifdef EN_OPENGL_MOBILE
   if (GpuContext.device.display != EGL_NO_DISPLAY) 
      {
      eglMakeCurrent(GpuContext.device.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

      if (GpuContext.device.context != EGL_NO_CONTEXT) 
         eglDestroyContext(GpuContext.device.display, GpuContext.device.context);

      if (GpuContext.device.surface != EGL_NO_SURFACE) 
         eglDestroySurface(GpuContext.device.display, GpuContext.device.surface);
       
#ifdef EN_PLATFORM_BLACKBERRY
      if (GpuContext.device.screen.window != NULL)
         screen_destroy_window(GpuContext.device.screen.window);
#endif

      eglTerminate(GpuContext.device.display);
      }
   eglReleaseThread();

   GpuContext.device.display = EGL_NO_DISPLAY;
   GpuContext.device.context = EGL_NO_CONTEXT;
   GpuContext.device.surface = EGL_NO_SURFACE;
   GpuContext.device.egl     = Nversion(0, 0, 0, 0, 0);
#ifdef EN_PLATFORM_BLACKBERRY
   GpuContext.device.screen.window = NULL;
#endif

#endif

#ifdef EN_PLATFORM_WINDOWS
   // Disabling and deleting all rendering contexts
   wglMakeCurrent(NULL,NULL); 
   for(uint8 i=0; i<contexts; i++)
      if (hRC[i])
         {
         wglDeleteContext(hRC[i]);
         hRC[i] = NULL;
         }   
   contexts = 0;
   
   // Disabling device context
   if (hDC)
      {
      ReleaseDC(hWnd, hDC); 
      hDC = 0;
      }
   
   // Deleting window
   if (hWnd)
      {
      DestroyWindow(hWnd);  
      hWnd = 0;
      } 
   
   // Unregistering window class
   if (registered)
      {
      UnregisterClass(L"WindowClass", (HINSTANCE)GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE)); // GWL_HINSTANCE is deprecated in x64 environment
      registered = false;
      }
#endif

#ifdef EN_PLATFORM_BLACKBERRY
   //Destroy libscreen context
   screen_destroy_context(GpuContext.device.screen.context);
#endif
   }

   void Context::Device::defaults(void)
   {
#ifdef EN_PLATFORM_ANDROID
   window     = NULL;
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   screen.context = NULL;
   screen.window  = NULL;
   screen.display = NULL;   
#endif
#ifdef EN_PLATFORM_WINDOWS
   registered = false;
   hWnd       = NULL;
   hDC        = NULL;
   hRC        = NULL;
#endif
#ifdef EN_OPENGL_MOBILE
   display    = EGL_NO_DISPLAY;
   context    = EGL_NO_CONTEXT;
   surface    = EGL_NO_SURFACE;
   egl        = Nversion(0, 0, 0, 0, 0);
#endif
   contexts   = 0;
   vendor.clear();
   renderer.clear();
   width      = 800;        // TODO: Query default/current display device for native resolution.
   height     = 600;
   api.value  = 0;
   sl.value   = 0;
   }

   // Returns true if such extension is supported
   bool Context::Support::extension(string& name)
   {
   set<string>::iterator i = extensions.find(name);
   if (i != extensions.end())
      return true;
   return false;
   }

   void Context::Support::defaults(void)
   {
   extensions.clear();

   // Input Assembler
   maxInputLayoutAttributesCount = 0;

   // Texture
   maxTextureSize                 = 0;
   maxTextureCubeSize             = 0;
   maxTexture3DSize               = 0;
   maxTextureLayers               = 0;
   maxTextureBufferSize           = 0;
   maxTextureLodBias              = 0.0f;


   maxRenderTargets               = 0;
   maxClipDistances               = 0;
   subpixelBits                   = 0;
   maxTextureUnits                = 0;
   maxRenderbufferSize            = 0;
   maxViewportWidth               = 0;
   maxViewportHeight              = 0;
   maxViewports                   = 0;
   viewportSubpixelBits           = 0;
   viewportBoundsRange            = 0.0f;
   layerProvokingVertex           = 0;
   viewportProvokingVertex        = 0;
   pointSizeMin                   = 0.0f;
   pointSizeMax                   = 0.0f;
   pointSizeGranularity           = 0.0f;
   aliasedLineWidthMin            = 0.0f;
   aliasedLineWidthMax            = 0.0f;
   antialiasedLineWidthMin        = 0.0f;  
   antialiasedLineWidthMax        = 0.0f;
   antialiasedLineGranularity     = 0.0f;  
   maxElementIndices              = 0;
   maxElementVerices              = 0;
   compressedTextureFormats       = 0; // TODO: This should be vector of formats!
   numCompressedTextureFormats    = 0;
   maxTextureBufferSize           = 0;
   maxRectTextureSize             = 0;
   programBinaryFormats           = 0; // TODO: This should be vector of formats!
   numProgramBinaryFormats        = 0;
   shaderBinaryFormats            = 0; // TODO: This should be vector of formats!
   numShaderBinaryFormats         = 0;
   shaderCompiler                 = false;
   minMapBufferAligment           = 0;
   maxFramebufferColorAttachments = 0;
   postTransformVertexCacheSize   = 32;
   maxPatchSize                   = 0;
   }

   // Returns true if current RC support specified or better version of Graphic API 
   bool Context::Screen::support(Nversion version)
   {
   return api.better(version);
   }

   void Context::Screen::defaults(void)
   {
   api.value  = 0;
   sl.value   = 0;
   created    = false;
   width      = 0;
   height     = 0;
   fullscreen = false;
   samples    = 1;
   display    = -1;
   }

   // Sets all variables to default values
   void Context::State::defaults(void)
   { 
   culling.enabled            = false;
   culling.face               = gl::Face[BackFace];
   culling.function           = gl::NormalCalculationMethod[CounterClockWise];
                              
   depth.clear                = 1.0;
   depth.function             = TranslateCompareMethod[Less];
   depth.testing              = false;
   depth.writing              = true;
                              
   scissor.bottom             = 0;
   scissor.left               = 0;
   scissor.width              = 0;
   scissor.height             = 0;
   scissor.enabled            = false;
   
   stencil.testing            = false;
   stencil.writing            = true;
   stencil.clear              = false;
   stencil.front.reference    = 0;
   stencil.front.function     = TranslateCompareMethod[Always];
   stencil.front.mask         = 0xFF;
   stencil.front.operation[0] = gl::StencilModification[Keep];
   stencil.front.operation[1] = gl::StencilModification[Keep];
   stencil.front.operation[2] = gl::StencilModification[Keep];
   stencil.back.reference     = 0;
   stencil.back.function      = TranslateCompareMethod[Always];
   stencil.back.mask          = 0xFF;
   stencil.back.operation[0]  = gl::StencilModification[Keep];
   stencil.back.operation[1]  = gl::StencilModification[Keep];
   stencil.back.operation[2]  = gl::StencilModification[Keep];

   color.clear.r              = 0.0f;
   color.clear.g              = 0.0f;
   color.clear.b              = 0.0f;
   color.clear.a              = 0.0f;
   color.enabled[0]           = true;
   color.enabled[1]           = true;
   color.enabled[2]           = true;
   color.enabled[3]           = true;
            
   output.blendColor          = float4(0.0f, 0.0f, 0.0f, 0.0f);
   for(uint8 i=0; i<16; ++i)
      {
      output.color[i].blend.on          = false;
      output.color[i].blend.srcFuncRGB  = gl::BlendFunction[One].type;
      output.color[i].blend.srcFuncA    = gl::BlendFunction[One].type;
      output.color[i].blend.dstFuncRGB  = gl::BlendFunction[Zero].type;
      output.color[i].blend.dstFuncA    = gl::BlendFunction[Zero].type;
      output.color[i].blend.equationRGB = gl::BlendEquation[Add];
      output.color[i].blend.equationA   = gl::BlendEquation[Add];
      }
   output.framebuffer.read  = 0;
   output.framebuffer.write = 0;

   // All values are dirty at beginning 
   // and need to be set in driver.
   update[0]  = gl::CullingUpdate;
   update[1]  = gl::CullingFunctionUpdate;
   update[2]  = gl::CullingFaceUpdate;
   update[3]  = gl::DepthWritingUpdate;
   update[4]  = gl::DepthTestingUpdate;
   update[5]  = gl::DepthFunctionUpdate;
   update[6]  = gl::ScissorUpdate;
   update[7]  = gl::ScissorRectUpdate;
   update[8]  = gl::StencilWritingUpdate;
   update[9]  = gl::StencilTestingUpdate;
   update[10] = gl::StencilOpsUpdate;
   update[11] = gl::StencilFuncUpdate;
   update[12] = gl::ColorWritingUpdate;
   update[13] = gl::ClearUpdate;
   update[14] = gl::BlendUpdate;
   update[15] = gl::BlendColorUpdate;
   update[16] = gl::BlendFuncUpdate;
   update[17] = gl::BlendEquationUpdate;

   dirty = 18;

   // Set all bits to dirty
   memset(&dirtyBits, 0xFFFFFFFF, sizeof(dirtyBits));
   }

      namespace gl
      {

      // Checks OpenGL eror state
      bool IsError(const char* function)
      {
      // Check for errors
      uint32 error = glGetError();
      if (error == GL_NO_ERROR)
         return false;
      
      // Compose error message
      string info;
      info += "ERROR: OpenGL function ";
      info += function;
      info += " caused error:\n";
      info += "       ";
      while(error)
           {
           // Create error message
           if (error == GL_INVALID_ENUM)
              info += "GL_INVALID_ENUM\n";
           else
           if (error == GL_INVALID_VALUE)
              info += "GL_INVALID_VALUE\n";
           else
           if (error == GL_INVALID_OPERATION)
              info += "GL_INVALID_OPERATION\n";
           else
           if (error == GL_OUT_OF_MEMORY)
              info += "GL_OUT_OF_MEMORY\n";
#ifndef EN_MOBILE_GPU
#ifndef EN_PLATFORM_OSX
           else
           if (error == GL_STACK_OVERFLOW)
              info += "GL_STACK_OVERFLOW\n";
           else
           if (error == GL_STACK_UNDERFLOW)
              info += "GL_STACK_UNDERFLOW\n";
           else
           if (error == GL_TABLE_TOO_LARGE)
              info += "GL_TABLE_TOO_LARGE\n";
#endif
#endif
           else
              info += "Unknown error enum!\n";
      
           // Check for more error messages
           error = glGetError();
           }
      
      Log << info.c_str();
      return true; 
      }

      void Trace(const char* function)
      {
#ifdef EN_PROFILER_TRACE_GRAPHICS_API
      Log << "OpenGL: " << function << endl;
#endif
#ifdef EN_PROFILER_VALIDATE_GRAPHICS_API 
      if (IsError(function))
         assert(0);
#endif
      }

      }
   }

gpu::Context   GpuContext;
gpu::Interface Gpu; 
}

void   (*en::gpu::Interface::display)(void)                                                 = NULL; 
//bool   (*en::gpu::Interface::Support::Input::type)(const en::gpu::ColumnType type)          = NULL; // Checks if specified input type is supported
//bool   (*en::gpu::Interface::Support::Texture::type)(const en::gpu::TextureType type)       = NULL; // Checks if specified texture type is supported
//bool   (*en::gpu::Interface::Support::Texture::format)(const en::gpu::TextureFormat format) = NULL; // Checks if specified texture format is supported


#endif
