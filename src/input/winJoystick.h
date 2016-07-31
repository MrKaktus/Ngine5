/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none

*/

#ifndef ENG_INPUT_WINDOWS_JOYSTICK
#define ENG_INPUT_WINDOWS_JOYSTICK

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)

#include "input/common.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

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

   class WinJoystick : public Joystick
         {
         public:
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

         virtual bool        on(void);                // Turns joystick on
         virtual bool        off(void);               // Turns joystick off
         virtual CameraState state(void) const;       // Returns joystick state (on/off/initializing...)
         virtual bool        pressed(const uint8 button) const; // Button state
         virtual float       position(const en::input::JoystickAxis axis) const; // Joystick state
         virtual void        update(void);            // Update joystick events

         WinJoystick(uint32 _id, LPDIRECTINPUTDEVICE8 _handle);
         virtual ~WinJoystick();              // Polymorphic deletes require a virtual base destructor
         };
   }
}
#endif

#endif
