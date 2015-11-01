/*

 Ngine v5.0
 
 Module      : OpenGL 1.5 functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 1.5 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_1_5_CORE
#define ENG_RENDERING_OPENGL_1_5_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLBEGINQUERYPROC           glBeginQuery;
extern PFNGLBINDBUFFERPROC           glBindBuffer;
extern PFNGLBUFFERDATAPROC           glBufferData;
extern PFNGLBUFFERSUBDATAPROC        glBufferSubData;
extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
extern PFNGLDELETEQUERIESPROC        glDeleteQueries;
extern PFNGLENDQUERYPROC             glEndQuery;
extern PFNGLGENBUFFERSPROC           glGenBuffers;
extern PFNGLGENQUERIESPROC           glGenQueries;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC    glGetBufferPointerv;
extern PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData;
extern PFNGLGETQUERYIVPROC           glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC     glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC    glGetQueryObjectuiv;
extern PFNGLISBUFFERPROC             glIsBuffer;
extern PFNGLISQUERYPROC              glIsQuery;
extern PFNGLMAPBUFFERPROC            glMapBuffer;
extern PFNGLUNMAPBUFFERPROC          glUnmapBuffer;
#endif

#endif