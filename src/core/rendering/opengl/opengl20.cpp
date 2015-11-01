/*

 Ngine v5.0
 
 Module      : OpenGL 2.0 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 2.0 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl20.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLATTACHSHADERPROC             glAttachShader             = nullptr;
PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation       = nullptr;
PFNGLCOMPILESHADERPROC            glCompileShader            = nullptr;
PFNGLCREATEPROGRAMPROC            glCreateProgram            = nullptr;
PFNGLCREATESHADERPROC             glCreateShader             = nullptr;
PFNGLDELETEPROGRAMPROC            glDeleteProgram            = nullptr;
PFNGLDELETESHADERPROC             glDeleteShader             = nullptr;
PFNGLDETACHSHADERPROC             glDetachShader             = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLDRAWBUFFERSPROC              glDrawBuffers              = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = nullptr;
PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib          = nullptr;
PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform         = nullptr;
PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders       = nullptr;
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation        = nullptr;
PFNGLGETPROGRAMIVPROC             glGetProgramiv             = nullptr;
PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog        = nullptr;
PFNGLGETSHADERIVPROC              glGetShaderiv              = nullptr;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog         = nullptr;
PFNGLGETSHADERSOURCEPROC          glGetShaderSource          = nullptr;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation       = nullptr;
PFNGLGETUNIFORMFVPROC             glGetUniformfv             = nullptr;
PFNGLGETUNIFORMIVPROC             glGetUniformiv             = nullptr;
PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv        = nullptr;
PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv        = nullptr;
PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv        = nullptr;
PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv  = nullptr;
PFNGLISPROGRAMPROC                glIsProgram                = nullptr;
PFNGLISSHADERPROC                 glIsShader                 = nullptr;
PFNGLLINKPROGRAMPROC              glLinkProgram              = nullptr;
PFNGLSHADERSOURCEPROC             glShaderSource             = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate      = nullptr;
PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate      = nullptr;
PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate        = nullptr;
PFNGLUNIFORM1FPROC                glUniform1f                = nullptr;
PFNGLUNIFORM1FVPROC               glUniform1fv               = nullptr;
PFNGLUNIFORM1IPROC                glUniform1i                = nullptr;
PFNGLUNIFORM1IVPROC               glUniform1iv               = nullptr;
PFNGLUNIFORM2FPROC                glUniform2f                = nullptr;
PFNGLUNIFORM2FVPROC               glUniform2fv               = nullptr;
PFNGLUNIFORM2IPROC                glUniform2i                = nullptr;
PFNGLUNIFORM2IVPROC               glUniform2iv               = nullptr;
PFNGLUNIFORM3FPROC                glUniform3f                = nullptr;
PFNGLUNIFORM3FVPROC               glUniform3fv               = nullptr;
PFNGLUNIFORM3IPROC                glUniform3i                = nullptr;
PFNGLUNIFORM3IVPROC               glUniform3iv               = nullptr;
PFNGLUNIFORM4FPROC                glUniform4f                = nullptr;
PFNGLUNIFORM4FVPROC               glUniform4fv               = nullptr;
PFNGLUNIFORM4IPROC                glUniform4i                = nullptr;
PFNGLUNIFORM4IVPROC               glUniform4iv               = nullptr;
PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv         = nullptr;
PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv         = nullptr;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv         = nullptr;
PFNGLUSEPROGRAMPROC               glUseProgram               = nullptr;
PFNGLVALIDATEPROGRAMPROC          glValidateProgram          = nullptr;
PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d           = nullptr;
PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv          = nullptr;
PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f           = nullptr;
PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv          = nullptr;
PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s           = nullptr;
PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv          = nullptr;
PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d           = nullptr;
PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv          = nullptr;
PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f           = nullptr;
PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv          = nullptr;
PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s           = nullptr;
PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv          = nullptr;
PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d           = nullptr;
PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv          = nullptr;
PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f           = nullptr;
PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv          = nullptr;
PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s           = nullptr;
PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv          = nullptr;
PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv         = nullptr;
PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv         = nullptr;
PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv         = nullptr;
PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub         = nullptr;
PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv        = nullptr;
PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv        = nullptr;
PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv        = nullptr;
PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv          = nullptr;
PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d           = nullptr;
PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv          = nullptr;
PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f           = nullptr;
PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv          = nullptr;
PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv          = nullptr;
PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s           = nullptr;
PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv          = nullptr;
PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv         = nullptr;
PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv         = nullptr;
PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv         = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = nullptr;
#endif