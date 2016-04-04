/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#ifndef ENG_INPUT
#define ENG_INPUT

#include "core/defines.h"
#include "core/types.h"

#include "input/hmd.h"

//#include "scene/state.h"
#include <vector>
using namespace std;

namespace en
{
   namespace input
   {
   enum Key
        {
        Key_Unknown          = 0,
        Key_A                   , // Letters
        Key_B                   ,
        Key_C                   ,
        Key_D                   ,
        Key_E                   ,
        Key_F                   ,
        Key_G                   ,
        Key_H                   ,
        Key_I                   ,
        Key_J                   ,
        Key_K                   ,
        Key_L                   ,
        Key_M                   ,
        Key_N                   ,
        Key_O                   ,
        Key_P                   ,
        Key_Q                   ,
        Key_R                   ,
        Key_S                   ,
        Key_T                   ,
        Key_U                   ,
        Key_V                   ,
        Key_W                   ,
        Key_X                   ,
        Key_Y                   ,
        Key_Z                   ,
        Key_1                   , // Cyphers
        Key_2                   ,
        Key_3                   ,
        Key_4                   ,
        Key_5                   ,
        Key_6                   ,
        Key_7                   ,
        Key_8                   ,
        Key_9                   ,
        Key_0                   ,
        Key_Up                  , // Arrows
        Key_Down                ,
        Key_Left                ,
        Key_Right               ,
        Key_Alt                 , // Controlers
        Key_LeftAlt             ,
        Key_RightAlt            ,
        Key_Ctrl                ,
        Key_LeftCtrl            ,
        Key_RightCtrl           ,
        Key_Shift               , 
        Key_LeftShift           ,
        Key_RightShift          ,
        Key_Esc                 , // Main keys
        Key_Tab                 ,
        Key_CapsLock            ,
        Key_Backspace           ,
        Key_Enter               ,
        Key_Space               ,
        Key_Tilde               ,
        Key_Minus               ,
        Key_Equal               ,
        Key_Baskslash           ,
        Key_OpenBrace           ,
        Key_CloseBrace          ,
        Key_Colon               ,
        Key_Quote               ,
        Key_Comma               ,
        Key_Period              ,
        Key_Slash               ,  
        Key_F1                  , // Function keys
        Key_F2                  ,
        Key_F3                  ,
        Key_F4                  ,
        Key_F5                  ,
        Key_F6                  ,
        Key_F7                  ,
        Key_F8                  ,
        Key_F9                  ,
        Key_F10                 ,
        Key_F11                 ,
        Key_F12                 ,   
        Key_Insert              , // Text control 
        Key_Delete              ,
        Key_Home                ,
        Key_End                 ,
        Key_PageUp              ,
        Key_PageDown            ,
        Key_NumLock             , // Numepad 
        Key_NumPad1             ,
        Key_NumPad2             ,
        Key_NumPad3             ,
        Key_NumPad4             ,
        Key_NumPad5             ,
        Key_NumPad6             ,
        Key_NumPad7             ,
        Key_NumPad8             ,
        Key_NumPad9             ,
        Key_NumPad0             ,
        Key_PrtScr              , // Additional
        Key_Pause               ,
        Key_ScrollLock          ,
        KeysCount
        };

   enum KeyState
        {
        Released             = 0,
        Pressed
        };

   enum MouseButton
        {
        LeftMouseButton      = 0,
        RightMouseButton        ,
        MiddleMouseButton       ,
        MouseButtonsCount    
        };

   enum JoystickAxis
        {
        JoystickAxisX        = 0,
        JoystickAxisY           ,
        JoystickAxisZ           ,
        JoystickAxisCount
        };

   enum CameraType
        {
        Default              = 0,
        CreativeSenz3D          ,
        CreativeRealSense       ,
        KinectForXbox360        ,
        //KinectForWindows        ,
        //KinectForXboxOne        ,
        //KinectForWindowsV2      ,
        CameraTypesCount
        };

   enum CameraStream
        {
        Color                = 0,
        Depth                   ,
        IR                      ,
        IntensityLeft           ,
        IntensityRight          ,
        Skeletons               ,
        CameraStreamsCount
        };

   enum CameraState
        {
        Disconnected         = 0,
        Initializing            ,
        NoBandwith              ,
        NoPower                 ,
        Ready                   ,
        Used                    ,
        CameraStateCount
        };

   enum KinectTrackingState
        {
        Unknown                  = 0,
        Predicted                   ,
        Tracked                 
        };

