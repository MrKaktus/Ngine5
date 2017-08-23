

#include "core/configuration.h"


#if defined(EN_PLATFORM_OSX)
#import <AppKit/AppKit.h>
#import <CoreGraphics/CGDirectDisplay.h>   // Mouse - OSX Screen Placement
#include "platform/osx/AppDelegate.h"

#include "utilities/utilities.h"
#include "input/osxInput.h"
#include "core/rendering/metal/mtlDisplay.h" // Mouse - OSX Screen reference


// Common
#include "scene/context.h"   // Default Events handling


namespace en
{
   namespace input
   {
   // 0xF700 - 0xF747 unicode characters mapped to 0x00 - 0x47 range
   const Key TranslateFunctionKey[0x48] = 
      {
      Key::Up,         // NSUpArrowFunctionKey        = 0xF700,
      Key::Down,       // NSDownArrowFunctionKey      = 0xF701,
      Key::Left,       // NSLeftArrowFunctionKey      = 0xF702,
      Key::Right,      // NSRightArrowFunctionKey     = 0xF703,
      Key::F1,         // NSF1FunctionKey             = 0xF704,
      Key::F2,         // NSF2FunctionKey             = 0xF705,
      Key::F3,         // NSF3FunctionKey             = 0xF706,
      Key::F4,         // NSF4FunctionKey             = 0xF707,
      Key::F5,         // NSF5FunctionKey             = 0xF708,
      Key::F6,         // NSF6FunctionKey             = 0xF709,
      Key::F7,         // NSF7FunctionKey             = 0xF70A,
      Key::F8,         // NSF8FunctionKey             = 0xF70B,
      Key::F9,         // NSF9FunctionKey             = 0xF70C,
      Key::F10,        // NSF10FunctionKey            = 0xF70D,
      Key::F11,        // NSF11FunctionKey            = 0xF70E,
      Key::F12,        // NSF12FunctionKey            = 0xF70F,
      Key::Unknown,    // NSF13FunctionKey            = 0xF710,
      Key::Unknown,    // NSF14FunctionKey            = 0xF711,
      Key::Unknown,    // NSF15FunctionKey            = 0xF712,
      Key::Unknown,    // NSF16FunctionKey            = 0xF713,
      Key::Unknown,    // NSF17FunctionKey            = 0xF714,
      Key::Unknown,    // NSF18FunctionKey            = 0xF715,
      Key::Unknown,    // NSF19FunctionKey            = 0xF716,
      Key::Unknown,    // NSF20FunctionKey            = 0xF717,
      Key::Unknown,    // NSF21FunctionKey            = 0xF718,
      Key::Unknown,    // NSF22FunctionKey            = 0xF719,
      Key::Unknown,    // NSF23FunctionKey            = 0xF71A,
      Key::Unknown,    // NSF24FunctionKey            = 0xF71B,
      Key::Unknown,    // NSF25FunctionKey            = 0xF71C,
      Key::Unknown,    // NSF26FunctionKey            = 0xF71D,
      Key::Unknown,    // NSF27FunctionKey            = 0xF71E,
      Key::Unknown,    // NSF28FunctionKey            = 0xF71F,
      Key::Unknown,    // NSF29FunctionKey            = 0xF720,
      Key::Unknown,    // NSF30FunctionKey            = 0xF721,
      Key::Unknown,    // NSF31FunctionKey            = 0xF722,
      Key::Unknown,    // NSF32FunctionKey            = 0xF723,
      Key::Unknown,    // NSF33FunctionKey            = 0xF724,
      Key::Unknown,    // NSF34FunctionKey            = 0xF725,
      Key::Unknown,    // NSF35FunctionKey            = 0xF726,
      Key::Insert,     // NSInsertFunctionKey         = 0xF727,
      Key::Delete,     // NSDeleteFunctionKey         = 0xF728,
      Key::Home,       // NSHomeFunctionKey           = 0xF729,
      Key::Unknown,    // NSBeginFunctionKey          = 0xF72A,
      Key::End,        // NSEndFunctionKey            = 0xF72B,
      Key::PageUp,     // NSPageUpFunctionKey         = 0xF72C,
      Key::PageDown,   // NSPageDownFunctionKey       = 0xF72D,
      Key::PrtScr,     // NSPrintScreenFunctionKey    = 0xF72E,
      Key::ScrollLock, // NSScrollLockFunctionKey     = 0xF72F,
      Key::Pause,      // NSPauseFunctionKey          = 0xF730,
      Key::Unknown,    // NSSysReqFunctionKey         = 0xF731,
      Key::Unknown,    // NSBreakFunctionKey          = 0xF732,
      Key::Unknown,    // NSResetFunctionKey          = 0xF733,
      Key::Unknown,    // NSStopFunctionKey           = 0xF734,
      Key::Unknown,    // NSMenuFunctionKey           = 0xF735,
      Key::Unknown,    // NSUserFunctionKey           = 0xF736,
      Key::Unknown,    // NSSystemFunctionKey         = 0xF737,
      Key::Unknown,    // NSPrintFunctionKey          = 0xF738,
      Key::Unknown,    // NSClearLineFunctionKey      = 0xF739,
      Key::Unknown,    // NSClearDisplayFunctionKey   = 0xF73A,
      Key::Unknown,    // NSInsertLineFunctionKey     = 0xF73B,
      Key::Unknown,    // NSDeleteLineFunctionKey     = 0xF73C,
      Key::Unknown,    // NSInsertCharFunctionKey     = 0xF73D,
      Key::Unknown,    // NSDeleteCharFunctionKey     = 0xF73E,
      Key::Unknown,    // NSPrevFunctionKey           = 0xF73F,
      Key::Unknown,    // NSNextFunctionKey           = 0xF740,
      Key::Unknown,    // NSSelectFunctionKey         = 0xF741,
      Key::Unknown,    // NSExecuteFunctionKey        = 0xF742,
      Key::Unknown,    // NSUndoFunctionKey           = 0xF743,
      Key::Unknown,    // NSRedoFunctionKey           = 0xF744,
      Key::Unknown,    // NSFindFunctionKey           = 0xF745,
      Key::Unknown,    // NSHelpFunctionKey           = 0xF746,
      Key::Unknown     // NSModeSwitchFunctionKey     = 0xF747
      };
      
