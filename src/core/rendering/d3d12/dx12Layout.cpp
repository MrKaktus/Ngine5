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


   // Optimisation: This table is not needed. Frontend type can be directly cast to D3D12 type.
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn879482(v=vs.85).aspx
   const D3D12_SHADER_VISIBILITY TranslateShaderStage[underlyingType(ShaderStage::Count)] =
      {
      D3D12_SHADER_VISIBILITY_ALL                 ,  // All
      D3D12_SHADER_VISIBILITY_VERTEX              ,  // Vertex
      D3D12_SHADER_VISIBILITY_HULL                ,  // Control
      D3D12_SHADER_VISIBILITY_DOMAIN              ,  // Evaluation
      D3D12_SHADER_VISIBILITY_GEOMETRY            ,  // Geometry
      D3D12_SHADER_VISIBILITY_PIXEL                  // Fragment
      };
         
   Ptr<SetLayout> Direct3D12Device::createSetLayout(const uint32 count, 
                                                    const ResourceGroup* group,
                                                    const ShaderStages stageMask)
   {
   assert( count );
   assert( group );

   Ptr<SetLayoutD3D12> result = new SetLayoutD3D12();

   // Current assigned HLSL slot for each resource type
   // TODO: Are those slots numerated separately per each resource type or is this shared pool ?
   //       See this: https://msdn.microsoft.com/en-us/library/windows/desktop/dn899207(v=vs.85).aspx
   uint32 types = underlyingType(ResourceType::Count) - 1;   // Uniform and Storage buffers are both handled as CBV
   uint32* slot = new uint32[types];
   for(uint32 i=0u; i<types; ++i)
      slot[i] = 0u;
      
   D3D12_DESCRIPTOR_RANGE* rangeInfo = new D3D12_DESCRIPTOR_RANGE[count];
 //D3D12_DESCRIPTOR_RANGE1* rangeInfo = new D3D12_DESCRIPTOR_RANGE1[count]; // v1.1
   for(uint32 i=0u; i<count; ++i)
      {
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
      rangeInfo[i].RangeType          = TranslateResourceType[resourceType];
      rangeInfo[i].NumDescriptors     = group[i].count;     // UINT - -1 or UINT_MAX to specify unbounded size (only last entry)

      // Storage buffers share register pool with Uniform buffers (as CBV)
      if (group[i].type == ResourceType::Storage)
         resourceType--;

      // Register Slots are assigned in order of Resource Group declarations.
      // Register Space is set during Root Signature creation, in order in
      // which Descriptor Sets are assigned to Pipeline Layout.
      rangeInfo[i].BaseShaderRegister = slot[resourceType];
      rangeInfo[i].RegisterSpace      = 0u;                 // To be patched.

      // Increase index by amount of slots used
      slot[resourceType] += group[i].count;

      // RootSignature 1.0:
      //
      // - Assumes Descriptors and Data are both Volatile
      //
      // RootSignature 1.1 new feature:
      // (see for details: https://msdn.microsoft.com/en-us/library/windows/desktop/mt709473(v=vs.85).aspx )
      //
      //rangeInfo[i].Flags              = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;  // Descriptors Static, all Data static at execute (except UAV's)
      //
      // D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE              
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE                     // Data changes on the fly (UAV's)
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE  // Data changes when not executing
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC                       // Data never changes

      // UINT - offset in descriptors from the start of the root signature
      // D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND - immediatelly follow previous one
      rangeInfo[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      }

   // Descriptor Ranges Table
   result->table.NumDescriptorRanges = count;
   result->table.pDescriptorRanges   = rangeInfo;

   // Shader visibility
   //
   // D3D12 is not supporting visibility selection for multiple shaders
   // stages, except of all of them. Therefore in case that more than
   // one (but not all) shader stages are selected, we emulate this
   // behavior by falling back to "All" option.
   result->visibility = D3D12_SHADER_VISIBILITY_ALL;
   switch(underlyingType(stageMask))
      {
      case ShaderStages::Vertex:
         result->visibility = D3D12_SHADER_VISIBILITY_VERTEX;
         break;

      case ShaderStages::Control:
         result->visibility = D3D12_SHADER_VISIBILITY_HULL;
         break;
         
      case ShaderStages::Evaluation:
         result->visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
         break;
         
      case ShaderStages::Geometry:
         result->visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
         break;
         
      case ShaderStages::Fragment:
         result->visibility = D3D12_SHADER_VISIBILITY_PIXEL;
         break;
         
      default:
         break;
      };
      
   delete [] slot;

   return ptr_reinterpret_cast<SetLayout>(&result);
   }

   Ptr<PipelineLayout> Direct3D12Device::createPipelineLayout(const uint32 sets,
                                                              const Ptr<SetLayout>* set,
                                                              const uint32 immutableSamplers,
                                                              const Ptr<Sampler>* sampler,
                                                              const ShaderStages stageMask)
   {
   Ptr<PipelineLayoutD3D12> result = nullptr; 

   // Root Signature 1.1:
   // https://msdn.microsoft.com/en-us/library/windows/desktop/mt709473(v=vs.85).aspx
   // Specifies when descriptors are static, and when their data is static. And when they are volatile.

   bool allStages = false;
   bool stageUsesDescriptors[5] = { false, false, false, false, false };

   // Gather immutable sampler states.
   D3D12_STATIC_SAMPLER_DESC* samplers = nullptr;
   if (immutableSamplers)
      {
      // D3D12 is not supporting visibility selection for multiple shaders
      // stages, except of all of them. Therefore in case that more than
      // one (but not all) shader stages are selected, we emulate this
      // behavior by falling back to "All" option.
      D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
      switch(underlyingType(stageMask))
         {
         case ShaderStages::Vertex:
            visibility = D3D12_SHADER_VISIBILITY_VERTEX;
            stageUsesDescriptors[0] = true;
            break;

         case ShaderStages::Control:
            visibility = D3D12_SHADER_VISIBILITY_HULL;
            stageUsesDescriptors[1] = true;
            break;
            
         case ShaderStages::Evaluation:
            visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
            stageUsesDescriptors[2] = true;
            break;
            
         case ShaderStages::Geometry:
            visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
            stageUsesDescriptors[3] = true;
            break;
            
         case ShaderStages::Fragment:
            visibility = D3D12_SHADER_VISIBILITY_PIXEL;
            stageUsesDescriptors[4] = true;
            break;
            
         default:
            allStages = true;
            break;
         };
         
      samplers = new D3D12_STATIC_SAMPLER_DESC[immutableSamplers];
      for(uint32 i=0; i<immutableSamplers; ++i)
         {
         SamplerD3D12* ptr = raw_reinterpret_cast<SamplerD3D12>(&sampler);

         // While Vulkan uses the same descriptor for dynamic Samplers and
         // immutable Samplers, D3D12 uses two separate structures for those.
         // D3D12_SAMPLER_DESC        - dynamic Sampler
         // D3D12_STATIC_SAMPLER_DESC - immutable Sampler
         // So we need to copy data from one to another :/. Alternative would
         // be to introduce separate immutable sampler objects on API level,
         // but that sounds awkward.
         //
         samplers[i].Filter           = ptr->state.Filter;
         samplers[i].AddressU         = ptr->state.AddressU;
         samplers[i].AddressV         = ptr->state.AddressV;
         samplers[i].AddressW         = ptr->state.AddressW;
         samplers[i].MipLODBias       = ptr->state.MipLODBias;
         samplers[i].MaxAnisotropy    = ptr->state.MaxAnisotropy;
         samplers[i].ComparisonFunc   = ptr->state.ComparisonFunc;
         samplers[i].BorderColor      = ptr->border;
         samplers[i].MinLOD           = ptr->state.MinLOD;
         samplers[i].MaxLOD           = ptr->state.MaxLOD;
         samplers[i].ShaderRegister   = i; // All Immutable Samplers are enumerated in the order they are provided
         samplers[i].RegisterSpace    = // UINT
         samplers[i].ShaderVisibility = visibility;
         }
      }
   
   // Gather Descriptor Tables
   D3D12_ROOT_PARAMETER* tables = nullptr;
 //D3D12_ROOT_PARAMETER1* tables = nullptr; // v1.1
   if (sets)
      {

      tables = new D3D12_ROOT_PARAMETER[sets];
    //tables = new D3D12_ROOT_PARAMETER1[sets]; // v1.1
      for(uint32 i=0; i<sets; ++i)
         {
         SetLayoutD3D12* ptr = raw_reinterpret_cast<SetLayoutD3D12>(&set[i]);
         
         tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
         tables[i].DescriptorTable  = ptr->table;
         tables[i].ShaderVisibility = ptr->visibility;

         // Patch Descriptor Table spaces
         // HLSL Register Space matches SPIRV Set indexing
         uint32 ranges = ptr->table.NumDescriptorRanges;
         for(uint32 j=0; i<ranges; ++j)
            ptr->table.pDescriptorRanges[j].RegisterSpace = i;

       //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
       //tables[i].Constants        = //D3D12_ROOT_CONSTANTS;

       //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_CBV;
       //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_SRV;
       //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_UAV;
       //tables[i].Descriptor       = //D3D12_ROOT_DESCRIPTOR;
       //   UINT ShaderRegister;
       //   UINT RegisterSpace;
            
         // Record shader stages that refer to any descriptors
         if (ptr->visibility == D3D12_SHADER_VISIBILITY_ALL)
            allStages = true;
         else
            stageUsesDescriptors[static_cast<uint32>(ptr->visibility) - 1] = true;
         }
      }


   D3D12_ROOT_SIGNATURE_DESC desc;
 //D3D12_ROOT_SIGNATURE_DESC1 desc; // v1.1
   desc.NumParameters     = sets;
   desc.pParameters       = tables;
   desc.NumStaticSamplers = immutableSamplers;
   desc.pStaticSamplers   = samplers;
   desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
   
   // Don't use this flag to get small optimization on Programmable Vertex Fetch shaders
   desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

   if (!allStages)
      {
      // Optimize pipeline layout by preventin wiring down descriptors
      // to stages that never access them in any way. It will also deny
      // access to all shader stages in "default" Root Signature.
      if (!stageUsesDescriptors[0]) desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[1]) desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[2]) desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[3]) desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[4]) desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
      }

   // TODO: Add support for StreamOut
   // D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT
  
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

   delete [] tables;
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
