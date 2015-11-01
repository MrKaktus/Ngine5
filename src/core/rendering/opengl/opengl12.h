/*

 Ngine v5.0
 
 Module      : OpenGL 1.2 functions pointers
 Requirements:
 Description : Stores pointers of OpenGL 1.2 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_1_2_CORE
#define ENG_RENDERING_OPENGL_1_2_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC        glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC     glTexSubImage3D;
#endif

#endif