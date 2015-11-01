/*

 Ngine v5.0
 
 Module      : OpenGL 4.1 core functions pointers
 Requirements: storage
 Description : Stores pointers of OpenGL 4.1 core
               functions. They need to be directly 
               bound to functions in Windows ICD
               graphic driver library.

*/

#ifndef ENG_RENDERING_OPENGL_4_1_CORE
#define ENG_RENDERING_OPENGL_4_1_CORE

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

extern PFNGLACTIVESHADERPROGRAMPROC       glActiveShaderProgram;
extern PFNGLBINDPROGRAMPIPELINEPROC       glBindProgramPipeline;
extern PFNGLCLEARDEPTHFPROC               glClearDepthf;
extern PFNGLCREATESHADERPROGRAMVPROC      glCreateShaderProgramv;
extern PFNGLDELETEPROGRAMPIPELINESPROC    glDeleteProgramPipelines;
extern PFNGLDEPTHRANGEFPROC               glDepthRangef;
extern PFNGLDEPTHRANGEARRAYVPROC          glDepthRangeArrayv;
extern PFNGLDEPTHRANGEINDEXEDPROC         glDepthRangeIndexed;
extern PFNGLGETDOUBLEI_VPROC              glGetDoublei_v;
extern PFNGLGETFLOATI_VPROC               glGetFloati_v;
extern PFNGLGETPROGRAMBINARYPROC          glGetProgramBinary;
extern PFNGLGENPROGRAMPIPELINESPROC       glGenProgramPipelines;
extern PFNGLGETPROGRAMPIPELINEIVPROC      glGetProgramPipelineiv;
extern PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;
extern PFNGLGETSHADERPRECISIONFORMATPROC  glGetShaderPrecisionFormat;
extern PFNGLGETVERTEXATTRIBLDVPROC        glGetVertexAttribLdv;
extern PFNGLISPROGRAMPIPELINEPROC         glIsProgramPipeline;
extern PFNGLPROGRAMBINARYPROC             glProgramBinary;
extern PFNGLPROGRAMPARAMETERIPROC         glProgramParameteri;
extern PFNGLPROGRAMUNIFORM1DPROC          glProgramUniform1d;
extern PFNGLPROGRAMUNIFORM1DVPROC         glProgramUniform1dv;
extern PFNGLPROGRAMUNIFORM1FPROC          glProgramUniform1f;
extern PFNGLPROGRAMUNIFORM1FVPROC         glProgramUniform1fv;
extern PFNGLPROGRAMUNIFORM1IPROC          glProgramUniform1i;
extern PFNGLPROGRAMUNIFORM1IVPROC         glProgramUniform1iv;
extern PFNGLPROGRAMUNIFORM1UIPROC         glProgramUniform1ui;
extern PFNGLPROGRAMUNIFORM1UIVPROC        glProgramUniform1uiv;
extern PFNGLPROGRAMUNIFORM2DPROC          glProgramUniform2d;
extern PFNGLPROGRAMUNIFORM2DVPROC         glProgramUniform2dv;
extern PFNGLPROGRAMUNIFORM2FPROC          glProgramUniform2f;
extern PFNGLPROGRAMUNIFORM2FVPROC         glProgramUniform2fv;
extern PFNGLPROGRAMUNIFORM2IPROC          glProgramUniform2i;
extern PFNGLPROGRAMUNIFORM2IVPROC         glProgramUniform2iv;
extern PFNGLPROGRAMUNIFORM2UIPROC         glProgramUniform2ui;
extern PFNGLPROGRAMUNIFORM2UIVPROC        glProgramUniform2uiv;
extern PFNGLPROGRAMUNIFORM3DPROC          glProgramUniform3d;
extern PFNGLPROGRAMUNIFORM3DVPROC         glProgramUniform3dv;
extern PFNGLPROGRAMUNIFORM3FPROC          glProgramUniform3f;
extern PFNGLPROGRAMUNIFORM3FVPROC         glProgramUniform3fv;
extern PFNGLPROGRAMUNIFORM3IPROC          glProgramUniform3i;
extern PFNGLPROGRAMUNIFORM3IVPROC         glProgramUniform3iv;
extern PFNGLPROGRAMUNIFORM3UIPROC         glProgramUniform3ui;
extern PFNGLPROGRAMUNIFORM3UIVPROC        glProgramUniform3uiv;
extern PFNGLPROGRAMUNIFORM4DPROC          glProgramUniform4d;
extern PFNGLPROGRAMUNIFORM4DVPROC         glProgramUniform4dv;
extern PFNGLPROGRAMUNIFORM4FPROC          glProgramUniform4f;
extern PFNGLPROGRAMUNIFORM4FVPROC         glProgramUniform4fv;
extern PFNGLPROGRAMUNIFORM4IPROC          glProgramUniform4i;
extern PFNGLPROGRAMUNIFORM4IVPROC         glProgramUniform4iv;
extern PFNGLPROGRAMUNIFORM4UIPROC         glProgramUniform4ui;
extern PFNGLPROGRAMUNIFORM4UIVPROC        glProgramUniform4uiv;
extern PFNGLPROGRAMUNIFORMMATRIX2DVPROC   glProgramUniformMatrix2dv;
extern PFNGLPROGRAMUNIFORMMATRIX2FVPROC   glProgramUniformMatrix2fv;
extern PFNGLPROGRAMUNIFORMMATRIX3DVPROC   glProgramUniformMatrix3dv;
extern PFNGLPROGRAMUNIFORMMATRIX3FVPROC   glProgramUniformMatrix3fv;
extern PFNGLPROGRAMUNIFORMMATRIX4DVPROC   glProgramUniformMatrix4dv;
extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC   glProgramUniformMatrix4fv;
extern PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
extern PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
extern PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
extern PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
extern PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
extern PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
extern PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
extern PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
extern PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
extern PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
extern PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
extern PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
extern PFNGLRELEASESHADERCOMPILERPROC     glReleaseShaderCompiler;
extern PFNGLSCISSORARRAYVPROC             glScissorArrayv;
extern PFNGLSCISSORINDEXEDPROC            glScissorIndexed;
extern PFNGLSCISSORINDEXEDVPROC           glScissorIndexedv;
extern PFNGLSHADERBINARYPROC              glShaderBinary;
extern PFNGLUSEPROGRAMSTAGESPROC          glUseProgramStages;
extern PFNGLVALIDATEPROGRAMPIPELINEPROC   glValidateProgramPipeline;
extern PFNGLVERTEXATTRIBL1DPROC           glVertexAttribL1d;
extern PFNGLVERTEXATTRIBL1DVPROC          glVertexAttribL1dv;
extern PFNGLVERTEXATTRIBL2DPROC           glVertexAttribL2d;
extern PFNGLVERTEXATTRIBL2DVPROC          glVertexAttribL2dv;
extern PFNGLVERTEXATTRIBL3DPROC           glVertexAttribL3d;
extern PFNGLVERTEXATTRIBL3DVPROC          glVertexAttribL3dv;
extern PFNGLVERTEXATTRIBL4DPROC           glVertexAttribL4d;
extern PFNGLVERTEXATTRIBL4DVPROC          glVertexAttribL4dv;
extern PFNGLVERTEXATTRIBLPOINTERPROC      glVertexAttribLPointer;
extern PFNGLVIEWPORTARRAYVPROC            glViewportArrayv;
extern PFNGLVIEWPORTINDEXEDFPROC          glViewportIndexedf;
extern PFNGLVIEWPORTINDEXEDFVPROC         glViewportIndexedfv;
#endif

#endif