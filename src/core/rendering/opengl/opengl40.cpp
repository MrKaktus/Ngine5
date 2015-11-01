/*

 Ngine v5.0
 
 Module      : OpenGL 4.0 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 4.0 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl40.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLBINDTRANSFORMFEEDBACKPROC          glBindTransformFeedback          = nullptr;
PFNGLBLENDEQUATIONIPROC                 glBlendEquationi                 = nullptr;
PFNGLBLENDEQUATIONSEPARATEIPROC         glBlendEquationSeparatei         = nullptr;
PFNGLBLENDFUNCIPROC                     glBlendFunci                     = nullptr;
PFNGLBLENDFUNCSEPARATEIPROC             glBlendFuncSeparatei             = nullptr; 
PFNGLDELETETRANSFORMFEEDBACKSPROC       glDeleteTransformFeedbacks       = nullptr;                                             
PFNGLDRAWARRAYSINDIRECTPROC             glDrawArraysIndirect             = nullptr;
PFNGLDRAWELEMENTSINDIRECTPROC           glDrawElementsIndirect           = nullptr;
PFNGLDRAWTRANSFORMFEEDBACKPROC          glDrawTransformFeedback          = nullptr;
PFNGLGENTRANSFORMFEEDBACKSPROC          glGenTransformFeedbacks          = nullptr;
PFNGLGETACTIVESUBROUTINENAMEPROC        glGetActiveSubroutineName        = nullptr;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC   glGetActiveSubroutineUniformiv   = nullptr;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName = nullptr;
PFNGLGETPROGRAMSTAGEIVPROC              glGetProgramStageiv              = nullptr;
PFNGLGETSUBROUTINEINDEXPROC             glGetSubroutineIndex             = nullptr;
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC   glGetSubroutineUniformLocation   = nullptr;
PFNGLGETUNIFORMDVPROC                   glGetUniformdv                   = nullptr;
PFNGLGETUNIFORMSUBROUTINEUIVPROC        glGetUniformSubroutineuiv        = nullptr;
PFNGLISTRANSFORMFEEDBACKPROC            glIsTransformFeedback            = nullptr;
PFNGLMINSAMPLESHADINGPROC               glMinSampleShading               = nullptr;
PFNGLPATCHPARAMETERIPROC                glPatchParameteri                = nullptr;
PFNGLPATCHPARAMETERFVPROC               glPatchParameterfv               = nullptr;
PFNGLPAUSETRANSFORMFEEDBACKPROC         glPauseTransformFeedback         = nullptr;
PFNGLRESUMETRANSFORMFEEDBACKPROC        glResumeTransformFeedback        = nullptr;
PFNGLUNIFORM1DPROC                      glUniform1d                      = nullptr;
PFNGLUNIFORM1DVPROC                     glUniform1dv                     = nullptr;
PFNGLUNIFORM2DPROC                      glUniform2d                      = nullptr;
PFNGLUNIFORM2DVPROC                     glUniform2dv                     = nullptr;
PFNGLUNIFORM3DPROC                      glUniform3d                      = nullptr;
PFNGLUNIFORM3DVPROC                     glUniform3dv                     = nullptr;
PFNGLUNIFORM4DPROC                      glUniform4d                      = nullptr;
PFNGLUNIFORM4DVPROC                     glUniform4dv                     = nullptr;
PFNGLUNIFORMMATRIX2DVPROC               glUniformMatrix2dv               = nullptr;
PFNGLUNIFORMMATRIX3DVPROC               glUniformMatrix3dv               = nullptr;
PFNGLUNIFORMMATRIX4DVPROC               glUniformMatrix4dv               = nullptr;
PFNGLUNIFORMMATRIX2X3DVPROC             glUniformMatrix2x3dv             = nullptr;
PFNGLUNIFORMMATRIX2X4DVPROC             glUniformMatrix2x4dv             = nullptr;
PFNGLUNIFORMMATRIX3X2DVPROC             glUniformMatrix3x2dv             = nullptr;
PFNGLUNIFORMMATRIX3X4DVPROC             glUniformMatrix3x4dv             = nullptr;
PFNGLUNIFORMMATRIX4X2DVPROC             glUniformMatrix4x2dv             = nullptr;
PFNGLUNIFORMMATRIX4X3DVPROC             glUniformMatrix4x3dv             = nullptr;
PFNGLUNIFORMSUBROUTINESUIVPROC          glUniformSubroutinesuiv          = nullptr;
#endif