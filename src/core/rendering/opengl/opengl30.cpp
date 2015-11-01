/*

 Ngine v5.0
 
 Module      : OpenGL 3.0 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 3.0 core
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl30.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBEGINCONDITIONALRENDERPROC              glBeginConditionalRender              = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKPROC              glBeginTransformFeedback              = nullptr;
PFNGLBINDBUFFERBASEPROC                      glBindBufferBase                      = nullptr;
PFNGLBINDBUFFERRANGEPROC                     glBindBufferRange                     = nullptr;
PFNGLBINDFRAGDATALOCATIONPROC                glBindFragDataLocation                = nullptr;
PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer                     = nullptr;
PFNGLBINDRENDERBUFFERPROC                    glBindRenderbuffer                    = nullptr;
PFNGLBINDVERTEXARRAYPROC                     glBindVertexArray                     = nullptr; 
PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer                     = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus              = nullptr;
PFNGLCLAMPCOLORPROC                          glClampColor                          = nullptr;
PFNGLCLEARBUFFERFIPROC                       glClearBufferfi                       = nullptr;
PFNGLCLEARBUFFERFVPROC                       glClearBufferfv                       = nullptr;
PFNGLCLEARBUFFERIVPROC                       glClearBufferiv                       = nullptr;
PFNGLCLEARBUFFERUIVPROC                      glClearBufferuiv                      = nullptr;
PFNGLCOLORMASKIPROC                          glColorMaski                          = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers                  = nullptr;
PFNGLDELETERENDERBUFFERSPROC                 glDeleteRenderbuffers                 = nullptr;
PFNGLDELETEVERTEXARRAYSPROC                  glDeleteVertexArrays                  = nullptr;
PFNGLDISABLEIPROC                            glDisablei                            = nullptr;
PFNGLENABLEIPROC                             glEnablei                             = nullptr;
PFNGLENDCONDITIONALRENDERPROC                glEndConditionalRender                = nullptr;
PFNGLENDTRANSFORMFEEDBACKPROC                glEndTransformFeedback                = nullptr;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC              glFlushMappedBufferRange              = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC             glFramebufferRenderbuffer             = nullptr;
PFNGLFRAMEBUFFERTEXTURE1DPROC                glFramebufferTexture1D                = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D                = nullptr;
PFNGLFRAMEBUFFERTEXTURE3DPROC                glFramebufferTexture3D                = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC             glFramebufferTextureLayer             = nullptr;
PFNGLGENERATEMIPMAPPROC                      glGenerateMipmap                      = nullptr;
PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers                     = nullptr;
PFNGLGENRENDERBUFFERSPROC                    glGenRenderbuffers                    = nullptr;
PFNGLGENVERTEXARRAYSPROC                     glGenVertexArrays                     = nullptr;  
PFNGLGETBOOLEANI_VPROC                       glGetBooleani_v                       = nullptr;
PFNGLGETFRAGDATALOCATIONPROC                 glGetFragDataLocation                 = nullptr;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = nullptr;
PFNGLGETINTEGERI_VPROC                       glGetIntegeri_v                       = nullptr;
PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv          = nullptr;
PFNGLGETSTRINGIPROC                          glGetStringi                          = nullptr;
PFNGLGETTEXPARAMETERIIVPROC                  glGetTexParameterIiv                  = nullptr;
PFNGLGETTEXPARAMETERIUIVPROC                 glGetTexParameterIuiv                 = nullptr;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC         glGetTransformFeedbackVarying         = nullptr;
PFNGLGETUNIFORMUIVPROC                       glGetUniformuiv                       = nullptr;
PFNGLGETVERTEXATTRIBIIVPROC                  glGetVertexAttribIiv                  = nullptr;
PFNGLGETVERTEXATTRIBIUIVPROC                 glGetVertexAttribIuiv                 = nullptr;
PFNGLISENABLEDIPROC                          glIsEnabledi                          = nullptr;
PFNGLISFRAMEBUFFERPROC                       glIsFramebuffer                       = nullptr;
PFNGLISRENDERBUFFERPROC                      glIsRenderbuffer                      = nullptr;
PFNGLISVERTEXARRAYPROC                       glIsVertexArray                       = nullptr;
PFNGLMAPBUFFERRANGEPROC                      glMapBufferRange                      = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC                 glRenderbufferStorage                 = nullptr;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC      glRenderbufferStorageMultisample      = nullptr;
PFNGLTEXPARAMETERIIVPROC                     glTexParameterIiv                     = nullptr;
PFNGLTEXPARAMETERIUIVPROC                    glTexParameterIuiv                    = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC           glTransformFeedbackVaryings           = nullptr;
PFNGLUNIFORM1UIPROC                          glUniform1ui                          = nullptr;
PFNGLUNIFORM1UIVPROC                         glUniform1uiv                         = nullptr;
PFNGLUNIFORM2UIPROC                          glUniform2ui                          = nullptr;
PFNGLUNIFORM2UIVPROC                         glUniform2uiv                         = nullptr;
PFNGLUNIFORM3UIPROC                          glUniform3ui                          = nullptr;
PFNGLUNIFORM3UIVPROC                         glUniform3uiv                         = nullptr;
PFNGLUNIFORM4UIPROC                          glUniform4ui                          = nullptr;
PFNGLUNIFORM4UIVPROC                         glUniform4uiv                         = nullptr;
PFNGLVERTEXATTRIBI1IPROC                     glVertexAttribI1i                     = nullptr;
PFNGLVERTEXATTRIBI1IVPROC                    glVertexAttribI1iv                    = nullptr;
PFNGLVERTEXATTRIBI1UIPROC                    glVertexAttribI1ui                    = nullptr;
PFNGLVERTEXATTRIBI1UIVPROC                   glVertexAttribI1uiv                   = nullptr;
PFNGLVERTEXATTRIBI2IPROC                     glVertexAttribI2i                     = nullptr;
PFNGLVERTEXATTRIBI2IVPROC                    glVertexAttribI2iv                    = nullptr;
PFNGLVERTEXATTRIBI2UIPROC                    glVertexAttribI2ui                    = nullptr;
PFNGLVERTEXATTRIBI2UIVPROC                   glVertexAttribI2uiv                   = nullptr;
PFNGLVERTEXATTRIBI3IPROC                     glVertexAttribI3i                     = nullptr;
PFNGLVERTEXATTRIBI3IVPROC                    glVertexAttribI3iv                    = nullptr;
PFNGLVERTEXATTRIBI3UIPROC                    glVertexAttribI3ui                    = nullptr;
PFNGLVERTEXATTRIBI3UIVPROC                   glVertexAttribI3uiv                   = nullptr;
PFNGLVERTEXATTRIBI4BVPROC                    glVertexAttribI4bv                    = nullptr;
PFNGLVERTEXATTRIBI4IPROC                     glVertexAttribI4i                     = nullptr;
PFNGLVERTEXATTRIBI4IVPROC                    glVertexAttribI4iv                    = nullptr;
PFNGLVERTEXATTRIBI4SVPROC                    glVertexAttribI4sv                    = nullptr;
PFNGLVERTEXATTRIBI4UBVPROC                   glVertexAttribI4ubv                   = nullptr;
PFNGLVERTEXATTRIBI4UIPROC                    glVertexAttribI4ui                    = nullptr;
PFNGLVERTEXATTRIBI4UIVPROC                   glVertexAttribI4uiv                   = nullptr;
PFNGLVERTEXATTRIBI4USVPROC                   glVertexAttribI4usv                   = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC                glVertexAttribIPointer                = nullptr;
#endif