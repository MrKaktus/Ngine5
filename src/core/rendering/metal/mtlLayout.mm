/*

 Ngine v5.0
 
 Module      : Metal Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlLayout.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/metal/mtlSampler.h"

namespace en
{
   namespace gpu
   {
   Ptr<SetLayout> MetalDevice::createSetLayout(const uint32 count,
                                               const ResourceGroup* group,
                                               const ShaderStages stageMask)
   {
   // TODO: Finish / Emulate !
   return Ptr<SetLayout>(nullptr);
   }

   Ptr<PipelineLayout> MetalDevice::createPipelineLayout(const uint32 sets,
                                                         const Ptr<SetLayout>* set,
                                                         const uint32 immutableSamplers,
                                                         const Ptr<Sampler>* sampler,
                                                         const ShaderStages stageMask)
   {
   // TODO: Finish / Emulate !
   return Ptr<PipelineLayout>(nullptr);
   }
   
   }
}
#endif
