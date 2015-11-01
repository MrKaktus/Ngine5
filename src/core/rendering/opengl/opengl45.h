/*

 Ngine v5.0
 
 Module      : OpenGL 4.5 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 4.5 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_4_5_CORE
#define ENG_RENDERING_OPENGL_4_5_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLCLIPCONTROLPROC                                 glClipControl;
extern PFNGLMEMORYBARRIERBYREGIONPROC                       glMemoryBarrierByRegion;
extern PFNGLCREATETRANSFORMFEEDBACKSPROC                    glCreateTransformFeedbacks;
extern PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC                 glTransformFeedbackBufferBase;
extern PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC                glTransformFeedbackBufferRange;
extern PFNGLGETTRANSFORMFEEDBACKIVPROC                      glGetTransformFeedbackiv;
extern PFNGLGETTRANSFORMFEEDBACKI_VPROC                     glGetTransformFeedbacki_v;
extern PFNGLGETTRANSFORMFEEDBACKI64_VPROC                   glGetTransformFeedbacki64_v;
extern PFNGLCREATEBUFFERSPROC                               glCreateBuffers;
extern PFNGLNAMEDBUFFERSTORAGEPROC                          glNamedBufferStorage;
extern PFNGLNAMEDBUFFERDATAPROC                             glNamedBufferData;
extern PFNGLNAMEDBUFFERSUBDATAPROC                          glNamedBufferSubData;
extern PFNGLCOPYNAMEDBUFFERSUBDATAPROC                      glCopyNamedBufferSubData;
extern PFNGLCLEARNAMEDBUFFERDATAPROC                        glClearNamedBufferData;
extern PFNGLCLEARNAMEDBUFFERSUBDATAPROC                     glClearNamedBufferSubData;
extern PFNGLMAPNAMEDBUFFERPROC                              glMapNamedBuffer;
extern PFNGLMAPNAMEDBUFFERRANGEPROC                         glMapNamedBufferRange;
extern PFNGLUNMAPNAMEDBUFFERPROC                            glUnmapNamedBuffer;
extern PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC                 glFlushMappedNamedBufferRange;
extern PFNGLGETNAMEDBUFFERPARAMETERIVPROC                   glGetNamedBufferParameteriv;
extern PFNGLGETNAMEDBUFFERPARAMETERI64VPROC                 glGetNamedBufferParameteri64v;
extern PFNGLGETNAMEDBUFFERPOINTERVPROC                      glGetNamedBufferPointerv;
extern PFNGLGETNAMEDBUFFERSUBDATAPROC                       glGetNamedBufferSubData;
extern PFNGLCREATEFRAMEBUFFERSPROC                          glCreateFramebuffers;
extern PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC                glNamedFramebufferRenderbuffer;
extern PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC                  glNamedFramebufferParameteri;
extern PFNGLNAMEDFRAMEBUFFERTEXTUREPROC                     glNamedFramebufferTexture;
extern PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC                glNamedFramebufferTextureLayer;
extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC                  glNamedFramebufferDrawBuffer;
extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC                 glNamedFramebufferDrawBuffers;
extern PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC                  glNamedFramebufferReadBuffer;
extern PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC              glInvalidateNamedFramebufferData;
extern PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC           glInvalidateNamedFramebufferSubData;
extern PFNGLCLEARNAMEDFRAMEBUFFERIVPROC                     glClearNamedFramebufferiv;
extern PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC                    glClearNamedFramebufferuiv;
extern PFNGLCLEARNAMEDFRAMEBUFFERFVPROC                     glClearNamedFramebufferfv;
extern PFNGLCLEARNAMEDFRAMEBUFFERFIPROC                     glClearNamedFramebufferfi;
extern PFNGLBLITNAMEDFRAMEBUFFERPROC                        glBlitNamedFramebuffer;
extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC                 glCheckNamedFramebufferStatus;
extern PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC              glGetNamedFramebufferParameteriv;
extern PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC    glGetNamedFramebufferAttachmentParameteriv;
extern PFNGLCREATERENDERBUFFERSPROC                         glCreateRenderbuffers;
extern PFNGLNAMEDRENDERBUFFERSTORAGEPROC                    glNamedRenderbufferStorage;
extern PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC         glNamedRenderbufferStorageMultisample;
extern PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC             glGetNamedRenderbufferParameteriv;
extern PFNGLCREATETEXTURESPROC                              glCreateTextures;
extern PFNGLTEXTUREBUFFERPROC                               glTextureBuffer;
extern PFNGLTEXTUREBUFFERRANGEPROC                          glTextureBufferRange;
extern PFNGLTEXTURESTORAGE1DPROC                            glTextureStorage1D;
extern PFNGLTEXTURESTORAGE2DPROC                            glTextureStorage2D;
extern PFNGLTEXTURESTORAGE3DPROC                            glTextureStorage3D;
extern PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC                 glTextureStorage2DMultisample;
extern PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC                 glTextureStorage3DMultisample;
extern PFNGLTEXTURESUBIMAGE1DPROC                           glTextureSubImage1D;
extern PFNGLTEXTURESUBIMAGE2DPROC                           glTextureSubImage2D;
extern PFNGLTEXTURESUBIMAGE3DPROC                           glTextureSubImage3D;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC                 glCompressedTextureSubImage1D;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC                 glCompressedTextureSubImage2D;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC                 glCompressedTextureSubImage3D;
extern PFNGLCOPYTEXTURESUBIMAGE1DPROC                       glCopyTextureSubImage1D;
extern PFNGLCOPYTEXTURESUBIMAGE2DPROC                       glCopyTextureSubImage2D;
extern PFNGLCOPYTEXTURESUBIMAGE3DPROC                       glCopyTextureSubImage3D;
extern PFNGLTEXTUREPARAMETERFPROC                           glTextureParameterf;
extern PFNGLTEXTUREPARAMETERFVPROC                          glTextureParameterfv;
extern PFNGLTEXTUREPARAMETERIPROC                           glTextureParameteri;
extern PFNGLTEXTUREPARAMETERIIVPROC                         glTextureParameterIiv;
extern PFNGLTEXTUREPARAMETERIUIVPROC                        glTextureParameterIuiv;
extern PFNGLTEXTUREPARAMETERIVPROC                          glTextureParameteriv;
extern PFNGLGENERATETEXTUREMIPMAPPROC                       glGenerateTextureMipmap;
extern PFNGLBINDTEXTUREUNITPROC                             glBindTextureUnit;
extern PFNGLGETTEXTUREIMAGEPROC                             glGetTextureImage;
extern PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC                   glGetCompressedTextureImage;
extern PFNGLGETTEXTURELEVELPARAMETERFVPROC                  glGetTextureLevelParameterfv;
extern PFNGLGETTEXTURELEVELPARAMETERIVPROC                  glGetTextureLevelParameteriv;
extern PFNGLGETTEXTUREPARAMETERFVPROC                       glGetTextureParameterfv;
extern PFNGLGETTEXTUREPARAMETERIIVPROC                      glGetTextureParameterIiv;
extern PFNGLGETTEXTUREPARAMETERIUIVPROC                     glGetTextureParameterIuiv;
extern PFNGLGETTEXTUREPARAMETERIVPROC                       glGetTextureParameteriv;
extern PFNGLCREATEVERTEXARRAYSPROC                          glCreateVertexArrays;
extern PFNGLDISABLEVERTEXARRAYATTRIBPROC                    glDisableVertexArrayAttrib;
extern PFNGLENABLEVERTEXARRAYATTRIBPROC                     glEnableVertexArrayAttrib;
extern PFNGLVERTEXARRAYELEMENTBUFFERPROC                    glVertexArrayElementBuffer;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC                     glVertexArrayVertexBuffer;
extern PFNGLVERTEXARRAYVERTEXBUFFERSPROC                    glVertexArrayVertexBuffers;
extern PFNGLVERTEXARRAYATTRIBFORMATPROC                     glVertexArrayAttribFormat;
extern PFNGLVERTEXARRAYATTRIBIFORMATPROC                    glVertexArrayAttribIFormat;
extern PFNGLVERTEXARRAYATTRIBLFORMATPROC                    glVertexArrayAttribLFormat;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC                    glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYBINDINGDIVISORPROC                   glVertexArrayBindingDivisor;
extern PFNGLGETVERTEXARRAYIVPROC                            glGetVertexArrayiv;
extern PFNGLGETVERTEXARRAYINDEXEDIVPROC                     glGetVertexArrayIndexediv;
extern PFNGLGETVERTEXARRAYINDEXED64IVPROC                   glGetVertexArrayIndexed64iv;
extern PFNGLCREATESAMPLERSPROC                              glCreateSamplers;
extern PFNGLCREATEPROGRAMPIPELINESPROC                      glCreateProgramPipelines;
extern PFNGLCREATEQUERIESPROC                               glCreateQueries;
extern PFNGLGETQUERYBUFFEROBJECTIVPROC                      glGetQueryBufferObjectiv;
extern PFNGLGETQUERYBUFFEROBJECTUIVPROC                     glGetQueryBufferObjectuiv;
extern PFNGLGETQUERYBUFFEROBJECTI64VPROC                    glGetQueryBufferObjecti64v;
extern PFNGLGETQUERYBUFFEROBJECTUI64VPROC                   glGetQueryBufferObjectui64v;
extern PFNGLGETTEXTURESUBIMAGEPROC                          glGetTextureSubImage;
extern PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC                glGetCompressedTextureSubImage;
extern PFNGLGETGRAPHICSRESETSTATUSPROC                      glGetGraphicsResetStatus;
extern PFNGLREADNPIXELSPROC                                 glReadnPixels;
extern PFNGLGETNUNIFORMFVPROC                               glGetnUniformfv;
extern PFNGLGETNUNIFORMIVPROC                               glGetnUniformiv;
extern PFNGLGETNUNIFORMUIVPROC                              glGetnUniformuiv;
extern PFNGLTEXTUREBARRIERPROC                              glTextureBarrier;
#endif

#endif