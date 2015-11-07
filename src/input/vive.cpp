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
#include "core/rendering/rendering.h"

#include "core/rendering/opengl/gl43Texture.h"
#include "resources/effect.h"

#if OPEN_VR
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "input/vive.h"

namespace en
{
   namespace input
   {
   //# IMPLEMENTATION
   //##########################################################################

   string deviceString(vr::IVRSystem* ctx, vr::TrackedDeviceIndex_t deviceIndex, vr::TrackedDeviceProperty deviceProperty, vr::TrackedPropertyError* error = nullptr)
   {
   uint32 stringLen = ctx->GetStringTrackedDeviceProperty(deviceIndex, deviceProperty, nullptr, 0, error);
   if (stringLen != 0)
      {
	  uint8* buffer = new uint8[stringLen];
      stringLen = ctx->GetStringTrackedDeviceProperty(deviceIndex, deviceProperty, reinterpret_cast<char*>(buffer), stringLen, error);
      string text(reinterpret_cast<char*>(buffer));
      delete [] buffer;
      return text;
      }
   return string();
   }

   ValveHMD::ValveHMD(uint8 index) :
      enabled(false),
      sharedRT(true),
      deviceType(HMDUnknown),
      displayId(-1),
      displayMode(Direct),
      context(nullptr),
      renderModels(nullptr),
      compositor(nullptr),
      currentIndex(0),
      chaperone(nullptr),
      areaDimensions(0.0f, 0.0f),
      model(nullptr),
      distortion(nullptr)
   {
   // OpenVR currently supports only one HMD
   assert( index == 0 );

   // Initialize the SteamVR Runtime
   vr::HmdError res = vr::HmdError_None;
   context = vr::VR_Init(&res);
   if (res != vr::HmdError_None)
      {
      context = nullptr;
      Log << "ERROR OpenVR Init Failed: " << vr::VR_GetStringForHmdError(res) << endl;
      return;
      }

   // Initialize interface to HMD Mesh Library
   renderModels = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &res);
   if (!renderModels)
      {
      context = nullptr;
      vr::VR_Shutdown();
      Log << "ERROR OpenVR Init Failed: " << vr::VR_GetStringForHmdError(res) << endl;
      return;
      }

   // Acquire HMD window position and size
//   context->GetWindowBounds(&windowPositionX, &windowPositionY, &displayResolution.width, &displayResolution.height);

   // Get chaperone sub-system handle
   chaperone = vr::VRChaperone(); // or use (vr::IVRChaperone*)vr::VR_GetGenericInterface(vr::IVRChaperone_Version, &res);

   // Get play area size in XZ plane. 
   // Center point is players preferred starting point.
   chaperone->GetPlayAreaSize(&areaDimensions.x, &areaDimensions.y);

   // Get play area axis-aligned orientantion from starting position.
   // Corners are in clockwise order, placed on XZ plane where player is looking forward in -Z direction.
   vr::HmdQuad_t tempCorners;
   if (chaperone->GetPlayAreaRect(&tempCorners))
      for(uint32 i=0; i<4; ++i)
         areaCorners[i] = float3(tempCorners.vCorners[i].v[0], tempCorners.vCorners[i].v[1], tempCorners.vCorners[i].v[2]);

   // Other sub-systems to use in the future

   //VR_INTERFACE vr::IVROverlay *VR_CALLTYPE VROverlay();
   //VR_INTERFACE vr::IVRControlPanel *VR_CALLTYPE VRControlPanel();
   //VR_INTERFACE vr::IVRTrackedCamera *VR_CALLTYPE VRTrackedCamera();

   // Print out device info
   Log << "Detected HMD system:\n";
   for(vr::TrackedDeviceIndex_t i=0; i<vr::k_unMaxTrackedDeviceCount; ++i)
      {
	  // Prop_TrackingSystemName_String
	  // Prop_ModelNumber_String
	  // Prop_SerialNumber_String
	  // Prop_RenderModelName_String
	  // Prop_ManufacturerName_String
	  // Prop_TrackingFirmwareVersion_String
	  // Prop_HardwareRevision_String
      string deviceName = deviceString(context, i, vr::Prop_RenderModelName_String );
      string serialNumber = deviceString(context, i, vr::Prop_SerialNumber_String );
      if (!deviceName.empty() && !serialNumber.empty())
         {
         string strWindowTitle = "  - device: " + deviceName + " \t" + serialNumber + "\n";
         Log << strWindowTitle.c_str();
         }
      }

   // Acquire camera tangents for projection matrices
   // Hint: Tangents are returned in order: Top, Bottom, Left, Right
   context->GetProjectionRaw(vr::Eye_Left,  &tangents[0].z, &tangents[0].w, &tangents[0].x, &tangents[0].y);
   context->GetProjectionRaw(vr::Eye_Right, &tangents[1].z, &tangents[1].w, &tangents[1].x, &tangents[1].y);

   // Tangents need to be absolute values to properly calculate projection matrices
   for(uint8 i=0; i<2; ++i)
      {
      tangents[i].x = abs(tangents[i].x);
      tangents[i].y = abs(tangents[i].y);
      tangents[i].z = abs(tangents[i].z);
      tangents[i].w = abs(tangents[i].w);
      }

   // HMD to eye view vectors
   // Hint: OpenVR works on row-major matrices, we use column-major ones in OpenGL
   vr::HmdMatrix34_t matEyeLeft = context->GetEyeToHeadTransform(vr::Eye_Left);
   eyeVector[0].x = matEyeLeft.m[0][3];
   eyeVector[0].y = matEyeLeft.m[1][3];
   eyeVector[0].z = matEyeLeft.m[2][3];

   vr::HmdMatrix34_t matEyeRight = context->GetEyeToHeadTransform(vr::Eye_Right);
   eyeVector[1].x = matEyeRight.m[0][3];
   eyeVector[1].y = matEyeRight.m[1][3];
   eyeVector[1].z = matEyeRight.m[2][3];

   deviceType = HMDVive;

   //// Determine HMD type
   //if (hmd->Type == ovrHmd_DK1)
   //   deviceType = HMDVive;
   //else
   //   deviceType = HMDUnknown;
   }

   ValveHMD::~ValveHMD()
   {
   // TODO: Disable all subsystems here
   context->ReleaseInputFocus(); 

   // Release all OpenVR interfaces
   compositor   = nullptr;
   renderModels = nullptr;
   context      = nullptr;

   // Remove all controller interfaces
   for(uint32 i=0; i<vr::k_unMaxTrackedDeviceCount; ++i)
      handle[i] = nullptr;

   // Clean all cached resources
   modelCache.clear();
   }

   bool ValveHMD::on(uint32v2 windowResolution, bool sharedRendertarget)
   {
   if (enabled)
      return true;

   sharedRT = sharedRendertarget;

   // Init compositor
   vr::HmdError res = vr::HmdError_None;
   compositor = reinterpret_cast<vr::IVRCompositor*>(vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &res)); 
   if (!compositor || res != vr::HmdError_None)
      {
      compositor = nullptr;
      Log << "ERROR OpenVR Compositor initialization failed: " << vr::VR_GetStringForHmdError(res) << endl;
      return false;
      }

