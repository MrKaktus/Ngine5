/*

 Ngine v5.0
 
 Module      : GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_DEVICE
#define ENG_CORE_RENDERING_DEVICE

#include "core/utilities/TintrusivePointer.h" 

namespace en
{
   namespace gpu
   {
   // Per graphic API context, initialized depending on API choosed at runtime
   class GraphicAPI : public SafeObject
      {
      public:
      virtual ~GraphicAPI();                              // Polymorphic deletes require a virtual base destructor
      };

   // Per device context that can be used to perform operations on GPU
   class GpuDevice : public SafeObject
      {
      public:
      virtual ~GpuDevice();                              // Polymorphic deletes require a virtual base destructor
      };

   }
}

#endif