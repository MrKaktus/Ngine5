

#include "core/configuration.h"

#if defined(EN_PLATFORM_ANDROID)
#include "input/andInput.h"
#include <android/native_activity.h>
#include <android/sensor.h>

namespace en
{
namespace input
{

AndInterface::AndInterface() :
    looper(nullptr),
    inputQueue(nullptr),
    sensorQueue(nullptr),
    sensorManager(nullptr),
    fileDescriptor(0),
    allowCallback(false),
    CommonInput()
{
    // Queue for input events
    inputQueue = nullptr;

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
        Log << "                 Accelerometer available.\n";
        accelerometer.available = true;
    }

    // Compass
    compass.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
    if (compass.sensor)
    {
        Log << "                 Compass available.\n";
        compass.available = true;
    }

    // Gyroscope
    gyroscope.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
    if (gyroscope.sensor)
    {
        Log << "                 Gyroscope available.\n";
        gyroscope.available = true;
    }

    // Light
    light.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_LIGHT);
    if (light.sensor)
    {
        Log << "                 Light sensor available.\n";
        light.available = true;
    }

    // Proximity
    proximity.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_PROXIMITY);
    if (proximity.sensor)
    {
        Log << "                 Proximity sensor available.\n";
        proximity.available = true;
    }

    // Temperature
    temperature.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_AMBIENT_TEMPERATURE);
    if (temperature.sensor)
    {
        Log << "                 Ambient Temperature sensor available.\n";
        temperature.available = true;
    }
    else 
    {
        temperature.sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_TEMPERATURE);
        if (temperature.sensor)
        {
            Log << "                 Temperature sensor available.\n";
            temperature.available = true;
        }
    }
 
    inputData.id      = InputEvent; //LOOPER_ID_INPUT;
    inputData.app     = nullptr;
    inputData.process = InputContext.touchscreen.update;

    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(looper, fileDescriptor, MainEvent, ALOOPER_EVENT_INPUT, NULL, &inputData);

    sensorQueue = ASensorManager_createEventQueue(sensorManager, looper, AppEvent, NULL, NULL);

    // Allow onInputQueueCreated callback to attach to looper.
    allowCallback = true;
}
   
AndInterface::~AndInterface()
{
    if (inputQueue)
    {
        AInputQueue_detachLooper(inputQueue);
        inputQueue = nullptr;
    }
}
   
void AndInterface::update()
{
    // Process all input events
    if (AInputQueue_hasEvents(inputQueue))
    {
        AInputEvent* event = NULL;
        // TODO: 
        // E/NativeActivity(27972): channel '40a263a0 com.game.Hexagon/android.app.NativeActivity (client)' ~ Failed to receive dispatch signal.  status=-11
        while (AInputQueue_getEvent(inputQueue, &event) >= 0)
        {
            sint32 events = 0;
            sint32 result = ALooper_pollOnce(0, NULL, &events, NULL);

            // Check if event can be processed 
            if (AInputQueue_preDispatchEvent(inputQueue, event))
            {
               continue;
            }

            // Keyboard
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
            {
                // TODO: We don't use Android virtual keyboard.
            }
            else
            // Touch
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)  
            {
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
                    {
                        pressed = true;
                    }

                    // One gesture is ending in this event
                    if ( (touches == InputContext.touchscreen.gestures) && 
                         ( (changedIndex != 0) || (action & AMOTION_EVENT_ACTION_UP) || (action & AMOTION_EVENT_ACTION_OUTSIDE)) )
                    {
                        released = true;
                    }

                    // HW WA: It is possible that device will set wrong "action"
                    //        bits during pressing or releasing touch events in
                    //        multitouch mode. This additional check ensures that
                    //        all released touches will be captured.
                    if (touches < InputContext.touchscreen.gestures)
                    {
                        releasedWA = true;
                        for(uint8 i=0; i<10; ++i)
                        {
                            updatedWA[i] = false;
                        }
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
                        {
                            if (InputContext.touchscreen.gesture[index].second == id)
                            {
                                break;
                            }
                        }

                        // Update existing gesture
                        InputContext.touchscreen.gesture[index].first.touches.push_back(touch);
                        if (releasedWA)
                        {
                            updatedWA[index] = true;
                        }

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
                            {
                                InputContext.touchscreen.gesture[j] = InputContext.touchscreen.gesture[j+1];
                            }
                        }
                    }

                    // Found which gesture wasn't updated in this event and remove it
                    if (releasedWA)
                    {
                        for(uint8 i=0; i<10; ++i)
                        {
                            if (!updatedWA[i])
                            {
                                // Move old gesture to history 
                                // TODO!!!
                                                  
                                // Queue of gestures in progres stores them in order
                                // of time they were started. Therefore shift all 
                                // later gestures one field up.
                                InputContext.touchscreen.gestures--;
                                for(uint8 j=i; j<InputContext.touchscreen.gestures; ++j)
                                {
                                    InputContext.touchscreen.gesture[j] = InputContext.touchscreen.gesture[j+1];
                                }

                                break;
                            }
                        }
                    }
                }

                AInputQueue_finishEvent(InputContext.inputQueue, event, 1);
            }
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
            {
                InputContext.accelerometer.delta.x = 0.0f;
            }
            if (abs(InputContext.accelerometer.delta.y) < InputContext.accelerometer.noise)
            {
                InputContext.accelerometer.delta.y = 0.0f;
            }
            if (abs(InputContext.accelerometer.delta.z) < InputContext.accelerometer.noise)
            {
                InputContext.accelerometer.delta.z = 0.0f;
            }

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
            {
                InputContext.gyroscope.azimuth = 0.0f;
            }
            if (abs(InputContext.gyroscope.pitch) < InputContext.gyroscope.noise)
            {
                InputContext.gyroscope.pitch = 0.0f;
            }
            if (abs(InputContext.gyroscope.roll) < InputContext.gyroscope.noise)
            {
                InputContext.gyroscope.roll = 0.0f;
            }
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
}
   
void AndInterface::update(struct android_app* app, struct android_poll_source* source)
{
    Log << "Touchscreen update called!";
 
    // TODO: Finish
}

} // en::input
} // en

#endif