   const Key TranslateKey[256] =
      {
      Key::Unknown,      // 0x00
      Key::Unknown,      // 0x01
      Key::Unknown,      // 0x02
      Key::Unknown,      // 0x03 - Enter
      Key::Unknown,      // 0x04
      Key::Unknown,      // 0x05
      Key::Unknown,      // 0x06
      Key::Unknown,      // 0x07
      Key::Backspace,    // 0x08
      Key::Tab,          // 0x09
      Key::Unknown,      // 0x0A - NewLine
      Key::Unknown,      // 0x0B
      Key::Unknown,      // 0x0C - FormFeed
      Key::Enter,        // 0x0D - CarriageReturn
      Key::Unknown,      // 0x0E
      Key::Unknown,      // 0x0F
      Key::Shift,        // 0x10
      Key::Ctrl,         // 0x11
      Key::Alt,          // 0x12
      Key::Pause,        // 0x13
      Key::CapsLock,     // 0x14
      Key::Unknown,      // 0x15
      Key::Unknown,      // 0x16
      Key::Unknown,      // 0x17
      Key::Unknown,      // 0x18
      Key::Unknown,      // 0x19 - BackTab
      Key::Unknown,      // 0x1A
      Key::Esc,          // 0x1B
      Key::Unknown,      // 0x1C
      Key::Unknown,      // 0x1D
      Key::Unknown,      // 0x1E
      Key::Unknown,      // 0x1F
      Key::Space,        // 0x20
      Key::PageUp,       // 0x21
      Key::PageDown,     // 0x22
      Key::End,          // 0x23
      Key::Home,         // 0x24
      Key::Left,         // 0x25
      Key::Up,           // 0x26
      Key::Right,        // 0x27
      Key::Down,         // 0x28
      Key::Unknown,      // 0x29 
      Key::Unknown,      // 0x2A 
      Key::Unknown,      // 0x2B 
      Key::PrtScr,       // 0x2C
      Key::Insert,       // 0x2D            
      Key::Delete,       // 0x2E  
      Key::Unknown,      // 0x2F                     
      Key::Cypher0,      // 0x30         
      Key::Cypher1,      // 0x31                 
      Key::Cypher2,      // 0x32                 
      Key::Cypher3,      // 0x33                 
      Key::Cypher4,      // 0x34                 
      Key::Cypher5,      // 0x35                 
      Key::Cypher6,      // 0x36                 
      Key::Cypher7,      // 0x37                 
      Key::Cypher8,      // 0x38                 
      Key::Cypher9,      // 0x39
      Key::Unknown,      // 0x3A 
      Key::Unknown,      // 0x3B 
      Key::Unknown,      // 0x3C 
      Key::Unknown,      // 0x3D 
      Key::Unknown,      // 0x3E 
      Key::Unknown,      // 0x3F 
      Key::Unknown,      // 0x40                      
      Key::A,            // 0x41                 
      Key::B,            // 0x42                  
      Key::C,            // 0x43                  
      Key::D,            // 0x44                  
      Key::E,            // 0x45                  
      Key::F,            // 0x46                  
      Key::G,            // 0x47                  
      Key::H,            // 0x48                  
      Key::I,            // 0x49                  
      Key::J,            // 0x4A                  
      Key::K,            // 0x4B                  
      Key::L,            // 0x4C                  
      Key::M,            // 0x4D                  
      Key::N,            // 0x4E                  
      Key::O,            // 0x4F                  
      Key::P,            // 0x50                  
      Key::Q,            // 0x51                  
      Key::R,            // 0x52                  
      Key::S,            // 0x53                  
      Key::T,            // 0x54                  
      Key::U,            // 0x55                  
      Key::V,            // 0x56                  
      Key::W,            // 0x57                  
      Key::X,            // 0x58                  
      Key::Y,            // 0x59                  
      Key::Z,            // 0x5A    
      Key::Unknown,      // 0x5B 
      Key::Unknown,      // 0x5C 
      Key::Unknown,      // 0x5D 
      Key::Unknown,      // 0x5E 
      Key::Unknown,      // 0x5F 
      Key::NumPad0,      // 0x60                          
      Key::NumPad1,      // 0x61            
      Key::NumPad2,      // 0x62            
      Key::NumPad3,      // 0x63            
      Key::NumPad4,      // 0x64            
      Key::NumPad5,      // 0x65            
      Key::NumPad6,      // 0x66            
      Key::NumPad7,      // 0x67            
      Key::NumPad8,      // 0x68            
      Key::NumPad9,      // 0x69 
      Key::Unknown,      // 0x6A 
      Key::Unknown,      // 0x6B 
      Key::Unknown,      // 0x6C 
      Key::Unknown,      // 0x6D 
      Key::Unknown,      // 0x6E 
      Key::Unknown,      // 0x6F 
      Key::F1,           // 0x70                 
      Key::F2,           // 0x71                 
      Key::F3,           // 0x72                 
      Key::F4,           // 0x73                 
      Key::F5,           // 0x74                 
      Key::F6,           // 0x75                 
      Key::F7,           // 0x76                 
      Key::F8,           // 0x77                 
      Key::F9,           // 0x78                 
      Key::F10,          // 0x79                 
      Key::F11,          // 0x7A                 
      Key::F12,          // 0x7B  
      Key::Unknown,      // 0x7C 
      Key::Unknown,      // 0x7D 
      Key::Unknown,      // 0x7E 
      Key::Unknown,      // 0x7F - Delete
      Key::Unknown,      // 0x80 
      Key::Unknown,      // 0x81 
      Key::Unknown,      // 0x82 
      Key::Unknown,      // 0x83 
      Key::Unknown,      // 0x84 
      Key::Unknown,      // 0x85
      Key::Unknown,      // 0x86 
      Key::Unknown,      // 0x87 
      Key::Unknown,      // 0x88 
      Key::Unknown,      // 0x89
      Key::Unknown,      // 0x8A 
      Key::Unknown,      // 0x8B 
      Key::Unknown,      // 0x8C 
      Key::Unknown,      // 0x8D 
      Key::Unknown,      // 0x8E 
      Key::Unknown,      // 0x8F 
      Key::NumLock,      // 0x90                          
      Key::ScrollLock,   // 0x91 
      Key::Unknown,      // 0x92 
      Key::Unknown,      // 0x93 
      Key::Unknown,      // 0x94 
      Key::Unknown,      // 0x95
      Key::Unknown,      // 0x96 
      Key::Unknown,      // 0x97 
      Key::Unknown,      // 0x98 
      Key::Unknown,      // 0x99
      Key::Unknown,      // 0x9A 
      Key::Unknown,      // 0x9B 
      Key::Unknown,      // 0x9C 
      Key::Unknown,      // 0x9D 
      Key::Unknown,      // 0x9E 
      Key::Unknown,      // 0x9F          
      Key::LeftShift,    // 0xA0   
      Key::RightShift,   // 0xA1 
      Key::LeftCtrl,     // 0xA2   
      Key::RightCtrl,    // 0xA3             
      Key::LeftAlt,      // 0xA4   
      Key::RightAlt,     // 0xA5   
      Key::Unknown,      // 0xA6 
      Key::Unknown,      // 0xA7 
      Key::Unknown,      // 0xA8 
      Key::Unknown,      // 0xA9
      Key::Unknown,      // 0xAA 
      Key::Unknown,      // 0xAB 
      Key::Unknown,      // 0xAC 
      Key::Unknown,      // 0xAD 
      Key::Unknown,      // 0xAE 
      Key::Unknown,      // 0xAF
      Key::Unknown,      // 0xB0 
      Key::Unknown,      // 0xB1    
      Key::Unknown,      // 0xB2 
      Key::Unknown,      // 0xB3 
      Key::Unknown,      // 0xB4 
      Key::Unknown,      // 0xB5
      Key::Unknown,      // 0xB6 
      Key::Unknown,      // 0xB7 
      Key::Unknown,      // 0xB8 
      Key::Unknown,      // 0xB9             
      Key::Semicolon,    // 0xBA
      Key::Equal,        // 0xBB 
      Key::Comma,        // 0xBC 
      Key::Minus,        // 0xBD           
      Key::Period,       // 0xBE              
      Key::Slash,        // 0xBF 
      Key::Tilde,        // 0xC0 
      Key::Unknown,      // 0xC1    
      Key::Unknown,      // 0xC2 
      Key::Unknown,      // 0xC3 
      Key::Unknown,      // 0xC4 
      Key::Unknown,      // 0xC5
      Key::Unknown,      // 0xC6 
      Key::Unknown,      // 0xC7 
      Key::Unknown,      // 0xC8 
      Key::Unknown,      // 0xC9                
      Key::Unknown,      // 0xCA 
      Key::Unknown,      // 0xCB 
      Key::Unknown,      // 0xCC 
      Key::Unknown,      // 0xCD 
      Key::Unknown,      // 0xCE 
      Key::Unknown,      // 0xCF
      Key::Unknown,      // 0xD0
      Key::Unknown,      // 0xD1    
      Key::Unknown,      // 0xD2 
      Key::Unknown,      // 0xD3 
      Key::Unknown,      // 0xD4 
      Key::Unknown,      // 0xD5
      Key::Unknown,      // 0xD6 
      Key::Unknown,      // 0xD7 
      Key::Unknown,      // 0xD8 
      Key::Unknown,      // 0xD9                
      Key::Unknown,      // 0xDA 
      Key::OpenBracket,  // 0xDB
      Key::Unknown,      // 0xDC        
      Key::CloseBracket, // 0xDD
      Key::Quote,        // 0xDE     
      Key::Unknown,      // 0xDF
      Key::Unknown,      // 0xE0
      Key::Unknown,      // 0xE1          
      Key::Baskslash,    // 0xE2  
      Key::Unknown,      // 0xE3 
      Key::Unknown,      // 0xE4 
      Key::Unknown,      // 0xE5
      Key::Unknown,      // 0xE6 
      Key::Unknown,      // 0xE7 
      Key::Unknown,      // 0xE8 
      Key::Unknown,      // 0xE9                
      Key::Unknown,      // 0xEA 
      Key::Unknown,      // 0xEB 
      Key::Unknown,      // 0xEC 
      Key::Unknown,      // 0xED 
      Key::Unknown,      // 0xEE 
      Key::Unknown,      // 0xEF
      Key::Unknown,      // 0xF0
      Key::Unknown,      // 0xF1    
      Key::Unknown,      // 0xF2 
      Key::Unknown,      // 0xF3 
      Key::Unknown,      // 0xF4 
      Key::Unknown,      // 0xF5
      Key::Unknown,      // 0xF6 
      Key::Unknown,      // 0xF7 
      Key::Unknown,      // 0xF8 
      Key::Unknown,      // 0xF9                
      Key::Unknown,      // 0xFA             
      Key::Unknown,      // 0xFB 
      Key::Unknown,      // 0xFC 
      Key::Unknown,      // 0xFD 
      Key::Unknown,      // 0xFE 
      Key::Unknown       // 0xFF
      };

