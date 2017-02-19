/*

 Ngine v5.0
 
 Module      : D3D12 Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Layout.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   { 
   static const D3D12_DESCRIPTOR_RANGE_TYPE TranslateResourceType[underlyingType(ResourceType::Count)] =
      {
      D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, // Sampler
      D3D12_DESCRIPTOR_RANGE_TYPE_SRV,     // Texture
      D3D12_DESCRIPTOR_RANGE_TYPE_UAV,     // Image
      D3D12_DESCRIPTOR_RANGE_TYPE_CBV,     // Uniform
      D3D12_DESCRIPTOR_RANGE_TYPE_CBV,     // Storage
      };

   PipelineLayoutD3D12::PipelineLayoutD3D12(ID3D12RootSignature* handle) :
      handle(_handle)
   {
   }

   PipelineLayoutD3D12::~PipelineLayoutD3D12()
   {
   assert( handle );
   ProfileCom( handle->Release() )
   handle = nullptr;
   }









	


   // DESCRIPTOR POOL
   //////////////////////////////////////////////////////////////////////////


   DescriptorsD3D12::DescriptorsD3D12(ID3D12DescriptorHeap* _handle, ID3D12DescriptorHeap* _handleSamplers) :
      handle(_handle),
      handleSamplers(_handleSamplers)
   {
   }
   
   DescriptorsD3D12::~DescriptorsD3D12()
   {
   assert( handle || handleSamplers );
   
   if (handle)
      {
      ProfileCom( handle->Release() )
      handle = nullptr;
      }
   if (handleSamplers)
      {
      ProfileCom( handleSamplers->Release() )
      handleSamplers = nullptr;
      }
   }
   
   Ptr<DescriptorSet> DescriptorsD3D12::allocate(const Ptr<SetLayout> layout)
   {
   Ptr<DescriptorSetD3D12> result = nullptr;
   
   // TODO: Allocate Descriptor Set from Desriptor Pool
   
   return ptr_reinterpret_cast<DescriptorSet>(&result);
   }
   
   bool DescriptorsD3D12::allocate(const uint32 count, const Ptr<SetLayout>* layouts, Ptr<DescriptorSet>* sets)
   {
   bool result = false;

   // TODO: Allocate group of Descriptor Sets from Desriptor Pool

   return result;
   }
   
   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   Ptr<SetLayout> Direct3D12Device::createSetLayout(const uint32 count, 
                                                    const ResourceGroup* group,
                                                    const ShaderStage stageMask)
   {
   Ptr<SetLayoutD3D12> result = new SetLayoutD3D12();

   // Current assigned HLSL slot for each resource type
   uint32 types = underlyingType(ResourceType::Count);   // TODO: Fix for CBV !!!!
   uint32* slot = new uint32[types];
   for(uint32 i=0u; i<types; ++i)
      slot[i] = 0u;
      
   D3D12_DESCRIPTOR_RANGE* rangeInfo = new D3D12_DESCRIPTOR_RANGE[count];
   for(uint32 i=0u; i<count; ++i)
      {
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
      rangeInfo[i].RangeType          = TranslateResourceType[resourceType];
      rangeInfo[i].NumDescriptors     = group[i].count;     // UINT - -1 or UINT_MAX to specify unbounded size (only last entry)
      rangeInfo[i].BaseShaderRegister = slot[resourceType]; // UINT - register in HLSL this resource maps to, for SRVs, 3 maps to ": register(t3);" in HLSL.
      rangeInfo[i].RegisterSpace      = 0u;                 // UINT - register space in which resources are bound

      // RootSignature 1.1 new feature:
      // D3D12_ROOT_DESCRIPTOR_TABLE1:
      // D3D12_DESCRIPTOR_RANGE1:
      // rangeInfo[i]->Flags;  // D3D12_DESCRIPTOR_RANGE_FLAGS 
      //
      // D3D12_DESCRIPTOR_RANGE_FLAG_NONE                              
      // D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE              
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE                     
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE  
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC                       

      // UINT - offset in descriptors from the start of the root signature
      // D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND - immediatelly follow previous one
      rangeInfo[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

      // Increase index by amount of slots used
      slot[resourceType] += group[i].count;
      }

   // Descriptor Ranges Table
   result->table.NumDescriptorRanges = count;
   result->table.pDescriptorRanges   = rangeInfo;

   delete [] slot;

   return ptr_reinterpret_cast<SetLayout>(&result);
   }

   Ptr<PipelineLayout> Direct3D12Device::createPipelineLayout(const uint32 sets,
                                                              const Ptr<SetLayout>* set,
                                                              const uint32 immutableSamplers,
                                                              const Ptr<Sampler>* sampler)
   {
   Ptr<PipelineLayoutD3D12> result = nullptr; 

  // Root Signature 1.1:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/mt709473(v=vs.85).aspx
  // Specifies when descriptors are static, and when their data is static. And when they are volatile.


//typedef enum D3D12_ROOT_PARAMETER_TYPE { 
//  D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE  = 0,
//  D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS   = ( D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE + 1 ),
//  D3D12_ROOT_PARAMETER_TYPE_CBV               = ( D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS + 1 ),
//  D3D12_ROOT_PARAMETER_TYPE_SRV               = ( D3D12_ROOT_PARAMETER_TYPE_CBV + 1 ),
//  D3D12_ROOT_PARAMETER_TYPE_UAV               = ( D3D12_ROOT_PARAMETER_TYPE_SRV + 1 )
//} D3D12_ROOT_PARAMETER_TYPE;


//   D3D12_ROOT_PARAMETER param;
//   param.ParameterType =  ; // D3D12_ROOT_PARAMETER_TYPE
//  union {
//    D3D12_ROOT_DESCRIPTOR_TABLE DescriptorTable;
//    D3D12_ROOT_CONSTANTS        Constants;
//    D3D12_ROOT_DESCRIPTOR       Descriptor;
//  };
//  D3D12_SHADER_VISIBILITY   ShaderVisibility;
//   
//   D3D12_ROOT_PARAMETER1 param1_1;
//   param.ParameterType =  ; // D3D12_ROOT_PARAMETER_TYPE
//  union {
//    D3D12_ROOT_DESCRIPTOR_TABLE1 DescriptorTable;
//    D3D12_ROOT_CONSTANTS         Constants;
//    D3D12_ROOT_DESCRIPTOR1       Descriptor;
//  };
//  D3D12_SHADER_VISIBILITY   ShaderVisibility;


//D3D12_ROOT_SIGNATURE_DESC1 rootDesc1_1;
//  UINT                            NumParameters;
//  const D3D12_ROOT_PARAMETER1     *pParameters;
//  UINT                            NumStaticSamplers;
//  const D3D12_STATIC_SAMPLER_DESC *pStaticSamplers;
//  D3D12_ROOT_SIGNATURE_FLAGS      Flags;

D3D12_SAMPLER_DESC {
  D3D12_FILTER               Filter;
  D3D12_TEXTURE_ADDRESS_MODE AddressU;
  D3D12_TEXTURE_ADDRESS_MODE AddressV;
  D3D12_TEXTURE_ADDRESS_MODE AddressW;
  FLOAT                      MipLODBias;
  UINT                       MaxAnisotropy;
  D3D12_COMPARISON_FUNC      ComparisonFunc;
  FLOAT                      BorderColor[4];
  FLOAT                      MinLOD;
  FLOAT                      MaxLOD;

D3D12_STATIC_BORDER_COLOR { 
  D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK  = 0,
  D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK       = ( D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK + 1 ),
  D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE       = ( D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK + 1 )
  
D3D12_STATIC_SAMPLER_DESC
  D3D12_FILTER               Filter;
  D3D12_TEXTURE_ADDRESS_MODE AddressU;
  D3D12_TEXTURE_ADDRESS_MODE AddressV;
  D3D12_TEXTURE_ADDRESS_MODE AddressW;
  FLOAT                      MipLODBias;
  UINT                       MaxAnisotropy;
  D3D12_COMPARISON_FUNC      ComparisonFunc;
  D3D12_STATIC_BORDER_COLOR  BorderColor;
  FLOAT                      MinLOD;
  FLOAT                      MaxLOD;
  UINT                       ShaderRegister;
  UINT                       RegisterSpace;
  D3D12_SHADER_VISIBILITY    ShaderVisibility;


   // Gather immutable sampler states
   D3D12_STATIC_SAMPLER_DESC* samplers = nullptr;
   if (immutableSamplers)
      {
      sampler = new D3D12_STATIC_SAMPLER_DESC[immutableSamplers];
      for(uint32 i=0; i<immutableSamplers; ++i)
         {
         SamplerD3D12* ptr =
         samplers[i] = ptr->handle;
         }
      }
      
   D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc;
   desc.NumParameters     = sets;
//  const D3D12_ROOT_PARAMETER      *pParameters;
   desc.NumStaticSamplers = immutableSamplers;
   desc.pStaticSamplers   = samplers;
   desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
   
   // Don't use this flag to get minimum optimization on Programmable Vertex Fetch shaders
   desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


  D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS      = 0x2,
  D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS        = 0x4,
  D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS      = 0x8,
  D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS    = 0x10,
  D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS       = 0x20,
  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT
  
   // Deny access to all shader stages in "default" Root Signature if no resources are needed
   //
   // desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS   |
   //               D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS     |
   //               D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS   |
   //               D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
   //               D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
  
   // Serialize Root Signature
   ID3DBlob* signature = nullptr;
   ID3DBlob* error     = nullptr;
   ProfileCom( D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &error) )
   if (error)
      {
      // TODO: Log Serialization error blob
      }
      
   ID3D12RootSignature* handle = nullptr;
   Profile( this, CreateRootSignature(0u,
                                      signature->GetBufferPointer(),
                                      signature->GetBufferSize(),
                                      IID_PPV_ARGS(&handle)) ) // __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(&handle)
   if (SUCCEEDED(lastResult[Scheduler.core()]))
      result = new PipelineLayoutD3D12(handle);

   delete [] samplers;
   
   return ptr_reinterpret_cast<PipelineLayout>(&result);
   }

   Ptr<Descriptors> Direct3D12Device::createDescriptorsPool(const uint32 maxSets, const uint32 (&count)[underlyingType(ResourceType::Count)])
   {
   Ptr<DescriptorsD3D12> result = nullptr;
   
   // All resources except of Samplers share the same heap in D3D12.
   // We set D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE to indicate that
   // Descriptors are not staged in CPU memory, but directly set in
   // shader visible space.
   //
   // See: Shader Visible Descriptor Heaps
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899211(v=vs.85).aspx
   //
   // See: Non Shader Visible Descriptor Heaps
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899199(v=vs.85).aspx
   
   // Create Samplers Heap if requested
   ID3D12DescriptorHeap* handleSamplers = nullptr;
   if (count[0] > 0)
      {
      D3D12_DESCRIPTOR_HEAP_DESC desc;
      desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
      desc.NumDescriptors = count[0];
      desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      desc.NodeMask       = 0u; // TODO: Set bit to current GPU index in SLI
   
      Profile( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&handleSamplers)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&handleSamplers)
      if (!SUCCEEDED(lastResult[Scheduler.core()]))
         {
         return ptr_reinterpret_cast<Descriptors>(&result);
         }
      }
      
   uint32 resourceTypesCount = underlyingType(ResourceType::Count);
   
   // Calculate amount of slots required for Textures, Images, Uniforms and Storage Buffers
   uint32 slots = 0;
   for(uint32 i=1; i<resourceTypesCount; ++i)
      slots += count[i];
   
   ID3D12DescriptorHeap* handle = nullptr;
   if (slots > 0)
      {
      D3D12_DESCRIPTOR_HEAP_DESC desc;
      desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      desc.NumDescriptors = slots;
      desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      desc.NodeMask       = 0u; // TODO: Set bit to current GPU index in SLI
   
      Profile( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&handle)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&handle)
      if (!SUCCEEDED(lastResult[Scheduler.core()]))
         {
         return ptr_reinterpret_cast<Descriptors>(&result);
         }
      }

   // D3D12 has no limit of Descriptor Sets that can be allocated from it.
   result = new DescriptorsD3D12(handle, handleSamplers);

   return ptr_reinterpret_cast<Descriptors>(&result);
   }

   }
}
#endif
