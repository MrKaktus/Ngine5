/*

 Ngine v5.0
 
 Module      : OpenGL 4.2 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 4.2 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl42.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBINDIMAGETEXTUREPROC                            glBindImageTexture                            = nullptr;
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             glDrawArraysInstancedBaseInstance             = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           glDrawElementsInstancedBaseInstance           = nullptr;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance = nullptr;
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC              glDrawTransformFeedbackInstanced              = nullptr;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC        glDrawTransformFeedbackStreamInstanced        = nullptr;
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC              glGetActiveAtomicCounterBufferiv              = nullptr;
PFNGLGETINTERNALFORMATIVPROC                         glGetInternalformativ                         = nullptr;
PFNGLMEMORYBARRIERPROC                               glMemoryBarrier                               = nullptr;
PFNGLTEXSTORAGE1DPROC                                glTexStorage1D                                = nullptr;
PFNGLTEXSTORAGE2DPROC                                glTexStorage2D                                = nullptr;
PFNGLTEXSTORAGE3DPROC                                glTexStorage3D                                = nullptr;
PFNGLTEXTURESTORAGE1DEXTPROC                         glTextureStorage1DEXT                         = nullptr;
PFNGLTEXTURESTORAGE2DEXTPROC                         glTextureStorage2DEXT                         = nullptr;
PFNGLTEXTURESTORAGE3DEXTPROC                         glTextureStorage3DEXT                         = nullptr;
#endif