   // Detect any Compositor errors on init
   uint32 size = compositor->GetLastError(nullptr, 0);
   if (size > 1)
      {
      uint8* buffer = new uint8[size];
      compositor->GetLastError(reinterpret_cast<char*>(buffer), size);
      Log << "ERROR OpenVR Compositor error: " << buffer << endl;
      delete [] buffer;
      }

   // Calculate resolution of Render Target
   context->GetRecommendedRenderTargetSize(&resolutionRT.width, &resolutionRT.height);
   if (sharedRT)
      resolutionRT.x = resolutionRT.x * 2;

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

   assert( GpuContext.screen.created );

   // Store resolution of window to which we will be mirroring to
   window = windowResolution;

   // Create double buffered textures for each used rendertarget
   for(uint32 i=0; i<(sharedRT ? 1U : 2U); ++i)
      {
      for(sint32 j=0; j<2; ++j)
         {
         // Create texture interface that can be exposed to application
         en::gpu::TextureState state;
         state.type    = Texture2D;
         state.format  = FormatRGBA_8_sRGB; //FormatRGBA_8;
         state.width   = resolutionRT.width;
         state.height  = resolutionRT.height;
         state.depth   = 1;
         state.layers  = 1;
         state.samples = 1;
         state.mipmaps = 1;
 
         swap[i][j] = Gpu.texture.create(state);
         }
      }

   // Create distortion model
   distortionModel();

   // Create distortion rendering program
   Effect effect(eGLSL_4_10, "enOpenVR", false);
   distortion = effect.program();
   assert( distortion.samplerLocation("inTexture", samplerLocation) );

   // There is no game state set yet, so this won't work, it's too soon !

   //// Try to manually init controllers on startup
   //vr::TrackedDeviceIndex_t controllerIndexes[vr::k_unMaxTrackedDeviceCount];
   //uint32 count = context->GetSortedTrackedDeviceIndicesOfClass(vr::TrackedDeviceClass_Controller, controllerIndexes, vr::k_unMaxTrackedDeviceCount);
   //for(uint32 i=0; i<count; ++i)
   //   {
   //   if (i == vr::k_unMaxTrackedDeviceCount)
   //      break;
   //
   //   // Activate discovered controllers
   //   vr::TrackedDeviceIndex_t deviceId = controllerIndexes[count - i - 1];
   //   if (handle[deviceId] == nullptr)
   //      activateController(deviceId);
   //   }
   
   enabled = true;
   return true;
   }

   bool ValveHMD::off(void)
   {
   if (!enabled)
      return true;

   // Turn off display
   if (displayMode == Direct)
      {
      // TODO: Turn off display here
      }

   // Remove texture interfaces
   swap[0][0] = nullptr;
   swap[0][1] = nullptr;
   swap[1][0] = nullptr;
   swap[1][1] = nullptr;
   //mirror     = nullptr;
   model      = nullptr;

   // Disable HMD
 
   // TODO: Disable HMD here , disable Compositor

   enabled = false;
   return true;
   }

    // MSAA Forward:


    // // Create MSAA x4 color buffer attachment to FBO
	// glGenTextures(1, &framebufferDesc.m_nRenderTextureId );
	// glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId );
	// glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
    // 
    // // Create Resolve Color texture
	// glGenTextures(1, &framebufferDesc.m_nResolveTextureId );
	// glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId );
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // 
    // 
    // 
    // 
    // // Create FBO with MSAA x4 (plus backing depth buffer)
    // glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId );
	// glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);
    // 
    // // MSAA x4 depth buffer 
	// glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	// glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight );
    // 
	// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	framebufferDesc.m_nDepthBufferId );
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);
    // 
    // 
    // 
    // // Create Resolve FBO
	// glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId );
	// glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);
    // 
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);
    // 
	// // check FBO status
	// GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	// if (status != GL_FRAMEBUFFER_COMPLETE)
	// {
	// 	return false;
	// }
    // 
	// glBindFramebuffer( GL_FRAMEBUFFER, 0 );




   bool ValveHMD::state(void) const
   {
   return enabled;
   }

   HMDType ValveHMD::device(void) const
   {
   return deviceType;
   }

   HMDVendor ValveHMD::vendor(void) const
   {
   return HMDValve;
   }

   HMDMode ValveHMD::mode(void) const
   {
   return displayMode;
   }

   sint32 ValveHMD::display(void) const
   {
   return displayId;
   }

   uint32v2 ValveHMD::resolution(void) const
   {
   return displayResolution;
   }

   uint32v2 ValveHMD::renderTarget(void) const
   {
   return resolutionRT;
   }

   bool ValveHMD::sharedRendertarget(void) const
   {
   return sharedRT;
   }

   uint32v4 ValveHMD::viewport(uint8 eye) const
   {
   return view[eye];
   }

   float4 ValveHMD::frustumTangents(Eye eye) const
   {
   return tangents[static_cast<uint32>(eye)];
   }

   void ValveHMD::distortionModel(void)
   {
   using namespace en::gpu;
   using namespace en::resource;

   assert( Gpu.screen.created() );
   assert( model == nullptr );

   model = new en::resource::Model();
   model->name = string("OpenVR Distortion Mesh");
   model->mesh.resize(2);

   sint16 lensGridSegmentCountH = 43;
   sint16 lensGridSegmentCountV = 43;
   uint32 vertices = lensGridSegmentCountV * lensGridSegmentCountH;
   uint32 indices = (lensGridSegmentCountV-1) * (lensGridSegmentCountH-1) * 6;
   float w = 1.0f / static_cast<float>(lensGridSegmentCountH-1);
   float h = 1.0f / static_cast<float>(lensGridSegmentCountV-1);
   float u, v = 0;

   // Generates distortion model with two meshes, each for each eye.
   for(uint8 eye=0; eye<2; ++eye)
      {
      // Create Vertex Buffer
      BufferSettings vboSettings(VertexBuffer,
                                 vertices,
                                 ColumnInfo(Float2,"inPosition"),
                                 ColumnInfo(Float2,"inTexCoord0"),
                                 ColumnInfo(Float2,"inTexCoord1"),
                                 ColumnInfo(Float2,"inTexCoord2"));
      
      model->mesh[eye].geometry.buffer = Gpu.buffer.create(vboSettings);
      model->mesh[eye].geometry.begin  = 0;
      model->mesh[eye].geometry.end    = vertices;

      // Fill Vertex Buffer with data
      aligned(1);
      struct DistortionVertex
             {
             float2 position;
             float2 uv[3];
             };
      aligndefault

      // Generate distortion mesh vertices
      DistortionVertex* dst = static_cast<DistortionVertex*>(model->mesh[eye].geometry.buffer.map());

      // Eye distortion verts
      float Xoffset = eye == 0 ? -1.0f : 0.0f;

      for(uint32 y=0; y<lensGridSegmentCountV; ++y)
         for(uint32 x=0; x<lensGridSegmentCountH; ++x)
            {
            u =        static_cast<float>(x) * w; 
            v = 1.0f - static_cast<float>(y) * h;

			vr::DistortionCoordinates_t dc0 = context->ComputeDistortion(static_cast<vr::Hmd_Eye>(eye), u, v);

			dst->position = float2(Xoffset + u,   -1.0f + 2.0f * y * h);

            // If both eyes share one render target, adjust UV's per eye
            if (sharedRT)
               {
               if (eye == 0)
                  {
			      dst->uv[0] = float2(0.5f * dc0.rfRed[0],   1.0f - dc0.rfRed[1]);
			      dst->uv[1] = float2(0.5f * dc0.rfGreen[0], 1.0f - dc0.rfGreen[1]);
			      dst->uv[2] = float2(0.5f * dc0.rfBlue[0],  1.0f - dc0.rfBlue[1]);
                  }
               else
                  {
			      dst->uv[0] = float2(0.5f * dc0.rfRed[0]   + 0.5f,  1.0f - dc0.rfRed[1]);
			      dst->uv[1] = float2(0.5f * dc0.rfGreen[0] + 0.5f,  1.0f - dc0.rfGreen[1]);
			      dst->uv[2] = float2(0.5f * dc0.rfBlue[0]  + 0.5f,  1.0f - dc0.rfBlue[1]);
                  }
               }
            else
               {
			   dst->uv[0] = float2(dc0.rfRed[0],   1.0f - dc0.rfRed[1]);
			   dst->uv[1] = float2(dc0.rfGreen[0], 1.0f - dc0.rfGreen[1]);
			   dst->uv[2] = float2(dc0.rfBlue[0],  1.0f - dc0.rfBlue[1]);
               }

			dst++;
            }

      model->mesh[eye].geometry.buffer.unmap();

      // Create Index Buffer
      model->mesh[eye].elements.buffer  = Gpu.buffer.create(BufferSettings(IndexBuffer, indices, ColumnType(UShort)));
      model->mesh[eye].elements.type    = Triangles;
      model->mesh[eye].elements.offset  = 0;
      model->mesh[eye].elements.indexes = indices;

      // Generate distortion mesh indices
      uint16* index = static_cast<uint16*>(model->mesh[eye].elements.buffer.map());
      for(uint16 y=0; y<lensGridSegmentCountV-1; ++y)
         for(uint16 x=0; x<lensGridSegmentCountH-1; ++x)
            {
			uint16 a = lensGridSegmentCountH * y + x;
			uint16 b = lensGridSegmentCountH * y + x + 1;
			uint16 c = lensGridSegmentCountH * (y+1) + x + 1;
			uint16 d = lensGridSegmentCountH * (y+1) + x;
			*index = a; index++;
			*index = b; index++;
			*index = c; index++;
			*index = a; index++;
			*index = c; index++;
			*index = d; index++;
            }
      model->mesh[eye].elements.buffer.unmap();
      }
   }


