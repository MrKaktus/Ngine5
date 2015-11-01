/*

 Ngine v5.0
 
 Module      : OpenGL 1.3 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 1.3 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl13.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLACTIVETEXTUREPROC           glActiveTexture           = nullptr;
PFNGLCLIENTACTIVETEXTUREPROC     glClientActiveTexture     = nullptr;
PFNGLCOMPRESSEDTEXIMAGE1DPROC    glCompressedTexImage1D    = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D    = nullptr;
PFNGLCOMPRESSEDTEXIMAGE3DPROC    glCompressedTexImage3D    = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage   = nullptr;
PFNGLLOADTRANSPOSEMATRIXFPROC    glLoadTransposeMatrixf    = nullptr;
PFNGLLOADTRANSPOSEMATRIXDPROC    glLoadTransposeMatrixd    = nullptr;
PFNGLMULTTRANSPOSEMATRIXFPROC    glMultTransposeMatrixf    = nullptr;
PFNGLMULTTRANSPOSEMATRIXDPROC    glMultTransposeMatrixd    = nullptr;
PFNGLMULTITEXCOORD1DPROC         glMultiTexCoord1d         = nullptr;
PFNGLMULTITEXCOORD1DVPROC        glMultiTexCoord1dv        = nullptr;
PFNGLMULTITEXCOORD1FPROC         glMultiTexCoord1f         = nullptr;
PFNGLMULTITEXCOORD1FVPROC        glMultiTexCoord1fv        = nullptr;
PFNGLMULTITEXCOORD1IPROC         glMultiTexCoord1i         = nullptr;
PFNGLMULTITEXCOORD1IVPROC        glMultiTexCoord1iv        = nullptr;
PFNGLMULTITEXCOORD1SPROC         glMultiTexCoord1s         = nullptr;
PFNGLMULTITEXCOORD1SVPROC        glMultiTexCoord1sv        = nullptr;
PFNGLMULTITEXCOORD2DPROC         glMultiTexCoord2d         = nullptr;
PFNGLMULTITEXCOORD2DVPROC        glMultiTexCoord2dv        = nullptr;
PFNGLMULTITEXCOORD2FPROC         glMultiTexCoord2f         = nullptr;
PFNGLMULTITEXCOORD2FVPROC        glMultiTexCoord2fv        = nullptr;
PFNGLMULTITEXCOORD2IPROC         glMultiTexCoord2i         = nullptr;
PFNGLMULTITEXCOORD2IVPROC        glMultiTexCoord2iv        = nullptr;
PFNGLMULTITEXCOORD2SPROC         glMultiTexCoord2s         = nullptr;
PFNGLMULTITEXCOORD2SVPROC        glMultiTexCoord2sv        = nullptr;
PFNGLMULTITEXCOORD3DPROC         glMultiTexCoord3d         = nullptr;
PFNGLMULTITEXCOORD3DVPROC        glMultiTexCoord3dv        = nullptr;
PFNGLMULTITEXCOORD3FPROC         glMultiTexCoord3f         = nullptr;
PFNGLMULTITEXCOORD3FVPROC        glMultiTexCoord3fv        = nullptr;
PFNGLMULTITEXCOORD3IPROC         glMultiTexCoord3i         = nullptr;
PFNGLMULTITEXCOORD3IVPROC        glMultiTexCoord3iv        = nullptr;
PFNGLMULTITEXCOORD3SPROC         glMultiTexCoord3s         = nullptr;
PFNGLMULTITEXCOORD3SVPROC        glMultiTexCoord3sv        = nullptr;
PFNGLMULTITEXCOORD4DPROC         glMultiTexCoord4d         = nullptr;
PFNGLMULTITEXCOORD4DVPROC        glMultiTexCoord4dv        = nullptr;
PFNGLMULTITEXCOORD4FPROC         glMultiTexCoord4f         = nullptr;
PFNGLMULTITEXCOORD4FVPROC        glMultiTexCoord4fv        = nullptr;
PFNGLMULTITEXCOORD4IPROC         glMultiTexCoord4i         = nullptr;
PFNGLMULTITEXCOORD4IVPROC        glMultiTexCoord4iv        = nullptr;
PFNGLMULTITEXCOORD4SPROC         glMultiTexCoord4s         = nullptr;
PFNGLMULTITEXCOORD4SVPROC        glMultiTexCoord4sv        = nullptr;
PFNGLSAMPLECOVERAGEPROC          glSampleCoverage          = nullptr;
#endif