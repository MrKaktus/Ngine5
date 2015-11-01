/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#include <string>
using namespace std;

#include "core/defines.h"
#include "core/log/log.h"
#include "utilities/strings.h"
#include "utilities/utilities.h"
#include "utilities/gpcpu/gpcpu.h"
#ifdef EN_PLATFORM_ANDROID
#include <android/native_activity.h>
#include <android/sensor.h>
#include "Ngine4/platform/context.h"
#endif
#ifdef EN_PLATFORM_BLACKBERRY
#include <bbads/bbads.h>
#include <bbads/event.h>
#include <bps/audiodevice.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/sensor.h>
#include <bps/event.h>
#include <bps/bps.h>
#endif
#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>                             
#include <objbase.h> 
#include "platform/windows/win_events.h"  // Window events
#endif
#include "audio/context.h"
#include "input/context.h"
#include "scene/context.h"
#include "core/rendering/context.h"
#include "monetization/context.h"

#include "assert.h"

extern void en::state::HandleEventByState(en::input::Event& event);

namespace en
{
   namespace input
   {
#ifdef EN_PLATFORM_WINDOWS
   const Key winKeyToEnum[256] = {
      Key_Unknown,    // 0x00 
      Key_Unknown,    // 0x01 
      Key_Unknown,    // 0x02 
      Key_Unknown,    // 0x03 
      Key_Unknown,    // 0x04 
      Key_Unknown,    // 0x05 
      Key_Unknown,    // 0x06 
      Key_Unknown,    // 0x07 
      Key_Backspace,  // 0x08   
      Key_Tab,        // 0x09 
      Key_Unknown,    // 0x0A 
      Key_Unknown,    // 0x0B 
      Key_Unknown,    // 0x0C 
      Key_Enter,      // 0x0D
      Key_Unknown,    // 0x0E 
      Key_Unknown,    // 0x0F 
      Key_Shift,      // 0x10 
      Key_Ctrl,       // 0x11              
      Key_Alt,        // 0x12 
      Key_Pause,      // 0x13 
      Key_CapsLock,   // 0x14 
      Key_Unknown,    // 0x15 
      Key_Unknown,    // 0x16 
      Key_Unknown,    // 0x17 
      Key_Unknown,    // 0x18 
      Key_Unknown,    // 0x19 
      Key_Unknown,    // 0x1A 
      Key_Esc,        // 0x1B
      Key_Unknown,    // 0x1C 
      Key_Unknown,    // 0x1D 
      Key_Unknown,    // 0x1E
      Key_Unknown,    // 0x1F
      Key_Space,      // 0x20
      Key_PageUp,     // 0x21              
      Key_PageDown,   // 0x22       
      Key_End,        // 0x23    
      Key_Home,       // 0x24    
      Key_Left,       // 0x25 
      Key_Up,         // 0x26       
      Key_Right,      // 0x27  
      Key_Down,       // 0x28 
      Key_Unknown,    // 0x29 
      Key_Unknown,    // 0x2A 
      Key_Unknown,    // 0x2B 
      Key_PrtScr,     // 0x2C
      Key_Insert,     // 0x2D            
      Key_Delete,     // 0x2E  
      Key_Unknown,    // 0x2F                     
      Key_0,          // 0x30         
      Key_1,          // 0x31                 
      Key_2,          // 0x32                 
      Key_3,          // 0x33                 
      Key_4,          // 0x34                 
      Key_5,          // 0x35                 
      Key_6,          // 0x36                 
      Key_7,          // 0x37                 
      Key_8,          // 0x38                 
      Key_9,          // 0x39
      Key_Unknown,    // 0x3A 
      Key_Unknown,    // 0x3B 
      Key_Unknown,    // 0x3C 
      Key_Unknown,    // 0x3D 
      Key_Unknown,    // 0x3E 
      Key_Unknown,    // 0x3F 
      Key_Unknown,    // 0x40                      
      Key_A,          // 0x41                 
      Key_B,          // 0x42                  
      Key_C,          // 0x43                  
      Key_D,          // 0x44                  
      Key_E,          // 0x45                  
      Key_F,          // 0x46                  
      Key_G,          // 0x47                  
      Key_H,          // 0x48                  
      Key_I,          // 0x49                  
      Key_J,          // 0x4A                  
      Key_K,          // 0x4B                  
      Key_L,          // 0x4C                  
      Key_M,          // 0x4D                  
      Key_N,          // 0x4E                  
      Key_O,          // 0x4F                  
      Key_P,          // 0x50                  
      Key_Q,          // 0x51                  
      Key_R,          // 0x52                  
      Key_S,          // 0x53                  
      Key_T,          // 0x54                  
      Key_U,          // 0x55                  
      Key_V,          // 0x56                  
      Key_W,          // 0x57                  
      Key_X,          // 0x58                  
      Key_Y,          // 0x59                  
      Key_Z,          // 0x5A    
      Key_Unknown,    // 0x5B 
      Key_Unknown,    // 0x5C 
      Key_Unknown,    // 0x5D 
      Key_Unknown,    // 0x5E 
      Key_Unknown,    // 0x5F 
      Key_NumPad0,    // 0x60                          
      Key_NumPad1,    // 0x61            
      Key_NumPad2,    // 0x62            
      Key_NumPad3,    // 0x63            
      Key_NumPad4,    // 0x64            
      Key_NumPad5,    // 0x65            
      Key_NumPad6,    // 0x66            
      Key_NumPad7,    // 0x67            
      Key_NumPad8,    // 0x68            
      Key_NumPad9,    // 0x69 
      Key_Unknown,    // 0x6A 
      Key_Unknown,    // 0x6B 
      Key_Unknown,    // 0x6C 
      Key_Unknown,    // 0x6D 
      Key_Unknown,    // 0x6E 
      Key_Unknown,    // 0x6F 
      Key_F1,         // 0x70                 
      Key_F2,         // 0x71                 
      Key_F3,         // 0x72                 
      Key_F4,         // 0x73                 
      Key_F5,         // 0x74                 
      Key_F6,         // 0x75                 
      Key_F7,         // 0x76                 
      Key_F8,         // 0x77                 
      Key_F9,         // 0x78                 
      Key_F10,        // 0x79                 
      Key_F11,        // 0x7A                 
      Key_F12,        // 0x7B  
      Key_Unknown,    // 0x7C 
      Key_Unknown,    // 0x7D 
      Key_Unknown,    // 0x7E 
      Key_Unknown,    // 0x7F 
      Key_Unknown,    // 0x80 
      Key_Unknown,    // 0x81 
      Key_Unknown,    // 0x82 
      Key_Unknown,    // 0x83 
      Key_Unknown,    // 0x84 
      Key_Unknown,    // 0x85
      Key_Unknown,    // 0x86 
      Key_Unknown,    // 0x87 
      Key_Unknown,    // 0x88 
      Key_Unknown,    // 0x89
      Key_Unknown,    // 0x8A 
      Key_Unknown,    // 0x8B 
      Key_Unknown,    // 0x8C 
      Key_Unknown,    // 0x8D 
      Key_Unknown,    // 0x8E 
      Key_Unknown,    // 0x8F 
      Key_NumLock,    // 0x90                          
      Key_ScrollLock, // 0x91 
      Key_Unknown,    // 0x92 
      Key_Unknown,    // 0x93 
      Key_Unknown,    // 0x94 
      Key_Unknown,    // 0x95
      Key_Unknown,    // 0x96 
      Key_Unknown,    // 0x97 
      Key_Unknown,    // 0x98 
      Key_Unknown,    // 0x99
      Key_Unknown,    // 0x9A 
      Key_Unknown,    // 0x9B 
      Key_Unknown,    // 0x9C 
      Key_Unknown,    // 0x9D 
      Key_Unknown,    // 0x9E 
      Key_Unknown,    // 0x9F          
      Key_LeftShift,  // 0xA0   
      Key_RightShift, // 0xA1 
      Key_LeftCtrl,   // 0xA2   
      Key_RightCtrl,  // 0xA3             
      Key_LeftAlt,    // 0xA4   
      Key_RightAlt,   // 0xA5   
      Key_Unknown,    // 0xA6 
      Key_Unknown,    // 0xA7 
      Key_Unknown,    // 0xA8 
      Key_Unknown,    // 0xA9
      Key_Unknown,    // 0xAA 
      Key_Unknown,    // 0xAB 
      Key_Unknown,    // 0xAC 
      Key_Unknown,    // 0xAD 
      Key_Unknown,    // 0xAE 
      Key_Unknown,    // 0xAF
      Key_Unknown,    // 0xB0 
      Key_Unknown,    // 0xB1    
      Key_Unknown,    // 0xB2 
      Key_Unknown,    // 0xB3 
      Key_Unknown,    // 0xB4 
      Key_Unknown,    // 0xB5
      Key_Unknown,    // 0xB6 
      Key_Unknown,    // 0xB7 
      Key_Unknown,    // 0xB8 
      Key_Unknown,    // 0xB9             
      Key_Colon,      // 0xBA 
      Key_Equal,      // 0xBB 
      Key_Comma,      // 0xBC 
      Key_Minus,      // 0xBD           
      Key_Period,     // 0xBE              
      Key_Slash,      // 0xBF 
      Key_Tilde,      // 0xC0 
      Key_Unknown,    // 0xC1    
      Key_Unknown,    // 0xC2 
      Key_Unknown,    // 0xC3 
      Key_Unknown,    // 0xC4 
      Key_Unknown,    // 0xC5
      Key_Unknown,    // 0xC6 
      Key_Unknown,    // 0xC7 
      Key_Unknown,    // 0xC8 
      Key_Unknown,    // 0xC9                
      Key_Unknown,    // 0xCA 
      Key_Unknown,    // 0xCB 
      Key_Unknown,    // 0xCC 
      Key_Unknown,    // 0xCD 
      Key_Unknown,    // 0xCE 
      Key_Unknown,    // 0xCF
      Key_Unknown,    // 0xD0
      Key_Unknown,    // 0xD1    
      Key_Unknown,    // 0xD2 
      Key_Unknown,    // 0xD3 
      Key_Unknown,    // 0xD4 
      Key_Unknown,    // 0xD5
      Key_Unknown,    // 0xD6 
      Key_Unknown,    // 0xD7 
      Key_Unknown,    // 0xD8 
      Key_Unknown,    // 0xD9                
      Key_Unknown,    // 0xDA 
      Key_OpenBrace,  // 0xDB  
      Key_Unknown,    // 0xDC        
      Key_CloseBrace, // 0xDD                 
      Key_Quote,      // 0xDE     
      Key_Unknown,    // 0xDF
      Key_Unknown,    // 0xE0
      Key_Unknown,    // 0xE1          
      Key_Baskslash,  // 0xE2  
      Key_Unknown,    // 0xE3 
      Key_Unknown,    // 0xE4 
      Key_Unknown,    // 0xE5
      Key_Unknown,    // 0xE6 
      Key_Unknown,    // 0xE7 
      Key_Unknown,    // 0xE8 
      Key_Unknown,    // 0xE9                
      Key_Unknown,    // 0xEA 
      Key_Unknown,    // 0xEB 
      Key_Unknown,    // 0xEC 
      Key_Unknown,    // 0xED 
      Key_Unknown,    // 0xEE 
      Key_Unknown,    // 0xEF
      Key_Unknown,    // 0xF0
      Key_Unknown,    // 0xF1    
      Key_Unknown,    // 0xF2 
      Key_Unknown,    // 0xF3 
      Key_Unknown,    // 0xF4 
      Key_Unknown,    // 0xF5
      Key_Unknown,    // 0xF6 
      Key_Unknown,    // 0xF7 
      Key_Unknown,    // 0xF8 
      Key_Unknown,    // 0xF9                
      Key_Unknown,    // 0xFA             
      Key_Unknown,    // 0xFB 
      Key_Unknown,    // 0xFC 
      Key_Unknown,    // 0xFD 
      Key_Unknown,    // 0xFE 
      Key_Unknown };  // 0xFF   
#endif   

