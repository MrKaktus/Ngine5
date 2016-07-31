/*

 Ngine v5.0
 
 Module      : Common Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_INPUT_ASSEMBLER
#define ENG_CORE_RENDERING_COMMON_INPUT_ASSEMBLER

#include "core/rendering/inputAssembler.h"

namespace en
{
   namespace gpu
   {
   extern const uint8 TranslateAttributeSize[underlyingType(Attribute::Count)];
   }
}

#endif