   // Note that these relate to physical keyboard locations, not the key caps that are printed.
   // For example, a French keyboard has the Q and A keys flipped, and Key::A is the Key::Q.
   const Key TranslateCarbonKey[128] =
      {
      Key::A,              // 0x00
      Key::S,              // 0x01
      Key::D,              // 0x02
      Key::F,              // 0x03
      Key::H,              // 0x04
      Key::G,              // 0x05
      Key::Z,              // 0x06
      Key::X,              // 0x07
      Key::C,              // 0x08
      Key::V,              // 0x09
      Key::Unknown,        // 0x0A
      Key::B,              // 0x0B
      Key::Q,              // 0x0C
      Key::W,              // 0x0D
      Key::E,              // 0x0E
      Key::R,              // 0x0F
      Key::Y,              // 0x10
      Key::T,              // 0x11
      Key::Cypher1,        // 0x12
      Key::Cypher2,        // 0x13
      Key::Cypher3,        // 0x14
      Key::Cypher4,        // 0x15
      Key::Cypher6,        // 0x16
      Key::Cypher5,        // 0x17
      Key::Unknown,        // 0x18 - NumPad Equal
      Key::Cypher9,        // 0x19
      Key::Cypher7,        // 0x1A
      Key::Minus,          // 0x1B
      Key::Cypher8,        // 0x1C
      Key::Cypher0,        // 0x1D
      Key::CloseBracket,   // 0x1E
      Key::O,              // 0x1F
      Key::U,              // 0x20
      Key::OpenBracket,    // 0x21
      Key::I,              // 0x22
      Key::P,              // 0x23
      Key::Enter,          // 0x24
      Key::L,              // 0x25
      Key::J,              // 0x26
      Key::Quote,          // 0x27
      Key::K,              // 0x28
      Key::Semicolon,      // 0x29
      Key::Baskslash,      // 0x2A
      Key::Comma,          // 0x2B
      Key::Slash,          // 0x2C
      Key::N,              // 0x2D
      Key::M,              // 0x2E
      Key::Period,         // 0x2F
      Key::Tab,            // 0x30
      Key::Space,          // 0x31
      Key::Tilde,          // 0x32
      Key::Backspace,      // 0x33
      Key::Unknown,        // 0x34
      Key::Esc,            // 0x35
      Key::RightOS,        // 0x36
      Key::LeftOS,         // 0x37
      Key::LeftShift,      // 0x38
      Key::Unknown,        // 0x39
      Key::LeftAlt,        // 0x3A
      Key::LeftCtrl,       // 0x3B
      Key::RightShift,     // 0x3C
      Key::RightAlt,       // 0x3D
      Key::RightCtrl,      // 0x3E
      Key::Unknown,        // 0x3F
      Key::Unknown,        // 0x40 - F17
      Key::Period,         // 0x41
      Key::Unknown,        // 0x42
      Key::NumPadAsterisk, // 0x43
      Key::Unknown,        // 0x44
      Key::NumPadPlus,     // 0x45
      Key::Unknown,        // 0x46
      Key::Unknown,        // 0x47
      Key::Unknown,        // 0x48
      Key::Unknown,        // 0x49
      Key::Unknown,        // 0x4A
      Key::NumPadSlash,    // 0x4B
      Key::NumPadEnter,    // 0x4C
      Key::Unknown,        // 0x4D
      Key::NumPadMinus,    // 0x4E
      Key::Unknown,        // 0x4F - F18
      Key::Unknown,        // 0x50 - F19
      Key::Unknown,        // 0x51
      Key::NumPad0,        // 0x52
      Key::NumPad1,        // 0x53
      Key::NumPad2,        // 0x54
      Key::NumPad3,        // 0x55
      Key::NumPad4,        // 0x56
      Key::NumPad5,        // 0x57
      Key::NumPad6,        // 0x58
      Key::NumPad7,        // 0x59
      Key::Unknown,        // 0x5A
      Key::NumPad8,        // 0x5B
      Key::NumPad9,        // 0x5C
      Key::Unknown,        // 0x5D
      Key::Unknown,        // 0x5E
      Key::Unknown,        // 0x5F
      Key::F5,             // 0x60
      Key::F6,             // 0x61
      Key::F7,             // 0x62
      Key::F3,             // 0x63
      Key::F8,             // 0x64
      Key::Unknown,        // 0x65
      Key::Unknown,        // 0x66
      Key::Unknown,        // 0x67
      Key::Unknown,        // 0x68
      Key::Unknown,        // 0x69 - F13
      Key::Unknown,        // 0x6A - F16
      Key::Unknown,        // 0x6B - F14
      Key::Unknown,        // 0x6C
      Key::Unknown,        // 0x6D
      Key::Unknown,        // 0x6E
      Key::Unknown,        // 0x6F
      Key::Unknown,        // 0x70
      Key::Unknown,        // 0x71 - F15
      Key::Unknown,        // 0x72 - Help
      Key::Home,           // 0x73
      Key::PageUp,         // 0x74
      Key::Delete,         // 0x75
      Key::F4,             // 0x76
      Key::End,            // 0x77
      Key::F2,             // 0x78
      Key::PageDown,       // 0x79
      Key::F1,             // 0x7A
      Key::Left,           // 0x7B
      Key::Right,          // 0x7C
      Key::Down,           // 0x7D
      Key::Up,             // 0x7E
      Key::Unknown,        // 0x7F
      };

