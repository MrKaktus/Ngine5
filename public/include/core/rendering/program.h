/*

 Ngine v5.0
 
 Module      : Program.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_PROGRAM
#define ENG_CORE_RENDERING_PROGRAM

#include "core/defines.h"
#include "core/types.h"

#if !defined(EN_PLATFORM_OSX)


#include "core/utilities/Tproxy.h"

#include "core/rendering/state.h"
#include "core/rendering/buffer.h"
#include "core/rendering/sampler.h"
#include "core/rendering/texture.h"

using namespace std;

namespace en
{
   namespace gpu
   {
   // Pipeline stages
   enum PipelineStage
        {
        Vertex                    = 0,
        Control                      ,
        Evaluation                   ,
        Geometry                     ,
        Fragment                     ,
        PipelineStagesCount
        };

   // Parameter
   class Parameter : public ProxyInterface<class ParameterDescriptor>
         {
         public:
         Parameter(class ParameterDescriptor* src);

         const char* name(void);
         uint32      size(void);
         bool set(const void* value);
         bool set(const float value);
         bool set(const float2 value);
         bool set(const float3 value);
         bool set(const float4 value);
         bool set(const float4x4 value);  
         bool set(const sint32 value);
         bool set(const uint32 value);
         bool set(const sint16 value);
         bool set(const uint16 value);
         };

   // Block
   class Block : public ProxyInterface<class BlockDescriptor>
         {
         public:
         Block(class BlockDescriptor* src);

         const char* name(void);
         uint32      size(void);
         bool set(const Ptr<Buffer> buffer);
         };

   // Shader
   class Shader : public ProxyInterface<class ShaderDescriptor>
         {
         public:
         Shader(class ShaderDescriptor* src);
         };

   // Program
   class Program : public ProxyInterface<class ProgramDescriptor>
         {
         public:
         Program(class ProgramDescriptor* src);

         uint32    parameters(void);
         Parameter parameter(const uint32 id);
         Parameter parameter(const string& name);
         uint32    blocks(void);
         Block     block(const uint32 id);
         Block     block(const string& name);

         uint32    samplers(void);                                        // Samplers count
         //const char* name(const uint32 location);                       // Get name of given sampler
         bool      samplerLocation(const char* name, uint32& location);   // Get samplers location
         bool      sampler(const uint32 location, Ptr<Sampler> sampler);  // Set sampler state
         bool      texture(const uint32 location, Ptr<Texture> texture);  // Set texture used by sampler

         //Sampler   sampler(const uint32 id);
         //Sampler   sampler(const string& name);

         bool      draw(const DrawableType type, const uint32 patchSize = 3, const uint32 inst = 1);
         bool      draw(const Ptr<Buffer> buffer, const DrawableType type, const uint32 patchSize = 3, const uint32 inst = 1);
         bool      draw(const Ptr<Buffer> buffer, const Ptr<Buffer> indexBuffer, const DrawableType type, const uint32 patchSize = 3, const uint32 inst = 1);
         };
   }
}
#endif

#endif