   enum EventType
        {
        None                     = 0,
        KeyPressed                  , // Keyboard    in: enum Key
        KeyReleased                 , //             in: enum Key
        MouseMoved                  , // Mouse       in: 
        MouseButtonPressed          , //             in: enum MouseButton
        MouseButtonReleased         , //             in: enum MouseButton
        JoystickPlugged             , // Joystick    in:
        JoystickUnplugged           , //             in:
        JoystickMoved               , //             in:
        JoystickButtonPressed       , //             in: uint8 buttonNumber
        JoystickButtonReleased      , //             in: uint8 buttonNumber
        ControllerActivated         , // VR Haptic Controller
        ControllerDeactivated       , //
        ControllerButtonPressed     , //
        ControllerButtonReleased    , //
        ControllerButtonTouched     , //
        ControllerButtonUntouched   , //
        VRInputFocusAvailable       , // Controllers are available for capturing their input (other application released them).
        VRInputFocusLost            , // Controllers input capture was taken from us (by other app or SteamVR).
        VRSceneFocusLost            , // VR Process Environment
        VRSceneFocusGained          , //
        VRSceneApplicationChanged   , //

        CameraColorData             , // Camera
        CameraDepthData             , //
        CameraInfraredData          , //
        CameraUVMapData             , //
        CameraSkeletonData          , //
        TouchscreenMoved            , // Touchscreen 
        TouchscreenTouched          , //             
        TouchscreenReleased         , //             
        AppClose                    , // System
        AppWindowHidden             , //
        AppWindowInBackground       , //
        AppWindowActive             , //
        InputEventsCount
        };

   // Body Skeleton description (20 bones for KinectForXbox360)
   struct Skeleton
          {
          float4*              bone;
          KinectTrackingState* boneState;
          float4               position;
          KinectTrackingState  state;
          };

   // Event is a 128 bit structure
   // ( -fpermissive under GCC and QCC )
   aligned(1)
   struct Event
          {
          EventType type; // 32 bits
          };
       
   struct KeyboardEvent : public Event
          {
          Key key;
          };
      
   struct MouseEvent : public Event
          {
          MouseButton button;
          uint16 x;
          uint16 y;
          };
      
   struct JoystickButtonEvent : public Event
          {
          uint8 id;
          uint8 button;
          };

   struct JoystickMoveEvent : public Event
          {
          uint8        id;
          JoystickAxis axis;
          float        position;
          };
      
   struct ControllerEvent : public Event
          {
          Ptr<Controller> pointer;
          };

   struct CameraEvent : public Event
          {
          uint32 id;
          uint32 size;
          void*  ptr;
          };
      
   struct SkeletonEvent : public Event
          {
          uint32 id;
          uint32 bodies;
          Skeleton* skeleton;
          };
      
   aligndefault
   CompileTimeAssert(sizeof(Event) == 4, Event_struct_has_wrong_size);
    
   // Event is a 128 bit structure
   // ( -fpermissive under GCC and QCC )
//   aligned(1)
//   struct Event
//          {
//          EventType type; // 32 bits
//
//          union 
//          {
//          struct Keyboard
//                 {
//                 Key key;            // 32 bits
//                 } keyboard;
//   
//          struct //Mouse
//                 {
//                 MouseButton button; // 32 bits
//                 uint16 x;           // 16 bits
//                 uint16 y;           // 16 bits
//                 } mouse;
//   
//          struct //Joystick
//                 {
//                 uint8 id;
//                 uint8 button;
//                 } joystick;
//   
//          struct //Camera
//                 {
//                 uint32 id;
//
//                 union 
//                 {
//                 struct //Data
//                        {
//                        uint8* ptr;
//                        uint32 size;
//                        } data;
//
//                 CameraSkeleton* skeleton; // 32 bits
//                 }; // Union
//
//                 } camera;
//          }; // Union
//
//          };
//   aligndefault
//   CompileTimeAssert(sizeof(Event) == 16, Event_struct_has_wrong_size);

       
       
       
       
       
       
       
   // Event callback function type
   typedef void (*EventHandlingFuncPtr)(Event& event);

   struct Touch
          {
          float2 position;  // Position with subpixel precision. 
                            // Returned in classic screenspace 
                            // coordinate system with point 0,0 
                            // in upper left corner and Y axis
                            // raising down.
          float  pressure;  // Pressure 0-lightest 1-normal
          float  size;      // Size of touch in pixels
          sint64 time;      // Time when touch occured
          };

   struct Gesture
          {
          vector<Touch> touches;
          };






