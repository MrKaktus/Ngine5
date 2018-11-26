/*

 Ngine v5.0
 
 Module      : Input common interface.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_COMMON_INTERFACE
#define ENG_INPUT_COMMON_INTERFACE

#include "input/input.h"
#include "core/rendering/common/display.h"
using namespace en::gpu;

#include <vector>

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
      std::shared_ptr<CommonDisplay> _display;   // Current screen on which mouse pointer is visible
      KeyState buttons[underlyingType(MouseButton::Count)]; // States of mouse buttons
      uint32 x;
      uint32 y;

      public:
      virtual std::shared_ptr<Display> display(void) const;
      virtual float2 position(void) const;
      virtual uint32 position(const Axis axis) const;
      virtual bool   position(const uint32 x, const uint32 y) = 0;
      virtual bool   position(const std::shared_ptr<Display> display, const uint32 x, const uint32 y) = 0;
      virtual uint32v2 virtualPosition(void) const = 0;     
      virtual bool     virtualPosition(const uint32 x, const uint32 y) = 0;
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
      std::vector< std::shared_ptr<Keyboard> >   keyboards;
      std::vector< std::shared_ptr<Mouse> >      mouses;
      std::vector< std::shared_ptr<Joystick> >   joysticks;
      std::vector< std::shared_ptr<HMD> >        hmds;
      std::vector< std::shared_ptr<Controller> > controllers;
      std::vector< std::shared_ptr<Camera> >     cameras;
      
      // Events
      EventHandlingFuncPtr callback[InputEventsCount];    // Callback per event type
      
      public:
      virtual uint8           available(IO type) const;          // Count of available peripherials of given type
      virtual std::shared_ptr<Keyboard>   keyboard(uint8 index = 0) const;   // N'th Keyboard
      virtual std::shared_ptr<Mouse>      mouse(uint8 index = 0) const;      // N'th Mouse
      virtual std::shared_ptr<Joystick>   joystick(uint8 index = 0) const;   // N'th Joystick
      virtual std::shared_ptr<HMD>        hmd(uint8 index = 0) const;        // N'th Head Mounted Display (VR/AR)
      virtual std::shared_ptr<Controller> controller(uint8 index = 0) const; // N'th Motion Controller
      virtual std::shared_ptr<Camera>     camera(uint8 index = 0) const;     // N'th Camera (Color, Depth, IR, or other)

      virtual void update(void);                                 // Gets actual input state, call function handling cached events
         
      CommonInterface();
      virtual void init(void);
      virtual ~CommonInterface();
      };
   }
}
#endif