   Context::Context(void)
   {
   // Clear state
   memset(&keyboard, 0, sizeof(Context::Keyboard));
   memset(&mouse, 0, sizeof(Context::Mouse));
   memset(&joystick, 0, sizeof(Context::Joystick));
   memset(&touchscreen, 0, sizeof(Context::Touchscreen));
   memset(&accelerometer, 0, sizeof(Context::Accelerometer));
   memset(&compass, 0, sizeof(Context::Compass));
   memset(&gyroscope, 0, sizeof(Context::Gyroscope));
   memset(&light, 0, sizeof(Context::Light));
   memset(&proximity, 0, sizeof(Context::Proximity));
   memset(&temperature, 0, sizeof(Context::Temperature));
   memset(&events, 0, sizeof(Context::Events));


   joystick.device.clear();

#ifdef EN_PLATFORM_WINDOWS
#if INTEL_PERCEPTUAL_COMPUTING_2014
   camera.session = nullptr;
#endif
#endif
   camera.device.clear();
   hmd.device.clear();

#ifdef EN_PLATFORM_ANDROID
   looper              = NULL;
   inputQueue          = NULL;
   sensorQueue         = NULL;
   sensorManager       = NULL;
   fileDescriptor      = 0;
   allowCallback       = false;
#endif
   }

//#ifdef EN_PLATFORM_WINDOWS
//#if !INTEL_PERCEPTUAL_COMPUTING_2014
//   Context::Camera::CreativeSenz3D::CreativeSenz3D() :
//      sps(2),
//      image(2)
//   {
//   }
//#endif
//#endif