   OSXInterface::OSXInterface() :
      CommonInterface()
   {
   // Register keyboard
   keyboards.push_back(Ptr<Keyboard>(new CommonKeyboard()));
   count[underlyingType(IO::Keyboard)]++;
   stateFlags = 0;
   
   // Register mouse (or Touchpad pretending to be mouse)
   mouses.push_back(Ptr<Mouse>(new OSXMouse()));
   count[underlyingType(IO::Mouse)]++;
   
   // Register touchpad
   // TODO: Detect MacBook or Magic Touch Pad and add it's support
   // touchpads.push_back(Ptr<TouchPad>(new OSXTouchPad()));
   // count[underlyingType(IO::TouchPad)]++;
   }
   
   OSXInterface::~OSXInterface()
   {
   }

   void OSXInterface::update()
   {
   // Drain system events
   NSEvent* event = nil;
   do
   {
      // Queries incoming events until all are done (doesn't sleep the thread)
      event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                 untilDate:[NSDate distantPast]
                                    inMode:NSDefaultRunLoopMode
                                   dequeue:YES];

      // ProcessDefferedEvents
      // ProcessEvent
      // FMacApplication::ProcessKeyDownEvent
      
      // Unicode Characters
      // NSLineSeparatorCharacter      = 0x2028,
      // NSParagraphSeparatorCharacter = 0x2029
   
      // NSDeleteCharacter:
      // case 0x1b:             // EKeys::Escape
   
      switch([event type])
         {
         // Mouse Events
         // https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/EventOverview/HandlingMouseEvents/HandlingMouseEvents.html#//apple_ref/doc/uid/10000060i-CH6
         case NSEventTypeLeftMouseDown:
            {
            ptr_dynamic_cast<CommonMouse, Mouse>(mouses[0])->buttons[underlyingType(MouseButton::Left)] = KeyState::Pressed;

            // Call event handling function
            MouseEvent outEvent(MouseButtonPressed);
            outEvent.button = MouseButton::Left;
            callback[outEvent.type]( reinterpret_cast<Event&>(outEvent) );
            break;
            }
            
         case NSEventTypeLeftMouseUp:
            {
            ptr_dynamic_cast<CommonMouse, Mouse>(mouses[0])->buttons[underlyingType(MouseButton::Left)] = KeyState::Released;
            
            // Call event handling function
            MouseEvent outEvent(MouseButtonReleased);
            outEvent.button = MouseButton::Left;
            callback[outEvent.type]( reinterpret_cast<Event&>(outEvent) );
            break;
            }
            
         case NSEventTypeRightMouseDown:
            {
            ptr_dynamic_cast<CommonMouse, Mouse>(mouses[0])->buttons[underlyingType(MouseButton::Right)] = KeyState::Pressed;

            // Call event handling function
            MouseEvent outEvent(MouseButtonPressed);
            outEvent.button = MouseButton::Right;
            callback[outEvent.type]( reinterpret_cast<Event&>(outEvent) );
            break;
            }
            
         case NSEventTypeRightMouseUp:
            {
            ptr_dynamic_cast<CommonMouse, Mouse>(mouses[0])->buttons[underlyingType(MouseButton::Right)] = KeyState::Released;
            
            // Call event handling function
            MouseEvent outEvent(MouseButtonReleased);
            outEvent.button = MouseButton::Right;
            callback[outEvent.type]( reinterpret_cast<Event&>(outEvent) );
            break;
            }
            
         case NSEventTypeMouseMoved:
            {
            Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(mouses[0u]);
               
            NSPoint eventLocation = [event locationInWindow];

            // TODO: Did mouse moved to new screen ?
            // - if so, update screen pointer it lays on
            
            // WA: FIXME: For now, always position on main display.
            mouse->_display = ptr_dynamic_cast<CommonDisplay, Display>(Graphics->primaryDisplay()); // CGMainDisplayID();
            
            
            // Requires mouse position in global screen coordinates, we get it in window coordinates.
            //CGError error;
            //error = CGGetDisplaysWithPoint( CGPoint point, uint32_t maxDisplays, CGDirectDisplayID *displays, uint32_t *matchingDisplayCount );
            
            // TODO: Did mouse moved over another window?
            // - if so, and position is returned relative to that window, calculate screen position taking this into notice
            if ([event window] == nil)
               {
               // Convert event location to per pixel location on the screen on which mouse is located
               NSScreen* handle = ptr_dynamic_cast<DisplayMTL, CommonDisplay>(mouse->_display)->handle;
               NSRect frame = NSMakeRect(eventLocation.x, (eventLocation.y - 1.0f), 0.0f, 0.0f);
               NSRect info = [handle convertRectToBacking:frame];  // [handle frame] - Screen resolution
               mouse->x = static_cast<uint32>(info.origin.x);
               mouse->y = static_cast<uint32>(info.origin.y);
               }
            else
               {
               // Convert event location to per pixel location on the screen on which mouse is located (from current window)
               // TODO . . .
               }
               
            // Call event handling function
            MouseEvent outEvent(MouseMoved);
            outEvent.x = static_cast<uint16>(mouse->x);
            outEvent.y = static_cast<uint16>(mouse->y);
            callback[outEvent.type]( reinterpret_cast<Event&>(outEvent) );
            break;
            }
            
         case NSEventTypeLeftMouseDragged:
         case NSEventTypeRightMouseDragged:
         case NSEventTypeMouseEntered:
         case NSEventTypeMouseExited:
            [NSApp sendEvent:event];
            break;
            
         case NSEventTypeKeyDown:
            {
            // Pass control over key combinations like Cmd-Q, Cmd-S to OS
            //if ([self checkForSystemMnemonic: event])
            //   break;
               
            // Engine doesn't support "repeating hold key"
            if ([event isARepeat]) // [event ARepeat]
               break;
               
            } // Continue in shared decoding section.
         case NSEventTypeKeyUp:
            {
            // There are 3 types of input:
            // - Unicode Characters - [Event characters]
            // - Unicode Characters cleaned from special ones - [event charactersIgnoringModifiers]
            // - Key codes - [Event keyCode]
            
            // If there is no real char due to typing multi-char, for eg. "Ã«"
            if ([[event characters] length] == 0)
               break;
             
            uint16 unicodeKey = static_cast<uint16>([[event charactersIgnoringModifiers] characterAtIndex:0]);
            
            Key key = Key::Unknown;
            
            // Translate unicode of special function key to enum
            if ( (unicodeKey >= 0xF700) &&
                 (unicodeKey <= 0xF747) )
               {
               key = TranslateFunctionKey[unicodeKey - 0xF700];
               }
            else // Translate traditional ASCII keycodes
            if ( (unicodeKey < 0xFF) &&
                 (key == Key::Unknown) )
               {
               key = TranslateKey[unicodeKey & 0xFF];
               }
            else // Translate Carbon physical keycodes
            if ( (unicodeKey < 0x80) &&
                 (key == Key::Unknown) )
               {
               key = TranslateCarbonKey[unicodeKey & 0xFF];
               }
   
            // Send event only for recognized keys
            if (key != Key::Unknown)
               {
               KeyboardEvent outEvent(key);
               
               if ([event type] == NSEventTypeKeyDown)
                  {
                  ptr_reinterpret_cast<CommonKeyboard>(&keyboards[0])->keys[underlyingType(key)] = KeyState::Pressed;
                  outEvent.type = KeyPressed;
                  }
               else
               if ([event type] == NSEventTypeKeyUp)
                  {
                  ptr_reinterpret_cast<CommonKeyboard>(&keyboards[0])->keys[underlyingType(key)] = KeyState::Released;
                  outEvent.type = KeyReleased;
                  }
   
               // Call event handling function
               callback[outEvent.type]( reinterpret_cast<Event&>(outEvent) );
               }
            }
            break;
    
// enum class StateKey : uint8
//    {
//    CapsLock     = 0,
//    Shift           ,
//    Ctrl            ,
//    Alt             ,
//    Windows         ,
//    NumPad          ,
//    Help            ,
//    Fn              ,
//    };
         
         // State key changed (CapsLock, NumLock etc.)
         case NSEventTypeFlagsChanged:
            {
            // Each key state change is reported as separate system event
            uint32 newStateFlags = static_cast<uint32>([event modifierFlags]);
            
            Ptr<CommonKeyboard> ptr = ptr_dynamic_cast<CommonKeyboard, Keyboard>(keyboards[0]);
            
            // CapsLock
            if (bitsChanged(newStateFlags, stateFlags, NSAlphaShiftKeyMask))
               ptr->keyLock[0] = checkBitmask(newStateFlags, NSAlphaShiftKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            // Shift
            if (bitsChanged(newStateFlags, stateFlags, NSShiftKeyMask))
               ptr->keyLock[1] = checkBitmask(newStateFlags, NSShiftKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            // Ctrl
            if (bitsChanged(newStateFlags, stateFlags, NSControlKeyMask))
               ptr->keyLock[2] = checkBitmask(newStateFlags, NSControlKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            // Alt / Option
            if (bitsChanged(newStateFlags, stateFlags, NSAlternateKeyMask))
               ptr->keyLock[3] = checkBitmask(newStateFlags, NSAlternateKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            // Windows / Command
            if (bitsChanged(newStateFlags, stateFlags, NSCommandKeyMask))
               ptr->keyLock[4] = checkBitmask(newStateFlags, NSCommandKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            // NumPad
            if (bitsChanged(newStateFlags, stateFlags, NSNumericPadKeyMask))
               ptr->keyLock[5] = checkBitmask(newStateFlags, NSNumericPadKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            //
            if (bitsChanged(newStateFlags, stateFlags, NSHelpKeyMask))
               ptr->keyLock[6] = checkBitmask(newStateFlags, NSHelpKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            else
            // Fn
            if (bitsChanged(newStateFlags, stateFlags, NSFunctionKeyMask))
               ptr->keyLock[7] = checkBitmask(newStateFlags, NSFunctionKeyMask) ? KeyLock::TurnedOn : KeyLock::TurnedOff;
            
            // TODO: Should keyPressed / keyReleased event be spawned ? Or will there be separate event from OSX for that ?
            
            stateFlags = newStateFlags;
            }
            break;
            
         case NSEventTypeAppKitDefined:
         case NSEventTypeSystemDefined:
         case NSEventTypeApplicationDefined:
         case NSEventTypePeriodic:
         case NSEventTypeCursorUpdate:
         case NSEventTypeScrollWheel:
         case NSEventTypeTabletPoint:
         case NSEventTypeTabletProximity:
         case NSEventTypeOtherMouseDown:
         case NSEventTypeOtherMouseUp:
         case NSEventTypeOtherMouseDragged:
         case NSEventTypeGesture:
         case NSEventTypeMagnify:
         case NSEventTypeSwipe:
         case NSEventTypeRotate:
         case NSEventTypeBeginGesture:
         case NSEventTypeEndGesture:
         case NSEventTypeSmartMagnify:
         case NSEventTypeQuickLook:
         case NSEventTypePressure:
         default:
            [NSApp sendEvent:event];
            break;
         }

      // Auto-release pool to ensure that ARC will flush garbage collector
      @autoreleasepool
         {
         [event release];
         }

   } while(event != nil);
   
   }

   void waitForEvent()
   {
   // Sleeps thread until some event occurs
   NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                       untilDate:[NSDate distantFuture]
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:NO];
   }
   
   
   // MOUSE
   ////////////////////////////////////////////////////////////////////////////////
   
   OSXMouse::OSXMouse() :
      CommonMouse()
   {
   // https://developer.apple.com/library/mac/documentation/GraphicsImaging/Conceptual/QuartzDisplayServicesConceptual/Articles/MouseCursor.html
   // CGAssociateMouseAndMouseCursorPosition (false); <- Prevent mouse cursor from being moved by mouse movement
   }
   
   OSXMouse::~OSXMouse()
   {
   // CGAssociateMouseAndMouseCursorPosition (true); <- Attach mouse movement back to mouse cursor movement
   }

   bool OSXMouse::position(const uint32 x, const uint32 y)
   {
   // http://stackoverflow.com/questions/1236498/how-to-get-the-display-name-with-the-display-id-in-mac-os-x
   assert( _display );
   Ptr<DisplayMTL> ptr = ptr_reinterpret_cast<DisplayMTL>(&_display);
   NSScreen* handle = ptr->handle;
   
   assert( handle );
   NSDictionary* screenDictionary = [handle deviceDescription];
   NSNumber* screenID = [screenDictionary objectForKey:@"NSScreenNumber"];

   assert( screenID );
   NSRect frame = [handle convertRectFromBacking:NSMakeRect(x, y, 0, 0)];
   CGDisplayMoveCursorToPoint([screenID unsignedIntValue], frame.origin); //CGMainDisplayID()
   return true;
   }

   bool OSXMouse::position(const Ptr<Display> __display, const uint32 x, const uint32 y)
   {
   assert( __display );
   _display = ptr_reinterpret_cast<CommonDisplay>(&__display);
   Ptr<DisplayMTL> ptr = ptr_reinterpret_cast<DisplayMTL>(&_display);
   NSScreen* handle = ptr->handle;
   
   assert( handle );
   NSDictionary* screenDictionary = [handle deviceDescription];
   NSNumber* screenID = [screenDictionary objectForKey:@"NSScreenNumber"];
    
   assert( screenID );
   NSRect frame = [handle convertRectFromBacking:NSMakeRect(x, y, 0, 0)];
   CGDisplayMoveCursorToPoint([screenID unsignedIntValue], frame.origin);
   return true;
   }

   uint32v2 OSXMouse::virtualPosition(void) const
   {
   // TODO: Finish!
   assert( 0 );
   return uint32v2(0, 0);
   }
     
   bool OSXMouse::virtualPosition(const uint32 x, const uint32 y)
   {
   // TODO: Finish!
   assert( 0 );
   return false;
   }

   void OSXMouse::show(void)
   {
   CGDisplayShowCursor(kCGNullDirectDisplay);
   }
   
   void OSXMouse::hide(void)
   {
   CGDisplayHideCursor(kCGNullDirectDisplay);
   }
   
   }
}
#endif
