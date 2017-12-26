/*

 Ngine v5.0
 
 Module      : Metal Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlDepthStencil.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   // Optimization: This table is not needed. Backend type can be directly cast to Metal type.
   static const MTLCompareFunction TranslateCompareFunction[CompareMethodsCount] = 
      {
      MTLCompareFunctionNever,             // Never              
      MTLCompareFunctionLess,              // Less
      MTLCompareFunctionEqual,             // Equal
      MTLCompareFunctionLessEqual,         // LessOrEqual
      MTLCompareFunctionGreater,           // Greater
      MTLCompareFunctionNotEqual,          // NotEqual
      MTLCompareFunctionGreaterEqual,      // GreaterOrEqual
      MTLCompareFunctionAlways             // Always
      };

   // Optimization: This table is not needed. Backend type can be directly cast to Metal type.
   static const MTLStencilOperation TranslateStencilOperation[StencilModificationsCount] =
      {
      MTLStencilOperationKeep,             // Keep
      MTLStencilOperationZero,             // Clear
      MTLStencilOperationReplace,          // Reference
      MTLStencilOperationIncrementClamp,   // Increase
      MTLStencilOperationDecrementClamp,   // Decrease
      MTLStencilOperationInvert,           // InvertBits
      MTLStencilOperationIncrementWrap,    // IncreaseWrap
      MTLStencilOperationDecrementWrap     // DecreaseWrap
      };

   DepthStencilStateMTL::DepthStencilStateMTL(const MetalDevice* gpu,
                                              const DepthStencilStateInfo& desc) :
      state(nullptr),
      reference(0, 0)
   {
   MTLDepthStencilDescriptor* descriptor = allocateObjectiveC(MTLDepthStencilDescriptor);

   descriptor.depthWriteEnabled    = desc.enableDepthWrite;
   descriptor.depthCompareFunction = TranslateCompareFunction[desc.depthFunction];
   if (desc.enableStencilTest)
      {
      // TODO: Future optimization could check if state isn't pass-through Always/3xKeep and disable it.
      for(uint8 i=0; i<2; ++i)
         {
         MTLStencilDescriptor* stencil = (i == 0) ? descriptor.frontFaceStencil : descriptor.backFaceStencil;
      
         stencil.stencilFailureOperation   = static_cast<MTLStencilOperation>(underlyingType(desc.stencil[i].whenStencilFails));  // Optimisation: TranslateStencilOperation[desc.stencil[i].whenStencilFails];
         stencil.depthFailureOperation     = static_cast<MTLStencilOperation>(underlyingType(desc.stencil[i].whenDepthFails));    // Optimisation: TranslateStencilOperation[desc.stencil[i].whenDepthFails];
         stencil.depthStencilPassOperation = static_cast<MTLStencilOperation>(underlyingType(desc.stencil[i].whenBothPass));      // Optimisation: TranslateStencilOperation[desc.stencil[i].whenBothPass];
         stencil.stencilCompareFunction    = static_cast<MTLCompareFunction>(underlyingType(desc.stencil[i].function));           // Optimisation: TranslateCompareFunction[desc.stencil[i].function];
         stencil.readMask                  = 0xFFFFFFFF;
         stencil.writeMask                 = 0xFFFFFFFF;
         }
      }

   // Create State Object
   state = [gpu->device newDepthStencilStateWithDescriptor:descriptor];
   reference.x = desc.stencil[0].reference;
   reference.y = desc.stencil[1].reference;

   deallocateObjectiveC(descriptor);
   }

   DepthStencilStateMTL::~DepthStencilStateMTL()
   {
   state = nil;
   }

   shared_ptr<DepthStencilState> MetalDevice::createDepthStencilState(const DepthStencilStateInfo& desc)
   {
   return make_shared<DepthStencilStateMTL>(this, desc);
   }
   
   }
}

#endif
