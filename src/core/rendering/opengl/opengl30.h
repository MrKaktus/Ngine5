/*

 Ngine v5.0
 
 Module      : OpenGL 3.0 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 3.0 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_3_0_CORE
#define ENG_RENDERING_OPENGL_3_0_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLBEGINCONDITIONALRENDERPROC              glBeginConditionalRender;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC              glBeginTransformFeedback;
extern PFNGLBINDBUFFERBASEPROC                      glBindBufferBase;
extern PFNGLBINDBUFFERRANGEPROC                     glBindBufferRange;
extern PFNGLBINDFRAGDATALOCATIONPROC                glBindFragDataLocation;
extern PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer;
extern PFNGLBINDRENDERBUFFERPROC                    glBindRenderbuffer;
extern PFNGLBINDVERTEXARRAYPROC                     glBindVertexArray; 
extern PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus;
extern PFNGLCLAMPCOLORPROC                          glClampColor;
extern PFNGLCLEARBUFFERFIPROC                       glClearBufferfi;
extern PFNGLCLEARBUFFERFVPROC                       glClearBufferfv;
extern PFNGLCLEARBUFFERIVPROC                       glClearBufferiv;
extern PFNGLCLEARBUFFERUIVPROC                      glClearBufferuiv;
extern PFNGLCOLORMASKIPROC                          glColorMaski;
extern PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers;
extern PFNGLDELETERENDERBUFFERSPROC                 glDeleteRenderbuffers;
extern PFNGLDELETEVERTEXARRAYSPROC                  glDeleteVertexArrays;
extern PFNGLDISABLEIPROC                            glDisablei;
extern PFNGLENABLEIPROC                             glEnablei;
extern PFNGLENDCONDITIONALRENDERPROC                glEndConditionalRender;
extern PFNGLENDTRANSFORMFEEDBACKPROC                glEndTransformFeedback;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC              glFlushMappedBufferRange;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC             glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC                glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC                glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC             glFramebufferTextureLayer;
extern PFNGLGENERATEMIPMAPPROC                      glGenerateMipmap;
extern PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers;
extern PFNGLGENRENDERBUFFERSPROC                    glGenRenderbuffers;
extern PFNGLGENVERTEXARRAYSPROC                     glGenVertexArrays;  
extern PFNGLGETBOOLEANI_VPROC                       glGetBooleani_v;
extern PFNGLGETFRAGDATALOCATIONPROC                 glGetFragDataLocation;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
extern PFNGLGETINTEGERI_VPROC                       glGetIntegeri_v;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv;
extern PFNGLGETSTRINGIPROC                          glGetStringi;
extern PFNGLGETTEXPARAMETERIIVPROC                  glGetTexParameterIiv;
extern PFNGLGETTEXPARAMETERIUIVPROC                 glGetTexParameterIuiv;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC         glGetTransformFeedbackVarying;
extern PFNGLGETUNIFORMUIVPROC                       glGetUniformuiv;
extern PFNGLGETVERTEXATTRIBIIVPROC                  glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC                 glGetVertexAttribIuiv;
extern PFNGLISENABLEDIPROC                          glIsEnabledi;
extern PFNGLISFRAMEBUFFERPROC                       glIsFramebuffer;
extern PFNGLISRENDERBUFFERPROC                      glIsRenderbuffer;
extern PFNGLISVERTEXARRAYPROC                       glIsVertexArray;
extern PFNGLMAPBUFFERRANGEPROC                      glMapBufferRange;
extern PFNGLRENDERBUFFERSTORAGEPROC                 glRenderbufferStorage;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC      glRenderbufferStorageMultisample;
extern PFNGLTEXPARAMETERIIVPROC                     glTexParameterIiv;
extern PFNGLTEXPARAMETERIUIVPROC                    glTexParameterIuiv;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC           glTransformFeedbackVaryings;
extern PFNGLUNIFORM1UIPROC                          glUniform1ui;
extern PFNGLUNIFORM1UIVPROC                         glUniform1uiv;
extern PFNGLUNIFORM2UIPROC                          glUniform2ui;
extern PFNGLUNIFORM2UIVPROC                         glUniform2uiv;
extern PFNGLUNIFORM3UIPROC                          glUniform3ui;
extern PFNGLUNIFORM3UIVPROC                         glUniform3uiv;
extern PFNGLUNIFORM4UIPROC                          glUniform4ui;
extern PFNGLUNIFORM4UIVPROC                         glUniform4uiv;
extern PFNGLVERTEXATTRIBI1IPROC                     glVertexAttribI1i;
extern PFNGLVERTEXATTRIBI1IVPROC                    glVertexAttribI1iv;
extern PFNGLVERTEXATTRIBI1UIPROC                    glVertexAttribI1ui;
extern PFNGLVERTEXATTRIBI1UIVPROC                   glVertexAttribI1uiv;
extern PFNGLVERTEXATTRIBI2IPROC                     glVertexAttribI2i;
extern PFNGLVERTEXATTRIBI2IVPROC                    glVertexAttribI2iv;
extern PFNGLVERTEXATTRIBI2UIPROC                    glVertexAttribI2ui;
extern PFNGLVERTEXATTRIBI2UIVPROC                   glVertexAttribI2uiv;
extern PFNGLVERTEXATTRIBI3IPROC                     glVertexAttribI3i;
extern PFNGLVERTEXATTRIBI3IVPROC                    glVertexAttribI3iv;
extern PFNGLVERTEXATTRIBI3UIPROC                    glVertexAttribI3ui;
extern PFNGLVERTEXATTRIBI3UIVPROC                   glVertexAttribI3uiv;
extern PFNGLVERTEXATTRIBI4BVPROC                    glVertexAttribI4bv;
extern PFNGLVERTEXATTRIBI4IPROC                     glVertexAttribI4i;
extern PFNGLVERTEXATTRIBI4IVPROC                    glVertexAttribI4iv;
extern PFNGLVERTEXATTRIBI4SVPROC                    glVertexAttribI4sv;
extern PFNGLVERTEXATTRIBI4UBVPROC                   glVertexAttribI4ubv;
extern PFNGLVERTEXATTRIBI4UIPROC                    glVertexAttribI4ui;
extern PFNGLVERTEXATTRIBI4UIVPROC                   glVertexAttribI4uiv;
extern PFNGLVERTEXATTRIBI4USVPROC                   glVertexAttribI4usv;
extern PFNGLVERTEXATTRIBIPOINTERPROC                glVertexAttribIPointer;
#endif

#endif