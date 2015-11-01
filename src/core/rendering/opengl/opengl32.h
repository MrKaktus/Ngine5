/*

 Ngine v4.0
 
 Module      : OpenGL 3.2 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 3.2 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_3_2_CORE
#define ENG_RENDERING_OPENGL_3_2_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLCLIENTWAITSYNCPROC                  glClientWaitSync;
extern PFNGLDELETESYNCPROC                      glDeleteSync;
extern PFNGLDRAWELEMENTSBASEVERTEXPROC          glDrawElementsBaseVertex;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC     glDrawRangeElementsBaseVertex;
extern PFNGLFENCESYNCPROC                       glFenceSync;
extern PFNGLGETBUFFERPARAMETERI64VPROC          glGetBufferParameteri64v;
extern PFNGLGETINTEGER64I_VPROC                 glGetInteger64i_v;
extern PFNGLGETINTEGER64VPROC                   glGetInteger64v;
extern PFNGLGETMULTISAMPLEFVPROC                glGetMultisamplefv;
extern PFNGLGETSYNCIVPROC                       glGetSynciv;
extern PFNGLISSYNCPROC                          glIsSync;
extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC     glMultiDrawElementsBaseVertex;
extern PFNGLFRAMEBUFFERTEXTUREPROC              glFramebufferTexture;
extern PFNGLPROGRAMPARAMETERIPROC               glProgramParameteri;
extern PFNGLPROVOKINGVERTEXPROC                 glProvokingVertex;
extern PFNGLSAMPLEMASKIPROC                     glSampleMaski;
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC           glTexImage2DMultisample;
extern PFNGLTEXIMAGE3DMULTISAMPLEPROC           glTexImage3DMultisample;
extern PFNGLWAITSYNCPROC                        glWaitSync;
#endif

#endif