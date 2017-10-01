/*

 Ngine v5.0
 
 Module      : Metal Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_SHADER
#define ENG_CORE_RENDERING_METAL_SHADER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/shader.h"

namespace en
{
   namespace gpu
   {
   class ShaderMTL : public Shader
      {
      public:
      id<MTLFunction> function;
      
      ShaderMTL(id<MTLFunction> function);
      virtual ~ShaderMTL();
      };
      
   class ShaderLibraryMTL : public ShaderLibrary
      {
      public:
      id<MTLLibrary> library;
      
      ShaderLibraryMTL(id<MTLLibrary> library);
      virtual ~ShaderLibraryMTL();

      virtual Ptr<Shader> createShader(const ShaderStage stage,
                                       const string& entrypoint);
      };
   }
}
#endif

#endif
