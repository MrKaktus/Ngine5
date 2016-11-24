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
   // Render pipeline shader stages (can be used as a bitmask)
   enum class ShaderStage : uint8
        {
        Vertex                    = 0x01,
        Control                   = 0x02,
        Evaluation                = 0x04,
        Geometry                  = 0x08,
        Fragment                  = 0x10,
        Count
        };
     
   inline ShaderStage operator| (ShaderStage a, ShaderStage b)
   {
   return static_cast<ShaderStage>(underlyingType(a) | underlyingType(b));
   }
 
   class Shader : public SafeObject<Shader>
      {
      public:
      virtual ~Shader() {};
      };
   }
}
#endif
