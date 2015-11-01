/*

 Ngine v5.0
 
 Module      : OpenGL 2.1 functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 2.1 
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_2_1_CORE
#define ENG_RENDERING_OPENGL_2_1_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;
#endif

#endif