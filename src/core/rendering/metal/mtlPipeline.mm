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
 
   




   
   
   


   Ptr<Pipeline> MetalDevice::create(const Ptr<RenderPass>     renderPass,
                                     const Ptr<InputAssembler> inputAssembler,
                                     const Ptr<ViewportState>  viewportState,
                                     const Ptr<RasterState>    rasterState,
                                     const Ptr<MultisamplingState> multisamplingState,
                                     const Ptr<DepthStencilState> depthStencilState,
                                     const Ptr<BlendState>     blendState,
                                     const Ptr<Shader>         vertex,
                                     const Ptr<Shader>         fragment
                                 /*const Ptr<PipelineLayout> pipelineLayout*/)
   {
   Ptr<PipelineMTL> pipeline = nullptr;

   // Required States
   assert( renderPass );
   assert( inputAssembler );
   assert( blendState );
   assert( viewportState );
   assert( vertex );
   
   // Fragment Shader is optional if rasterization is disabled
   const Ptr<RasterStateMTL> rasterizer = ptr_dynamic_cast<RasterStateMTL, RasterState>(rasterState);
   assert( fragment || (!fragment && rasterizer && !rasterizer->enableRasterization) );
   
   // Cast to Metal states
   const Ptr<RenderPassMTL>     pass     = ptr_dynamic_cast<RenderPassMTL,     RenderPass>(renderPass);
   const Ptr<InputAssemblerMTL> input    = ptr_dynamic_cast<InputAssemblerMTL, InputAssembler>(inputAssembler);
   const Ptr<BlendStateMTL>     blend    = ptr_dynamic_cast<BlendStateMTL,     BlendState>(blendState);

   // Pipeline state
   MTLRenderPipelineDescriptor* pipeDesc = [[MTLRenderPipelineDescriptor alloc] init];
   pipeDesc.vertexFunction               = ptr_dynamic_cast<ShaderMTL, Shader>(vertex)->function;
   pipeDesc.fragmentFunction             = fragment ? ptr_dynamic_cast<ShaderMTL, Shader>(fragment)->function : nil;
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
   if (multisamplingState)
      {
      const Ptr<MultisamplingStateMTL> multisampling = ptr_dynamic_cast<MultisamplingStateMTL, MultisamplingState>(multisamplingState);

      pipeDesc.sampleCount               = multisampling->samples;
      pipeDesc.alphaToCoverageEnabled    = multisampling->alphaToCoverage;
      pipeDesc.alphaToOneEnabled         = multisampling->alphaToOne;
      }
      
   // Optional Rasterization State
   if (rasterState)
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
   NSError* error = nullptr;
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
      pipeline->depthStencil = ptr_dynamic_cast<DepthStencilStateMTL, DepthStencilState>(depthStencilState);
      pipeline->raster       = *ptr_dynamic_cast<RasterStateMTL, RasterState>(rasterState);
      pipeline->viewport     = *ptr_dynamic_cast<ViewportStateMTL,  ViewportState>(viewportState);
      }

   return ptr_dynamic_cast<Pipeline, PipelineMTL>(pipeline);
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
