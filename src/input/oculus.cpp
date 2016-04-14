/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#include "core/log/log.h"
#include "input/context.h"
#include "scene/cam.h"

#include "utilities/strings.h"

#include "core/rendering/context.h"
#include "core/rendering/rendering.hpp"

#if /*defined(EN_PLATFORM_OSX) ||*/ defined(EN_PLATFORM_WINDOWS)

   #if OCULUS_VR
   //#pragma comment(lib, "LibOVR")

   #include "OculusSDK/LibOVR/Include/Extras/OVR_Math.h" // Oculus 
   #include "input/oculus.h"
   #if OCULUS_SDK_RENDERING
   #include "core/rendering/context.h"
   #include "core/rendering/opengl/gl43Texture.h"
   #endif
   #endif

#endif

namespace en
{
   namespace input
   {
   //# IMPLEMENTATION
   //##########################################################################

#if /*defined(EN_PLATFORM_OSX) ||*/ defined(EN_PLATFORM_WINDOWS)
#if OCULUS_VR

   OculusDK2::OculusDK2(uint8 index) :
      enabled(false),
      sharedRT(true),
      deviceType(HMDUnknown),
      displayId(-1),
      displayMode(Direct)
   {
   //assert( index < ovrHmd_Detect() );
   ovrResult res = ovr_Create(&session, &luid);
   assert( res == ovrSuccess );
   //assert( hmd );

   // Get HMD description
   info = ovr_GetHmdDesc(session);

   // Determine Oculus type
   if (info.Type == ovrHmd_DK1)
      deviceType = HMDOculusDK1;
   else
   if (info.Type == ovrHmd_DKHD)
      deviceType = HMDOculusDKHD;
   else
   if (info.Type == 5)
      deviceType = HMDOculusDKCrystalCove;
   else
   if (info.Type == ovrHmd_DK2)
      deviceType = HMDOculusDK2;
   else
   if (info.Type == 7) // ovrHmd_BlackStar
      deviceType = HMDOculusBlackStar;
   else
   if (info.Type == ovrHmd_CB)
      deviceType = HMDOculusCrescentBay;
   else
   if (info.Type == ovrHmd_Other)
      deviceType = HMDOculusUnknown;
   else
   if (info.Type == ovrHmd_E3_2015)
      deviceType = HMDOculusE3_2015;
   else
   if (info.Type == ovrHmd_ES06)
      deviceType = HMDOculusES06;
   else
   if (info.Type == ovrHmd_ES09)
      deviceType = HMDOculusES09;
   else
      deviceType = HMDUnknown;

   // Does Oculus support Extended Desktop mode in SDK 0.6.0 ????

   // Check if Oculus can work in Direct Rendering mode
   //if (!(hmd->HmdCaps & ovrHmdCap_ExtendDesktop))
   //   displayMode = Direct;

   // Get display number on desktop
   //if (displayMode == Desktop)
   //   {
   //   #ifdef EN_PLATFORM_WINDOWS 
   //   sint32 displayNumber = 0;
   //   DISPLAY_DEVICE Device;
   //   memset(&Device, 0, sizeof(Device));
   //   Device.cb = sizeof(Device);
   //   while(EnumDisplayDevices(NULL, displayNumber, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
   //        {
   //        if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
   //           {
   //           DEVMODE DispMode;
   //           memset(&DispMode, 0, sizeof(DispMode));
   //           DispMode.dmSize = sizeof(DispMode);
   //           if ( !EnumDisplaySettings(Device.DeviceName, ENUM_REGISTRY_SETTINGS, &DispMode) )
   //              break;
   //   
   //           // Compare name and resolution to find matching Display number
   //           string name = stringFromWchar(Device.DeviceName, 32);
   //           Log << "Display " << displayNumber << " : " << name << endl;
   //           if ( name.compare(0, 12, string(hmd->DisplayDeviceName)) && 
   //                hmd->Resolution.w == DispMode.dmPelsWidth &&
   //                hmd->Resolution.h == DispMode.dmPelsHeight )
   //              {
   //              Log << "Oculus found on " << displayNumber << " : " << name << endl;
   //              displayId = displayNumber;
   //              break;
   //              }
   //           }
   //        displayNumber++;
   //        }
   //   #endif
   //   }
   };

   OculusDK2::~OculusDK2()
   {
   ovr_ConfigureTracking(session, 0, 0);
   ovr_Destroy(session);
   }

