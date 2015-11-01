/*

 Ngine v5.0
 
 Module      : OpenGL 3.1 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 3.1 core
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl31.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLCOPYBUFFERSUBDATAPROC         glCopyBufferSubData         = nullptr;
PFNGLDRAWARRAYSINSTANCEDPROC       glDrawArraysInstanced       = nullptr;
PFNGLDRAWELEMENTSINSTANCEDPROC     glDrawElementsInstanced     = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC   glGetActiveUniformBlockiv   = nullptr;   
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = nullptr;
PFNGLGETACTIVEUNIFORMSIVPROC       glGetActiveUniformsiv       = nullptr;
PFNGLGETACTIVEUNIFORMNAMEPROC      glGetActiveUniformName      = nullptr;
PFNGLGETUNIFORMBLOCKINDEXPROC      glGetUniformBlockIndex      = nullptr;
PFNGLGETUNIFORMINDICESPROC         glGetUniformIndices         = nullptr;     
PFNGLPRIMITIVERESTARTINDEXPROC     glPrimitiveRestartIndex     = nullptr;
PFNGLTEXBUFFERPROC                 glTexBuffer                 = nullptr;
PFNGLUNIFORMBLOCKBINDINGPROC       glUniformBlockBinding       = nullptr; 
#endif