   class Joystick : public SafeObject
         {
         public:
         virtual bool        on(void) = 0;                // Turns joystick on
         virtual bool        off(void) = 0;               // Turns joystick off
         virtual CameraState state(void) const = 0;       // Returns joystick state (on/off/initializing...)
         virtual bool        pressed(const uint8 button) const = 0; // Button state
         virtual float       position(const en::input::JoystickAxis axis) const = 0; // Joystick state
         virtual void        update(void) = 0;            // Update joystick events

         virtual ~Joystick() {};                          // Polymorphic deletes require a virtual base destructor
         };

   struct CameraInfo
          {
          CameraType type;             // Type of camera
          uint32     streams;          // Bitfield of supported streams (use CameraStream enum's as bit's positions)
          struct Color                 
                 {                     
                 uint32 width;         // Sensor width in pixels
                 uint32 height;        // Sensor height in pixels
                 uint32 fps;           // Sensor capture rate in Frames Per Second
                 float  hFOV;          // Horizontal Field Of View
                 float  vFOV;          // Vertical Field Of View
                 } color;
          struct Depth
                 {
                 uint32 width;         // Sensor width in pixels
                 uint32 height;        // Sensor height in pixels
                 uint32 fps;           // Sensor capture rate in Frames Per Second
                 float  hFOV;          // Horizontal Field Of View
                 float  vFOV;          // Vertical Field Of View
                 float  minDistance;   // Minimum depth sensor range
                 float  maxDistance;   // Maximum depth sensor range
                 float  scale;         // Scale for which depth need to be multiplied, to acquire depth in meters
                 uint16 saturate;
                 uint16 lowConfidence;
                 } depth;
          struct Skeleton
                 {
                 uint32 bodies;        // Maximum number of tracked bodies
                 uint32 bones;         // Maximum number of bones per skeleton
                 } skeleton;
          };

   struct StreamSettings
          {
          gpu::Format format;
          uint32 width;
          uint32 height;
          uint32 hz;
          };

   class Camera : public SafeObject
         {
         public:
         virtual bool        on(void) = 0;                // Turns camera on
         virtual bool        off(void) = 0;               // Turns camera off
         virtual CameraState state(void) const = 0;       // Returns camera state (on/off/initializing...)
         virtual CameraType  type(void) const = 0;        // Returns camera type
         virtual bool        support(CameraStream type) const = 0; // Returns true if camera supports given stream type
         virtual CameraInfo  info(void) const = 0;        // Returns all camera properties
         virtual void        update(void) = 0;            // Update camera stream events

         virtual ~Camera() = 0;                           // Polymorphic deletes require a virtual base destructor
         };




   struct Interface
          {
          // Keyboard - text input device
          struct Keyboard
                 {
                 bool on(void);        // Turns sensor on
                 bool off(void);       // Turns sensor off
                 bool state(void);     // Returns actual sensor state (on/off)
                 bool available(void); // Returns if sensor is available in HW
                 bool pressed(const en::input::Key key);
                 } keyboard;

          // Mouse - pointing device
          struct Mouse
                 {
                 bool   on(void);        // Turns sensor on
                 bool   off(void);       // Turns sensor off
                 bool   state(void);     // Returns actual sensor state (on/off)
                 bool   available(void); // Returns if sensor is available in HW
                 float2 position(void);  // Mouse position
                 uint32 position(const en::input::Axis axis);
                 bool   position(const uint32 x, const uint32 y);
                 bool   pressed(const en::input::MouseButton button);
                 void   show(void);      // Show cursor
                 void   hide(void);      // Hide cursor
                 } mouse;

          // Joystick - direction
          struct Joystick
                 {
                 uint8           available(void) const;       // Returns count of available joysticks's
                 Ptr<input::Joystick> get(uint8 index = 0) const;  // Get pointer to N'th available joystick interface
                 } joystick;

          //struct Joystick
          //       {
          //       bool   on(const uint8 id = 0);        // Turns sensor on
          //       bool   off(const uint8 id = 0);       // Turns sensor off
          //       bool   state(const uint8 id = 0);     // Returns actual sensor state (on/off)
          //       bool   available(const uint8 id = 0); // Returns if sensor is available in HW
          //       uint8  count(void);                   // Returns joysticks count

          //       bool   pressed(const uint8 button);
          //       bool   pressed(const uint8 joystick, const uint8 button);
          //       float  position(const en::input::JoystickAxis axis);
          //       float  position(const uint8 joystick, const en::input::JoystickAxis axis);
          //       } joystick;

          // Touchscreen - touches and gestures
          struct Touchscreen
                 {
                 bool on(void);              // Turns sensor on
                 bool off(void);             // Turns sensor off
                 bool state(void);           // Returns actual sensor state (on/off)
                 bool available(void);       // Returns if sensor is available in HW

