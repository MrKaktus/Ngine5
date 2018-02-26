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

#include <memory>
using namespace std;

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/buffer.h"
#include "core/rendering/sampler.h"
#include "core/rendering/texture.h"

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
   // Vulkan maxUniformBufferRange:
   //
   // 16KB  - ARM 
   // 64KB  - NVidia, Tegra, Qualcomm
   // 128MB - PowerVR, Intel
   // 4GB   - AMD, Intel
   //
   // Vulkan maxStorageBufferRange: (128MB-4GB)
   // 
   // 128MB - Adreno, ARM Mali-T880, PowerVR Rouge
   //   1GB - Intel
   //   2GB - NVidia
   //   4GB - AMD
   //
   // Quallcom - Adreno
   // ARM      - Mali
   // PowerVR  - Rogue
   //
   // 16KB-64KB - UBO's backed ( Uniform, Storage )
   // X GB      - Memory backed ( Storage, Texture, Image )
   
   // Direct3D12 has ShaderStage visibility set per Root Parameter (so one for whole resource Set),
   // while Vulkan can specify this visibility on per Resource Group (Range) basis.
   // Vulkan can also specify mask of Shader stages, while D3D12 only one stage, or all.
   // HLSL refers to resources through virtual "stages", while SPIRV through Sets (that cannot be merged).

   // To emulate Vulkan Descriptors in D3D12:
   // - several Descriptor Tables need to be created, each visible for separate stage, if multi-stage visible
   // - all those Tables need to refer to the same resources, and have the same Register Ranges,
   //   but Register Ranges cannot overlapp in Register Space (and Register Space would need to be shared,
   //   to match reflection of Vulkan "set" adressing).
   // - this would introduce resources aliasing, and require shader be compiled with D3D10_SHADER_RESOURCES_MAY_ALIAS
   // - this also introduces waste of Descriptor Slots, and Descriptor Set slots in Root Signature
   //
   // Because of the above, better alternative is to:
   // - Always declare multi-stage sets as visible to All in D3D12, or
   // - completly disallow multi-stage visibility from frontend API (limit to single stage or all like in D3D12).

   // Interestin cmparison of binding models:
   // https://github.com/gpuweb/gpuweb/issues/19

   // Layout rules:
   // - Layout entries are set in priority order:
   //   - Push Constants
   //   - Direct resources
   //   - Descriptor Sets
   //     - Sets are numerated from 0, in order in which they are declared in Layout
   //     - Descriptors inside given Set, are numerated from 0, per resource-type, in order in which they are bound
   
   // TODO: "per resource-type" ? clarify 

   enum class ResourceType : uint32
      {
      Sampler = 0,
      Texture    ,
      Image      ,
      Uniform    ,
   // Formatted  ,   // Could be introduced to support Texel Buffers
      Storage    ,
      Count
      };

   struct ResourceGroup
      {
      ResourceType type;
      uint32       count;
      TextureType  textureType; // If resource is Texture, it's type needs to be specified in Layout
      
      ResourceGroup(
         const ResourceType type,
         const uint32 count,
         const TextureType _textureType = TextureType::Texture2D) :
            type(type),
            count(count),
            textureType(_textureType)
         {};
      };
      
   // Layout of resources in Descriptors Set
   class SetLayout
      {
      public:
      virtual ~SetLayout() {};
      };

   // Layout of all resources used by the Pipeline object
   class PipelineLayout
      {
      public:
      virtual ~PipelineLayout() {};
      };

   // Set of Descriptors, handles to resources of different kind
   class DescriptorSet
      {
      public:
      virtual void setBuffer(
         const uint32 slot,
         const Buffer& buffer) = 0;
         
      virtual void setSampler(
         const uint32 slot,
         const Sampler& sampler) = 0;
         
      virtual void setTextureView(
         const uint32 slot,
         const TextureView& view) = 0;

      virtual ~DescriptorSet() {};
      };
      
   // Range of Descriptors that can be used, to allocated from it set of Descriptors
   class Descriptors : public enable_shared_from_this<Descriptors>
      {
      public:
      virtual shared_ptr<DescriptorSet> allocate(
         const shared_ptr<SetLayout> layout) = 0;
         
      virtual bool allocate(
         const uint32 count,
         const shared_ptr<SetLayout>(&layouts)[],
         shared_ptr<DescriptorSet>** sets) = 0;

      virtual ~Descriptors() {};
      };
   }
}
#endif
