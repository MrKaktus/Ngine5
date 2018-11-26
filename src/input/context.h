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


//   enum Key
//        {
//        Key_Unknown          = 0,
//        Key_A                   , // Letters
//        Key_B                   ,
//        Key_C                   ,
//        Key_D                   ,
//        Key_E                   ,
//        Key_F                   ,
//        Key_G                   ,
//        Key_H                   ,
//        Key_I                   ,
//        Key_J                   ,
//        Key_K                   ,
//        Key_L                   ,
//        Key_M                   ,
//        Key_N                   ,
//        Key_O                   ,
//        Key_P                   ,
//        Key_Q                   ,
//        Key_R                   ,
//        Key_S                   ,
//        Key_T                   ,
//        Key_U                   ,
//        Key_V                   ,
//        Key_W                   ,
//        Key_X                   ,
//        Key_Y                   ,
//        Key_Z                   ,
//        Key_1                   , // Cyphers
//        Key_2                   ,
//        Key_3                   ,
//        Key_4                   ,
//        Key_5                   ,
//        Key_6                   ,
//        Key_7                   ,
//        Key_8                   ,
//        Key_9                   ,
//        Key_0                   ,
//        Key_Up                  , // Arrows
//        Key_Down                ,
//        Key_Left                ,
//        Key_Right               ,
//        Key_Alt                 , // Controlers
//        Key_LeftAlt             ,
//        Key_RightAlt            ,
//        Key_Ctrl                ,
//        Key_LeftCtrl            ,
//        Key_RightCtrl           ,
//        Key_Shift               , 
//        Key_LeftShift           ,
//        Key_RightShift          ,
//        Key_Esc                 , // Main keys
//        Key_Tab                 ,
//        Key_CapsLock            ,
//        Key_Backspace           ,
//        Key_Enter               ,
//        Key_Space               ,
//        Key_Tilde               ,
//        Key_Minus               ,
//        Key_Equal               ,
//        Key_Baskslash           ,
//        Key_OpenBrace           ,
//        Key_CloseBrace          ,
//        Key_Colon               ,
//        Key_Quote               ,
//        Key_Comma               ,
//        Key_Period              ,
//        Key_Slash               ,  
//        Key_F1                  , // Function keys
//        Key_F2                  ,
//        Key_F3                  ,
//        Key_F4                  ,
//        Key_F5                  ,
//        Key_F6                  ,
//        Key_F7                  ,
//        Key_F8                  ,
//        Key_F9                  ,
//        Key_F10                 ,
//        Key_F11                 ,
//        Key_F12                 ,   
//        Key_Insert              , // Text control 
//        Key_Delete              ,
//        Key_Home                ,
//        Key_End                 ,
//        Key_PageUp              ,
//        Key_PageDown            ,
//        Key_NumLock             , // Numepad 
//        Key_NumPad1             ,
//        Key_NumPad2             ,
//        Key_NumPad3             ,
//        Key_NumPad4             ,
//        Key_NumPad5             ,
//        Key_NumPad6             ,
//        Key_NumPad7             ,
//        Key_NumPad8             ,
//        Key_NumPad9             ,
//        Key_NumPad0             ,
//        Key_PrtScr              , // Additional
//        Key_Pause               ,
//        Key_ScrollLock          ,
//        KeysCount
//        };
//
//namespace en
//{
//   namespace input
//   {
//   // Input Context
//   struct Context
//          {
//          struct Keyboard
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//                 en::input::KeyState keys[underlyingType(Key::KeysCount)];  // States of keyboard keys
//                 } keyboard;
//   
//          struct Mouse
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//                 en::input::KeyState buttons[underlyingType(MouseButton::Count)]; // States of mouse buttons
//                 uint32 x;
//                 uint32 y;
//                 } mouse;
//
//          struct Joystick
//                 {
//                 void init(void);
//                 void destroy(void);
//
//#ifdef EN_PLATFORM_WINDOWS
//                 LPDIRECTINPUT8 context;   // DirectInput device context
//#endif
//                 vector< std::shared_ptr<input::Joystick> > device;
//                 } joystick;
//
//          struct Touchscreen
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//
//                 sint32  generatorId; // Generates unique id's of objects
//                 pair<Gesture,sint32> gesture[10]; // Up to 10 simultaneous gestures can be performed
//                 pair<Gesture,sint32> history[64]; // Up to 64 finished gestures
//                 uint8   gestures;    // Number currently performed gestures
//                 uint8   head;        // Head of the round robin history queue
//                 uint8   tail;        // Tail of the round robin history queue
//
//#ifdef EN_PLATFORM_ANDROID
//                 static void update(struct android_app* app, struct android_poll_source* source);
//#endif
//                 } touchscreen;
//
//          struct Accelerometer
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//#ifdef EN_PLATFORM_ANDROID
//                 const ASensor* sensor; // Sensor
//#endif
//                 float3 delta;          // Delata of last two acceleration vectors
//                 float3 vector;         // Acceleration vector
//                 float  noise;          // Sensor noise range
//                 } accelerometer;
//
//          struct Compass
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//#ifdef EN_PLATFORM_ANDROID
//                 const ASensor* sensor; // Sensor
//#endif
//                 } compass;
//
//          struct Gyroscope
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//#ifdef EN_PLATFORM_ANDROID
//                 const ASensor* sensor; // Sensor
//#endif 
//                 float  noise;
//                 float  azimuth; // yaw
//                 float  pitch;
//                 float  roll;
//                 } gyroscope;
//
//          struct Light
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//#ifdef EN_PLATFORM_ANDROID
//                 const ASensor* sensor; // Sensor
//#endif
//                 float lux;       // Ambient light intensity
//                 } light;
//
//          struct Proximity
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//#ifdef EN_PLATFORM_ANDROID
//                 const ASensor* sensor; // Sensor
//#endif
//                 float distance;  // Distance in cm (or binary near/far)
//                 } proximity;
//
//          struct Temperature
//                 {
//                 bool available;  // Is it available in the HW
//                 bool used;       // Is it used by the application
//                 bool on;         // Is it turned on
//#ifdef EN_PLATFORM_ANDROID
//                 const ASensor* sensor; // Sensor
//#endif
//                 float celcius;   // Temperature in celcius degrees
//                 } temperature;
//
//          struct Camera
//                 {
//                 void init(void);
//                 void destroy(void);
//#ifdef EN_PLATFORM_WINDOWS
//#if INTEL_PERCEPTUAL_COMPUTING_2014
//                 PXCSession* session;                 // Perceptual Computing Session
//#endif
//#endif
//                 vector< std::shared_ptr<input::Camera> > device;
//                 } camera;
//
//          struct HMD
//                 {
//                 void init(void);
//                 void destroy(void);
//
//                 vector< std::shared_ptr<input::HMD> > device;
//                 } hmd;
//
//          struct Events
//                 {
//                 en::input::EventHandlingFuncPtr callback;
//                 } events;
//
//          Context();
//
//          bool create(void);
//          void destroy(void);
//          };
//
//#ifdef EN_PLATFORM_WINDOWS
//   extern const Key winKeyToEnum[256];
//#endif
//   }
//
//extern input::Context InputContext;
//}

#endif


