/*

 Ngine v5.0
 
 Module      : Input common interface.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_COMMON_INTERFACE
#define ENG_INPUT_COMMON_INTERFACE

#include "input/input.h"
using namespace en::gpu;

#include <vector>
using namespace std;

// Enable/Disable input devices support
#define OCULUS_VR                         1   // Compile with built in support for Oculus VR HMD's
#define OPEN_VR                           0   // Compile with built in support for OpenVR, SteamVR and HTC Vive HMD
#define INTEL_PERCEPTUAL_COMPUTING_2014   0   // Compile with built in support for Intel Depth Cameras
#define MICROSOFT_KINECT                  0   // Compile with built in support for Microsoft Kinect

namespace en
{
   namespace input
   {
   #define KeyLockStatesCount 8
   
   class CommonKeyboard : public Keyboard
      {
      public:
      // Keyboard
      KeyState keys[underlyingType(Key::KeysCount)];  // States of keyboard keys
      KeyLock  keyLock[KeyLockStatesCount];           // CapsLock, NumLock, ScrollLock, etc.
      
      public:
      bool pressed(const Key key) const;

      CommonKeyboard();
      ~CommonKeyboard();                 // Polymorphic deletes require a virtual base destructor
      };

   class CommonMouse : public Mouse
      {
      public:
      Ptr<Screen> display;   // Current screen on which mouse pointer is visible
      KeyState buttons[underlyingType(MouseButton::Count)]; // States of mouse buttons
      uint32 x;
      uint32 y;

      public:
      virtual Ptr<Screen> screen(void) const = 0;
      virtual float2 position(void) const;
      virtual uint32 position(const Axis axis) const;
      virtual bool   position(const uint32 x, const uint32 y) = 0;
      virtual bool   position(const Ptr<Screen> screen, const uint32 x, const uint32 y) = 0;
      virtual bool   pressed(const MouseButton button) const;
      virtual void   show(void) = 0;
      virtual void   hide(void) = 0;

      CommonMouse();
      ~CommonMouse();                 // Polymorphic deletes require a virtual base destructor
      };
      
   class CommonInterface : public Interface
      {
      public:
      // General
      uint32 count[underlyingType(IO::Count)];
      vector< Ptr<Keyboard> > keyboards;
      vector< Ptr<Mouse> > mouses;
      vector< Ptr<Joystick> > joysticks;
      vector< Ptr<HMD> > hmds;
      vector< Ptr<Controller> > controllers;
      vector< Ptr<Camera> > cameras;
      
      // Events
      EventHandlingFuncPtr callback[InputEventsCount];    // Callback per event type
      
      public:
      virtual uint8           available(IO type) const;          // Count of available peripherials of given type
      virtual Ptr<Keyboard>   keyboard(uint8 index = 0) const;   // N'th Keyboard
      virtual Ptr<Mouse>      mouse(uint8 index = 0) const;      // N'th Mouse
      virtual Ptr<Joystick>   joystick(uint8 index = 0) const;   // N'th Joystick
      virtual Ptr<HMD>        hmd(uint8 index = 0) const;        // N'th Head Mounted Display (VR/AR)
      virtual Ptr<Controller> controller(uint8 index = 0) const; // N'th Motion Controller
      virtual Ptr<Camera>     camera(uint8 index = 0) const;     // N'th Camera (Color, Depth, IR, or other)

      virtual void update(void);                                 // Gets actual input state, call function handling cached events
         
      CommonInterface();
      virtual ~CommonInterface();
      };
   }
}
#endif