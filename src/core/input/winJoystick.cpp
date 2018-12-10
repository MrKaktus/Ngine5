/*

Ngine v5.0

Module      : Input controllers and devices.
Requirements: none
Description : Captures and holds signals from
              input devices and interfaces like
              keyboard, mouse or touchscreen.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "core/log/log.h"
#include "core/input/winJoystick.h"
#include "core/input/winInput.h"

namespace en
{
   namespace input
   {
   WinJoystick::WinJoystick(uint32 _id, LPDIRECTINPUTDEVICE8 _handle) :
      id(_id),
      currentState(Initializing),
      handle(_handle)
   {
   for(uint32 i=0; i<128; ++i)
      buttons[i] = KeyState::Released;
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

   WinJoystick::~WinJoystick() 
   {
   }

   bool WinJoystick::on(void)
   {
   return true;
   }

   bool WinJoystick::off(void)
   {
   return true;
   }

   CameraState WinJoystick::state(void) const
   {
   return currentState;
   }

   bool WinJoystick::pressed(const uint8 button) const
   {
   assert( button < 128 );
   return buttons[button] == KeyState::Pressed ? true : false;
   }

   float WinJoystick::position(const en::input::JoystickAxis axis) const
   {
   if (axis == JoystickAxis::X)
      return float(x.position - 32768) / 32768.0f;
   if (axis == JoystickAxis::Y)
      return float(y.position - 32768) / 32768.0f;
   if (axis == JoystickAxis::Z)
      return float(z.position - 32768) / 32768.0f;
   return 0.0f;
   }

void WinJoystick::update(void)
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
    {
        if (hr == DIERR_NOTACQUIRED)
        {
            handle->Acquire();
        }
        else
        {
            Log << "WARNING! Cannot acquire joystick for update!\n";
            return;
        }
    }

    // Check if buffer for joystick events isn't too small
    if (hr == DI_BUFFEROVERFLOW)
    {
        Log << "WARNING! Joystick event buffer too small!\n";
    }

    WinInterface* input = reinterpret_cast<WinInterface*>(en::Input.get());

    for(sint32 e=0; e<elements; ++e)
    {
        DIDEVICEOBJECTDATA* state = &buffer[e];

        if (static_cast<uint32>(state->dwOfs) == DIJOFS_X)
        {
            // Save state for later
            x.position = state->dwData;

            // Call event handling function 
            JoystickMoveEvent* event = new JoystickMoveEvent(id, JoystickAxis::X, static_cast<float>(x.position - 32768) / 32768.0f);
            input->forwardEvent(reinterpret_cast<Event*>(event)); 
        }
        else
        if (static_cast<uint32>(state->dwOfs) == DIJOFS_Y)
        {
            // Save state for later
            y.position = state->dwData;

            // Call event handling function 
            JoystickMoveEvent* event = new JoystickMoveEvent(id, JoystickAxis::Y, static_cast<float>(y.position - 32768) / 32768.0f);
            input->forwardEvent(reinterpret_cast<Event*>(event));  
        }
        else
        if (static_cast<uint32>(state->dwOfs) == DIJOFS_Z)
        {
            // Save state for later
            z.position = state->dwData;

            // Call event handling function 
            JoystickMoveEvent* event = new JoystickMoveEvent(id, JoystickAxis::Z, static_cast<float>(z.position - 32768) / 32768.0f);
            input->forwardEvent(reinterpret_cast<Event*>(event)); 
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
        {
            if (static_cast<uint32>(state->dwOfs) == DIJOFS_BUTTON(j))
            {
                // Save state for later
                buttons[j] = state->dwData & 0x80 ? KeyState::Pressed : KeyState::Released;

                // Call event handling function 
                JoystickButtonEvent* event = new JoystickButtonEvent(id, j);
                event->type = state->dwData & 0x80 ? JoystickButtonPressed : JoystickButtonReleased;
                input->forwardEvent(reinterpret_cast<Event*>(event)); 
            }
        }
    }
}

   }
}
#endif
