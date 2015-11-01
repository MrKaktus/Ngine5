/*

 Ngine v5.0
 
 Module      : OpenGL 3.2 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 3.2 core
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl32.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLCLIENTWAITSYNCPROC                  glClientWaitSync                  = nullptr;
PFNGLDELETESYNCPROC                      glDeleteSync                      = nullptr;
PFNGLDRAWELEMENTSBASEVERTEXPROC          glDrawElementsBaseVertex          = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = nullptr;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC     glDrawRangeElementsBaseVertex     = nullptr;
PFNGLFENCESYNCPROC                       glFenceSync                       = nullptr;
PFNGLGETBUFFERPARAMETERI64VPROC          glGetBufferParameteri64v          = nullptr;
PFNGLGETINTEGER64I_VPROC                 glGetInteger64i_v                 = nullptr;
PFNGLGETINTEGER64VPROC                   glGetInteger64v                   = nullptr;
PFNGLGETMULTISAMPLEFVPROC                glGetMultisamplefv                = nullptr;
PFNGLGETSYNCIVPROC                       glGetSynciv                       = nullptr;
PFNGLISSYNCPROC                          glIsSync                          = nullptr;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC     glMultiDrawElementsBaseVertex     = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC              glFramebufferTexture              = nullptr;
PFNGLPROGRAMPARAMETERIPROC               glProgramParameteri               = nullptr;
PFNGLPROVOKINGVERTEXPROC                 glProvokingVertex                 = nullptr;
PFNGLSAMPLEMASKIPROC                     glSampleMaski                     = nullptr;
PFNGLTEXIMAGE2DMULTISAMPLEPROC           glTexImage2DMultisample           = nullptr;
PFNGLTEXIMAGE3DMULTISAMPLEPROC           glTexImage3DMultisample           = nullptr;
PFNGLWAITSYNCPROC                        glWaitSync                        = nullptr;
#endif