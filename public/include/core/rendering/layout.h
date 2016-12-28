/*

 Ngine v5.0
 
 Module      : Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_RESOURCE_LAYOUT
#define ENG_CORE_RENDERING_RESOURCE_LAYOUT

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   // 64 bytes  - Push Constants
   //
   // Direct3D12 Root Signature:
   // 256 bytes (64 DWORD's) for all entries (Sets pointers, resource handles, immediates)
   //
   // Vulkan Push Constants:
   // 256 bytes - NVidia
   // 128 bytes - AMD, Intel, Qualcomm, ImgTec
   //
   // Vulkan max uniform Buffer range:
   //
   // 16KB  - ARM
   // 64KB  - NVidia, Tegra, Qualcomm
   // 128MB - PowerVR
   // 4GB   - AMD, Intel
   //
   // 16KB-64KB - UBO's backed ( Uniform, Storage )
   // X GB      - Memory backed ( Storage, Texture, Image )
   
   // Direct3D12 has ShaderStage visibility set per Root Parameter (so one for whole resources Set),
   // while Vulkan can specify this visibility on per Resource Group (Range) basis.

   enum class ResourceType : uint32
      {
      Sampler = 0,
      Texture    ,
      Image      ,
      Uniform    ,
      Storage    ,
      Count
      };

   struct ResourceGroup
      {
      ResourceType type;
      uint32       count;
      };
      
   // Layout of resources in Descriptors Set
   class SetLayout : public SafeObject<SetLayout>
      {
      public:
      virtual ~SetLayout() {};                                   // Polymorphic deletes require a virtual base destructor
      };

   // Layout of all resources used by the Pipeline object
   class PipelineLayout : public SafeObject<PipelineLayout>
      {
      public:
      virtual ~PipelineLayout() {};                              // Polymorphic deletes require a virtual base destructor
      };

   // Set of handles to resources of different kind
   class DescriptorSet : public SafeObject<DescriptorSet>
      {
      public:
      // TODO: Methods to binding resources to it
      virtual ~DescriptorSet() {};
      };

   }
}
#endif