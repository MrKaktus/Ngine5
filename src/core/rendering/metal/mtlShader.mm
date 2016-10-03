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

#include "core/defines.h"
#include "core/types.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/log/log.h"
#include "utilities/osxStrings.h"
#include "core/rendering/metal/mtlShader.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   
   ShaderMTL::ShaderMTL(id <MTLLibrary> library, id <MTLFunction> function) :
      library(library),
      function(function)
   {
   }
   
   ShaderMTL::~ShaderMTL()
   {
   [function release];
   [library release];
   }
   
   Ptr<Shader> MetalDevice::createShader(const string& source, const string& entrypoint)
   {
   Ptr<ShaderMTL> shader = nullptr;

   NSString* code = stringTo_NSString(source);
   
   MTLCompileOptions* options = [MTLCompileOptions alloc];
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
   options.languageVersion    = MTLLanguageVersion1_2;
   
   NSError* error = nil;
   id <MTLLibrary> library = nil;
   library = [device newLibraryWithSource:code options:options error:&error];
   if (error)
      {
      if ([error code] == MTLLibraryErrorCompileWarning)
         {
         Log << "Warning! Shader compiled with warnings:\n";
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
         return Ptr<Shader>(nullptr);
         }
      }
    
   error = nil;
   id <MTLFunction> function = nil;
   function = [library newFunctionWithName:stringTo_NSString(entrypoint)];
   if (error)
      {
      Log << "Error! Failed to find shader entry point \"" << entrypoint << "\" in library created from source.\n";
      [library release];
      return Ptr<Shader>(nullptr);
      }

   return ptr_dynamic_cast<Shader, ShaderMTL>(Ptr<ShaderMTL>(new ShaderMTL(library, function)));
   }

      
//    NSString* shaderText = [NSString stringWithContentsOfFile:stringTo<NSString*>(filepath) encoding:NSUTF8StringEncoding error:&error];
//    if (!shaderText)
//    {
//        const char *messageFormat = "failed to load shader file.  error is %s";
//        // [[error description] UTF8String]
//
//    }






   //id <MTLLibrary>  gCommonLib    = ATFMTLNewLibraryWithSourceFile(gDevice, @"SimpleShader.metal");
   //id <MTLFunction> gBlitVertProg = ATFMTLNewFunctionFromLibrary(gCommonLib, @"vertex");
   //id <MTLFunction> gBlitFragProg = ATFMTLNewFunctionFromLibrary(gCommonLib, @"fragment");





   }
}
#endif
