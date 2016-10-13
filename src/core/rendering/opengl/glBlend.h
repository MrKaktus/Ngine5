/*

 Ngine v5.0
 
 Module      : OpenGL Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_BLEND_STATE
#define ENG_CORE_RENDERING_OPENGL_BLEND_STATE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/blend.h"

#include "core/rendering/opengl/glCache.h"

namespace en
{
   namespace gpu
   {
   // Frontend state translated to OpenGL state
   struct BlendAttachmentStateGL
      {
      bool   blending;       // Enablers
      bool   logicOperation;
      uint16 srcRGB;         // Blend parameters
      uint16 dstRGB;
      uint16 rgbFunc;
      uint16 srcAlpha;
      uint16 dstAlpha;
      uint16 alphaFunc;
      uint16 logic;          // Logic operation
      ColorMask      writeMask;
      };

   class BlendStateGL : public BlendState
      {
      public:
      // OpenGL State
      float4 blendColor;
      uint32 attachments;
      BlendAttachmentStateGL* color;
      
      // OpenGL State Compressed
      CachedAttachmentBlendState cachedBlend[MaxColorAttachmentsCount];
      CachedAttachmentLogicState cachedLogicOp[MaxColorAttachmentsCount];

      BlendStateGL(const BlendStateInfo& state,
                   const uint32 attachments, 
                   const BlendAttachmentInfo* color);
      virtual ~BlendStateGL();
      };

   }
}
#endif

#endif