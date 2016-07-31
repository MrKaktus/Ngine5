/*

 Ngine v5.0
 
 Module      : Coordinate System Axes
 Requirements: none
 Description : This class describes scene primitive
               used to display axes of given coordinate
               system.

*/

#include "scene/axes.h"
//#include "core/rendering/context.h" // TODO: Remove after finished refactor
#include "core/rendering/common/device.h"
#include "resources/context.h"
#include "resources/effect.h"

using namespace en::gpu;
using namespace en::resource;
using namespace std;

namespace en
{
   namespace scene
   {
   string strScene(
"// UBO - Scene Constants                                            \n"
"layout(std140) uniform enScene                                      \n"
"   {                                                                \n"
"   mat4  camViewProjection[2];                                      \n"
"   mat4  camProjection[2];                                          \n"
"   mat4  camView[2];                                                \n"
"   ivec4 packed0; /* instances, stereo, res, res */                 \n"
"   } scene;                                                         \n"
"                                                                    \n"
);

   // Alternative way of selecting eye, when we draw in interleaved order LRLRLR..
   string strEyeSelectionInterleaved(
"int enEye = gl_InstanceID & 1;                                      \n"
"                                                                    \n"
);

   string strVertexShaderCamSelection(
"// Select eye in stereoscopic mode, otherwise 0.                    \n"
"int enEye = clamp((gl_InstanceID - scene.packed0.x), 0, 1);         \n"
"                                                                    \n"
"mat4 enCamViewProjection(void)                                      \n"
"{                                                                   \n"
"scene.camViewProjection[uint(enEye)];                               \n"
"}                                                                   \n"
"                                                                    \n"
"mat4 enCamProjection(void)                                          \n"
"{                                                                   \n"
"scene.camProjection[uint(enEye)];                                   \n"
"}                                                                   \n"
"                                                                    \n"
"mat4 enCamView(void)                                                \n"
"{                                                                   \n"
"scene.camView[uint(enEye)];                                         \n"
"}                                                                   \n"
"                                                                    \n"
);

   // Requires:
   // - 8th ClipPlane must be enabled
   // - need to be executed in last geometry stage
   string strOutVertexStereoClipping(
"vec4 enOutVertexPosition(vec4 projectedPosition)                                  \n"
"{                                                                                 \n"
"const vec4  eyeClipPlane[2] = { (-1.0, 0.0, 0.0, 0.0), (1.0, 0.0, 0.0, 0.0) };    \n"
"const float eyeOffsetScale[2] = { -0.5, 0.5 };                                    \n"
"                                                                                  \n"
"if (scene.packed0.y != 0)                                                         \n"
"   {                                                                              \n"
"   // Squeeze and move vertex to one of two halves of the NDC space.              \n"
"   projectedPosition.x *= 0.5;                                                    \n"
"   projectedPosition.x += eyeOffsetScale[uint(enEye)];                            \n"
"                                                                                  \n"
"   //Clip it against Clip Plane in the middle of NDC to keep in given eye space.  \n"
"   gl_ClipDistance[7] = dot(projectedPosition, eyeClipPlane[uint(enEye)]);        \n"
"   }                                                                              \n"
"                                                                                  \n"
"return projectedPosition;                                                         \n"
"}                                                                                 \n"
"                                                                                  \n"
);

   // Requires:
   // - works only if stereo is disabled
   string strOutVertex(
"vec4 enOutVertexPosition(vec4 projectedPosition)                                  \n"
"{                                                                                 \n"
"return projectedPosition;                                                         \n"
"}                                                                                 \n"
"                                                                                  \n"
);




   string strVertexShaderCamSelectionInterpolant(
"out int enViewport;                                                 \n"
"                                                                    \n"
"mat4 enPassViewport(void)                                           \n"
"{                                                                   \n"
"enViewport = enEye;                                                 \n"
"}                                                                   \n"
"                                                                    \n"
);

   string strVertexShaderViewportSelection(
"// Vertex shader Viewport selection.                                \n"
"#extension GL_AMD_vertex_shader_viewport_index : require            \n"
"int gl_ViewportIndex = enEye;                                       \n"
"                                                                    \n"
);

   string strGeometryShaderViewportSelection(
"in int enViewport;                                                  \n"
"                                                                    \n"
"// Geometry shader Viewport selection.                              \n"
"int gl_ViewportIndex = enViewport;                                  \n"
"                                                                    \n"
);


   string strPassThroughGeometryShaderPart1(
"#extension GL_NV_geometry_shader_passthrough : require                \n"
"                                                                      \n"
"layout(triangles) in;                                                 \n"
"// No output primitive layout qualifiers required.                    \n"
"                                                                      \n"
"// Redeclare gl_PerVertex to pass through 'gl_Position'.              \n"
"layout(passthrough) in gl_PerVertex                                   \n"
"{                                                                     \n"
"vec4 gl_Position;                                                     \n"
"};                                                                    \n"
"                                                                      \n"
"// Declare 'Inputs' with 'passthrough' to automatically copy members. \n"
"layout(passthrough) in Inputs                                         \n"
"{                                                                     \n"
);

// TODO: Insert  Vertex Shader output declarations here

   string strPassThroughGeometryShaderPart2(
"};                                                                    \n"
"                                                                      \n"
"// No output block declaration required.                              \n"
"                                                                      \n"
"void main()                                                           \n"
"{                                                                     \n"
"// The shader simply computes and writes gl_Layer.                    \n"
"// We don't loop over vertices or call EmitVertex().                  \n"
"gl_ViewportIndex = enViewport;                                        \n" 
"}                                                                     \n"
);



