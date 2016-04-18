/*

Ngine v5.0

Module      : Input controllers and devices.
Requirements: none
Description : Captures and holds signals from
              input devices and interfaces like
              keyboard, mouse or touchscreen.

*/

#include "core/configuration.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "core/log/log.h"
#include "input/context.h"

namespace en
{
   namespace input
   {
   struct AxisState
      {
      sint32 position;
      sint32 rotation;
      sint32 velocity;
      sint32 angularVelocity;
      sint32 acceleration;
      sint32 angularAcceleration;
      sint32 force;
      sint32 torque;
      };

   class DirectInputJoystick : public Joystick
         {
         public:
         virtual bool        on(void);                // Turns joystick on
         virtual bool        off(void);               // Turns joystick off
         virtual CameraState state(void) const;       // Returns joystick state (on/off/initializing...)
         virtual bool        pressed(const uint8 button) const; // Button state
         virtual float       position(const en::input::JoystickAxis axis) const; // Joystick state
         virtual void        update(void);            // Update camera stream events

         DirectInputJoystick(uint32 _id, LPDIRECTINPUTDEVICE8 _handle);
         virtual ~DirectInputJoystick();              // Polymorphic deletes require a virtual base destructor

         private:
         uint32               id;                     // Joystick id on the devices list
         CameraState          currentState;           // Is it turned on
         LPDIRECTINPUTDEVICE8 handle;                 // Joystick handle
         //DIDEVCAPS            capabilities;           // Joysticks capabilities

         KeyState             buttons[128];           // Buttons state
         AxisState            x;                      // X Axis state
         AxisState            y;                      // Y Axis state
         AxisState            z;                      // Z Axis state
         AxisState            slider[2];              // Sliders state
         uint32               pov[4];                 // POV directions
         };


                 //uint8 count;      // Count of available joysticks
                 //bool used[16];    // Is each joystick used by the application
                 //bool on[16];      // Is each joystick turned on

                 //struct State
                 //       {
 
                 //       } state[16];


   DirectInputJoystick::DirectInputJoystick(uint32 _id, LPDIRECTINPUTDEVICE8 _handle) :
      id(_id),
      currentState(Initializing),
      handle(_handle)
   {
   for(uint32 i=0; i<128; ++i)
      buttons[i] = Released;
   memset(&x, 0, sizeof(Axis));
   memset(&y, 0, sizeof(Axis));
   memset(&z, 0, sizeof(Axis));
   memset(&slider[0], 0, sizeof(Axis));   
   memset(&slider[1], 0, sizeof(Axis)); 
   for(uint32 i=0; i<4; ++i)
      pov[i] = 0;

   x.position = 32768;
   y.position = 32768;
   z.position = 32768;
   }

   DirectInputJoystick::~DirectInputJoystick() 
   {
   }

   bool DirectInputJoystick::on(void)
   {
   return true;
   }

   bool DirectInputJoystick::off(void)
   {
   return true;
   }

   CameraState DirectInputJoystick::state(void) const
   {
   return currentState;
   }

   bool DirectInputJoystick::pressed(const uint8 button) const
   {
   return buttons[button] & 1;
   }

   float DirectInputJoystick::position(const en::input::JoystickAxis axis) const
   {
   if (axis == JoystickAxisX)
      return float(x.position - 32768) / 32768.0f;
   if (axis == JoystickAxisY)
      return float(y.position - 32768) / 32768.0f;
   if (axis == JoystickAxisZ)
      return float(z.position - 32768) / 32768.0f;
   return 0.0f;
   }

