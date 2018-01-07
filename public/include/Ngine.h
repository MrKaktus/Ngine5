/*

 Ngine v5.0
 
 Prototype of final thread and task friendly
 framework for game development. Main goals are:
 - fully thread pool (task oriented) execution
 - multithreaded data processing 
 - multiplatform compilation
 - safe data & code structures
 - scene graph management
 - scripting support
 - layered rendering context
 - virtual texturing

*/

#ifndef ENG
#define ENG

#include <memory>
using namespace std;

#include "core/defines.h"
#include "core/types.h"
//#include "core/configuration.h"
#include "core/storage.h"
#include "core/log/log.h"
#include "core/config/config.h"
#include "core/utilities/parser.h"
#include "core/utilities/TarrayAdvanced.h"

#ifdef EN_PLATFORM_OSX
//#include "platform/osx/osx_callbacks.h"
#include "platform/osx/osx_init.h"
#include "platform/osx/osx_main.h"
#endif




#include "rendering/stereo.h"

#include "utilities/utilities.h"
#include "utilities/strings.h"
#include "utilities/gpcpu/gpcpu.h"
#include "utilities/timer.h" // TODO: correct OS defines for UNIX family and IPHONE
#include "input/input.h"     // TODO: Add support for MacOS
#include "platform/system.h"
#include "platform/android/and_init.h"

#include "platform/blackberry/bb_init.h"
#include "platform/blackberry/bb_main.h"

#include "platform/windows/win_init.h"
//#include "platform/windows/win_main.h"
//#include "platform/windows/win_events.h"

#include "audio/audio.h"         // Public interface - TODO: Finish!

#include "core/rendering/device.h" // Public interface
#include "resources/resources.h"   // Public interface
#include "resources/forsyth.h"     // Public interface
#include "resources/bmp.h"         // Public interface
#include "resources/tga.h"         // Public interface
#include "resources/png.h"         // Public interface
#include "resources/exr.h"         // Public interface
#include "resources/hdr.h"         // Public interface
#include "resources/obj.h"         // Public interface
#include "resources/mtl.h"         // Public interface

#include "scene/scene.h"

#include "scene/state.h"         // Public interface

#include "resources/effect.h"

#include "threading.h"
#ifdef EN_PLATFORM_WINDOWS
#include "threading/scheduler.h"
#endif
#include "audio/audio.h"
#include "utilities/random.h"

#endif
