#include "assert.h"

#include "core/xr/openxr/oxrInterface.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrHeadset.h"
#include "core/xr/openxr/d3d12/oxrdx12PresentationSession.h"
#include "core/xr/openxr/vulkan/oxrvkPresentationSession.h"

namespace en
{
namespace xr
{

oxrHeadset::oxrHeadset(oxrInterface& _session) :
    session(&_session),
    runtime(XR_NULL_SYSTEM_ID),
    gpu(nullptr),
    blendModesCount(0),
    blendModes(nullptr),
    viewConfigurationsCount(0),
    viewConfigurationTypes(nullptr),
    viewConfigurations(nullptr),
    supportsPresentationModeDirect(false),
	supportsPresentationModeMergedDigitally(false),
	supportsPresentationModeMergedOptically(false)
{

    // Init system (backing runtime)
    //////////////////////////////////

// TODO: Move this section to instance initialization

    XrSystemGetInfo desc;
    desc.type       = XR_TYPE_SYSTEM_GET_INFO;
    desc.next       = nullptr;
#if defined(EN_PLATFORM_ANDROID)
    // On Android it might be Headset or Phone. Detect which one.
    desc.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY; // XR_FORM_FACTOR_HANDHELD_DISPLAY
#endif
#if defined(EN_PLATFORM_WINDOWS)
    desc.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
#endif

    // System abstracts underlying Runtime.
    Validate( session, xrGetSystem(session->instance, &desc, &runtime) )

    // (!!! API DESIGN ISSUE !!!)
    // There is no support for multiple runtimes! Loader selects "primary runtime" 
    // based on some internal guideline (XML descriptor, etc.) and loads it. This 
    // is main limitation of OpenXR API. Applications are in fact tied to given 
    // Store-Runtime pair and there is no programmable way of selecting on which 
    // Headset given application wants to run. From one point of view it makes
    // sense because user is already wearing some headset, and launches application
    // from it, which causes reverse chainining:
    // User-> Headset -> Runtime -> Store (landing page)
    // Thus when application is spawned, it's already caused by specific runtime,
    // and thus application is directed back to it. If Stores would be separated
    // from runtime, the chaining would be as follows:
    // User-> Headset -> Runtime -> OS -> Store A -> App C
    //                              OS -> Store B -> App D
    //                              OS -> App E
    // Application should still get hint, user of which headset caused it to run.
    // Engine can detect that by using different backing API's (for e.g. OpenVR
    // and Oculus SDK, and through them, checking which headset is in use).

    // Query system properties (backing runtime) 
    //////////////////////////////////////////////

    Validate( session, xrGetSystemProperties(session->instance, runtime, &systemInfo) )
    assert( systemInfo.type == XR_TYPE_SYSTEM_PROPERTIES );
    assert( systemInfo.next == nullptr );
    assert( systemInfo.trackingProperties.orientationTracking == XR_TRUE );
    assert( systemInfo.trackingProperties.positionTracking    == XR_TRUE );

    // XrSystemId                    systemId;
    // uint32_t                      vendorId;
    // char                          systemName[XR_MAX_SYSTEM_NAME_SIZE];
    // XrSystemGraphicsProperties    graphicsProperties;
    // uint32_t    maxSwapchainImageHeight;
    // uint32_t    maxSwapchainImageWidth;
    // uint32_t    maxViewCount;
    // uint32_t    maxLayerCount;

    // Query supported blend modes (presentation modes)
    /////////////////////////////////////////////////////

	// Headset properties are exposed as Runtime properties (as there is no headset abstraction at all).
	// There is no headset plug & play detection, as API is designed around standalone model (like Oculus Quest).

    Validate( session, xrEnumerateEnvironmentBlendModes(session->instance, runtime, 0, &blendModesCount, nullptr) )
    if (blendModesCount)
    {
        blendModes = new XrEnvironmentBlendMode[blendModesCount];
        assert( blendModes );

        uint32 blendModesWritten = 0;    
        Validate( session, xrEnumerateEnvironmentBlendModes(session->instance, runtime, blendModesCount, &blendModesWritten, blendModes) )
        blendModesCount = blendModesWritten;

        for(uint32 i=0; i<blendModesCount; ++i)
        {
            if (blendModes[i] == XR_ENVIRONMENT_BLEND_MODE_OPAQUE)
            {
                supportsPresentationModeDirect = true;
            }
            else
            if (blendModes[i] == XR_ENVIRONMENT_BLEND_MODE_ADDITIVE)
            {
                supportsPresentationModeMergedOptically = true;
            }
            else
            if (blendModes[i] == XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND)
            {
                supportsPresentationModeMergedDigitally = true;
            }
        }
    }

    // Query supported view configurations (presentation descriptors)
    ///////////////////////////////////////////////////////////////////

    // View Configurations are semi-equivalent to different allowed Presentation Session descriptor combinations.
    // Given View Configuration is set during xrBeginSession which is equivalent of creating Presentation Session.

    // Acquire supported view configurations, before creating XrSession (presentation session)
    Validate( session, xrEnumerateViewConfigurations(session->instance, runtime, 0, &viewConfigurationsCount, nullptr) )
    if (viewConfigurationsCount)
    {
        // Acquire list of supported view configuration types
        viewConfigurationTypes = new XrViewConfigurationType[viewConfigurationsCount];

        uint32 viewConfigurationsWritten = 0;
        Validate( session, xrEnumerateViewConfigurations(session->instance, runtime, viewConfigurationsCount, &viewConfigurationsWritten, viewConfigurationTypes) )
        assert( viewConfigurationsCount == viewConfigurationsWritten );

        // Acquire list of supported view configuration properties
        viewConfigurations = new XrViewConfigurationProperties[viewConfigurationsCount];
        for(uint32 i=0; i<viewConfigurationsCount; ++i)
        {
            Validate( session, xrGetViewConfigurationProperties(session->instance, runtime, viewConfigurationTypes[i], &viewConfigurations[i]) )
        }
    }
}

oxrHeadset::~oxrHeadset()
{
    // TOOD: Destroy all dynamic allocations!

	if (viewConfigurationsCount)
	{
		delete[] viewConfigurations;
		delete[] viewConfigurationTypes;
	}

	if (blendModesCount)
	{
		delete[] blendModes;
	}
}

gpu::GpuDevice* oxrHeadset::device(void)
{
    return gpu;
}

/*
// TODO: Internal
bool oxrHeadset::supportsPresentationMode(const XRPresentationMode mode)
{
	if (mode == PresentationMode::Direct)
	{
	    return supportsPresentationModeDirect;
	}
	else
	if (mode == PresentationMode::MergedDigitally)
	{
	    return supportsPresentationModeMergedDigitally;
	}
	else
	if (mode == PresentationMode::MergedOptically)
	{
	    return supportsPresentationModeMergedOptically;
	}
	
	// Shouldn't reach this place
	assert( 0 );
	return false;
}
*/

PresentationSession* oxrHeadset::createPresentationSession(const PresentationSessionDescriptor* descriptor)
{
    assert( descriptor );

    gpu::QueueType  queueType  = gpu::QueueType::Universal;
    uint32          queueIndex = 0;



    // Select view configuration to use
    uint32 selectedIndex = viewConfigurationsCount;
    for(uint32 i=0; i<viewConfigurationsCount; ++i)
    {
        assert( viewConfigurations[i].viewConfigurationType == viewConfigurationTypes[i] );

        if (viewConfigurationTypes[i] == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) // PRIMARY_MONO represents phone that is Mono by default but after slide-in can be Stereo
        {
            selectedIndex = i;
            break;
        }
    }

    // Verify that view configuration was found
    if (selectedIndex == viewConfigurationsCount)
    {
        // TODO: Error! Presentation configuration is not supported
        return nullptr;
    }

    // What it is for? (indicates that application can change view FOV - why, when and how?)
    bool mutableFOV = viewConfigurations[selectedIndex].fovMutable;

    // Selected view configuration (Presentation Session Descriptor equivalent)
    XrViewConfigurationType viewConfigurationType = viewConfigurations[selectedIndex].viewConfigurationType;





    oxrPresentationSession* result = nullptr;

    // Presentation Session is created against specific graphics API
    en::gpu::RenderingAPI api = en::Graphics->type();
    if (api == en::gpu::RenderingAPI::Direct3D)
    {
        // Create D3D12 based Presentation Session object
        result = new oxrPresentationSessionD3D12(runtime, viewConfigurationType, *descriptor, gpu, queueType, queueIndex);
    }
    else
    if (api == en::gpu::RenderingAPI::Vulkan)
    {
        // Create Vulkan based Presentation Session object
        result = new oxrVkPresentationSession(runtime, viewConfigurationType, *descriptor, gpu, queueType, queueIndex);
    }

    // TODO: Log critical error
    return result;
}



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




// Begin-End:





*/





/*
xrDestroySwapchain(handle);

for (XrSpace visualizedSpace : m_visualizedSpaces) {
    xrDestroySpace(visualizedSpace);
}

if (m_appSpace != XR_NULL_HANDLE) {
    xrDestroySpace(m_appSpace);
}

if (m_session != XR_NULL_HANDLE) {
    xrDestroySession(m_session);
}

if (m_instance != XR_NULL_HANDLE) {
    xrDestroyInstance(m_instance);
*/

} // en::xr
} // en