   bool Context::create(void)
   {
   Log << "Starting module: Input.\n";

   // Clear callbacks array
   events.callback = &en::state::HandleEventByState;

#ifdef EN_PLATFORM_ANDROID
   // Queue for input events
   inputQueue = NULL;

   // Get information about sensors
   sensorManager = ASensorManager_getInstance();

   // Touchscreen
   touchscreen.available = true;
   touchscreen.on = true;
   touchscreen.generatorId = 1;

   // Accelerometr
   accelerometer.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
   if (accelerometer.sensor)
      {
      Log << "                 Accelerometer available." << endl;
      accelerometer.available = true;
      }

   // Compass
   compass.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
   if (compass.sensor)
      {
      Log << "                 Compass available." << endl;
      compass.available = true;
      }

   // Gyroscope
   gyroscope.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
   if (gyroscope.sensor)
      {
      Log << "                 Gyroscope available." << endl;
      gyroscope.available = true;
      }

   // Light
   light.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_LIGHT);
   if (light.sensor)
      {
      Log << "                 Light sensor available." << endl;
      light.available = true;
      }

   // Proximity
   proximity.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_PROXIMITY);
   if (proximity.sensor)
      {
      Log << "                 Proximity sensor available." << endl;
      proximity.available = true;
      }

   // Temperature
   temperature.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_AMBIENT_TEMPERATURE);
   if (temperature.sensor)
      {
      Log << "                 Ambient Temperature sensor available." << endl;
      temperature.available = true;
      }
   else 
      {
      temperature.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_TEMPERATURE);
      if (temperature.sensor)
         {
         Log << "                 Temperature sensor available." << endl;
         temperature.available = true;
         }
      }
 
   inputData.id      = InputEvent; //LOOPER_ID_INPUT;
   inputData.app     = NULL;
   inputData.process = InputContext.touchscreen.update;

   looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
   ALooper_addFd(looper, fileDescriptor, MainEvent, ALOOPER_EVENT_INPUT, NULL,
            &inputData);

   sensorQueue = ASensorManager_createEventQueue(sensorManager, looper, AppEvent, NULL, NULL);

   // Allow onInputQueueCreated callback to attach to looper.
   allowCallback = true;
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   // Touchscreen
   touchscreen.available = true;
   touchscreen.on = true;
   touchscreen.generatorId = 1;
#endif
#ifdef EN_PLATFORM_WINDOWS
   keyboard.available = true;
   keyboard.on        = true;
   mouse.available    = true;
   mouse.on           = true;
#endif

   joystick.init();
   camera.init();
   hmd.init();

   return true;
   }

   void Context::destroy(void)
   {
   Log << "Closing module: Input." << endl;
#ifdef EN_PLATFORM_ANDROID
   if (en::InputContext.inputQueue)
      {
      AInputQueue_detachLooper(en::InputContext.inputQueue);
      en::InputContext.inputQueue = NULL;
      }
#endif

   joystick.destroy();
   camera.destroy();
   hmd.destroy();
   }

#ifdef EN_PLATFORM_ANDROID
   void Context::Touchscreen::update(struct android_app* app, struct android_poll_source* source)
   {
   Log << "Touchscreen update called!";


   }
