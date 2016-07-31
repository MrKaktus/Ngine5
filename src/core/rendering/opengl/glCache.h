/*

 Ngine v5.0
 
 Module      : OpenGL State Machine Cache.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_CACHE
#define ENG_CORE_RENDERING_OPENGL_CACHE

#include "core/defines.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/opengl/opengl.h"

namespace en
{
   namespace gpu
   {
   typedef union 
      {
      struct Bitfield
         {
         uint32 blending  : 1; // Blending enabled       
         uint32 logic     : 1; // Logic Operation enabled (ten refer to separate bitfield for logicOp)
         uint32 srcRGB    : 5;
         uint32 srcAlpha  : 5;
         uint32 dstRGB    : 5;
         uint32 dstAlpha  : 5;
         uint32 rgbFunc   : 3; // Equation RGB
         uint32 alphaFunc : 3; // Equation Alpha
         uint32 writeMask : 4; // 4 channels
         };
      uint32 vector;
      } CachedAttachmentBlendState;

   #define CacheBlendFunctionShift 2
   #define CacheBlendFunctionMask  0xFFFFF << CacheBlendFunctionShift
   #define CacheBlendEquationShift 22
   #define CacheBlendEquationMask  0x3F << CacheBlendEquationShift



   typedef union 
      {
      struct Bitfield
         {
         uint8 logicOp   : 4; // Logic Operation
         uint8 reserved  : 4;
         };
      uint8 vector;
      } CachedAttachmentLogicState;

   struct DeviceVector
      {
      CachedAttachmentBlendState blend[MaxColorAttachmentsCount];   // Current blend state of Color Attachments
      CachedAttachmentLogicState logic[MaxColorAttachmentsCount];   // Current logic operation of Color Attachments
      };
   }
}
#endif

#endif