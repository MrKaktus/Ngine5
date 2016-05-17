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
      CommonDevice()
   {
   }

   OpenGLDevice::~OpenGLDevice()
   {
   }
   
   }
}
