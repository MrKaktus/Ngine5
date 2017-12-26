/*

 Ngine v5.0
 
 Module      : Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_WINDOW
#define ENG_CORE_RENDERING_WINDOW

#include <memory>
using namespace std;

#include "core/rendering/texture.h"

namespace en
{
   namespace gpu
   {
   class Display;
   class Semaphore;

   enum WindowMode
      {
      Windowed          = 0,   // Create Window
      BorderlessWindow     ,   // Create borderless Window
      Fullscreen               // Switch given display to full screen mode
      };
      
   struct WindowSettings
      {
      WindowMode   mode;         // Mode in which we create surface (BorderlessWindow by default).
      shared_ptr<Display> display; // Display on which window will be created, if not specified, primary display is selected.
      uint32v2     position;     // Position on the display in pixels from Upper-Left corner. Ignored in Fullscreen mode.
      uint32v2     size;         // Window size in pixels of the screen native resolution (if zeros are set, native
                                 // resolution will be assumed). In Fullscreen mode, if size is set, it need to match
                                 // one of resolutions supported by the display (if it's smaller than native resolution,
                                 // Display will change resolution and use native HW scaler).
                                 // Swap-Chain properties:
      Format       format;       // Pixel Format for backing surfaces. (Default Format::RGBA_8)
      uint32v2     resolution;   // Destination surface resolution. If set to zeros, resolution will match window size
                                 // (default state). Can be set to smaller resolution than window size if application
                                 // wants to benefit from Windowing System scaler that will upsample the image (useful
                                 // on high DPI displays, allows saving of memory needed for allocation of Swap-Chain
                                 // surfaces, and GPU power needed to rasterize in high resolution).
                                 // In Fullscreen mode, this field should be set to zeroes (default), and you should 
                                 // use size instead to obtain the same results.
      bool         verticalSync; // If enabled (by default), will block rendering thread until VSync occurs.
      
                                 // TODO: Should block rendering thread on present call, only if previous frame VSync didn't occur yet.

      WindowSettings();
      };

   class Window
      {
      public:
      virtual shared_ptr<Display> display(void) const = 0;   // Display on which window's center point is currently located
      virtual uint32v2 position(void) const = 0;
      virtual uint32v2 size(void) const = 0;          // Size in displays native resolution pixels
      virtual uint32v2 resolution(void) const = 0;    // Resolution of backing image
      virtual uint32   frame(void) const = 0;         // Frames count. Increased after each present call, starts from 0.

      virtual bool movable(void) = 0;
      virtual void move(const uint32v2 position) = 0;
      virtual void resize(const uint32v2 size) = 0;
      virtual void active(void) = 0;
      virtual void transparent(const float opacity) = 0;
      virtual void opaque(void) = 0;
      virtual shared_ptr<Texture> surface(const shared_ptr<Semaphore> signalSemaphore = nullptr) = 0; // Will signal this semaphore, once Swap-Chain surface is presented, and can be reused
      virtual void present(const shared_ptr<Semaphore> waitForSemaphore = nullptr) = 0;        // Will wait for semaphore to signal, when rendering to Swap-Chain surface is being done, and then will present
                                                                                        // Presenting is always performed from first queue of type QueueType::Universal (queue 0).
      
      virtual ~Window() {};                               // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
