/*

 Ngine v5.0
 
 Module      : OpenGL 1.4 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 1.4 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl14.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBLENDCOLORPROC            glBlendColor            = nullptr;
PFNGLBLENDFUNCSEPARATEPROC     glBlendFuncSeparate     = nullptr;
PFNGLFOGCOORDFPROC             glFogCoordf             = nullptr;
PFNGLFOGCOORDFVPROC            glFogCoordfv            = nullptr;
PFNGLFOGCOORDDPROC             glFogCoordd             = nullptr;
PFNGLFOGCOORDDVPROC            glFogCoorddv            = nullptr;
PFNGLFOGCOORDPOINTERPROC       glFogCoordPointer       = nullptr;
PFNGLMULTIDRAWARRAYSPROC       glMultiDrawArrays       = nullptr;
PFNGLMULTIDRAWELEMENTSPROC     glMultiDrawElements     = nullptr;
PFNGLPOINTPARAMETERFPROC       glPointParameterf       = nullptr;
PFNGLPOINTPARAMETERFVPROC      glPointParameterfv      = nullptr;
PFNGLPOINTPARAMETERIPROC       glPointParameteri       = nullptr;
PFNGLPOINTPARAMETERIVPROC      glPointParameteriv      = nullptr;
PFNGLSECONDARYCOLOR3BPROC      glSecondaryColor3b      = nullptr;
PFNGLSECONDARYCOLOR3BVPROC     glSecondaryColor3bv     = nullptr;
PFNGLSECONDARYCOLOR3DPROC      glSecondaryColor3d      = nullptr;
PFNGLSECONDARYCOLOR3DVPROC     glSecondaryColor3dv     = nullptr;
PFNGLSECONDARYCOLOR3FPROC      glSecondaryColor3f      = nullptr;
PFNGLSECONDARYCOLOR3FVPROC     glSecondaryColor3fv     = nullptr;
PFNGLSECONDARYCOLOR3IPROC      glSecondaryColor3i      = nullptr;
PFNGLSECONDARYCOLOR3IVPROC     glSecondaryColor3iv     = nullptr;
PFNGLSECONDARYCOLOR3SPROC      glSecondaryColor3s      = nullptr;
PFNGLSECONDARYCOLOR3SVPROC     glSecondaryColor3sv     = nullptr;
PFNGLSECONDARYCOLOR3UBPROC     glSecondaryColor3ub     = nullptr;
PFNGLSECONDARYCOLOR3UBVPROC    glSecondaryColor3ubv    = nullptr;
PFNGLSECONDARYCOLOR3UIPROC     glSecondaryColor3ui     = nullptr;
PFNGLSECONDARYCOLOR3UIVPROC    glSecondaryColor3uiv    = nullptr;
PFNGLSECONDARYCOLOR3USPROC     glSecondaryColor3us     = nullptr;
PFNGLSECONDARYCOLOR3USVPROC    glSecondaryColor3usv    = nullptr;
PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer = nullptr;
PFNGLWINDOWPOS2DPROC           glWindowPos2d           = nullptr;
PFNGLWINDOWPOS2DVPROC          glWindowPos2dv          = nullptr;
PFNGLWINDOWPOS2FPROC           glWindowPos2f           = nullptr;
PFNGLWINDOWPOS2FVPROC          glWindowPos2fv          = nullptr;
PFNGLWINDOWPOS2IPROC           glWindowPos2i           = nullptr;
PFNGLWINDOWPOS2IVPROC          glWindowPos2iv          = nullptr;
PFNGLWINDOWPOS2SPROC           glWindowPos2s           = nullptr;
PFNGLWINDOWPOS2SVPROC          glWindowPos2sv          = nullptr;
PFNGLWINDOWPOS3DPROC           glWindowPos3d           = nullptr;
PFNGLWINDOWPOS3DVPROC          glWindowPos3dv          = nullptr;
PFNGLWINDOWPOS3FPROC           glWindowPos3f           = nullptr;
PFNGLWINDOWPOS3FVPROC          glWindowPos3fv          = nullptr;
PFNGLWINDOWPOS3IPROC           glWindowPos3i           = nullptr;
PFNGLWINDOWPOS3IVPROC          glWindowPos3iv          = nullptr;
PFNGLWINDOWPOS3SPROC           glWindowPos3s           = nullptr;
PFNGLWINDOWPOS3SVPROC          glWindowPos3sv          = nullptr;
#endif