/*

 Ngine v5.0
 
 Module      : Effect 
 Requirements: none
 Description : Helper class combining different shader
               stages into complete pipeline program.

*/

#include "core/storage.h"
#include "resources/context.h"
#include "resources/effect.h"

namespace en
{
   //enum ShadingLanguageVersion
   //     {
   //     GLSL_Unsupported             = 0,  // Fixed Function
   //     GLSL_1_10                       ,  // GLSL 1.10 for OpenGL 2.0
   //     GLSL_1_20                       ,  // GLSL 1.20 for OpenGL 2.1
   //     GLSL_1_30                       ,  // GLSL 1.30 for OpenGL 3.0
   //     GLSL_1_40                       ,  // GLSL 1.40 for OpenGL 3.1
   //     GLSL_1_50                       ,  // GLSL 1.50 for OpenGL 3.2
   //     GLSL_3_30                       ,  // GLSL 3.30
   //     GLSL_4_00                       ,  // GLSL 4.00
   //     GLSL_4_10                       ,  // GLSL 4.10
   //     GLSL_4_20                       ,  // GLSL 4.20
   //     GLSL_4_30                       ,  // GLSL 4.30
   //     ShadingLanguageVersionsCount
   //     };


   //enum ShaderStage
   //     {
   //     VertexShader                = 0,
   //     TesselationControlShader       ,  // Hull Shader
   //     TesselationEvaluationShader    ,  // Domain Shader
   //     GeometryShader                 ,
   //     FragmentShader                 ,
   //     ShaderStagesCount
   //     };
   
   //extern const char* ShadingLanguageString[ShadingLanguageVersionsCount];
   //extern const char* ShaderStageExtensionString[PipelineStagesCount];
   //extern const char* shadersPath;
   
   const char* ShadingLanguageString[ShadingLanguageVersionsCount] =
      {
      "glsl.1.10",
      "glsl.1.20",
      "glsl.1.30",
      "glsl.1.40",
      "glsl.1.50",
      "glsl.3.30",
      "glsl.4.00",
      "glsl.4.10",
      "glsl.4.20",
      "glsl.4.30",
      "glsl.4.40",
      "glsl.4.50",
      "essl.1.00",
      "essl.3.00"
      };
   
   const char* ShaderStageExtensionString[PipelineStagesCount] =
      {
      ".vs",
      ".tcs",
      ".tes",
      ".gs",
      ".fs"
      };

   const char* ShadingLanguageVersion[ShadingLanguageVersionsCount] =
      {
      "#version 110      \n\n",
      "#version 120      \n\n",
      "#version 130      \n\n",
      "#version 140      \n\n",
      "#version 150      \n\n",
      "#version 330 core \n\n",
      "#version 400 core \n\n",
      "#version 410 core \n\n",
      "#version 420 core \n\n",
      "#version 430 core \n\n",
      "#version 440 core \n\n",
      "#version 450 core \n\n",
      "#version 100      \n\n",
      "#version 300      \n\n"
      };

   string enScene(
"// UBO - Scene Constants         \n"
"uniform enScene                  \n"
"   {                             \n"
"   mat4 camViewProjection;       \n"
"   mat4 camProjection;           \n"
"   mat4 camView;                 \n"
"   } scene;                      \n");

   Effect::Effect(ShadingLanguage version, string name) :
      binary(nullptr),
      dirty(true)
   {
   for(uint8 stage=0; stage<PipelineStagesCount; ++stage)
      {
      // Determine file name and path
      string filename = name + "." + ShadingLanguageString[version] + ShaderStageExtensionString[stage];
      string path = filename;
      if (!Storage.exist(path))
         {
         path = ResourcesContext.path.shaders + filename;
         if (!Storage.exist(path))
            {
            path = "resources/engine/shaders/" + filename;
            if (!Storage.exist(path))
               continue;
            }
         }

      // If file exist attach to it engine header
      attach((PipelineStage)(stage), ShadingLanguageVersion[version]);
      attach((PipelineStage)(stage), enScene);
      Storage.read(path, shader[stage]);
      }
   }
   
   Effect::~Effect()
   {
   }

   void Effect::attach(PipelineStage stage, string code)
   {
   this->code[stage].push_back(code);
   dirty = true;
   }
 
   void Effect::clear(PipelineStage stage)
   {
   code[stage].clear();
   dirty = true;
   }

   void Effect::clear(void)
   {
   for(uint8 stage=0; stage<PipelineStagesCount; ++stage)
      code[stage].clear();
   dirty = true;
   }

   Program Effect::program(void)
   {
   if (dirty)
      {
      vector<Shader> object;
      for(uint8 stage=0; stage<PipelineStagesCount; ++stage)
         {
         // Take array of code fragments for given shader stage,
         // and add at it's end main shader code for compilation.
         if (!shader[stage].empty())
            {
            code[stage].push_back(shader[stage]);
   
            // If there is any code for given stage try to compile it
            if (!code[stage].empty())
               object.push_back(Gpu.shader.create((PipelineStage)stage, code[stage]));
   
            // Restore original array
            code[stage].pop_back();
            }
         }
   
      binary = Gpu.program.create(object);
      }
   return binary;
   }

}