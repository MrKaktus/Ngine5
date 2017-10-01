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

#include "core/rendering/metal/mtlShader.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/types.h"
#include "core/log/log.h"
#include "utilities/osxStrings.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   // SHADER
   //////////////////////////////////////////////////////////////////////////
  
   ShaderMTL::ShaderMTL(id<MTLFunction> _function) :
      function(_function)
   {
   }
   
   ShaderMTL::~ShaderMTL()
   {
   deallocateObjectiveC(function);
   }
   
   // SHADER LIBRARY
   //////////////////////////////////////////////////////////////////////////
  
   ShaderLibraryMTL::ShaderLibraryMTL(id <MTLLibrary> _library) :
      library(_library)
   {
   }
   
   ShaderLibraryMTL::~ShaderLibraryMTL()
   {
   deallocateObjectiveC(library);
   }
   
   Ptr<Shader> ShaderLibraryMTL::createShader(const ShaderStage stage, const string& entrypoint)
   {
   Ptr<ShaderMTL> result = nullptr;
   
   NSError* error = nil;
   id <MTLFunction> function = [library newFunctionWithName:stringTo_NSString(entrypoint)];
   if (!error)
      result = new ShaderMTL(function);
   else
      {
      Log << "Error! Failed to find shader entry point \"" << entrypoint << "\" in library created from source.\n";
      deallocateObjectiveC(function);
      }
      
   return ptr_reinterpret_cast<Shader>(&result);
   }
   
   // DEVICE
   //////////////////////////////////////////////////////////////////////////

      // TODO:
      // Vulkan - entrypoint is specified at Pipeline creation I guess
      // Metal  - has libraries, from which we pick functions as entry points
      
      
   Ptr<ShaderLibrary> MetalDevice::createShaderLibrary(const string& source)
   {
   return createShaderLibrary((const uint8*)source.c_str(), source.size());
   }
   
   Ptr<ShaderLibrary> MetalDevice::createShaderLibrary(const uint8* data, const uint64 size)
   {
   Ptr<ShaderLibrary> result = nullptr;
   
   NSString* code = [[NSString alloc] initWithUTF8String:(const char*)data];
   assert( [code length] == size );
   
   MTLCompileOptions* options = allocateObjectiveC(MTLCompileOptions);
   options.preprocessorMacros = nil;
#if defined(EN_PLATFORM_IOS)
   // On mobile use fast math
   options.fastMathEnabled    = YES;
#endif
#if defined(EN_PLATFORM_OSX)
   // On desktop keep to IEEE 754 standard
   options.fastMathEnabled    = NO;
#endif
   // TODO: Dynamic version in future ?
   options.languageVersion    = MTLLanguageVersion2_0; //MTLLanguageVersion1_2;
   
   NSError* error = nil;
   id <MTLLibrary> library = nil;
   library = [device newLibraryWithSource:code options:options error:&error];
   if (error)
      {
      if ([error code] == MTLLibraryErrorCompileWarning)
         {
         Log << "Warning! Shader library compiled with warnings:\n";
         Log << [[error description] UTF8String] << endl;
         }
      else
         {
         if ([error code] == MTLLibraryErrorUnsupported)
            Log << "Error! Compilation failed due to unsupported functionality.\n";
         if ([error code] == MTLLibraryErrorInternal)
            Log << "Error! Internal Metal shader compiler error..\n";
         if ([error code] == MTLLibraryErrorCompileFailure)
            Log << "Error! Failed to compile shader library from source.\n";
         Log << [[error description] UTF8String] << endl;
         
         deallocateObjectiveC(options);
         return result;
         }
      }
    
   deallocateObjectiveC(options);

   result = new ShaderLibraryMTL(library);
   
   return ptr_reinterpret_cast<ShaderLibrary>(&result);
   }




      
      
//    NSString* shaderText = [NSString stringWithContentsOfFile:stringTo<NSString*>(filepath) encoding:NSUTF8StringEncoding error:&error];
//    if (!shaderText)
//    {
//        const char *messageFormat = "failed to load shader file.  error is %s";
//        // [[error description] UTF8String]
//
//    }

   }
}
#endif
