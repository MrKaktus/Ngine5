/*

 Ngine v5.0
 
 Module      : Windows specific code.
 Requirements: none
 Description : Captures and processes 
               asynchronous callbacks from
               operating system to support
               input signals.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)

#include "input/winInput.h"

//#include "input/context.h"
//#include "core/rendering/context.h"
#include "platform/windows/win_events.h"

//#include "core/utilities/TintrusivePointer.h"
//#include "utilities/utilities.h"

#include <windows.h> 
#include <windowsx.h> // For GET_X_PARAM, GET_Y_PARAM which fix multiple monitor issues.

using namespace en;
using namespace en::input;

void ConvertWindowPositionToScreenPosition(HWND hWnd, LPARAM lParam, uint32& x, uint32& y)
{
// Convert Window mouse coordinates to Screen coordinates
POINT pt;
pt.x = (uint32)GET_X_LPARAM(lParam);
pt.y = (uint32)GET_Y_LPARAM(lParam);
ClientToScreen(hWnd, &pt);  // TODO: Is this calculated to screen as current display, or screen like desktop / virtual display ?
x = pt.x; 
y = pt.y; 
}

LRESULT CALLBACK WinEvents(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
Ptr<WinInterface> input = ptr_dynamic_cast<WinInterface, Interface>(en::Input);

switch(msg)
   {           
   case WM_KEYDOWN:
      {
      Key key = TranslateKey[(uint8)wParam];
      Ptr<CommonKeyboard> keyboard = ptr_dynamic_cast<CommonKeyboard, Keyboard>(input->keyboards[0]);

      // If key is continuously pressed, don't resend the 'pressed' event
      if (keyboard->keys[underlyingType(key)] == KeyState::Pressed)
         break;
    
      keyboard->keys[underlyingType(key)] = KeyState::Pressed; 
  
      // Call event handling function
      KeyboardEvent event(key);
      event.type  = KeyPressed;
      input->callback[event.type]( reinterpret_cast<Event&>(event) ); 
      break;
      }

   case WM_KEYUP:
      {
      Key key = TranslateKey[(uint8)wParam];
      Ptr<CommonKeyboard> keyboard = ptr_dynamic_cast<CommonKeyboard, Keyboard>(input->keyboards[0]);

      // If key is continuously released (which shouldn't happen), don't resend the 'released' event
      if (keyboard->keys[underlyingType(key)] == KeyState::Released)
         break;
      
      keyboard->keys[underlyingType(key)] = KeyState::Released; 
  
      // Call event handling function
      KeyboardEvent event(key);
      event.type = KeyReleased;
      input->callback[event.type]( reinterpret_cast<Event&>(event) );
      break;
      } 

   // TODO: Is Mouse position reported in current display coordinates or virtual display / desktop coordinates ?
   case WM_MOUSEMOVE:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);
      
      // Call event handling function
      MouseEvent event(MouseMoved);
      event.x = mouse->x;
      event.y = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) );
      break;
      }

   case WM_LBUTTONDOWN:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      mouse->buttons[underlyingType(MouseButton::Left)] = KeyState::Pressed;

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);

      // Call event handling function
      MouseEvent event(MouseButtonPressed);
      event.button = MouseButton::Left;
      event.x      = mouse->x;
      event.y      = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) );
      break;
      }

   case WM_LBUTTONUP:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      mouse->buttons[underlyingType(MouseButton::Left)] = KeyState::Released;

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);

      // Call event handling function
      MouseEvent event(MouseButtonReleased);
      event.button = MouseButton::Left;
      event.x      = mouse->x;
      event.y      = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) ); 
      break;
      }

   case WM_RBUTTONDOWN:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      mouse->buttons[underlyingType(MouseButton::Right)] = KeyState::Pressed;

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);

      // Call event handling function
      MouseEvent event(MouseButtonPressed);
      event.button = MouseButton::Right;
      event.x      = mouse->x;
      event.y      = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) ); 
      break;
      }

   case WM_RBUTTONUP:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      mouse->buttons[underlyingType(MouseButton::Right)] = KeyState::Released;

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);
      
      // Call event handling function
      MouseEvent event(MouseButtonReleased);
      event.button = MouseButton::Right;
      event.x      = mouse->x;
      event.y      = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) ); 
      break;
      }

   case WM_MBUTTONDOWN:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      mouse->buttons[underlyingType(MouseButton::Middle)] = KeyState::Pressed;

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);

      // Call event handling function
      MouseEvent event(MouseButtonPressed);
      event.button = MouseButton::Middle;
      event.x      = mouse->x;
      event.y      = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) ); 
      break;
      }

   case WM_MBUTTONUP:
      {
      Ptr<CommonMouse> mouse = ptr_dynamic_cast<CommonMouse, Mouse>(input->mouses[0]);

      mouse->buttons[underlyingType(MouseButton::Middle)] = KeyState::Released;

      // Convert Window mouse coordinates to Screen coordinates
      ConvertWindowPositionToScreenPosition(hWnd, lParam, mouse->x, mouse->y);

      // Call event handling function
      MouseEvent event(MouseButtonReleased);
      event.button = MouseButton::Middle;
      event.x      = mouse->x;
      event.y      = mouse->y;
      input->callback[event.type]( reinterpret_cast<Event&>(event) ); 
      break;
      }

    //   case MM_JOY1BUTTONDOWN:               // button is down 
    //        {
    //        Key key = winKeyToEnum[(uint8)wParam];

    //        if (en::InputContext.keyboard.keys[key] == Pressed)
    //           break;

    //        // Call event handling function
    //        event.type            = JoystickButtonPressed;
    //        event.joystick.button = 
    //        en::InputContext.events.callback(event);      
    //        break;
    //        }

    //if((UINT) wParam & JOY_BUTTON1) 
    //{ 
    //    PlaySound(lpButton1, SND_LOOP | SND_ASYNC | SND_MEMORY); 
    //    DrawFire(hWnd); 
    //} 
    //else if((UINT) wParam & JOY_BUTTON2) 
    //{ 
    //    PlaySound(lpButton2, SND_ASYNC | SND_MEMORY |  SND_LOOP); 
    //    DrawFire(hWnd); 
    //} 
    //break; 
    //   case MM_JOY1BUTTONUP:
    //        {
    //        }

       case WM_CLOSE:
            {
            // Call event handling function
            Event event(AppClose);
            input->callback[event.type](event); 
            break;
            }

       case WM_ACTIVATEAPP:
            {
            if (wParam == TRUE)
               ShowWindow(hWnd, SW_RESTORE);
            break;
            };

       case WM_SIZE: 
            {
            if (wParam == SIZE_RESTORED) 
               {
               // Call event handling function
               Event event(AppWindowActive);
               input->callback[event.type](event); 
               }
            if (wParam == SIZE_MINIMIZED) 
               {
               // Call event handling function
               Event event(AppWindowHidden);
               input->callback[event.type](event); 
               }

            return DefWindowProc(hWnd, msg, wParam, lParam);
            }

       case WM_DESTROY:
            PostQuitMessage(WM_QUIT);
            break;

       default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
       }

return 0;
}
#endif
