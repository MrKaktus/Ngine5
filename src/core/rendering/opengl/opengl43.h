/*

 Ngine v5.0
 
 Module      : OpenGL 4.3 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 4.3 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_4_3_CORE
#define ENG_RENDERING_OPENGL_4_3_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLCLEARBUFFERDATAPROC                             glClearBufferData;
extern PFNGLCLEARBUFFERSUBDATAPROC                          glClearBufferSubData;
extern PFNGLDISPATCHCOMPUTEPROC                             glDispatchCompute;
extern PFNGLDISPATCHCOMPUTEINDIRECTPROC                     glDispatchComputeIndirect;
extern PFNGLCOPYIMAGESUBDATAPROC                            glCopyImageSubData;
extern PFNGLFRAMEBUFFERPARAMETERIPROC                       glFramebufferParameteri;
extern PFNGLGETFRAMEBUFFERPARAMETERIVPROC                   glGetFramebufferParameteriv;
extern PFNGLGETINTERNALFORMATI64VPROC                       glGetInternalformati64v;
extern PFNGLINVALIDATETEXSUBIMAGEPROC                       glInvalidateTexSubImage;
extern PFNGLINVALIDATETEXIMAGEPROC                          glInvalidateTexImage;
extern PFNGLINVALIDATEBUFFERSUBDATAPROC                     glInvalidateBufferSubData;
extern PFNGLINVALIDATEBUFFERDATAPROC                        glInvalidateBufferData;
extern PFNGLINVALIDATEFRAMEBUFFERPROC                       glInvalidateFramebuffer;
extern PFNGLINVALIDATESUBFRAMEBUFFERPROC                    glInvalidateSubFramebuffer;
extern PFNGLMULTIDRAWARRAYSINDIRECTPROC                     glMultiDrawArraysIndirect;
extern PFNGLMULTIDRAWELEMENTSINDIRECTPROC                   glMultiDrawElementsIndirect;
extern PFNGLGETPROGRAMINTERFACEIVPROC                       glGetProgramInterfaceiv;
extern PFNGLGETPROGRAMRESOURCEINDEXPROC                     glGetProgramResourceIndex;
extern PFNGLGETPROGRAMRESOURCENAMEPROC                      glGetProgramResourceName;
extern PFNGLGETPROGRAMRESOURCEIVPROC                        glGetProgramResourceiv;
extern PFNGLGETPROGRAMRESOURCELOCATIONPROC                  glGetProgramResourceLocation;
extern PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC             glGetProgramResourceLocationIndex;
extern PFNGLSHADERSTORAGEBLOCKBINDINGPROC                   glShaderStorageBlockBinding;
extern PFNGLTEXBUFFERRANGEPROC                              glTexBufferRange;
extern PFNGLTEXSTORAGE2DMULTISAMPLEPROC                     glTexStorage2DMultisample;
extern PFNGLTEXSTORAGE3DMULTISAMPLEPROC                     glTexStorage3DMultisample;
extern PFNGLTEXTUREVIEWPROC                                 glTextureView;
extern PFNGLBINDVERTEXBUFFERPROC                            glBindVertexBuffer;
extern PFNGLVERTEXATTRIBFORMATPROC                          glVertexAttribFormat;
extern PFNGLVERTEXATTRIBIFORMATPROC                         glVertexAttribIFormat;
extern PFNGLVERTEXATTRIBLFORMATPROC                         glVertexAttribLFormat;
extern PFNGLVERTEXATTRIBBINDINGPROC                         glVertexAttribBinding;
extern PFNGLVERTEXBINDINGDIVISORPROC                        glVertexBindingDivisor;
extern PFNGLDEBUGMESSAGECONTROLPROC                         glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC                          glDebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKPROC                        glDebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGPROC                          glGetDebugMessageLog;
//extern PFNGLGETPOINTERVPROC                                 glGetPointerv;
extern PFNGLPUSHDEBUGGROUPPROC                              glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC                               glPopDebugGroup;
extern PFNGLOBJECTLABELPROC                                 glObjectLabel;
extern PFNGLGETOBJECTLABELPROC                              glGetObjectLabel;
extern PFNGLOBJECTPTRLABELPROC                              glObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELPROC                           glGetObjectPtrLabel;
#endif

#endif
