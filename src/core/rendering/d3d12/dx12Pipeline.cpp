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

#include "core/rendering/d3d12/dx12InputLayout.h"
#include "core/rendering/d3d12/dx12RenderPass.h"
#include "core/rendering/d3d12/dx12Blend.h"
#include "core/rendering/d3d12/dx12Multisampling.h"
#include "core/rendering/d3d12/dx12Shader.h"
#include "core/rendering/d3d12/dx12Layout.h"
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
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // LineStripes
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // Triangles
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // TriangleStripes
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH    , // Patches
      };

   static const D3D_PRIMITIVE_TOPOLOGY TranslatePrimitiveTopology[underlyingType(DrawableType::DrawableTypesCount)] =
      {
      D3D_PRIMITIVE_TOPOLOGY_POINTLIST                 ,  // Points
      D3D_PRIMITIVE_TOPOLOGY_LINELIST                  ,  // Lines
      D3D_PRIMITIVE_TOPOLOGY_LINESTRIP                 ,  // LineStripes
      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST              ,  // Triangles
      D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP             ,  // TriangleStripes
      D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST    // Patches (need to explicitly state patch size)
      };

   // D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ
   // D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ
   // D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ
   // D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ

   PipelineD3D12::PipelineD3D12(Direct3D12Device* _gpu, ID3D12PipelineState* _handle, shared_ptr<PipelineLayoutD3D12> _layout) :
      gpu(_gpu),
      handle(_handle),
      layout(_layout),
      topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED),
      stencilRef(0)
   {
   }

   PipelineD3D12::~PipelineD3D12()
   {
   layout = nullptr;

   assert( handle );
   ValidateCom(  handle->Release() )
   handle = nullptr;
   }

   shared_ptr<Pipeline> Direct3D12Device::createPipeline(const PipelineState& pipelineState)
   {
   shared_ptr<PipelineD3D12> result = nullptr;

   // Pipeline object is always created against Render Pass, and app responsibility is to
   // provide missing states (ViewportState, Shaders).
   assert( pipelineState.renderPass );
   assert( pipelineState.viewportState );

   // Cast to D3D12 states
   const RenderPassD3D12*         renderPass     = reinterpret_cast<RenderPassD3D12*>(pipelineState.renderPass.get());

   const InputLayoutD3D12*        input          = pipelineState.inputLayout ? reinterpret_cast<InputLayoutD3D12*>(pipelineState.inputLayout.get())
                                                                             : reinterpret_cast<InputLayoutD3D12*>(defaultState->inputLayout.get());

   const ViewportStateD3D12*      viewport       = reinterpret_cast<ViewportStateD3D12*>(pipelineState.viewportState.get());

   const RasterStateD3D12*        raster         = pipelineState.rasterState ? reinterpret_cast<RasterStateD3D12*>(pipelineState.rasterState.get())
                                                                             : reinterpret_cast<RasterStateD3D12*>(defaultState->rasterState.get());

   const MultisamplingStateD3D12* multisampling  = pipelineState.multisamplingState ? reinterpret_cast<MultisamplingStateD3D12*>(pipelineState.multisamplingState.get())
                                                                                    : reinterpret_cast<MultisamplingStateD3D12*>(defaultState->multisamplingState.get());
      
   const DepthStencilStateD3D12*  depthStencil   = pipelineState.depthStencilState ? reinterpret_cast<DepthStencilStateD3D12*>(pipelineState.depthStencilState.get())
                                                                                   : reinterpret_cast<DepthStencilStateD3D12*>(defaultState->depthStencilState.get());

   const BlendStateD3D12*         blend          = pipelineState.blendState ? reinterpret_cast<BlendStateD3D12*>(pipelineState.blendState.get())
                                                                            : reinterpret_cast<BlendStateD3D12*>(defaultState->blendState.get());

   shared_ptr<PipelineLayoutD3D12> layout        = pipelineState.pipelineLayout ? dynamic_pointer_cast<PipelineLayoutD3D12>(pipelineState.pipelineLayout)
                                                                                : dynamic_pointer_cast<PipelineLayoutD3D12>(defaultState->pipelineLayout);

   // Count amount of shader stages in use
   uint32 stages = 0;
   for(uint32 i=0; i<5; ++i)
      if (pipelineState.shader[i])
         stages++;

   assert( stages > 0 );

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
   desc.pRootSignature        = layout->handle;
   desc.VS                    = pipelineState.shader[0] ? reinterpret_cast<ShaderD3D12*>(pipelineState.shader[0].get())->state : noShader;
   desc.PS                    = pipelineState.shader[4] ? reinterpret_cast<ShaderD3D12*>(pipelineState.shader[4].get())->state : noShader;
   desc.DS                    = pipelineState.shader[2] ? reinterpret_cast<ShaderD3D12*>(pipelineState.shader[2].get())->state : noShader;
   desc.HS                    = pipelineState.shader[1] ? reinterpret_cast<ShaderD3D12*>(pipelineState.shader[1].get())->state : noShader;
   desc.GS                    = pipelineState.shader[3] ? reinterpret_cast<ShaderD3D12*>(pipelineState.shader[3].get())->state : noShader;
   
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
   desc.IBStripCutValue       = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
   if (input->restart &&
       (input->primitive == LineStripes ||
        input->primitive == TriangleStripes))
      desc.IBStripCutValue    = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
                             // What happens if IBO uses u16 indexes?
                             // Do we need to distinguish u16 and u32 types?
                             // D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
   desc.PrimitiveTopologyType = TranslateDrawableTopology[underlyingType(input->primitive)];
   desc.NumRenderTargets      = 0;
   if (highestBit(renderPass->usedAttachments, desc.NumRenderTargets))
      desc.NumRenderTargets++;
   for(uint32 i=0; i<support.maxColorAttachments; ++i)
      desc.RTVFormats[i]      = renderPass->colorState[i].format; // Copies format from set attachments, unset ones are init to DXGI_FORMAT_UNKNOWN
   desc.DSVFormat             = renderPass->depthState.format;
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

   // TODO: Use PSO cache/library
   //       https://msdn.microsoft.com/en-us/library/windows/desktop/mt709145(v=vs.85).aspx
   // ID3D12PipelineLibrary* lib;
   // SIZE_T cacheSize = lib->GetSerializedSize();
   // uint8* buffer = new uint8[cacheSize];
   // HRESULT lib->Serialize(buffer, cacheSize);
   //HRESULT lib->LoadGraphicsPipeline(
   //  [in]        LPCWSTR                            pName,  // Unique PSO name
   //  [in]  const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pDesc,
   //              REFIID                             riid,
   //  [out]       void                               **ppPipelineState  IID_PPV_ARGS(&pipeline)) ) // __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&pipeline)
   //);
   //HRESULT lib->StorePipeline(
   //  [in, optional] LPCWSTR             pName,   // Unique PSO name
   //  [in]           ID3D12PipelineState *pPipeline
   //);

   // Create pipeline state object
   ID3D12PipelineState* pipeline = nullptr;
   Validate( this, CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline)) ) // __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&pipeline)
   if (SUCCEEDED(lastResult[Scheduler.core()]))
      {
      result = make_shared<PipelineD3D12>(this, pipeline, layout);
      
      // Defer dynamic state: Viewport & Scissor State 
      const ViewportStateD3D12* viewport = reinterpret_cast<ViewportStateD3D12*>(pipelineState.viewportState.get());
      memcpy(&result->viewport[0], &viewport->viewport[0], viewport->count * sizeof(D3D12_VIEWPORT));
      memcpy(&result->scissor[0],  &viewport->scissor[0],  viewport->count * sizeof(D3D12_RECT));
      result->viewportsCount = viewport->count;
      
      // Defer dynamic state: Blend Color
      result->blendColor[0] = blend->blendColor.r;
      result->blendColor[1] = blend->blendColor.g;
      result->blendColor[2] = blend->blendColor.b;
      result->blendColor[3] = blend->blendColor.a;
      
      // Defer dynamic state: Stencil Reference
      result->stencilRef = depthStencil->reference;

      // Defer dynamic state: Primitve Type
      result->topology = TranslatePrimitiveTopology[underlyingType(input->primitive)];
      if (input->primitive == DrawableType::Patches)
         result->topology = (D3D_PRIMITIVE_TOPOLOGY)(underlyingType(result->topology) + input->points - 1);
      }

   return result;
   }

   }
}
#endif