   void DirectInputJoystick::update(void)
   {


   //// Joystick events
   //for(uint8 i=0; i<InputContext.joystick.count; ++i)
   //   {
   //   HRESULT hr = InputContext.joystick.handle[i]->Poll(); 

   //   if (FAILED(hr)) 
   //      {
   //      // DInput is telling us that the input stream has been
   //      // interrupted. We aren't tracking any state between polls, 
   //      // so we don't have any special reset that needs to be done. 
   //      // We just re-acquire and try again.
   //      hr = InputContext.joystick.handle[i]->Acquire();
   //      while (hr == DIERR_INPUTLOST) 
   //            hr = InputContext.joystick.handle[i]->Acquire();
   //    
   //      // If we encounter a fatal error, return failure.
   //      if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) 
   //         continue;

   //      // If another application has control of this device, return 
   //      // successfully. We'll just have to wait our turn to use the 
   //      // joystick.
   //      if (hr == DIERR_OTHERAPPHASPRIO) 
   //         continue;
   //      }

   //   // Get the input's device state
   //   DIJOYSTATE2 state;
   //   if (FAILED(hr = InputContext.joystick.handle[i]->GetDeviceState(sizeof(DIJOYSTATE2), &state))) 
   //      continue;

   //   // Update local state
   //   for(uint8 j=0; j<128; ++j)
   //      if (state.rgbButtons[j])
   //         InputContext.joystick.state[i].buttons[j] = Pressed;
   //      else
   //         InputContext.joystick.state[i].buttons[j] = Released;

   //   // X Axis
   //   InputContext.joystick.state[i].x.position            = state.lX;
   //   InputContext.joystick.state[i].x.rotation            = state.lRx;
   //   InputContext.joystick.state[i].x.velocity            = state.lVX;
   //   InputContext.joystick.state[i].x.angularVelocity     = state.lVRx;
   //   InputContext.joystick.state[i].x.acceleration        = state.lAX;
   //   InputContext.joystick.state[i].x.angularAcceleration = state.lARx;
   //   InputContext.joystick.state[i].x.force               = state.lFX;
   //   InputContext.joystick.state[i].x.torque              = state.lFRx;

   //   // Y Axis
   //   InputContext.joystick.state[i].y.position            = state.lY;
   //   InputContext.joystick.state[i].y.rotation            = state.lRy;
   //   InputContext.joystick.state[i].y.velocity            = state.lVY;
   //   InputContext.joystick.state[i].y.angularVelocity     = state.lVRy;
   //   InputContext.joystick.state[i].y.acceleration        = state.lAY;
   //   InputContext.joystick.state[i].y.angularAcceleration = state.lARy;
   //   InputContext.joystick.state[i].y.force               = state.lFY;
   //   InputContext.joystick.state[i].y.torque              = state.lFRy;

   //   // Z Axis
   //   InputContext.joystick.state[i].z.position            = state.lZ;
   //   InputContext.joystick.state[i].z.rotation            = state.lRz;
   //   InputContext.joystick.state[i].z.velocity            = state.lVZ;
   //   InputContext.joystick.state[i].z.angularVelocity     = state.lVRz;
   //   InputContext.joystick.state[i].z.acceleration        = state.lAZ;
   //   InputContext.joystick.state[i].z.angularAcceleration = state.lARz;
   //   InputContext.joystick.state[i].z.force               = state.lFZ;
   //   InputContext.joystick.state[i].z.torque              = state.lFRz;

   //   // Sliders
   //   for(uint8 j=0; j<2; ++j)
   //      {
   //      InputContext.joystick.state[i].slider[j].position     = state.rglSlider[j];
   //      InputContext.joystick.state[i].slider[j].velocity     = state.rglVSlider[j];
   //      InputContext.joystick.state[i].slider[j].acceleration = state.rglASlider[j];
   //      InputContext.joystick.state[i].slider[j].force        = state.rglFSlider[j];
   //      }

   //   InputContext.joystick.state[i].pov[0] = state.rgdwPOV[0];
   //   InputContext.joystick.state[i].pov[1] = state.rgdwPOV[1];
   //   InputContext.joystick.state[i].pov[2] = state.rgdwPOV[2];
   //   InputContext.joystick.state[i].pov[3] = state.rgdwPOV[3];





   // Get buffered events data
   sint32 elements = 64;
   DIDEVICEOBJECTDATA buffer[64]; 
   HRESULT hr;
   while (FAILED(hr = handle->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), buffer, reinterpret_cast<LPDWORD>(&elements), 0)))
         if (hr == DIERR_NOTACQUIRED)
            handle->Acquire();
         else
            {
            Log << "WARNING! Cannot acquire joystick for update!\n";
            return;
            }

   // Check if buffer for joystick events isn't too small
   if (hr == DI_BUFFEROVERFLOW)
      Log << "WARNING! Joystick event buffer too small!\n";