   Axes::Axes() :
      buffer(en::ResourcesContext.defaults.enAxes),
      //program(nullptr),
      //enModelMatrix(nullptr),
      Drawable()
   {
#if 0 //!defined(EN_PLATFORM_OSX)
   Effect effect(eGLSL_1_40, "enAxes");


   // RENDERER INIT

   enum StereoFeatureLevel
      {
      StereoDrawcall  = 0,   // FL0 - No GPU support. Two draw calls, one for each eye.
      StereoClipping     ,   // FL1 - No GPU support. One draw call, shared RT and Viewport, but sliced with ClipPlanes ( https://docs.google.com/presentation/d/19x9XDjUvkW_9gsfsMQzt3hZbRNziVsoCEHOn4AercAc/mobilepresent?slide=id.p )
      StereoGeometry     ,   // FL2 - GPU support for multiple viewports. GS selects destination.              ( NV_viewport_array / ARB_viewport_array / OpenGL 4.1 )
      StereoGeometryFast ,   // FL3 - GPU support for multiple viewports. Pass-Through GS selects destination. ( NV_geometry_shader_passthrough )
      StereoVertex       ,   // FL4 - GPU support for multiple viewports. VS selects destination.              ( AMD_vertex_shader_viewport_index )
      StereoFast             // FL5 - GPU support for multiple viewports. VS can select multiple destinations. ( NV_viewport_array2 ) -> this one is for multiresolution rendering
      };
   
   StereoFeatureLevel stereoMode = StereoClipping; 

   Ptr<CommonDevice> gpu = ptr_dynamic_cast<CommonDevice, GpuDevice>(Graphics->primaryDevice());

   // Determine supported Stereoscopic Rendering mode (when using OpenGL backend)
   if (gpu->api.better(OpenGL_1_0))
      {
      if (gpu->support.extension(NV_viewport_array2))
         stereoMode = StereoFast;
      else
      if ( gpu->support.extension(NV_viewport_array) ||
           gpu->support.extension(ARB_viewport_array) ||
           gpu->api.better(OpenGL_4_1) )
         {
         if (gpu->support.extension(AMD_vertex_shader_viewport_index))
            stereoMode = StereoVertex;
         else
            {
            if (gpu->support.extension(NV_geometry_shader_passthrough))
               stereoMode = StereoGeometryFast;
            else
               stereoMode = StereoGeometry;
            }
         }
      }
   // MATERIAL SHADER COMPILATION

   // During shader compilation . . .
   uint32 requiredClipPlanes = 0;

   // If shader requires more than 7 clip planes, fallback to double draw call.
   if ((stereoMode == StereoClipping) &&
       (requiredClipPlanes > 7))
      stereoMode = StereoDrawcall;   




   // FINAL SHADER COMPILATION

   // Vertex Shader is mandatory
   assert(effect.stage(Vertex));

   // Attach engine specific preprocessor
   effect.attach(Vertex, strScene);
   effect.attach(Vertex, strVertexShaderCamSelection);

   // Last geometry stage writes final vertex position.
   // If Stereoscopic rendering is enabled in Scene constants, 
   // this function perform StereoClipping, otherwise it's pass through.
   if (effect.stage(Geometry))
      {
      effect.attach(Geometry, strScene);
      // effect.attach(Geometry, strVertexShaderCamSelection);
      // TODO: Pass cam selection to GS
      effect.attach(Geometry, strOutVertexStereoClipping);
      }
   else
   if (effect.stage(Evaluation))
      {
      effect.attach(Evaluation, strScene);
      // effect.attach(Evaluation, strVertexShaderCamSelection);
      // TODO: Pass cam selection to TES
      effect.attach(Evaluation, strOutVertexStereoClipping);
      }
   else
      effect.attach(Vertex, strOutVertexStereoClipping);

   // Attach code that in automatic way will add support for Stereoscopic rendering.
   switch(stereoMode)
      {
      case StereoFast:
      case StereoVertex:
         effect.attach(Vertex, strVertexShaderViewportSelection);
         break;

      case StereoGeometryFast:
         // effect.attach(Vertex, strVertexShaderCamSelectionInterpolant);
         // effect.attach(Geometry, strPassThroughGeometryShaderPart1);   
         // TODO: Compose Pass-Through Geometry Shader. To do that engine needs to know Vertex Shader output declarations.
         //       For now fall back to Stereo Clipping mode.
         // effect.attach(Geometry, strPassThroughGeometryShaderPart2); 
         // break;

      case StereoGeometry:
         // TODO: Write pass through GS on your own. Should be generated by the engine.
         //       For now fall back to Stereo Clipping mode.
         // effect.attach(Vertex, strVertexShaderCamSelectionInterpolant);
         // effect.attach(Geometry, );
         // break;

      case StereoClipping:
      case StereoDrawcall:
         // Don't need to add any additional specific code. 
         break;

      default:
         // How we get here?
         assert( 0 );
         break;
      }
 
   pipeline      = effect.pipeline();
   enModelMatrix = program.parameter("enModelMatrix");
   enScene       = program.block("enScene");
#endif
   }
   
   void Axes::draw(const Ptr<Buffer> sceneParameters, const uint32 instances)
   {
   //enModelMatrix.set(*pWorldMatrix);
   //enScene.set(sceneParameters);
   //program.draw(buffer, LineStripes, 3, instances);
   }

   }
}
