/*

 Ngine v5.0
 
 Module      : OpenGL 4.3 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 4.3 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl43.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLCLEARBUFFERDATAPROC                             glClearBufferData                 = nullptr;
PFNGLCLEARBUFFERSUBDATAPROC                          glClearBufferSubData              = nullptr;
PFNGLDISPATCHCOMPUTEPROC                             glDispatchCompute                 = nullptr;
PFNGLDISPATCHCOMPUTEINDIRECTPROC                     glDispatchComputeIndirect         = nullptr;
PFNGLCOPYIMAGESUBDATAPROC                            glCopyImageSubData                = nullptr;
PFNGLFRAMEBUFFERPARAMETERIPROC                       glFramebufferParameteri           = nullptr;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC                   glGetFramebufferParameteriv       = nullptr;
PFNGLGETINTERNALFORMATI64VPROC                       glGetInternalformati64v           = nullptr;
PFNGLINVALIDATETEXSUBIMAGEPROC                       glInvalidateTexSubImage           = nullptr;
PFNGLINVALIDATETEXIMAGEPROC                          glInvalidateTexImage              = nullptr;
PFNGLINVALIDATEBUFFERSUBDATAPROC                     glInvalidateBufferSubData         = nullptr;
PFNGLINVALIDATEBUFFERDATAPROC                        glInvalidateBufferData            = nullptr;
PFNGLINVALIDATEFRAMEBUFFERPROC                       glInvalidateFramebuffer           = nullptr;
PFNGLINVALIDATESUBFRAMEBUFFERPROC                    glInvalidateSubFramebuffer        = nullptr;
PFNGLMULTIDRAWARRAYSINDIRECTPROC                     glMultiDrawArraysIndirect         = nullptr;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC                   glMultiDrawElementsIndirect       = nullptr;
PFNGLGETPROGRAMINTERFACEIVPROC                       glGetProgramInterfaceiv           = nullptr;
PFNGLGETPROGRAMRESOURCEINDEXPROC                     glGetProgramResourceIndex         = nullptr;
PFNGLGETPROGRAMRESOURCENAMEPROC                      glGetProgramResourceName          = nullptr;
PFNGLGETPROGRAMRESOURCEIVPROC                        glGetProgramResourceiv            = nullptr;
PFNGLGETPROGRAMRESOURCELOCATIONPROC                  glGetProgramResourceLocation      = nullptr;
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC             glGetProgramResourceLocationIndex = nullptr;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC                   glShaderStorageBlockBinding       = nullptr;
PFNGLTEXBUFFERRANGEPROC                              glTexBufferRange                  = nullptr;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC                     glTexStorage2DMultisample         = nullptr;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC                     glTexStorage3DMultisample         = nullptr;
PFNGLTEXTUREVIEWPROC                                 glTextureView                     = nullptr;
PFNGLBINDVERTEXBUFFERPROC                            glBindVertexBuffer                = nullptr;
PFNGLVERTEXATTRIBFORMATPROC                          glVertexAttribFormat              = nullptr;
PFNGLVERTEXATTRIBIFORMATPROC                         glVertexAttribIFormat             = nullptr;
PFNGLVERTEXATTRIBLFORMATPROC                         glVertexAttribLFormat             = nullptr;
PFNGLVERTEXATTRIBBINDINGPROC                         glVertexAttribBinding             = nullptr;
PFNGLVERTEXBINDINGDIVISORPROC                        glVertexBindingDivisor            = nullptr;
PFNGLDEBUGMESSAGECONTROLPROC                         glDebugMessageControl             = nullptr;
PFNGLDEBUGMESSAGEINSERTPROC                          glDebugMessageInsert              = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC                        glDebugMessageCallback            = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC                          glGetDebugMessageLog              = nullptr;
PFNGLPUSHDEBUGGROUPPROC                              glPushDebugGroup                  = nullptr;
PFNGLPOPDEBUGGROUPPROC                               glPopDebugGroup                   = nullptr;
PFNGLOBJECTLABELPROC                                 glObjectLabel                     = nullptr;
PFNGLGETOBJECTLABELPROC                              glGetObjectLabel                  = nullptr;
PFNGLOBJECTPTRLABELPROC                              glObjectPtrLabel                  = nullptr;
PFNGLGETOBJECTPTRLABELPROC                           glGetObjectPtrLabel               = nullptr;
#endif