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
#include "input/context.h"
#include "core/rendering/context.h"
#include "platform/windows/win_events.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h> 
#include <windowsx.h> // For GET_X_PARAM, GET_Y_PARAM which fix multiple monitor issues.

LRESULT CALLBACK WinEvents(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
using namespace en::input;

switch (msg)
       {           
       case WM_KEYDOWN:
            {
            Key key = winKeyToEnum[(uint8)wParam];

            if (en::InputContext.keyboard.keys[key] == Pressed)
               break;

            // Call event handling function
            KeyboardEvent event;
            memset(&event, 0, sizeof(KeyboardEvent));
            event.type = KeyPressed;
            event.key  = key;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

            en::InputContext.keyboard.keys[key] = Pressed;      
            break;
            }

       case WM_KEYUP:
            {
            Key key = winKeyToEnum[(uint8)wParam];

            if (en::InputContext.keyboard.keys[key] == Released)
               break;

            // Call event handling function
            KeyboardEvent event;
            memset(&event, 0, sizeof(KeyboardEvent));
            event.type = KeyReleased;
            event.key  = key;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) );

            en::InputContext.keyboard.keys[key] = Released;
            break;
            } 

       case WM_MOUSEMOVE:
            {
            assert( en::GpuContext.screen.created );
            POINT pt;
            pt.x = (uint32)GET_X_LPARAM(lParam);
            pt.y = (uint32)GET_Y_LPARAM(lParam);
            ClientToScreen(en::GpuContext.device.hWnd, &pt);

            en::InputContext.mouse.x = pt.x; 
            en::InputContext.mouse.y = pt.y; 

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type = MouseMoved;
            event.x    = en::InputContext.mouse.x;
            event.y    = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) );

            break;
            }

       case WM_LBUTTONDOWN:
            {
            en::InputContext.mouse.buttons[LeftMouseButton] = Pressed;
            en::InputContext.mouse.x = (uint32)GET_X_LPARAM(lParam);
            en::InputContext.mouse.y = (uint32)GET_Y_LPARAM(lParam);

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type   = MouseButtonPressed;
            event.button = LeftMouseButton;
            event.x      = en::InputContext.mouse.x;
            event.y      = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) );

            break;
            }

       case WM_LBUTTONUP:
            {
            en::InputContext.mouse.buttons[LeftMouseButton] = Released;
            en::InputContext.mouse.x = (uint32)GET_X_LPARAM(lParam);
            en::InputContext.mouse.y = (uint32)GET_Y_LPARAM(lParam);

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type   = MouseButtonReleased;
            event.button = LeftMouseButton;
            event.x      = en::InputContext.mouse.x;
            event.y      = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

            break;
            }

       case WM_RBUTTONDOWN:
            {
            en::InputContext.mouse.buttons[RightMouseButton] = Pressed;
            en::InputContext.mouse.x = (uint32)GET_X_LPARAM(lParam);
            en::InputContext.mouse.y = (uint32)GET_Y_LPARAM(lParam);

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type   = MouseButtonPressed;
            event.button = RightMouseButton;
            event.x      = en::InputContext.mouse.x;
            event.y      = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

            break;
            }

       case WM_RBUTTONUP:
            {
            en::InputContext.mouse.buttons[RightMouseButton] = Released;
            en::InputContext.mouse.x = (uint32)GET_X_LPARAM(lParam);
            en::InputContext.mouse.y = (uint32)GET_Y_LPARAM(lParam);

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type   = MouseButtonReleased;
            event.button = RightMouseButton;
            event.x      = en::InputContext.mouse.x;
            event.y      = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

            break;
            }

       case WM_MBUTTONDOWN:
            {
            en::InputContext.mouse.buttons[MiddleMouseButton] = Pressed;
            en::InputContext.mouse.x = (uint32)GET_X_LPARAM(lParam);
            en::InputContext.mouse.y = (uint32)GET_Y_LPARAM(lParam);

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type   = MouseButtonPressed;
            event.button = MiddleMouseButton;
            event.x      = en::InputContext.mouse.x;
            event.y      = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

            break;
            }

       case WM_MBUTTONUP:
            {
            en::InputContext.mouse.buttons[MiddleMouseButton] = Released;
            en::InputContext.mouse.x = (uint32)GET_X_LPARAM(lParam);
            en::InputContext.mouse.y = (uint32)GET_Y_LPARAM(lParam);

            // Call event handling function
            MouseEvent event;
            memset(&event, 0, sizeof(MouseEvent));
            event.type   = MouseButtonReleased;
            event.button = MiddleMouseButton;
            event.x      = en::InputContext.mouse.x;
            event.y      = en::InputContext.mouse.y;
            en::InputContext.events.callback( reinterpret_cast<Event&>(event) ); 

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
            Event event;
            memset(&event, 0, sizeof(Event));
            event.type = AppClose;
            en::InputContext.events.callback(event); 
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
               Event event;
               memset(&event, 0, sizeof(Event));
               event.type = AppWindowActive;
               en::InputContext.events.callback(event); 
               }
            if (wParam == SIZE_MINIMIZED) 
               {
               // Call event handling function
               Event event;
               memset(&event, 0, sizeof(Event));
               event.type = AppWindowHidden;
               en::InputContext.events.callback(event); 
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
