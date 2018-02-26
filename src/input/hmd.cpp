/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#include "input/context.h"
#include "input/hmd.h"

namespace en
{
   namespace input
   {
   
   /*

   XrResult result;
   XrInstance instance;

   // Query array of HMD descriptors

   uint32 headsetCount = 0;
   result = xrEnumeratePhysicalDevices(instance, 0, &headsetCount, nullptr);

   XrSemanticPath* devices = new XrSemanticPath[headsetCount];

   uint32 headsetsReturned = 0;
   result = xrEnumeratePhysicalDevices(instance, 
                                       headsetCount, 
                                      &headsetsReturned, 
                                       devices);

   assert( headsetCount == headsetsReturned );

   // Select HMD to use

   XrSemanticPath selectedHmd = nullptr;
   for(uint32 i=0; i<headsetCount; ++i)
      {
      XrSemanticPath device = devices[i];

      XrPhysicalDeviceProperties deviceInfo;
      deviceInfo.type = XR_TYPE_DEVICE_PROPERTIES;
      deviceInfo.next = nullptr;
      //deviceInfo.vendorID;
      //deviceInfo.deviceID;
      //deviceInfo.locallyUniqueID;
      //deviceInfo.groupId;
      //deviceInfo.deviceName;

      result = xrGetPhysicalDeviceProperties(instance,
                                             device, 
                                            &deviceInfo);

      XrPhysicalDeviceGraphicsProperties graphicsInfo;
      graphicsInfo.type = XR_TYPE_PHYSICAL_DEVICE_GRAPHICS_PROPERTIES;
      graphicsInfo.next = nullptr;
      // graphicsInfo.minimumSwapchainBuffering; // minimum 1
      // graphicsInfo.maxSwapchainPixelsHigh;    // max Height
      // graphicsInfo.maxSwapchainPixelsWide;    // max Width
      // graphicsInfo.maxSwapchainMSAA;          // max Samples
      // graphicsInfo.recommendedSwapchainPixelsHigh; // Height  for samplingQuality == 1.0
      // graphicsInfo.recommendedSwapchainPixelsWide; // Width   for samplingQuality == 1.0
      // graphicsInfo.recommendedSwapchainMSAA;       // Samples for samplingQuality == 1.0
      // graphicsInfo.maxViewportCount;               // Max count of Frame Views
      // graphicsInfo.deviceType; // XrDisplayDeviceType 

      // TODO: XrDisplayDeviceType

      result = xrGetPhysicalDeviceGraphicsProperties(instance,
                                                     device,
                                                    &graphicsInfo);

      XrPhysicalDeviceTrackingProperties trackingInfo;
      trackingInfo.type = XR_TYPE_PHYSICAL_DEVICE_TRACKING_PROPERTIES;
      trackingInfo.next = nullptr;
      //trackingInfo.orientationTracking; // XrBool32 
      //trackingInfo.positionTracking;    // XrBool32 
      //trackingInfo.eyeTracking;         // XrBool32 

      result = xrGetPhysicalDeviceTrackingProperties(instance,
                                                     device,
                                                    &trackingInfo);

      uint32 inputCount = 0;
      result = xrGetPhysicalDeviceInputSources(instance, device, 0, &inputCount, nullptr);

      XrSemanticPath* inputs = new XrSemanticPath[inputCount];

      uint32 inputReturned = 0;
      result = xrGetPhysicalDeviceInputSources(instance, 
                                               device, 
                                               inputCount, 
                                              &inputReturned, 
                                               inputs);
      assert( inputCount == inputReturned );

      // TODO: Input properties

      // XR_KHR_vulkan_graphics_device_identification
      VkInstance vkInstance;
      VkPhysicalDevice vkDevice;

      result = xrGetVulkanGraphicsDeviceKHR(instance,
                                            device,
                                            vkInstance,
                                           &vkDevice);

      // XR_KHR_D3D_graphics_device_identification
      LUID d3d12gpu;
      result = xrGetD3DGraphicsDeviceKHR(instance,
                                         device,
                                        &d3d12gpu);

      printf( "Evaluating device %s\n", deviceProperties.deviceName );
      if (deviceProperties.vendorID == 0x1234 && 
          deviceProperties.deviceID == 0x4567)
         {
         // This is the device we want
         selectedHmd = device;
         break;
         }
      }

   delete [] devices;

   // Create System object per HMD

   XrSystemCreateInfo systemDesc;
   systemDesc.type                   = XR_TYPE_SYSTEM_CREATE_INFO;
   systemDesc.next                   = nullptr;
   systemDesc.flags                  = 0;
   systemDesc.physicalDisplayDevice  = selectedHmd;  // nullptr - means defaultt device    // ask the runtime for the default
   systemDesc.graphicsApi            = XR_GRAPHICS_API_VULKAN_1_1;  // XrGraphicsAPI
   systemDesc.graphicsBinding.vulkan = ...; // XrGraphicsBinding
   systemDesc.enabledExtensionCount  = 0; // no extensions
   systemDesc.enabledExtensionNames  = nullptr;



   XrSystem system;
   result = xrCreateSystem(instance, &systemDesc, &system);

   // create images
   // create session
   // main loop
   // destroy session

   result = xrDestroySystem(instance, system);


// Swap-Chain frame cycle:

   // Query supported Swap-Chain formats
   // Format::RGBA_8 and Format::RGBA_8_sRGB are always supported
   uint32 formatsCount = 0;
   result = xrEnumerateSwapchainFormats(system, 0, &formatsCount, nullptr);

   uint32* rawFormat = new uint32[formatsCount];

   uint32 formatsReturned = 0;
   result = xrEnumerateSwapchainFormats(system, 
                                        formatsCount, 
                                       &formatsReturned, 
                                        rawFormat);
   assert( formatsCount == formatsReturned );

   // TODO: Based on used graphics API, use different translation function, to convert API specific format into gpu::Format.

   // TODO: App selects gpu::Format to use

   // TODO: Translate gpu::Format to API specifi format, based on used backing rendering API

   delete [] rawFormat;

   // Query amount of views that need to be rendered to
   uint32 viewsCount;
   result = xrGetEyeCount(system, &viewsCount);

   XrSwapchainCreateInfo desc;
   desc.type        = XR_TYPE_SWAPCHAIN_CREATE_INFO;
   desc.next        = nullptr;
   desc.flags       = ; // XrSwapchainCreateFlags
   desc.usage       = ; // XrSwapchainUsageFlags
   desc.format      = ; //int64_t VK_FORMAT_R8G8B8A8_SRGB
   desc.chainLength = 3; 
   desc.sampleCount = 1; // uint32_t 
   desc.width       = ;
   desc.height      = ;
   desc.faceCount   = 1;
   desc.layerCount  = 2;
   desc.mipCount    = 1;


   XrSwapchain swapChain;

   result = xrCreateSwapchain(system,
                             &desc,
                             &swapChain);

   uint32 textures = 0;

   // Query count of backing textures in a Swap-Chain (total for all frames)
   result = xrGetSwapchainImages(swapChain, 0, &textures, nullptr);

   XrSwapchainImage* texturesArray = new XrSwapchainImage[textures];

   uint32 texturesReturned = 0;
   result = xrGetSwapchainImages(swapChain, 
                                 textures, 
                                &texturesReturned,
                                 texturesArray);

   assert( textures == texturesReturned );

   // Acquire index of backin texture that can be used 
   // Order in which indexes are returned is undefined, 
   // so app cannot build frame in advance.
   uint32 textureId = 0;
   result = xrAcquireSwapchainImage(swapChain, &textureId);

   // Wait for texture pointed by textureId to be available 
   // (not read anymore by Compositor). Timeout is in nanoseconds
   uint64 timeout = 0; 
   result = xrWaitSwapchainImage(swapChain, timeout);

   // HERE: Render to surface
   // HERE: Wait untile GPU finishes it's workload
   //       Does it indicate hard CPU-GPU sync? Kinda makes sense.

   // Indicate that rendering to texture pointed by textureId is done.
   result = xrReleaseSwapchainImage(swapChain);

   result = xrDestroySwapchain(system,
                               swapChain);



// Swap-Chain device session:


   XrSessionCreateInfo sessionDesc;
   sessionDesc.type  = XR_TYPE_SESSION_CREATE_INFO;
   sessionDesc.next  = nullptr;
   sessionDesc.flags = XR_SESSION_CREATE_POSITIONAL_TRACKING_BIT;
   if (eyeTrackingSupported)
      sessionDesc.flags |= XR_SESSION_CREATE_EYE_TRACKING_BIT;

   // Session is lost, when focus/input is lost
   XrSession deviceSession;
   result = xrCreateSession(system,
                           &sessionDesc,
                           &deviceSession);


   result = xrDestroySession(system,
                             deviceSession);



// Head pose for frame:

XrHeadPoseProjection pose;
XrRigidTransformf viewTransform;
XrMatrix4x4f projectionMatrix;
XrVector3f gazeDirection;  // Vector where eye is looking
XrEye eye;  // May be Center for monoscopic displays?


// TODO: XrHeadPosePerEye ???

   XrHeadPose headPose;
   headPose.type = XR_TYPE_HEAD_POSE;
   headPose.next = nullptr;
XrTrackerPoseState poseState;
XrHeadPoseFlags flags;  // XR_HEAD_POSE_EYES_TRACKED_BIT - provides eye tracking
                        // XR_HEAD_POSE_POSITION_TRACKED_BIT - positional tracking provided (missing for AR devices?)
XrFov combinedViewFoV;
uint32_t projectionCapacityInput = 2; // Say how many viewports want to query. No more than: XrPhysicalDeviceGraphicsProperties::maxViewportCount
uint32_t projectionCountOutput; // Returned amount of viewport poses
XrHeadPoseProjection* projections; // Array of: Viewport pose info


   XrNanoseconds displayTime;

   xrGetDisplayTime(); // predicted display time


   // Returns the head/eye poses for a particular display time. 
   // Prediction changes constantly based on dT from now to requested displayTime.
   // Count of Views in HMD may change on each query. 
   // Is this designed for CAVE systems/CubeMap redering where some Views are not
   // seen based on direction the head is pointing at?
   result = xrGetHeadPose(session,
                          displayTime,
XrCoordinateSystem coordinateSystem,  // Pose returned for given coordinate system (like our space?). May be XR_NULL_HANDLE
                         &headPose);


xrGetTrackingBoundsKHR


// Timing:

   XrWaitFrameDescription waitDesc;
   waitDesc.type        = XR_TYPE_WAIT_FRAME_DESCRIPTION;
   waitDesc.next        = nullptr;
   waitDsc.swapInterval = 1; // Number of display refreshes this wait should consume (1+)
                             // Application may ask to drop frequency (render every second frame, with ASW)

   XrWaitFrameInfo waitInfo;
   waitInfo.type        = XR_TYPE_WAIT_FRAME_INFO;
   waitInfo.next        = nullptr;
   // waitInfo.predictedDisplayTime;   // XrNanoseconds - expected display time of next frame
   // waitInfo.predictedDisplayPeriod; // uint64 - expected display period 
                                       // App can ask for prediction for future frames.
                                       // This allows VRR? or just frame re-aligning?

   // Returns prediction for the middle of the time period when photons are emmited.
   // Not coupled with xrEndFrame, may be called on separate thread, but called once per frame.
   result = xrWaitFrame(session,
                       &waitDesc,
                       &waitInfo);


// Begin-End:

   // Currently unused. Declared for extensions.
   XrBeginFrameDescription beginDesc;
   beginDesc.type = XR_TYPE_BEGIN_FRAME_DESCRIPTION;
   beginDesc.next = nullptr;

   // Begin-End may be called on separate threads, but those calls must interleave.
   result = xrBeginFrame(session,
                         nullptr); // &begnDesc in the future





   // Is capabe of handling worst case scenation of CAVE.
   // Each frame only sub-set is presented.
   XrCompositorLayerMultiProjectionElement projections[6];

   uint32 projectionsCount = headPose.projectionCountOutput;
   for(uint32 i=0; i<projectionsCount; ++i)
      {
      projections[i].viewTransform    = headPose.projections[i].viewTransform;    // XrRigidTransformf
      projections[i].projectionMatrix = headPose.projections[i].projectionMatrix; // XrMatrix4x4f
      projections[i].swapchain        = swapChain; // Specifies for which SwapChain this view is presented.
                                                   // Allows to finish frame that renders for several different HMD's?
      projections[i].arrayLayer       = i; // Index in 2DArray
      projections[i].imageRect        = ;  // XrRect2Df - View mapping to area of backing texture
      } 

   // XR_KHR_extra_layer_info_depth
   XrExtraLayerInfoDepthKHR layerDepth;
   layerDepth.type                = XR_TYPE_EXTRA_LAYER_INFO_DEPTH_KHR;
   layerDepth.next                = nullptr;
   layerDepth.depthSwapchainCount = 1; // Count of swapChains
   layerDepth.depthSwapchains     = ;  // XrSwapchain*
   layerDepth.depthSwapchainRects = ;  // XrRect2Df* - Depth resolution may differ from color resolution!!
   layerDepth.minDepth = 0.01f;
   layerDepth.maxDepth = 1000.0f; // maxDepth >= minDepth so Reverse Z is not supported?

   XrCompositorLayerMultiProjection layer;
   // XrCompositorLayerHeader header:
   layer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   layer.header.next             = &layerDepth;
   layer.header.flags            = ; // XrCompositorLayerFlags
   layer.header.frameIndex       = ; // uint64_t 
   layer.header.displayTime      = ; // XrNanoseconds
   layer.header.synthesisCpuTime = ; // XrNanoseconds
   layer.header.synthesisGpuTime = ; // XrNanoseconds
   layer.header.eye              = ; // XrEye
   layer.header.colorScale       = ; // XrVector4f
   layer.header.coordinateSystem = ; //XrCoordinateSystem
   layer.projectionCount = projectionsCount;
   layer.projections     = &projections[0];

   // Extra overlays: Quad
   XrCompositorLayerQuad quadLayer;
   // XrCompositorLayerHeader header:
   quadLayer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   quadLayer.header.next             = &layerEx;
   quadLayer.header.flags            = ; // XrCompositorLayerFlags
   quadLayer.header.frameIndex       = ; // uint64_t 
   quadLayer.header.displayTime      = ; // XrNanoseconds
   quadLayer.header.synthesisCpuTime = ; // XrNanoseconds
   quadLayer.header.synthesisGpuTime = ; // XrNanoseconds
   quadLayer.header.eye              = ; // XrEye
   quadLayer.header.colorScale       = ; // XrVector4f
   quadLayer.header.coordinateSystem = ; //XrCoordinateSystem
   quadLayer.swapchain = swapChain;
   quadLayer.arrayLayer = ; //int32_t
   quadLayer.imageRect  = ; // XrRect2Df
   quadLayer.transform  = ; // XrRigidTransformf
   quadLayer.size       = ; // XrVector2f

   // XR_KHR_compositor_layer_cylinder

   // Extra overlays: Cylinder
   XrCompositorLayerCylinderKHR cylinderLayer;
   // XrCompositorLayerHeader header:
   cylinderLayer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   cylinderLayer.header.next             = &layerEx;
   cylinderLayer.header.flags            = ; // XrCompositorLayerFlags
   cylinderLayer.header.frameIndex       = ; // uint64_t 
   cylinderLayer.header.displayTime      = ; // XrNanoseconds
   cylinderLayer.header.synthesisCpuTime = ; // XrNanoseconds
   cylinderLayer.header.synthesisGpuTime = ; // XrNanoseconds
   cylinderLayer.header.eye              = ; // XrEye
   cylinderLayer.header.colorScale       = ; // XrVector4f
   cylinderLayer.header.coordinateSystem = ; //XrCoordinateSystem
   cylinderLayer.swapchain = swapChain;
   cylinderLayer.arrayLayer   = ; //int32_t
   cylinderLayer.imageRect    = ; // XrRect2Df
   cylinderLayer.transform    = ; // XrRigidTransformf
   cylinderLayer.radius       = ; // float
   cylinderLayer.centralAngle = ; // float
   cylinderLayer.aspectRatio  = ; // float

   // XR_KHR_compositor_layer_equirect

   // Extra overlays: Euirect
   XrCompositorLayerEquirectKHR equirectLayer;
   // XrCompositorLayerHeader header:
   equirectLayer.header.type             = XR_TYPE_COMPOSITOR_LAYER_HEADER;
   equirectLayer.header.next             = &layerEx;
   equirectLayer.header.flags            = ; // XrCompositorLayerFlags
   equirectLayer.header.frameIndex       = ; // uint64_t 
   equirectLayer.header.displayTime      = ; // XrNanoseconds
   equirectLayer.header.synthesisCpuTime = ; // XrNanoseconds
   equirectLayer.header.synthesisGpuTime = ; // XrNanoseconds
   equirectLayer.header.eye              = ; // XrEye
   equirectLayer.header.colorScale       = ; // XrVector4f
   equirectLayer.header.coordinateSystem = ; //XrCoordinateSystem
   equirectLayer.swapchain = swapChain;
   equirectLayer.arrayLayer   = ; //int32_t
   equirectLayer.imageRect    = ; // XrRect2Df
   equirectLayer.transform    = ; // XrRigidTransformf
   equirectLayer.offset       = ; // XrOffset2Df
   equirectLayer.scale        = ; // XrVector2f
   equirectLayer.bias         = ; // XrVector2f

   // XR_KHR_fixed_dual_projection_layer

   // This seems crippled :/
   XrFixedDualProjectionParamsKHR dualProjInfo;
   //dualProjInfo.viewTransform;         // XrRigidTransformf
   //dualProjInfo.viewTransforOuter;     // XrRigidTransformf
   //dualProjInfo.projectionMatrix;      // XrMatrix4x4f
   //dualProjInfo.projectionMatrixOuter; // XrMatrix4x4f

   result = xrQueryFixedDualProjectionParamsKHR(system,
                                               &dualProjInfo);



   // Has time for which frame was predicted
   XrEndFrameDescription endDesc; 
   endDesc.type        = XR_TYPE_END_FRAME_DESCRIPTION;
   endDesc.next        = nullptr;
   endDesc.flags       = 0; // XrEndFrameDescriptionFlags reserved for future.
   endDesc.displayTime = ;  // XrNanoseconds - time for which frame was rendered
   endDesc.layerCount  = 1;
   endDesc.layers      = &layer; // + quadLayer + cylinderLayer

   // Submits current set of layers, but also set of layers for future frames,
   // queued ahead is possible. Mostly for video decoding.
   result = xrEndFrame(session,
                      &endDesc);


*/




   }
}