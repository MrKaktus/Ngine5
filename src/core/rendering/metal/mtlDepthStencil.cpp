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

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlDepthStencil.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   { 
   static const MTLCompareFunction TranslateCompareFunction[CompareMethodsCount] = 
      {
      MTLCompareFunctionNever,             // Never              
      MTLCompareFunctionAlways,            // Always
      MTLCompareFunctionLess,              // Less
      MTLCompareFunctionLessEqual,         // LessOrEqual
      MTLCompareFunctionEqual,             // Equal
      MTLCompareFunctionGreater,           // Greater
      MTLCompareFunctionGreaterEqual,      // GreaterOrEqual
      MTLCompareFunctionNotEqual           // NotEqual
      };

   static const MTLStencilOperation TranslateStencilOperation[StencilModificationsCount] =
      {
      MTLStencilOperationKeep,             // Keep
      MTLStencilOperationZero,             // Clear
      MTLStencilOperationReplace,          // Reference
      MTLStencilOperationIncrementClamp,   // Increase
      MTLStencilOperationIncrementWrap,    // IncreaseWrap
      MTLStencilOperationDecrementClamp,   // Decrease
      MTLStencilOperationDecrementWrap,    // DecreaseWrap
      MTLStencilOperationInvert            // InvertBits
      };

   DepthStencilStateMTL::DepthStencilStateMTL(const DepthStencilStateInfo& desc) :
      state(nullptr)
   {
   MTLDepthStencilDescriptor* descriptor = [[MTLDepthStencilDescriptor alloc] init];

   descriptor->depthWriteEnabled    = desc.enableDepthWrite;
   descriptor->depthCompareFunction = TranslateCompareFunction[desc.depthFunction];
   if (desc.enableStencilTest)
      {
      // TODO: Future optimization could check if state isn't pass-through Always/3xKeep and disable it.
      descriptor->frontFaceStencil     = [[MTLStencilDescriptor alloc] init];
      descriptor->backFaceStencil      = [[MTLStencilDescriptor alloc] init];
      for(uint8 i=0; i<2; ++i)
         {
         MTLStencilDescriptor* stencil = (i == 0) ? state.frontFaceStencil : state.backFaceStencil;
      
         stencil->stencilFailureOperation   = TranslateStencilOperation[desc.stencil[i].whenStencilFails];
         stencil->depthFailureOperation     = TranslateStencilOperation[desc.stencil[i].whenDepthFails]; 
         stencil->depthStencilPassOperation = TranslateStencilOperation[desc.stencil[i].whenBothPass];
         stencil->stencilCompareFunction    = TranslateCompareFunction[desc.stencil[i].function];
         stencil->readMask                  = 0xFFFFFFFF;
         stencil->writeMask                 = 0xFFFFFFFF;
         }
      }

   // Create State Object
   state = [device newDepthStencilStateWithDescriptor:desc];
   reference.x = desc.stencil[0].reference;
   reference.y = desc.stencil[1].reference;

   // Release temporary resources
   if (descriptor->frontFaceStencil != nullptr)
      [descriptor->frontFaceStencil release];
   if (descriptor->backFaceStencil != nullptr)
      [descriptor->backFaceStencil release];
   [descriptor release];
   }

   DepthStencilStateMTL::~DepthStencilStateMTL()
   {
   state = nil;
   }

   //  // Set dynamic Depth-Stencil State and Stencil Test Reference Values
   //  [renderCommandEncoder setDepthStencilState:depthStencilState]; 
   //  if (!supportSeparateStencilReferenceValue)
   //     [renderCommandEncoder setStencilReferenceValue: reference.x ]; 
   //  else  
   //     [renderCommandEncoder setStencilFrontReferenceValue:reference.x backReferenceValue:reference.y];

   }
}

#endif