   bool OculusDK2::on(uint32v2 windowResolution, bool sharedRendertarget)
   {
   if (enabled)
      return true;

   sharedRT = sharedRendertarget;

   // Calculate resolution of Render Target
   ovrSizei recommenedTex0Size = ovr_GetFovTextureSize(session, ovrEye_Left,  info.DefaultEyeFov[0], 1.0f);
   ovrSizei recommenedTex1Size = ovr_GetFovTextureSize(session, ovrEye_Right, info.DefaultEyeFov[1], 1.0f);
   resolutionRT.x = static_cast<uint32>(recommenedTex0Size.w + recommenedTex1Size.w);
   resolutionRT.y = static_cast<uint32>(max( recommenedTex0Size.h, recommenedTex1Size.h ));
   if (!sharedRT)
      {
      resolutionRT.x = static_cast<uint32>(recommenedTex0Size.w);
      resolutionRT.y = static_cast<uint32>(recommenedTex0Size.h);
      }

   // Calculate viewports location and size per eye
   for(uint8 eye=0; eye<2; ++eye)
      {
      view[eye].x      = 0;
      view[eye].y      = 0;
      view[eye].width  = resolutionRT.width;
      view[eye].height = resolutionRT.height;
      if (sharedRT)
         view[eye].width /= 2;
      }
   if (sharedRT)
      view[1].x = view[0].width + 1;

   // Configure SDK rendering
   //#if OCULUS_SDK_RENDERING
   assert( GpuContext.screen.created );

   // Create double buffered textures for each used rendertarget
   for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
      {
      // >>>>> OpenGL Specific code section - START
      ovr_CreateSwapTextureSetGL(session, GL_SRGB8_ALPHA8, resolutionRT.width, resolutionRT.height, &textureSet[i]);
      for(sint32 j=0; j<textureSet[i]->TextureCount; ++j)
         {
         assert( textureSet[i]->TextureCount == 2 );

         ovrGLTexture* tex = (ovrGLTexture*)&textureSet[i]->Textures[j];
         // This modifies OpenGL state of currently bound Texture to texture unit X !!!
         glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         // Create texture interface that can be exposed to application
         en::gpu::TextureState state;
         state.type    = TextureType::Texture2D;
         state.format  = Format::RGBA_8_sRGB;
         state.width   = resolutionRT.width;
         state.height  = resolutionRT.height;
         state.depth   = 1;
         state.layers  = 1;
         state.samples = 1;
         state.mipmaps = 1;

         swap[i][j] = ptr_dynamic_cast<en::gpu::Texture, en::gpu::TextureGL>(Ptr<en::gpu::TextureGL>(new en::gpu::TextureGL(state, tex->OGL.TexId)));
         }
      // >>>>> OpenGL Specific code section - END 
      }

   // Create mirror texture that can be read by FBO to optional window

   // >>>>> OpenGL Specific code section - START
   ovr_CreateMirrorTextureGL(session, GL_SRGB8_ALPHA8, windowResolution.width, windowResolution.height, (ovrTexture**)&mirrorTexture);
   en::gpu::TextureState state;
   state.type    = TextureType::Texture2D;
   state.format  = Format::RGBA_8_sRGB;
   state.width   = windowResolution.width;
   state.height  = windowResolution.height;
   state.depth   = 1;
   state.layers  = 1;
   state.samples = 1;
   state.mipmaps = 1;

   mirror = ptr_dynamic_cast<en::gpu::Texture, en::gpu::TextureGL>(Ptr<en::gpu::TextureGL>(new en::gpu::TextureGL(state, mirrorTexture->OGL.TexId)));
   // >>>>> OpenGL Specific code section - END
   //#endif

   // Create Framebuffer to use with mirroring
   fbo = Gpu.output.buffer.create();
   fbo->color(Read, 0, mirror);

   // All Caps are enabled by default from 0.8
   //ovr_SetEnabledCaps(session, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction );

   //// Attach to window in direct rendering mode
   //#if !OCULUS_SDK_RENDERING
   //if (displayMode == Direct)
   //   {
   //   assert( GpuContext.screen.created );
   //   #ifdef EN_PLATFORM_WINDOWS
   //   if (!ovrHmd_AttachToWindow(hmd, GpuContext.device.hWnd, NULL, NULL))
   //      Log << "ERROR: Cannot attach Oculus to window for Direct Rendering!\n";
   //   #endif
   //   }
   //#endif

   // Turn on the Oculus
   ovrResult res = ovr_ConfigureTracking(session, ovrTrackingCap_Orientation      | 
                                                  ovrTrackingCap_MagYawCorrection | 
                                                  ovrTrackingCap_Position, 
                                                  ovrTrackingCap_Orientation);
   if (res != ovrSuccess)
      return false;

   EyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left,  info.DefaultEyeFov[0]);
   EyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, info.DefaultEyeFov[1]);