#endif


   void Interface::update(void)
   {
#ifdef EN_PLATFORM_ANDROID
   // Process all input events
   if (AInputQueue_hasEvents(InputContext.inputQueue))
      {
      AInputEvent* event = NULL;
      // TODO: 
      // E/NativeActivity(27972): channel '40a263a0 com.game.Hexagon/android.app.NativeActivity (client)' ~ Failed to receive dispatch signal.  status=-11
      while (AInputQueue_getEvent(InputContext.inputQueue, &event) >= 0)  
            {
            sint32 events = 0;
            sint32 result = ALooper_pollOnce(0, NULL, &events, NULL);

            // Check if event can be processed 
            if (AInputQueue_preDispatchEvent(InputContext.inputQueue, event)) 
               continue;
      
            // Keyboard
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
               {
               // TODO: We don't use Android virtual keyboard.
               }
            else
            // Touch
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)  
               if (AInputEvent_getSource(event) == AINPUT_SOURCE_TOUCHSCREEN)
                  { 
                  uint32 touches      = AMotionEvent_getPointerCount(event);
                  sint32 action       = AMotionEvent_getAction(event);
                  sint32 changedIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;;
                  sint32 changedId    = AMotionEvent_getPointerId(event, changedIndex);

                  bool pressed    = false;
                  bool released   = false;
                  bool releasedWA = false;
                  bool updatedWA[10];

                  // There is new gesture starting in this event
                  if (touches > InputContext.touchscreen.gestures)
                     pressed = true;

                  // One gesture is ending in this event
                  if ( (touches == InputContext.touchscreen.gestures) && 
                       ( (changedIndex != 0) || (action & AMOTION_EVENT_ACTION_UP) || (action & AMOTION_EVENT_ACTION_OUTSIDE)) )
                     released = true;

                  // HW WA: It is possible that device will set wrong "action"
                  //        bits during pressing or releasing touch events in
                  //        multitouch mode. This additional check ensures that
                  //        all released touches will be captured.
                  if (touches < InputContext.touchscreen.gestures)
                     {
                     releasedWA = true;
                     for(uint8 i=0; i<10; ++i)
                        updatedWA[i] = false;
                     }

                  // Proces all touch points
                  for(uint8 i=0; i<touches; ++i)
                     {
                     Touch touch;
                     touch.position.x = AMotionEvent_getX(event, i);
                     touch.position.y = AMotionEvent_getY(event, i);
                     touch.pressure   = AMotionEvent_getPressure(event, i);
                     touch.size       = AMotionEvent_getSize(event, i);
                     touch.time       = AMotionEvent_getEventTime(event);

                     // Add new gesture
                     if (pressed && (i == changedIndex))
                        {
                        uint8 index = InputContext.touchscreen.gestures;
                        InputContext.touchscreen.gesture[index].first.touches.clear();
                        InputContext.touchscreen.gesture[index].first.touches.push_back(touch);
                        InputContext.touchscreen.gesture[index].second = changedId;
                        InputContext.touchscreen.gestures++;
                        continue;
                        }

                     // Find current gesture to update or remove
                     uint8 index = 0;
                     sint32 id = AMotionEvent_getPointerId(event, i);
                     for(; index<10; ++index)
                        if (InputContext.touchscreen.gesture[index].second == id)
                           break;

                     // Update existing gesture
                     InputContext.touchscreen.gesture[index].first.touches.push_back(touch);
                     if (releasedWA)
                        updatedWA[index] = true;

                     // Remove old gesture
                     if (released && (i == changedIndex))
                        {
                        // Move old gesture to history 
                        // TODO!!!

                        // Queue of gestures in progres stores them in order
                        // of time they were started. Therefore shift all 
                        // later gestures one field up.
                        InputContext.touchscreen.gestures--;
                        for(uint8 j=index; j<InputContext.touchscreen.gestures; ++j)
                           InputContext.touchscreen.gesture[j] = InputContext.touchscreen.gesture[j+1];
                        }
                     }

                  // Found which gesture wasn't updated in this event and remove it
                  if (releasedWA)
                     for(uint8 i=0; i<10; ++i)
                        if (!updatedWA[i])
                           {
                           // Move old gesture to history 
                           // TODO!!!
                                             
                           // Queue of gestures in progres stores them in order
                           // of time they were started. Therefore shift all 
                           // later gestures one field up.
                           InputContext.touchscreen.gestures--;
                           for(uint8 j=i; j<InputContext.touchscreen.gestures; ++j)
                              InputContext.touchscreen.gesture[j] = InputContext.touchscreen.gesture[j+1];
                           break;
                           }
                  }

            AInputQueue_finishEvent(InputContext.inputQueue, event, 1);
            }
      }

   // Process all sensor events
   ASensorEvent event;
   while (ASensorEventQueue_getEvents(InputContext.sensorQueue, &event, 1) > 0) 
         {
         // Accelerometer
         if (event.type == ASENSOR_TYPE_ACCELEROMETER) 
            {
            // Calculate new delta
            InputContext.accelerometer.delta.x = event.acceleration.v[0] - InputContext.accelerometer.vector.x;
            InputContext.accelerometer.delta.y = event.acceleration.v[1] - InputContext.accelerometer.vector.y;
            InputContext.accelerometer.delta.z = event.acceleration.v[2] - InputContext.accelerometer.vector.z;

            // Filter sensor noise
            if (abs(InputContext.accelerometer.delta.x) < InputContext.accelerometer.noise)
               InputContext.accelerometer.delta.x = 0.0f;
            if (abs(InputContext.accelerometer.delta.y) < InputContext.accelerometer.noise)
               InputContext.accelerometer.delta.y = 0.0f;
            if (abs(InputContext.accelerometer.delta.z) < InputContext.accelerometer.noise)
               InputContext.accelerometer.delta.z = 0.0f;

            // Update current acceleration
            memcpy(&InputContext.accelerometer.vector, &event.acceleration.v[0], sizeof(float3));
            }

         // Compass
         if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD)
            {
            // event.magnetic.
            // TODO!
            }

         // Gyroscope
         if (event.type == ASENSOR_TYPE_GYROSCOPE)
            {
            InputContext.gyroscope.azimuth = event.vector.azimuth;
            InputContext.gyroscope.pitch   = event.vector.pitch;
            InputContext.gyroscope.roll    = event.vector.roll;

            // Filter sensor noise
            if (abs(InputContext.gyroscope.azimuth) < InputContext.gyroscope.noise)
               InputContext.gyroscope.azimuth = 0.0f;
            if (abs(InputContext.gyroscope.pitch) < InputContext.gyroscope.noise)
               InputContext.gyroscope.pitch = 0.0f;
            if (abs(InputContext.gyroscope.roll) < InputContext.gyroscope.noise)
               InputContext.gyroscope.roll = 0.0f;
            }

         // Light sensor
         if (event.type == ASENSOR_TYPE_LIGHT)
            {
            InputContext.light.lux = event.light;
            }

         // Proximity sensor
         if (event.type == ASENSOR_TYPE_PROXIMITY)
            {
            InputContext.proximity.distance = event.distance;
            }

         // Temperature sensor
         if (event.type == ASENSOR_TYPE_TEMPERATURE ||
               event.type == ASENSOR_TYPE_AMBIENT_TEMPERATURE)
            {
            InputContext.temperature.celcius = event.temperature;
            }
         }
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   bps_event_t* event = NULL; 
   bps_get_event(&event, 1);

   // Engine event
   Event enEvent;
   memset(&enEvent, 0, sizeof(Event));

   // Process events
   while(event)
        {
        // Check domain of the event
        sint32 domain = bps_event_get_domain(event);
  
        // Process screen events  
        if (domain == screen_get_domain())
           {
           sint32 eventType;
           screen_event_t screenEvent = screen_event_get_event(event);
           screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_TYPE, &eventType);

           int screen_val;
           screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_TYPE, &screen_val);

           char buf[1024];
           sint32 pos[2];

           screen_get_event_property_cv(screenEvent, SCREEN_PROPERTY_ID_STRING, sizeof(buf), buf);
           screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_POSITION, pos);

           if (eventType == SCREEN_EVENT_CREATE)
              {
              sint32 visible = 0;

              // Set all registered advertisements visible
              //for (uint8 i=0; i<MonetizationContext.banners.size(); ++i)
                 // TODO: Finish it, declare ad_banner
                 // {
                 // if (bbads_banner_is_visible(ad_banner, &visible) != BBADS_EOK)
                 //    Log << "WARNING: Cannot receive advertisement visibility status!\n";
                 // 
                 // if (!visible)
                 //    {
                 //    if (bbads_banner_set_window_visible(ad_banner) != BBADS_EOK)
                 //       Log << "WARNING: Cannot set advertisement window as visible!\n";
                 //    if (bbads_banner_display(MonetizationContext.advertisement, GpuContext.device.screen.context, event) != BBADS_EOK)
                 //       Log << "WARNING: Cannot set advertisement visible during window creation!\n";
                 //    }
                 //}
              }

           // Keyboard
           if (eventType == SCREEN_EVENT_KEYBOARD)
              {
              // TODO: Finish implementation
              // screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_KEY_FLAGS, &flags);
              // screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_KEY_SYM, &value);
              }

           // Mouse
           if (eventType == SCREEN_EVENT_POINTER) 
              {
              // Update buttons state
              sint32 buttons;
              screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_BUTTONS, &buttons);
              InputContext.mouse.buttons[LeftMouseButton]   = checkBit(buttons, SCREEN_LEFT_MOUSE_BUTTON)   ? Pressed : Released;
              InputContext.mouse.buttons[MiddleMouseButton] = checkBit(buttons, SCREEN_MIDDLE_MOUSE_BUTTON) ? Pressed : Released;
              InputContext.mouse.buttons[RightMouseButton]  = checkBit(buttons, SCREEN_RIGHT_MOUSE_BUTTON)  ? Pressed : Released;

              // TODO: Finish implementation

              // int wheel;
              // // A move event will be fired unless a button state changed.
              // bool move = true;
              // bool left_move = false;
              // // This is a mouse move event
              // screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_SOURCE_POSITION, position);
              // screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_MOUSE_WHEEL, &wheel);
              }

           // Joystick
           if (eventType == SCREEN_EVENT_JOYSTICK)
              {
              // TODO: Finish implementation
              }

           // Gamepad
           if (eventType == SCREEN_EVENT_GAMEPAD)
              {
              // TODO: Finish implementation
              }

           // Touchscreen
           if ( eventType == SCREEN_EVENT_MTOUCH_TOUCH ||
                eventType == SCREEN_EVENT_MTOUCH_MOVE  ||
                eventType == SCREEN_EVENT_MTOUCH_RELEASE )
              {
              sint32 position[2];
              sint32 pressure;
              sint32 size[2];
              sint64 timestamp;
              sint32 touchId;
              sint32 sequenceId;

              screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_TOUCH_ID, (int*)&touchId); 
              screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_SEQUENCE_ID, (int*)&sequenceId);       
              screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_SOURCE_POSITION, (int*)&position);
              screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_TOUCH_PRESSURE, (int*)&pressure);
              screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_SIZE, (int*)&size);
              screen_get_event_property_llv(screenEvent, SCREEN_PROPERTY_TIMESTAMP, (long long*)&timestamp);
			     // screen_get_event_property_iv(screenEvent, SCREEN_PROPERTY_TOUCH_ORIENTATION, (int*)&mtouch_event->orientation);

              Touch touch;
              touch.position.x = float(position[0]);
              touch.position.y = float(position[1]);
              touch.pressure   = float(pressure);
              touch.size       = float(size[0] > size[1] ? size[0] : size[1]) / float(GpuContext.screen.width);
              touch.time       = timestamp;

              // Add new gesture
              if (eventType == SCREEN_EVENT_MTOUCH_TOUCH)
                 {
                 uint8 index = InputContext.touchscreen.gestures;
                 InputContext.touchscreen.gesture[index].first.touches.clear();
                 InputContext.touchscreen.gesture[index].first.touches.push_back(touch);
                 InputContext.touchscreen.gesture[index].second = touchId; // InputContext.touchscreen.generatorId; 
                 InputContext.touchscreen.gestures++;
                 //InputContext.touchscreen.generatorId++;
                 //if (InputContext.touchscreen.generatorId < 0) 
                 //   InputContext.touchscreen.generatorId = 1;
                 }

              // Find current gesture to update or remove
              uint8 index = 0;
              for(; index<10; ++index)
                 if (InputContext.touchscreen.gesture[index].second == touchId)
                    break;

              // Update existing gesture
              if (eventType == SCREEN_EVENT_MTOUCH_MOVE)
                 InputContext.touchscreen.gesture[index].first.touches.push_back(touch);

              // Remove old gesture
              if (eventType == SCREEN_EVENT_MTOUCH_RELEASE)
                 {
                 // Move old gesture to history 
                 // TODO!!!

                 // Queue of gestures in progres stores them in order
                 // of time they were started. Therefore shift all 
                 // later gestures one field up.
                 InputContext.touchscreen.gestures--;
                 for(uint8 j=index; j<InputContext.touchscreen.gestures; ++j)
                    InputContext.touchscreen.gesture[j] = InputContext.touchscreen.gesture[j+1];
                 }
              }
           }
        else
        // Process audio events
        if (domain == audiodevice_get_domain())
           {
           // If it is a audio device event then it means a new audio device
           // has been enabled and a switch is required.  We close the old
           // one, open the new audio device using the path and get the card 
           // number so that we can close and re-open the mixer with the new 
           // card number.
           const char* deviceName = audiodevice_event_get_path(event);
           if (deviceName != NULL)
              {
              //snd_mixer_close(AudioContext.mixer);
              //snd_pcm_close(AudioContext.pcm);
              //AudioContext.recreate(deviceName);
              } 
           }
        else
        // Process system events
        if (domain == navigator_get_domain())
           {
           uint32 code = bps_event_get_code(event);
           if (code == NAVIGATOR_EXIT)
              {
              enEvent.type = AppClose;
              en::InputContext.events.callback(enEvent); 
              }
           if (code == NAVIGATOR_WINDOW_ACTIVE)
              {
              enEvent.type = AppWindowActive;
              en::InputContext.events.callback(enEvent); 
              }
           if (code == NAVIGATOR_WINDOW_INACTIVE)
              {
              enEvent.type = AppWindowHidden;
              en::InputContext.events.callback(enEvent); 
              }
           if (code == NAVIGATOR_WINDOW_STATE)
              {
              navigator_window_state_t state = navigator_event_get_window_state(event);      
              if (state == NAVIGATOR_WINDOW_FULLSCREEN) 
                 {
                 enEvent.type = AppWindowActive;
                 en::InputContext.events.callback(enEvent); 
                 }
              if (state == NAVIGATOR_WINDOW_THUMBNAIL)
                 {
                 enEvent.type = AppWindowInBackground;
                 en::InputContext.events.callback(enEvent);
                 }
              if (state == NAVIGATOR_WINDOW_INVISIBLE)
                 {
                 enEvent.type = AppWindowHidden;
                 en::InputContext.events.callback(enEvent);
                 }
              }
           }
        else
        // Process advertisement banner events
        if (domain == bbads_get_domain())
           { 
           bbads_banner_t* advertisement;
           bbads_event_get_banner(event, &advertisement);

           uint32 code = bps_event_get_code(event);
           if (code == BBADS_EVENT_NAVIGATING)  
              {
              Log << "Advertisement banner was clicked, opening web browser.\n";
              // TODO: 
              // Should this mark game as invisible ?
              // Or there will be separate event about that ?
              }
           }

        // Check if there is more events fto process
        bps_get_event(&event, 1);
        }
