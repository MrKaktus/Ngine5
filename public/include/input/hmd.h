/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#ifndef ENG_INPUT_HMD
#define ENG_INPUT_HMD

#include "core/defines.h"
#include "core/types.h"
#include "resources/resources.h" // For Distortion Mesh generation
#include "utilities/timer.h"     // For Force Feedback timings of controllers

namespace en
{
   namespace input
   {
   enum HMDType
      {
      HMDUnknown               = 0,
      HMDOculusDK1                ,
      HMDOculusDKHD               ,
      HMDOculusDKCrystalCove      ,
      HMDOculusDK2                ,
      HMDOculusBlackStar          ,
      HMDOculusCrescentBay        ,
      HMDOculusE3_2015            ,
      HMDOculusES06               ,
      HMDOculusES09               ,
      HMDOculusRift               , // CV1
      HMDOculusUnknown            ,
      HMDVive                     ,
      HMDVivePre                  ,
      };

   enum HMDVendor
      {
      HMDVendorUnknown         = 0,
      HMDOculus                   ,
      HMDValve
      };

   enum HMDMode
      {
      Desktop                  = 0,
      Direct
      };

   enum Eye
      {
      LeftEye                  = 0,
      RightEye                    ,
      EyesCount
      };

   enum ControllerButton
      {
      SystemButton             = 0,
      AppMenuButton               ,
      GripButton                  ,
      TouchpadButton              ,
      FireButton                  
      };                       
                               
   enum Axis                   
      {                        
      AxisX                    = 0,
      AxisY                       ,
      AxisCount
      };

   enum DebugHUD
      {
      Off                      = 0,
      LatencyTiming               ,
      ApplicationRenderTiming     ,
      CompositorRenderTiming      , 
      Info
      };

//   // Per Eye Distortion Settings
//   struct DistortionSettings
//      {
//      uint32   width;            // In:  Rendertarget width
//      uint32   height;           // In:  Rendertarget height
//      uint32v4 viewport;         // In:  Viewport rectangle
//      float2   UVScaleOffset[2]; // Out: UV Scale and Offset vectors for distortion shader
//      };

   class HMD
      {
      public:
      virtual bool      on(uint32v2 windowResolution, 
                           bool sharedRendertarget = true) = 0;      // Turns HMD on, pass resolution of window you want to mirror image to (in future window handle or NULL)
      virtual bool      off(void) = 0;                               // Turns HMD off
      virtual bool      state(void) const = 0;                       // Returns HMD state (on/off)
      virtual HMDType   device(void) const = 0;                      // Returns HMD type
      virtual HMDVendor vendor(void) const = 0;                      // Returns HMD vendor
      
                                                                     // Rendering settings:
      virtual HMDMode   mode(void) const = 0;                        // Mode of binding HMD's display to rendering API
      virtual sint32    display(void) const = 0;                     // Display number in extended desktop mode
      virtual uint32v2  resolution(void) const = 0;                  // HMD Display physical resolution in pixels
      virtual uint32v2  renderTarget(void) const = 0;                // Proposed Render Target resolution
      virtual bool      sharedRendertarget(void) const = 0;          // Is HMD using one shared render target for both eyes?
      virtual float4    frustumTangents(Eye eye) const = 0;          // Tangents defining projection frustum for each eye
      virtual uint32v4  viewport(uint8 eye) const = 0;               // Get viewport for current eye
      virtual void      reset(void) = 0;                             // Reset HMD position and orientation tracking
      virtual float2    playAreaDimensions(void) = 0;                // Play area dimensions in X and Z axes (in meters)
      virtual bool      playAreaLocation(float3* corners) = 0;       // 4 locations of play area corners from starting point, in CW order, with forward vector in -Z direction
      virtual bool      debugHUD(DebugHUD mode) = 0;                 // Turns On/Off different Debug HUD's if supported by given HMD

      virtual void      startFrame(const uint32 frameIndex = 0) = 0; // Call at the beginning of game loop
      virtual void      update(void) = 0;                            // Update status of any peripherials / controllers assigned to this HMD [PRIVATE FUNCTION]
      virtual float3    position(void) const = 0;                    // HMD's position 
                                                                     
                                                                     // Rendering scene per eye:
      virtual shared_ptr<en::gpu::Texture> color(Eye eye = LeftEye) const = 0;  // Acquire rendering destination texture for given eye
      virtual float3    position(Eye eye) const = 0;                 // HMD's predicted position for dT when given eye is rendered
      virtual float4x4  orientation(Eye eye) const = 0;              // HMD's predicted orientation for dT when given eye is rendered
      virtual float3    vector(Eye eye) const = 0;                   // HMD's offset from it's position to given eye position
      
                                                                     // Rendering distored image to HMD display:
      virtual void      endFrame(bool mirror = true) = 0;            // Call at the end of frame rendering to display in HMD
                                                                     // If requested, content will be also blitted to current window
                                                                    
      virtual ~HMD() {};                                             // Polymorphic deletes require a virtual base destructor
      };

   class Controller
      {
      public:
      virtual float3   position(void) const = 0;                                    // Controller position
      virtual float4x4 orientation(void) const = 0;                                 // Controller orientation
      virtual shared_ptr<en::resource::Model> model(void) const = 0;                       // Model of controller that can be used to show it in VR
      virtual shared_ptr<en::gpu::Texture>    texture(void) const = 0;                     // Texture that can be used with controllers model
      virtual Time     cooldown(void) const = 0;                                    // Returns controller cooldown time for Force Feedback
      virtual Time     maxDuration(void) const = 0;                                 // Returns Force Feedback max duration time, or 0 if Force Feedback is not supported
      virtual bool     pulse(uint32 axis, Time duration) = 0;                       // Spawns Force Feedback pulse for given period of time on given controller Axis
      virtual bool     pressed(const en::input::ControllerButton button) const = 0; // Query if given button is actually pressed
      virtual bool     touched(const en::input::ControllerButton button) const = 0; // Query if given button is actually touched
      virtual float2   touchpadPosition(void) const = 0;                            // Touchpad position if present. [-1.0 .. 1.0] range in both Axes.
      virtual float2   joystickPosition(void) const = 0;                            // Joystick position if present. [-1.0 .. 1.0] range in both Axes.
      virtual float    triggerPosition(void) const = 0;                             // Trigger position where 0.0 is released to 1.0 maximally pressed.
                                                                    
      virtual ~Controller() {};                                                     // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
