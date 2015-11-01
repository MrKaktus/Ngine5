/*

 Ngine v5.0
 
 Module      : OpenGL 1.5 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 1.5 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl15.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBEGINQUERYPROC           glBeginQuery           = nullptr;
PFNGLBINDBUFFERPROC           glBindBuffer           = nullptr;
PFNGLBUFFERDATAPROC           glBufferData           = nullptr;
PFNGLBUFFERSUBDATAPROC        glBufferSubData        = nullptr;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers        = nullptr;
PFNGLDELETEQUERIESPROC        glDeleteQueries        = nullptr;
PFNGLENDQUERYPROC             glEndQuery             = nullptr;
PFNGLGENBUFFERSPROC           glGenBuffers           = nullptr;
PFNGLGENQUERIESPROC           glGenQueries           = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = nullptr;
PFNGLGETBUFFERPOINTERVPROC    glGetBufferPointerv    = nullptr;
PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData     = nullptr;
PFNGLGETQUERYIVPROC           glGetQueryiv           = nullptr;
PFNGLGETQUERYOBJECTIVPROC     glGetQueryObjectiv     = nullptr;
PFNGLGETQUERYOBJECTUIVPROC    glGetQueryObjectuiv    = nullptr;
PFNGLISBUFFERPROC             glIsBuffer             = nullptr;
PFNGLISQUERYPROC              glIsQuery              = nullptr;
PFNGLMAPBUFFERPROC            glMapBuffer            = nullptr;
PFNGLUNMAPBUFFERPROC          glUnmapBuffer          = nullptr;
#endif