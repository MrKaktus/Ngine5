/*

 Ngine v5.0
 
 Module      : OpenGL 4.1 functions pointers
 Requirements: 
 Description : Stores pointers of OpenGL 4.1 core 
               functions. They need to be directly
               bound to functions in Windows ICD
               graphic driver library.

*/

#include "core/rendering/opengl/opengl41.h"

#ifdef EN_PLATFORM_WINDOWS
PFNGLACTIVESHADERPROGRAMPROC       glActiveShaderProgram       = nullptr;
PFNGLBINDPROGRAMPIPELINEPROC       glBindProgramPipeline       = nullptr;
PFNGLCLEARDEPTHFPROC               glClearDepthf               = nullptr;
PFNGLCREATESHADERPROGRAMVPROC      glCreateShaderProgramv      = nullptr;
PFNGLDELETEPROGRAMPIPELINESPROC    glDeleteProgramPipelines    = nullptr;
PFNGLDEPTHRANGEFPROC               glDepthRangef               = nullptr;
PFNGLDEPTHRANGEARRAYVPROC          glDepthRangeArrayv          = nullptr;
PFNGLDEPTHRANGEINDEXEDPROC         glDepthRangeIndexed         = nullptr;
PFNGLGETDOUBLEI_VPROC              glGetDoublei_v              = nullptr;
PFNGLGETFLOATI_VPROC               glGetFloati_v               = nullptr;
PFNGLGETPROGRAMBINARYPROC          glGetProgramBinary          = nullptr;
PFNGLGENPROGRAMPIPELINESPROC       glGenProgramPipelines       = nullptr;
PFNGLGETPROGRAMPIPELINEIVPROC      glGetProgramPipelineiv      = nullptr;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog = nullptr;
PFNGLGETSHADERPRECISIONFORMATPROC  glGetShaderPrecisionFormat  = nullptr;
PFNGLGETVERTEXATTRIBLDVPROC        glGetVertexAttribLdv        = nullptr;
PFNGLISPROGRAMPIPELINEPROC         glIsProgramPipeline         = nullptr;
PFNGLPROGRAMBINARYPROC             glProgramBinary             = nullptr;
PFNGLPROGRAMUNIFORM1DPROC          glProgramUniform1d          = nullptr;
PFNGLPROGRAMUNIFORM1DVPROC         glProgramUniform1dv         = nullptr;
PFNGLPROGRAMUNIFORM1FPROC          glProgramUniform1f          = nullptr;
PFNGLPROGRAMUNIFORM1FVPROC         glProgramUniform1fv         = nullptr;
PFNGLPROGRAMUNIFORM1IPROC          glProgramUniform1i          = nullptr;
PFNGLPROGRAMUNIFORM1IVPROC         glProgramUniform1iv         = nullptr;
PFNGLPROGRAMUNIFORM1UIPROC         glProgramUniform1ui         = nullptr;
PFNGLPROGRAMUNIFORM1UIVPROC        glProgramUniform1uiv        = nullptr;
PFNGLPROGRAMUNIFORM2DPROC          glProgramUniform2d          = nullptr;
PFNGLPROGRAMUNIFORM2DVPROC         glProgramUniform2dv         = nullptr;
PFNGLPROGRAMUNIFORM2FPROC          glProgramUniform2f          = nullptr;
PFNGLPROGRAMUNIFORM2FVPROC         glProgramUniform2fv         = nullptr;
PFNGLPROGRAMUNIFORM2IPROC          glProgramUniform2i          = nullptr;
PFNGLPROGRAMUNIFORM2IVPROC         glProgramUniform2iv         = nullptr;
PFNGLPROGRAMUNIFORM2UIPROC         glProgramUniform2ui         = nullptr;
PFNGLPROGRAMUNIFORM2UIVPROC        glProgramUniform2uiv        = nullptr;
PFNGLPROGRAMUNIFORM3DPROC          glProgramUniform3d          = nullptr;
PFNGLPROGRAMUNIFORM3DVPROC         glProgramUniform3dv         = nullptr;
PFNGLPROGRAMUNIFORM3FPROC          glProgramUniform3f          = nullptr;
PFNGLPROGRAMUNIFORM3FVPROC         glProgramUniform3fv         = nullptr;
PFNGLPROGRAMUNIFORM3IPROC          glProgramUniform3i          = nullptr;
PFNGLPROGRAMUNIFORM3IVPROC         glProgramUniform3iv         = nullptr;
PFNGLPROGRAMUNIFORM3UIPROC         glProgramUniform3ui         = nullptr;
PFNGLPROGRAMUNIFORM3UIVPROC        glProgramUniform3uiv        = nullptr;
PFNGLPROGRAMUNIFORM4DPROC          glProgramUniform4d          = nullptr;
PFNGLPROGRAMUNIFORM4DVPROC         glProgramUniform4dv         = nullptr;
PFNGLPROGRAMUNIFORM4FPROC          glProgramUniform4f          = nullptr;
PFNGLPROGRAMUNIFORM4FVPROC         glProgramUniform4fv         = nullptr;
PFNGLPROGRAMUNIFORM4IPROC          glProgramUniform4i          = nullptr;
PFNGLPROGRAMUNIFORM4IVPROC         glProgramUniform4iv         = nullptr;
PFNGLPROGRAMUNIFORM4UIPROC         glProgramUniform4ui         = nullptr;
PFNGLPROGRAMUNIFORM4UIVPROC        glProgramUniform4uiv        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC   glProgramUniformMatrix2dv   = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC   glProgramUniformMatrix2fv   = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC   glProgramUniformMatrix3dv   = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC   glProgramUniformMatrix3fv   = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC   glProgramUniformMatrix4dv   = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC   glProgramUniformMatrix4fv   = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv = nullptr;
PFNGLRELEASESHADERCOMPILERPROC     glReleaseShaderCompiler     = nullptr;
PFNGLSCISSORARRAYVPROC             glScissorArrayv             = nullptr;
PFNGLSCISSORINDEXEDPROC            glScissorIndexed            = nullptr;
PFNGLSCISSORINDEXEDVPROC           glScissorIndexedv           = nullptr;
PFNGLSHADERBINARYPROC              glShaderBinary              = nullptr;
PFNGLUSEPROGRAMSTAGESPROC          glUseProgramStages          = nullptr;
PFNGLVALIDATEPROGRAMPIPELINEPROC   glValidateProgramPipeline   = nullptr;
PFNGLVERTEXATTRIBL1DPROC           glVertexAttribL1d           = nullptr;
PFNGLVERTEXATTRIBL1DVPROC          glVertexAttribL1dv          = nullptr;
PFNGLVERTEXATTRIBL2DPROC           glVertexAttribL2d           = nullptr;
PFNGLVERTEXATTRIBL2DVPROC          glVertexAttribL2dv          = nullptr;
PFNGLVERTEXATTRIBL3DPROC           glVertexAttribL3d           = nullptr;
PFNGLVERTEXATTRIBL3DVPROC          glVertexAttribL3dv          = nullptr;
PFNGLVERTEXATTRIBL4DPROC           glVertexAttribL4d           = nullptr;
PFNGLVERTEXATTRIBL4DVPROC          glVertexAttribL4dv          = nullptr;
PFNGLVERTEXATTRIBLPOINTERPROC      glVertexAttribLPointer      = nullptr;
PFNGLVIEWPORTARRAYVPROC            glViewportArrayv            = nullptr;
PFNGLVIEWPORTINDEXEDFPROC          glViewportIndexedf          = nullptr;
PFNGLVIEWPORTINDEXEDFVPROC         glViewportIndexedfv         = nullptr;
#endif