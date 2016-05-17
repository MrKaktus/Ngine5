/*

 Ngine v5.0
 
 Module      : Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_DEPTH_STENCIL_STATE
#define ENG_CORE_RENDERING_DEPTH_STENCIL_STATE

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   struct DepthStencilStateInfo
      {
      bool enableDepthTest;
      bool enableDepthWrite;
      bool enableDepthBounds;
      bool enableStencilTest;
      CompareMethod depthFunction;
      float2 depthRange;

      struct StencilOperation
         {
         CompareMethod       function;         // Compare method to use for each test
         StencilModification whenStencilFails; // What to do if stencil test fails
         StencilModification whenDepthFails;   // What to do if stencil test passes but depth test fails
         StencilModification whenBothPass;     // What to do if both tests passes
         uint32              reference;        // Reference value
         uint32              readMask;         // Mask that is ANDed with both the reference value and the stored stencil value before the test, also called "compare mask"
         uint32              writeMask;        // Mask that is ANDed with output stencil value before writing to stencil attachment
         } stencil[2];

      DepthStencilStateInfo();
      };

   class DepthStencilState : public SafeObject<DepthStencilState>
      {
      public:
      virtual ~DepthStencilState() {};                           // Polymorphic deletes require a virtual base destructor
      };

   }
}

#endif
