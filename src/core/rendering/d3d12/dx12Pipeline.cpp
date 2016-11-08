/*

 Ngine v5.0
 
 Module      : D3D12 Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Pipeline.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12InputAssembler.h"
#include "core/rendering/d3d12/dx12RenderPass.h"
#include "core/rendering/d3d12/dx12Blend.h"
#include "core/rendering/d3d12/dx12Multisampling.h"
#include "core/rendering/d3d12/dx12Shader.h"

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   {
   // Topology of primitives to draw
   const D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslateDrawableTopology[DrawableTypesCount]
      {
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT    , // Points
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // Lines
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // LineLoops
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // LineStripes
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // Triangles
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // TriangleFans
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // TriangleStripes
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH    , // Patches
      };
    
   PipelineD3D12::PipelineD3D12(Direct3D12Device* _gpu, VkPipeline _handle) :
      gpu(_gpu),
      handle(_handle)
   {
   }

   PipelineD3D12::~PipelineD3D12()
   {

   }

   Ptr<Pipeline> Direct3D12Device::createPipeline(const PipelineState& pipelineState)
   {
   Ptr<PipelineD3D12> result = nullptr;

   // Input Assembler State is Required
   assert( pipelineState.inputAssembler );
 
   // Cast to D3D12 states
   const Ptr<RenderPassD3D12>         renderPass     = ptr_reinterpret_cast<RenderPassD3D12>(&pipelineState.renderPass);
   const Ptr<InputAssemblerD3D12>     input          = ptr_reinterpret_cast<InputAssemblerD3D12>(&pipelineState.inputAssembler);
   const Ptr<ViewportStateD3D12>      viewport       = ptr_reinterpret_cast<ViewportStateD3D12>(&pipelineState.viewportState);
   const Ptr<RasterStateD3D12>        raster         = ptr_reinterpret_cast<RasterStateD3D12>(&pipelineState.rasterState);
   const Ptr<MultisamplingStateD3D12> multisampling  = ptr_reinterpret_cast<MultisamplingStateD3D12>(&pipelineState.multisamplingState);
   const Ptr<DepthStencilStateD3D12>  depthStencil   = ptr_reinterpret_cast<DepthStencilStateD3D12>(&pipelineState.depthStencilState);
   const Ptr<BlendStateD3D12>         blend          = ptr_reinterpret_cast<BlendStateD3D12>(&pipelineState.blendState);

   const Ptr<PipelineLayoutD3D12>     layout         = ptr_reinterpret_cast<PipelineLayoutD3D12>(&pipelineState.pipelineLayout);


   assert( blend );
   assert( raster );
   assert( depthStencil );
   assert( input );
   assert( multisampling );

   // Instead of StreamOut, use UAV's
   D3D12_STREAM_OUTPUT_DESC streamOut;
   // const D3D12_SO_DECLARATION_ENTRY *pSODeclaration;
   // UINT                             NumEntries;
   // const UINT                       *pBufferStrides;
   // UINT                             NumStrides;
   // UINT                             RasterizedStream;

   D3D12_CACHED_PIPELINE_STATE cache;
   cache.pCachedBlob           = nullptr;
   cache.CachedBlobSizeInBytes = 0u;
  
   D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
  ID3D12RootSignature                *pRootSignature;
  D3D12_SHADER_BYTECODE              VS;
  D3D12_SHADER_BYTECODE              PS;
  D3D12_SHADER_BYTECODE              DS;
  D3D12_SHADER_BYTECODE              HS;
  D3D12_SHADER_BYTECODE              GS;
  D3D12_STREAM_OUTPUT_DESC           StreamOutput;
   desc.BlendState            = blend->desc;
  UINT                               SampleMask;
   desc.RasterizerState       = raster->state;
   desc.DepthStencilState     = depthStencil->state;
   desc.InputLayout           = input->state;
   desc.IBStripCutValue       = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
                             // What happens if IBO uses u16 indexes?
                             // Do we need to distinguich u16 and u32 types?
                             // D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
   desc.PrimitiveTopologyType = TranslateDrawableTopology[underlyingType(input->primitive)];
   
  UINT                               NumRenderTargets;
  DXGI_FORMAT                        RTVFormats[8];
  DXGI_FORMAT                        DSVFormat;
  
   desc.SampleDesc            = multisampling->state;
   desc.NodeMask              = 0u; // Only use for multi-GPU
   desc.CachedPSO             = cache;
   desc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;

   // Create pipeline state object
   ID3D12PipelineState* pipeline;
   Profile( this, CreateGraphicsPipelineState(&desc, __uuidof(ID3D12PipelineState), (void**)&pipeline) )
   if (SUCCEDED(lastResult[Scheduler.core()]))
      result = new PipelineD3D12(pipeline);

   return ptr_reinterpret_cast<Pipeline>(&result);
   }







   // TODO: Finish all the stuff
   }
}
#endif
