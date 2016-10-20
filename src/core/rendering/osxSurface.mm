/*

 Ngine v5.0
 
 Module      : OSX Shared Surface.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/osxSurface.h"

#include "core/types.h"
#include "core/rendering/device.h"
//#include "Foundation/NSDictionary.h"

#pragma push_macro("aligned")
#undef aligned
#include "Foundation/Foundation.h"
#include "CoreVideo/CoreVideo.h"
#pragma pop_macro("aligned")


namespace en
{
   namespace gpu
   {
   SharedSurfaceOSX::SharedSurfaceOSX(const IOSurfaceRef _surface, const uint32v2 _resolution) :
      surface(_surface),
      resolution(_resolution)
   {
   }
   
   SharedSurfaceOSX::~SharedSurfaceOSX()
   {
   CFRelease(surface);
   }

   Ptr<SharedSurface> createSharedSurface(const uint32v2 resolution)
   {
   assert( resolution.width );
   assert( resolution.height );
   
   Ptr<SharedSurfaceOSX> result = nullptr;

   // Create backing IOSurface
   NSDictionary *surfaceDict = @{ (id)kIOSurfaceWidth           : @((int)resolution.width),
                                  (id)kIOSurfaceHeight          : @((int)resolution.height),
                                  (id)kIOSurfacePixelFormat     : @(kCVPixelFormatType_32BGRA),
                                  (id)kIOSurfaceBytesPerElement : @(4)
                                };
      
   result = new SharedSurfaceOSX(IOSurfaceCreate((CFDictionaryRef)surfaceDict), resolution);

   return ptr_reinterpret_cast<SharedSurface>(&result);
   }
      
      
   // DUMMY OPENGL CONTEXT FOR SHARED TEXTURE HANDLE CREATON
   ///////////////////////////////////////////////////////////
   
   
   osxOpenGLContext::osxOpenGLContext()
   {
   // Create Pixel Format
   NSOpenGLPixelFormatAttribute attributes[] =
      {
      NSOpenGLPFAAccelerated,
      NSOpenGLPFADoubleBuffer,
      NSOpenGLPFADepthSize,     24,
      NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
      0
      };

   pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
   
   assert( pixelFormat );
   
   // Create OpenGL Context
   context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
   [context makeCurrentContext];
   }
   
   osxOpenGLContext::~osxOpenGLContext()
   {
   [pixelFormat release];
   [context release];
   }

   uint32 osxOpenGLContext::createSharedTexture(Ptr<SharedSurface> backingSurface)
   {
   // TODO: In future it should be part of OpenGLDevice and return Ptr<Texture>
   //Ptr<TextureGL> result = nullptr;
   
#if defined(EN_PLATFORM_IOS)
   // IOSurfaces are not supported on iOS
   assert( 0 );
#endif

#if defined(EN_PLATFORM_OSX)
   Ptr<SharedSurfaceOSX> ioSurface = ptr_reinterpret_cast<SharedSurfaceOSX>(&backingSurface);
   
   [context makeCurrentContext];
    
   CGLContextObj glCoreContext = [context CGLContextObj];
   
   // Generate OpenGL
   GLuint glHandle = 0u;
   glGenTextures(1, &glHandle);
   
   // Assign IOSurface as texture backing
   glBindTexture(GL_TEXTURE_RECTANGLE_ARB, glHandle);
   CGLError error;
   error = CGLTexImageIOSurface2D(glCoreContext,
                                  GL_TEXTURE_RECTANGLE_ARB, GL_RGBA8,
                                  ioSurface->resolution.width,
                                  ioSurface->resolution.height,
                                  GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
                                  ioSurface->surface, 0u);
    
   assert(error == kCGLNoError);
   glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0u);
#endif

   return glHandle;
   //return ptr_reinterpret_cast<Texture>(&result);
   }
   
   void osxOpenGLContext::destroySharedTexture(const uint32 handle)
   {
   glDeleteTextures(1, &handle);
   }
      
   OpenGLContext* createDummyOpenGLContext(void)
   {
   return reinterpret_cast<OpenGLContext*>(new osxOpenGLContext());
   }
   
   }
}
