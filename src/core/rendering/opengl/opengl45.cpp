/*

 Ngine v5.0
 
 Module      : OpenGL 4.5 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 4.5 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl43.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLCLIPCONTROLPROC                                 glClipControl                              = nullptr;  
PFNGLMEMORYBARRIERBYREGIONPROC                       glMemoryBarrierByRegion                    = nullptr;
PFNGLCREATETRANSFORMFEEDBACKSPROC                    glCreateTransformFeedbacks                 = nullptr;
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC                 glTransformFeedbackBufferBase              = nullptr;
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC                glTransformFeedbackBufferRange             = nullptr;
PFNGLGETTRANSFORMFEEDBACKIVPROC                      glGetTransformFeedbackiv                   = nullptr;
PFNGLGETTRANSFORMFEEDBACKI_VPROC                     glGetTransformFeedbacki_v                  = nullptr;
PFNGLGETTRANSFORMFEEDBACKI64_VPROC                   glGetTransformFeedbacki64_v                = nullptr;
PFNGLCREATEBUFFERSPROC                               glCreateBuffers                            = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC                          glNamedBufferStorage                       = nullptr;
PFNGLNAMEDBUFFERDATAPROC                             glNamedBufferData                          = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC                          glNamedBufferSubData                       = nullptr;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC                      glCopyNamedBufferSubData                   = nullptr;
PFNGLCLEARNAMEDBUFFERDATAPROC                        glClearNamedBufferData                     = nullptr;
PFNGLCLEARNAMEDBUFFERSUBDATAPROC                     glClearNamedBufferSubData                  = nullptr;
PFNGLMAPNAMEDBUFFERPROC                              glMapNamedBuffer                           = nullptr;
PFNGLMAPNAMEDBUFFERRANGEPROC                         glMapNamedBufferRange                      = nullptr;
PFNGLUNMAPNAMEDBUFFERPROC                            glUnmapNamedBuffer                         = nullptr;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC                 glFlushMappedNamedBufferRange              = nullptr;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC                   glGetNamedBufferParameteriv                = nullptr;
PFNGLGETNAMEDBUFFERPARAMETERI64VPROC                 glGetNamedBufferParameteri64v              = nullptr;
PFNGLGETNAMEDBUFFERPOINTERVPROC                      glGetNamedBufferPointerv                   = nullptr;
PFNGLGETNAMEDBUFFERSUBDATAPROC                       glGetNamedBufferSubData                    = nullptr;
PFNGLCREATEFRAMEBUFFERSPROC                          glCreateFramebuffers                       = nullptr;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC                glNamedFramebufferRenderbuffer             = nullptr;
PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC                  glNamedFramebufferParameteri               = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC                     glNamedFramebufferTexture                  = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC                glNamedFramebufferTextureLayer             = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC                  glNamedFramebufferDrawBuffer               = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC                 glNamedFramebufferDrawBuffers              = nullptr;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC                  glNamedFramebufferReadBuffer               = nullptr;
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC              glInvalidateNamedFramebufferData           = nullptr;
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC           glInvalidateNamedFramebufferSubData        = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERIVPROC                     glClearNamedFramebufferiv                  = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC                    glClearNamedFramebufferuiv                 = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERFVPROC                     glClearNamedFramebufferfv                  = nullptr;
PFNGLCLEARNAMEDFRAMEBUFFERFIPROC                     glClearNamedFramebufferfi                  = nullptr;
PFNGLBLITNAMEDFRAMEBUFFERPROC                        glBlitNamedFramebuffer                     = nullptr;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC                 glCheckNamedFramebufferStatus              = nullptr;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC              glGetNamedFramebufferParameteriv           = nullptr;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC    glGetNamedFramebufferAttachmentParameteriv = nullptr;
PFNGLCREATERENDERBUFFERSPROC                         glCreateRenderbuffers                      = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC                    glNamedRenderbufferStorage                 = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC         glNamedRenderbufferStorageMultisample      = nullptr;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC             glGetNamedRenderbufferParameteriv          = nullptr;
PFNGLCREATETEXTURESPROC                              glCreateTextures                           = nullptr;
PFNGLTEXTUREBUFFERPROC                               glTextureBuffer                            = nullptr;
PFNGLTEXTUREBUFFERRANGEPROC                          glTextureBufferRange                       = nullptr;
PFNGLTEXTURESTORAGE1DPROC                            glTextureStorage1D                         = nullptr;
PFNGLTEXTURESTORAGE2DPROC                            glTextureStorage2D                         = nullptr;
PFNGLTEXTURESTORAGE3DPROC                            glTextureStorage3D                         = nullptr;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC                 glTextureStorage2DMultisample              = nullptr;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC                 glTextureStorage3DMultisample              = nullptr;
PFNGLTEXTURESUBIMAGE1DPROC                           glTextureSubImage1D                        = nullptr;
PFNGLTEXTURESUBIMAGE2DPROC                           glTextureSubImage2D                        = nullptr;
PFNGLTEXTURESUBIMAGE3DPROC                           glTextureSubImage3D                        = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC                 glCompressedTextureSubImage1D              = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC                 glCompressedTextureSubImage2D              = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC                 glCompressedTextureSubImage3D              = nullptr;
PFNGLCOPYTEXTURESUBIMAGE1DPROC                       glCopyTextureSubImage1D                    = nullptr;
PFNGLCOPYTEXTURESUBIMAGE2DPROC                       glCopyTextureSubImage2D                    = nullptr;
PFNGLCOPYTEXTURESUBIMAGE3DPROC                       glCopyTextureSubImage3D                    = nullptr;
PFNGLTEXTUREPARAMETERFPROC                           glTextureParameterf                        = nullptr;
PFNGLTEXTUREPARAMETERFVPROC                          glTextureParameterfv                       = nullptr;
PFNGLTEXTUREPARAMETERIPROC                           glTextureParameteri                        = nullptr;
PFNGLTEXTUREPARAMETERIIVPROC                         glTextureParameterIiv                      = nullptr;
PFNGLTEXTUREPARAMETERIUIVPROC                        glTextureParameterIuiv                     = nullptr;
PFNGLTEXTUREPARAMETERIVPROC                          glTextureParameteriv                       = nullptr;
PFNGLGENERATETEXTUREMIPMAPPROC                       glGenerateTextureMipmap                    = nullptr;
PFNGLBINDTEXTUREUNITPROC                             glBindTextureUnit                          = nullptr;
PFNGLGETTEXTUREIMAGEPROC                             glGetTextureImage                          = nullptr;
PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC                   glGetCompressedTextureImage                = nullptr;
PFNGLGETTEXTURELEVELPARAMETERFVPROC                  glGetTextureLevelParameterfv               = nullptr;
PFNGLGETTEXTURELEVELPARAMETERIVPROC                  glGetTextureLevelParameteriv               = nullptr;
PFNGLGETTEXTUREPARAMETERFVPROC                       glGetTextureParameterfv                    = nullptr;
PFNGLGETTEXTUREPARAMETERIIVPROC                      glGetTextureParameterIiv                   = nullptr;
PFNGLGETTEXTUREPARAMETERIUIVPROC                     glGetTextureParameterIuiv                  = nullptr;
PFNGLGETTEXTUREPARAMETERIVPROC                       glGetTextureParameteriv                    = nullptr;
PFNGLCREATEVERTEXARRAYSPROC                          glCreateVertexArrays                       = nullptr;
PFNGLDISABLEVERTEXARRAYATTRIBPROC                    glDisableVertexArrayAttrib                 = nullptr;
PFNGLENABLEVERTEXARRAYATTRIBPROC                     glEnableVertexArrayAttrib                  = nullptr;
PFNGLVERTEXARRAYELEMENTBUFFERPROC                    glVertexArrayElementBuffer                 = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERPROC                     glVertexArrayVertexBuffer                  = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERSPROC                    glVertexArrayVertexBuffers                 = nullptr;
PFNGLVERTEXARRAYATTRIBFORMATPROC                     glVertexArrayAttribFormat                  = nullptr;
PFNGLVERTEXARRAYATTRIBIFORMATPROC                    glVertexArrayAttribIFormat                 = nullptr;
PFNGLVERTEXARRAYATTRIBLFORMATPROC                    glVertexArrayAttribLFormat                 = nullptr;
PFNGLVERTEXARRAYATTRIBBINDINGPROC                    glVertexArrayAttribBinding                 = nullptr;
PFNGLVERTEXARRAYBINDINGDIVISORPROC                   glVertexArrayBindingDivisor                = nullptr;
PFNGLGETVERTEXARRAYIVPROC                            glGetVertexArrayiv                         = nullptr;
PFNGLGETVERTEXARRAYINDEXEDIVPROC                     glGetVertexArrayIndexediv                  = nullptr;
PFNGLGETVERTEXARRAYINDEXED64IVPROC                   glGetVertexArrayIndexed64iv                = nullptr;
PFNGLCREATESAMPLERSPROC                              glCreateSamplers                           = nullptr;
PFNGLCREATEPROGRAMPIPELINESPROC                      glCreateProgramPipelines                   = nullptr;
PFNGLCREATEQUERIESPROC                               glCreateQueries                            = nullptr;
PFNGLGETQUERYBUFFEROBJECTIVPROC                      glGetQueryBufferObjectiv                   = nullptr;
PFNGLGETQUERYBUFFEROBJECTUIVPROC                     glGetQueryBufferObjectuiv                  = nullptr;
PFNGLGETQUERYBUFFEROBJECTI64VPROC                    glGetQueryBufferObjecti64v                 = nullptr;
PFNGLGETQUERYBUFFEROBJECTUI64VPROC                   glGetQueryBufferObjectui64v                = nullptr;
PFNGLGETTEXTURESUBIMAGEPROC                          glGetTextureSubImage                       = nullptr;
PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC                glGetCompressedTextureSubImage             = nullptr;
PFNGLGETGRAPHICSRESETSTATUSPROC                      glGetGraphicsResetStatus                   = nullptr;
PFNGLREADNPIXELSPROC                                 glReadnPixels                              = nullptr;
PFNGLGETNUNIFORMFVPROC                               glGetnUniformfv                            = nullptr;
PFNGLGETNUNIFORMIVPROC                               glGetnUniformiv                            = nullptr;
PFNGLGETNUNIFORMUIVPROC                              glGetnUniformuiv                           = nullptr;
PFNGLTEXTUREBARRIERPROC                              glTextureBarrier                           = nullptr;
#endif