#endif
#ifdef EN_PLATFORM_WINDOWS
   if (PeekMessage(&InputContext.msg, NULL, 0, 0, PM_REMOVE)) // | PM_QS_INPUT
      {
      TranslateMessage(&InputContext.msg);
      DispatchMessage(&InputContext.msg);   // Call WndProc to process incoming events
      }
#endif

   // Joystick events
   for(uint8 i=0; i<InputContext.joystick.device.size(); ++i)
      InputContext.joystick.device[i]->update();

   // Camera stream events
   for(uint8 i=0; i<InputContext.camera.device.size(); ++i)
      InputContext.camera.device[i]->update();

   // VR controllers update
   for(uint8 i=0; i<InputContext.hmd.device.size(); ++i)
      InputContext.hmd.device[i]->update();
   }

   bool Interface::Keyboard::on(void)
   {
   if (InputContext.keyboard.available)
      InputContext.keyboard.used = true;
   return InputContext.keyboard.used;
   }

   bool Interface::Keyboard::off(void)
   {
   InputContext.keyboard.used = false;
   return InputContext.keyboard.used;
   }

   bool Interface::Keyboard::state(void)
   {
   return InputContext.keyboard.used;
   }

   bool Interface::Keyboard::available(void)
   {
   return InputContext.keyboard.available;
   }

   bool Interface::Keyboard::pressed(const en::input::Key key)
   {
   return InputContext.keyboard.keys[key] == Pressed ? true : false;
   }

   bool Interface::Mouse::on(void)
   {
   if (InputContext.mouse.available)
      InputContext.mouse.used = true;
   return InputContext.mouse.used;
   }

   bool Interface::Mouse::off(void)
   {
   InputContext.mouse.used = false;
   return InputContext.mouse.used;
   }

   bool Interface::Mouse::state(void)
   {
   return InputContext.mouse.used;
   }

   bool Interface::Mouse::available(void)
   {
   return InputContext.mouse.available;
   }

   float2 Interface::Mouse::position(void)
   {
   float mx = float(InputContext.mouse.x) / float(GpuContext.screen.width);
   float my = float(InputContext.mouse.y) / float(GpuContext.screen.height);

   return float2(mx, my);
   }

   uint32 Interface::Mouse::position(en::input::Axis axis)
   {
   if (axis == AxisX)
      return InputContext.mouse.x;
   if (axis == AxisY)
      return InputContext.mouse.y;
   return 0;
   }

   bool Interface::Mouse::position(const uint32 x, const uint32 y)
   {
#ifdef EN_PLATFORM_WINDOWS
   return SetCursorPos(x,y);
#endif
   return false;
   }

   bool Interface::Mouse::pressed(const en::input::MouseButton button)
   {
   return InputContext.mouse.buttons[button] == Pressed ? true : false;
   }

   //bool Interface::Mouse::inArea(const float4 area, const en::input::MouseButton button)
   //{
   // NRenderingContext& enGpu = NRenderingContext::getInstance();

   // float mx = float(InputContext.mouse.x) / float(enGpu.screen.width());
   // float my = float(InputContext.mouse.y) / float(enGpu.screen.height());

   // if ( !enGpu.screen.created() )
   //    return false;
   //  
   // if ( (key < 3) &&
   //      (m_keys[key]) &&
   //      (mx > area.x) &&
   //      (my > area.y) &&
   //      (mx < area.x + area.z) &&
   //      (my < area.y + area.w) )
   //    return true;
   // return false;
   //}

   void Interface::Mouse::show(void)
   {
#ifdef EN_PLATFORM_WINDOWS
   ShowCursor(true);
#endif
   }

   void Interface::Mouse::hide(void)
   {
#ifdef EN_PLATFORM_WINDOWS
   ShowCursor(false);
#endif
   }

   //uint8 Interface::Joystick::count(void)
   //{
   //return InputContext.joystick.count;
   //}

   //bool Interface::Joystick::on(uint8 num)
   //{
   //if (InputContext.joystick.count <= num)
   //   return false;

   //InputContext.joystick.used[num] = true;
   //if (!InputContext.joystick.on[num])
   //   InputContext.joystick.on[num] = true;
   //return true;
   //}

   //bool Interface::Joystick::off(uint8 num)
   //{
   //if (InputContext.joystick.count <= num)
   //   return false;

   //InputContext.joystick.used[num] = false;
   //if ( InputContext.joystick.on[num] )
   //   InputContext.joystick.on[num] = false;
   //return true;
   //}

   //bool Interface::Joystick::state(uint8 num)
   //{
   //if (InputContext.joystick.count <= num)
   //   return false;
   //return InputContext.joystick.used[num];
   //}

   //bool Interface::Joystick::available(uint8 count)
   //{
   //return InputContext.joystick.count >= count ? true : false;
   //}

   //bool Interface::Joystick::pressed(const uint8 button)
   //{
   //return InputContext.joystick.state[0].buttons[button] == Pressed ? true : false;
   //} 

   //bool Interface::Joystick::pressed(const uint8 joystick, const uint8 button)
   //{
   //if (InputContext.joystick.count <= joystick)
   //   return false;   

   //return InputContext.joystick.state[joystick].buttons[button] == Pressed ? true : false;
   //}

   //float Interface::Joystick::position(const en::input::JoystickAxis axis)
   //{
   //if (axis == JoystickAxisX)
   //   return float(InputContext.joystick.state[0].x.position - 32768) / 32768.0f;
   //if (axis == JoystickAxisY)
   //   return float(InputContext.joystick.state[0].y.position - 32768) / 32768.0f;
   //if (axis == JoystickAxisZ)
   //   return float(InputContext.joystick.state[0].z.position - 32768) / 32768.0f;
   //return 0.0f;
   //}

   //float Interface::Joystick::position(const uint8 joystick, const en::input::JoystickAxis axis)
   //{
   //if (InputContext.joystick.count <= joystick)
   //   return 0.0f;  

   //if (axis == JoystickAxisX)
   //   return float(InputContext.joystick.state[joystick].x.position - 32768) / 32768.0f;
   //if (axis == JoystickAxisY)
   //   return float(InputContext.joystick.state[joystick].y.position - 32768) / 32768.0f;
   //if (axis == JoystickAxisZ)
   //   return float(InputContext.joystick.state[joystick].z.position - 32768) / 32768.0f;
   //return 0.0f;
   //}

   bool Interface::Touchscreen::on(void)
   {
   if (InputContext.touchscreen.available)
      {
      InputContext.touchscreen.used = true;
      if (!InputContext.touchscreen.on)
         InputContext.touchscreen.on = true;
      }

   return InputContext.touchscreen.used;
   }

   bool Interface::Touchscreen::off(void)
   {
   InputContext.touchscreen.used = false;
   if (InputContext.touchscreen.available &&
       InputContext.touchscreen.on)
      InputContext.touchscreen.on = false;

   return InputContext.touchscreen.used;
   }

   bool Interface::Touchscreen::state(void)
   {
   return InputContext.touchscreen.used;
   }

   bool Interface::Touchscreen::available(void)
   {
   return InputContext.touchscreen.available;
   }

   uint8 Interface::Touchscreen::gestures(void)
   {
   return InputContext.touchscreen.gestures;
   }

   sint32 Interface::Touchscreen::gestureId(uint8 gesture)
   {
   assert(gesture < InputContext.touchscreen.gestures);
   return InputContext.touchscreen.gesture[gesture].second;
   }

   Gesture Interface::Touchscreen::gesture(uint8 gesture)
   {
   assert(gesture < InputContext.touchscreen.gestures);
   return InputContext.touchscreen.gesture[gesture].first;
   }

   Gesture Interface::Touchscreen::gesture(sint32 gestureId)
   {
   for(uint8 i=0; i<InputContext.touchscreen.gestures; ++i)
      if (InputContext.touchscreen.gesture[i].second == gestureId)
         return InputContext.touchscreen.gesture[i].first;

   return Gesture();
   }

   bool Interface::Touchscreen::gesturePresent(sint32 gestureId)
   {
   for(uint8 i=0; i<InputContext.touchscreen.gestures; ++i)
      if (InputContext.touchscreen.gesture[i].second == gestureId)
         return true;
   return false;
   }

   bool Interface::Touchscreen::touched(float4 rect)
   {
   for(uint8 i=0; i<InputContext.touchscreen.gestures; ++i)
      {
      float2 position = InputContext.touchscreen.gesture[i].first.touches[InputContext.touchscreen.gesture[i].first.touches.size() - 1].position;
      float2 normalized;
      normalized.x = position.x / float(GpuContext.screen.width);
      normalized.y = position.y / float(GpuContext.screen.height);
      if (inRectangle(rect, normalized))
         return true;
      }
   return false;
   }

   bool Interface::Accelerometer::on(void)
   {
   if (InputContext.accelerometer.available)
      {
      InputContext.accelerometer.used = true;
      if (!InputContext.accelerometer.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Accelerometr with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.accelerometer.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.accelerometer.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.accelerometer.on     = true;
         InputContext.accelerometer.vector = float3(0.0f, 0.0f, 9.8f);
         }
      }

   return InputContext.accelerometer.used;
   }

   bool Interface::Accelerometer::on(const float noise)
   {
   if (InputContext.accelerometer.available)
      {
      InputContext.accelerometer.used  = true;
      InputContext.accelerometer.noise = noise;
      if (!InputContext.accelerometer.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Accelerometr with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.accelerometer.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.accelerometer.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.accelerometer.on     = true;
         InputContext.accelerometer.vector = float3(0.0f, 0.0f, 9.8f);
         }
      }

   return InputContext.accelerometer.used;
   }

   bool Interface::Accelerometer::off(void)
   {
   InputContext.accelerometer.used = false;
   if (InputContext.accelerometer.available &&
       InputContext.accelerometer.on)
      {
#ifdef EN_PLATFORM_ANDROID
      ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                      InputContext.accelerometer.sensor);
#endif
      InputContext.accelerometer.on = false;
      }

   return InputContext.accelerometer.used;
   }

   bool Interface::Accelerometer::state(void)
   {
   return InputContext.accelerometer.used;
   }

   bool Interface::Accelerometer::available(void)
   {
   return InputContext.accelerometer.available;
   }

   float3 Interface::Accelerometer::deltas(void)
   {
   return InputContext.accelerometer.delta;
   }

   float3 Interface::Accelerometer::vector(void)
   {
   return InputContext.accelerometer.vector;
   }





   bool Interface::Compass::on(void)
   {
   if (InputContext.compass.available)
      {
      InputContext.compass.used = true;
      if (!InputContext.compass.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Compass with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.compass.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.compass.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.compass.on     = true;
         }
      }

   return InputContext.compass.used;
   }

   bool Interface::Compass::off(void)
   {
   InputContext.compass.used = false;
   if (InputContext.compass.available &&
       InputContext.compass.on)
      {
#ifdef EN_PLATFORM_ANDROID
      ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                      InputContext.compass.sensor);
#endif
      InputContext.compass.on = false;
      }

   return InputContext.compass.used;
   }

   bool Interface::Compass::state(void)
   {
   return InputContext.compass.used;
   }

   bool Interface::Compass::available(void)
   {
   return InputContext.compass.available;
   }




   bool Interface::Gyroscope::on(void)
   {
   if (InputContext.gyroscope.available)
      {
      InputContext.gyroscope.used = true;
      if (!InputContext.gyroscope.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Gyroscope with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.gyroscope.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.gyroscope.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.gyroscope.on     = true;
         }
      }

   return InputContext.gyroscope.used;
   }

   bool Interface::Gyroscope::on(const float noise)
   {
   if (InputContext.gyroscope.available)
      {
      InputContext.gyroscope.used = true;
      InputContext.gyroscope.noise = noise;
      if (!InputContext.gyroscope.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Gyroscope with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.gyroscope.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.gyroscope.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.gyroscope.on     = true;
         }
      }

   return InputContext.gyroscope.used;
   }

   bool Interface::Gyroscope::off(void)
   {
   InputContext.gyroscope.used = false;
   if (InputContext.gyroscope.available &&
       InputContext.gyroscope.on)
      {
#ifdef EN_PLATFORM_ANDROID
      ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                      InputContext.gyroscope.sensor);
#endif
      InputContext.gyroscope.on = false;
      }

   return InputContext.gyroscope.used;
   }

   bool Interface::Gyroscope::state(void)
   {
   return InputContext.gyroscope.used;
   }

   bool Interface::Gyroscope::available(void)
   {
   return InputContext.gyroscope.available;
   }

   void Interface::Gyroscope::noise(const float noise)
   {
   InputContext.gyroscope.noise = noise;
   }

   float Interface::Gyroscope::azimuth(void)
   {
   return InputContext.gyroscope.azimuth;
   }

   float Interface::Gyroscope::pitch(void)
   {
   return InputContext.gyroscope.pitch;
   }

   float Interface::Gyroscope::roll(void)
   {
   return InputContext.gyroscope.roll;
   }



   bool Interface::Light::on(void)
   {
   if (InputContext.light.available)
      {
      InputContext.light.used = true;
      if (!InputContext.light.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Light sensor with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.light.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.light.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.light.on  = true;
         InputContext.light.lux = 0.0f;
         }
      }

   return InputContext.light.used;
   }

   bool Interface::Light::off(void)
   {
   InputContext.light.used = false;
   if (InputContext.light.available &&
       InputContext.light.on)
      {
#ifdef EN_PLATFORM_ANDROID
      ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                      InputContext.light.sensor);
#endif
      InputContext.light.on = false;
      }

   return InputContext.light.used;
   }

   bool Interface::Light::state(void)
   {
   return InputContext.light.used;
   }

   bool Interface::Light::available(void)
   {
   return InputContext.light.available;
   }

   float Interface::Light::lux(void)
   {
   return InputContext.light.lux;
   }



   bool Interface::Proximity::on(void)
   {
   if (InputContext.proximity.available)
      {
      InputContext.proximity.used = true;
      if (!InputContext.proximity.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Proximity with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.proximity.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.proximity.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.proximity.on       = true;
         InputContext.proximity.distance = 0.0f;
         }
      }

   return InputContext.proximity.used;
   }

   bool Interface::Proximity::off(void)
   {
   InputContext.proximity.used = false;
   if (InputContext.proximity.available &&
       InputContext.proximity.on)
      {
#ifdef EN_PLATFORM_ANDROID
      ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                      InputContext.proximity.sensor);
#endif
      InputContext.proximity.on = false;
      }

   return InputContext.proximity.used;
   }

   bool Interface::Proximity::state(void)
   {
   return InputContext.proximity.used;
   }

   bool Interface::Proximity::available(void)
   {
   return InputContext.proximity.available;
   }

   float Interface::Proximity::distance(void)
   {
   return InputContext.proximity.distance;
   }
 
 
 
   bool Interface::Temperature::on(void)
   {
   if (InputContext.temperature.available)
      {
      InputContext.temperature.used = true;
      if (!InputContext.temperature.on)
         {
#ifdef EN_PLATFORM_ANDROID
         // Check Proximity with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.temperature.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.temperature.sensor, 
                                        (1000L/60)*1000);
#endif
         InputContext.temperature.on      = true;
         InputContext.temperature.celcius = 0.0f;
         }
      }

   return InputContext.temperature.used;
   }

   bool Interface::Temperature::off(void)
   {
   InputContext.temperature.used = false;
   if (InputContext.temperature.available &&
       InputContext.temperature.on)
      {
#ifdef EN_PLATFORM_ANDROID
      ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                      InputContext.temperature.sensor);
#endif
      InputContext.temperature.on = false;
      }

   return InputContext.temperature.used;
   }

   bool Interface::Temperature::state(void)
   {
   return InputContext.temperature.used;
   }

   bool Interface::Temperature::available(void)
   {
   return InputContext.temperature.available;
   }
 
   float Interface::Temperature::celcius(void)
   {
   return InputContext.temperature.celcius;
   }

   void Interface::Events::set(EventHandlingFuncPtr ptr)
   {
   InputContext.events.callback = ptr;
   }

   void Interface::Events::clear(void)
   {
   InputContext.events.callback = &en::state::HandleEventByState;
   }

   }

input::Context   InputContext;
input::Interface Input;
}
