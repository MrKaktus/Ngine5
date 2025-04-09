

#include "core/configuration.h"

#if defined(EN_PLATFORM_BLACKBERRY)
#include <bbads/bbads.h>
#include <bbads/event.h>
#include <bps/audiodevice.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/sensor.h>
#include <bps/event.h>
#include <bps/bps.h>

#include "utilities/utilities.h"
#include "input/bbInput.h"

namespace en
{
namespace input
{

BBInterface::BBInterface() :
    CommonInput()
{
    // TODO: Spawn touchscreen and set it's variables
   
    // Touchscreen
    touchscreen.available = true;
    touchscreen.on = true;
    touchscreen.generatorId = 1;
}
   
void BBInterface::update()
{
    bps_event_t* event = nullptr;
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
                //for(uint8 i=0; i<MonetizationContext.banners.size(); ++i)
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
                InputContext.mouse.buttons[underlyingType(MouseButton::Left)]   = checkBit(buttons, SCREEN_LEFT_MOUSE_BUTTON)   ? Pressed : Released;
                InputContext.mouse.buttons[underlyingType(MouseButton::Middle)] = checkBit(buttons, SCREEN_MIDDLE_MOUSE_BUTTON) ? Pressed : Released;
                InputContext.mouse.buttons[underlyingType(MouseButton::Right)]  = checkBit(buttons, SCREEN_RIGHT_MOUSE_BUTTON)  ? Pressed : Released;

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
                {
                    if (InputContext.touchscreen.gesture[index].second == touchId)
                    {
                        break;
                    }
                }

                // Update existing gesture
                if (eventType == SCREEN_EVENT_MTOUCH_MOVE)
                {
                    InputContext.touchscreen.gesture[index].first.touches.push_back(touch);
                }

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
                    {
                        InputContext.touchscreen.gesture[j] = InputContext.touchscreen.gesture[j+1];
                    }
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
                callback(enEvent); 
            }
            if (code == NAVIGATOR_WINDOW_ACTIVE)
            {
                enEvent.type = AppWindowActive;
                callback(enEvent);
            }
            if (code == NAVIGATOR_WINDOW_INACTIVE)
            {
                enEvent.type = AppWindowHidden;
                callback(enEvent);
            }
            if (code == NAVIGATOR_WINDOW_STATE)
            {
                navigator_window_state_t state = navigator_event_get_window_state(event);      
                if (state == NAVIGATOR_WINDOW_FULLSCREEN) 
                {
                    enEvent.type = AppWindowActive;
                    callback(enEvent);
                }
                if (state == NAVIGATOR_WINDOW_THUMBNAIL)
                {
                    enEvent.type = AppWindowInBackground;
                    callback(enEvent);
                }
                if (state == NAVIGATOR_WINDOW_INVISIBLE)
                {
                    enEvent.type = AppWindowHidden;
                    callback(enEvent);
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
}
   
} // en::input
} // en

#endif
