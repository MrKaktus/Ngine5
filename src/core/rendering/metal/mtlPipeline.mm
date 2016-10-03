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

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/log/log.h"
#include "utilities/osxStrings.h"

#include "core/rendering/metal/mtlPipeline.h"
#include "core/rendering/metal/mtlInputAssembler.h"
#include "core/rendering/metal/mtlRenderPass.h"
#include "core/rendering/metal/mtlBlend.h"
#include "core/rendering/metal/mtlMultisampling.h"
#include "core/rendering/metal/mtlShader.h"

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
      MTLPrimitiveTopologyClassLine        , // LineLoops       (unsupported)
      MTLPrimitiveTopologyClassLine        , // LineStripes
      MTLPrimitiveTopologyClassTriangle    , // Triangles
      MTLPrimitiveTopologyClassTriangle    , // TriangleFans    (unsupported)
      MTLPrimitiveTopologyClassTriangle    , // TriangleStripes
      MTLPrimitiveTopologyClassUnspecified , // Patches         (unsupported)
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
   [handle release];
   depthStencil = nullptr;
   }
 
   Ptr<Pipeline> MetalDevice::createPipeline(const PipelineState& pipelineState)
   {
   Ptr<PipelineMTL> pipeline = nullptr;

   // Required States
   assert( pipelineState.renderPass );
   assert( pipelineState.inputAssembler );
   assert( pipelineState.blendState );
   assert( pipelineState.viewportState );
   assert( pipelineState.shader[0] );
   
   // Fragment Shader is optional if rasterization is disabled
   const Ptr<RasterStateMTL> rasterizer = ptr_reinterpret_cast<RasterStateMTL>(&pipelineState.rasterState);
   assert( pipelineState.shader[4] ||
           (!pipelineState.shader[4] && pipelineState.rasterState &&
            !ptr_reinterpret_cast<RasterStateMTL>(&pipelineState.rasterState)->enableRasterization) );
   
   // Cast to Metal states
   const Ptr<RenderPassMTL>     pass     = ptr_reinterpret_cast<RenderPassMTL>(&pipelineState.renderPass);
   const Ptr<InputAssemblerMTL> input    = ptr_reinterpret_cast<InputAssemblerMTL>(&pipelineState.inputAssembler);
   const Ptr<BlendStateMTL>     blend    = ptr_reinterpret_cast<BlendStateMTL>(&pipelineState.blendState);

   // Extract entry point functions from shader libraries
   // TODO: This may not be optimal?
   NSError* error = nil;
   string entrypoint = pipelineState.function[0];
   Ptr<ShaderMTL> vertexShader = ptr_reinterpret_cast<ShaderMTL>(&pipelineState.shader[0]);
   id <MTLFunction> functionVertex = [vertexShader->library newFunctionWithName:stringTo_NSString(entrypoint)];
   if (error)
      {
      Log << "Error! Failed to find shader entry point \"" << entrypoint << "\" in library created from source.\n";
      return Ptr<Pipeline>(nullptr);
      }
      
   entrypoint = pipelineState.function[4];
   id <MTLFunction> functionFragment = nil;
   if (pipelineState.shader[4])
      {
      Ptr<ShaderMTL> fragmentShader = ptr_reinterpret_cast<ShaderMTL>(&pipelineState.shader[4]);
      functionFragment = [fragmentShader->library newFunctionWithName:stringTo_NSString(entrypoint)];
      if (error)
         {
         Log << "Error! Failed to find shader entry point \"" << entrypoint << "\" in library created from source.\n";
         [functionVertex release];
         return Ptr<Pipeline>(nullptr);
         }
      }
      
   // Pipeline state
   MTLRenderPipelineDescriptor* pipeDesc = [[MTLRenderPipelineDescriptor alloc] init];
   pipeDesc.vertexFunction               = functionVertex;
   pipeDesc.fragmentFunction             = pipelineState.shader[4] ? functionFragment : nil;
   pipeDesc.vertexDescriptor             = input->desc;
   pipeDesc.sampleCount                  = 1;
   pipeDesc.alphaToCoverageEnabled       = NO;
   pipeDesc.alphaToOneEnabled            = NO;
   pipeDesc.rasterizationEnabled         = YES;
   for(uint32 i=0; i<8; ++i)                                  // TODO: Change 8 to support.maxColorAttachments
      pipeDesc.colorAttachments[i]       = blend->blendInfo[i];  // Copy descriptors
   pipeDesc.depthAttachmentPixelFormat   = MTLPixelFormatInvalid;
   pipeDesc.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;
   pipeDesc.inputPrimitiveTopology       = MTLPrimitiveTopologyClassUnspecified;
    
   // Required Pipeline State depending from Render Pass
   for(uint32 i=0; i<8; ++i)                                  // TODO: Change 8 to support.maxColorAttachments
      pipeDesc.colorAttachments[i].pixelFormat = [pass->desc.colorAttachments[i].texture pixelFormat];
      
   // Optional Multisample State
   if (pipelineState.multisamplingState)
      {
      const Ptr<MultisamplingStateMTL> multisampling = ptr_reinterpret_cast<MultisamplingStateMTL>(&pipelineState.multisamplingState);

      pipeDesc.sampleCount               = multisampling->samples;
      pipeDesc.alphaToCoverageEnabled    = multisampling->alphaToCoverage;
      pipeDesc.alphaToOneEnabled         = multisampling->alphaToOne;
      }
      
   // Optional Rasterization State
   if (pipelineState.rasterState)
      {
      pipeDesc.rasterizationEnabled      = rasterizer->enableRasterization;
      }
   
   // Optional Pipeline State depending from Render Pass
   if (pass->desc.depthAttachment.texture)
      pipeDesc.depthAttachmentPixelFormat   = [pass->desc.depthAttachment.texture pixelFormat];
   if (pass->desc.stencilAttachment.texture)
      pipeDesc.stencilAttachmentPixelFormat = [pass->desc.stencilAttachment.texture pixelFormat];
   if (pass->desc.renderTargetArrayLength > 0) // Metal 1.0 for OSX - Default is unspecified, but needs to be set, when layered rendering is performed.
      pipeDesc.inputPrimitiveTopology       = TranslateDrawableTopology[input->primitive];

   // Create Pipeline
   error = nullptr;
   pipeline = new PipelineMTL(device, pipeDesc, &error);
   [pipeDesc release];

//typedef NS_ENUM(NSUInteger, MTLCompilerError) {
//    MTLCompilerErrorNoError = 0,
//    MTLCompilerErrorFatalError = 1,
//    MTLCompilerErrorCompilationError = 2,
//};

   if (error)
      {
      Log << "Error! Failed to create pipeline. Error code %u\n" << [error code];
      Log << [[error description] UTF8String] << endl;
      return Ptr<Pipeline>(nullptr);
      }
   else // Populate Pipeline with Metal dynamic states
      {
      pipeline->depthStencil = ptr_reinterpret_cast<DepthStencilStateMTL>(&pipelineState.depthStencilState);
      pipeline->raster       = *ptr_reinterpret_cast<RasterStateMTL>(&pipelineState.rasterState);
      pipeline->viewport     = *ptr_reinterpret_cast<ViewportStateMTL>(&pipelineState.viewportState);
      }

   return ptr_reinterpret_cast<Pipeline>(&pipeline);
   }



   void CommandBufferMTL::set(const Ptr<Pipeline> pipeline)
   {
   Ptr<PipelineMTL> ptr = ptr_dynamic_cast<PipelineMTL, Pipeline>(pipeline);
   
   [renderEncoder setRenderPipelineState: ptr->handle];
   
   // Rasterization State
   [renderEncoder setFrontFacingWinding: ptr->raster.frontFace];
   [renderEncoder setCullMode: ptr->raster.culling];
   [renderEncoder setDepthClipMode: ptr->raster.depthClamp]; // IOS 9.0+
   [renderEncoder setDepthBias: ptr->raster.depthBiasConstantFactor
                    slopeScale: ptr->raster.depthBiasSlopeFactor
                         clamp: ptr->raster.depthBiasClamp];
   [renderEncoder setTriangleFillMode: ptr->raster.fillMode];

   // Depth-Stencil State
   [renderEncoder setDepthStencilState:ptr->depthStencil->state];
 //if (!support.separateStencilReferenceValue)
 //   [renderEncoder setStencilReferenceValue: ptr->depthStencil->reference.x ];
 //else
      [renderEncoder setStencilFrontReferenceValue: ptr->depthStencil->reference.x    // IOS 9.0+
                                backReferenceValue: ptr->depthStencil->reference.y];

   // Scissor & Viewport (TODO: in which order is executed ?)
   [renderEncoder setViewport:ptr->viewport.viewport];
   [renderEncoder setScissorRect:ptr->viewport.scissor];
   }


   }
}

#endif
