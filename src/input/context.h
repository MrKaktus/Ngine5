/*

 Ngine v5.0
 
 Module      : Input context
 Visibility  : Engine internal code
 Requirements: none
 Description : Declaration of engine input context
               internal data for interaction with
               other modules.

*/

#ifndef ENG_CORE_INPUT_CONTEXT
#define ENG_CORE_INPUT_CONTEXT

#include "core/defines.h"
#include "core/types.h"
#include "input/input.h"

// Enable/Disable input devices support
#define OCULUS_VR                         1   // Compile with built in support for Oculus VR HMD's
#define OPEN_VR                           0   // Compile with built in support for OpenVR, SteamVR and HTC Vive HMD
#define INTEL_PERCEPTUAL_COMPUTING_2014   0   // Compile with built in support for Intel Depth Cameras
#define MICROSOFT_KINECT                  0   // Compile with built in support for Microsoft Kinect

#ifdef EN_PLATFORM_ANDROID
#include <android/input.h>
#include <android/sensor.h>
#endif

#ifdef EN_PLATFORM_WINDOWS

   #if INTEL_PERCEPTUAL_COMPUTING_2014
   #pragma comment(lib, "libpxc_d.lib")

   #include "pxcsensemanager.h"         // Creative RealSense
   #include "pxcprojection.h"           // For computation of UV coordinates of Depth samples on Color map
   
   // Additional Library Directories:   $(RSSDK_DIR)/lib/$(PlatformName)    
   // Additional Dependiences       :   libpxc_d.lib;
   
   // Old SDK 2013:
   
   //// Additional Include Directories:   $(PCSDK_DIR)/include; $(PCSDK_DIR)/sample/common/include; 
   //#include "pxcsession.h"              // Creative Senz3D
   //#include "pxccapture.h"
   //#include "util_capture.h"
   #endif

   #if MICROSOFT_KINECT
   #pragma comment(lib, "Kinect10.lib")
   #endif

   #define DIRECTINPUT_VERSION 0x0800
   #include <dinput.h>
#endif


namespace en
{
   namespace input
   {
#ifdef EN_PLATFORM_ANDROID
   // Add declarations of deprecated sensors
   #ifndef ASENSOR_TYPE_ORIENTATION
   #define ASENSOR_TYPE_ORIENTATION         3
   #endif
   #ifndef ASENSOR_TYPE_PRESSURE
   #define ASENSOR_TYPE_PRESSURE            6
   #endif
   #ifndef ASENSOR_TYPE_TEMPERATURE
   #define ASENSOR_TYPE_TEMPERATURE         7
   #endif
   #ifndef ASENSOR_TYPE_GRAVITY
   #define ASENSOR_TYPE_GRAVITY             9 
   #endif
   #ifndef ASENSOR_TYPE_LINEAR_ACCELERATION
   #define ASENSOR_TYPE_LINEAR_ACCELERATION 10
   #endif
   #ifndef ASENSOR_TYPE_ROTATION_VECTOR
   #define ASENSOR_TYPE_ROTATION_VECTOR     11
   #endif
   #ifndef ASENSOR_TYPE_RELATIVE_HUMIDITY
   #define ASENSOR_TYPE_RELATIVE_HUMIDITY   12
   #endif
   #ifndef ASENSOR_TYPE_AMBIENT_TEMPERATURE
   #define ASENSOR_TYPE_AMBIENT_TEMPERATURE 13
   #endif

   // Identifies messages in event queue.
   enum AndroidEventType
        {
        MainEvent  = 1,  // Main events
        InputEvent    ,  // Input events
        AppEvent         // Application events
        };

   /**
    * Data associated with an ALooper fd that will be returned as the "outData"
    * when that source has data ready.
    */
   struct android_poll_source 
          {
          // The identifier of this source.
          // May be LOOPER_ID_MAIN or LOOPER_ID_INPUT.
          int32_t id;
          
          // The android_app this ident is associated with.
          struct android_app* app;
          
