/*

 Ngine v5.0
 
 Module      : OpenGL 1.2 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 1.2 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl12.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D = nullptr;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = nullptr;
PFNGLTEXIMAGE3DPROC        glTexImage3D        = nullptr;
PFNGLTEXSUBIMAGE3DPROC     glTexSubImage3D     = nullptr;
#endif