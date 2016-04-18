/*

 Ngine v5.0
 
 Module      : Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_PIPELINE
#define ENG_CORE_RENDERING_PIPELINE

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   // Cross API issues:
   //
   // DX12   - has LogicOp per RT
   // Vulkan - has shared LogicOp used for all RT
   // Metal  - has no LogicOp concept
   //
   // Alpha To Coverage:
   // 
   // DX12   - Stored in Blend State
   // Vulkan - Stored in Multisample State           -> Multisample State choosed as most Sane One!
   // Metal  - Pipeline State
   //
   // Rasterization Enabler:
   //
   // Vulkan - Stored in Raster State
   // Metal  - Pipeline State
   //
   // Samples count:
   //
   // D3D12  - Rasterizer State (ForcedSampleCount)
   // Vulkan - Stored in Multisample State           -> Multisample State choosed as most Sane One!
   // Metal  - Pipeline State
   //
   // Multisample Enable:
   //
   // D3D12  - Rasterizer Desc
   // Vulkan - Multisample State
   //
   // D3D12 decided not to support Points Fill Mode ?
   // D3D12 has Depth Bias as Int, Vulkan as float
   // D3D12 has Depth "clip", Vulkan has "clamp"
   //
   // Add feature flags:
   //
   // struct Support
   //    {
   //    bool logicOperation;                  // Supports global logic operation appliced to all Render Targets
   //    bool logicOperationPerRenderTarget;   // Supports local logic operation specified per Render Target
   //    bool dualSourceBlending;              // Supports Dual-Source blending
   //    } support;
   //

   // Handle for Pipeline State Object binding specification
   class Pipeline : public SafeObject
      {
      public:
      virtual ~Pipeline() {};                            // Polymorphic deletes require a virtual base destructor
      };

   }
}

#endif