   ovr_RecenterPose(session);

   enabled = true;
   return true;
   }

   bool OculusDK2::off(void)
   {
   if (!enabled)
      return true;

   // Turn off display
   if (displayMode == Direct)
      {
      // TODO: Turn off display here
      }

   // Remove Oculus texture interfaces
   swap[0][0] = nullptr;
   swap[0][1] = nullptr;
   swap[1][0] = nullptr;
   swap[1][1] = nullptr;
   mirror     = nullptr;

   // Disable Oculus 
   ovr_ConfigureTracking(session, 0, 0);
   enabled = false;
   return true;
   }

   bool OculusDK2::state(void) const
   {
   return enabled;
   }

   HMDType OculusDK2::device(void) const
   {
   return deviceType;
   }

   HMDVendor OculusDK2::vendor(void) const
   {
   return HMDOculus;
   }

   HMDMode OculusDK2::mode(void) const
   {
   return displayMode;
   }

   sint32 OculusDK2::display(void) const
   {
   return displayId;
   }

   uint32v2 OculusDK2::resolution(void) const
   {
   return uint32v2(info.Resolution.w, info.Resolution.h);
   }

   uint32v2 OculusDK2::renderTarget(void) const
   {
   return resolutionRT;
   }

   bool OculusDK2::sharedRendertarget(void) const
   {
   return sharedRT;
   }

   uint32v4 OculusDK2::viewport(uint8 eye) const
   {
   return view[eye];
   }

   float4 OculusDK2::frustumTangents(Eye eye) const
   {
   uint32 i = static_cast<uint32>(eye);
   return float4(info.DefaultEyeFov[i].UpTan,
                 info.DefaultEyeFov[i].DownTan,
                 info.DefaultEyeFov[i].LeftTan,
                 info.DefaultEyeFov[i].RightTan ); 
   }

   //Ptr<en::resource::Model> OculusDK2::distortionModel(DistortionSettings* settings)
   //{
   //#if !OCULUS_SDK_RENDERING
   //using namespace en::gpu;
   //using namespace en::resource;
   //
   //if (!Gpu.screen.created())
   //   return Ptr<Model>(nullptr);
   //
   //Ptr<en::resource::Model> model = new en::resource::Model();
   //model->name = string("Oculus Distortion Mesh");
   //model->mesh.resize(2);
   //
   //for(uint8 eye=0; eye<2; ++eye)
   //   {
   //   // Allocate & generate distortion mesh vertices
   //   ovrDistortionMesh ovrMesh;
   //   uint32 distortionCaps = ovrDistortionCap_TimeWarp |
   //                           ovrDistortionCap_Vignette;
   //   ovrHmd_CreateDistortionMesh(hmd, EyeRenderDesc[eye].Eye, EyeRenderDesc[eye].Fov, distortionCaps, &ovrMesh);
   //
   //   ovrSizei textureSize;
   //   textureSize.w = settings[eye].width;
   //   textureSize.h = settings[eye].height;
   //   ovrHmd_GetRenderScaleAndOffset(EyeRenderDesc[eye].Fov, textureSize, *((ovrRecti*)&settings[eye].viewport), (ovrVector2f*)&settings[eye].UVScaleOffset);
   //
   //   // Create Vertex Buffer
   //   BufferSettings vboSettings(VertexBuffer,
   //                              ovrMesh.VertexCount,
   //                              ColumnInfo(Float2,"inPosition"),
   //                              ColumnInfo(Float2,"inTexCoord0"),
   //                              ColumnInfo(Float2,"inTexCoord1"),
   //                              ColumnInfo(Float2,"inTexCoord2"),
   //                              ColumnInfo(Float_8_Norm, "inVignette"),
   //                              ColumnInfo(Float_8_Norm, "inTimeWarp"));
   //   
   //   model->mesh[eye].geometry.buffer = Gpu.buffer.create(vboSettings);
   //   model->mesh[eye].geometry.begin  = 0;
   //   model->mesh[eye].geometry.end    = ovrMesh.VertexCount;
   //
   //   // Fill Vertex Buffer with data
   //   aligned(1);
   //   struct DistortionVertex
   //          {
   //          float2 position;
   //          float2 uv[3];
   //          uint8  vignette;
   //          uint8  timewarp;
   //          };
   //   aligndefault
   //
   //   DistortionVertex*    dst = static_cast<DistortionVertex*>(model->mesh[eye].geometry.buffer.map());
   //   ovrDistortionVertex* src = ovrMesh.pVertexData;
   //   for(uint32 i=0; i<ovrMesh.VertexCount; ++i)
   //      {
   //      dst->position = (*(float2*)&src->ScreenPosNDC);
   //      dst->uv[0]    = (*(float2*)&src->TanEyeAnglesR);
   //      dst->uv[1]    = (*(float2*)&src->TanEyeAnglesG);
   //      dst->uv[2]    = (*(float2*)&src->TanEyeAnglesB);
   //      dst->vignette = static_cast<uint8>( src->VignetteFactor * 255.0f );
   //      dst->timewarp = static_cast<uint8>( src->TimeWarpFactor * 255.0f );
   //      dst++; src++;
   //      }
   //
   //   model->mesh[eye].geometry.buffer.unmap();
   //
   //   // Create Index Buffer
   //   model->mesh[eye].elements.buffer  = Gpu.buffer.create(BufferSettings(IndexBuffer, ovrMesh.IndexCount, ColumnType(UShort)), ovrMesh.pIndexData);
   //   model->mesh[eye].elements.type    = Triangles;
   //   model->mesh[eye].elements.offset  = 0;
   //   model->mesh[eye].elements.indexes = ovrMesh.IndexCount;
   //
   //   ovrHmd_DestroyDistortionMesh(&ovrMesh);
   //   }
   //
   //return model;
   //#endif
   //return Ptr<en::resource::Model>(nullptr);
   //}
   //
   //void OculusDK2::distortionUVScaleOffset(DistortionSettings* settings)
   //{
   //#if !OCULUS_SDK_RENDERING
   //ovrSizei textureSize;
   //for(uint32 eye=0; eye<2; ++eye)
   //   {
   //   textureSize.w = settings[eye].width;
   //   textureSize.h = settings[eye].height;
   //   ovrHmd_GetRenderScaleAndOffset(EyeRenderDesc[eye].Fov, textureSize, *((ovrRecti*)&settings[eye].viewport), (ovrVector2f*)&settings[eye].UVScaleOffset);
   //   }
   //#endif
   //}


   void OculusDK2::startFrame(uint32 frameIndex)
   {
   ovrVector3f hmdToEyeViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset,
                                         EyeRenderDesc[1].HmdToEyeViewOffset };

   // Swap used rendertarget textures in swap-chain
   for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
      textureSet[i]->CurrentIndex = (textureSet[i]->CurrentIndex + 1) % textureSet[i]->TextureCount;
      
   //#if OCULUS_SDK_RENDERING
   //ovrFrameTiming   ftiming  = ovrH_GetFrameTiming(hmd, 0);
   ovrTrackingState hmdState = ovr_GetTrackingState(session, 0, ovrTrue); //ftiming.DisplayMidpointSeconds
   ovr_CalcEyePoses(hmdState.HeadPose.ThePose, hmdToEyeViewOffset, &eyePose[0]);
   //#else
   //startTime = ovrHmd_BeginFrameTiming(hmd, frameIndex);
   //
   //ovrHmd_GetEyePoses(hmd, 0, hmdToEyeViewOffset, &eyePose[0], NULL); // &hmdState - Samples are tracking head properly without obtaining this info, how??
   //#endif
   }

   void OculusDK2::update(void)
   {
   // TOOD: In future add here support of events from Oculus Touch
   }

   float3 OculusDK2::position(void) const
   {
   // Oculus Crystal Cove, DK2 and better have position tracking
   if (hmdState.StatusFlags & ovrStatus_PositionTracked)
      return float3( &hmdState.HeadPose.ThePose.Position.x );

   return float3(0.0f, 0.0f, 0.0f);
   }

   void OculusDK2::reset(void)
   {
   ovr_RecenterPose(session);
   }

   float2 OculusDK2::playAreaDimensions(void)
   {
   return float2(0.0f, 0.0f);
   }

   bool OculusDK2::playAreaLocation(float3* corners)
   {
   return false;
   }

   bool OculusDK2::debugHUD(DebugHUD mode)
   {
   switch(mode)
      {
      case Off:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_Off);
         break;
      case LatencyTiming:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_LatencyTiming);
         break;
      case RenderTiming:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_RenderTiming);
         break;
      case PerformanceHeadroom:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_PerfHeadroom);
         break;
      case Info:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_VersionInfo);
         break;

      default:
      break;
      }
   return true;
   }


