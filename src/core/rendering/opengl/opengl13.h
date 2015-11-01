/*

 Ngine v5.0
 
 Module      : OpenGL 1.3 functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 1.3 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_1_3_CORE
#define ENG_RENDERING_OPENGL_1_3_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC     glClientActiveTexture;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC    glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC    glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage;
extern PFNGLLOADTRANSPOSEMATRIXFPROC    glLoadTransposeMatrixf;
extern PFNGLLOADTRANSPOSEMATRIXDPROC    glLoadTransposeMatrixd;
extern PFNGLMULTTRANSPOSEMATRIXFPROC    glMultTransposeMatrixf;
extern PFNGLMULTTRANSPOSEMATRIXDPROC    glMultTransposeMatrixd;
extern PFNGLMULTITEXCOORD1DPROC         glMultiTexCoord1d;
extern PFNGLMULTITEXCOORD1DVPROC        glMultiTexCoord1dv;
extern PFNGLMULTITEXCOORD1FPROC         glMultiTexCoord1f;
extern PFNGLMULTITEXCOORD1FVPROC        glMultiTexCoord1fv;
extern PFNGLMULTITEXCOORD1IPROC         glMultiTexCoord1i;
extern PFNGLMULTITEXCOORD1IVPROC        glMultiTexCoord1iv;
extern PFNGLMULTITEXCOORD1SPROC         glMultiTexCoord1s;
extern PFNGLMULTITEXCOORD1SVPROC        glMultiTexCoord1sv;
extern PFNGLMULTITEXCOORD2DPROC         glMultiTexCoord2d;
extern PFNGLMULTITEXCOORD2DVPROC        glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD2FPROC         glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC        glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD2IPROC         glMultiTexCoord2i;
extern PFNGLMULTITEXCOORD2IVPROC        glMultiTexCoord2iv;
extern PFNGLMULTITEXCOORD2SPROC         glMultiTexCoord2s;
extern PFNGLMULTITEXCOORD2SVPROC        glMultiTexCoord2sv;
extern PFNGLMULTITEXCOORD3DPROC         glMultiTexCoord3d;
extern PFNGLMULTITEXCOORD3DVPROC        glMultiTexCoord3dv;
extern PFNGLMULTITEXCOORD3FPROC         glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD3FVPROC        glMultiTexCoord3fv;
extern PFNGLMULTITEXCOORD3IPROC         glMultiTexCoord3i;
extern PFNGLMULTITEXCOORD3IVPROC        glMultiTexCoord3iv;
extern PFNGLMULTITEXCOORD3SPROC         glMultiTexCoord3s;
extern PFNGLMULTITEXCOORD3SVPROC        glMultiTexCoord3sv;
extern PFNGLMULTITEXCOORD4DPROC         glMultiTexCoord4d;
extern PFNGLMULTITEXCOORD4DVPROC        glMultiTexCoord4dv;
extern PFNGLMULTITEXCOORD4FPROC         glMultiTexCoord4f;
extern PFNGLMULTITEXCOORD4FVPROC        glMultiTexCoord4fv;
extern PFNGLMULTITEXCOORD4IPROC         glMultiTexCoord4i;
extern PFNGLMULTITEXCOORD4IVPROC        glMultiTexCoord4iv;
extern PFNGLMULTITEXCOORD4SPROC         glMultiTexCoord4s;
extern PFNGLMULTITEXCOORD4SVPROC        glMultiTexCoord4sv;
extern PFNGLSAMPLECOVERAGEPROC          glSampleCoverage;
#endif

#endif