          // Function to call to perform the standard processing 
          // of data from this source.
          void (*process)(struct android_app* app, struct android_poll_source* source);
          };
#endif

   // Input Context
   struct Context
          {
#ifdef EN_PLATFORM_ANDROID
          ALooper*            looper;
          AInputQueue*        inputQueue;
          ASensorEventQueue*  sensorQueue;
          ASensorManager*     sensorManager;
          sint32              fileDescriptor;
          android_poll_source inputData;
          bool                allowCallback;
#endif
#ifdef EN_PLATFORM_WINDOWS
          MSG msg;          // Message handle
#endif
          struct Keyboard
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
                 en::input::KeyState keys[KeysCount];  // States of keyboard keys
                 } keyboard;
   
          struct Mouse
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
                 en::input::KeyState buttons[MouseButtonsCount]; // States of mouse buttons
                 uint32 x;
                 uint32 y;
                 } mouse;

          struct Joystick
                 {
                 void init(void);
                 void destroy(void);

#ifdef EN_PLATFORM_WINDOWS
                 LPDIRECTINPUT8 context;   // DirectInput device context
#endif
                 vector< Ptr<input::Joystick> > device;
                 } joystick;

          struct Touchscreen
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on

                 sint32  generatorId; // Generates unique id's of objects
                 pair<Gesture,sint32> gesture[10]; // Up to 10 simultaneous gestures can be performed
                 pair<Gesture,sint32> history[64]; // Up to 64 finished gestures
                 uint8   gestures;    // Number currently performed gestures
                 uint8   head;        // Head of the round robin history queue
                 uint8   tail;        // Tail of the round robin history queue

#ifdef EN_PLATFORM_ANDROID
                 static void update(struct android_app* app, struct android_poll_source* source);
#endif
                 } touchscreen;

          struct Accelerometer
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
#ifdef EN_PLATFORM_ANDROID
                 const ASensor* sensor; // Sensor
#endif
                 float3 delta;          // Delata of last two acceleration vectors
                 float3 vector;         // Acceleration vector
                 float  noise;          // Sensor noise range
                 } accelerometer;

          struct Compass
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
#ifdef EN_PLATFORM_ANDROID
                 const ASensor* sensor; // Sensor
#endif
                 } compass;

          struct Gyroscope
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
#ifdef EN_PLATFORM_ANDROID
                 const ASensor* sensor; // Sensor
#endif 
                 float  noise;
                 float  azimuth; // yaw
                 float  pitch;
                 float  roll;
                 } gyroscope;

          struct Light
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
#ifdef EN_PLATFORM_ANDROID
                 const ASensor* sensor; // Sensor
#endif
                 float lux;       // Ambient light intensity
                 } light;

          struct Proximity
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
#ifdef EN_PLATFORM_ANDROID
                 const ASensor* sensor; // Sensor
#endif
                 float distance;  // Distance in cm (or binary near/far)
                 } proximity;

          struct Temperature
                 {
                 bool available;  // Is it available in the HW
                 bool used;       // Is it used by the application
                 bool on;         // Is it turned on
#ifdef EN_PLATFORM_ANDROID
                 const ASensor* sensor; // Sensor
#endif
                 float celcius;   // Temperature in celcius degrees
                 } temperature;

          struct Camera
                 {
                 void init(void);
                 void destroy(void);
#ifdef EN_PLATFORM_WINDOWS
#if INTEL_PERCEPTUAL_COMPUTING_2014
                 PXCSession* session;                 // Perceptual Computing Session
#endif
#endif
                 vector< Ptr<input::Camera> > device;
                 } camera;

          struct HMD
                 {
                 void init(void);
                 void destroy(void);

                 vector< Ptr<input::HMD> > device;
                 } hmd;

          struct Events
                 {
                 en::input::EventHandlingFuncPtr callback;
                 } events;

          Context();

          bool create(void);
          void destroy(void);
          };

#ifdef EN_PLATFORM_WINDOWS
   extern const Key winKeyToEnum[256];
#endif
   }

extern input::Context InputContext;
}

#endif


