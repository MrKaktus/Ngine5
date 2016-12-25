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
    
   PipelineD3D12::PipelineD3D12(Direct3D12Device* _gpu, const ID3D12PipelineState* _handle) :
      gpu(_gpu),
      handle(_handle)
   {
   }

   PipelineD3D12::~PipelineD3D12()
   {
   assert( handle );
   handle->Release();
   handle = nullptr;
   }

   Ptr<Pipeline> Direct3D12Device::createPipeline(const PipelineState& pipelineState)
   {
   Ptr<PipelineD3D12> result = nullptr;

   // Input Assembler State is Required
   assert( pipelineState.inputAssembler );
 
   // Cast to D3D12 states
   const RenderPassD3D12*         renderPass     = raw_reinterpret_cast<RenderPassD3D12>(&pipelineState.renderPass);
   const InputAssemblerD3D12*     input          = raw_reinterpret_cast<InputAssemblerD3D12>(&pipelineState.inputAssembler);

   const RasterStateD3D12*        raster         = pipelineState.rasterState ? raw_reinterpret_cast<RasterStateD3D12>(&pipelineState.rasterState)
                                                                             : raw_reinterpret_cast<RasterStateD3D12>(&defaultState.rasterState);

   const MultisamplingStateD3D12* multisampling  = pipelineState.multisamplingState ? raw_reinterpret_cast<MultisamplingStateD3D12>(&pipelineState.multisamplingState)
                                                                                    : raw_reinterpret_cast<MultisamplingStateD3D12>(&defaultState.multisamplingState);
      
   const DepthStencilStateD3D12*  depthStencil   = pipelineState.depthStencilState ? raw_reinterpret_cast<DepthStencilStateD3D12>(&pipelineState.depthStencilState)
                                                                                   : raw_reinterpret_cast<DepthStencilStateD3D12>(&defaultState.depthStencilState);

   const BlendStateD3D12*         blend          = pipelineState.blendState ? raw_reinterpret_cast<BlendStateD3D12>(&pipelineState.blendState)
                                                                            : raw_reinterpret_cast<BlendStateD3D12>(&defaultState.blendState);

   const PipelineLayoutD3D12*     layout         = raw_reinterpret_cast<PipelineLayoutD3D12>(&pipelineState.pipelineLayout);

   // At this point, those states should be set by app or device (Direct3D12 doesn't allow null states)
   assert( raster );
   assert( multisampling );
   assert( depthStencil );
   assert( blend );

   D3D12_SHADER_BYTECODE noShader;
   noShader.pShaderBytecode = nullptr;
   noShader.BytecodeLength  = 0u;

   // Pipeline descriptor
   D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
  ID3D12RootSignature                *pRootSignature;  // TODO: <--- Layout cast
  
   desc.VS                    = pipelineState.shader[0] ? raw_reinterpret_cast<ShaderD3D12>(&pipelineState.shader[0])->state : noShader;
   desc.PS                    = pipelineState.shader[4] ? raw_reinterpret_cast<ShaderD3D12>(&pipelineState.shader[4])->state : noShader;
   desc.DS                    = pipelineState.shader[2] ? raw_reinterpret_cast<ShaderD3D12>(&pipelineState.shader[2])->state : noShader;
   desc.HS                    = pipelineState.shader[1] ? raw_reinterpret_cast<ShaderD3D12>(&pipelineState.shader[1])->state : noShader;
   desc.GS                    = pipelineState.shader[3] ? raw_reinterpret_cast<ShaderD3D12>(&pipelineState.shader[3])->state : noShader;
   
   // StreamOut is currently unsupported
   desc.StreamOutput.pSODeclaration   = nullptr;
   desc.StreamOutput.NumEntries       = 0u;
   desc.StreamOutput.pBufferStrides   = nullptr;
   desc.StreamOutput.NumStrides       = 0u;
   desc.StreamOutput.RasterizedStream = 0u;
   
   desc.BlendState            = blend->desc;
   desc.SampleMask            = blend->enabledSamples; // Determines for which MSAA samples, blend will be applied, for all enabled RT.
   desc.RasterizerState       = raster->state;
   desc.DepthStencilState     = depthStencil->state;
   desc.InputLayout           = input->state;
   desc.IBStripCutValue       = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
                             // What happens if IBO uses u16 indexes?
                             // Do we need to distinguish u16 and u32 types?
                             // D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
   desc.PrimitiveTopologyType = TranslateDrawableTopology[underlyingType(input->primitive)];
   desc.NumRenderTargets      = renderPass->NumRenderTargets;
   for(uint32 i=0; i<8; ++i)
      desc.RTVFormats[i]      = renderPass->RTVFormats[i];
   desc.DSVFormat             = renderPass->DSVFormat;
   desc.SampleDesc            = multisampling->state;
   desc.NodeMask              = 0u; // Only use for multi-GPU
   desc.CachedPSO.pCachedBlob           = nullptr;
   desc.CachedPSO.CachedBlobSizeInBytes = 0u;
   desc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;

   // Patch BlendingState
   desc.BlendState.AlphaToCoverageEnable      = multisampling->alphaToCoverage;
   
   // Patch RasterizerState
   desc.RasterizerState.MultisampleEnable     = multisampling->state.Count > 1 ? TRUE : FALSE;
   desc.RasterizerState.AntialiasedLineEnable = FALSE; // Currently unsupported (See dx12Raster.cpp).
   desc.RasterizerState.ForcedSampleCount     = 0u;    // Currently unsupported (See dx12Raster.cpp).

   // Create pipeline state object
   ID3D12PipelineState* pipeline;
   Profile( this, CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline)) ) // __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&pipeline)
   if (SUCCEDED(lastResult[Scheduler.core()]))
      {
      result = new PipelineD3D12(pipeline);
      
      // Pass-Through - Dynamic, set on CommandBuffer
      result->blendColor[0] = blend->blendColor.r;
      result->blendColor[1] = blend->blendColor.g;
      result->blendColor[2] = blend->blendColor.b;
      result->blendColor[3] = blend->blendColor.a;
      
      // Viewport State is dynamic 
      const ViewportStateD3D12* viewport = raw_reinterpret_cast<ViewportStateD3D12>(&pipelineState.viewportState);
      memcpy(&result->viewport[0], &viewport->viewport[0], viewport->count * sizeof(D3D12_VIEWPORT));
      memcpy(&result->scissor[0],  &viewport->scissor[0],  viewport->count * sizeof(D3D12_RECT));
      result->viewportsCount = viewport->count;
      
      // TODO: Set on Bind to Command Buffer !!!!!
      // command->OMSetBlendFactor(blendColor);
      // command->RSSetViewports(viewportsCount, &viewport[0]);
      // command->RSSetScissorRects(viewportsCount, &scissor[0]);
      }

   return ptr_reinterpret_cast<Pipeline>(&result);
   }

   }
}
#endif
