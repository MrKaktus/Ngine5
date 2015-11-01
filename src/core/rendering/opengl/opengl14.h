/*

 Ngine v5.0
 
 Module      : OpenGL 1.4 functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 1.4 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_1_4_CORE
#define ENG_RENDERING_OPENGL_1_4_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLBLENDCOLORPROC            glBlendColor;
extern PFNGLBLENDFUNCSEPARATEPROC     glBlendFuncSeparate;
extern PFNGLFOGCOORDFPROC             glFogCoordf;
extern PFNGLFOGCOORDFVPROC            glFogCoordfv;
extern PFNGLFOGCOORDDPROC             glFogCoordd;
extern PFNGLFOGCOORDDVPROC            glFogCoorddv;
extern PFNGLFOGCOORDPOINTERPROC       glFogCoordPointer;
extern PFNGLMULTIDRAWARRAYSPROC       glMultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC     glMultiDrawElements;
extern PFNGLPOINTPARAMETERFPROC       glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC      glPointParameterfv;
extern PFNGLPOINTPARAMETERIPROC       glPointParameteri;
extern PFNGLPOINTPARAMETERIVPROC      glPointParameteriv;
extern PFNGLSECONDARYCOLOR3BPROC      glSecondaryColor3b;
extern PFNGLSECONDARYCOLOR3BVPROC     glSecondaryColor3bv;
extern PFNGLSECONDARYCOLOR3DPROC      glSecondaryColor3d;
extern PFNGLSECONDARYCOLOR3DVPROC     glSecondaryColor3dv;
extern PFNGLSECONDARYCOLOR3FPROC      glSecondaryColor3f;
extern PFNGLSECONDARYCOLOR3FVPROC     glSecondaryColor3fv;
extern PFNGLSECONDARYCOLOR3IPROC      glSecondaryColor3i;
extern PFNGLSECONDARYCOLOR3IVPROC     glSecondaryColor3iv;
extern PFNGLSECONDARYCOLOR3SPROC      glSecondaryColor3s;
extern PFNGLSECONDARYCOLOR3SVPROC     glSecondaryColor3sv;
extern PFNGLSECONDARYCOLOR3UBPROC     glSecondaryColor3ub;
extern PFNGLSECONDARYCOLOR3UBVPROC    glSecondaryColor3ubv;
extern PFNGLSECONDARYCOLOR3UIPROC     glSecondaryColor3ui;
extern PFNGLSECONDARYCOLOR3UIVPROC    glSecondaryColor3uiv;
extern PFNGLSECONDARYCOLOR3USPROC     glSecondaryColor3us;
extern PFNGLSECONDARYCOLOR3USVPROC    glSecondaryColor3usv;
extern PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer;
extern PFNGLWINDOWPOS2DPROC           glWindowPos2d;
extern PFNGLWINDOWPOS2DVPROC          glWindowPos2dv;
extern PFNGLWINDOWPOS2FPROC           glWindowPos2f;
extern PFNGLWINDOWPOS2FVPROC          glWindowPos2fv;
extern PFNGLWINDOWPOS2IPROC           glWindowPos2i;
extern PFNGLWINDOWPOS2IVPROC          glWindowPos2iv;
extern PFNGLWINDOWPOS2SPROC           glWindowPos2s;
extern PFNGLWINDOWPOS2SVPROC          glWindowPos2sv;
extern PFNGLWINDOWPOS3DPROC           glWindowPos3d;
extern PFNGLWINDOWPOS3DVPROC          glWindowPos3dv;
extern PFNGLWINDOWPOS3FPROC           glWindowPos3f;
extern PFNGLWINDOWPOS3FVPROC          glWindowPos3fv;
extern PFNGLWINDOWPOS3IPROC           glWindowPos3i;
extern PFNGLWINDOWPOS3IVPROC          glWindowPos3iv;
extern PFNGLWINDOWPOS3SPROC           glWindowPos3s;
extern PFNGLWINDOWPOS3SVPROC          glWindowPos3sv;
#endif

#endif