/// Layer HUD enables the HMD user to see information about a layer
///
///     App can toggle layer HUD modes as such:
///     \code{.cpp}
///    ovrLayerHud_Off = 0, ///< Turns off the layer HUD
///    ovrLayerHud_Info = 1, ///< Shows info about a specific layer
///         ovrLayerHudMode LayerHudMode = ovrLayerHud_Info;
///         ovr_SetInt(Hmd, OVR_LAYER_HUD_MODE, (int)LayerHudMode);
///     \endcode
///


   float3 OculusDK2::position(Eye eye) const
   {
   // As Osulus uses OpenGL coordinate system where -Z is Look vector, we need to swap X and Z axes.
   return float3( -eyePose[(uint8)eye].Position.x, eyePose[(uint8)eye].Position.y, -eyePose[(uint8)eye].Position.z );
   } 

   float4x4 OculusDK2::orientation(Eye eye) const
   {
   // Hint: Oculus works on row-major matrices, we use column-major ones in OpenGL
   float3 rotation;
   OVR::Quat<float> quat( eyePose[(uint8)eye].Orientation );
   quat.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&rotation.y, &rotation.x, &rotation.z);

   // -X and -Z because in Engine coordinate system axes are reverted, so does the rotations.
   return mul(float4x4::rotationY(degrees(rotation.y)), 
          mul(float4x4::rotationX(-degrees(rotation.x)), 
              float4x4::rotationZ(-degrees(rotation.z))));
   }

   float3 OculusDK2::vector(Eye eye) const
   {
   return float3( &EyeRenderDesc[eye].HmdToEyeViewOffset.x );
   }

   Ptr<Texture> OculusDK2::color(Eye eye) const
   {
   assert( enabled );
   
   uint32 index = sharedRT ? 0 : static_cast<uint32>(eye);
   return swap[index][ textureSet[index]->CurrentIndex ];
   }
          
   //Ptr<Texture> OculusDK2::framebuffer(void) const
   //{
   //assert( enabled );
   //
   //return mirror;
   //}
  
   //void OculusDK2::waitForSync(void)
   //{
   //#if !OCULUS_SDK_RENDERING  
   //ovr_WaitTillTime(startTime.TimewarpPointSeconds);
   //#endif
   //}

   //float4 OculusDK2::latencyTesterSquareColor(void)
   //{
   //#if !OCULUS_SDK_RENDERING  
   //// Return actual latency-tester square color
   //uint8 latencyColor[3];
   //if (ovrHmd_GetLatencyTest2DrawColor(hmd, latencyColor))
   //   return float4(latencyColor[0] / 255.0f, latencyColor[1] / 255.0f, latencyColor[2] / 255.0f, 1.0f);
   //#endif
   //return float4(0.0f, 0.0f, 0.0f, 0.0f);
   //}

   //float4x4 OculusDK2::eyeRotationStartMatrix(Eye eye)
   //{
   //#if !OCULUS_SDK_RENDERING  
   //ovrMatrix4f timeWarpMatrices[2];            
   //ovrHmd_GetEyeTimewarpMatrices(hmd, (ovrEyeType)eye, eyePose[(uint32)eye], timeWarpMatrices);
   //return float4x4( &(OVR::Matrix4f(timeWarpMatrices[0]).Transposed().M[0][0]) );
   //#endif
   //return float4x4();
   //}

   //float4x4 OculusDK2::eyeRotationEndMatrix(Eye eye)
   //{
   //#if !OCULUS_SDK_RENDERING  
   //ovrMatrix4f timeWarpMatrices[2];            
   //ovrHmd_GetEyeTimewarpMatrices(hmd, (ovrEyeType)eye, eyePose[(uint32)eye], timeWarpMatrices);
   //return float4x4( &(OVR::Matrix4f(timeWarpMatrices[1]).Transposed().M[0][0]) );
   //#endif
   //return float4x4();
   //}

   void OculusDK2::endFrame(bool mirrorToScreen)
   {
   // Do distortion rendering, Present and flush/sync

   // Set up positional data.
   ovrViewScaleDesc viewScaleDesc;
   viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
   viewScaleDesc.HmdToEyeViewOffset[0] = EyeRenderDesc[0].HmdToEyeViewOffset;
   viewScaleDesc.HmdToEyeViewOffset[1] = EyeRenderDesc[1].HmdToEyeViewOffset;
   
   ovrLayerEyeFov ld;
   ld.Header.Type  = ovrLayerType_EyeFov;
   ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
   
   for(uint32 eye=0; eye<2; ++eye)
      {
      ld.ColorTexture[eye] = sharedRT ? textureSet[0] : textureSet[eye];
      ld.Viewport[eye]     = OVR::Recti(view[eye].x, view[eye].y, view[eye].width, view[eye].height);
      ld.Fov[eye]          = info.DefaultEyeFov[eye];
      ld.RenderPose[eye]   = eyePose[eye];
      }
   
   ovrLayerHeader* layers = &ld.Header;
   ovrResult result = ovr_SubmitFrame(session, 0, &viewScaleDesc, &layers, 1);
   //isVisible = result == ovrSuccess;

   // Blit HMD content to window (flip the texture upside-down during blit)
   if ( mirrorToScreen &&
        mirror->width() != 0 &&
        mirror->height() != 0 )
      {
      Gpu.scissor.rect(0, 0, mirror->width(), mirror->height());
      Gpu.output.buffer.setRead(fbo);
      Gpu.output.buffer.setDefaultWrite();
      Gpu.output.buffer.copy(uint32v4(0, mirror->height(), mirror->width(), 0), uint32v4(0, 0, mirror->width(), mirror->height()), Nearest);
      Gpu.output.buffer.setDefaultRead();
      }

   }   

   //ovrHmd OculusDK2::handle(void)
   //{
   //return hmd;
   //}
