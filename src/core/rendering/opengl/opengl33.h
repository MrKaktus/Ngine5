/*

 Ngine v5.0
 
 Module      : OpenGL 3.3 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 3.3 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_3_3_CORE
#define ENG_RENDERING_OPENGL_3_3_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
extern PFNGLBINDSAMPLERPROC                 glBindSampler;
extern PFNGLDELETESAMPLERSPROC              glDeleteSamplers;
extern PFNGLGENSAMPLERSPROC                 glGenSamplers;
extern PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex;
extern PFNGLGETQUERYOBJECTI64VPROC          glGetQueryObjecti64v;
extern PFNGLGETQUERYOBJECTUI64VPROC         glGetQueryObjectui64v;
extern PFNGLGETSAMPLERPARAMETERIVPROC       glGetSamplerParameteriv;
extern PFNGLGETSAMPLERPARAMETERFVPROC       glGetSamplerParameterfv;
extern PFNGLGETSAMPLERPARAMETERIIVPROC      glGetSamplerParameterIiv;
extern PFNGLGETSAMPLERPARAMETERIUIVPROC     glGetSamplerParameterIuiv;
extern PFNGLISSAMPLERPROC                   glIsSampler;
extern PFNGLQUERYCOUNTERPROC                glQueryCounter;
extern PFNGLSAMPLERPARAMETERIPROC           glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERFPROC           glSamplerParameterf;
extern PFNGLSAMPLERPARAMETERIVPROC          glSamplerParameteriv;
extern PFNGLSAMPLERPARAMETERFVPROC          glSamplerParameterfv;
extern PFNGLSAMPLERPARAMETERIIVPROC         glSamplerParameterIiv;
extern PFNGLSAMPLERPARAMETERIUIVPROC        glSamplerParameterIuiv;
extern PFNGLVERTEXATTRIBP1UIPROC            glVertexAttribP1ui;
extern PFNGLVERTEXATTRIBP1UIVPROC           glVertexAttribP1uiv;
extern PFNGLVERTEXATTRIBP2UIPROC            glVertexAttribP2ui;
extern PFNGLVERTEXATTRIBP2UIVPROC           glVertexAttribP2uiv;
extern PFNGLVERTEXATTRIBP3UIPROC            glVertexAttribP3ui;
extern PFNGLVERTEXATTRIBP3UIVPROC           glVertexAttribP3uiv;
extern PFNGLVERTEXATTRIBP4UIPROC            glVertexAttribP4ui;
extern PFNGLVERTEXATTRIBP4UIVPROC           glVertexAttribP4uiv;
#endif

#endif