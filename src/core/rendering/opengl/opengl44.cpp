/*

 Ngine v5.0
 
 Module      : OpenGL 4.4 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 4.4 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl43.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBUFFERSTORAGEPROC               glBufferStorage     = nullptr;
PFNGLCLEARTEXIMAGEPROC               glClearTexImage     = nullptr;
PFNGLCLEARTEXSUBIMAGEPROC            glClearTexSubImage  = nullptr;
PFNGLBINDBUFFERSBASEPROC             glBindBuffersBase   = nullptr;
PFNGLBINDBUFFERSRANGEPROC            glBindBuffersRange  = nullptr;
PFNGLBINDTEXTURESPROC                glBindTextures      = nullptr;
PFNGLBINDSAMPLERSPROC                glBindSamplers      = nullptr;
PFNGLBINDIMAGETEXTURESPROC           glBindImageTextures = nullptr;
PFNGLBINDVERTEXBUFFERSPROC           glBindVertexBuffers = nullptr;
#endif