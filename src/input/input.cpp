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
#if defined(EN_PLATFORM_OSX)  // New dynamic Interface
#include "input/osxInput.h"
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
#if 0
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

   //joystick.init();
   camera.init();
   hmd.init();

   return true;
   }

   void Context::destroy(void)
   {
   Log << "Closing module: Input." << endl;

   //joystick.destroy();
   camera.destroy();
   hmd.destroy();
   }
#endif




#if 1  // New dynamic Interface



   bool Interface::create(void)
   {
   #if defined(EN_PLATFORM_ANDROID)
   Input = ptr_dynamic_cast<Interface, AndInterface>(Ptr<AndInterface>(new AndInterface()));
   return true;
   #elif defined(EN_PLATFORM_BLACKBERRY)
   Input = ptr_dynamic_cast<Interface, BBInterface>(Ptr<BBInterface>(new BBInterface()));
   return true;
   #elif defined(EN_PLATFORM_IOS)
   Input = ptr_dynamic_cast<Interface, IOSInterface>(Ptr<IOSInterface>(new IOSInterface()));
   return true;
   #elif defined(EN_PLATFORM_OSX)
   Input = ptr_dynamic_cast<Interface, OSXInterface>(Ptr<OSXInterface>(new OSXInterface()));
   return true;
   #elif defined(EN_PLATFORM_WINDOWS)
   Input = ptr_dynamic_cast<Interface, WinInterface>(Ptr<WinInterface>(new WinInterface()));
   return true;
   #else
   // How did we ended up here?
   Input = ptr_dynamic_cast<Interface, CommonInterface>(Ptr<CommonInterface>(new CommonInterface()));
   return false;
   #endif
   }



   // COMMON KEYBOARD
   ////////////////////////////////////////////////////////////////////////////////

   CommonKeyboard::CommonKeyboard() :
      Keyboard()
   {
   // Set all keys to "Released" & "TurnedOff" state
   memset(&keys, 0, sizeof(KeyState) * underlyingType(Key::KeysCount));
   memset(&keyLock, 0, sizeof(KeyLock) * KeyLockStatesCount);
   }
   
   CommonKeyboard::~CommonKeyboard()
   {
   }
   
   bool CommonKeyboard::pressed(const Key key) const
   {
   assert( key != Key::KeysCount );
   return keys[underlyingType(key)] == KeyState::Pressed ? true : false;
   }
   
   // COMMON MOUSE
   ////////////////////////////////////////////////////////////////////////////////

   CommonMouse::CommonMouse() :
      display(nullptr),
      x(0u),
      y(0u),
      Mouse()
   {
   // Set all buttons to "Released"
   memset(&buttons, 0, sizeof(KeyState) * underlyingType(MouseButton::Count));
   }
   
   CommonMouse::~CommonMouse()
   {
   }
 
   Ptr<Screen> CommonMouse::screen(void) const
   {
   return display;
   }
   
   float2 CommonMouse::position(void) const
   {
   assert( display );
   return float2( static_cast<float>(x) / static_cast<float>(display->resolution.x),
                  static_cast<float>(y) / static_cast<float>(display->resolution.y) );
   }
   
   uint32 CommonMouse::position(const Axis axis) const
   {
   return axis == AxisX ? x : y;
   }
   
   bool CommonMouse::pressed(const MouseButton button) const
   {
   assert( button != MouseButton::Count );
   return buttons[underlyingType(button)] == KeyState::Pressed ? true : false;
   }


   // COMMON INTERFACE
   ////////////////////////////////////////////////////////////////////////////////
   
   
   CommonInterface::CommonInterface() :
      Interface()
   {
   // General
   memset(&count, 0, sizeof(uint32) * underlyingType(IO::TypesCount));
   keyboards.clear();
   mouses.clear();
   joysticks.clear();
   hmds.clear();
   controllers.clear();
   cameras.clear();
   
   // Clear callbacks array
   for(uint32 i=0; i<InputEventsCount; ++i)
      callback[i] = &en::state::HandleEventByState;
      
   // TODO: Here init SDK's like OculusSDK, OpenVR, etc.
   
   }

   CommonInterface::~CommonInterface()
   {
   Log << "Closing module: Input." << endl;
   }

   uint8 CommonInterface::available(IO type) const
   {
   assert( type != IO::TypesCount );
   return count[underlyingType(type)];
   }
   
   Ptr<Keyboard> CommonInterface::keyboard(uint8 index) const
   {
   if (index >= count[underlyingType(IO::Keyboard)])
      return Ptr<Keyboard>(nullptr);
      
   return keyboards[index];
   }

   Ptr<Mouse> CommonInterface::mouse(uint8 index) const
   {
   if (index >= count[underlyingType(IO::Mouse)])
      return Ptr<Mouse>(nullptr);
      
   return mouses[index];
   }
   
   Ptr<Joystick> CommonInterface::joystick(uint8 index) const
   {
   if (index >= count[underlyingType(IO::Joystick)])
      return Ptr<Joystick>(nullptr);
      
   return joysticks[index];
   }

   Ptr<HMD> CommonInterface::hmd(uint8 index) const
   {
   if (index >= count[underlyingType(IO::HMD)])
      return Ptr<HMD>(nullptr);
      
   return hmds[index];
   }

   Ptr<Controller> CommonInterface::controller(uint8 index) const
   {
   if (index >= count[underlyingType(IO::Controller)])
      return Ptr<Controller>(nullptr);
      
   return controllers[index];
   }
   
   Ptr<Camera> CommonInterface::camera(uint8 index) const
   {
   if (index >= count[underlyingType(IO::Camera)])
      return Ptr<Camera>(nullptr);
      
   return cameras[index];
   }

   void CommonInterface::update(void)
   {
   // Joystick events
   for(uint8 i=0; i<count[underlyingType(IO::Joystick)]; ++i)
      joysticks[i]->update();

   // VR/AR HMD's update
   for(uint8 i=0; i<count[underlyingType(IO::HMD)]; ++i)
      hmds[i]->update();
      
   // Camera stream events
   for(uint8 i=0; i<count[underlyingType(IO::Camera)]; ++i)
      cameras[i]->update();
   }





#else






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
#endif

   }

#if 1

Ptr<input::Interface> Input;

#else
input::Context   InputContext;
input::Interface Input;
#endif

}
