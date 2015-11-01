/*

 Ngine v5.0
 
 Module      : OpenGL
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               This module implements OpenGL backend.

*/

#ifndef ENG_CORE_RENDERING_OPENGL
#define ENG_CORE_RENDERING_OPENGL

#include "core/defines.h"
#include "core/types.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                    // OpenGL
#include "opengl/include/glext.h"     // OpenGL Extensions 
#include "opengl/include/wglext.h"    // OpenGL Windows Extensions 

#include "core/rendering/opengl/extensions.h"
#include "core/rendering/opengl/opengl12.h"
#include "core/rendering/opengl/opengl13.h"
#include "core/rendering/opengl/opengl14.h"
#include "core/rendering/opengl/opengl15.h"
#include "core/rendering/opengl/opengl20.h"
#include "core/rendering/opengl/opengl21.h"
#include "core/rendering/opengl/opengl30.h"
#include "core/rendering/opengl/opengl31.h"
#include "core/rendering/opengl/opengl32.h"
#include "core/rendering/opengl/opengl33.h"
#include "core/rendering/opengl/opengl40.h"
#include "core/rendering/opengl/opengl41.h"
#include "core/rendering/opengl/opengl42.h"
#include "core/rendering/opengl/opengl43.h"
#include "core/rendering/opengl/opengl44.h"
#include "core/rendering/opengl/opengl45.h"
#endif

namespace en
{
   namespace gpu
   {
      namespace gl
      {
      //enum Version
      //     {
      //     OpenGL_Unsupported           = 0,
      //     OpenGL_1_0                      ,  // OpenGL 1.0   - 20.06.1992
      //     OpenGL_1_1                      ,  // OpenGL 1.1   - 29.03.1997
      //     OpenGL_1_2                      ,  // OpenGL 1.2   - 16.03.1998
      //     OpenGL_1_2_1                    ,  // OpenGL 1.2.1 - 14.10.1998
      //     OpenGL_1_3                      ,  // OpenGL 1.3   - 14.08.2001
      //     OpenGL_1_4                      ,  // OpenGL 1.4   - 24.07.2002
      //     OpenGL_1_5                      ,  // OpenGL 1.5   - 29.07.2003
      //     OpenGL_2_0                      ,  // OpenGL 2.0   - 07.09.2004
      //     OpenGL_2_1                      ,  // OpenGL 2.1   - 02.07.2006
      //     OpenGL_3_0                      ,  // OpenGL 3.0   - 11.08.2008
      //     OpenGL_3_1                      ,  // OpenGL 3.1   - 24.03.2009
      //     OpenGL_3_2                      ,  // OpenGL 3.2   - 03.08.2009
      //     OpenGL_3_3                      ,  // OpenGL 3.3   - 11.03.2010
      //     OpenGL_4_0                      ,  // OpenGL 4.0   - 11.03.2010
      //     OpenGL_4_1                      ,  // OpenGL 4.1   - 26.07.2010
      //     OpenGL_4_2                      ,  // OpenGL 4.2   - 08.08.2011
      //     OpenGL_4_3                      ,  // OpenGL 4.3   - 06.08.2012
      //     VersionsCount
      //     };

      //enum ShadingLanguageVersion
      //     {
      //     GLSL_Unsupported             = 0,  // Fixed Function
      //     GLSL_1_10                       ,  // GLSL 1.10 for OpenGL 2.0
      //     GLSL_1_20                       ,  // GLSL 1.20 for OpenGL 2.1
      //     GLSL_1_30                       ,  // GLSL 1.30 for OpenGL 3.0
      //     GLSL_1_40                       ,  // GLSL 1.40 for OpenGL 3.1
      //     GLSL_1_50                       ,  // GLSL 1.50 for OpenGL 3.2
      //     GLSL_3_30                       ,  // GLSL 3.30
      //     GLSL_4_00                       ,  // GLSL 4.00
      //     GLSL_4_10                       ,  // GLSL 4.10
      //     GLSL_4_20                       ,  // GLSL 4.20
      //     GLSL_4_30                       ,  // GLSL 4.30
      //     ShadingLanguageVersionsCount
      //     };
     
      //enum MobileVersion
      //     {
      //     OpenGL_ES_Unsupported        = 0,  
      //     OpenGL_ES_1_0                   ,  // OpenGL ES 1.0 - 
      //     OpenGL_ES_1_1                   ,  // OpenGL ES 1.1 - 
      //     OpenGL_ES_2_0                   ,  // OpenGL ES 2.0 -    03.2007
      //     OpenGL_ES_3_0                   ,  // OpenGL ES 3.0 - 06.08.2012
      //     MobileVersionsCount
      //     };

      //enum MobileShadingLanguageVersion
      //     {
      //     ESSL_Unsupported             = 0,  // Fixed Function
      //     ESSL_1_00                       ,  // ESSL 1.00 for OpenGL ES 2.0
      //     ESSL_3_00                       ,  // ESSL 3.00
      //     MobileShadingLanguageVersionsCount
      //     };
      }
   }
}

#endif