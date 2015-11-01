/*

 Ngine v5.0
 
 Module      : OpenGL 4.0 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 4.0 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_4_0_CORE
#define ENG_RENDERING_OPENGL_4_0_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLBINDTRANSFORMFEEDBACKPROC          glBindTransformFeedback;
extern PFNGLBLENDEQUATIONIPROC                 glBlendEquationi;
extern PFNGLBLENDEQUATIONSEPARATEIPROC         glBlendEquationSeparatei;
extern PFNGLBLENDFUNCIPROC                     glBlendFunci;
extern PFNGLBLENDFUNCSEPARATEIPROC             glBlendFuncSeparatei;
extern PFNGLDELETETRANSFORMFEEDBACKSPROC       glDeleteTransformFeedbacks;
extern PFNGLDRAWARRAYSINDIRECTPROC             glDrawArraysIndirect;
extern PFNGLDRAWELEMENTSINDIRECTPROC           glDrawElementsIndirect;
extern PFNGLDRAWTRANSFORMFEEDBACKPROC          glDrawTransformFeedback;
extern PFNGLGENTRANSFORMFEEDBACKSPROC          glGenTransformFeedbacks;
extern PFNGLGETACTIVESUBROUTINENAMEPROC        glGetActiveSubroutineName;
extern PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC   glGetActiveSubroutineUniformiv;
extern PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
extern PFNGLGETPROGRAMSTAGEIVPROC              glGetProgramStageiv;
extern PFNGLGETSUBROUTINEINDEXPROC             glGetSubroutineIndex;
extern PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC   glGetSubroutineUniformLocation;
extern PFNGLGETUNIFORMDVPROC                   glGetUniformdv;
extern PFNGLGETUNIFORMSUBROUTINEUIVPROC        glGetUniformSubroutineuiv;
extern PFNGLISTRANSFORMFEEDBACKPROC            glIsTransformFeedback;
extern PFNGLMINSAMPLESHADINGPROC               glMinSampleShading;
extern PFNGLPATCHPARAMETERIPROC                glPatchParameteri;
extern PFNGLPATCHPARAMETERFVPROC               glPatchParameterfv;
extern PFNGLPAUSETRANSFORMFEEDBACKPROC         glPauseTransformFeedback;
extern PFNGLRESUMETRANSFORMFEEDBACKPROC        glResumeTransformFeedback;
extern PFNGLUNIFORM1DPROC                      glUniform1d;
extern PFNGLUNIFORM1DVPROC                     glUniform1dv;
extern PFNGLUNIFORM2DPROC                      glUniform2d;
extern PFNGLUNIFORM2DVPROC                     glUniform2dv;
extern PFNGLUNIFORM3DPROC                      glUniform3d;
extern PFNGLUNIFORM3DVPROC                     glUniform3dv;
extern PFNGLUNIFORM4DPROC                      glUniform4d;
extern PFNGLUNIFORM4DVPROC                     glUniform4dv;
extern PFNGLUNIFORMMATRIX2DVPROC               glUniformMatrix2dv;
extern PFNGLUNIFORMMATRIX3DVPROC               glUniformMatrix3dv;
extern PFNGLUNIFORMMATRIX4DVPROC               glUniformMatrix4dv;
extern PFNGLUNIFORMMATRIX2X3DVPROC             glUniformMatrix2x3dv;
extern PFNGLUNIFORMMATRIX2X4DVPROC             glUniformMatrix2x4dv;
extern PFNGLUNIFORMMATRIX3X2DVPROC             glUniformMatrix3x2dv;
extern PFNGLUNIFORMMATRIX3X4DVPROC             glUniformMatrix3x4dv;
extern PFNGLUNIFORMMATRIX4X2DVPROC             glUniformMatrix4x2dv;
extern PFNGLUNIFORMMATRIX4X3DVPROC             glUniformMatrix4x3dv;
extern PFNGLUNIFORMSUBROUTINESUIVPROC          glUniformSubroutinesuiv;
#endif
 
#endif