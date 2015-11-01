/*

 Ngine v5.0
 
 Module      : OpenGL State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/opengl/glState.h"

namespace en
{
   namespace gpu
   {
   // Comparison Method 
   static const uint16 TranslateCompareMethod[CompareMethodsCount] = 
      {
      GL_NEVER   ,   // Never          
      GL_ALWAYS  ,   // Always         
      GL_LESS    ,   // Less           
      GL_LEQUAL  ,   // LessOrEqual    
      GL_EQUAL   ,   // Equal          
      GL_GREATER ,   // Greater        
      GL_GEQUAL  ,   // GreaterOrEqual 
      GL_NOTEQUAL    // NotEqual   
      };
   }
}