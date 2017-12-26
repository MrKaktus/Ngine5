/*

 Ngine v5.0
 
 Module      : Metal Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlPipeline.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/log/log.h"
#include "utilities/osxStrings.h"

#include "core/rendering/metal/mtlInputLayout.h"
#include "core/rendering/metal/mtlRenderPass.h"
#include "core/rendering/metal/mtlBlend.h"
#include "core/rendering/metal/mtlMultisampling.h"
#include "core/rendering/metal/mtlShader.h"
#include "core/rendering/metal/mtlTexture.h"
#include "core/rendering/metal/mtlLayout.h"

#include "core/rendering/metal/mtlCommandBuffer.h"   // for CommandBuffer::set(Pipeline)

#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   // Topology of primitives to draw
   const MTLPrimitiveTopologyClass TranslateDrawableTopology[DrawableTypesCount]
      {
      MTLPrimitiveTopologyClassPoint       , // Points
      MTLPrimitiveTopologyClassLine        , // Lines
      MTLPrimitiveTopologyClassLine        , // LineStripes
      MTLPrimitiveTopologyClassTriangle    , // Triangles
      MTLPrimitiveTopologyClassTriangle    , // TriangleStripes
      MTLPrimitiveTopologyClassUnspecified , // Patches         (unsupported)
      };

   // Types of primitives to draw
   const MTLPrimitiveType TranslateDrawableType[DrawableTypesCount]
      {
      MTLPrimitiveTypePoint                , // Points
      MTLPrimitiveTypeLine                 , // Lines
      MTLPrimitiveTypeLineStrip            , // LineStripes
      MTLPrimitiveTypeTriangle             , // Triangles
      MTLPrimitiveTypeTriangleStrip        , // TriangleStripes
      MTLPrimitiveTypeTriangleStrip        , // Patches         (unsupported)
      };

   PipelineMTL::PipelineMTL(const id<MTLDevice> device, MTLRenderPipelineDescriptor* desc, NSError** result) :
      handle([device newRenderPipelineStateWithDescriptor:desc error:result]),
      depthStencil(nullptr),
      raster(RasterStateInfo()),
      viewport()
   {
   }
   
   PipelineMTL::~PipelineMTL()
   {
   depthStencil = nullptr;
   
   deallocateObjectiveC(handle);
   }
 
   shared_ptr<Pipeline> MetalDevice::createPipeline(const PipelineState& pipelineState)
   {
   shared_ptr<PipelineMTL> pipeline = nullptr;

   // Pipeline object is always created against Render Pass, and app responsibility is to
   // provide missing states (ViewportState, Shaders).
   assert( pipelineState.renderPass );
   assert( pipelineState.viewportState );

   // Cast to D3D12 states
   const RenderPassMTL*         renderPass     = reinterpret_cast<RenderPassMTL*>(pipelineState.renderPass.get());

   const InputLayoutMTL*        input          = pipelineState.inputLayout ? reinterpret_cast<InputLayoutMTL*>(pipelineState.inputLayout.get())
                                                                           : reinterpret_cast<InputLayoutMTL*>(defaultState->inputLayout.get());

   const RasterStateMTL*        raster         = pipelineState.rasterState ? reinterpret_cast<RasterStateMTL*>(pipelineState.rasterState.get())
                                                                           : reinterpret_cast<RasterStateMTL*>(defaultState->rasterState.get());

   const MultisamplingStateMTL* multisampling  = pipelineState.multisamplingState ? reinterpret_cast<MultisamplingStateMTL*>(pipelineState.multisamplingState.get())
                                                                                  : reinterpret_cast<MultisamplingStateMTL*>(defaultState->multisamplingState.get());
      
   const BlendStateMTL*         blend          = pipelineState.blendState ? reinterpret_cast<BlendStateMTL*>(pipelineState.blendState.get())
                                                                          : reinterpret_cast<BlendStateMTL*>(defaultState->blendState.get());

   const PipelineLayoutMTL*     layout         = pipelineState.pipelineLayout ? reinterpret_cast<PipelineLayoutMTL*>(pipelineState.pipelineLayout.get())
                                                                              : reinterpret_cast<PipelineLayoutMTL*>(defaultState->pipelineLayout.get());

   // Minimum Vertex Shader is required (Tessellation and Geometry Shaders are not supported by Metal)
   assert( pipelineState.shader[0] );
   assert( pipelineState.shader[1] == nullptr );
   assert( pipelineState.shader[2] == nullptr );
   assert( pipelineState.shader[3] == nullptr );
		    
   // Fragment Shader is optional if rasterization is disabled
   assert( pipelineState.shader[4] ||
           (!pipelineState.shader[4] && !raster->enableRasterization) );
   
   // Extract entry point functions from shader libraries
   // TODO: This may not be optimal?
   NSError* error = nil;
   string entrypoint = pipelineState.function[0];
   ShaderMTL* vertexShader = reinterpret_cast<ShaderMTL*>(pipelineState.shader[0].get());
   id<MTLFunction> functionVertex = [vertexShader->library newFunctionWithName:stringTo_NSString(entrypoint)];
   if (error)
      {
      Log << "Error! Failed to find shader entry point \"" << entrypoint << "\" in library created from source.\n";
      return shared_ptr<Pipeline>(nullptr);
      }
      
   entrypoint = pipelineState.function[4];
   id<MTLFunction> functionFragment = nil;
   if (pipelineState.shader[4])
      {
      ShaderMTL* fragmentShader = reinterpret_cast<ShaderMTL*>(pipelineState.shader[4].get());
      functionFragment = [fragmentShader->library newFunctionWithName:stringTo_NSString(entrypoint)];
      if (error)
         {
         Log << "Error! Failed to find shader entry point \"" << entrypoint << "\" in library created from source.\n";
         
         deallocateObjectiveC(functionVertex);
            
         return shared_ptr<Pipeline>(nullptr);
         }
      }
  
   // Pipeline state
   MTLRenderPipelineDescriptor* pipeDesc = allocateObjectiveC(MTLRenderPipelineDescriptor);
   pipeDesc.vertexFunction               = functionVertex;
   pipeDesc.fragmentFunction             = pipelineState.shader[4] ? functionFragment : nil;
   pipeDesc.vertexDescriptor             = input->desc;
   pipeDesc.sampleCount                  = 1;
   pipeDesc.alphaToCoverageEnabled       = NO;
   pipeDesc.alphaToOneEnabled            = NO;
   pipeDesc.rasterizationEnabled         = raster->enableRasterization; // Optional Rasterization State
   for(uint32 i=0; i<8; ++i)                                  // TODO: Change 8 to support.maxColorAttachments
      pipeDesc.colorAttachments[i]       = blend->blendInfo[i];  // Copy descriptors
   pipeDesc.depthAttachmentPixelFormat   = MTLPixelFormatInvalid;
   pipeDesc.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;
   pipeDesc.inputPrimitiveTopology       = MTLPrimitiveTopologyClassUnspecified;
   
   // Required Pipeline State depending from Render Pass
   for(uint32 i=0; i<8; ++i)                                  // TODO: Change 8 to support.maxColorAttachments
      pipeDesc.colorAttachments[i].pixelFormat = TranslateTextureFormat[underlyingType(renderPass->format[i])];

   // Optional Multisample State
   if (multisampling != reinterpret_cast<MultisamplingStateMTL*>(defaultState->multisamplingState.get()))
      {
      pipeDesc.sampleCount               = multisampling->samples;
      pipeDesc.alphaToCoverageEnabled    = multisampling->alphaToCoverage;
      pipeDesc.alphaToOneEnabled         = multisampling->alphaToOne;
      }
      
   // Optional Pipeline State depending from Pipeline Layout
   for(uint32 i=0; i<layout->setsCount; ++i)
      {
      // TODO: Extract DescriptorSets mutability from PipelineLayout
      assert( i < 30 );
      pipeDesc.vertexBuffers[30-i].mutability   = MTLMutabilityImmutable; //layout->
      pipeDesc.fragmentBuffers[30-i].mutability = MTLMutabilityImmutable; //layout->
      }
   
   // Optional Pipeline State depending from Render Pass
   pipeDesc.depthAttachmentPixelFormat   = TranslateTextureFormat[underlyingType(renderPass->format[8])];
   pipeDesc.stencilAttachmentPixelFormat = TranslateTextureFormat[underlyingType(renderPass->format[9])];
      
   // TODO: !! This is stored now in Framebuffer !
   if (renderPass->desc.renderTargetArrayLength > 0) // Metal 1.0 for OSX - Default is unspecified, but needs to be set, when layered rendering is performed.
      pipeDesc.inputPrimitiveTopology       = TranslateDrawableTopology[input->primitive];

   // Create Pipeline
   error = nil;
   pipeline = make_shared<PipelineMTL>(device, pipeDesc, &error);
   
   deallocateObjectiveC(pipeDesc);
      
//typedef NS_ENUM(NSUInteger, MTLCompilerError) {
//    MTLCompilerErrorNoError = 0,
//    MTLCompilerErrorFatalError = 1,
//    MTLCompilerErrorCompilationError = 2,
//};

   if (error)
      {
      Log << "Error! Failed to create pipeline. Error code %u\n" << [error code];
      Log << [[error description] UTF8String] << endl;
      return shared_ptr<Pipeline>(nullptr);
      }
   else // Populate Pipeline with Metal dynamic states
      {
      pipeline->depthStencil = dynamic_pointer_cast<DepthStencilStateMTL>(pipelineState.depthStencilState);
      pipeline->raster       = *reinterpret_cast<RasterStateMTL*>(pipelineState.rasterState.get());
      pipeline->viewport     = *reinterpret_cast<ViewportStateMTL*>(pipelineState.viewportState.get());
      pipeline->primitive    = TranslateDrawableType[input->primitive];
      }

   return pipeline;
   }



   void CommandBufferMTL::setPipeline(const Pipeline& _pipeline)
   {
   const PipelineMTL& pipeline = reinterpret_cast<const PipelineMTL&>(_pipeline);
   
   [renderEncoder setRenderPipelineState: pipeline.handle];
   
   // Dynamic States:
   
   // Metal needs primitive topology type to be specified at
   // PSO creation time, when layered rendering is enabled, and
   // can specify primitive adjacency and ordering at Draw time.
   //
   // D3D12 always need to specify primitive topology type at
   // PSO creation, but still can specify primitive adjacency
   // and ordering at Draw time.
   //
   // Vulkan on the other end is most restrictive, it cannot
   // dynamically change drawn primitive type, as it needs
   // primitive topology, adjacency and ordering info at PSO
   // creation time.
   //
   // Thus Vulkan behavior needs to be followed.
   primitive = pipeline.primitive;

   // Rasterization State
   [renderEncoder setFrontFacingWinding: pipeline.raster.frontFace];
   [renderEncoder setCullMode: pipeline.raster.culling];
   [renderEncoder setDepthClipMode: pipeline.raster.depthClamp]; // IOS 9.0+
   [renderEncoder setDepthBias: pipeline.raster.depthBiasConstantFactor
                    slopeScale: pipeline.raster.depthBiasSlopeFactor
                         clamp: pipeline.raster.depthBiasClamp];
   [renderEncoder setTriangleFillMode: pipeline.raster.fillMode];

   // Depth-Stencil State
   [renderEncoder setDepthStencilState:pipeline.depthStencil->state];
 //if (!support.separateStencilReferenceValue)
 //   [renderEncoder setStencilReferenceValue: pipeline.depthStencil->reference.x ];
 //else
      [renderEncoder setStencilFrontReferenceValue: pipeline.depthStencil->reference.x    // IOS 9.0+
                                backReferenceValue: pipeline.depthStencil->reference.y];

   // Scissor & Viewport (TODO: in which order is executed ?)
   [renderEncoder setViewport:pipeline.viewport.viewport];
   [renderEncoder setScissorRect:pipeline.viewport.scissor];
   }


   }
}

#endif