   for(sint32 e=0; e<elements; ++e)
      {
      DIDEVICEOBJECTDATA* state = &buffer[e];

      if (static_cast<uint32>(state->dwOfs) == DIJOFS_X)
         {
         // Save state for later
         x.position = state->dwData;

         // Call event handling function 
         JoystickMoveEvent event;
         event.type     = JoystickMoved;
         event.id       = id;
         event.axis     = JoystickAxisX;
         event.position = static_cast<float>(x.position - 32768) / 32768.0f;
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
         }
      else
      if (static_cast<uint32>(state->dwOfs) == DIJOFS_Y)
         {
         // Save state for later
         y.position = state->dwData;

         // Call event handling function 
         JoystickMoveEvent event;
         event.type     = JoystickMoved;
         event.id       = id;
         event.axis     = JoystickAxisY;
         event.position = static_cast<float>(y.position - 32768) / 32768.0f;
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
         }
      else
      if (static_cast<uint32>(state->dwOfs) == DIJOFS_Z)
         {
         // Save state for later
         z.position = state->dwData;

         // Call event handling function 
         JoystickMoveEvent event;
         event.type     = JoystickMoved;
         event.id       = id;
         event.axis     = JoystickAxisZ;
         event.position = static_cast<float>(z.position - 32768) / 32768.0f;
         InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
         }
      else
      if (static_cast<uint32>(state->dwOfs) == DIJOFS_RX)
         {
         }
      else
      if (static_cast<uint32>(state->dwOfs) == DIJOFS_RY)
         {
         }
      else
      if (static_cast<uint32>(state->dwOfs) == DIJOFS_RZ)
         {
         }
      else
      for(uint32 j=0; j<32; ++j)
         if (static_cast<uint32>(state->dwOfs) == DIJOFS_BUTTON(j))
            {
            // Save state for later
            buttons[j] = state->dwData & 0x80 ? Pressed : Released;

            // Call event handling function 
            JoystickButtonEvent event;
            event.type   = state->dwData & 0x80 ? JoystickButtonPressed : JoystickButtonReleased;
            event.id     = id;
            event.button = j;
            InputContext.events.callback( reinterpret_cast<Event&>(event) ); 
            }
      }
   }

   //# CONTEXT
   //##########################################################################

   //BOOL CALLBACK enumAxesCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context)
   //{
   //HWND hDlg = (HWND)context;

   //// Use default settings

   ////DIPROPRANGE propRange; 
   ////propRange.diph.dwSize       = sizeof(DIPROPRANGE); 
   ////propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
   ////propRange.diph.dwHow        = DIPH_BYID; 
   ////propRange.diph.dwObj        = instance->dwType;
   ////propRange.lMin              = -1000; 
   ////propRange.lMax              = +1000; 
   //// 
   ////// Set the range for the axis
   ////if (FAILED(InputContext.joystick.handle[0]->SetProperty(DIPROP_RANGE, &propRange.diph))) 
   ////   return DIENUM_STOP;

   //return DIENUM_CONTINUE;
   //}

   BOOL CALLBACK initJoystickCallback(const DIDEVICEINSTANCE* instance, VOID* _context)
   {
   LPDIRECTINPUTDEVICE8 handle;
   HRESULT hr;

   // Try to obtain interface to enumerated joystick. 
   // If it fails, it is possible that user unplugged it during enumeration.
   if (FAILED(hr = InputContext.joystick.context->CreateDevice(instance->guidInstance, &handle, nullptr))) 
      return DIENUM_CONTINUE;

   // Specify extended DIJOYSTATE2 structure, as data format, that will be
   // used for receiving data from IDirectInputDevice::GetDeviceState().
   if (FAILED(hr = handle->SetDataFormat(&c_dfDIJoystick2)))
      return DIENUM_CONTINUE;

   //// Acquire joystick exclusively so that it won't be shared with any other
   //// applications or operating system in any way.
   //if (FAILED(hr = handle->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_FOREGROUND))) 
   //   return DIENUM_CONTINUE;

   // Buffer up to 64 state changes of device. This will be used to spawn events
   // to be handled by game.
   DIPROPDWORD  property; 
   property.diph.dwSize       = sizeof(DIPROPDWORD); 
   property.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
   property.diph.dwObj        = 0; 
   property.diph.dwHow        = DIPH_DEVICE; 
   property.dwData            = 64; 
   if (FAILED(hr = handle->SetProperty(DIPROP_BUFFERSIZE, &property.diph)))
      return DIENUM_CONTINUE;

   //// Determine how many axis the joystick has (so we don't error out setting
   //// properties for unavailable axis)
   //joystick.capabilities[i].dwSize = sizeof(DIDEVCAPS);
   //if (FAILED(hr = handle->GetCapabilities(&joystick.capabilities[i])))
   //   return DIENUM_CONTINUE;

   //// Enumerate the axes of the joyctick and set the range of each axis. Note:
   //// we could just use the defaults, but we're just trying to show an example
   //// of enumerating device objects (axes, buttons, etc.).
   //if (FAILED(hr = handle->EnumObjects(enumAxesCallback, NULL, DIDFT_AXIS)))
   //   return DIENUM_CONTINUE;

   InputContext.joystick.device.push_back(new DirectInputJoystick(InputContext.joystick.device.size(), handle));
   return DIENUM_CONTINUE;
   }

   void Context::Joystick::init(void)
   {
   // Create a DirectInput device
   HRESULT hr;
   if (SUCCEEDED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&context, NULL))) 
      // Get handles to available joysticks
      hr = context->EnumDevices(DI8DEVCLASS_GAMECTRL, initJoystickCallback, NULL, DIEDFL_ATTACHEDONLY);
   }

   void Context::Joystick::destroy(void)
   {
   }

   }
}
#endif
