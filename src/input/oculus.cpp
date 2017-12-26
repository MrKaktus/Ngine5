/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#include "input/oculus.h"

#if defined(EN_MODULE_OCULUS)

#include "core/log/log.h"
#include "core/rendering/opengl/opengl.h"   // For OpenGL integration
#include "core/rendering/opengl/glTexture.h"   // For OpenGL integration

#include "scene/cam.h"
#include "utilities/strings.h"


//#pragma comment(lib, "LibOVR")
#include "OculusSDK/LibOVR/Include/Extras/OVR_Math.h" // Oculus 



namespace en
{
   namespace input
   {
   //# IMPLEMENTATION
   //##########################################################################

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
   //ovr_ConfigureTracking(session, 0, 0);
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
   //assert( GpuContext.screen.created );

   // Create double buffered textures for each used rendertarget
   for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
      {
      ovrTextureSwapChainDesc desc;
      desc.Type        = ovrTexture_2D;
      desc.Format      = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
      desc.ArraySize   = 1;                   // Not supported on PC at this time.
      desc.Width       = resolutionRT.width;
      desc.Height      = resolutionRT.height;
      desc.MipLevels   = 1;
      desc.SampleCount = 1;                   // Current only supported on depth textures
      desc.StaticImage = ovrFalse;   
      desc.MiscFlags   = ovrTextureMisc_None; // (ovrTextureFlags) ovrTextureMisc_ProtectedContent 
      desc.BindFlags   = ovrTextureBind_None; // (ovrTextureBindFlags) Not used for GL.

      // >>>>> OpenGL Specific code section - START
      ovrResult result = ovr_CreateTextureSwapChainGL(session, &desc, &swapChain[i]);
      ovr_GetTextureSwapChainLength(session, swapChain[i], (int*)&swapChainLength[i]);

      for(sint32 j=0; j<swapChainLength[i]; ++j)
         {
         assert( swapChainLength[i] <= 2 );
         uint32 handle;
         ovr_GetTextureSwapChainBufferGL(session, swapChain[i], j, &handle);

         // This modifies OpenGL state of currently bound Texture to texture unit X !!!
         glBindTexture(GL_TEXTURE_2D, handle);
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

         swap[i][j] = make_shared<TextureGL>(state, handle);
         }
      // >>>>> OpenGL Specific code section - END 
      }

   // Create mirror texture that can be read by FBO to optional window
   en::gpu::TextureState state;
   state.type    = TextureType::Texture2D;
   state.format  = Format::RGBA_8_sRGB;
   state.width   = windowResolution.width;
   state.height  = windowResolution.height;
   state.depth   = 1;
   state.layers  = 1;
   state.samples = 1;
   state.mipmaps = 1;

   ovrMirrorTextureDesc desc;
   desc.Format      = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
   desc.Width       = windowResolution.width;
   desc.Height      = windowResolution.height;
   desc.MiscFlags   = ovrTextureMisc_None; // (ovrTextureFlags) ovrTextureMisc_ProtectedContent 
 
   // >>>>> OpenGL Specific code section - START
   ovrResult result = ovr_CreateMirrorTextureGL(session, &desc, &mirrorTexture);

   uint32 handle;
   result = ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &handle);

   mirror = make_shared<TextureGL>(state, handle);
   // >>>>> OpenGL Specific code section - END

   // Create Framebuffer to use with mirroring
// TODO: REWORK FOR NEW RENDERING INTERFACE
//   fbo = Gpu.output.buffer.create();
//   fbo->color(Read, 0, mirror);

   // All Caps are enabled by default from 0.8
   //ovr_SetEnabledCaps(session, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction );

   ovr_SetTrackingOriginType(session, ovrTrackingOrigin_EyeLevel);      // ovrTrackingOrigin_FloorLevel - For standing experiences

   // Turn on the Oculus
   //ovrResult res = ovr_ConfigureTracking(session, ovrTrackingCap_Orientation      | 
   //                                               ovrTrackingCap_MagYawCorrection | 
   //                                               ovrTrackingCap_Position, 
   //                                               ovrTrackingCap_Orientation);
   //if (res != ovrSuccess)
   //   return false;

   EyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left,  info.DefaultEyeFov[0]);
   EyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, info.DefaultEyeFov[1]);

   ovr_RecenterTrackingOrigin(session);

   enabled = true;
   return true;
   }

   bool OculusDK2::off(void)
   {
   if (!enabled)
      return true;

   // Remove Oculus Swap-Chain
   for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
      {
      ovr_DestroyTextureSwapChain(session, swapChain[i]);
      for(sint32 j=0; j<swapChainLength[i]; ++j)
         swap[i][j] = nullptr;
      }
   mirror = nullptr;

   // Turn off display
   if (displayMode == Direct)
      {
      // TODO: Turn off display here
      }

   // Disable Oculus 
   //ovr_ConfigureTracking(session, 0, 0);
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

   //shared_ptr<en::resource::Model> OculusDK2::distortionModel(DistortionSettings* settings)
   //{
   //#if !OCULUS_SDK_RENDERING
   //using namespace en::gpu;
   //using namespace en::resource;
   //
   //if (!Gpu.screen.created())
   //   return shared_ptr<Model>(nullptr);
   //
   //shared_ptr<en::resource::Model> model = new en::resource::Model();
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
   //return shared_ptr<en::resource::Model>(nullptr);
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


   void OculusDK2::startFrame(uint32 _frameIndex)
   {
   frameIndex = _frameIndex;

   // Those values may change at runtime
   EyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left,  info.DefaultEyeFov[0]);
   EyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, info.DefaultEyeFov[1]);

   ovrVector3f hmdToEyeOffset[2] = { EyeRenderDesc[0].HmdToEyeOffset,
                                     EyeRenderDesc[1].HmdToEyeOffset };

   ovr_GetEyePoses(session, frameIndex, ovrTrue, hmdToEyeOffset, &eyePose[0], &sensorSampleTime);      

   // Swap used rendertarget textures in swap-chain
   for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
      ovr_GetTextureSwapChainCurrentIndex(session, swapChain[i], &currentIndex[i]);

   //ovr_GetTextureSwapChainBufferGL(session, swapChain, curIndex, &curTexId);  // Not needed, we've already packaged those textures



   //// Swap used rendertarget textures in swap-chain
   //for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
   //   textureSet[i]->CurrentIndex = (textureSet[i]->CurrentIndex + 1) % textureSet[i]->TextureCount;

   ////#if OCULUS_SDK_RENDERING
   ////ovrFrameTiming   ftiming  = ovrH_GetFrameTiming(hmd, 0);
   //ovrTrackingState hmdState = ovr_GetTrackingState(session, 0, ovrTrue); //ftiming.DisplayMidpointSeconds
   //ovr_CalcEyePoses(hmdState.HeadPose.ThePose, hmdToEyeViewOffset, &eyePose[0]);
   ////#else
   ////startTime = ovrHmd_BeginFrameTiming(hmd, frameIndex);
   ////
   ////ovrHmd_GetEyePoses(hmd, 0, hmdToEyeViewOffset, &eyePose[0], NULL); // &hmdState - Samples are tracking head properly without obtaining this info, how??
   ////#endif
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
   ovr_RecenterTrackingOrigin(session);
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
      case ApplicationRenderTiming:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_AppRenderTiming);
         break;
      case CompositorRenderTiming:
         ovr_SetInt(session, OVR_PERF_HUD_MODE, (int)ovrPerfHud_CompRenderTiming);
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
   return float3( &EyeRenderDesc[eye].HmdToEyeOffset.x );
   }

   shared_ptr<Texture> OculusDK2::color(Eye eye) const
   {
   assert( enabled );
   
   uint32 index = sharedRT ? 0 : min(1, static_cast<uint32>(eye));
   return swap[index][currentIndex];
   }
          
   //shared_ptr<Texture> OculusDK2::framebuffer(void) const
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

   void OculusDK2::endRendering(Eye eye)
   {
   // Commit changes to textures in Swap-Chain
   uint32 index = sharedRT ? 0 : static_cast<uint32>(eye);
   ovr_CommitTextureSwapChain(session, swapChain[index]);
   }

   void OculusDK2::endFrame(bool mirrorToScreen)
   {
   // Do distortion rendering, Present and flush/sync

   // Set up positional data.
   ovrViewScaleDesc viewScaleDesc;
   viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
   viewScaleDesc.HmdToEyeOffset[0] = EyeRenderDesc[0].HmdToEyeOffset;
   viewScaleDesc.HmdToEyeOffset[1] = EyeRenderDesc[1].HmdToEyeOffset;
   
   ovrLayerEyeFov ld;
   ld.Header.Type  = ovrLayerType_EyeFov;
   ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
   
   for(uint32 eye=0; eye<2; ++eye)
      {
      ld.ColorTexture[eye] = sharedRT ? swapChain[0] : swapChain[eye];
      ld.Viewport[eye]     = OVR::Recti(view[eye].x, view[eye].y, view[eye].width, view[eye].height);
      ld.Fov[eye]          = info.DefaultEyeFov[eye];
      ld.RenderPose[eye]   = eyePose[eye];
      ld.SensorSampleTime  = sensorSampleTime;
      }
   
   ovrLayerHeader* layers = &ld.Header;
   ovrResult result = ovr_SubmitFrame(session, frameIndex, &viewScaleDesc, &layers, 1);
   //isVisible = result == ovrSuccess;

   ovrSessionStatus sessionStatus;
   ovr_GetSessionStatus(session, &sessionStatus);
 //if (sessionStatus.ShouldQuit)
   if (sessionStatus.ShouldRecenter)
      ovr_RecenterTrackingOrigin(session);

   // Blit HMD content to window (flip the texture upside-down during blit)
   if ( mirrorToScreen &&
        mirror->width() != 0 &&
        mirror->height() != 0 )
      {
// TODO: REWORK TO NEW RENDERING INTERFACE
//      Gpu.scissor.rect(0, 0, mirror->width(), mirror->height());
//      Gpu.output.buffer.setRead(fbo);
//      Gpu.output.buffer.setDefaultWrite();
//      Gpu.output.buffer.copy(uint32v4(0, mirror->height(), mirror->width(), 0), uint32v4(0, 0, mirror->width(), mirror->height()), Nearest);
//      Gpu.output.buffer.setDefaultRead();
      }

   }   

   //ovrHmd OculusDK2::handle(void)
   //{
   //return hmd;
   //}

   //# CONTEXT
   //##########################################################################

   void InitOculusSDK(void)
   {
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

   // Register Oculus HMD
   CommonInterface* input = reinterpret_cast<CommonInterface*>(en::Input.get());
   input->hmds.push_back(make_shared<OculusDK2>(0));
   }

   void CloseOculusSDK(void)
   {
   ovr_Shutdown(); 
   }

   }
}
#endif
