/*

 Ngine v5.0
 
 Module      : OpenGL 2.0 functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 2.0 
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_2_0_CORE
#define ENG_RENDERING_OPENGL_2_0_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC             glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC             glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC                glIsProgram;
extern PFNGLISSHADERPROC                 glIsShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate;
extern PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
extern PFNGLUNIFORM1FPROC                glUniform1f;
extern PFNGLUNIFORM1FVPROC               glUniform1fv;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORM1IVPROC               glUniform1iv;
extern PFNGLUNIFORM2FPROC                glUniform2f;
extern PFNGLUNIFORM2FVPROC               glUniform2fv;
extern PFNGLUNIFORM2IPROC                glUniform2i;
extern PFNGLUNIFORM2IVPROC               glUniform2iv;
extern PFNGLUNIFORM3FPROC                glUniform3f;
extern PFNGLUNIFORM3FVPROC               glUniform3fv;
extern PFNGLUNIFORM3IPROC                glUniform3i;
extern PFNGLUNIFORM3IVPROC               glUniform3iv;
extern PFNGLUNIFORM4FPROC                glUniform4f;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLUNIFORM4IPROC                glUniform4i;
extern PFNGLUNIFORM4IVPROC               glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
#endif

#endif