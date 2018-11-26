/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_HMD_OPENVR
#define ENG_INPUT_HMD_OPENVR

#include "core/defines.h"

#if defined(EN_MODULE_OPENVR)

#include "input/input.h"
#include "input/hmd.h"
#include "./openvr-1.0.3/headers/openvr.h"

#include <map>

namespace en
{
   namespace input
   {
   class ValveController : public Controller
      {
      public:
      float3   position(void) const;                        // Controller position
      float4x4 orientation(void) const;                     // Controller orientation
      std::shared_ptr<en::resource::Model> model(void) const;
      std::shared_ptr<en::gpu::Texture>    texture(void) const;
      Time     cooldown(void) const;                        // Returns controller cooldown time for Force Feedback
      Time     maxDuration(void) const;                     // Returns Force Feedback max duration time, or 0 if Force Feedback is not supported
      bool     pulse(uint32 axis, Time duration);           // Spawns Force Feedback pulse for given period of time on given controller Axis
      bool     pressed(const en::input::ControllerButton button) const;
      bool     touched(const en::input::ControllerButton button) const;
      float2   touchpadPosition(void) const;
      float2   joystickPosition(void) const;
      float    triggerPosition(void) const;

      ValveController(vr::IVRSystem* vrContext, 
                      vr::TrackedDevicePose_t* poseRender, 
                      vr::VRControllerState_t* state, 
                      std::shared_ptr<en::resource::Model> model, 
                      std::shared_ptr<en::gpu::Texture> texture,
                      uint32 discoveredId);
     ~ValveController();

      private:
      vr::IVRSystem*           context;         // Device context
      vr::TrackedDeviceIndex_t controllerId;    // Controller Id
      vr::TrackedDevicePose_t* poseState;       // Pointer to HMD array of controller poses
      vr::VRControllerState_t* state;

      std::shared_ptr<en::resource::Model> controllerModel;
      std::shared_ptr<en::gpu::Texture> albedo;

      sint32 trackpadId;
      sint32 joystickId;
      sint32 triggerId;
      Timer  lastPulse[5];     // Time from last haptic pulse on controllers each axis. There is 5ms of cooldown.
      Time   timeCooldown;     // Force Feedback cool down time
      Time   timeMaxDuration;  // Max allowed Force Feedback pulse duration
      std::string nameSystem;       // Button names
      std::string nameAppMenu;
      std::string nameGrip;
      std::string nameTouchpad;
      std::string nameTrigger;
      std::string nameAxisTrackpad; // Axes names
      std::string nameAxisJoystick;
      std::string nameAxisTrigger;
      };

   class ValveHMD : public HMD
      {
      public:
      virtual bool      on(uint32v2 windowResolution,
                           bool sharedRendertarget = true);
      virtual bool      off(void);
      virtual bool      state(void) const;
      virtual HMDType   device(void) const;
      virtual HMDVendor vendor(void) const; 
      virtual HMDMode   mode(void) const;
      virtual sint32    display(void) const;
      virtual uint32v2  resolution(void) const;
      virtual uint32v2  renderTarget(void) const;
      virtual bool      sharedRendertarget(void) const;
      virtual float4    frustumTangents(Eye eye) const;
      virtual uint32v4  viewport(uint8 eye) const;
      virtual void      reset(void);
      virtual float2    playAreaDimensions(void);
      virtual bool      playAreaLocation(float3* corners);
      virtual bool      debugHUD(DebugHUD mode);

      virtual void      startFrame(const uint32 frameIndex = 0); // Acquires predicted head position for given frame
      virtual void      update(void);
      virtual float3    position(void) const;                    // Head position (when head tracking is supported)
      
      virtual std::shared_ptr<en::gpu::Texture> color(Eye eye = LeftEye) const;          // Returns current swap-chain texture that should be used for given eye                           
      virtual float3    position(Eye eye) const; 
      virtual float4x4  orientation(Eye eye) const;               
      virtual float3    vector(Eye eye) const;
      
      virtual void      endFrame(bool mirror = true);
      
      // Private API
      
      ValveHMD(uint8 index);
      virtual void distortionModel(void);
      virtual pair< std::shared_ptr<en::resource::Model>, std::shared_ptr<en::gpu::Texture> > controllerModel(const std::string name);
      virtual void activateController(vr::TrackedDeviceIndex_t deviceId);
      virtual ~ValveHMD();
      
      private:
      bool             enabled;           // Is it turned on
      bool             sharedRT;          // True if using shared RT, otherwise using two separate RT's per eye (for saving VRAM)
      HMDType          deviceType;        // Type of HMD 
      sint32           windowPositionX;   // Window position X on desktop in Extended Mode
      sint32           windowPositionY;   // Window position Y on desktop in Extended Mode
      uint32v2         displayResolution; // Window / display resolution
      uint32v2         resolutionRT;      // RenderTarget resolution
      float4           tangents[2];       // Frustum tangents per eye
      float3           eyeVector[2];      // Eye to HMD/head center vectors
      uint32v4         view[2];           // Viewport position and size per eye
      sint32           displayId;         // Display number on desktop
      HMDMode          displayMode;       // Desktop or Direct rendering
      
      vr::IVRSystem*       context;       // Device context
      vr::IVRRenderModels* renderModels;  // Resources interface
      vr::IVRCompositor*   compositor;    // Image compositor
      uint32               currentIndex;  // Swap used rendertarget textures in swap-chain
      std::shared_ptr<en::gpu::Texture> swap[2][2];   // Up to two sets, 2 textures each
#if defined(EN_MODULE_RENDERER_METAL)
      std::shared_ptr<SharedSurface>   surface[2][2]; // Shared surfaces
      uint32               opengl[2][2];  // OpenGL texture handles for VRCompositor
      OpenGLContext*       glContext;     // Dummy OpenGL context used to create OpenGL handles of Swap-Chain surfaces
#endif
      vr::IVRChaperone*    chaperone;
      float2               areaDimensions;
      float3               areaCorners[4];      

      // HMD and Controllers state
      vr::TrackedDevicePose_t poseRender[vr::k_unMaxTrackedDeviceCount];   // Device position and orientation for rendering (can be HMD, controllers, etc.)
      vr::VRControllerState_t controller[vr::k_unMaxTrackedDeviceCount];
     
      // Controlers handles
      std::shared_ptr<ValveController> handle[vr::k_unMaxTrackedDeviceCount];   // Pointers to detected controllers

      map<string, pair< std::shared_ptr<en::resource::Model>, std::shared_ptr<en::gpu::Texture> > > modelCache;

 
      // Resources for rendering to Window
      uint32v2 window;                    // Resolution of window to mirror to
      std::shared_ptr<en::resource::Model> model;     // Distortion lenses model
// TODO:
//      en::gpu::Program      distortion;   // Distortion rendering program
      uint32            samplerLocation;  // Sampler location
      };  

   void InitOpenVR(void); 
   void CloseOpenVR(void);  
   }
}
#endif

#endif
