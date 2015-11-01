/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : Captures and holds signals from
               input devices and interfaces like
               keyboard, mouse or touchscreen.

*/

#include "input/context.h"
#include "input/hmd.h"

namespace en
{
   namespace input
   {
   //# INTERFACE
   //##########################################################################

   uint8 Interface::HMD::available(void) const
   {
   return InputContext.hmd.device.size();
   }

   Ptr<input::HMD> Interface::HMD::get(uint8 index) const
   {
   if (index < InputContext.hmd.device.size())
      return InputContext.hmd.device[index];
   return Ptr<input::HMD>(nullptr);
   }

   }
}