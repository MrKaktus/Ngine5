/*

 Ngine v5.0
 
 Module      : Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_SHADER
#define ENG_CORE_RENDERING_SHADER

#include "core/types.h"
#include "core/rendering/state.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   // Pipeline shader stages
   enum class ShaderStage : uint32
        {
        Vertex                    = 0,
        Control                      ,
        Evaluation                   ,
        Geometry                     ,
        Fragment                     ,
        Compute                      ,
        Count
        };
      
   class Shader : public SafeObject<Shader>
      {
      public:
      virtual ~Shader() {};
      };
   }
}
#endif
