/*

 Ngine v4.0
 
 Module      : Rendering context.
 Visibility  : Engine only.
 Requirements: none
 Description : Declares structure with rendering 
               context internal data used by engine.
               This data can be only accessed by
               internal functions and methods of
               rendering context making them global
               but invisible from programmer side. 

*/

#include "core/configuration.h"
#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

#include "core/types.h"
#include "core/utilities/Tsingleton.h"
#include "core/utilities/Tproxy.h"
#include "core/utilities/Tarray.h"
#include "core/utilities/TarrayAdvanced.h"
#include "utilities/Nversion.h"
#include "threading.h"
#include "threading/mutex.h"


#ifdef EN_PLATFORM_ANDROID
#include <GLES2/gl2.h>                // OpenGL ES 2.0
#include <GLES2/gl2ext.h>             // OpenGL ES Extensions 
#endif

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                    // OpenGL
#include "opengl/include/glext.h"     // OpenGL Extensions 
#include "opengl/include/wglext.h"    // OpenGL Windows Extensions 
#endif



#include <string>
#include <set>
#include <map>
using namespace std;

#include "core/rendering/opengl/extensions.h"
#include "core/rendering/opengl/opengl12.h"
#include "core/rendering/opengl/opengl13.h"
#include "core/rendering/opengl/opengl14.h"
#include "core/rendering/opengl/opengl15.h"
#include "core/rendering/opengl/opengl20.h"
#include "core/rendering/opengl/opengl21.h"
#include "core/rendering/opengl/opengl30.h"
#include "core/rendering/opengl/opengl31.h"
#include "core/rendering/opengl/opengl32.h"
#include "core/rendering/opengl/opengl33.h"
#include "core/rendering/opengl/opengl40.h"
#include "core/rendering/opengl/opengl41.h"
#include "core/rendering/opengl/opengl42.h"
#include "core/rendering/rendering.h"

//typedef bool(*ptr_NRenderingContextScreenCreate)(bool fullscreen, uint16  w, uint16  h, string& title);    // Function type
//
//#define dynamic(retval, name) \
//        retval(*name)(bool fullscreen, uint16  w, uint16  h, string& title); \   // Function type
//        ptr_NRenderingContextScreenCreate name;





// GPU - SHADER
/************************************************************/
en::gpu::Shader oglShaderCreate(const en::gpu::PipelineStage stage, const string& code);

bool   oglShaderDestroy(en::gpu::ShaderDescriptor* const shader);

// GPU - PROGRAM
/************************************************************/
en::gpu::Program oglProgramCreate(const vector<en::gpu::Shader>& shaders);

uint32             oglProgramParametersCount(en::gpu::ProgramDescriptor* program);  
en::gpu::Parameter oglProgramParameter(en::gpu::ProgramDescriptor* program, const uint32 id);
en::gpu::Parameter oglProgramParameter(en::gpu::ProgramDescriptor* program, const string& name);
uint32             oglProgramSamplersCount(en::gpu::ProgramDescriptor* program);  
en::gpu::Sampler   oglProgramSampler(en::gpu::ProgramDescriptor* program, const uint32 id);
en::gpu::Sampler   oglProgramSampler(en::gpu::ProgramDescriptor* program, const string& name);
bool               oglProgramDraw(en::gpu::ProgramDescriptor* program, 
                                  const en::gpu::BufferDescriptor* buffer, 
                                  const en::gpu::BufferDescriptor* indexBuffer, 
                                  const en::gpu::DrawableType type, 
                                  const uint32 inst);
bool               oglProgramDestroy(en::gpu::ProgramDescriptor* const program); 

// GPU - PARAMETERS
/************************************************************/
const char* oglProgramParameterName(en::gpu::ParameterDescriptor* parameter);
uint32 oglProgramParameterSize(en::gpu::ParameterDescriptor* parameter); 
uint32 oglProgramParameterSize(uint16 type);
bool   oglProgramParameterIsSampler(uint16 type);
bool   oglProgramParameterUpdate(en::gpu::ParameterDescriptor* parameter);
bool   oglProgramParameterUpdate(en::gpu::ParameterDescriptor* parameter, const void* value);



#endif