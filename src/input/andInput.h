/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_ANDROID
#define ENG_INPUT_ANDROID

#include "input/common.h"

#if defined(EN_PLATFORM_ANDROID)
#include <android/input.h>
#include <android/sensor.h>

namespace en
{
   namespace input
   {
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

   class AndInterface : public CommonInput
      {
      public:
      ALooper*            looper;
      AInputQueue*        inputQueue;
      ASensorEventQueue*  sensorQueue;
      ASensorManager*     sensorManager;
      sint32              fileDescriptor;
      android_poll_source inputData;
      bool                allowCallback;
      
      void update(void);                             // Gets actual input state, call function handling cached events
      
      AndInterface();
      virtual ~AndInterface();                       // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif

#endif
