/*

 Ngine v5.0
 
 Module      : OpenGL 3.3 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 3.3 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl33.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed = nullptr;
PFNGLBINDSAMPLERPROC                 glBindSampler                 = nullptr;
PFNGLDELETESAMPLERSPROC              glDeleteSamplers              = nullptr;
PFNGLGENSAMPLERSPROC                 glGenSamplers                 = nullptr;
PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex            = nullptr;
PFNGLGETQUERYOBJECTI64VPROC          glGetQueryObjecti64v          = nullptr;
PFNGLGETQUERYOBJECTUI64VPROC         glGetQueryObjectui64v         = nullptr;
PFNGLGETSAMPLERPARAMETERIVPROC       glGetSamplerParameteriv       = nullptr;
PFNGLGETSAMPLERPARAMETERFVPROC       glGetSamplerParameterfv       = nullptr;
PFNGLGETSAMPLERPARAMETERIIVPROC      glGetSamplerParameterIiv      = nullptr;
PFNGLGETSAMPLERPARAMETERIUIVPROC     glGetSamplerParameterIuiv     = nullptr;
PFNGLISSAMPLERPROC                   glIsSampler                   = nullptr;
PFNGLQUERYCOUNTERPROC                glQueryCounter                = nullptr;
PFNGLSAMPLERPARAMETERIPROC           glSamplerParameteri           = nullptr;
PFNGLSAMPLERPARAMETERFPROC           glSamplerParameterf           = nullptr;
PFNGLSAMPLERPARAMETERIVPROC          glSamplerParameteriv          = nullptr;
PFNGLSAMPLERPARAMETERFVPROC          glSamplerParameterfv          = nullptr;
PFNGLSAMPLERPARAMETERIIVPROC         glSamplerParameterIiv         = nullptr;
PFNGLSAMPLERPARAMETERIUIVPROC        glSamplerParameterIuiv        = nullptr;
PFNGLVERTEXATTRIBP1UIPROC            glVertexAttribP1ui            = nullptr;
PFNGLVERTEXATTRIBP1UIVPROC           glVertexAttribP1uiv           = nullptr;
PFNGLVERTEXATTRIBP2UIPROC            glVertexAttribP2ui            = nullptr;
PFNGLVERTEXATTRIBP2UIVPROC           glVertexAttribP2uiv           = nullptr;
PFNGLVERTEXATTRIBP3UIPROC            glVertexAttribP3ui            = nullptr;
PFNGLVERTEXATTRIBP3UIVPROC           glVertexAttribP3uiv           = nullptr;
PFNGLVERTEXATTRIBP4UIPROC            glVertexAttribP4ui            = nullptr;
PFNGLVERTEXATTRIBP4UIVPROC           glVertexAttribP4uiv           = nullptr;
#endif