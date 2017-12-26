/*

 Ngine v5.0
 
 Module      : Stereoscopy rendering
 Requirements: none
 Description : Supports client distortion rendering in 
               stereoscopy mode to Oculus HMD.

*/

#include "platform/system.h"
#include "rendering/stereo.h"
#include "core/log/log.h"
#include "resources/effect.h"

using namespace en::system;

namespace en
{
//   Stereoscopy::Stereoscopy(shared_ptr<HMD> hmd) : 
//      device(dynamic_pointer_cast<OculusX>(hmd)),
//      program(nullptr),
//      sampler(0),
//      eyeToSourceUVScale(nullptr),
//      eyeToSourceUVOffset(nullptr),
//      eyeRotationStart(nullptr),
//      eyeRotationEnd(nullptr),
//      latencyProgram(nullptr),
//      latencyProjection(nullptr),
//      latencyColor(nullptr),
//      latencyBuffer(nullptr),
//      ready(false),
//      enable(false),
//      position(0.0f, 0.0f, 0.0f),
//      rotation(0.0f, 0.0f, 0.0f)
//   {
//   if (!device)
//      return;
//   
//   device->on(uint32v2(0, 0));
//   
//   // Get recommended render target size for this Oculus
//   size = device->renderTarget();
//
//   // Source rendertarget consist image for both eyes side by side.
//   for(uint8 eye=0; eye<2; ++eye)
//      {
//      settings[eye].width           = size.width;
//      settings[eye].height          = size.height;
//      settings[eye].viewport.y      = 0;
//      settings[eye].viewport.width  = size.width / 2;
//      settings[eye].viewport.height = size.height;
//      }
//   settings[0].viewport.x = 0;
//   settings[1].viewport.x = size.width / 2;
//   
//   model = device->distortionModel(settings);
//
//   // Check if shouldn't apply WA for Windows 8.1 
//   string effectName("oculus3");
//   if ( ( System.name() == Windows8   ||
//          System.name() == Windows8_1 ) &&
//        ( device->display() == Gpu.screen.display() ) )
//      effectName = string("oculus3win8wa");
//
//   Effect effect(eGLSL_1_10, effectName);
//   program             = effect.program();
//   assert( program.samplerLocation("inTexture", sampler) );
//   eyeToSourceUVScale  = program.parameter("EyeToSourceUVScale");
//   eyeToSourceUVOffset = program.parameter("EyeToSourceUVOffset");
//   eyeRotationStart    = program.parameter("EyeRotationStart");
//   eyeRotationEnd      = program.parameter("EyeRotationEnd");
//   
//   // Quad for latency tester
//   float scale = 0.04f;
//   float aspect = static_cast<float>(size.width) / static_cast<float>(size.height);
//   float latencyQuad[16] = { 1.0f-scale, 1.0f-(scale*aspect), -1.0f, 1.0f,
//                             1.0f+scale, 1.0f-(scale*aspect), -1.0f, 1.0f,
//                             1.0f-scale, 1.0f+(scale*aspect), -1.0f, 1.0f,
//                             1.0f+scale, 1.0f+(scale*aspect), -1.0f, 1.0f  };
//   
//   Effect latencyEffect(eGLSL_1_10, "resources/engine/shaders/latency");
//   latencyProgram      = latencyEffect.program();
//   latencyProjection   = latencyProgram.parameter("enProjection");
//   latencyColor        = latencyProgram.parameter("color");
//   latencyBuffer       = Gpu.buffer.create(BufferSettings(VertexBuffer, 16, ColumnInfo(Float4, "inPosition")), &latencyQuad);
//   
//   latencyProjection.set( scene::FrustumSettings(0.15f, 2.0f, float4(1.0f, 1.0f, 1.0f, 1.0f)).projection() );
//   
//
//   // Texture source is not set yet!
//   }
//   
//   Stereoscopy::~Stereoscopy()
//   {
//   if (device)
//      device->off();
//   }
//
//   void Stereoscopy::on(void)
//   {
//   enable = true;
//   }
//
//   void Stereoscopy::off(void)
//   {
//   enable = false;
//   }
//
//   bool Stereoscopy::enabled(void)
//   {
//   return enable;
//   }
//   
//   void Stereoscopy::startFrame(const uint32 frameIndex)
//   {
//   if (!device)
//      return;
//
//   timerR.start();
//   if (enable)
//      device->startFrame(frameIndex);
//   }
//   
//   uint32v4 Stereoscopy::viewport(uint8 eye)
//   {
//   return settings[eye].viewport;
//   }
//   
//   void Stereoscopy::source(shared_ptr<Texture> src)
//   {
//   assert( device ); 
//
//   size.width  = src->width();
//   size.height = src->height();
//   
//   // Source rendertarget consist image for both eyes side by side.
//   for(uint8 eye=0; eye<2; ++eye)
//      {
//      settings[eye].width           = size.width;
//      settings[eye].height          = size.height;
//      settings[eye].viewport.y      = 0;
//      settings[eye].viewport.width  = size.width / 2;
//      settings[eye].viewport.height = size.height;
//      }
//   settings[0].viewport.x = 0;
//   settings[1].viewport.x = (size.width + 1) / 2;
//   
//   device->distortionUVScaleOffset(settings);
//   program.texture(sampler, src);
//   //sampler.wraping(Clamp, Clamp);
//   
//   texture = src; // Debug
//   ready = true;
//   }
//   
//   bool Stereoscopy::display(void)
//   {   
//   if (enable)
//      {
//      // Be sure that source texture is attached
//      if (!ready)
//         return false;
//
//      // Wait to be as close to VSync as possible (Time-Warp)
//      device->waitForSync();
//
//      // Reproject rendered scene taking into notice lenses distortion
//      Gpu.output.buffer.setDefault();
//      Gpu.screen.view(0, 0, Gpu.screen.width(), Gpu.screen.height());
//      Gpu.scissor.rect(0, 0, Gpu.screen.width(), Gpu.screen.height());
//   
//      Gpu.color.buffer.clearValue(0.0f, 0.0f, 0.0f, 1.0f);
//      Gpu.depth.buffer.clearValue(1.0f);
//      Gpu.color.buffer.clear();
//      Gpu.depth.buffer.clear();
//   
//      Gpu.depth.test.off();
//      Gpu.culling.off();
//      Gpu.scissor.off();
//      Gpu.output.blend.off();
//   
//      // Render latency-tester square
//      // THIS WHOLE STEREOSCOPY INTERFACE IS NOW DEPRECATED
//      //float4 color = device->latencyTesterSquareColor();
//      if (color.w > 0.0f)
//         {
//         // THIS WHOLE STEREOSCOPY INTERFACE IS NOW DEPRECATED
//         //latencyColor.set( color );
//         latencyProgram.draw(latencyBuffer, TriangleStripes);
//         }
//   
//      // Model already consists meshes for both eyes
//      for(uint8 i=0; i<model->mesh.size(); ++i)
//         {
//         float2 scale  = float2(settings[i].UVScaleOffset[0].x, settings[i].UVScaleOffset[0].y);
//         float2 offset = float2(settings[i].UVScaleOffset[1].x, settings[i].UVScaleOffset[1].y);
//   
//         // OpenGL invert framebuffer Y axis
//         scale.y  = -scale.y;
//         offset.y = 1.0f - offset.y;
//
//         // OpenGL in Windows 8.1 WA - Flip in both Y and X axes to emulate rotation by 270 degrees
//         //scale.x  = -scale.x;
//   
//         eyeToSourceUVScale.set(scale);
//         eyeToSourceUVOffset.set(offset);
//   
//         // THIS WHOLE STEREOSCOPY INTERFACE IS NOW DEPRECATED
//         //eyeRotationStart.set( device->eyeRotationStartMatrix((Eye)i) );
//         //eyeRotationEnd.set( device->eyeRotationEndMatrix((Eye)i) );
//         program.texture(sampler, texture);
//   
//         program.draw(model->mesh[i].geometry.buffer,
//                      model->mesh[i].elements.buffer,
//                      model->mesh[i].elements.type);  
//         }
//      }
//   
//   Gpu.display();                 // Present
//
//   if (enable)
//      {                        
//      //if (device->mode() == Desktop)
//         Gpu.waitForIdle();       // Flush/Sync
//
//      //timerS.start();  
//      device->endFrame();         // Time-Warp: Mark frame end time       
//      //syncing = timerS.elapsed(); 
//
//      // Debug
//      //char tempString[100];
//      //sprintf_s(tempString,"Frame time = %0.2f ms\tsync time = %0.4f ms\n", timerR.elapsed().seconds() * 1000.0, syncing.seconds() * 1000.0 ); 
//      //Log << tempString;
//      }
// 
//   return true;
//   }
}
