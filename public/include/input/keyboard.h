/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#ifndef ENG_INPUT_KEYBOARD
#define ENG_INPUT_KEYBOARD

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace input
   {
   // Physical keys
   enum class Key : uint8
      {
      Unknown              = 0,
      A                       , // Letters
      B                       ,
      C                       ,
      D                       ,
      E                       ,
      F                       ,
      G                       ,
      H                       ,
      I                       ,
      J                       ,
      K                       ,
      L                       ,
      M                       ,
      N                       ,
      O                       ,
      P                       ,
      Q                       ,
      R                       ,
      S                       ,
      T                       ,
      U                       ,
      V                       ,
      W                       ,
      X                       ,
      Y                       ,
      Z                       ,
      Cypher1                 , // Cyphers
      Cypher2                 ,
      Cypher3                 ,
      Cypher4                 ,
      Cypher5                 ,
      Cypher6                 ,
      Cypher7                 ,
      Cypher8                 ,
      Cypher9                 ,
      Cypher0                 ,
      Up                      , // Arrows
      Down                    ,
      Left                    ,
      Right                   , // Controlers
      LeftOS                  , // - OSX: LeftCommand   Win: LeftWindows
      RightOS                 , // - OSX: RightCommand  Win: RightWindows
      Alt                     ,
      LeftAlt                 , // - OSX: LeftOption
      RightAlt                , // - OSX: RightOption
      Ctrl                    ,
      LeftCtrl                ,
      RightCtrl               ,
      Shift                   ,
      LeftShift               ,
      RightShift              ,
      Esc                     , // Main keys
      Tab                     ,
      CapsLock                ,
      Backspace               , // - OSX: Delete
      Enter                   , // - OSX: Return
      Space                   ,
      Tilde                   ,
      Minus                   ,
      Equal                   ,
      Baskslash               ,
      OpenBracket             ,
      CloseBracket            ,
      Semicolon               ,
      Quote                   ,
      Comma                   ,
      Period                  ,
      Slash                   ,  
      F1                      , // Function keys
      F2                      ,
      F3                      ,
      F4                      ,
      F5                      ,
      F6                      ,
      F7                      ,
      F8                      ,
      F9                      ,
      F10                     ,
      F11                     ,
      F12                     ,   
      Insert                  , // Text control 
      Delete                  ,
      Home                    ,
      End                     ,
      PageUp                  ,
      PageDown                ,
      NumLock                 , // Numepad
      NumPadSlash             ,
      NumPadAsterisk          ,
      NumPadMinus             ,
      NumPadPlus              ,
      NumPadEnter             ,
      NumPad1                 ,
      NumPad2                 ,
      NumPad3                 ,
      NumPad4                 ,
      NumPad5                 ,
      NumPad6                 ,
      NumPad7                 ,
      NumPad8                 ,
      NumPad9                 ,
      NumPad0                 ,
      PrtScr                  , // Additional
      Pause                   ,
      ScrollLock              ,
      KeysCount
      };

   // Current key state
   enum class KeyState : uint8
      {
      Released             = 0,
      Pressed
      };

   // CapsLock, NumLock, ScrollLock etc. lock state
   enum class KeyLock : uint8
      {
      TurnedOff            = 0,
      TurnedOn                ,
      };
      
   class Keyboard : public SafeObject<Keyboard>
      {
      public:
      virtual bool pressed(const Key key) const = 0;
      
      virtual ~Keyboard() {};                     // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
