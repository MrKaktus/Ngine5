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

   PipelineLayoutD3D12::PipelineLayoutD3D12()
   {
   }

   PipelineLayoutD3D12::~PipelineLayoutD3D12()
   {
   }

   Ptr<SetLayout> Direct3D12Device::createSetLayout(const uint32 count, const Resources* group)
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

   Ptr<PipelineLayout> Direct3D12Device::createPipelineLayout()
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
//
//
//D3D12_ROOT_SIGNATURE_DESC rootDesc;
//  UINT                            NumParameters;
//  const D3D12_ROOT_PARAMETER      *pParameters;
//  UINT                            NumStaticSamplers;
//  const D3D12_STATIC_SAMPLER_DESC *pStaticSamplers;
//  D3D12_ROOT_SIGNATURE_FLAGS      Flags;
//
//D3D12_ROOT_SIGNATURE_DESC1 rootDesc1_1;
//  UINT                            NumParameters;
//  const D3D12_ROOT_PARAMETER1     *pParameters;
//  UINT                            NumStaticSamplers;
//  const D3D12_STATIC_SAMPLER_DESC *pStaticSamplers;
//  D3D12_ROOT_SIGNATURE_FLAGS      Flags;

   return ptr_reinterpret_cast<PipelineLayout>(&result);
   }

   }
}
#endif
