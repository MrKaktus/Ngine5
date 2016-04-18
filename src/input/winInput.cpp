

#include "core/configuration.h"

#if defined(EN_PLATFORM_WINDOWS)
#include <windows.h>
#include <objbase.h> 
#include "platform/windows/win_events.h"  // Window events

#include "input/winInput.h"

namespace en
{
   namespace input
   {
   const Key TranslateKey[256] =
      {
      Key::Unknown,    // 0x00
      Key::Unknown,    // 0x01
      Key::Unknown,    // 0x02
      Key::Unknown,    // 0x03
      Key::Unknown,    // 0x04
      Key::Unknown,    // 0x05
      Key::Unknown,    // 0x06
      Key::Unknown,    // 0x07
      Key::Backspace,  // 0x08
      Key::Tab,        // 0x09
      Key::Unknown,    // 0x0A
      Key::Unknown,    // 0x0B
      Key::Unknown,    // 0x0C
      Key::Enter,      // 0x0D
      Key::Unknown,    // 0x0E 
      Key::Unknown,    // 0x0F 
      Key::Shift,      // 0x10 
      Key::Ctrl,       // 0x11              
      Key::Alt,        // 0x12 
      Key::Pause,      // 0x13 
      Key::CapsLock,   // 0x14 
      Key::Unknown,    // 0x15 
      Key::Unknown,    // 0x16 
      Key::Unknown,    // 0x17 
      Key::Unknown,    // 0x18 
      Key::Unknown,    // 0x19 
      Key::Unknown,    // 0x1A 
      Key::Esc,        // 0x1B
      Key::Unknown,    // 0x1C 
      Key::Unknown,    // 0x1D 
      Key::Unknown,    // 0x1E
      Key::Unknown,    // 0x1F
      Key::Space,      // 0x20
      Key::PageUp,     // 0x21              
      Key::PageDown,   // 0x22       
      Key::End,        // 0x23    
      Key::Home,       // 0x24    
      Key::Left,       // 0x25 
      Key::Up,         // 0x26       
      Key::Right,      // 0x27  
      Key::Down,       // 0x28 
      Key::Unknown,    // 0x29 
      Key::Unknown,    // 0x2A 
      Key::Unknown,    // 0x2B 
      Key::PrtScr,     // 0x2C
      Key::Insert,     // 0x2D            
      Key::Delete,     // 0x2E  
      Key::Unknown,    // 0x2F                     
      Key::Cypher0,    // 0x30         
      Key::Cypher1,    // 0x31                 
      Key::Cypher2,    // 0x32                 
      Key::Cypher3,    // 0x33                 
      Key::Cypher4,    // 0x34                 
      Key::Cypher5,    // 0x35                 
      Key::Cypher6,    // 0x36                 
      Key::Cypher7,    // 0x37                 
      Key::Cypher8,    // 0x38                 
      Key::Cypher9,    // 0x39
      Key::Unknown,    // 0x3A 
      Key::Unknown,    // 0x3B 
      Key::Unknown,    // 0x3C 
      Key::Unknown,    // 0x3D 
      Key::Unknown,    // 0x3E 
      Key::Unknown,    // 0x3F 
      Key::Unknown,    // 0x40                      
      Key::A,          // 0x41                 
      Key::B,          // 0x42                  
      Key::C,          // 0x43                  
      Key::D,          // 0x44                  
      Key::E,          // 0x45                  
      Key::F,          // 0x46                  
      Key::G,          // 0x47                  
      Key::H,          // 0x48                  
      Key::I,          // 0x49                  
      Key::J,          // 0x4A                  
      Key::K,          // 0x4B                  
      Key::L,          // 0x4C                  
      Key::M,          // 0x4D                  
      Key::N,          // 0x4E                  
      Key::O,          // 0x4F                  
      Key::P,          // 0x50                  
      Key::Q,          // 0x51                  
      Key::R,          // 0x52                  
      Key::S,          // 0x53                  
      Key::T,          // 0x54                  
      Key::U,          // 0x55                  
      Key::V,          // 0x56                  
      Key::W,          // 0x57                  
      Key::X,          // 0x58                  
      Key::Y,          // 0x59                  
      Key::Z,          // 0x5A    
      Key::Unknown,    // 0x5B 
      Key::Unknown,    // 0x5C 
      Key::Unknown,    // 0x5D 
      Key::Unknown,    // 0x5E 
      Key::Unknown,    // 0x5F 
      Key::NumPad0,    // 0x60                          
      Key::NumPad1,    // 0x61            
      Key::NumPad2,    // 0x62            
      Key::NumPad3,    // 0x63            
      Key::NumPad4,    // 0x64            
      Key::NumPad5,    // 0x65            
      Key::NumPad6,    // 0x66            
      Key::NumPad7,    // 0x67            
      Key::NumPad8,    // 0x68            
      Key::NumPad9,    // 0x69 
      Key::Unknown,    // 0x6A 
      Key::Unknown,    // 0x6B 
      Key::Unknown,    // 0x6C 
      Key::Unknown,    // 0x6D 
      Key::Unknown,    // 0x6E 
      Key::Unknown,    // 0x6F 
      Key::F1,         // 0x70                 
      Key::F2,         // 0x71                 
      Key::F3,         // 0x72                 
      Key::F4,         // 0x73                 
      Key::F5,         // 0x74                 
      Key::F6,         // 0x75                 
      Key::F7,         // 0x76                 
      Key::F8,         // 0x77                 
      Key::F9,         // 0x78                 
      Key::F10,        // 0x79                 
      Key::F11,        // 0x7A                 
      Key::F12,        // 0x7B  
      Key::Unknown,    // 0x7C 
      Key::Unknown,    // 0x7D 
      Key::Unknown,    // 0x7E 
      Key::Unknown,    // 0x7F 
      Key::Unknown,    // 0x80 
      Key::Unknown,    // 0x81 
      Key::Unknown,    // 0x82 
      Key::Unknown,    // 0x83 
      Key::Unknown,    // 0x84 
      Key::Unknown,    // 0x85
      Key::Unknown,    // 0x86 
      Key::Unknown,    // 0x87 
      Key::Unknown,    // 0x88 
      Key::Unknown,    // 0x89
      Key::Unknown,    // 0x8A 
      Key::Unknown,    // 0x8B 
      Key::Unknown,    // 0x8C 
      Key::Unknown,    // 0x8D 
      Key::Unknown,    // 0x8E 
      Key::Unknown,    // 0x8F 
      Key::NumLock,    // 0x90                          
      Key::ScrollLock, // 0x91 
      Key::Unknown,    // 0x92 
      Key::Unknown,    // 0x93 
      Key::Unknown,    // 0x94 
      Key::Unknown,    // 0x95
      Key::Unknown,    // 0x96 
      Key::Unknown,    // 0x97 
      Key::Unknown,    // 0x98 
      Key::Unknown,    // 0x99
      Key::Unknown,    // 0x9A 
      Key::Unknown,    // 0x9B 
      Key::Unknown,    // 0x9C 
      Key::Unknown,    // 0x9D 
      Key::Unknown,    // 0x9E 
      Key::Unknown,    // 0x9F          
      Key::LeftShift,  // 0xA0   
      Key::RightShift, // 0xA1 
      Key::LeftCtrl,   // 0xA2   
      Key::RightCtrl,  // 0xA3             
      Key::LeftAlt,    // 0xA4   
      Key::RightAlt,   // 0xA5   
      Key::Unknown,    // 0xA6 
      Key::Unknown,    // 0xA7 
      Key::Unknown,    // 0xA8 
      Key::Unknown,    // 0xA9
      Key::Unknown,    // 0xAA 
      Key::Unknown,    // 0xAB 
      Key::Unknown,    // 0xAC 
      Key::Unknown,    // 0xAD 
      Key::Unknown,    // 0xAE 
      Key::Unknown,    // 0xAF
      Key::Unknown,    // 0xB0 
      Key::Unknown,    // 0xB1    
      Key::Unknown,    // 0xB2 
      Key::Unknown,    // 0xB3 
      Key::Unknown,    // 0xB4 
      Key::Unknown,    // 0xB5
      Key::Unknown,    // 0xB6 
      Key::Unknown,    // 0xB7 
      Key::Unknown,    // 0xB8 
      Key::Unknown,    // 0xB9             
      Key::Colon,      // 0xBA 
      Key::Equal,      // 0xBB 
      Key::Comma,      // 0xBC 
      Key::Minus,      // 0xBD           
      Key::Period,     // 0xBE              
      Key::Slash,      // 0xBF 
      Key::Tilde,      // 0xC0 
      Key::Unknown,    // 0xC1    
      Key::Unknown,    // 0xC2 
      Key::Unknown,    // 0xC3 
      Key::Unknown,    // 0xC4 
      Key::Unknown,    // 0xC5
      Key::Unknown,    // 0xC6 
      Key::Unknown,    // 0xC7 
      Key::Unknown,    // 0xC8 
      Key::Unknown,    // 0xC9                
      Key::Unknown,    // 0xCA 
      Key::Unknown,    // 0xCB 
      Key::Unknown,    // 0xCC 
      Key::Unknown,    // 0xCD 
      Key::Unknown,    // 0xCE 
      Key::Unknown,    // 0xCF
      Key::Unknown,    // 0xD0
      Key::Unknown,    // 0xD1    
      Key::Unknown,    // 0xD2 
      Key::Unknown,    // 0xD3 
      Key::Unknown,    // 0xD4 
      Key::Unknown,    // 0xD5
      Key::Unknown,    // 0xD6 
      Key::Unknown,    // 0xD7 
      Key::Unknown,    // 0xD8 
      Key::Unknown,    // 0xD9                
      Key::Unknown,    // 0xDA 
      Key::OpenBrace,  // 0xDB  
      Key::Unknown,    // 0xDC        
      Key::CloseBrace, // 0xDD                 
      Key::Quote,      // 0xDE     
      Key::Unknown,    // 0xDF
      Key::Unknown,    // 0xE0
      Key::Unknown,    // 0xE1          
      Key::Baskslash,  // 0xE2  
      Key::Unknown,    // 0xE3 
      Key::Unknown,    // 0xE4 
      Key::Unknown,    // 0xE5
      Key::Unknown,    // 0xE6 
      Key::Unknown,    // 0xE7 
      Key::Unknown,    // 0xE8 
      Key::Unknown,    // 0xE9                
      Key::Unknown,    // 0xEA 
      Key::Unknown,    // 0xEB 
      Key::Unknown,    // 0xEC 
      Key::Unknown,    // 0xED 
      Key::Unknown,    // 0xEE 
      Key::Unknown,    // 0xEF
      Key::Unknown,    // 0xF0
      Key::Unknown,    // 0xF1    
      Key::Unknown,    // 0xF2 
      Key::Unknown,    // 0xF3 
      Key::Unknown,    // 0xF4 
      Key::Unknown,    // 0xF5
      Key::Unknown,    // 0xF6 
      Key::Unknown,    // 0xF7 
      Key::Unknown,    // 0xF8 
      Key::Unknown,    // 0xF9                
      Key::Unknown,    // 0xFA             
      Key::Unknown,    // 0xFB 
      Key::Unknown,    // 0xFC 
      Key::Unknown,    // 0xFD 
      Key::Unknown,    // 0xFE 
      Key::Unknown     // 0xFF   
      };

   WinInterface::WinInterface() :
      CommonInterface()
   {

   }
   
   void WinInterface::update()
   {
   if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // | PM_QS_INPUT
      {
      TranslateMessage(&msg);
      DispatchMessage(&msg);   // Call WndProc to process incoming events
      }
   }


   }
}
#endif
