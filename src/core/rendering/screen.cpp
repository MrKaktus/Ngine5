/*

 Ngine v5.0
 
 Module      : Screen.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/log/log.h"

//#include "input/context.h"   
#include "input/oculus.h"  // For Oculus define

#include "core/rendering/opengl/gl33Sampler.h"
#include "core/rendering/opengl/glTexture.h"

#include "core/rendering/context.h"
#include "platform/context.h"

#include "threading/scheduler.h"
#include "utilities/strings.h"
#include "resources/context.h"

#ifdef EN_PLATFORM_WINDOWS
#include "platform/windows/win_events.h"
#endif
#include "resources/bmp.h"  // For screenshot

//#include "core/rendering/api.cpp"

#if !defined(EN_PLATFORM_OSX)

namespace en
{
   namespace gpu
   {

   #ifdef EN_OPENGL_DESKTOP
   ScreenSettings::ScreenSettings() :
      shadingLanguage(GLSL_1_10),
      display(-1),
      width(0),  
      height(0), 
      samples(1),
#ifndef EN_PLATFORM_OSX // QUICK DIRTY WA FOR NEW RENDERING API ABSTRACTION
      mode(Fullscreen)
#else
      mode(Window)
#endif
   {
   }
   #endif
   #ifdef EN_OPENGL_MOBILE
   ScreenSettings::ScreenSettings() :
      shadingLanguage(ESSL_1_00),
      display(-1),
      width(0),  
      height(0), 
      samples(1), 
      mode(Fullscreen)
   {
   }
   #endif

   // Creates window with default settings
   bool Interface::Screen::create(const string& title)
   {
   ScreenSettings settings;
   
   // If there are screen settings set in config, use them.
   Config.get("en.rendering.display", &settings.display);
   Config.get("en.rendering.width", &settings.width);
   Config.get("en.rendering.height", &settings.height);
   Config.get("en.rendering.samples", &settings.samples);
   Config.get("en.rendering.shadingLanguage", settings.shadingLanguage);
   
   // Create window
   bool created = false;
#ifdef EN_OPENGL_MOBILE
   created = egl::ScreenCreate(settings, title);
#endif
#ifdef EN_PLATFORM_WINDOWS
   created = wgl::ScreenCreate(settings, title);
#endif
   
   // Link dynamic interface taking into notice actual capabilities
   if (created)
      GpuContext.linkToInterface();
   
   return created;
   }    
   
   // Creates window with basic settings         
   bool Interface::Screen::create(const ScreenMode fullscreen, const uint16 width, const uint16 height, const string& title)
   {
   ScreenSettings settings;
   settings.mode   = fullscreen;
   settings.width  = width;
   settings.height = height;  
   
   // If there are screen settings set in config, use them.
   Config.get("en.rendering.samples", &settings.samples);
   Config.get("en.rendering.shadingLanguage", settings.shadingLanguage);

   // Create window
   bool created = false;
#ifdef EN_OPENGL_MOBILE
   created = egl::ScreenCreate(settings, title);
#endif
#ifdef EN_PLATFORM_WINDOWS
   created = wgl::ScreenCreate(settings, title);
#endif
   
   // Link dynamic interface taking into notice actual capabilities
   if (created)
      GpuContext.linkToInterface();
   
   return created;
   } 

   // Creates window with advanced settings            
   bool Interface::Screen::create(ScreenSettings* settings, const string& title)
   {
   // If pointer was passed, it should be correct
   if (settings == NULL)
      return false;
   
   // Create window
   bool created = false;
#ifdef EN_OPENGL_MOBILE
   created = egl::ScreenCreate(*settings, title);
#endif
#ifdef EN_PLATFORM_WINDOWS
   created = wgl::ScreenCreate(*settings, title);
#endif
   
   // Link dynamic interface taking into notice actual capabilities
   if (created)
      GpuContext.linkToInterface();
   
   return created;
   } 

   bool Interface::Screen::capture(const string filename)
   {
   uint32 width  = GpuContext.screen.width;
   uint32 height = GpuContext.screen.height;
   uint8* data = new uint8[width * height * 3];

   Profile( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) )
   Profile( glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, data) )

   bool result = en::bmp::save(width, height, data, string(filename + ".bmp"));
   
   ////Swap R and B components
   //if (format == gpu::FormatABGR_8)
   //   for(uint32 i=0; i<(DIBHeader.width * DIBHeader.height); ++i)
   //      {
   //      uint8 temp    = buffer[i*4];
   //      buffer[i*4]   = buffer[i*4+2]; // R <- B
   //      buffer[i*4+2] = temp;          // B -> R
   //      }

   delete [] data;
   return result;
   }

   void Interface::Screen::view(uint32 x, uint32 y, uint32 width, uint32 height)
   {
   if (!GpuContext.screen.created)
      return;
   Profile( glViewport(x, y, width, height) )
   }

   void Interface::Screen::view(uint32v4 viewport)
   {
   if (!GpuContext.screen.created)
      return;
   Profile( glViewport(viewport.x, viewport.y, viewport.width, viewport.height) )
   }

   // Tells if screen is created
   bool Interface::Screen::created(void)
   {
   return GpuContext.screen.created;
   }
   
   // Returns screen width
   uint16 Interface::Screen::width(void)
   {
   return GpuContext.screen.width;
   }
    
   // Returns screen height
   uint16 Interface::Screen::height(void)
   {
   return GpuContext.screen.height;
   }
   
   // Returns fullscreen status
   bool Interface::Screen::fullscreen(void)
   {
   return GpuContext.screen.fullscreen;
   }
   
   // Returns screen aspect ratio
   float Interface::Screen::aspect(void)
   {
   if (!GpuContext.screen.created)
      return 0.0f;
   
   return static_cast<float>(GpuContext.screen.width) / static_cast<float>(GpuContext.screen.height);
   }

   sint32 Interface::Screen::display(void)
   {
   return GpuContext.screen.display;
   }

   bool Interface::Screen::destroy(void)
   {
#ifdef EN_PLATFORM_ANDROID
   return egl::ScreenDestroy();
#endif
#ifdef EN_PLATFORM_WINDOWS
   return wgl::ScreenDestroy();
#else
   return false;
#endif
   }

#ifdef EN_PLATFORM_BLACKBERRY
const char* Context::Device::Screen::windowGroupName = "NgineDefaultWindowGroup";
#endif

      namespace gl
      {
      // OpenGL API versions
      const Nversion openglVersions[] = 
         { 
         OpenGL_1_0,
         OpenGL_1_1,
         OpenGL_1_2,
         OpenGL_1_2_1,
         OpenGL_1_3,
         OpenGL_1_4,
         OpenGL_1_5,
         OpenGL_2_0,
         OpenGL_2_1,
         OpenGL_3_0,
         OpenGL_3_1,
         OpenGL_3_2,
         OpenGL_3_3,
         OpenGL_4_0,
         OpenGL_4_1,
         OpenGL_4_2,
         OpenGL_4_3,
         OpenGL_4_4,
         OpenGL_Next
         };
      
      // GLSL versions
      const Nversion glslVersions[] = 
         { 
         GLSL_1_10,
         GLSL_1_20,
         GLSL_1_30,
         GLSL_1_40,
         GLSL_1_50,
         GLSL_3_30,
         GLSL_4_00,
         GLSL_4_10,
         GLSL_4_20,
         GLSL_4_30,
         GLSL_4_40,
         GLSL_Next
         };

      // OpenGL API versions
      const Nversion openglesVersions[] = 
         { 
         OpenGL_ES_1_0,
         OpenGL_ES_1_1,
         OpenGL_ES_2_0,
         OpenGL_ES_3_0,
         OpenGL_ES_3_1
         };

      // ESSL versions
      const Nversion esslVersions[] = 
         { 
         ESSL_1_00,
         ESSL_3_00
         };

      #ifdef EN_OPENGL_DESKTOP

      void GatherDeviceInformation(void)
      {
      // Clean error indicator
      glGetError();
    
      // To get latest supported versions of OpenGL 
      // in OpenGL 3.0 Core and later contexts, this
      // code should be used:
      //
      // glGetIntegerv(GL_MAJOR_VERSION, &device.api.high);
      // glGetIntegerv(GL_MINOR_VERSION, &device.api.medium);
      // 
      // But data about gpu are gathered in dummy
      // OpenGL context which works in deprecaded
      // mode. This forces implementation in old
      // deprecated way:
      
      // Gets OpenGL version
      string tmp;
      uint64 dot, dot2, end;
      tmp  = string((char*)glGetString(GL_VERSION));
      dot  = tmp.find('.', 0);
      dot2 = tmp.find('.', dot+1);
      end  = tmp.find(' ', 0);
      GpuContext.device.api.api  = EN_OpenGL;
      GpuContext.device.api.high = stringTo<uint32>(tmp.substr(0,dot));   
      if (dot2 == string::npos || dot2 > end)
         GpuContext.device.api.medium = stringTo<uint32>(tmp.substr(dot+1, end-dot-1));
      else
         {
         GpuContext.device.api.medium = stringTo<uint32>(tmp.substr(dot+1, dot2-dot-1));
         GpuContext.device.api.low    = stringTo<uint32>(tmp.substr(dot2+1, end-dot2-1));
         }
      
      // Determine OpenGL release
      bool found = false;
      for(uint8 i=0; i<OpenGLRevisions; i++)
         if (openglVersions[i].high   == GpuContext.device.api.high   &&
             openglVersions[i].medium == GpuContext.device.api.medium &&
             openglVersions[i].low    == GpuContext.device.api.low )
            {
            GpuContext.device.api.release = openglVersions[i].release;
            found = true;
            break;
            }
      if (!found)
         GpuContext.device.api.release = OpenGL_Next.release;

      // Gets Shading Language version
      tmp  = string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
      dot  = tmp.find('.',0);
      dot2 = tmp.find('.',dot+1);
      end  = tmp.find(' ',0);
      GpuContext.device.sl.api  = EN_GLSL;
      GpuContext.device.sl.high = stringTo<uint32>(tmp.substr(0,dot));   
      if (dot2 == string::npos || dot2 > end)
         GpuContext.device.sl.medium = stringTo<uint32>(tmp.substr(dot+1, end-dot-1));
      else
         {
         GpuContext.device.sl.medium = stringTo<uint32>(tmp.substr(dot+1, dot2-dot-1));
         GpuContext.device.sl.low    = stringTo<uint32>(tmp.substr(dot2+1, end-dot2-1));
         }
      
      // Determine Shading Language release
      for(uint8 i=0; i<GLSLRevisions; i++)
         if (glslVersions[i].high   == GpuContext.device.sl.high   &&
             glslVersions[i].medium == GpuContext.device.sl.medium &&
             glslVersions[i].low    == GpuContext.device.sl.low )
            {
            GpuContext.device.sl.release = glslVersions[i].release;
            break;
            }
      
      // Get OpenGL vendor and renderer strings
      GpuContext.device.vendor   = string((char*)glGetString(GL_VENDOR));
      GpuContext.device.renderer = string((char*)glGetString(GL_RENDERER));
      
      // Destroy dummy window
      GpuContext.device.destroy();
    
      // Clear all settings
      //gl20::BuffersClear();
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      ClearTextureSupport();
#endif
      gl20::ProgramsClear();
      }

      #endif
      #ifdef EN_OPENGL_MOBILE

      void GatherDeviceInformation(void)
      {
      // Clean error indicator
      glGetError();
     
      // Gets OpenGL ES version
      string tmp;
      sint32 dot, dot2, begin, end;
      tmp   = string((char*)glGetString(GL_VERSION));
      dot   = tmp.find('.',0);
      dot2  = tmp.find('.',dot+1);
      begin = tmp.rfind(' ',dot);
      end   = tmp.find(' ',0);
      GpuContext.device.api.api  = EN_OpenGL_ES;
      GpuContext.device.api.high = stringTo<uint32>(tmp.substr(begin+1,dot));   
      if (dot2 == static_cast<sint32>(string::npos) || dot2 > end)
         GpuContext.device.api.medium = stringTo<uint32>(tmp.substr(dot+1, end-dot-1));
      else
         {
         GpuContext.device.api.medium = stringTo<uint32>(tmp.substr(dot+1, dot2-dot-1));
         GpuContext.device.api.low    = stringTo<uint32>(tmp.substr(dot2+1, end-dot2-1));
         }
   
      // Determine OpenGL ES release
      for(uint8 i=0; i<OpenGLESRevisions; i++)
         if (openglesVersions[i].high   == GpuContext.device.api.high   &&
             openglesVersions[i].medium == GpuContext.device.api.medium &&
             openglesVersions[i].low    == GpuContext.device.api.low )
            {
            GpuContext.device.api.release = openglesVersions[i].release;
            break;
            }
    
      // Gets Shading Language version
      GpuContext.device.sl = ESSL_Unsupported;
      if (GpuContext.device.api.better(OpenGL_ES_1_1))
         {
         tmp.clear();
         tmp   = string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
         dot   = tmp.find('.',0);
         dot2  = tmp.find('.',dot+1); 
         begin = tmp.rfind(' ',dot);
         end   = tmp.find(' ',0);
         GpuContext.device.sl.api  = EN_ESSL;
         GpuContext.device.sl.high = stringTo<uint32>(tmp.substr(begin+1,dot));    
         if (dot2 == static_cast<sint32>(string::npos) || dot2 > end)
            GpuContext.device.sl.medium = stringTo<uint32>(tmp.substr(dot+1, end-dot-1));
         else
            {
            GpuContext.device.sl.medium = stringTo<uint32>(tmp.substr(dot+1, dot2-dot-1));
            GpuContext.device.sl.low    = stringTo<uint32>(tmp.substr(dot2+1, end-dot2-1));
            }
     
         // Determine Shading Language release
         for(uint8 i=0; i<ESSLRevisions; i++)
            if (esslVersions[i].high   == GpuContext.device.sl.high   &&
                esslVersions[i].medium == GpuContext.device.sl.medium &&
                esslVersions[i].low    == GpuContext.device.sl.low )
               {
               GpuContext.device.sl.release = esslVersions[i].release;
               break;
               }
         }

      // Get OpenGL ES vendor and renderer strings
      GpuContext.device.vendor   = string((char*)glGetString(GL_VENDOR));
      GpuContext.device.renderer = string((char*)glGetString(GL_RENDERER));

      // Query device native resolution
      EGLint width, height;
      eglQuerySurface(GpuContext.device.display, GpuContext.device.surface, EGL_WIDTH, &width);
      eglQuerySurface(GpuContext.device.display, GpuContext.device.surface, EGL_HEIGHT, &height);
      GpuContext.device.width  = (uint16)width;
      GpuContext.device.height = (uint16)height;

      // Creates table of supported extensions strings
      tmp   = string((char*)glGetString(GL_EXTENSIONS));
      begin = 0;
      end   = tmp.find(' ',0);
      while(end != static_cast<sint32>(string::npos))
           {
           GpuContext.support.extensions.insert(GpuContext.support.extensions.end(),tmp.substr(begin,end-begin));
           begin = end+1;
           end   = tmp.find(' ',begin);
           }

#ifndef EN_PLATFORM_BLACKBERRY  
      // Destroy dummy window
      GpuContext.device.destroy();
#endif

      // Clear all settings
      //gl20::BuffersClear();
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      ClearTextureSupport();
#endif
      gl20::ProgramsClear();
      }

      #endif
      }

      namespace egl
      {
      #ifdef EN_OPENGL_MOBILE

      // Create OpenGL ES 2.0 Context by default
      EGLint contextAttribs[] = 
         {
         EGL_CONTEXT_CLIENT_VERSION, 2,
         EGL_NONE
         };

      // Default Pixel Format Descriptor
      EGLint pfdDefault[] = 
         {
         EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,         //   - OpenGL ES 2.0 required
         EGL_RED_SIZE,        8,                          // -.
         EGL_GREEN_SIZE,      8,                          //  +- COLOR - Minimum 24bit RGB (R8G8B8)
         EGL_BLUE_SIZE,       8,                          // _|
         EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,             //   - Can run in WINDOW mode
         EGL_NONE
         };

      // Normal Pixel Format Descriptor
      EGLint pfdNormal[] = 
         { 
         EGL_NATIVE_RENDERABLE, EGL_TRUE,                 //   - Hardware accelerated 
         EGL_RENDERABLE_TYPE,   EGL_OPENGL_ES2_BIT,       //   - OpenGL ES 2.0 required
         EGL_BUFFER_SIZE,       24,                       // -.
         EGL_RED_SIZE,          8,                        //  |
         EGL_GREEN_SIZE,        8,                        //  +- COLOR - Minimum 24bit RGB (R8G8B8)
         EGL_BLUE_SIZE,         8,                        //  |
         EGL_ALPHA_SIZE,        8,                        // _|
         EGL_DEPTH_SIZE,        16,                       //   - DEPTH   - Minimum 16bit
         EGL_STENCIL_SIZE,      8,                        //   - STANCIL - Minimum 8bit    
         EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,           //   - Can run in WINDOW mode
         EGL_SAMPLES,           1,                        //   - MULTISAMPLING - 1,2,4,8,16 samples
         EGL_NONE 
         };

      // Advanced Pixel Format Descriptor
      EGLint pfdAdvanced[] = 
         { 
         EGL_NATIVE_RENDERABLE, EGL_TRUE,                 //   - Hardware accelerated 
         EGL_RENDERABLE_TYPE,   EGL_OPENGL_ES2_BIT,       //   - OpenGL ES 2.0 required
         EGL_BUFFER_SIZE,       24,                       // -.
         EGL_RED_SIZE,          8,                        //  |
         EGL_GREEN_SIZE,        8,                        //  +- COLOR - Minimum 32bit RGBA (R8G8B8A8)
         EGL_BLUE_SIZE,         8,                        //  |
         EGL_ALPHA_SIZE,        8,                        // _|
         EGL_DEPTH_SIZE,        24,                       //   - DEPTH   - Minimum 24bit
         EGL_STENCIL_SIZE,      8,                        //   - STANCIL - Minimum 8bit    
         EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,           //   - Can run in WINDOW mode
         EGL_SAMPLES,           1,                        //   - MULTISAMPLING - 1,2,4,8,16 samples
         EGL_NONE 
         };

      bool CreateDummyWindow(void)
      {
      // Clear settings 
      GpuContext.device.destroy();		
      GpuContext.support.defaults();
      GpuContext.screen.defaults();
      GpuContext.state.defaults();

      //gl20::BuffersClear();
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      ClearTextureSupport();
#endif
      gl20::ProgramsClear();

#ifdef EN_PLATFORM_BLACKBERRY
      // Create Screen Context
      if (screen_create_context(&GpuContext.device.screen.context, SCREEN_APPLICATION_CONTEXT) != 0) 
         {
         Log << "Error! Cannot create Screen Context.\n";
         GpuContext.device.destroy();	
         return false;         
         }

      //Initialize BPS library
      if (bps_initialize() == BPS_FAILURE)
         {
         Log << "Error! Cannot initialize BPS library.\n";
         return false; 
         }
#endif

      // Get connection to Default Display
      GpuContext.device.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
      if (GpuContext.device.display == EGL_NO_DISPLAY)
         {
         Log << "Error! Cannot connect EGL to Device.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Initialize EGL
      EGLint major, minor;
      if (eglInitialize(GpuContext.device.display, &major, &minor) != EGL_TRUE)
         {
         Log << "Error! Cannot initialize EGL.\n";
         GpuContext.device.destroy();	
         return false;
         }
      GpuContext.device.egl.high   = major;
      GpuContext.device.egl.medium = minor;

      // Mention that mobile OpenGL is required
      if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) 
         {
         Log << "Error! Cannot use OpenGL ES with EGL.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Find pixel format that matches 
      EGLConfig config;
      EGLint numConfigs;
      if (!eglChooseConfig(GpuContext.device.display, pfdDefault, &config, 1, &numConfigs))
         {
         Log << "Error! Cannot find default pixel format for dummy window.\n";
         GpuContext.device.destroy();	
         return false;
         }

#ifdef EN_PLATFORM_ANDROID
      // At this moment we need to be sure that Native Window is already 
      // created. We lock until onNativeWindowCreated callback.
      while(GpuContext.device.window == NULL);

      // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
      // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
      // As soon as we picked a EGLConfig, we can safely reconfigure the
      // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
      EGLint format;
      eglGetConfigAttrib(GpuContext.device.display, config, EGL_NATIVE_VISUAL_ID, &format);
      ANativeWindow_setBuffersGeometry(GpuContext.device.window, 0, 0, format);

      GpuContext.device.surface = eglCreateWindowSurface(GpuContext.device.display, config, GpuContext.device.window, NULL);
#endif

      // Create Device Context
      GpuContext.device.context = eglCreateContext(GpuContext.device.display, config, EGL_NO_CONTEXT, contextAttribs); 
      if (GpuContext.device.context == EGL_NO_CONTEXT) 
         {
         Log << "Error! Cannot create EGL Device Context.\n";
         GpuContext.device.destroy();	
         return false;         
         }

#ifdef EN_PLATFORM_BLACKBERRY
      // Create Window Context
      if (screen_create_window(&GpuContext.device.screen.window, GpuContext.device.screen.context) != 0) 
         {
         Log << "Error! Cannot create Window Context.\n";
         GpuContext.device.destroy();	
         return false; 
         }

      // Set Window to RGBA8888
      int format = SCREEN_FORMAT_RGBX8888;
      if (screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_FORMAT, &format) != 0) 
         {
         Log << "Error! Cannot set Window format to RGBX8888.\n";
         GpuContext.device.destroy();	
         return false; 
         }
      
      // Window needs to be compatible with OpenGL ES 2.0 and do the Rotation without reallocation.
      int usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
      if (screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_USAGE, &usage) != 0)         
         {
         Log << "Error! Cannot configure Window Context.\n";
         GpuContext.device.destroy();	
         return false; 
         }

      // Get Display connected to Screen Window
      if (screen_get_window_property_pv(GpuContext.device.screen.window, SCREEN_PROPERTY_DISPLAY, (void **)&GpuContext.device.screen.display) != 0) 
         {
         Log << "Error! Cannot obtain Screen Display.\n";
         GpuContext.device.destroy();	
         return false; 
         }



    //int screen_resolution[2];
    //if (screen_get_display_property_iv(GpuContext.device.screen.display, SCREEN_PROPERTY_SIZE, screen_resolution) != 0) 
    //    {
    //    perror("screen_get_display_property_iv");
    //    bbutil_terminate();
    //    return EXIT_FAILURE;
    //   } 

    //int angle = atoi(getenv("ORIENTATION"));

    //screen_display_mode_t screen_mode;
    //if (screen_get_display_property_pv(GpuContext.device.screen.display, SCREEN_PROPERTY_MODE, (void**)&screen_mode) != 0) 
    //    {
    //    perror("screen_get_display_property_pv");
    //    bbutil_terminate();
    //    return EXIT_FAILURE;
    //    }

    //int size[2];
    //if (screen_get_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_BUFFER_SIZE, size) != 0) 
    //   {
    //    perror("screen_get_window_property_iv");
    //    bbutil_terminate();
    //    return EXIT_FAILURE;
    //    }

    //int buffer_size[2] = {size[0], size[1]};

    //if ((angle == 0) || (angle == 180)) {
    //    if (((screen_mode.width > screen_mode.height) && (size[0] < size[1])) ||
    //        ((screen_mode.width < screen_mode.height) && (size[0] > size[1]))) {
    //            buffer_size[1] = size[0];
    //            buffer_size[0] = size[1];
    //    }
    //} else if ((angle == 90) || (angle == 270)){
    //    if (((screen_mode.width > screen_mode.height) && (size[0] > size[1])) ||
    //        ((screen_mode.width < screen_mode.height && size[0] < size[1]))) {
    //            buffer_size[1] = size[0];
    //            buffer_size[0] = size[1];
    //    }
    //} else {
    //     fprintf(stderr, "Navigator returned an unexpected orientation angle.\n");
    //     bbutil_terminate();
    //     return EXIT_FAILURE;
    //}

    //rc = screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
    //if (rc) {
    //    perror("screen_set_window_property_iv");
    //    bbutil_terminate();
    //    return EXIT_FAILURE;
    //}

    //rc = screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_ROTATION, &angle);
    //if (rc) {
    //    perror("screen_set_window_property_iv");
    //    bbutil_terminate();
    //    return EXIT_FAILURE;
    //}

      // Create double buffered Window
      if (screen_create_window_buffers(GpuContext.device.screen.window, 2) != 0) 
         {
         Log << "Error! Cannot create double buffered Window.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Create EGL Surface
      GpuContext.device.surface = eglCreateWindowSurface(GpuContext.device.display, config, GpuContext.device.screen.window, NULL);
      if (GpuContext.device.surface == EGL_NO_SURFACE) 
         {
         Log << "Error! Cannot create EGL surface.\n";
         GpuContext.device.destroy();	
         return false;
         }   
#endif
 
      // Make Context current
      if (eglMakeCurrent(GpuContext.device.display, 
                         GpuContext.device.surface, 
                         GpuContext.device.surface, 
                         GpuContext.device.context) == EGL_FALSE) 
         {
         Log << "Error! Cannot set device context as current.\n";
         GpuContext.device.destroy();	
         return false;
         } 

      return true;
      }

      bool ScreenCreate(const ScreenSettings& settings, const string& title)
      {
      // If gpu is already creating screen break.
      if (!GpuContext.screen.lock.tryLock())
         return false;

      // Checks if screen isn't already created.
      if (GpuContext.screen.created)
         {
         GpuContext.screen.lock.unlock();
         return false;
         }

      // On mobile devices ignore screen settings:
      // - sl version, assume OpenGL ES 2.0 and ESSL 1.0
      // - fullscreen mode on
      // - native width
      // - native height
   
      // Clear settings 
      //GpuContext.device.destroy();		
      //GpuContext.support.defaults(); // TODO: Should we clear this ? ? ? 
      GpuContext.screen.defaults();
      GpuContext.state.defaults();

      //gl20::BuffersClear();
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      ClearTextureSupport();
#endif
      gl20::ProgramsClear();

#ifdef EN_PLATFORM_BLACKBERRY
      // Create Screen Context
      if (screen_create_context(&GpuContext.device.screen.context, SCREEN_APPLICATION_CONTEXT) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot create Screen Context.\n";
         GpuContext.device.destroy();	
         return false;         
         }

      //Initialize BPS library
      if (bps_initialize() != BPS_SUCCESS)
         {
         Log << "Error! Cannot initialize BPS library.\n";
         return false; 
         }
#endif

      // Get connection to Default Display
      GpuContext.device.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
      if (GpuContext.device.display == EGL_NO_DISPLAY)
         {
         Log << "Error! Cannot connect EGL to Device.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Initialize EGL
      EGLint major, minor;
      if (!eglInitialize(GpuContext.device.display, &major, &minor))
         {
         Log << "Error! Cannot initialize EGL.\n";
         GpuContext.device.destroy();	
         return false;
         }
      GpuContext.device.egl.high   = major;
      GpuContext.device.egl.medium = minor;

      // Mention that mobile OpenGL is required
      if (!eglBindAPI(EGL_OPENGL_ES_API)) 
         {
         Log << "Error! Cannot use OpenGL ES with EGL.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Find pixel format that matches 
      EGLConfig config;
      EGLint numConfigs;
      if (!eglChooseConfig(GpuContext.device.display, pfdDefault, &config, 1, &numConfigs))
         {
         Log << "Error! Cannot find default pixel format for dummy window.\n";
         GpuContext.device.destroy();	
         return false;
         }

#ifdef EN_PLATFORM_ANDROID
      // At this moment we need to be sure that Native Window is already 
      // created. We lock until onNativeWindowCreated callback.
      while(GpuContext.device.window == NULL);

      // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
      // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
      // As soon as we picked a EGLConfig, we can safely reconfigure the
      // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
      EGLint format;
      eglGetConfigAttrib(GpuContext.device.display, config, EGL_NATIVE_VISUAL_ID, &format);
      ANativeWindow_setBuffersGeometry(GpuContext.device.window, 0, 0, format);

      GpuContext.device.surface = eglCreateWindowSurface(GpuContext.device.display, config, GpuContext.device.window, NULL);
#endif

      // Create Device Context
      GpuContext.device.context = eglCreateContext(GpuContext.device.display, config, EGL_NO_CONTEXT, contextAttribs); 
      if (GpuContext.device.context == EGL_NO_CONTEXT) 
         {
         Log << "Error! Cannot create EGL Device Context.\n";
         GpuContext.device.destroy();	
         return false;         
         }

#ifdef EN_PLATFORM_BLACKBERRY
      // Create Window Context
      if (screen_create_window(&GpuContext.device.screen.window, GpuContext.device.screen.context) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot create Window Context.\n";
         GpuContext.device.destroy();	
         return false; 
         }

      // Create Window Group (for advertisements purposes)
      if (screen_create_window_group(GpuContext.device.screen.window, GpuContext.device.screen.windowGroupName) != BPS_SUCCESS)
         {
         Log << "Error! Cannot create Window Group.\n";
         GpuContext.device.destroy();	
         return false; 
         }

      // Set Window to RGBA8888
      int format = SCREEN_FORMAT_RGBX8888;
      if (screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_FORMAT, &format) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot set Window format to RGBX8888.\n";
         GpuContext.device.destroy();	
         return false; 
         }
    
      // Window needs to be compatible with OpenGL ES 2.0 and do the Rotation without reallocation.
      int usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
      if (screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_USAGE, &usage) != BPS_SUCCESS)         
         {
         Log << "Error! Cannot configure Window Context.\n";
         GpuContext.device.destroy();	
         return false; 
         }

      // Get Display connected to Screen Window
      if (screen_get_window_property_pv(GpuContext.device.screen.window, SCREEN_PROPERTY_DISPLAY, (void **)&GpuContext.device.screen.display) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot obtain Screen Display.\n";
         GpuContext.device.destroy();	
         return false; 
         }

      //// Get Display native resolution
      //sint32 screen_resolution[2];
      //if (screen_get_display_property_iv(GpuContext.device.screen.display, SCREEN_PROPERTY_SIZE, screen_resolution) != BPS_SUCCESS) 
      //   {
      //   Log << "Error! Cannot obtain display native resolution.\n";
      //   GpuContext.device.destroy();	
      //   return false;
      //   } 

      // Get information about display
      screen_display_mode_t display;
      if (screen_get_display_property_pv(GpuContext.device.screen.display, SCREEN_PROPERTY_MODE, (void**)&display) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot obtain information about display.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Get Window buffer resolution
      sint32 WindowResolution[2];
      if (screen_get_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_BUFFER_SIZE, WindowResolution) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot obtain display mode.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Correct Window buffer resolution, so it will match current orientation and display resolution
      sint32 CorrectedResolution[2] = {WindowResolution[0], WindowResolution[1]};

      // Get current orientation of the device
      sint32 angle = atoi(getenv("ORIENTATION"));

      // Landscape orientation
      if ( (angle == 0) || 
           (angle == 180) ) 
         {
         // If current display resolution doesn't match, switch width with height 
         if ( ( (display.width > display.height) && (WindowResolution[0] < WindowResolution[1]) ) ||
              ( (display.width < display.height) && (WindowResolution[0] > WindowResolution[1]) ) ) 
            {
            CorrectedResolution[1] = WindowResolution[0];
            CorrectedResolution[0] = WindowResolution[1];
            }
         } 
      else 
      // Portrait orientation
      if ( (angle == 90) || 
           (angle == 270) )
         {
         // 
         if ( ( (display.width > display.height) && (WindowResolution[0] > WindowResolution[1]) ) ||
              ( (display.width < display.height) && (WindowResolution[0] < WindowResolution[1]) ) ) 
            {
            CorrectedResolution[1] = WindowResolution[0];
            CorrectedResolution[0] = WindowResolution[1];
            }
         } 
      else 
      // Free orientation (not supported)
         {
         Log << "Error! Navigator returned an unexpected orientation angle.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Correct Window buffer resolution, so it will match display resolution
      if (screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_BUFFER_SIZE, CorrectedResolution) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot match Window buffer resolution with Display resolution.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Match Window rotation with Device rotation
      if (screen_set_window_property_iv(GpuContext.device.screen.window, SCREEN_PROPERTY_ROTATION, &angle) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot match Window rotation angle with Navigator orientation angle.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Create double buffered Window
      if (screen_create_window_buffers(GpuContext.device.screen.window, 2) != BPS_SUCCESS) 
         {
         Log << "Error! Cannot create double buffered Window.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Create EGL Surface
      GpuContext.device.surface = eglCreateWindowSurface(GpuContext.device.display, config, GpuContext.device.screen.window, NULL);
      if (GpuContext.device.surface == EGL_NO_SURFACE) 
         {
         Log << "Error! Cannot create EGL surface.\n";
         GpuContext.device.destroy();	
         return false;
         }   
#endif

      // Make Context current
      if (eglMakeCurrent(GpuContext.device.display, 
                         GpuContext.device.surface, 
                         GpuContext.device.surface, 
                         GpuContext.device.context) == EGL_FALSE) 
         {
         Log << "Error! Cannot set device context as current.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Swap buffer every frame
      eglSwapInterval(GpuContext.device.display, 1);

#ifdef EN_PLATFORM_BLACKBERRY
      // Signal BPS library that screen events will be requested
      if (screen_request_events(GpuContext.device.screen.context) != BPS_SUCCESS) 
         {
         Log << "Error! Requesting screen events failed.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Signal BPS library that navigator events will be requested
      if (navigator_request_events(0) != BPS_SUCCESS) 
         {
         Log << "Error! Requesting navigator events failed.\n";
         GpuContext.device.destroy();	
         return false;
         }

      // Gather information about device capabilities
      gl::GatherDeviceInformation();
#endif

      // Updating screen information
      GpuContext.screen.fullscreen = true;
      GpuContext.screen.width      = GpuContext.device.width; 
      GpuContext.screen.height     = GpuContext.device.height;
      GpuContext.screen.samples    = 1;
      GpuContext.screen.api        = GpuContext.device.api;
      GpuContext.screen.sl         = ESSL_1_00;
      GpuContext.screen.created    = true;
      
      // TODO: Set scissor test area to full screen
      //gpu.render.test.scissor.m_width   = w;
      //gpu.render.test.scissor.m_height  = h;

#ifdef EN_DEBUG
      // DEBUG INFO - Prints supported extensions
      Log << '\n';
      Log << "Rendering Context debug information: \n";
      Log << "==================================== \n";
      Log << "Features   : \n";
      Log << string("  EGL      : " + stringFrom(GpuContext.device.egl.high) + "." + stringFrom(GpuContext.device.egl.medium) + '\n');
      Log << string("  OpenGL ES: " + stringFrom(GpuContext.device.api.high) + "." + stringFrom(GpuContext.device.api.medium) + '\n');
      Log << string("  ESSL     : " + stringFrom(GpuContext.device.sl.high) + "." + stringFrom(GpuContext.device.sl.medium) + '\n'); 
      Log << string("  Vendor   : " + GpuContext.device.vendor + '\n');
      Log << string("  Device   : " + GpuContext.device.renderer + "\n\n");
      Log << string("Supported " + stringFrom(GpuContext.support.extensions.size()) + " extensions:\n");
      for(set<string>::iterator i = GpuContext.support.extensions.begin(); i != GpuContext.support.extensions.end(); i++)
         Log << string("  " + *i + '\n');
      Log << '\n';
#ifdef EN_PLATFORM_BLACKBERRY
      Log << " Device:\n";
      Log << string("  Orientation : " + stringFrom(angle) + " degrees.\n");
      Log << string("  Resolution  : " + stringFrom(display.width) + "x" + stringFrom(display.height) + '\n');
      Log << string("  Window res. : " + stringFrom(WindowResolution[0]) + "x" + stringFrom(WindowResolution[1]) + '\n');
      Log << '\n';
#endif
      Log << " Window created:\n";
      Log << string("  Resolution : " + stringFrom(GpuContext.screen.width) + "x" + stringFrom(GpuContext.screen.height) + '\n');
      Log << string("  OpenGL ES  : " + stringFrom(GpuContext.screen.api.high) + "." + stringFrom(GpuContext.screen.api.medium) + '\n');
      Log << string("  ESSL       : " + stringFrom(GpuContext.screen.sl.high) + "." + stringFrom(GpuContext.screen.sl.medium) + '\n');
#endif
    
      // Gather information about features supported by this context
      gpu::gl::GatherContextCapabilities();
      gpu::gl::StateInit();
      gpu::gl20::BuffersInit();   
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      InitTextureSupport();  
#endif
      gpu::gl20::ProgramsInit();

      // Clear errors indicator
      glGetError();
      
      // Set default viewport
      Profile( glViewport(0, 0, GpuContext.screen.width, GpuContext.screen.height) );

      // Creating default resources
      ResourcesContext.create();

      // Screen creation completed.
      GpuContext.screen.lock.unlock();
      return true;
      }

      // Destroys current window
      bool ScreenDestroy(void)
      {  
      if (!GpuContext.screen.created)
          return false;
     
      // Free all resources connected to gpu
      ResourcesContext.destroy();
         
      // Clear whole state
      GpuContext.device.destroy();
      GpuContext.defaults();

      return true;
      }

      void Display(void)
      {
      // Update deffered state in driver
      for(uint32 i=0; i<GpuContext.state.dirty; ++i)
         {
         GpuContext.state.update[i]();
         GpuContext.state.update[i] = NULL;  // For debug purposes
         }
      GpuContext.state.dirty = 0;
      
      // Swap buffers (Turn off while Java managed)
      eglSwapBuffers(GpuContext.device.display, GpuContext.device.surface);
      }

      #endif
      }


      namespace wgl
      {
      #ifdef EN_PLATFORM_WINDOWS

      // Shading Language Support
      struct SLSupport
             {
             Nversion sl;  
             uint8    min; // First API version that supports
             uint8    max; // Last API version that supports
             };

      // GLSL support table
      const static SLSupport glslSupport[] = 
         { 
         { GLSL_4_40, OpenGL_4_4.release, OpenGL_4_4.release },
         { GLSL_4_30, OpenGL_4_3.release, OpenGL_4_4.release }, 
         { GLSL_4_20, OpenGL_4_2.release, OpenGL_4_4.release },
         { GLSL_4_10, OpenGL_4_1.release, OpenGL_4_4.release }, 
         { GLSL_4_00, OpenGL_4_0.release, OpenGL_4_4.release },  
         { GLSL_3_30, OpenGL_3_3.release, OpenGL_4_4.release }, 
         { GLSL_1_50, OpenGL_3_2.release, OpenGL_4_4.release },
         { GLSL_1_40, OpenGL_3_1.release, OpenGL_4_4.release },
         { GLSL_1_30, OpenGL_3_0.release, OpenGL_4_4.release },
         // Legacy
         { GLSL_1_20, OpenGL_2_1.release, OpenGL_2_1.release },
         { GLSL_1_10, OpenGL_2_0.release, OpenGL_2_1.release } 
         };

      // Default Pixel Format Descriptor 
      const static PIXELFORMATDESCRIPTOR pfdDefault = 
         { 
         sizeof(PIXELFORMATDESCRIPTOR),
         1,
         PFD_DRAW_TO_WINDOW |            // Window with OpenGL double buffered RBGA surface
         PFD_SUPPORT_OPENGL |
         PFD_DOUBLEBUFFER,               // _
         PFD_TYPE_RGBA,                  //  |  COLOR - Minimum 24bit RGB
         24,                             //  |  " Specifies the number of color bitplanes in each color buffer. 
         0, 0,                           //  +-   For RGBA pixel types, it is the size of the color buffer,
         0, 0,                           //  |    excluding the alpha bitplanes. "
         0, 0,                           //  | ( http://msdn.microsoft.com/en-us/library/dd368826(v=vs.85).aspx )   
         0, 0,                           // -  
         0,                              // ACCUM     - off
         0, 0, 0, 0,                     // 
         16,                             // DEPTH     - 16bit
         0,                              // STENCIL   - off
         0,                              // AUXILIARY - off
         PFD_MAIN_PLANE,                 // Main Drawing Layer - (obsolete iLayerType, equals 0)
         0,
         0, 0, 0 
         };
     
      // Legacy Pixel Format Descriptor 
      const static PIXELFORMATDESCRIPTOR pfdLegacy = 
         { 
         sizeof(PIXELFORMATDESCRIPTOR),
         1,
         PFD_DRAW_TO_WINDOW |            // Support window 
         PFD_SUPPORT_OPENGL |            // Support OpenGL double buffered RBGA surface
         PFD_DOUBLEBUFFER,               // _
         PFD_TYPE_RGBA,                  //  |  COLOR - Minimum 24bit RGB
         24,                             //  |  " Specifies the number of color bitplanes in each color buffer. 
         0, 0,                           //  +-   For RGBA pixel types, it is the size of the color buffer,
         0, 0,                           //  |    excluding the alpha bitplanes. "
         0, 0,                           //  | ( http://msdn.microsoft.com/en-us/library/dd368826(v=vs.85).aspx )   
         0, 0,                           // -  
         0,                              // ACCUM     - off
         0, 0, 0, 0,                     // 
         32,                             // DEPTH     - 32bit Z-Buffer
         0,                              // STENCIL   - off (included in Depth Z-Buffer)
         0,                              // AUXILIARY - off
         PFD_MAIN_PLANE,                 // Main Drawing Layer - (obsolete iLayerType, equals 0)
         0,
         0, 0, 0 
         };

      WorkerContext::WorkerContext(uint8 core, ScreenSettings settings) :
         settings(settings),
         core(core),
         Task(new TaskState())
      {
      }
      
      void WorkerContext::work(void)
      {
      // If application requires minimum GLSL 1.30 functionality,
      // OpenGL 3.0 Core profile or better context needs to be
      // created.
      if ( settings.shadingLanguage.better(GLSL_1_30) && 
           GpuContext.support.extension(ARB_create_context) )
         {
         // Engine works only with Core profiles
         uint32 glAttributes[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                                   WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                                   WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                                   GL_CONTEXT_PROFILE_MASK, GL_CONTEXT_CORE_PROFILE_BIT,
                                   0, 0 };
     
         // Debug version supports better debugging
      #ifdef EN_DEBUG
         glAttributes[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
      #endif 

         // Try to create window with best OpenGL API version
         glAttributes[1] = GpuContext.screen.api.high;
         glAttributes[3] = GpuContext.screen.api.medium;
     
         // Below 3.2 version we are not using profiles
         if ( !GpuContext.screen.api.better(OpenGL_3_2) )
            {
            glAttributes[6] = 0;
            glAttributes[7] = 0;  
            }
     
         // Try to create OpenGL context in new way
         GpuContext.device.hRC[core] = wglCreateContextAttribsARB(GpuContext.device.hDC, GpuContext.device.hRC[0], (const int *) &glAttributes);
         }
      // If application requires older shading language ( 1.10 or 1.20 )
      // old method of creating OpenGL context will be used. In result
      // any OpenGL version context can be created, but it will be in
      // compatibility mode. For this cases engine should use maximum
      // OpenGL 2.1.2 capabilities.
      else
         GpuContext.device.hRC[core] = wglCreateContext(GpuContext.device.hDC);
      
      // Activating Rendering Context worker on current thread
      if (GpuContext.device.hRC[core] != nullptr)
         wglMakeCurrent(GpuContext.device.hDC, GpuContext.device.hRC[core]);
      
      string activated("Rendering worker ");
      activated.append(stringFrom(core));
      activated.append(" activated.\n");
      DebugLog( activated );
      }

      bool CreateDummyWindow(void)
      {
      HINSTANCE             AppInstance;        // Application handle
      WNDCLASS              Window;             // Window class
      uint32                PixelFormat;        // Format of pixel
      
      // Get this application instance
      AppInstance          = (HINSTANCE)GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE); // GWL_HINSTANCE is deprecated in x64 environment
      
      // Window settings
      Window.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // Have its own Device Context and also cannot be resized    
      Window.lpfnWndProc   = (WNDPROC) WinEvents;                 // Procedure that handles OS evets
      Window.cbClsExtra    = 0;                                   // No extra window data
      Window.cbWndExtra    = 0;                                   //
      Window.hInstance     = AppInstance;                         // Handle to instance of program that this window belongs to
      Window.hIcon         = LoadIcon(NULL, IDI_WINLOGO);         // Load default icon
      Window.hCursor       = LoadCursor(NULL, IDC_ARROW);         // Load arrow pointer
      Window.hbrBackground = NULL;                                // No background (OpenGL will handle this)
      Window.lpszMenuName  = NULL;                                // No menu
      Window.lpszClassName = L"WindowClass";                      // Class name
      
      // Registering Window Class
      if (!RegisterClass(&Window))	
         {					
         Log << "Error! Cannot register window class." << endl;
         return false;
         }
      GpuContext.device.registered = true;
      
      // Create Dummy Window with OpenGL RC to gather required
      // information together with wgl functions pointers and
      // final Pixel Format number.
      GpuContext.device.hWnd = CreateWindow(
         L"WindowClass",               // Window class name
         L"Ngine 4.0 initializing...", // Title of window            
         WS_CLIPSIBLINGS |             // \_Prevents from overdrawing
         WS_CLIPCHILDREN |             // / by other windows
         WS_OVERLAPPEDWINDOW,          // Additional styles
         0, 0,                         // Position
         128,                          // True window widht
         128,                          // True window height
         NULL,                         // No parent window
         NULL,                         // No menu
         AppInstance,                  // Handle to this instance of program
         NULL );                       // Won't pass anything
      
      // Check if dummy window was created
      if (GpuContext.device.hWnd == NULL)
         {
         Log << "Error! Cannot create window.\n";
         GpuContext.device.destroy();	
         return false;
         }  
      
      // Acquiring Device Context
      GpuContext.device.hDC = GetDC(GpuContext.device.hWnd);
      if (GpuContext.device.hDC == NULL)
         {
         Log << "Error! Cannot create device context.\n";		
         GpuContext.device.destroy();	
         return false;
         }  
      
      // Choosing Pixel Format the old way
      PixelFormat = ChoosePixelFormat(GpuContext.device.hDC, &pfdDefault);
      if (PixelFormat == NULL)
         {
         Log << "Error! Cannot find aproprieate pixel format.\n";	
         GpuContext.device.destroy();	
         return false;
         }  
      
      // Activating Pixel Format
      if (!SetPixelFormat(GpuContext.device.hDC, PixelFormat, &pfdDefault))
         {	
         Log << "Error! Cannot init pixel format.\n";
         GpuContext.device.destroy();		
         return false;
         }  
      
      // Creating OpenGL Rendering Context
      GpuContext.device.hRC      = new HGLRC();
      GpuContext.device.contexts = 1; 
      GpuContext.device.hRC[0]   = wglCreateContext(GpuContext.device.hDC);
      if (GpuContext.device.hRC[0] == NULL)
         {
         Log << "Error! Cannot create window rendering context.\n";
         GpuContext.device.destroy();						
         return false;
         }  
      
      // Activating rendering context
      if(!wglMakeCurrent(GpuContext.device.hDC, GpuContext.device.hRC[0]))
         {	
         Log << "Error! Cannot activate rendering context.\n";
         GpuContext.device.destroy();	
         return false;
         } 
      
      return true;
      }

      void InitDisplaysInfo(void)
      {
#ifdef EN_PLATFORM_WINDOWS
      // Display Device settings
      DISPLAY_DEVICE DispDevice;         
      memset(&DispDevice, 0, sizeof(DispDevice));
      DispDevice.cb = sizeof(DispDevice);

      uint32 display = 0;
      while(EnumDisplayDevices(NULL, display, &DispDevice, EDD_GET_DEVICE_INTERFACE_NAME))
         {
         DEVMODE DispMode;
         ZeroMemory(&DispMode, sizeof(DispMode));
         DispMode.dmSize = sizeof(DispMode);

         if ( !EnumDisplaySettings(DispDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &DispMode) )
            break;

         if (DispDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
            {
            Log << "Display: " << display << endl;
            Log << "Name: " << stringFromWchar((wchar_t*)&DispDevice.DeviceName, 32) << endl;
            Log << "Name: " << stringFromWchar((wchar_t*)&DispDevice.DeviceString, 128) << endl;
            if (DispDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) 
               Log << "Main Monitor!" << endl;
            Log << "Resolution: " << DispMode.dmPelsWidth << "x" << DispMode.dmPelsHeight << endl;
            
            }

         display++;
         }
#endif
      }

      bool ScreenCreate(const ScreenSettings& settings, const string& title)
      {
      // Checks if screen isn't already created.
      if (GpuContext.screen.created)
         return false;
     
      // If gpu is already creating screen break.
      if (!GpuContext.screen.lock.tryLock())
         return false;
     
      // Check if gpu/drivers support requested 
      // shading language version.
      if (!GpuContext.device.sl.better(settings.shadingLanguage))
         {
         Log << "Error! Graphic card or current graphic drivers\n";
         Log << "       don't support requested shading language.\n";	
         return false;
         }
     
      // Clear settings 
      GpuContext.device.destroy();		
      GpuContext.support.defaults();
      GpuContext.screen.defaults();
      GpuContext.state.defaults();
      
      //gl20::BuffersClear();
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      ClearTextureSupport();
#endif
      gl20::ProgramsClear();
  
      // Create window with default OpenGL context
      if (!CreateDummyWindow())
         return false;
     
      // To get list of supported extensions, this 
      // code should be used since OpenGL 3.0 Core:
      //
      // uint32 count = 0;
      // glGetIntegerv(GL_NUM_EXTENSIONS, &count);
      // for(uint16 i=0; i<count; i++)
      //    support.m_extensions.insert(support.m_extensions.end(),string((char*)glGetStringi(GL_EXTENSIONS,i));
      //
      // But data about gpu, are gathered in dummy
      // OpenGL context which works in deprecaded
      // mode. This forces implementation in old
      // deprecated way:
     
      // Creates table of supported extensions strings
      string tmp;
      sint32 begin, end;
      tmp   = string((char*)glGetString(GL_EXTENSIONS));
      begin = 0;
      end   = tmp.find(' ',0);
      while(end != string::npos)
           {
           GpuContext.support.extensions.insert(GpuContext.support.extensions.end(),tmp.substr(begin,end-begin));
           begin = end+1;
           end   = tmp.find(' ',begin);
           }
     
      // This extension together with WGL_EXT_swap_interval
      // should be returned in list of supported extensions
      // after calling glGetString(GL_EXTENSIONS);
      // Because NVidia drivers don't follow this rule it's
      // specification says as follow:
      // 
      // "Applications should call wglGetProcAddress to see 
      // whether or not wglGetExtensionsStringARB is supported."
      // (http://www.opengl.org/registry/specs/ARB/wgl_extensions_string.txt)
      //
      bool wglexts = false;
      wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");  
      if (wglGetExtensionsStringARB != NULL)
         {
         tmp = string((char*)wglGetExtensionsStringARB(GpuContext.device.hDC));
         wglexts = true;
         }
      else
         {
         wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
         if (wglGetExtensionsStringEXT != NULL)
            {
            tmp = string((char*)wglGetExtensionsStringEXT()); 
            wglexts = true; 
            }
         }
     
      // If it is possible to obtain WGL extensions list add
      // them to the rest of supported extensions.
      if (wglexts)
         {
         begin = 0;
         end   = tmp.find(' ',0);
         while(end != string::npos)
              {
              GpuContext.support.extensions.insert(GpuContext.support.extensions.end(),tmp.substr(begin,end-begin));
              begin = end+1;
              end   = tmp.find(' ',begin);
              }
         }
     
      #ifdef EN_DEBUG
      // DEBUG INFO - Prints supported extensions
      Log << endl;
      Log << "Rendering Context debug information: " << endl;
      Log << "==================================== " << endl;
      Log << "Features: " << endl;
      Log << "  OpenGL: " << GpuContext.device.api.high << "." << GpuContext.device.api.medium << endl;
      Log << "  GLSL  : " << GpuContext.device.sl.high << "." << GpuContext.device.sl.medium << endl; 
      Log << "  Vendor: " << GpuContext.device.vendor << endl;
      Log << "  Device: " << GpuContext.device.renderer << endl;
      Log << "Supported " << GpuContext.support.extensions.size() << " extensions: " << endl;
      for(set<string>::iterator i = GpuContext.support.extensions.begin(); i != GpuContext.support.extensions.end(); i++)
         Log << "  " << *i << endl;
      Log << endl;
      #endif
     
      // To select proper Pixel Format for final window,
      // first pointers to specified wgl functions needs
      // to be gathered.
      BindWglFunctionsPointers();

      // Pixel Format is choosed, supported shading
      // language and graphic API versions are know
      // together with supported extensions. Creation
      // of proper window begins.
      GpuContext.device.destroy();	
     
      HINSTANCE             AppInstance;        // Application handle
      WNDCLASS              Window;             // Window class
      DEVMODE               DispDevice;         // Display Device settings
      DWORD                 Style;              // Window style
      DWORD                 ExStyle;            // Window extended style
      RECT                  WindowRect;         // Window rectangle
      
      // Get this application instance
      AppInstance          = (HINSTANCE)GetWindowLongPtr(GetConsoleWindow(), GWLP_HINSTANCE);  // GWL_HINSTANCE is deprecated in x64 environment
     
      // Window settings
      Window.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Have its own Device Context and also cannot be resized        // Oculus uses CLASSDC which is not thread safe!    
      Window.lpfnWndProc   = (WNDPROC) WinEvents;                  // Procedure that handles OS evets
      Window.cbClsExtra    = 0;                                    // No extra window data
      Window.cbWndExtra    = 0;                                    //
      Window.hInstance     = AppInstance;                          // Handle to instance of program that this window belongs to
      Window.hIcon         = LoadIcon(NULL, IDI_WINLOGO);          // Load default icon
      Window.hCursor       = LoadCursor(NULL, IDC_ARROW);          // Load arrow pointer
      Window.hbrBackground = NULL;                                 // No background (OpenGL will handle this)
      Window.lpszMenuName  = NULL;                                 // No menu
      Window.lpszClassName = L"WindowClass";                       // Class name
     
      // Window size      
      WindowRect.left      = (long)0;      
      WindowRect.right     = (long)settings.width;  
      WindowRect.top       = (long)0;    
      WindowRect.bottom    = (long)settings.height;
     
      // Display area position on desktop
      uint32v2 position    = uint32v2(0, 0);

      // Registering Window Class
      if (!RegisterClass(&Window))	
         {					
         Log << "Error! Cannot register window class." << endl;
         GpuContext.device.destroy();	
         GpuContext.support.defaults();
         return false;
         }
      GpuContext.device.registered = true;

      // Preparing for fullscreen window
      if (settings.mode == Fullscreen)
         {
         // Native resolution
         uint32 nativeResolutionX = 0;
         uint32 nativeResolutionY = 0;

         // Display Device settings
         DISPLAY_DEVICE Device;
         memset(&Device, 0, sizeof(Device));
         Device.cb = sizeof(Device);

         // Enumerate through active monitors to find the one requested
         if (settings.display >= 0)
            {
            uint32 display = 0;
            uint32 active = 0;
            while(EnumDisplayDevices(NULL, display, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
               {
               if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
                  {
                  if (settings.display == active)
                     {
                     DEVMODE DispMode;
                     memset(&DispMode, 0, sizeof(DispMode));
                     DispMode.dmSize = sizeof(DispMode);
                     
                     if ( !EnumDisplaySettings(Device.DeviceName, ENUM_REGISTRY_SETTINGS, &DispMode) )
                        break;
                     nativeResolutionX = DispMode.dmPelsWidth;
                     nativeResolutionY = DispMode.dmPelsHeight;
                     position.x        = DispMode.dmPosition.x;
                     position.y        = DispMode.dmPosition.y; 
                     GpuContext.screen.display = active;

                     // TODO: Add "found" flag, and verify if settings.display is not exceeding current displays count!
                     break;
                     }
                  active++;
                  }
               display++;
               }
            }

         // Display Device settings
         memset(&DispDevice,0,sizeof(DispDevice));         // Clearing structure
         DispDevice.dmSize       = sizeof(DispDevice);     // Size of this structure
         DispDevice.dmBitsPerPel = 32;                     // bpp
         DispDevice.dmFields     = DM_BITSPERPEL | 
                                   DM_PELSWIDTH  | 
                                   DM_PELSHEIGHT;          // Sets which fields were filled
         if (settings.display >= 0)
            memcpy(&DispDevice.dmDeviceName, &Device.DeviceName, sizeof(DispDevice.dmDeviceName));   // Specifies which display to use
         if ( (settings.width == 0) && 
              (settings.height == 0) )
            {
            DispDevice.dmPelsWidth  = nativeResolutionX;   // Display native widht
            DispDevice.dmPelsHeight = nativeResolutionY;   // Display native height
            }
         else
            {
            DispDevice.dmPelsWidth  = settings.width;      // Screen widht
            DispDevice.dmPelsHeight = settings.height;     // Screen height
            }

         // Changing Display settings
         if (settings.display >= 0)
            {
            if (ChangeDisplaySettingsEx(Device.DeviceName, &DispDevice, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
               {					
               Log << "Error! Cannot change display settings for fullscreen." << endl;
               GpuContext.device.destroy();	
               GpuContext.support.defaults();
               return false;
               }
            }
         else
            {
            if (ChangeDisplaySettings(&DispDevice, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
               {					
               Log << "Error! Cannot change display settings for fullscreen." << endl;
               GpuContext.device.destroy();	
               GpuContext.support.defaults();
               return false;
               }
            }

         // Setting additional data
         Style   = WS_POPUP;                               // Window style
         ExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;        // Window extended style
         ShowCursor(FALSE);                                // Hide mouse 
         }
      else
         {
         // We are creating window and we don't have it's size
         if ( (settings.width == 0) && 
              (settings.height == 0) )
            {
            // Display Device settings
            DISPLAY_DEVICE Device;
            memset(&Device, 0, sizeof(Device));
            Device.cb = sizeof(Device);

            uint32 display = 0;
            while(EnumDisplayDevices(NULL, display, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
               {
               if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
                  {
                  DEVMODE DispMode;
                  memset(&DispMode, 0, sizeof(DispMode));
                  DispMode.dmSize = sizeof(DispMode);
                  
                  if ( !EnumDisplaySettings(Device.DeviceName, ENUM_REGISTRY_SETTINGS, &DispMode) )
                     break;
                  WindowRect.right  = (long)DispMode.dmPelsWidth;
                  WindowRect.bottom = (long)DispMode.dmPelsHeight;
                  break;
                  }
               display++;
               }
            }

         // Preparing for desktop window (adding size of borders and bar)
         if (settings.mode != BorderlessWindow)
            {
            Style   = WS_CLIPSIBLINGS |                       // \_Prevents from overdrawing
                      WS_CLIPCHILDREN |                       // / by other windows
                      WS_POPUP        | 
                      WS_OVERLAPPEDWINDOW;                    // Window style
            ExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;     // Window extended style  
         
            AdjustWindowRectEx(&WindowRect, Style, FALSE, ExStyle);  // Calculates true size of window with bars and borders
            }
         else
         // Borderless window
            {
            Style   = WS_POPUP;                               // Window style                // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
            ExStyle = 0;                                      // Window extended style  
            }
         }

      // Creating Window in modern way
      wstring windowTitle = wstring(title.begin(),title.end());
      GpuContext.device.hWnd = CreateWindowEx( 
         ExStyle,                             // Extended style 
         L"WindowClass",                      // Window class name
         (LPCWSTR)windowTitle.c_str(),        // Title of window            
         Style,                               // Additional styles
         position.x, position.y,              // Position on desktop
         WindowRect.right  - WindowRect.left, // True window widht
         WindowRect.bottom - WindowRect.top,  // True window height
         NULL, //GetDesktopWindow(),                  // Desktop is parent window
         NULL,                                // No menu
         AppInstance,                         // Handle to this instance of program
         NULL );                              // Won't pass anything
     
      // Check if window was created
      if (GpuContext.device.hWnd == NULL)
         {
         Log << "Error! Cannot create window.\n";
         GpuContext.device.destroy();	
         GpuContext.support.defaults();	
         return false;
         }  

      // Acquiring Device Context
      GpuContext.device.hDC = GetDC(GpuContext.device.hWnd);
      if (GpuContext.device.hDC == NULL)
         {
         Log << "Error! Cannot create device context.\n";		
         GpuContext.device.destroy();	
         GpuContext.support.defaults();	
         return false;
         }  

      // Advanced Pixel Format Descriptor
      float  fAttributes[] = {0, 0};
      uint32 iAttributes[] = { WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, //   - Hardware accelerated 
                               WGL_SUPPORT_OPENGL_ARB, TRUE,                    // _ - OpenGL supporting
                               WGL_DOUBLE_BUFFER_ARB, TRUE,                     //  |_ DOUBLE BUFFER - By exchange 
                               WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,      // _| 
                               WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,           //  |
                               WGL_COLOR_BITS_ARB, 24,                          //  +- COLOR - Minimum 32bit RGBA (R8G8B8A8)
                               WGL_RED_BITS_ARB,   8,                           //  |
                               WGL_GREEN_BITS_ARB, 8,                           //  |
                               WGL_BLUE_BITS_ARB,  8,                           //  |
                               WGL_ALPHA_BITS_ARB, 8,                           // _|
                               WGL_DEPTH_BITS_ARB, 24,                          //   - DEPTH   - Minimum 24bit
                               WGL_STENCIL_BITS_ARB, 8,                         //   - STANCIL - Minimum 8bit     
                               WGL_DRAW_TO_WINDOW_ARB, TRUE,                    //   - Can run in WINDOW mode
                               WGL_SAMPLES_ARB, settings.samples,               //   - MULTISAMPLING - 1,2,4,8,16 samples
                               0, 0 };
     
      // Choosing advanced Pixel Format in modern way  
      uint32 PixelFormat = NULL;         // Format of pixel
      bool choosedPixelFormat = false;   // Flag indicating if apropriate pixel format was choosed 
      if (GpuContext.support.extension(ARB_pixel_format))
         {
         uint32 numFormats = 0;
         if (wglChoosePixelFormatARB(GpuContext.device.hDC, (const int*)iAttributes, fAttributes, 1, (int*)&PixelFormat, (unsigned int*)&numFormats))
            if (numFormats >= 1)
               choosedPixelFormat = true;
         }
     
      // Old way for choosing advanced Pixel Format
      if (!choosedPixelFormat &&
          GpuContext.support.extension(EXT_pixel_format))
         {
         uint32 numFormats;
         if (wglChoosePixelFormatEXT(GpuContext.device.hDC, (const int*)iAttributes, fAttributes, 1, (int*)&PixelFormat, (unsigned int*)&numFormats))
            if (numFormats >= 1)
               choosedPixelFormat = true;
         }
     
      // Basic Pixel Format
      if (!choosedPixelFormat)
         PixelFormat = ChoosePixelFormat(GpuContext.device.hDC, &pfdLegacy);
     
      // If couldn't find Pixel Format report error
      if (PixelFormat == NULL)
         {
         Log << "Error! Cannot find aproprieate pixel format.\n";	
         GpuContext.device.destroy();
         GpuContext.support.defaults();	
         return false;
         } 

      // Empty Pixel Format descriptor for returned data
      PIXELFORMATDESCRIPTOR pfdEmpty;
      memset(&pfdEmpty, 0, sizeof(pfdEmpty));

      // Activating Pixel Format
      if (!SetPixelFormat(GpuContext.device.hDC, PixelFormat, &pfdEmpty))
         {	
         Log << "Error! Cannot init pixel format.\n";
         GpuContext.device.destroy();	
         GpuContext.support.defaults();
         return false;
         } 
     
      // Find Graphic API version range for specified SL version
      uint8 index = 0;
      for(uint8 i=0; i<GLSLRevisions; i++)
         if (glslSupport[i].sl == settings.shadingLanguage)
            {
            index = i;
            break;
            }
     
      // Alloc handles for OpenGL Rendering Contexts
      GpuContext.device.contexts = Scheduler.workers(); 
      GpuContext.device.hRC      = new HGLRC[GpuContext.device.contexts];
     
      // If application requires minimum GLSL 1.30 functionality,
      // OpenGL 3.0 Core profile or better context needs to be
      // created.
      if ( settings.shadingLanguage.better(GLSL_1_30) && 
           GpuContext.support.extension(ARB_create_context) )
         {
         // Engine works only with Core profiles
         uint32 glAttributes[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                                   WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                                   WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                                   GL_CONTEXT_PROFILE_MASK, GL_CONTEXT_CORE_PROFILE_BIT,
                                   0, 0 };
     
         // Debug version supports better debugging
      #ifdef _DEBUG
         glAttributes[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
      #endif 
     
         // Try to create window with best OpenGL API version
         for(uint8 i=glslSupport[index].max; i>=glslSupport[index].min; --i)
            {
            glAttributes[1] = gl::openglVersions[i].high;
            glAttributes[3] = gl::openglVersions[i].medium;
     
            // Below 3.2 version we are not using profiles
            if ( !gl::openglVersions[i].better(OpenGL_3_2) )
               {
               glAttributes[6] = 0;
               glAttributes[7] = 0;  
               }
     
            // Try to create OpenGL context in new way
            GpuContext.device.hRC[0] = wglCreateContextAttribsARB(GpuContext.device.hDC, 0, (const int *)glAttributes);
            if (GpuContext.device.hRC[0] != NULL)
               {
               GpuContext.screen.api = gl::openglVersions[i];
               break;
               }
            }
         }
      // If application requires older shading language ( 1.10 or 1.20 )
      // old method of creating OpenGL context will be used. In result
      // any OpenGL version context can be created, but it will be in
      // compatibility mode. For this cases engine should use maximum
      // OpenGL 2.1.2 capabilities.
      else
         {
         GpuContext.device.hRC[0] = wglCreateContext(GpuContext.device.hDC);
         if (GpuContext.device.hRC != NULL)
            GpuContext.screen.api = GpuContext.device.api;
         }
      
      // Checking if main OpenGL Rendering Context was created properly
      if (GpuContext.device.hRC[0] == NULL)
         {
         Log << "Error! Cannot create window rendering context.\n";
         GpuContext.device.destroy();	
         GpuContext.support.defaults();				
         return false;
         }  

      // Activating rendering context
      if(!wglMakeCurrent(GpuContext.device.hDC, GpuContext.device.hRC[0]))
         {	
         Log << "Error! Cannot activate rendering context.\n";
         GpuContext.device.destroy();	
         GpuContext.support.defaults();
         return false;
         } 
      
      // In Windows, engine needs to get functions pointers from 
      // OpenGL ICD drivers, because Microsoft's opengl32.dll does 
      // not support extensions higher than v1.1
      BindFunctionsPointers();
      
      // Create worker Rendering Contexts
      for (uint8 i=1; i<GpuContext.device.contexts; ++i)
          Scheduler.run(new WorkerContext(i, settings), i);
      
      // Setting created window as current 
      ShowWindow(GpuContext.device.hWnd, SW_SHOW);
      SetForegroundWindow(GpuContext.device.hWnd);
      SetFocus(GpuContext.device.hWnd);

      // Updating screen information
      GpuContext.screen.fullscreen = settings.mode == Fullscreen ? true : false;
      GpuContext.screen.samples    = settings.samples;
      GpuContext.screen.sl         = settings.shadingLanguage;
      GpuContext.screen.created    = true;
      if ( (settings.width == 0) && 
           (settings.height == 0) )
         {
         if (settings.mode == Fullscreen)
            {
            GpuContext.screen.width   = static_cast<uint16>(DispDevice.dmPelsWidth);    // Display native widht
            GpuContext.screen.height  = static_cast<uint16>(DispDevice.dmPelsHeight);   // Display native height
            }
         else
            {
            GpuContext.screen.width   = static_cast<uint16>(WindowRect.right);          // Display native widht
            GpuContext.screen.height  = static_cast<uint16>(WindowRect.bottom);         // Display native height
            }
         }
      else
         {
         GpuContext.screen.width   = settings.width;
         GpuContext.screen.height  = settings.height;
         }

      // TODO: Set scissor test area to full screen
      //gpu.render.test.scissor.m_width   = w;
      //gpu.render.test.scissor.m_height  = h;

      #ifdef _DEBUG
      Log << " Window created: " << endl;
      Log << "  OpenGL: " << GpuContext.screen.api.high << "." << GpuContext.screen.api.medium << endl;
      Log << "  GLSL  : " << GpuContext.screen.sl.high << "." << GpuContext.screen.sl.medium << endl; 
      #endif
      
      // Gather information about features supported by this context
      gpu::gl::GatherContextCapabilities();
      gpu::gl::StateInit();
      gpu::gl20::BuffersInit();  
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
      gpu::InitSamplers();
      gpu::InitTextureSupport();
#endif  
      gpu::gl20::ProgramsInit();

      // Clear errors indicator
      glGetError();

      // Set default viewport
      Profile( glViewport(0, 0, GpuContext.screen.width, GpuContext.screen.height) )
      
      // Create empty VAO for draw calls without input buffers
      if (GpuContext.screen.support(OpenGL_3_0))
         Profile( glGenVertexArrays(1, &GpuContext.emptyVAO) )

      // Enable seamless texture filering
      if (GpuContext.screen.support(OpenGL_3_2))
         Profile( glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) )

// <<<<<< NEW GPU CONTEXT IMPLEMENTATION

      // Init Sampler Cache
      GpuContext.sampler.cacheSize = 64;
      GpuContext.sampler.cache     = new Context::Sampler::SamplerCache[GpuContext.sampler.cacheSize];
      memset(GpuContext.sampler.cache, 0, sizeof(Context::Sampler::SamplerCache) * GpuContext.sampler.cacheSize);

      // Init texture units cache
      GpuContext.textureUnit.texture = new Ptr<Texture>[GpuContext.support.maxTextureUnits];
      GpuContext.textureUnit.sampler = new Ptr<Sampler>[GpuContext.support.maxTextureUnits];
      GpuContext.textureUnit.metric  = new uint32[GpuContext.support.maxTextureUnits];
      memset(GpuContext.textureUnit.metric, 0, sizeof(uint32) * GpuContext.support.maxTextureUnits);

// <<<<<< NEW GPU CONTEXT IMPLEMENTATION

      // Creating default resources
      ResourcesContext.create();

      // Screen creation completed.
      GpuContext.screen.lock.unlock();
      return true;
      }

      // Destroys current window
      bool ScreenDestroy(void)
      {  
      if (!GpuContext.screen.created)
          return false;
     
      // Destroy default empty VAO
      if (GpuContext.screen.support(OpenGL_3_0))
         Profile( glDeleteVertexArrays(1, &GpuContext.emptyVAO) )

      // Free all resources connected to gpu
      ResourcesContext.destroy();

// <<<<<< NEW GPU CONTEXT IMPLEMENTATION

      // Clear Sampler Cache
      for(uint32 i=0; i<GpuContext.sampler.cacheSize; ++i)
         GpuContext.sampler.cache[i].sampler = nullptr;
      delete [] GpuContext.sampler.cache;

      // Clear texture units cache
      for(uint16 slot=0; slot<GpuContext.support.maxTextureUnits; ++slot)
         {
         GpuContext.textureUnit.texture[slot] = nullptr;
         GpuContext.textureUnit.sampler[slot] = nullptr;
         }
      delete [] GpuContext.textureUnit.texture;
      delete [] GpuContext.textureUnit.sampler;
      delete [] GpuContext.textureUnit.metric;

// <<<<<< NEW GPU CONTEXT IMPLEMENTATION

      // Returning to old display settings
      if (GpuContext.screen.fullscreen)
         {
         ChangeDisplaySettings(NULL,0);
         ShowCursor(TRUE);
         }
     
      // Unbind OpenGL functions pointers
      UnbindFunctionsPointers();
      UnbindWglFunctionsPointers();
     
      // Clear whole state
      GpuContext.device.destroy();
      GpuContext.defaults();

      return true;
      }

      void Display(void)
      { 
#ifndef GPU_IMMEDIATE_MODE
      // Update deffered state in driver
      for(uint32 i=0; i<GpuContext.state.dirty; ++i)
         {
         GpuContext.state.update[i]();
         GpuContext.state.update[i] = NULL;  // For debug purposes
         }
      GpuContext.state.dirty = 0;
#endif

      // Swap buffers
      SwapBuffers(GpuContext.device.hDC);
      }

      #endif
      }
   }
}

#endif
