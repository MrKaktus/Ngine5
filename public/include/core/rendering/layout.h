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

/// Direct3D12 has ShaderStage visibility set per Root Parameter (so one for whole resource Set),
/// while Vulkan can specify this visibility on per Resource Group (Range) basis.
/// Vulkan can also specify mask of Shader stages, while D3D12 only one stage, or all.
/// HLSL refers to resources through virtual "stages", while SPIRV through Sets (that cannot be merged).
///
/// To emulate Vulkan Descriptors in D3D12:
/// - several Descriptor Tables need to be created, each visible for separate stage, if multi-stage visible
/// - all those Tables need to refer to the same resources, and have the same Register Ranges,
///   but Register Ranges cannot overlapp in Register Space (and Register Space would need to be shared,
///   to match reflection of Vulkan "set" adressing).
/// - this would introduce resources aliasing, and require shader be compiled with D3D10_SHADER_RESOURCES_MAY_ALIAS
/// - this also introduces waste of Descriptor Slots, and Descriptor Set slots in Root Signature
///
/// Because of the above, better alternative is to:
/// - Always declare multi-stage sets as visible to All in D3D12, or
/// - completly disallow multi-stage visibility from frontend API (limit to single stage or all like in D3D12).

// TODO: Compare above to Metal Argument Buffers.

// Interestin comparison of binding models:
// https://github.com/gpuweb/gpuweb/issues/19

// Layout rules:
// - Layout entries are set in priority order:
//   - Push Constants
//   - Direct resources
//   - Descriptor Sets
//     - Sets are numerated from 0, in order in which they are declared in Layout
//     - Each Set has its index visible and declared in shader: 
//       layout(set = 0, binding = 17) uniform texture2D Textures[128];
//     - Each group of resource descriptors of the same type receives some unique Binding X (is seen as Array of Descriptors).
//     - Binding numbers are arbitrary (defined by app), but need to match between app and shader.
//       - So binding 0th texture in set 0, means setting it at [binding=17, index=0] in example above
//     - So descriptors inside given Set, are numerated from 0 per resource-type (binding N resource group), in order in which they are bound


enum class ResourceType : uint8
{
    Texture1D                 = 0,
    Texture1DArray               ,
    Texture2D                    ,
    Texture2DArray               ,
    Texture2DMultisample         ,
    Texture2DMultisampleArray    ,
    Texture3D                    ,
    TextureCubeMap               ,
    TextureCubeMapArray          ,

    Sampler                      ,
    UniformBuffer                , // TODO: We cannot have bindless arrays of those as they need to be typed in GLSL...
    StorageBuffer                ,

    Count                        ,
    Invalid                      , // TODO: Specific value like uint8 max?
};

// When creating descriptor set layout, following restriction apply:
// Each resource type can be used by only one group, unless its a
// consecutive collection of groups, each having one descriptor.
//
// Either array of descriptors:
// - ResourceGroup(ResourceType::Texture2D, 1000);
//
// or array of bindings:
// - ResourceGroup(ResourceType::UniformBuffer, 1);
// - ResourceGroup(ResourceType::UniformBuffer, 1);
// - ResourceGroup(ResourceType::UniformBuffer, 1);
//   
struct ResourceGroup
{
    ResourceType type;
    uint32       count;

    ResourceGroup(
            const ResourceType _type,
            const uint32 _count) :
        type(_type),
        count(_count)
    {};
};

/// Layout of resources in Descriptors Set
class SetLayout
{
    public:
    virtual ~SetLayout() {};
};

/// Layout of all resources used by the Pipeline object
class PipelineLayout
{
    public:
    virtual ~PipelineLayout() {};
};

/// Set of Descriptors, handles to resources of different kind
class DescriptorSet
{
    public:

    // slot is local index in given resource type group
    virtual void setBuffer(
        const uint32 slot,
        const Buffer& buffer) = 0;
       
    // slot is local index in given resource type group
    virtual void setSampler(
        const uint32 slot,
        const Sampler& sampler) = 0;
       
    // slot is local index in given resource type group
    virtual void setTextureView(
        const uint32 slot,
        const TextureView& view) = 0;

    virtual ~DescriptorSet() {};
};

// Range of Descriptors that can be used, to allocated from it set of Descriptors
class Descriptors
{
    public:
    virtual DescriptorSet* allocate(
        const SetLayout& layout) = 0;
       
    virtual bool allocate(
        const uint32 count,
        const SetLayout*(&layouts)[],
        DescriptorSet**& sets) = 0;

    virtual ~Descriptors() {};
};

} // en::gpu
} // en

#endif
