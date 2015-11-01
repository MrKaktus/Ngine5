/*

 Ngine v5.0
 
 Module      : OpenGL 3.1 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 3.1 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_3_1_CORE
#define ENG_RENDERING_OPENGL_3_1_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLCOPYBUFFERSUBDATAPROC         glCopyBufferSubData;
extern PFNGLDRAWARRAYSINSTANCEDPROC       glDrawArraysInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDPROC     glDrawElementsInstanced;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC   glGetActiveUniformBlockiv;    
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
extern PFNGLGETACTIVEUNIFORMSIVPROC       glGetActiveUniformsiv;
extern PFNGLGETACTIVEUNIFORMNAMEPROC      glGetActiveUniformName;
extern PFNGLGETUNIFORMBLOCKINDEXPROC      glGetUniformBlockIndex;
extern PFNGLGETUNIFORMINDICESPROC         glGetUniformIndices;     
extern PFNGLPRIMITIVERESTARTINDEXPROC     glPrimitiveRestartIndex;
extern PFNGLTEXBUFFERPROC                 glTexBuffer;
extern PFNGLUNIFORMBLOCKBINDINGPROC       glUniformBlockBinding; 
#endif

#endif