// struct RenderModel_Vertex_t
// {
// 	HmdVector3_t vPosition;		// position in meters in device space
// 	HmdVector3_t vNormal;		
// 	float rfTextureCoord[ 2 ];
// };
// 
// /** A texture map for use on a render model */
// struct RenderModel_TextureMap_t
// {
// 	uint16_t unWidth, unHeight; // width and height of the texture map in pixels
// 	const uint8_t *rubTextureMapData;	// Map texture data. All textures are RGBA with 8 bits per channel per pixel. Data size is width * height * 4ub
// };
// 
// struct RenderModel_t
// {
// 	uint64_t ulInternalHandle;					// Used internally by SteamVR
// 	const RenderModel_Vertex_t *rVertexData;			// Vertex data for the mesh
// 	uint32_t unVertexCount;						// Number of vertices in the vertex data
// 	const uint16_t *rIndexData;						// Indices into the vertex data for each triangle
// 	uint32_t unTriangleCount;					// Number of triangles in the mesh. Index count is 3 * TriangleCount
// 	RenderModel_TextureMap_t diffuseTexture;	// RGBA diffuse texture for the model
// };


   pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> > ValveHMD::controllerModel(const string name)
   {
   Ptr<en::resource::Model> model = nullptr;
   Ptr<en::gpu::Texture> texture = nullptr;

   // Check if needed model is not already in the cache
   map<string, pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> > >::iterator it = modelCache.find(name);
   if (it != modelCache.end())
      return it->second;

   vr::RenderModel_t tempModel;
   if (!renderModels->LoadRenderModel(name.c_str(), &tempModel))
      {
      Log << "Unable to load render model " << name << endl;
      return pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> >(model, texture);
      }

   // Create model for rendering
   model = new en::resource::Model();
   model->name = string("OpenVR Controller Mesh - " + name);
   model->mesh.resize(1);
   
   // Create Vertex Buffer
   BufferSettings vboSettings(VertexBuffer,
                              tempModel.unVertexCount,
                              ColumnInfo(Float3,"inPosition"),
                              ColumnInfo(Float3,"inNormal"),
                              ColumnInfo(Float2,"inTexCoord0"));
   
   model->mesh[0].geometry.buffer  = Gpu.buffer.create(vboSettings, (void*)(tempModel.rVertexData));
   model->mesh[0].geometry.begin   = 0;
   model->mesh[0].geometry.end     = tempModel.unVertexCount;
   
   // Create Index Buffer
   uint32 indices = tempModel.unTriangleCount * 3;
   model->mesh[0].elements.buffer  = Gpu.buffer.create(BufferSettings(IndexBuffer, indices, ColumnType(UShort)), (void*)(tempModel.rIndexData));
   model->mesh[0].elements.type    = Triangles;
   model->mesh[0].elements.offset  = 0;
   model->mesh[0].elements.indexes = indices;
   
   // Create texture
   TextureState settings(Texture2D, FormatRGBA_8, tempModel.diffuseTexture.unWidth, tempModel.diffuseTexture.unHeight);
   texture = en::Gpu.texture.create(settings);

   // Free temporary model
   renderModels->FreeRenderModel(&tempModel);
   
   // Add new model-texture pair to the cache
   modelCache.insert(pair<string, pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> > >(name, pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> >(model, texture) ));
 
   return pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> >(model, texture);
   }

   void ValveHMD::activateController(vr::TrackedDeviceIndex_t deviceId)
   {
   assert( handle[deviceId] == nullptr );

   // Acquire pointers to controller representation
   std::string name = deviceString(context, deviceId, vr::Prop_RenderModelName_String);
   pair< Ptr<en::resource::Model>, Ptr<en::gpu::Texture> > assets = controllerModel(name);
   
   // Create interface for detected controller
   handle[deviceId] = new ValveController(context, 
                                          &poseRender[deviceId], 
                                        //&poseGameplay[deviceId],   // TOOD: Pass also predicted pose for gameplay
                                          &controller[deviceId], 
                                          assets.first,           // Mesh
                                          assets.second,          // Texture
                                          deviceId);
   
   // Send event with new controller
   ControllerEvent event;
   memset(&event, 0, sizeof(ControllerEvent));
   event.type    = ControllerActivated;
   event.pointer = ptr_dynamic_cast<Controller, ValveController>(handle[deviceId]);
   en::InputContext.events.callback(event); 
   
   Log << "Device " << deviceId << " attached.\n";
   }

   void ValveHMD::startFrame(const uint32 frameIndex)
   {
   // Swap used rendertarget textures in swap-chain
   currentIndex = (currentIndex + 1) % 2;

   // Post Window-Swap operations (of previous frame)
   
   // We want to make sure the glFinish waits for the entire present to complete, not just the submission
   // of the command. So, we do a clear here right here so the glFinish will wait fully for the swap.
   Gpu.color.buffer.clearValue(0.0f, 0.0f, 0.0f, 1.0f);
   Gpu.color.buffer.clear();
   Gpu.depth.buffer.clear();
   
   // Flush and wait for swap
   // if (VSync)
   //Gpu.waitForIdle();
   
   // Update tracked device poses
   if (compositor)
      compositor->WaitGetPoses(poseRender, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
   }
            
   float3 ValveHMD::position(void) const
   {
   if (poseRender[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
      {
      const vr::HmdMatrix34_t& matPose = poseRender[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
      return float3( matPose.m[0][3], matPose.m[1][3], matPose.m[2][3] );
      }

   return float3(0.0f, 0.0f, 0.0f);
   }  

   void ValveHMD::update(void)
   {
   if (!enabled)
      return;

   // If input focus for controllers is lost, try to regain it
   if (context->IsInputFocusCapturedByAnotherProcess())
      {
      if (!context->CaptureInputFocus())
         {
         Log << " I have no controll over controllers, and cannot take it back!\n";
         }
      }

   // Process SteamVR events
   vr::VREvent_t vrevent;
   while(context->PollNextEvent(&vrevent))
      {
      switch(vrevent.eventType)
         {
         case vr::VREvent_TrackedDeviceActivated:
            {
            // We care only about controllers here
            vr::TrackedDeviceIndex_t deviceId = vrevent.trackedDeviceIndex;
	        if (context->GetTrackedDeviceClass(deviceId) != vr::TrackedDeviceClass_Controller)
               break;

            activateController(deviceId);
            }
            break;

         case vr::VREvent_TrackedDeviceDeactivated:
            {
            // We care only about controllers here
            vr::TrackedDeviceIndex_t deviceId = vrevent.trackedDeviceIndex;
	        if (context->GetTrackedDeviceClass(deviceId) != vr::TrackedDeviceClass_Controller)
               break;

            // Send event to discard controller
            ControllerEvent event;
            memset(&event, 0, sizeof(ControllerEvent));
            event.type    = ControllerDeactivated;
            event.pointer = ptr_dynamic_cast<Controller, ValveController>(handle[deviceId]);
            en::InputContext.events.callback(event); 

            // Remove controller
            handle[deviceId] = nullptr;

            Log << "Device " << deviceId << " detached.\n";
            }
            break;

         case vr::VREvent_TrackedDeviceUpdated:
            Log << "Device " << vrevent.trackedDeviceIndex << " updated.\n";
            break;

         case vr::VREvent_ButtonPress: 
            {
            // We care only about controllers here
            vr::TrackedDeviceIndex_t deviceId = vrevent.trackedDeviceIndex;
	        if (context->GetTrackedDeviceClass(deviceId) != vr::TrackedDeviceClass_Controller)
               Log << "PRESSED non CONTROLLER button ???? " << deviceId << "\n";

            // Activate controller if it wasn't activated yet
            if (handle[deviceId] == nullptr)
               activateController(deviceId);

            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = ControllerButtonPressed;
            // TODO: Fill the event with data, data is controller
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_ButtonUnpress:
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = ControllerButtonReleased;
            // TODO: Fill the event with data, data is controller
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_ButtonTouch: 
            {
            // Activate controller if it wasn't activated yet
            vr::TrackedDeviceIndex_t deviceId = vrevent.trackedDeviceIndex;
            if (handle[deviceId] == nullptr)
               activateController(deviceId);

            Event event;
            memset(&event, 0, sizeof(Event));
            event.type =  ControllerButtonTouched;
            // TODO: Fill the event with data, data is controller
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_ButtonUntouch:
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = ControllerButtonUntouched;
            // TODO: Fill the event with data, data is controller
            en::InputContext.events.callback(event); 
            }
            break;


         // Other application released controllers
         case vr::VREvent_InputFocusReleased: 
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = VRInputFocusAvailable;
            en::InputContext.events.callback(event); 
            // TODO: We could pass more data (process info) in the future
            }
            break;

         // Application lost controll over controller
         case vr::VREvent_InputFocusCaptured: 
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = VRInputFocusLost;
            en::InputContext.events.callback(event); 
            // TODO: We could pass more data (process info) in the future
            }
            break;






         case vr::VREvent_SceneFocusLost: 
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = VRSceneFocusLost;
            // TODO: Fill the event with data, data is process
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_SceneFocusGained: 
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = VRSceneFocusGained;
            // TODO: Fill the event with data, data is process
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_SceneApplicationChanged: 
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = VRSceneApplicationChanged;
            // TODO: Fill the event with data, data is process
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_Quit:
         case vr::VREvent_ProcessQuit:
            {
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = AppClose;
            en::InputContext.events.callback(event); 
            }
            break;

         case vr::VREvent_MouseMove: // data is mouse
         case vr::VREvent_MouseButtonDown: // data is mouse
         case vr::VREvent_MouseButtonUp: // data is mouse
         case vr::VREvent_OverlayShown:
         case vr::VREvent_OverlayHidden:
         case vr::VREvent_DashboardActivated:
         case vr::VREvent_DashboardDeactivated:
         case vr::VREvent_DashboardThumbSelected: // Sent to the overlay manager - data is overlay
         case vr::VREvent_DashboardRequested: // Sent to the overlay manager - data is overlay
         case vr::VREvent_ResetDashboard: // Send to the overlay manager
         case vr::VREvent_RenderToast: // Send to the dashboard to render a toast - data is the notification ID
         case vr::VREvent_ImageLoaded: // Sent to overlays when a SetOverlayRaw or SetOverlayFromFile call finishes loading
         case vr::VREvent_Notification_Show:
         case vr::VREvent_Notification_Dismissed:
         case vr::VREvent_Notification_BeginInteraction:
         case vr::VREvent_ChaperoneDataHasChanged:
         case vr::VREvent_ChaperoneUniverseHasChanged:
         case vr::VREvent_StatusUpdate:
         default:
            break;
         }
      }

   // Process SteamVR controller state
   for(vr::TrackedDeviceIndex_t i=0; i<vr::k_unMaxTrackedDeviceCount; ++i)
      {
      // Activate controller if it wasn't activated yet
      if (context->IsTrackedDeviceConnected(i))
	     if (context->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller)
            if (handle[i] == nullptr)
               activateController(i);

      // Gather controller current state
      context->GetControllerState(i, &controller[i]);

      //if (controller[i].ulButtonPressed != 0)
      //   Log << "Controller " << i << " button " << controller[i].ulButtonPressed << " pressed." << endl;
      //if (controller[i].ulButtonTouched != 0)
      //   Log << "Controller " << i << " button " << controller[i].ulButtonTouched << " touched." << endl;
      }
   }

   void ValveHMD::reset(void)
   {
   // Resets pose for seated experience
   context->ResetSeatedZeroPose();
   }

   float2 ValveHMD::playAreaDimensions(void)
   {
   return areaDimensions;
   }

   bool ValveHMD::playAreaLocation(float3* corners)
   {
   for(uint32 i=0; i<4; ++i)
      corners[i] = areaCorners[i];
   return true;
   }

   bool ViveHMD::debugHUD(DebugHUD mode)
   {
   return false;
   }


//   inline float SIGN(float x) {return (x >= 0.0f) ? +1.0f : -1.0f;}
//   inline float NORM(float a, float b, float c, float d) {return sqrt(a * a + b * b + c * c + d * d);}
//
//   float4 quaternion(float3x3 rotation)
//   {
//   float r11 = rotation.m[0];
//   float r21 = rotation.m[1];
//   float r31 = rotation.m[2];
//   float r12 = rotation.m[3];
//   float r22 = rotation.m[4];
//   float r32 = rotation.m[5];
//   float r13 = rotation.m[6];
//   float r23 = rotation.m[7];
//   float r33 = rotation.m[8];
//
//   float q0 = ( r11 + r22 + r33 + 1.0f) / 4.0f;
//   float q1 = ( r11 - r22 - r33 + 1.0f) / 4.0f;
//   float q2 = (-r11 + r22 - r33 + 1.0f) / 4.0f;
//   float q3 = (-r11 - r22 + r33 + 1.0f) / 4.0f;
//   if(q0 < 0.0f) q0 = 0.0f;
//   if(q1 < 0.0f) q1 = 0.0f;
//   if(q2 < 0.0f) q2 = 0.0f;
//   if(q3 < 0.0f) q3 = 0.0f;
//   q0 = sqrt(q0);
//   q1 = sqrt(q1);
//   q2 = sqrt(q2);
//   q3 = sqrt(q3);
//   if(q0 >= q1 && q0 >= q2 && q0 >= q3) {
//       q0 *= +1.0f;
//       q1 *= SIGN(r32 - r23);
//       q2 *= SIGN(r13 - r31);
//       q3 *= SIGN(r21 - r12);
//   } else if(q1 >= q0 && q1 >= q2 && q1 >= q3) {
//       q0 *= SIGN(r32 - r23);
//       q1 *= +1.0f;
//       q2 *= SIGN(r21 + r12);
//       q3 *= SIGN(r13 + r31);
//   } else if(q2 >= q0 && q2 >= q1 && q2 >= q3) {
//       q0 *= SIGN(r13 - r31);
//       q1 *= SIGN(r21 + r12);
//       q2 *= +1.0f;
//       q3 *= SIGN(r32 + r23);
//   } else if(q3 >= q0 && q3 >= q1 && q3 >= q2) {
//       q0 *= SIGN(r21 - r12);
//       q1 *= SIGN(r31 + r13);
//       q2 *= SIGN(r32 + r23);
//       q3 *= +1.0f;
//   } else {
//       printf("coding error\n");
//   }
//   float r = NORM(q0, q1, q2, q3);
//   q0 /= r;
//   q1 /= r;
//   q2 /= r;
//   q3 /= r;
//   
//   return float4(q0, q1, q2, q3);
//   }
//
//   enum Axis
//   {
//       Axis_X = 0, Axis_Y = 1, Axis_Z = 2
//   };
//
//   // RotateDirection describes the rotation direction around an axis, interpreted as follows:
//   //  CW  - Clockwise while looking "down" from positive axis towards the origin.
//   //  CCW - Counter-clockwise while looking from the positive axis towards the origin,
//   //        which is in the negative axis direction.
//   //  CCW is the default for the RHS coordinate system. Oculus standard RHS coordinate
//   //  system defines Y up, X right, and Z back (pointing out from the screen). In this
//   //  system Rotate_CCW around Z will specifies counter-clockwise rotation in XY plane.
//   enum RotateDirection
//   {
//       Rotate_CCW = 1,
//       Rotate_CW  = -1 
//   };
//   
//   // Constants for right handed and left handed coordinate systems
//   enum HandedSystem
//   {
//       Handed_R = 1, Handed_L = -1
//   };
//
////------------------------------------------------------------------------------------//
//// ***** double constants
//#define MATH_DOUBLE_PI              3.14159265358979323846
//#define MATH_DOUBLE_TWOPI           (2*MATH_DOUBLE_PI)
//#define MATH_DOUBLE_PIOVER2         (0.5*MATH_DOUBLE_PI)
//#define MATH_DOUBLE_PIOVER4         (0.25*MATH_DOUBLE_PI)
//#define MATH_FLOAT_MAXVALUE             (FLT_MAX) 
//
//#define MATH_DOUBLE_RADTODEGREEFACTOR (360.0 / MATH_DOUBLE_TWOPI)
//#define MATH_DOUBLE_DEGREETORADFACTOR (MATH_DOUBLE_TWOPI / 360.0)
//
//#define MATH_DOUBLE_E               2.71828182845904523536
//#define MATH_DOUBLE_LOG2E           1.44269504088896340736
//#define MATH_DOUBLE_LOG10E          0.434294481903251827651
//#define MATH_DOUBLE_LN2             0.693147180559945309417
//#define MATH_DOUBLE_LN10            2.30258509299404568402
//
//#define MATH_DOUBLE_SQRT2           1.41421356237309504880
//#define MATH_DOUBLE_SQRT1_2         0.707106781186547524401
//
//#define MATH_DOUBLE_TOLERANCE		  1e-12	    // a default number for value equality tolerance: about 4096*Epsilon;
//#define MATH_DOUBLE_SINGULARITYRADIUS 1e-12		// about 1-cos(.0001 degree), for gimbal lock numerical problems    
//
//
//// ***** float constants
//#define MATH_FLOAT_PI		        float(MATH_DOUBLE_PI)
//#define MATH_FLOAT_TWOPI	        float(MATH_DOUBLE_TWOPI)
//#define MATH_FLOAT_PIOVER2          float(MATH_DOUBLE_PIOVER2)
//#define MATH_FLOAT_PIOVER4          float(MATH_DOUBLE_PIOVER4)
//
//#define MATH_FLOAT_RADTODEGREEFACTOR float(MATH_DOUBLE_RADTODEGREEFACTOR)
//#define MATH_FLOAT_DEGREETORADFACTOR float(MATH_DOUBLE_DEGREETORADFACTOR)
//
//#define MATH_FLOAT_E                float(MATH_DOUBLE_E)
//#define MATH_FLOAT_LOG2E            float(MATH_DOUBLE_LOG2E)
//#define MATH_FLOAT_LOG10E           float(MATH_DOUBLE_LOG10E)
//#define MATH_FLOAT_LN2              float(MATH_DOUBLE_LN2)
//#define MATH_FLOAT_LN10             float(MATH_DOUBLE_LN10)
//
//#define MATH_FLOAT_SQRT2            float(MATH_DOUBLE_SQRT2)
//#define MATH_FLOAT_SQRT1_2          float(MATH_DOUBLE_SQRT1_2)
//
//#define MATH_FLOAT_TOLERANCE		 1e-5f	// a default number for value equality tolerance: 1e-5, about 64*EPSILON;
//#define MATH_FLOAT_SINGULARITYRADIUS 1e-7f	// about 1-cos(.025 degree), for gimbal lock numerical problems  
//
//   // GetEulerAngles extracts Euler angles from the quaternion, in the specified order of
//   // axis rotations and the specified coordinate system. Right-handed coordinate system
//   // is the default, with CCW rotations while looking in the negative axis direction.
//   // Here a,b,c, are the Yaw/Pitch/Roll angles to be returned.
//   // rotation a around axis A1
//   // is followed by rotation b around axis A2
//   // is followed by rotation c around axis A3
//   // rotations are CCW or CW (D) in LH or RH coordinate system (S)
//   template <Axis A1, Axis A2, Axis A3, RotateDirection D, HandedSystem S>
//   void GetEulerAngles(float4 quat, float *a, float *b, float *c)
//   {
//       float Q[3] = { quat.x, quat.y, quat.z };  //Quaternion components x,y,z
//   
//       float ww  = quat.w*quat.w;
//       float Q11 = Q[A1]*Q[A1];
//       float Q22 = Q[A2]*Q[A2];
//       float Q33 = Q[A3]*Q[A3];
//   
//       float psign = float(-1.0);
//       // Determine whether even permutation
//       if (((A1 + 1) % 3 == A2) && ((A2 + 1) % 3 == A3))
//           psign = float(1.0);
//       
//       float s2 = psign * float(2.0) * (psign*quat.w*Q[A2] + Q[A1]*Q[A3]);
//   
//       if (s2 < (float)-1.0 + MATH_FLOAT_SINGULARITYRADIUS)
//       { // South pole singularity
//           *a = float(0.0);
//           *b = -S*D*MATH_FLOAT_PIOVER2;
//           *c = S*D*atan2((float)2.0*(psign*Q[A1]*Q[A2] + quat.w*Q[A3]),
//                                  ww + Q22 - Q11 - Q33 );
//       }
//       else if (s2 > (float)1.0 - MATH_FLOAT_SINGULARITYRADIUS)
//       {  // North pole singularity
//           *a = (float)0.0;
//           *b = S*D*MATH_FLOAT_PIOVER2;
//           *c = S*D*atan2((float)2.0*(psign*Q[A1]*Q[A2] + quat.w*Q[A3]),
//                                  ww + Q22 - Q11 - Q33);
//       }
//       else
//       {
//           *a = -S*D*atan2((float)-2.0*(quat.w*Q[A1] - psign*Q[A2]*Q[A3]),
//                                   ww + Q33 - Q11 - Q22);
//           *b = S*D*asin(s2);
//           *c = S*D*atan2((float)2.0*(quat.w*Q[A3] - psign*Q[A1]*Q[A2]),
//                                  ww + Q11 - Q22 - Q33);
//       }      
//       return;
//   }


   float3 ValveHMD::position(Eye eye) const
   {
   if (poseRender[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
      {
      const vr::HmdMatrix34_t& matPose = poseRender[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;

//   // Hint: Vive works on row-major matrices, we use column-major ones in OpenGL
//   float3 rotation;
//   OVR::Quat<float> quat( eyePose[(uint8)eye].Orientation );
//   quat.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&rotation.y, &rotation.x, &rotation.z);
//   return mul(float4x4::rotationY(degrees(rotation.y)), 
//          mul(float4x4::rotationX(-degrees(rotation.x)), 
//              float4x4::rotationZ(-degrees(rotation.z))));

      float4   position    = float4( matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 0.0f );
      float4x4 orientation = float4x4( matPose.m[0][0], matPose.m[0][1], -matPose.m[0][2], 0.0f,
                                       matPose.m[1][0], matPose.m[1][1], -matPose.m[1][2], 0.0f,
                                       matPose.m[2][0], matPose.m[2][1], -matPose.m[2][2], 0.0f,
                                       0.0f,            0.0f,            0.0f,            1.0f );
      return float4(position + mul(orientation, float4(eyeVector[static_cast<uint32>(eye)], 1.0f) )).xyz;
      }

   return float3(0.0f, 0.0f, 0.0f);
   }   

   float4x4 ValveHMD::orientation(Eye eye) const
   {
   if (poseRender[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
      {
      // HMD orientation is returned in OpenGL Right Handed Coordinate System 
      // where -Z is Forward and +X is Right. This orientation need to be converted 
      // to engine space where +Z is Forward and +X is Left.
      const vr::HmdMatrix34_t& matPose = poseRender[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;

      //float4 quat = quaternion(float3x3(matPose.m[0][0], matPose.m[0][1], -matPose.m[0][2],
      //                                  matPose.m[1][0], matPose.m[1][1], -matPose.m[1][2],
      //                                  matPose.m[2][0], matPose.m[2][1], -matPose.m[2][2]));
      //float3 rotation;
      //GetEulerAngles<Axis_X, Axis_Y, Axis_Z, Rotate_CCW, Handed_R>(quat, &rotation.x, &rotation.y, &rotation.z);
      //Log << " PITCH: " << degrees(rotation.x) << " TURN-YAW: " << degrees(rotation.y) << " ROLL: " << degrees(rotation.z) << endl;


      //return mul(float4x4::rotationY(-degrees(rotation.y)), 
      //           float4x4::rotationX(degrees(rotation.z)) );

      //return float4x4::rotationY(degrees(-rotation.y));

      //return mul(float4x4::rotationY(degrees(rotation.y)), 
      //       mul(float4x4::rotationX(-degrees(rotation.x)), 
      //           float4x4::rotationZ(-degrees(rotation.z))));

      float4x4 camOrientation = float4x4( matPose.m[0][0], matPose.m[0][1], -matPose.m[0][2], 0.0f,
                                          matPose.m[1][0], matPose.m[1][1], -matPose.m[1][2], 0.0f,
                                          matPose.m[2][0], matPose.m[2][1], -matPose.m[2][2], 0.0f,
                                          0.0f,            0.0f,             0.0f,            1.0f );

      // Turn by 180 degrees so that default HMD look matches other entities, which is +Z
//   float3 look = *reinterpret_cast<float3*>(&pRotation->m[8]);
//   float3 side = *reinterpret_cast<float3*>(&pRotation->m[0]);
//   float  fcos = cos(radians(180.0f));
//   float  fsin = sin(radians(180.0f));
//   *reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(look*fcos + side*fsin);  
//   *reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(side*fcos - look*fsin); 

      //float4x4 parentPivot;
      //float3 look = *reinterpret_cast<float3*>(&parentPivot.m[8]);
      //float3 side = *reinterpret_cast<float3*>(&parentPivot.m[0]);
      //float  fcos = cos(radians(180.0f));
      //float  fsin = sin(radians(180.0f));
      //*reinterpret_cast<float3*>(&parentPivot.m[8]) = normalize(look*fcos + side*fsin);  
      //*reinterpret_cast<float3*>(&parentPivot.m[0]) = normalize(side*fcos - look*fsin); 

      //return mul(camOrientation, parentPivot); //float4x4::rotationY(degrees(180.0)) 

      //float4 look = mul(camOrientation, float4(0.0f, 0.0f, -1.0f, 1.0f));
      //Log << look.x << " " << look.y << " " << look.z << " " << look.w << endl;
      //Log << -matPose.m[0][2] << " " << -matPose.m[1][2] << " " << -matPose.m[2][2] << endl;

      //return mul(float4x4::rotationY(degrees(180.0)), mul(camOrientation, float4x4::rotationY(degrees(180.0))) );

      return camOrientation;
      }

   return float4x4();
   }               

   float3 ValveHMD::vector(Eye eye) const
   {
   return eyeVector[static_cast<uint32>(eye)];
   }

   Ptr<Texture> ValveHMD::color(Eye eye) const
   {
   assert( enabled );
   
   uint32 index = sharedRT ? 0 : static_cast<uint32>(eye);
   return swap[index][ currentIndex ];
   }

   //Ptr<Texture> ValveHMD::framebuffer(void) const
   //{
   //assert( enabled );
   //
   //return mirror;
   //}

   void ValveHMD::endFrame(bool mirror)
   {
   // Do distortion rendering, Present and flush/sync

   // Draw distortion rendering directly to window (we don't need to mirror anything)
   if ( mirror &&
        window.width != 0 &&
        window.height != 0 )
      {
      Gpu.depth.test.off();
      Gpu.output.buffer.setDefaultWrite();
      Gpu.screen.view(uint32v4(0, 0, window.width, window.height));
      Gpu.scissor.rect(0, 0, window.width, window.height);
      for(uint32 eye=0; eye<2; ++eye)
         {
         // Set source texture
         uint32 index = sharedRT ? 0 : static_cast<uint32>(eye);
         distortion.texture(samplerLocation, swap[index][currentIndex]);
      
         // Draw to mirror
         distortion.draw(model->mesh[eye].geometry.buffer,
                         model->mesh[eye].elements.buffer,
                         model->mesh[eye].elements.type);
         }
      }

   // Specify viewports 
   vr::VRTextureBounds_t bounds[2];
   for(uint32 eye=0; eye<2; ++eye)
      {
      bounds[eye].uMin = 0.0f;
      bounds[eye].uMax = 1.0f;
      bounds[eye].vMin = 0.0f;
      bounds[eye].vMax = 1.0f;
      }
   if (sharedRT)
      {
      bounds[0].uMax = 0.5f;
      bounds[1].uMin = 0.5f;
      }

   // Specify source textures
   uint32 texId[2];
   for(uint32 eye=0; eye<2; ++eye)
      {
      uint32 index = sharedRT ? 0 : static_cast<uint32>(eye);
   
      // >>>>> OpenGL Specific code section - START
      Ptr<en::gpu::TextureGL> tex = ptr_dynamic_cast<en::gpu::TextureGL, en::gpu::Texture>(swap[index][currentIndex]);
      texId[eye] = tex->id;
      // >>>>> OpenGL Specific code section - END
      }

   // Submit to compositor
   if (compositor)
      {
      if (sharedRT)
         {
         compositor->Submit(vr::Eye_Left,  vr::API_OpenGL, (void*)texId[0], &bounds[0]);
         compositor->Submit(vr::Eye_Right, vr::API_OpenGL, (void*)texId[1], &bounds[1]);
         }
      else
         {
         compositor->Submit(vr::Eye_Left,  vr::API_OpenGL, (void*)texId[0], nullptr);
         compositor->Submit(vr::Eye_Right, vr::API_OpenGL, (void*)texId[1], nullptr);
         }
      }
   }

   //# CONTROLLER
   //##########################################################################

   ValveController::ValveController(vr::IVRSystem* vrContext, 
                                    vr::TrackedDevicePose_t* poseRender, 
                                    vr::VRControllerState_t* state, 
                                    Ptr<en::resource::Model> model, 
                                    Ptr<en::gpu::Texture> texture, 
                                    uint32 discoveredId) :
      context(vrContext),
      controllerId(discoveredId),
      poseState(poseRender),
      state(state),
      controllerModel(model),
      albedo(texture),
      trackpadId(-1),
      joystickId(-1),
      triggerId(-1)
   {
   // Find analog controllers axes (only one from each kind)
   for(uint32 i=0; i<5; ++i)
      {
      sint32 type = context->GetInt32TrackedDeviceProperty(controllerId, static_cast<vr::TrackedDeviceProperty>(vr::Prop_Axis0Type_Int32 + i));
      if (trackpadId == -1 && type == vr::k_eControllerAxis_TrackPad)
         trackpadId = i;
      if (joystickId == -1 && type == vr::k_eControllerAxis_Joystick)
         joystickId = i;
      if (triggerId == -1 && type == vr::k_eControllerAxis_Trigger)
         triggerId = i;
      }

   // Init Force Feeedback
   for(uint8 i=0; i<5; ++i)
      lastPulse[i].start();
   timeCooldown.miliseconds(5ULL);
   timeMaxDuration.miliseconds(65535ULL);


#ifdef EN_DEBUG

   #define k_unTrackingStringSize 256
   char text[k_unTrackingStringSize];

   Log << "   Controller " << controllerId << " properties: \n\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_TrackingSystemName_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_TrackingSystemName_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_ModelNumber_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_ModelNumber_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_SerialNumber_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_SerialNumber_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_RenderModelName_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_RenderModelName_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_ManufacturerName_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_ManufacturerName_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_TrackingFirmwareVersion_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_TrackingFirmwareVersion_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_HardwareRevision_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_HardwareRevision_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_AllWirelessDongleDescriptions_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_AllWirelessDongleDescriptions_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_ConnectedWirelessDongle_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_ConnectedWirelessDongle_String\" :" << text << "\n";

   if (context->GetStringTrackedDeviceProperty(controllerId, vr::Prop_AttachedDeviceId_String, text, k_unTrackingStringSize) > k_unTrackingStringSize)
      Log << "ERROR: Too long string!\n";
   Log << "   Property \"Prop_AttachedDeviceId_String\" :" << text << "\n";
#endif


   // Get names of controller buttons
   nameSystem   = string( context->GetButtonIdNameFromEnum(vr::k_EButton_System) );
   nameAppMenu  = string( context->GetButtonIdNameFromEnum(vr::k_EButton_ApplicationMenu) );
   nameGrip     = string( context->GetButtonIdNameFromEnum(vr::k_EButton_Grip) );
   nameTouchpad = string( context->GetButtonIdNameFromEnum(vr::k_EButton_SteamVR_Touchpad) );
   nameTrigger  = string( context->GetButtonIdNameFromEnum(vr::k_EButton_SteamVR_Trigger) );
   
   // Get names of controller axes
   nameAxisTrackpad = string( context->GetControllerAxisTypeNameFromEnum(vr::k_eControllerAxis_TrackPad) );
   nameAxisJoystick = string( context->GetControllerAxisTypeNameFromEnum(vr::k_eControllerAxis_Joystick) );
   nameAxisTrigger  = string( context->GetControllerAxisTypeNameFromEnum(vr::k_eControllerAxis_Trigger) );
   }

   ValveController::~ValveController()
   {
   }

   float3 ValveController::position(void) const
   {
   if (poseState->bPoseIsValid)
      {
      const vr::HmdMatrix34_t& matPose = poseState->mDeviceToAbsoluteTracking;
      return float3(matPose.m[0][3], matPose.m[1][3], matPose.m[2][3]);
      }

   return float3(0.0f, 0.0f, 0.0f);
   }

   float4x4 ValveController::orientation(void) const
   {
   if (poseState->bPoseIsValid)
      {
      // Orientation is returned in OpenGL Right Handed Coordinate System 
      // where -Z is Forward and +X is Right. This orientation need to be converted 
      // to engine space where +Z is Forward and +X is Left.
      const vr::HmdMatrix34_t& matPose = poseState->mDeviceToAbsoluteTracking;
      return float4x4( matPose.m[0][0], matPose.m[0][1], -matPose.m[0][2], 0.0f,
                       matPose.m[1][0], matPose.m[1][1], -matPose.m[1][2], 0.0f,
                       matPose.m[2][0], matPose.m[2][1], -matPose.m[2][2], 0.0f,
                       0.0f,            0.0f,             0.0f,            1.0f );
      }

   return float4x4();
   }

   Ptr<en::resource::Model> ValveController::model(void) const
   {
   return controllerModel;
   }

   Ptr<en::gpu::Texture> ValveController::texture(void) const
   {
   return albedo;
   }

   Time ValveController::cooldown(void) const
   {
   return timeCooldown;
   }

   Time ValveController::maxDuration(void) const
   {
   return timeMaxDuration;
   }

   bool ValveController::pulse(uint32 axis, Time duration)
   {
   assert( axis < 5 );

   // Trigger only if not too long
   if (duration > timeMaxDuration)
      return false;

   // Trigger only if cooldown ended
   if (lastPulse[axis].elapsed().seconds() < 0.005)
      return false;
   
   context->TriggerHapticPulse( controllerId, axis, static_cast<uint16>(duration.miliseconds()) );
   lastPulse[axis].start();
   return true;
   }

   bool ValveController::pressed(const en::input::ControllerButton button) const
   {
   if (button == SystemButton)   return (state->ulButtonPressed & ButtonMaskFromId(vr::k_EButton_System)) > 0 ? true : false;
   if (button == AppMenuButton)  return (state->ulButtonPressed & ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) > 0 ? true : false;
   if (button == GripButton)     return (state->ulButtonPressed & ButtonMaskFromId(vr::k_EButton_Grip)) > 0 ? true : false;
   if (button == TouchpadButton) return (state->ulButtonPressed & ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) > 0 ? true : false;
   if (button == FireButton)     return (state->ulButtonPressed & ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) > 0 ? true : false;
   return false;
   }

   bool ValveController::touched(const en::input::ControllerButton button) const
   {
   if (button == SystemButton)   return (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_System)) > 0 ? true : false;
   if (button == AppMenuButton)  return (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) > 0 ? true : false;
   if (button == GripButton)     return (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_Grip)) > 0 ? true : false;
   if (button == TouchpadButton) return (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) > 0 ? true : false;
   if (button == FireButton)     return (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) > 0 ? true : false;
   return false;
   }

   // Ranges from -1.0 to 1.0.
   float2 ValveController::touchpadPosition(void) const
   {
   // If Touchpad is not supported, its position is default
   if (trackpadId == -1)
      return float2();

   return float2( state->rAxis[trackpadId].x, state->rAxis[trackpadId].y );
   }

   // Ranges from -1.0 to 1.0.
   float2 ValveController::joystickPosition(void) const
   {
   // If Joystick is not supported, its position is default
   if (joystickId == -1)
      return float2();

   return float2( state->rAxis[joystickId].x, state->rAxis[joystickId].y );
   }

   // Ranges from 0.0 to 1.0 where 0.0 is fully released.
   float ValveController::triggerPosition(void) const
   {
   // If Trigger is not supported, its not pressed
   if (triggerId == -1)
      return 0.0f;

   return state->rAxis[triggerId].x;
   }









// menu button - no signal
// Red - 2
// fire - 8589934592
// analog - 4294967296
// squeez - 4
// 
// 
// 5 axes for analog inputs:
// x,y -1 .. 1 for track pads
// x 0..1 for triggers 0 -full release 1- full press
// 
// 	k_eControllerAxis_TrackPad = 1,
// 	k_eControllerAxis_Joystick = 2,
// 	k_eControllerAxis_Trigger = 3, // Analog trigger data is in the X axis



         //  /** Fills the supplied struct with the current state of the controller. Returns false if the controller index
         //  * is invalid. */
         //  virtual bool GetControllerState( vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState ) = 0;
         //  
         //  /** fills the supplied struct with the current state of the controller and the provided pose with the pose of 
         //  * the controller when the controller state was updated most recently. Use this form if you need a precise controller
         //  * pose as input to your application when the user presses or releases a button. */
         //  virtual bool GetControllerStateWithPose( TrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, TrackedDevicePose_t *pTrackedDevicePose ) = 0;


   //# CONTEXT
   //##########################################################################

   void Context::HMD::init(void)
   {
   // Get all detected OpenVR Head Mounted Displays
   uint32 devices = vr::VR_IsHmdPresent() ? 1 : 0;
   for(uint32 i=0; i<devices; ++i)
      device.push_back(Ptr<input::HMD>(new ValveHMD(i)));
   }

   void Context::HMD::destroy(void)
   {
   device.clear();
   vr::VR_Shutdown();
   }

   }
}

#endif
#endif