#endif              
#endif

   //# CONTEXT
   //##########################################################################

   void Context::HMD::init(void)
   {
#if /*defined(EN_PLATFORM_OSX) ||*/ defined(EN_PLATFORM_WINDOWS)
#if OCULUS_VR

   // Initialize 
   //ovrInitParams params;
   //params.RequestedMinorVersion;
   //params.LogCallback;
   //params.Flags;
   //params.ConnectionTimeoutMS;
   ovrResult res = ovr_Initialize( nullptr /*&params*/);
   if (res != ovrSuccess)
      {
      ovrErrorInfo* info = nullptr;
      ovr_GetLastErrorInfo(info);
      if (info->Result == ovrError_Initialize) 
         Log << "ERROR Oculus Init Failed: Generic initialization error.\n";
      if (info->Result == ovrError_LibLoad) 
         Log << "ERROR Oculus Init Failed: Couldn't load LibOVRRT.\n";
      if (info->Result == ovrError_LibVersion) 
         Log << "ERROR Oculus Init Failed: LibOVRRT version incompatibility.\n";
      if (info->Result == ovrError_ServiceConnection) 
         Log << "ERROR Oculus Init Failed: Couldn't connect to the OVR Service.\n";
      if (info->Result == ovrError_ServiceVersion) 
         Log << "ERROR Oculus Init Failed: OVR Service version incompatibility.\n";
      if (info->Result == ovrError_IncompatibleOS) 
         Log << "ERROR Oculus Init Failed: The operating system version is incompatible.\n";
      if (info->Result == ovrError_DisplayInit) 
         Log << "ERROR Oculus Init Failed: Unable to initialize the HMD display.\n";
      if (info->Result == ovrError_ServerStart) 
         Log << "ERROR Oculus Init Failed: Unable to start the server. Is it already running?\n";
      if (info->Result == ovrError_Reinitialization) 
         Log << "ERROR Oculus Init Failed: Attempting to re-initialize with a different version.\n";
      return;
      }

   // Get all detected Oculus Head Mounted Displays
   uint32 devices = 1; //ovrHmd_Detect();
   for(uint32 i=0; i<devices; ++i)
      device.push_back(Ptr<input::HMD>(new OculusDK2(i)));
#endif
#endif
   }

   void Context::HMD::destroy(void)
   {
   device.clear();
#if /*defined(EN_PLATFORM_OSX) ||*/ defined(EN_PLATFORM_WINDOWS)
#if OCULUS_VR
   ovr_Shutdown(); 
#endif
#endif
   }

   }
}