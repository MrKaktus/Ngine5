/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_WINDOWS
#define ENG_INPUT_WINDOWS

#include "input/common.h"

#if defined(EN_PLATFORM_WINDOWS)



#if INTEL_PERCEPTUAL_COMPUTING_2014
#pragma comment(lib, "libpxc_d.lib")

#include "pxcsensemanager.h"         // Creative RealSense
#include "pxcprojection.h"           // For computation of UV coordinates of Depth samples on Color map

// Additional Library Directories:   $(RSSDK_DIR)/lib/$(PlatformName)    
// Additional Dependiences       :   libpxc_d.lib;

// Old SDK 2013:

//// Additional Include Directories:   $(PCSDK_DIR)/include; $(PCSDK_DIR)/sample/common/include; 
//#include "pxcsession.h"              // Creative Senz3D
//#include "pxccapture.h"
//#include "util_capture.h"
#endif

#if MICROSOFT_KINECT
#pragma comment(lib, "Kinect10.lib")
#endif

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace en
{
   namespace input
   {    
   extern const Key TranslateKey[256];
  
   class WinMouse : public CommonMouse
      {
      public:
      bool   position(const uint32 x, const uint32 y);
      bool   position(const std::shared_ptr<Display> screen, const uint32 x, const uint32 y);
      uint32v2 virtualPosition(void) const;     
      bool     virtualPosition(const uint32 x, const uint32 y);
      void   show(void);
      void   hide(void);

      void updateDisplay(uint32v2 pos);

      WinMouse();
      virtual ~WinMouse();                           // Polymorphic deletes require a virtual base destructor
      };

   class WinInterface : public CommonInterface
      {
      public:
      MSG msg;                    // Message handle
      LPDIRECTINPUT8 directInput; // DirectInput device context

      void update(void);                             // Gets actual input state, call function handling cached events
      
      WinInterface();
      virtual void init(void);
      virtual ~WinInterface();                       // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif

#endif
