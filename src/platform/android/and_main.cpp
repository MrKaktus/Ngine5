/*

 Ngine v5.0
 
 Module      : Android specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#include "core/defines.h"
#include "core/types.h"

#include "core/storage/storage.h"
#include "core/config/context.h"
#include "core/log/context.h"
#include "audio/context.h"
#include "input/context.h"
#include "platform/context.h"
#include "scene/context.h"

#ifdef EN_PLATFORM_ANDROID
#include <jni.h>
//#include "Ngine4/platform/android/android_native_app_glue.h"
#include "platform/android/and_events.h"  
#include "platform/android/and_main.h"       
#include "threading/scheduler.h"

// TODO: Temporaty thread spawner, in future full Task-Pool
#include <pthread.h>

//// Native Activity state that should be stored when 
//// it is moved to the background or killed silently.
//struct ActivityState
//       {
//       ANativeActivity* activity; // The ANativeActivity object instance that this app is running in.
//       AConfiguration* config;    // The current configuration the app is running in.
//       ALooper* looper;           // The ALooper associated with the app's thread.
//       AInputQueue* inputQueue;   // When non-NULL, this is the input queue from which the app will receive user input events.
//       ANativeWindow* window;     // When non-NULL, this is the window surface that the app can draw in.
//       ARect contentRect;         // Current content rectangle of the window; this is the area where the window's content should be placed to be seen by the user.
//       };
//





namespace en
{
   namespace android
   {
   // TODO: Temporary thread for application
   pthread_t thread;
   
   // Application main thread.
   void* MainThread(void* in)
   {
   ANativeActivity* activity = (ANativeActivity*)in;

   // Init modules in proper order
   en::storage::Interface::create(activity->assetManager);
   ConfigContext.create(0, nullptr);
   LogContext.create();
   SystemContext.create();
   AudioContext.create();
   InputContext.create();
   // NSchedulerContext::create();
   GpuContext.create(NULL);

   StateContext.create();
   ApplicationMainC(0, NULL);
 
   //// Get subsystems singletons handles
   //NSchedulerContext& enScheduler = NSchedulerContext::getInstance();
   
   //// Main thread starts to work like other worker 
   //// threads, by executing application as first 
   //// task. When it will terminate from inside, it 
   //// will return here. This will allow sheduler 
   //// destructor to close rest of worker threads.
   //enScheduler.start(new MainTask(0, 0));
   en::Storage = nullptr;
   return NULL;
   }

   static void onStart(ANativeActivity* activity)
   {
   Log << "Callback: Start.\n";
   }

   static void onResume(ANativeActivity* activity)
   {
   Log << "Callback: Resume.\n";

   // Create application main thread
   pthread_attr_t attr; 
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   pthread_create(&thread, &attr, MainThread, activity);
   }

   static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) 
   {
   Log << "Callback: Input Queue Created.\n";

   // Wait until main threads input subsystem is ready.
   while(!InputContext.allowCallback);

   // Swith looper to new queue
   if (InputContext.inputQueue)
      AInputQueue_detachLooper(InputContext.inputQueue);
   if (queue != NULL)
      AInputQueue_attachLooper(queue,
                               InputContext.looper, 
                               en::input::InputEvent,  // LOOPER_ID_INPUT
                               NULL,
                               NULL); // &InputContext.inputData 

   // Then update queue pointer
   InputContext.inputQueue = queue;     
   }

   static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) 
   {
   if (GpuContext.device.window == NULL)
      {
      Log << "Callback: Native Window Created.\n";
      }
   else
   if (GpuContext.device.window != window)
      {
      Log << "Callback: Native Window has Changed.\n";

      // TODO: You should rebind window here ?
      }
   }

   static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) 
   {
   Log << "Callback: Native Window Resized.\n";
   GpuContext.device.window = window;
   GpuContext.device.width  = ANativeWindow_getWidth(window);
   GpuContext.device.height = ANativeWindow_getHeight(window);
   }

   static void onConfigurationChanged(ANativeActivity* activity) 
   {
   Log << "Callback: Configuration Changed.\n";

   // TODO: AConfiguration_fromAssetManager(android_app->config, activity->assetManager);
   }

   static void onWindowFocusChanged(ANativeActivity* activity, int focused) 
   {
   // App gains focus, init everything
   if (focused)
      {
      Log << "Callback: Appliocation gains focus.\n";

      // When app gains focus, start monitoring sensors.
      if (InputContext.accelerometer.available && 
          InputContext.accelerometer.used) 
         {
         // Check Accelerometr with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.accelerometer.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.accelerometer.sensor, 
                                        (1000L/60)*1000);

         InputContext.accelerometer.on     = true;
         InputContext.accelerometer.vector = float3(0.0f, 0.0f, 9.8f);
         }

      if (InputContext.compass.available && 
          InputContext.compass.used) 
         {
         // Check Compass with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.compass.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.compass.sensor, 
                                        (1000L/60)*1000);

         InputContext.compass.on = true;
         }

      if (InputContext.gyroscope.available && 
          InputContext.gyroscope.used) 
         {
         // Check Gyroscope with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.gyroscope.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.gyroscope.sensor, 
                                        (1000L/60)*1000);

         InputContext.gyroscope.on = true;
         }

      if (InputContext.light.available && 
          InputContext.light.used) 
         {
         // Check Light with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.light.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.light.sensor, 
                                        (1000L/60)*1000);

         InputContext.light.on = true;
         }

      if (InputContext.proximity.available && 
          InputContext.proximity.used) 
         {
         // Check Proximity with 60Hz frequency
         ASensorEventQueue_enableSensor(InputContext.sensorQueue,
                                        InputContext.proximity.sensor);
         ASensorEventQueue_setEventRate(InputContext.sensorQueue,
                                        InputContext.proximity.sensor, 
                                        (1000L/60)*1000);

         InputContext.proximity.on = true;
         }
      }
   else
      {
      // When app loses focus, stop monitoring the sensors.
      // This is to avoid consuming battery while not being used.
      if (InputContext.accelerometer.available && 
          InputContext.accelerometer.used) 
         {
         ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                         InputContext.accelerometer.sensor);

         InputContext.accelerometer.on = false;
         }

      if (InputContext.compass.available && 
          InputContext.compass.used) 
         {
         ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                         InputContext.compass.sensor);

         InputContext.compass.on = false;
         }

      if (InputContext.gyroscope.available && 
          InputContext.gyroscope.used) 
         {
         ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                         InputContext.gyroscope.sensor);

         InputContext.gyroscope.on = false;
         }

      if (InputContext.light.available && 
          InputContext.light.used) 
         {
         ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                         InputContext.light.sensor);

         InputContext.light.on = false;
         }

      if (InputContext.proximity.available && 
          InputContext.proximity.used) 
         {
         ASensorEventQueue_disableSensor(InputContext.sensorQueue,
                                         InputContext.proximity.sensor);

         InputContext.proximity.on = false;
         }
      }
   }

   static void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window) 
   {
   Log << "Callback: Native Window Redraw Needed.\n";
   }

   static void onPause(ANativeActivity* activity)
   {
   Log << "Callback: Pause.\n";
   }

   static void onStop(ANativeActivity* activity)
   {
   Log << "Callback: Stop.\n";
   }

   static void onDestroy(ANativeActivity* activity)
   {
   Log << "Callback: Destroy.\n";


   // Close modules in order
   en::InputContext.destroy();
   en::AudioContext.destroy();
   en::GpuContext.destroy();

   //NSchedulerContext& enScheduler = NSchedulerContext::getInstance();
   //     enScheduler.~NSchedulerContext();

   en::SystemContext.destroy();
   en::LogContext.destroy();
   }

   static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) 
   {
   Log << "Callback: Native Window Destroyed.\n";
   GpuContext.device.window = NULL;
   }

   static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) 
   {
   Log << "Callback: Input Queue Destroyed.\n";
   AInputQueue_detachLooper(en::InputContext.inputQueue);
   en::InputContext.inputQueue = NULL;
   }

   static void onContentRectChanged(ANativeActivity* activity, const ARect* rect) 
   {
   Log << "Callback: Configuration Changed.\n";
   }

   static void onLowMemory(ANativeActivity* activity)
   {
   Log << "Callback: Low Memory.\n";
   }

   static void* onSaveInstanceState(ANativeActivity* activity, size_t* outSize)
   {
   Log << "Callback: Save Instance State.\n";
   }

   }
}

// Main entry point
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) 
{
using namespace en;

// Application is starting for the first time, or was
// silently killed in background because device needed
// memory for other one. Create and init all subsystems
// from scratch.

Log << "Ngine4\n";

// Application states
activity->callbacks->onStart                    = en::android::onStart;
activity->callbacks->onResume                   = en::android::onResume;
activity->callbacks->onPause                    = en::android::onPause;
activity->callbacks->onStop                     = en::android::onStop;
activity->callbacks->onDestroy                  = en::android::onDestroy;

// Screen
activity->callbacks->onWindowFocusChanged       = en::android::onWindowFocusChanged;
activity->callbacks->onNativeWindowCreated      = en::android::onNativeWindowCreated;
activity->callbacks->onNativeWindowResized      = en::android::onNativeWindowResized;
activity->callbacks->onNativeWindowRedrawNeeded = en::android::onNativeWindowRedrawNeeded;
activity->callbacks->onNativeWindowDestroyed    = en::android::onNativeWindowDestroyed;

// Input
activity->callbacks->onInputQueueCreated        = en::android::onInputQueueCreated;
activity->callbacks->onInputQueueDestroyed      = en::android::onInputQueueDestroyed;
                                                
// Misc                                         
activity->callbacks->onConfigurationChanged     = en::android::onConfigurationChanged;
activity->callbacks->onContentRectChanged       = en::android::onContentRectChanged;
activity->callbacks->onLowMemory                = en::android::onLowMemory;
activity->callbacks->onSaveInstanceState        = en::android::onSaveInstanceState;
}


// Prevents from stripping this file out during compilation.
void dummy(void)
{
}




// Protects Ngine entry points from renaming
// by define's declared outside in header file.
// That defines will rename main entry points
// of user application. Then after that, this 
// two define's will safely rename Ngine entry
// points to proper one. This mechanism allows
// Ngine to gain control over user application
// on it's startup to initialize itself quietly
// and on user application exit for safe
// deinitialization.
#define ConsoleMain   android_main

// Native Activity entry point that is using
// android_native_app_glue.h
//void ConsoleMain(struct android_app* state)
//{
//
//// Make sure glue isn't stripped.
////app_dummy();
//
////ApplicationMainC(0, NULL);
//
//// Init modules in proper order
//en::StorageContext.create(state->activity->assetManager);
//en::ConfigContext.create();
//en::LogContext.create();
//en::Log << "Ngine4\n";
//en::Log << "Starting module: Storage." << endl;
//en::Log << "Starting module: Config." << endl;
//en::Log << "Starting module: Log." << endl;
//
////en::SystemContext.create(state);
//    NSchedulerContext::create();
//en::GpuContext.create();
//en::AudioContext.create();
//en::InputContext.create();
//
// // Get subsystems singletons handles
// NSchedulerContext& enScheduler = NSchedulerContext::getInstance();
//
// // Main thread starts to work like other worker 
// // threads, by executing application as first 
// // task. When it will terminate from inside, it 
// // will return here. This will allow sheduler 
// // destructor to close rest of worker threads.
// enScheduler.start(new MainTask(0, 0));
//
//// Close modules in order
//en::InputContext.destroy();
//en::AudioContext.destroy();
//en::GpuContext.destroy();
//     enScheduler.~NSchedulerContext();
//en::SystemContext.destroy();
//en::LogContext.destroy();
//return;
//}

#endif