                 uint8   gestures(void);                       // Returns actual number of gestures
                 sint32  gestureId(uint8 gesture);             // Returns gesture id
                 Gesture gesture(uint8 gesture);               // Returns N'th gesture from the queue of active ones
                 Gesture gesture(sint32 gestureId);            // Returns gesture by its id
                 bool    gesturePresent(sint32 gestureId);     // Returns true if gesture with given id is still active

                 bool    touched(float4 rect);                 // Returns true if screen is touched in mentioned rectangle by any gesture
                 } touchscreen;

          // Acceleration sensor - device movement acceleration
          struct Accelerometer
                 {
                 bool on(void);              // Turns sensor on
                 bool on(const float noise); // Turns sensor on and sets it noise range
                 bool off(void);             // Turns sensor off
                 bool state(void);           // Returns actual sensor state (on/off)
                 bool available(void);       // Returns if sensor is available in HW

                 void   noise(const float noise); // Sets sensors noise range
                 float3 deltas(void);             // Delta of last two sensor readings - m/s^2
                 float3 vector(void);             // Acceleration in three axes - m/s^2
                 } accelerometer;

          // Magnetic field sensor - orientation on globe
          struct Compass
                 {
                 bool on(void);        // Turns sensor on
                 bool off(void);       // Turns sensor off
                 bool state(void);     // Returns actual sensor state (on/off)
                 bool available(void); // Returns if sensor is available in HW
                 } compass;

          // Gyroscope sensor - device orientation in space
          struct Gyroscope
                 {
                 bool on(void);                   // Turns sensor on
                 bool on(const float noise);      // Turns sensor on and sets it noise range
                 bool off(void);                  // Turns sensor off
                 bool state(void);                // Returns actual sensor state (on/off)
                 bool available(void);            // Returns if sensor is available in HW

                 void   noise(const float noise); // Sets sensors noise range
                 float  azimuth(void);  // TODO: Rename to YAW, what about axis orientation?
                 float  pitch(void);
                 float  roll(void);
                 } gyroscope;

          // Light sensor - light exposure
          struct Light
                 {
                 bool on(void);        // Turns sensor on
                 bool off(void);       // Turns sensor off
                 bool state(void);     // Returns actual sensor state (on/off)
                 bool available(void); // Returns if sensor is available in HW

                 float lux(void);      // Returns ambient light level in SI lux:
                                       // Max Sun light 120000.0
                                       // Sun light     110000.0
                                       // Shade         20000.0
                                       // Overcast      10000.0 (cloudy)
                                       // Sunrise       400.0
                                       // Cloudy        100
                                       // Full moon     0.25
                                       // No moon       0.001
                 } light;

          // Proximity sensor - distance from device
          struct Proximity
                 {
                 bool on(void);        // Turns sensor on
                 bool off(void);       // Turns sensor off
                 bool state(void);     // Returns actual sensor state (on/off)
                 bool available(void); // Returns if sensor is available in HW

                 float distance(void); // Distance in cm (or binary near/far)
                 } proximity;

          // Temperature sensor - ambient temperature or temperature of device
          struct Temperature
                 {
                 bool on(void);        // Turns sensor on
                 bool off(void);       // Turns sensor off
                 bool state(void);     // Returns actual sensor state (on/off)
                 bool available(void); // Returns if sensor is available in HW

                 float celcius(void);  // Temperature in celcius degrees
                 } temperature;

          // Camera - color, depth, ir and skeleton detection sensors
          struct Camera
                 {
                 uint8           available(void) const;       // Returns count of available camera's
                 Ptr<input::Camera> get(uint8 index = 0) const;  // Get pointer to N'th available camera interface
                 } camera;

          // Head Mounted Display
          struct HMD
                 {
                 uint8           available(void) const;       // Returns count of available HMD's
                 Ptr<input::HMD> get(uint8 index = 0) const;  // Get pointer to N'th available HMD interface
                 } hmd;

          // VR Haptic Controller
          struct Controller
                 {
                 uint8                  available(void) const;       // Returns count of available Controllers at given moment
                 Ptr<input::Controller> get(uint8 index = 0) const;  // Get pointer to N'th available Controller interface
                 };

          // Events handling function callbacks
          struct Events
                 {
                 void set(EventHandlingFuncPtr ptr); // Registers function that will handle events
                 void clear(void);                   // Unregisters function assigned to handling events.
                                                     // By default events are passed to State Manager
                 } events;

          void update(void); // Gets actual input state, call function handling cached events
          };
   }

extern input::Interface Input;
}

#endif


