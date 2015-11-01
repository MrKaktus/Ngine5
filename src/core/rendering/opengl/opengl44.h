/*

 Ngine v5.0
 
 Module      : OpenGL 4.4 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 4.4 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_4_4_CORE
#define ENG_RENDERING_OPENGL_4_4_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLBUFFERSTORAGEPROC               glBufferStorage;
extern PFNGLCLEARTEXIMAGEPROC               glClearTexImage;
extern PFNGLCLEARTEXSUBIMAGEPROC            glClearTexSubImage;
extern PFNGLBINDBUFFERSBASEPROC             glBindBuffersBase;
extern PFNGLBINDBUFFERSRANGEPROC            glBindBuffersRange;
extern PFNGLBINDTEXTURESPROC                glBindTextures;
extern PFNGLBINDSAMPLERSPROC                glBindSamplers;
extern PFNGLBINDIMAGETEXTURESPROC           glBindImageTextures;
extern PFNGLBINDVERTEXBUFFERSPROC           glBindVertexBuffers;
#endif

#endif