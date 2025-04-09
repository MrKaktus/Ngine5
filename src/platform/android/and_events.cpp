/*

 Ngine v5.0
 
 Module      : Android specific code.
 Requirements: none
 Description : Captures and processes 
               asynchronous callbacks from
               operating system to support
               input signals.

*/

#include "core/defines.h"

#include "input/context.h"
#include "platform/context.h"

#ifdef EN_PLATFORM_ANDROID
#include <android/sensor.h>
#include "platform/android/and_events.h"
//
//// Input events from sensors.
//int32_t AndEvents(struct android_app* app, AInputEvent* event) 
//{
//
//
//
//
//    return 0;
//}

// Main commands, related to whole application behavior.
//void AndCommands(struct android_app* app, int32_t cmd) 
//{
//    switch (cmd) 
//    {
//        case APP_CMD_SAVE_STATE:
//        {
//            // The system has asked us to save our current state.  Do so.
//
//            //engine->app->savedState = malloc(sizeof(struct saved_state));
//            //*((struct saved_state*)engine->app->savedState) = engine->state;
//            //engine->app->savedStateSize = sizeof(struct saved_state);
//            break;
//        }
// 
//        case APP_CMD_INIT_WINDOW:
//        {
//            // The window is being shown. OpenGL ES context and all
//            // resources need to be restored now.
//
//            // TODO !!!!!!!!!!!!!!!!!!!!!!
//
//            //en::GpuContext.screen.
//            en::SystemContext.sleep = false;
//            break;
//        }
// 
//        case APP_CMD_TERM_WINDOW:
//        {
//            // The window is being hidden or closed, OpenGL ES context
//            // needs to be destroyed together with all resources.
//            en::Gpu.screen.destroy();
//            break;
//        }
// 
//        case APP_CMD_GAINED_FOCUS:
//        {
//
//            en::SystemContext.sleep = false;
//            break;
//        }
// 
//        case APP_CMD_LOST_FOCUS:
//        {
//
//            en::SystemContext.sleep = true;
//            break;
//        }
//    }
//}

#endif