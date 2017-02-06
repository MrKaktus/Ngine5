/*

 Ngine v5.0
 
 Module      : Metal Display.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlDisplay.h"

#if defined(EN_MODULE_RENDERER_METAL)

namespace en
{
   namespace gpu
   {
   DisplayMTL::DisplayMTL() :
      handle(nullptr),
      CommonDisplay()
   {
   }
   
   DisplayMTL::~DisplayMTL()
   {
   handle = nullptr;
   }
   
   }
}
#endif
