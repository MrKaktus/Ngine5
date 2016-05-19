/*

 Ngine v5.0
 
 Module      : OpenGL GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/log/log.h"
#include "core/rendering/opengl/glDevice.h"
//#include "core/rendering/opengl/glAPI.h"

//#include "core/rendering/opengl/glBlend.h"
//#include "core/rendering/opengl/glRenderPass.h"
//#include "core/rendering/opengl/glTexture.h"

namespace en
{
   namespace gpu
   {
   OpenGLDevice::OpenGLDevice() :
      lastResult(0),
      CommonDevice()
   {
   }

   OpenGLDevice::~OpenGLDevice()
   {
   }
   
   // Checks OpenGL eror state
   bool IsError(const char* function)
   {
   // Check for errors
   uint32 lastResult = glGetError();   // TODO: Move it to be per device
   if (lastResult == GL_NO_ERROR)
      return false;
      
   // Compose error message
   string info;
   info += "ERROR: OpenGL function ";
   info += function;
   info += " caused error:\n";
   info += "       ";
   while(lastResult)
      {
      // Create error message
      if (lastResult == GL_INVALID_ENUM)
         info += "GL_INVALID_ENUM\n";
      else
      if (lastResult == GL_INVALID_VALUE)
         info += "GL_INVALID_VALUE\n";
      else
      if (lastResult == GL_INVALID_OPERATION)
         info += "GL_INVALID_OPERATION\n";
      else
      if (lastResult == GL_OUT_OF_MEMORY)
         info += "GL_OUT_OF_MEMORY\n";
#ifndef EN_MOBILE_GPU
#ifndef EN_PLATFORM_OSX
      else
      if (lastResult == GL_STACK_OVERFLOW)
         info += "GL_STACK_OVERFLOW\n";
      else
      if (lastResult == GL_STACK_UNDERFLOW)
         info += "GL_STACK_UNDERFLOW\n";
      else
      if (lastResult == GL_TABLE_TOO_LARGE)
         info += "GL_TABLE_TOO_LARGE\n";
#endif
#endif
      else
         info += "Unknown error enum!\n";
      
      // Check for more error messages
      lastResult = glGetError();
      }
      
   Log << info.c_str();
   return true;
   }

   }
}
