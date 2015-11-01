/*

 Ngine v5.0
 
 Module      : OpenGL.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               This module implements OpenGL backend.

*/

#include "core/defines.h"
#include "core/types.h"
#include "core/log/log.h"
#include "core/rendering/opengl/opengl.h"

namespace en
{
   namespace gpu
   {
      namespace gl
      {
   //const static Nversion    ESSL_3_00(1,EN_ESSL,3,0,0);                         // ESSL 3.00  
   //const static Nversion    ESSL_1_00(0,EN_ESSL,1,0,0);                         // ESSL 1.00  
   //const static Nversion    ESSL_Unsupported(255,EN_ESSL,255,255,15);           // For marking unsupported features

   //const static Nversion    OpenGL_ES_3_1(4,EN_OpenGL_ES,3,1,0);                // OpenGL ES 3.1 - 
   //const static Nversion    OpenGL_ES_3_0(3,EN_OpenGL_ES,3,0,0);                // OpenGL ES 3.0 - 
   //const static Nversion    OpenGL_ES_2_0(2,EN_OpenGL_ES,2,0,0);                // OpenGL ES 2.0 - 
   //const static Nversion    OpenGL_ES_1_1(1,EN_OpenGL_ES,1,1,0);                // OpenGL ES 1.1 - 
   //const static Nversion    OpenGL_ES_1_0(0,EN_OpenGL_ES,1,0,0);                // OpenGL ES 1.0 - 
   //const static Nversion    OpenGL_ES_Unsupported(255,EN_OpenGL_ES,255,255,15); // For marking unsupported features

   //const static Nversion    GLSL_Next(255,EN_GLSL,9,90,0);                      // Future GLSL versions
   //const static Nversion    GLSL_4_40(10,EN_GLSL,4,40,0);                       // GLSL 4.40
   //const static Nversion    GLSL_4_30(9,EN_GLSL,4,30,0);                        // GLSL 4.30
   //const static Nversion    GLSL_4_20(8,EN_GLSL,4,20,0);                        // GLSL 4.20
   //const static Nversion    GLSL_4_10(7,EN_GLSL,4,10,0);                        // GLSL 4.10
   //const static Nversion    GLSL_4_00(6,EN_GLSL,4,0,0);                         // GLSL 4.00
   //const static Nversion    GLSL_3_30(5,EN_GLSL,3,30,0);                        // GLSL 3.30
   //const static Nversion    GLSL_1_50(4,EN_GLSL,1,50,0);                        // GLSL 1.50 for OpenGL 3.2
   //const static Nversion    GLSL_1_40(3,EN_GLSL,1,40,0);                        // GLSL 1.40 for OpenGL 3.1
   //const static Nversion    GLSL_1_30(2,EN_GLSL,1,30,0);                        // GLSL 1.30 for OpenGL 3.0
   //const static Nversion    GLSL_1_20(1,EN_GLSL,1,20,0);                        // GLSL 1.20 for OpenGL 2.1
   //const static Nversion    GLSL_1_10(0,EN_GLSL,1,10,0);                        // GLSL 1.10 for OpenGL 2.0
   //const static Nversion    GLSL_Unsupported(255,EN_GLSL,255,255,15);           // For marking unsupported features

   //const static Nversion    OpenGL_Next(255,EN_OpenGL,9,9,0);                   // Future OpenGL versions
   //const static Nversion    OpenGL_4_4(17,EN_OpenGL,4,4,0);                     // OpenGL 4.4    -   
   //const static Nversion    OpenGL_4_3(16,EN_OpenGL,4,3,0);                     // OpenGL 4.3    -   
   //const static Nversion    OpenGL_4_2(15,EN_OpenGL,4,2,0);                     // OpenGL 4.2    -
   //const static Nversion    OpenGL_4_1(14,EN_OpenGL,4,1,0);                     // OpenGL 4.1    - 
   //const static Nversion    OpenGL_4_0(13,EN_OpenGL,4,0,0);                     // OpenGL 4.0    - 11.03.2010 ok
   //const static Nversion    OpenGL_3_3(12,EN_OpenGL,3,3,0);                     // OpenGL 3.3    - 11.03.2010 ok
   //const static Nversion    OpenGL_3_2(11,EN_OpenGL,3,2,0);                     // OpenGL 3.2    - 24.07.2009 ok
   //const static Nversion    OpenGL_3_1(10,EN_OpenGL,3,1,0);                     // OpenGL 3.1    - 24.03.2009 ok
   //const static Nversion    OpenGL_3_0(9,EN_OpenGL,3,0,0);                      // OpenGL 3.0    - 11.08.2008 ok
   //const static Nversion    OpenGL_2_1(8,EN_OpenGL,2,1,0);                      // OpenGL 2.1    - 30.07.2006 spec
   //const static Nversion    OpenGL_2_0(7,EN_OpenGL,2,0,0);                      // OpenGL 2.0    - 22.10.2004 spec
   //const static Nversion    OpenGL_1_5(6,EN_OpenGL,1,5,0);                      // OpenGL 1.5    - 29.07.2003 / 30.10.2003 spec
   //const static Nversion    OpenGL_1_4(5,EN_OpenGL,1,4,0);                      // OpenGL 1.4    - 24.07.2002 spec
   //const static Nversion    OpenGL_1_3(4,EN_OpenGL,1,3,0);                      // OpenGL 1.3    - 14.08.2001 spec
   //const static Nversion    OpenGL_1_2_1(3,EN_OpenGL,1,2,1);                    // OpenGL 1.2.1  - 01.04.1999 spec
   //const static Nversion    OpenGL_1_2(2,EN_OpenGL,1,2,0);                      // OpenGL 1.2    -
   //const static Nversion    OpenGL_1_1(1,EN_OpenGL,1,1,0);                      // OpenGL 1.1    - 29.03.1997
   //const static Nversion    OpenGL_1_0(0,EN_OpenGL,1,0,0);                      // OpenGL 1.0    - 20.06.1992
   //const static Nversion    OpenGL_Unsupported(255,EN_OpenGL,255,255,15);       // For marking unsupported features
      }
   }   
}
