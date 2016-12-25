//#include "Ngine4/core/defines.h"
//#include "Ngine4/core/types.h"
//#include "Ngine4/core/utilities/TintrusivePointer.h"
//#include "Ngine4/core/rendering/vulkan/vkTexture.h"
//
//#include "vulkan/vulkan.h"
//
//#define MaxInputLayoutAttributesCount 16
//#define MaxColorAttachmentsCount         8
//
//namespace en
//{
//   namespace vulkan
//   {
//   using namespace en::gpu;
//
//   enum LoadOperation
//      {
//      DontLoad           = 0,
//      Load                  ,
//      Clear                 ,
//      LoadOperationsCount
//      };
//   
//   enum StoreOperation
//      {
//      DontStore          = 0,
//      Store                 ,
//      ResolveMSAA           ,
//      StoreOperationsCount
//      };
//   
//   enum ColorMask
//      {
//      ColorMaskRed       = 1,
//      ColorMaskGreen     = 2,
//      ColorMaskBlue      = 4,
//      ColorMaskAlpha     = 8,
//      ColorMaskAll       = 15
//      };
//
//   // Dynamic state objects
//
//
//   class RasterState : public SafeObject
//      {
//      };
//
//   class ViewportScissorState : public SafeObject
//      {
//      };
//
//   class BlendState : public SafeObject
//      {
//      };
//
//
//   // Framebuffer state
//
//
//
//
//   class Framebuffer : public SafeObject
//      {
//      };
//
//
//
//
//   // Dynamic states creation
//
//
//   struct ViewportStateInfo
//      {
//      float4 rect;
//      float2 depthRange;
//      };
//
//   typedef uint32v4 ScissorStateInfo;
//
//   struct RasterStateInfo
//      {
//      float depthBias;
//      float depthBiasClamp;
//      float slopeScaledDepthBias;
//      float pointSize;
//      float pointFadeThreshold;
//      float lineWidth;
//
//      RasterStateInfo();
//      };
//
//   Ptr<RasterState>          Create(const RasterStateInfo raster);
//
//   Ptr<ViewportScissorState> Create(const uint32 count, 
//                                    const ViewportStateInfo* viewports,
//                                    const ScissorStateInfo* scissors);
//   Ptr<DepthStencilState>    Create(const DepthStateInfo depth);
//   Ptr<BlendState>           Create(const float4 blendColor);
//
//
//   // Framebuffer creation
//
//
//   Ptr<ColorAttachment> Create(const Ptr<Texture> color, 
//                               const Ptr<Texture> resolveMsaa  = nullptr, 
//                               const uint32 colorMipMap        = 0,
//                               const uint32 msaaMipMap         = 0,
//                               const uint32 colorLayer         = 0,
//                               const uint32 msaaLayer          = 0,
//                               const uint32 layers             = 1);   // On iOS/OSX only one layer is supported
//
//   Ptr<ColorAttachment> Create(const Ptr<Texture> color, 
//                               const TextureFormat renderFormat,
//                               const Ptr<Texture> resolveMsaa = nullptr, 
//                               const uint32 colorMipMap       = 0,
//                               const uint32 msaaMipMap        = 0,
//                               const uint32 colorLayer        = 0,
//                               const uint32 msaaLayer         = 0,
//                               const uint32 layers            = 1);   // On iOS/OSX only one layer is supported 
//
//   Ptr<DepthStencilAttachment> Create(const Ptr<Texture> depthStencil, 
//                                      const Ptr<Texture> resolveMsaa  = nullptr, 
//                                      const uint32 depthStencilMipMap = 0,
//                                      const uint32 msaaMipMap         = 0,
//                                      const uint32 depthStencilLayer  = 0,
//                                      const uint32 msaaLayer          = 0,
//                                      const uint32 layers             = 1);
//
//
//
//
//
//
//
//LoadOperation  load;
//union {
//      float4 color;
//      uint32 raw;
//      } clear;
//StoreOperation store;
//
//
//
//   //struct SurfaceRange
//   //   {
//   //   uint32v3 position;
//   //   uint32v3 resolution;
//   //   };
//
//   //struct ResourceRange
//   //   {
//   //   uint32 mipmap;
//   //   uint32 mipmapsCount;
//   //   uint32 layer;
//   //   uint32 layersCount;
//   //   };
//
//
//   PFN_vkCreateColorAttachmentView  vkCreateColorAttachmentView  = nullptr;
//   PFN_vkCreateDepthStencilView     vkCreateDepthStencilView     = nullptr;
//   PFN_vkCreateFramebuffer          vkCreateFramebuffer          = nullptr;
//   PFN_vkCreateDynamicViewportState vkCreateDynamicViewportState = nullptr;
//
//   static const VkAttachmentLoadOp TranslateLoadOperation[LoadOperationsCount] =
//      {
//      VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // DontLoad
//      VK_ATTACHMENT_LOAD_OP_LOAD,               // Load
//      VK_ATTACHMENT_LOAD_OP_CLEAR               // Clear
//      };
//   
//   static const VkAttachmentStoreOp TranslateStoreOperation[StoreOperationsCount] = 
//      {
//      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // DontStore
//      VK_ATTACHMENT_STORE_OP_STORE,             // Store
//      VK_ATTACHMENT_STORE_OP_RESOLVE_MSAA       // ResolveMSAA
//      };
//
//   // DX12 Doesn't support Tiled Renderers, and as such, don't have load and store operations.
//
//   static const MTLLoadAction TranslateLoadOperation[LoadOperationsCount] =
//      {
//      MTLLoadActionDontCare,                    // DontLoad
//      MTLLoadActionLoad,                        // Load
//      MTLLoadActionClear                        // Clear
//      };
//   
//   static const MTLStoreAction TranslateStoreOperation[StoreOperationsCount] = 
//      {
//      MTLStoreActionDontCare,                   // DontStore
//      MTLStoreActionStore,                      // Store
//      MTLStoreActionMultisampleResolve          // ResolveMSAA
//      };
//
//
//
//
//   // Direct3D 12 is not supporting Triangle Stripes ???
//   static const D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslateDrawableType[DrawableTypesCount] = 
//      {
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,      // Points
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,       // Lines
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,                        // LineLoops
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,                        // LineStripes
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,   // Triangles
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,                        // TriangleFans
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,                        // TriangleStripes
//      D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH       // Patches
//      };
//
//   // Vulkan is not supporting Line Loops !
//   static const VkPrimitiveTopology TranslateDrawableType[DrawableTypesCount] = 
//      {
//      VK_PRIMITIVE_TOPOLOGY_POINT_LIST,         // Points
//      VK_PRIMITIVE_TOPOLOGY_LINE_LIST,          // Lines
//      0,                                        // LineLoops
//      VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,         // LineStripes
//      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,      // Triangles
//      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,       // TriangleFans
//      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,     // TriangleStripes
//      VK_PRIMITIVE_TOPOLOGY_PATCH               // Patches
//      };
//
//   // VK_PRIMITIVE_TOPOLOGY_LINE_LIST_ADJ
//   // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_ADJ
//   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_ADJ
//   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_ADJ
//
//
//
//
//
//   enum ProvokingVertex
//      {
//      ProvokingVertexFirst     = 0,
//      ProvokingVertexLast         ,
//      ProvokingVertexesCount
//      };
//
//   enum CoordinateOrigin
//      {
//      OriginUpperLeft          = 0,
//      OriginLowerLeft             ,
//      CoordinateOriginsCount
//      };
//
//   static const VkProvokingVertex TranslateProvokingVertex[ProvokingVertexesCount] = 
//      {
//      VK_PROVOKING_VERTEX_FIRST,
//      VK_PROVOKING_VERTEX_LAST  
//      };
//
//   static const VkCoordinateOrigin TranslateCoordinateOrigin[CoordinateOriginsCount] = 
//      {
//      VK_COORDINATE_ORIGIN_UPPER_LEFT,
//      VK_COORDINATE_ORIGIN_LOWER_LEFT
//      };
//
//
//   static const VkCullMode TranslateCullingMethod[FaceChoosesCount] = 
//      {
//      VK_CULL_MODE_FRONT,          // FrontFace
//      VK_CULL_MODE_BACK,           // BackFace
//      VK_CULL_MODE_FRONT_AND_BACK  // BothFaces
//      };
//
//   // VK_CULL_MODE_NONE            <- To disable Culling
//
//   static const VkFrontFace TranslateNormalCalculationMethod[NormalCalculationMethodsCount] =
//      {
//      VK_FRONT_FACE_CW,            // ClockWise
//      VK_FRONT_FACE_CCW            // CounterClockWise
//      };
//
//   static const VkFillMode TranslateFillMode[FillModesCount] =
//      {
//      VK_FILL_MODE_POINTS,         // PointsOnly
//      VK_FILL_MODE_WIREFRAME,      // Wireframe
//      VK_FILL_MODE_SOLID           // Solid
//      };
//
//
//
//
//
//
//
//
//
//
//
//
//   // Dynamic states
//
//
//   class vkRasterState : public RasterState
//      {
//      public:
//      VkDynamicRsState id;
//      };
//
//   class vkViewportScissorState : public ViewportScissorState
//      {
//      VkDynamicVpState id;
//      };
//
//   class vkDepthStencilState : public DepthStencilState
//      {
//      public:
//      VkDynamicDsState id;
//      };
//
//   class vkBlendState : public BlendState
//      {
//      public:
//      VkDynamicCbState id;
//      };
//
//
//
//
//
//
//   // VULKAN RASTER STATE
//   //-------------------------------------------------------------
//
//   RasterStateInfo::RasterStateInfo() :
//      depthBias(0.0f),
//      depthBiasClamp(0.0f),
//      slopeScaledDepthBias(0.0f),
//      pointSize(1.0f),
//      pointFadeThreshold(1.0f),
//      lineWidth(1.0f)
//   {
//   }
//
//   Ptr<RasterState> Create(const RasterStateInfo raster)
//   {
//   Ptr<RasterState> result = nullptr;
//
//   VkDynamicRsStateCreateInfo rasterStateInfo;
//   rasterStateInfo.sType                = VK_STRUCTURE_TYPE_DYNAMIC_RS_STATE_CREATE_INFO;
//   rasterStateInfo.pNext                = nullptr;
//   rasterStateInfo.depthBias            = raster.depthBias;            // Depth value added to given pixel's depth (prevents z-fighting on decals).
//   rasterStateInfo.depthBiasClamp       = raster.depthBiasClamp;       // More about depth bias and depth slopes:
//   rasterStateInfo.slopeScaledDepthBias = raster.slopeScaledDepthBias; // https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
//   rasterStateInfo.pointSize            = raster.pointSize;            // Point Fade Threshold - max value to which point size is clamped (deprecated in 4.2?)
//   rasterStateInfo.pointFadeThreshold   = raster.pointFadeThreshold;   // https://www.opengl.org/registry/specs/ARB/point_sprite.txt
//   rasterStateInfo.lineWidth            = raster.lineWidth;            // https://www.opengl.org/registry/specs/ARB/point_parameters.txt
//
//   VkDynamicRsState rasterState;
//   VkResult res = vkCreateDynamicRasterState( gpu[i].handle, &rasterStateInfo, &rasterState );
//   if (!res)
//      {
//      Ptr<vkRasterState> vkRaster = new vkRasterState();
//      vkRaster->id = rasterState;
//      result = ptr_dynamic_cast<RasterState, vkRasterState>(vkRaster);
//      }
//
//   return result;
//   }
//
//   // VULKAN VIEWPORT-SCISSOR STATE
//   //-------------------------------------------------------------
//
//   // glViewport() does not clip, unlike the viewport in d3d
//   // Set the scissor rect to the viewport unless it is explicitly set smaller to emulate d3d.
//
//   Ptr<ViewportScissorState> Create(const uint32 count, 
//                                    const ViewportStateInfo* viewports,
//                                    const ScissorStateInfo* scissors)
//   {
//   Ptr<ViewportScissorState> result = nullptr;
//
//   VkDynamicVpStateCreateInfo viewportScissorInfo;
//   viewportScissorInfo.sType                   = VK_STRUCTURE_TYPE_DYNAMIC_VP_STATE_CREATE_INFO;
//   viewportScissorInfo.pNext                   = nullptr;
//   viewportScissorInfo.viewportAndScissorCount = count;
//   viewportScissorInfo.pViewports              = reinterpret_cast<const VkViewport*>(&viewports[0]);
//   viewportScissorInfo.pScissors               = reinterpret_cast<const VkRect*>(&scissors[0]);
//
//   VkDynamicVpState viewportScissorState;
//   VkResult res = vkCreateDynamicViewportState( gpu[i].handle, &viewportScissorInfo, &viewportScissorState );
//   if (!res)
//      {
//      Ptr<vkViewportScissorState> vkViewportScissor = new vkViewportScissorState(fbo);
//      vkViewportScissor->id = viewportScissorState;
//      result = ptr_dynamic_cast<ViewportScissorState, vkViewportScissorState>(vkFbo);
//      }
//
//   return result;
//   }
//
//
//
//
//
//
//
//   // VULKAN BLEND STATE
//   //-------------------------------------------------------------
//
//   Ptr<BlendState> Create(const float4 blendColor)
//   {
//   Ptr<BlendState> result = nullptr;
//
//   VkDynamicCbStateCreateInfo blendStateInfo;
//   blendStateInfo.sType         = VK_STRUCTURE_TYPE_DYNAMIC_CB_STATE_CREATE_INFO;
//   blendStateInfo.pNext         = nullptr;
//   blendStateInfo.blendConst[0] = blendColor.r; 
//   blendStateInfo.blendConst[1] = blendColor.g; 
//   blendStateInfo.blendConst[2] = blendColor.b; 
//   blendStateInfo.blendConst[3] = blendColor.a; 
//
//   VkDynamicCbState blendState;
//   VkResult res = vkCreateDynamicColorBlendState( gpu[i].handle, &blendStateInfo, &blendState );
//   if (!res)
//      {
//      Ptr<vkBlendState> vkBlend = new vkBlendState();
//      vkBlend->id = blendState;
//      result = ptr_dynamic_cast<BlendState, vkBlendState>(vkBlend);
//      }
//
//   return result;
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//   
//
//
//
//
//
//
//
//
//
//
//
//   void vkCommandBuffer::bind(const Ptr<RasterState> raster)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_RASTER, 
//                                ptr_dynamic_cast<vkRasterState, RasterState>(raster)->id );
//   }
//
//   void vkCommandBuffer::bind(const Ptr<ViewportScissorState> viewportScissor)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_VIEWPORT, 
//                                ptr_dynamic_cast<vkViewportScissorState, ViewportScissorState>(viewportScissor)->id );
//   }
//
//   void vkCommandBuffer::bind(const Ptr<DepthStencilState> depthStencil)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_DEPTH_STENCIL, 
//                                ptr_dynamic_cast<vkDepthStencilState, DepthStencilState>(depthStencil)->id );
//   }
//
//   void vkCommandBuffer::bind(const Ptr<BlendState> blend)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_COLOR_BLEND, 
//                                ptr_dynamic_cast<vkBlendState, BlendState>(blend)->id );
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//   // VULKAN RENDER PASS
//   //------------------------------------------------------------------------------------------
//   
//
//
//
//
//   
//   XGL_ATTACHMENT_LOAD_OP colLoadOperation[8];
//   colLoadOperation[0] = VK_ATTACHMENT_LOAD_OP_CLEAR;
//   colLoadOperation[1] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   colLoadOperation[2] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   colLoadOperation[3] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   colLoadOperation[4] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   colLoadOperation[5] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   colLoadOperation[6] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   colLoadOperation[7] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   
//   XGL_ATTACHMENT_STORE_OP colStoreOperation[8];
//   colStoreOperation[0] = VK_ATTACHMENT_STORE_OP_RESOLVE_MSAA;
//   colStoreOperation[1] = VK_ATTACHMENT_STORE_OP_STORE;
//   colStoreOperation[2] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   colStoreOperation[3] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   colStoreOperation[4] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   colStoreOperation[5] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   colStoreOperation[6] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   colStoreOperation[7] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   
//   VkClearColorValue colClearValue;
//   colClearValue.floatColor[0] = 0.0f;
//   colClearValue.floatColor[0] = 0.0f;
//   colClearValue.floatColor[0] = 0.0f;
//   colClearValue.floatColor[0] = 1.0f;
//   colClearValue.rawColor[0]   = 0;
//   colClearValue.rawColor[0]   = 0;
//   colClearValue.rawColor[0]   = 0;
//   colClearValue.rawColor[0]   = 1;
//   
//   XGL_CLEAR_COLOR colClear[8];
//   for(uint32 i=0; i<8; ++i)
//      {
//      colClear[i].color       = colClearValue;
//      colClear[i].useRawValue = false;
//      }
//   
//   VkRect renderArea;
//   renderArea.offset.x      = 0;
//   renderArea.offset.y      = 0;
//   renderArea.extent.width  = 1920; 
//   renderArea.extent.height = 1080;
//   
//   XGL_RENDER_PASS_CREATE_INFO renderPassInfo;
//   renderPassInfo.sType                 = XGL_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//   renderPassInfo.pNext                 = nullptr;
//   renderPassInfo.renderArea            = renderArea;
//   renderPassInfo.framebuffer           = vkFbo->id;
//   renderPassInfo.colorAttachmentCount  = vkFbo->colorAttachments;         // This is doubled from FBO description to simplify tracing apps (don't need to know FBO info)
//   renderPassInfo.pColorLoadOps         = &colLoadOperation[0];            // Array of size equivalent to the number of attachments in the framebuffer
//   renderPassInfo.pColorStoreOps        = &colStoreOperation[0];           // Array of size equivalent to the number of attachments in the framebuffer
//   renderPassInfo.pColorLoadClearValues = &colClear[0];                    // Array of size equivalent to the number of attachments in the framebuffer
//   renderPassInfo.depthLoadOp           = XGL_ATTACHMENT_LOAD_OP_CLEAR;
//   renderPassInfo.depthLoadClearValue   = 1.0f;
//   renderPassInfo.depthStoreOp          = XGL_ATTACHMENT_STORE_OP_STORE;
//   renderPassInfo.stencilLoadOp         = XGL_ATTACHMENT_LOAD_OP_CLEAR;
//   renderPassInfo.stencilLoadClearValue = 0;
//   renderPassInfo.stencilStoreOp        = XGL_ATTACHMENT_STORE_OP_STORE;
//   
//   XGL_RENDER_PASS renderPass;
//   
//   xglCreateRenderPass( commandBuffer, &renderPassInfo, &renderPass);
//
//
//
//
//
//
//
////####################################################################################################################
//// STATE BLEND
////####################################################################################################################
//
//   // Cross API issues:
//
//   // DX12   - has LogiOp per RT
//   // Vulkan - has shared LogicOp used for all RT
//   // Metal  - has no LogicOp concept
//
//   // Proposed solution:
//   //
//   // Add feature flags:
//   //
//   // struct Support
//   //    {
//   //    bool logicOperation;                  // Supports global logic operation appliced to all Render Targets
//   //    bool logicOperationPerRenderTarget;   // Supports local logic operation specified per Render Target (can it be interleaved with blending?)
//   //    } support;
//   //
//
//   struct BlendInfo
//      {
//      bool alphaToCoverage;
//
//      BlendInfo();
//      };
//
//   struct BlendAttachmentInfo
//      {
//      bool           blending;       // Enablers
//    //bool           logicOperation;
//      BlendFunction  srcRGB;         // Blend parameters
//      BlendFunction  dstRGB;
//      BlendEquation  rgbFunc;
//      BlendFunction  srcAlpha;
//      BlendFunction  dstAlpha;
//      BlendEquation  alphaFunc;
//    //LogicOperation logic;          // Logic operation
//      ColorMask      writeMask;
//
//      BlendAttachmentInfo();
//      };
//
//   class StaticBlendState : public SafeObject
//      {
//      };
//
//
//   Ptr<StaticBlendState> Create(const BlendInfo& state,
//                                const uint32 attachments, 
//                                const BlendAttachmentInfo* color);
//
//
//
//   static const VkBlend TranslateBlend[BlendFunctionsCount] =
//      {
//      VK_BLEND_ZERO,                            // Zero
//      VK_BLEND_ONE,                             // One
//      VK_BLEND_SRC_COLOR,                       // Source
//      VK_BLEND_ONE_MINUS_SRC_COLOR,             // OneMinusSource
//      VK_BLEND_DEST_COLOR,                      // Destination
//      VK_BLEND_ONE_MINUS_DEST_COLOR,            // OneMinusDestination
//      VK_BLEND_SRC_ALPHA,                       // SourceAlpha
//      VK_BLEND_ONE_MINUS_SRC_ALPHA,             // OneMinusSourceAlpha
//      VK_BLEND_DEST_ALPHA,                      // DestinationAlpha
//      VK_BLEND_ONE_MINUS_DEST_ALPHA,            // OneMinusDestinationAlpha
//      VK_BLEND_CONSTANT_COLOR,                  // ConstantColor
//      VK_BLEND_ONE_MINUS_CONSTANT_COLOR,        // OneMinusConstantColor
//      VK_BLEND_CONSTANT_ALPHA,                  // ConstantAlpha
//      VK_BLEND_ONE_MINUS_CONSTANT_ALPHA,        // OneMinusConstantAlpha
//      VK_BLEND_SRC_ALPHA_SATURATE,              // SourceAlphaSaturate
//      VK_BLEND_SRC1_COLOR,                      // SecondSource
//      VK_BLEND_ONE_MINUS_SRC1_COLOR,            // OneMinusSecondSource
//      VK_BLEND_SRC1_ALPHA,                      // SecondSourceAlpha
//      VK_BLEND_ONE_MINUS_SRC1_ALPHA,            // OneMinusSecondSourceAlpha
//      };
//
//   static const VkBlendFunc TranslateBlendFunc[BlendEquationsCount] =
//      {
//      VK_BLEND_FUNC_ADD,                        // Add
//      VK_BLEND_FUNC_SUBTRACT,                   // Subtract
//      VK_BLEND_FUNC_REVERSE_SUBTRACT,           // DestinationMinusSource
//      VK_BLEND_FUNC_MIN,                        // Min
//      VK_BLEND_FUNC_MAX                         // Max
//      };
//
//   // Logical Operation is alternative to blending
//   static const VkLogicOp TranslateLogicOperation[LogicOperationsCount] = 
//      {
//      VK_LOGIC_OP_CLEAR,                        // Clear
//      VK_LOGIC_OP_SET,                          // Set
//      VK_LOGIC_OP_COPY,                         // Copy
//      VK_LOGIC_OP_COPY_INVERTED,                // CopyInverted
//      VK_LOGIC_OP_NOOP,                         // NoOperation
//      VK_LOGIC_OP_INVERT,                       // Invert
//      VK_LOGIC_OP_AND,                          // And
//      VK_LOGIC_OP_NAND,                         // NAnd
//      VK_LOGIC_OP_OR,                           // Or
//      VK_LOGIC_OP_NOR,                          // Nor
//      VK_LOGIC_OP_XOR,                          // Xor
//      VK_LOGIC_OP_EQUIV,                        // Equiv
//      VK_LOGIC_OP_AND_REVERSE,                  // AndReverse
//      VK_LOGIC_OP_AND_INVERTED,                 // AndInverted
//      VK_LOGIC_OP_OR_REVERSE,                   // OrReverse
//      VK_LOGIC_OP_OR_INVERTED                   // OrInverted
//      };
//
//   // Constant Color and Constant Alpha is automaticaly selected from common "Blend Factor" so there is only one enum here.
//   static const D3D12_BLEND TranslateBlend[BlendFunctionsCount] =
//      {
//      D3D12_BLEND_ZERO,                         // Zero
//      D3D12_BLEND_ONE,                          // One
//      D3D12_BLEND_SRC_COLOR,                    // Source
//      D3D12_BLEND_INV_SRC_COLOR,                // OneMinusSource
//      D3D12_BLEND_DEST_COLOR,                   // Destination
//      D3D12_BLEND_INV_DEST_COLOR,               // OneMinusDestination
//      D3D12_BLEND_SRC_ALPHA,                    // SourceAlpha
//      D3D12_BLEND_INV_SRC_ALPHA,                // OneMinusSourceAlpha
//      D3D12_BLEND_DEST_ALPHA,                   // DestinationAlpha
//      D3D12_BLEND_INV_DEST_ALPHA,               // OneMinusDestinationAlpha
//      D3D12_BLEND_BLEND_FACTOR,                 // ConstantColor
//      D3D12_BLEND_INV_BLEND_FACTOR,             // OneMinusConstantColor
//      D3D12_BLEND_BLEND_FACTOR,                 // ConstantAlpha
//      D3D12_BLEND_INV_BLEND_FACTOR,             // OneMinusConstantAlpha
//      D3D12_BLEND_SRC_ALPHA_SAT,                // SourceAlphaSaturate
//      D3D12_BLEND_SRC1_COLOR,                   // SecondSource
//      D3D12_BLEND_INV_SRC1_COLOR,               // OneMinusSecondSource
//      D3D12_BLEND_SRC1_ALPHA,                   // SecondSourceAlpha
//      D3D12_BLEND_INV_SRC1_ALPHA,               // OneMinusSecondSourceAlpha
//      };
//
//   static const D3D12_BLEND_OP TranslateBlendFunc[BlendEquationsCount] =
//      {
//      D3D12_BLEND_OP_ADD,                       // Add
//      D3D12_BLEND_OP_SUBTRACT,                  // Subtract
//      D3D12_BLEND_OP_REV_SUBTRACT,              // DestinationMinusSource
//      D3D12_BLEND_OP_MIN,                       // Min
//      D3D12_BLEND_OP_MAX                        // Max
//      };
//
//   // Logical Operation is alternative to blending
//   static const D3D12_LOGIC_OP TranslateLogicOperation[LogicOperationsCount] = 
//      {
//      D3D12_LOGIC_OP_CLEAR,                     // Clear
//      D3D12_LOGIC_OP_SET,                       // Set
//      D3D12_LOGIC_OP_COPY,                      // Copy
//      D3D12_LOGIC_OP_COPY_INVERTED,             // CopyInverted
//      D3D12_LOGIC_OP_NOOP,                      // NoOperation
//      D3D12_LOGIC_OP_INVERT,                    // Invert
//      D3D12_LOGIC_OP_AND,                       // And
//      D3D12_LOGIC_OP_NAND,                      // NAnd
//      D3D12_LOGIC_OP_OR,                        // Or
//      D3D12_LOGIC_OP_NOR,                       // Nor
//      D3D12_LOGIC_OP_XOR,                       // Xor
//      D3D12_LOGIC_OP_EQUIV,                     // Equiv
//      D3D12_LOGIC_OP_AND_REVERSE,               // AndReverse
//      D3D12_LOGIC_OP_AND_INVERTED,              // AndInverted
//      D3D12_LOGIC_OP_OR_REVERSE,                // OrReverse
//      D3D12_LOGIC_OP_OR_INVERTED                // OrInverted
//      };
//
//   // Metal doesn't support double source blending
//   static const MTLBlendFactor TranslateBlend[BlendFunctionsCount] =
//      {
//      MTLBlendFactorZero,                       // Zero
//      MTLBlendFactorOne                         // One
//      MTLBlendFactorSourceColor,                // Source
//      MTLBlendFactorOneMinusSourceColor,        // OneMinusSource
//      MTLBlendFactorDestinationColor,           // Destination
//      MTLBlendFactorOneMinusDestinationColor,   // OneMinusDestination
//      MTLBlendFactorSourceAlpha,                // SourceAlpha
//      MTLBlendFactorOneMinusSourceAlpha,        // OneMinusSourceAlpha
//      MTLBlendFactorDestinationAlpha,           // DestinationAlpha
//      MTLBlendFactorOneMinusDestinationAlpha,   // OneMinusDestinationAlpha
//      MTLBlendFactorBlendColor,                 // ConstantColor
//      MTLBlendFactorOneMinusBlendColor,         // OneMinusConstantColor
//      MTLBlendFactorBlendAlpha,                 // ConstantAlpha
//      MTLBlendFactorOneMinusBlendColor,         // OneMinusConstantAlpha
//      MTLBlendFactorOneMinusBlendAlpha,         // SourceAlphaSaturate
//      MTLBlendFactorZero,                       // SecondSource
//      MTLBlendFactorZero,                       // OneMinusSecondSource
//      MTLBlendFactorZero,                       // SecondSourceAlpha
//      MTLBlendFactorZero                        // OneMinusSecondSourceAlpha
//      };
//
//   static const MTLBlendOperation TranslateBlendFunc[BlendEquationsCount] =
//      {
//      MTLBlendOperationAdd,                     // Add
//      MTLBlendOperationSubtract,                // Subtract
//      MTLBlendOperationReverseSubtract,         // DestinationMinusSource
//      MTLBlendOperationMin,                     // Min
//      MTLBlendOperationMax                      // Max
//      };
//
//   // Metal is not supporting logical operation at all
//
//
//   BlendInfo::BlendInfo() :
//      alphaToCoverage(false)
//   {
//   };
//
//   // TODO: Compare with OpenGL, Vulkan, Metal default states
//   BlendAttachmentInfo::BlendAttachmentInfo() :
//      blending(false),
//    //logicOperation(false),
//      srcRGB(One),
//      dstRGB(Zero),
//      rgbFunc(Add),
//      srcAlpha(One),
//      dstAlpha(Zero),
//      alphaFunc(Add),
//    //logic(NoOperation),
//      writeMask(ColorMaskAll)
//   {
//   };
//
//   class dxStaticBlendState : public StaticBlendState
//      {
//      D3D12_BLEND_DESC desc;
//      };
//
//   class mtlStaticBlendState : public StaticBlendState
//      {
//      bool alphaToCoverage;
//      MTLRenderPipelineColorAttachmentDescriptor blendInfo[MaxColorAttachmentsCount];
//      };
//
//   class vkStaticBlendState : public StaticBlendState
//      {
//      VkPipelineCbAttachmentState blendInfo[MaxColorAttachmentsCount];
//      VkPipelineCbStateCreateInfo desc;
//      };
//
//   Ptr<StaticBlendState> Create(const BlendInfo& state,
//                                const uint32 attachments, 
//                                const BlendAttachmentInfo* color)
//   {
//   Ptr<dxStaticBlendState> dxState = new dxStaticBlendState();
//   dxState->desc.AlphaToCoverageEnable  = state.alphaToCoverage;
//   dxState->desc.IndependentBlendEnable = true;
//
//   for(uint32 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
//      {
//      //assert( !(color[i].logicOperation && color[i].blending) );
//      dxState->desc.RenderTarget[i].BlendEnable           = color[i].blending;
//      dxState->desc.RenderTarget[i].LogicOpEnable         = false;                                  // color[i].logicOperation;
//      dxState->desc.RenderTarget[i].SrcBlend              = TranslateBlend[color[i].srcRGB];
//      dxState->desc.RenderTarget[i].DestBlend             = TranslateBlend[color[i].dstRGB];
//      dxState->desc.RenderTarget[i].BlendOp               = TranslateBlendFunc[color[i].rgbFunc];
//      dxState->desc.RenderTarget[i].SrcBlendAlpha         = TranslateBlend[color[i].srcAlpha];
//      dxState->desc.RenderTarget[i].DestBlendAlpha        = TranslateBlend[color[i].dstAlpha];
//      dxState->desc.RenderTarget[i].BlendOpAlpha          = TranslateBlendFunc[color[i].alphaFunc];
//      dxState->desc.RenderTarget[i].LogicOp               = TranslateLogicOperation[NoOperation];   // TranslateLogicOperation[color[i].logic];
//      // Translate Color Write Mask
//      dxState->desc.RenderTarget[i].RenderTargetWriteMask = 0;
//      if (color[i].writeMask & ColorMaskRed)
//         dxState->desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
//      if (color[i].writeMask & ColorMaskGreen)
//         dxState->desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
//      if (color[i].writeMask & ColorMaskBlue)
//         dxState->desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
//      if (color[i].writeMask & ColorMaskAlpha)
//         dxState->desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
//      }
//
//   return ptr_dynamic_cast<StaticBlendState, dxStaticBlendState>(dxState);
//   }
//
//   #if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//   Ptr<StaticBlendState> Create(const BlendInfo& state,
//                                const uint32 attachments, 
//                                const BlendAttachmentInfo* color)
//   {
//   Ptr<mtlStaticBlendState> mtlState = new mtlStaticBlendState();
//
//   mtlState->alphaToCoverage = state.alphaToCoverage;
//
//   for(uint8 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
//      {
//      //assert( !(color[0].logicOperation && color[i].blending) );
//
//      [mtlState->blendInfo[i] setBlendingEnabled:             color[i].blending                      ];
//    //[mtlState->blendInfo[i] pixelFormat:                    /* color[i].format ???  */ ];  // Metal TODO: Pixel Format need to match the pixel format of color attachment !!
//      [mtlState->blendInfo[i] setSourceRGBBlendFactor:        TranslateBlend[color[i].srcRGB]        ];
//      [mtlState->blendInfo[i] setDestinationRGBBlendFactor:   TranslateBlend[color[i].dstRGB]        ];
//      [mtlState->blendInfo[i] setSourceAlphaBlendFactor:      TranslateBlendFunc[color[i].srcAlpha]  ];
//      [mtlState->blendInfo[i] setDestinationAlphaBlendFactor: TranslateBlend[color[i].dstAlpha]      ];
//      [mtlState->blendInfo[i] setRgbBlendOperation:           TranslateBlend[color[i].rgbFunc]       ];
//      [mtlState->blendInfo[i] setAlphaBlendOperation:         TranslateBlendFunc[color[i].alphaFunc] ];
//
//      // TODO: What about color mask ????
//      }
//
//   // TODO Finish Metal 
//   return ptr_dynamic_cast<StaticBlendState, mtlStaticBlendState>(mtlState);
//   }
//   #endif
//
//   Ptr<StaticBlendState> Create(const BlendInfo& state,
//                                const uint32 attachments, 
//                                const BlendAttachmentInfo* color)
//   {
//   Ptr<vkStaticBlendState> vkState = new vkStaticBlendState();
//   vkState->desc.sType                 = VK_STRUCTURE_TYPE_PIPELINE_CB_STATE_CREATE_INFO;
//   vkState->desc.pNext                 = nullptr;
//   vkState->desc.alphaToCoverageEnable = state.alphaToCoverage;
//   vkState->desc.logicOpEnable         = false;                                // color[0].logicOperation;
//   vkState->desc.logicOp               = TranslateLogicOperation[NoOperation]; // TranslateLogicOperation[color[0].logic];
//   vkState->desc.attachmentCount       = min(attachments, MaxColorAttachmentsCount);
//   vkState->desc.pAttachments          = nullptr; // &vkState->blendInfo[0]; This descriptor is shared and reused so we need to copy it and then init.
//
//   for(uint32 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
//      {
//      //assert( !(color[0].logicOperation && color[i].blending) );
//      vkState->blendInfo[i].blendEnable      = color[i].blending;
//      //VkFormat    vkState->blendInfo[i].format           = ;            // Vulkan TODO: Why we set blending format ????
//      vkState->blendInfo[i].srcBlendColor    = TranslateBlend[color[i].srcRGB];
//      vkState->blendInfo[i].destBlendColor   = TranslateBlend[color[i].dstRGB];
//      vkState->blendInfo[i].blendFuncColor   = TranslateBlendFunc[color[i].rgbFunc];
//      vkState->blendInfo[i].srcBlendAlpha    = TranslateBlend[color[i].srcAlpha];
//      vkState->blendInfo[i].destBlendAlpha   = TranslateBlend[color[i].dstAlpha];
//      vkState->blendInfo[i].blendFuncAlpha   = TranslateBlendFunc[color[i].alphaFunc];
//      // Translate Color Write Mask
//      vkState->blendInfo[i].channelWriteMask = 0;
//      if (color[i].writeMask & ColorMaskRed)           // https://cvs.khronos.org/bugzilla/show_bug.cgi?id=13632
//         vkState->blendInfo[i].channelWriteMask |= VK_CHANNEL_R_BIT;
//      if (color[i].writeMask & ColorMaskGreen)
//         vkState->blendInfo[i].channelWriteMask |= VK_CHANNEL_G_BIT;
//      if (color[i].writeMask & ColorMaskBlue)
//         vkState->blendInfo[i].channelWriteMask |= VK_CHANNEL_B_BIT;
//      if (color[i].writeMask & ColorMaskAlpha)
//         vkState->blendInfo[i].channelWriteMask |= VK_CHANNEL_A_BIT;
//      }
//
//   return ptr_dynamic_cast<StaticBlendState, vkStaticBlendState>(vkState);
//   }
//
//
////####################################################################################################################
//// STATE DEPTH-STENCIL
////####################################################################################################################
//
//   // These are per face:
//   //
//   //   D    S      D3D12                   Metal                   Vulkan          OpenGL
//   //        -   StencilFailOp       stencilFailureOperation    stencilFailOp       N/A
//   //   -    +   StencilDepthFailOp  depthFailureOperation      stencilDepthFailOp  N/A
//   //   +    +   StencilPassOp       depthStencilPassOperation  stencilPassOp       N/A
//
//
//   // S RM F     static                  static ?                  dynamic        dynamic
//   // S RM B       --                    static ?                    --           dynamic <- makes sense, we can divide stencil buffer bits per facing
//   // S WM F     static                  static ?                  dynamic        dynamic
//   // S WM B       --                    static ?                    --           dynamic <- https://www.opengl.org/registry/specs/EXT/stencil_two_side.txt
//   // S Ref F    dynamic                 dynamic ?                 dynamic        dynamic    (there is possibility of use case for two sided R/W bitmasks)
//   // S Ref B      --                       --                     dynamic        dynamic <- Two sided reference value needs two slots from state, one slot if returned by FS
//   // D boun min                                                   dynamic 
//   // D boun max                                                   dynamic
//   // D Wr  En   static                  static ?                   static        dynamic
//
//   // Defaults:
//   //
//   // stencilTest - false
//   //
//   // S Ref F     - 0
//   // S Ref B     - 0
//   // S Comp F    - Always
//   // S Comp B    - Always
//   // S RM xxx    - 0xFFFFFFFF
//   // S Op xxx    - Keep
//   //
//   // depthTest   - false
//   // D Func      - Less
//   //
//   // depthWrite  - true
//   //
//   // depthClamp  - false
//   //
//   // depthBounds - false
//
//
//
//   struct DynamicDepthSencilInfo
//      {  
//      float2 depthRange;
//
//      struct Stencil
//             {
//             uint32 reference; // Reference value
//             uint32 readMask;  // Mask used with reference value and input stencil buffer value, also called "compare mask"
//             uint32 writeMask; // Mask used with output stencil value
//             } stencil[2];
//
//      DynamicDepthStencilInfo();
//      };
//
//   struct StaticDepthStencilInfo
//      {
//      bool depthTest;
//      bool depthWrite;
//      bool depthBounds;                        // Extension in OpenGL, not present in Core Profile nor Direct3D, Google patent ? US 20040169651 A1 ?
//      bool stencilTest; 
//      CompareMethod depthFunction;
//
//      struct StencilOperation
//         {
//         CompareMethod       function;         // Compare method to use for each test
//         StencilModification whenStencilFails; // What to do if stencil test fails
//         StencilModification whenDepthFails;   // What to do if stencil test passes but depth test fails
//         StencilModification whenBothPass;     // What to do if both tests passes
//         } stencilOperation[2];
//
//      StaticDepthStencilInfo();
//      };
//
//   class DepthStencilState : public SafeObject
//      {
//      };
//
//   class StaticDepthStencilState : public SafeObject
//      {
//      };
//
//   Ptr<DepthStencilState>       Create(const DynamicDepthStencilInfo& state);
//   Ptr<StaticDepthStencilState> Create(const StaticDepthStencilInfo& state);
//   
//
//
//
//
//
//
//
//   class dxDepthStencilState : public DepthStencilState
//      {
//      public:
//      // TODO
//      };
//
//   #if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//   class mtlDepthStencilState : public DepthStencilState
//      {
//      public:
//      DynamicDepthStencilInfo desc;
//      };
//   #endif
//
//   class vkDepthStencilState : public DepthStencilState
//      {
//      public:
//      VkDynamicDsState id;
//      };
//
//
//
//   class dxStaticDepthStencilState : public StaticDepthStencilState
//      {
//      public:
//      D3D12_DEPTH_STENCIL_DESC desc;
//      };
//
//   #if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//   class mtlStaticDepthStencilState : public StaticDepthStencilState
//      {
//      public:
//      id<MTLDepthStencilState> desc;
//      };
//   #endif
//
//   class vkStaticDepthStencilState : public StaticDepthStencilState
//      {
//      public:
//      VkPipelineDsStateCreateInfo desc;
//      };
//
//   DynamicDepthStencilInfo::DynamicDepthStencilInfo() :
//      depthRange(-1.0f, 1.0f)
//   {
//   for(uint8 i=0; i<2; ++i)
//      {
//      stencil[i].reference = 0;
//      stencil[i].readMask  = 0XFFFFFFFF;
//      stencil[i].writeMask = 0xFFFFFFFF;
//      }
//   }
//
//   StaticDepthStencilInfo::StaticDepthStencilInfo() :
//      depthTest(false),
//      depthWrite(true),
//      depthBounds(false), 
//      stencilTest(false),  
//      depthFunction(Less)
//   {
//   for(uint8 i=0; i<2; ++i)
//      {
//      stencilOperation[i].function         = Always;
//      stencilOperation[i].whenStencilFails = Keep;
//      stencilOperation[i].whenDepthFails   = Keep;
//      stencilOperation[i].whenBothPass     = Keep;
//      }
//   }
//
//
//
//
//
//
//   Ptr<DepthStencilState> Create(const DynamicDepthStencilInfo& state)
//   {
//   Ptr<DepthStencilState> result = nullptr;
//
//
//   // Reference value - single, set directly in command list
//   // Read mask       - single, static
//   // write mask      - single, static
//
//
//// ComPtr<ID3D12GraphicsCommandList> pCommandList;
//// uint32 ref = 0;
//// pCommandList->OMSetStencilRef(ref);
//
//   return result;
//   }
//
//
//
//
// //  [MTLRenderCommandEncoder setStencilReferenceValue: state.stencil[0].reference ];   // Front reference value <- dynamic set at runtime 
//
// //  [renderCommandEncoder setDepthStencilState:depthStencilState];  // Set Static State ?? or is it dynamic if can be set to encoder ??
//
//
//   //  Proposition of Metal API extension:
//   //
//   //  [MTLRenderCommandEncoder setStencilReferenceValue:index: ];   
//   //  [MTLRenderCommandEncoder setStencilReadMask:index: ];      
//   //  [MTLRenderCommandEncoder setStencilWriteMask:index: ];
//   //
//   //  They allow setting separate Reference Value, Read Mask and Write Mask 
//   //  for Front and Back stencil operations. At the same time readMask and
//   //  writeMask fields should be removed from MTLStencilDescriptor (or left
//   //  but their values will be overwritten by above dynamic calls).
//   //
//   #if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//   Ptr<DepthStencilState> Create(const DynamicDepthStencilInfo& state)
//   {
//   Ptr<mtlDepthStencilState> mtlState = new mtlDepthStencilState();
//   mtlState->desc = state;
//   return ptr_dynamic_cast<DepthStencilState, mtlDepthStencilState>(mtlState);
//   }
//   #endif
//
//   Ptr<DepthStencilState> Create(const DynamicDepthStencilInfo& state)
//   {
//   Ptr<DepthStencilState> result = nullptr;
//
//   VkDynamicDsStateCreateInfo depthStencilInfo;
//   depthStencilInfo.sType            = VK_STRUCTURE_TYPE_DYNAMIC_DS_STATE_CREATE_INFO;
//   depthStencilInfo.pNext            = nullptr;
//   depthStencilInfo.minDepth         = state.depthRange.x;         // optional (depth_bounds_test)
//   depthStencilInfo.maxDepth         = state.depthRange.y;         // optional (depth_bounds_test)
//   depthStencilInfo.stencilReadMask  = state.stencil[0].readMask;  // Mask that is ANDed with both the reference value and the stored stencil value before the test.
//   depthStencilInfo.stencilWriteMask = state.stencil[0].writeMask;    
//   depthStencilInfo.stencilFrontRef  = state.stencil[0].reference;
//   depthStencilInfo.stencilBackRef   = state.stencil[1].reference;
//
//   VkDynamicDsState depthStencilState;
//   VkResult res = vkCreateDynamicDepthStencilState( gpu[i].handle, &depthStencilInfo, &depthStencilState );
//   if (!res)
//      {
//      Ptr<vkDepthStencilState> vkState = new vkDepthStencilState();
//      vkState->id = depthStencilState;
//      result = ptr_dynamic_cast<DepthStencilState, vkDepthStencilState>(vkState);
//      }
//
//   return result;
//   }
//
//
//
//
//   Ptr<StaticDepthStencilState> Create(const StaticDepthStencilState& state)
//   {
//   Ptr<dxStaticDepthStencilState> dxState = new dxStaticDepthStencilState();
//
//   D3D12_DEPTH_STENCILOP_DESC stencilInfo[2];
//   for(uint8 i=0; i<2; ++i)
//      {
//      stencilInfo[i].StencilFunc        = TranslateCompareFunction[state.stencilOperation[i].function];
//      stencilInfo[i].StencilFailOp      = TranslateStencilOperation[state.stencilOperation[i].whenStencilFails];
//      stencilInfo[i].StencilDepthFailOp = TranslateStencilOperation[state.stencilOperation[i].whenDepthFails]; 
//      stencilInfo[i].StencilPassOp      = TranslateStencilOperation[state.stencilOperation[i].whenBothPass];
//      }
//
//   dxState->desc.DepthEnable      = state.depthTest; 
//   dxState->desc.DepthWriteMask   = state.depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
//   dxState->desc.DepthFunc        = TranslateCompareFunction[state.depthFunction];
//   dxState->desc.StencilEnable    = state.stencilTest;
//
//// TODO: Dynamic State is Static for Dx12
//// dxState->desc.StencilReadMask  = state.readMask;
//// dxState->desc.StencilWriteMask = state.writeMask;
//
//   dxState->desc.FrontFace        = stencilInfo[0];
//   dxState->desc.BackFace         = stencilInfo[1];
//
//   return ptr_dynamic_cast<StaticDepthState, vkStaticDepthState>(vkState);
//   };
//
//
//   Ptr<StaticDepthStencilState> Create(const StaticDepthStencilInfo& state)
//   {
//   Ptr<vkStaticDepthStencilState> vkState = new vkStaticDepthStencilState();
//
//   VkStencilOpState stencilInfo[2];
//   for(uint8 i=0; i<2; ++i)
//      {
//      stencilInfo[i].stencilCompareOp   = TranslateCompareFunction[state.stencilOperation[i].function];
//      stencilInfo[i].stencilFailOp      = TranslateStencilOperation[state.stencilOperation[i].whenStencilFails];
//      stencilInfo[i].stencilDepthFailOp = TranslateStencilOperation[state.stencilOperation[i].whenDepthFails]; 
//      stencilInfo[i].stencilPassOp      = TranslateStencilOperation[state.stencilOperation[i].whenBothPass];
//      }
//
//   vkState->desc.sType             = VK_STRUCTURE_TYPE_PIPELINE_DS_STATE_CREATE_INFO;
//   vkState->desc.pNext             = nullptr;
// //VkFormat vkState->desc.format   =;      // Vulkan TODO: Why we set blending format ????
//   vkState->desc.depthTestEnable   = state.depthTest; 
//   vkState->desc.depthWriteEnable  = state.depthWrite;
//   vkState->desc.depthCompareOp    = TranslateCompareFunction[state.depthFunction];
//   vkState->desc.depthBoundsEnable = state.depthBounds;
//   vkState->desc.stencilTestEnable = state.stencilTest;
//   vkState->desc.front             = stencilInfo[0];
//   vkState->desc.back              = stencilInfo[1];
//
//   return ptr_dynamic_cast<StaticDepthStencilState, vkStaticDepthStencilState>(vkState);
//   };
//
//
//   #if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//   Ptr<StaticDepthStencilState> Create(const StaticDepthStencilInfo& state)
//   {
//   Ptr<mtlStaticDepthStencilState> mtlState = new mtlStaticDepthStencilState();
//
//   MTLStencilDescriptor* stencilInfo[2];
//   for(uint8 i=0; i<2; ++i)
//      {
//      stencilInfo[i] = [[MTLStencilDescriptor alloc] init];
//      stencilInfo[i].stencilCompareFunction    = TranslateCompareFunction[state.stencilOperation[i].function];
//      stencilInfo[i].stencilFailureOperation   = TranslateStencilOperation[state.stencilOperation[i].whenStencilFails];
//      stencilInfo[i].depthFailureOperation     = TranslateStencilOperation[state.stencilOperation[i].whenDepthFails]; 
//      stencilInfo[i].depthStencilPassOperation = TranslateStencilOperation[state.stencilOperation[i].whenBothPass];
//
//   // TODO: Dynamic State static for Metal:
//   // stencilInfo[i].readMask                  = 0xFFFFFFFF;
//   // stencilInfo[i].writeMask                 = 0xFFFFFFFF;
//      }
//
//   MTLDepthStencilDescriptor* desc = [[MTLDepthStencilDescriptor alloc] init];
//   desc.depthWriteEnabled    = state.depthWrite;
//   desc.depthCompareFunction = TranslateCompareFunction[state.depthFunction];
//   desc.frontFaceStencil     = stencilInfo[0];
//   desc.backFaceStencil      = stencilInfo[1];
//
//   mtlState->desc = [gDevice newDepthStencilStateWithDescriptor:desc];
//   [stencilInfo[0] release];
//   [stencilInfo[1] release];
//   [desc release];
//
//   return ptr_dynamic_cast<StaticDepthStencilState, mtlStaticDepthStencilState>(mtlState);
//   }
//   #endif
//
//
////####################################################################################################################
//// STATE PIPELINE
////####################################################################################################################
//
//
//   class Pipeline : public SafeObject
//      {
//      };
//
//   Ptr<Pipeline> Create(const Ptr<StaticBlendState> blendState);
//
//
//
//   class dxPipeline : public Pipeline
//      {
//      ID3D12PipelineState* id;
//      };
//
//   class mtlPipeline : public Pipeline
//      {
//      id<MTLRenderPipelineState> id;
//      };
//
//   class vkPipeline : public Pipeline
//      {
//      VkPipeline id;
//      };
//
//
//   Ptr<Pipeline> Create(const Ptr<StaticBlendState> blendState)
//   {
//   Ptr<Pipeline> result = nullptr;
//
//   ID3D12Device* device;
//
//   // Set Static Blend State
//   Ptr<dxStaticBlendState> blend = ptr_dynamic_cast<dxStaticBlendState, StaticBlendState>(blendState);
//
//   D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
//
//   // TODO: Fill descriptor
//
//   ID3D12RootSignature           *pRootSignature;
//   D3D12_SHADER_BYTECODE         VS;
//   D3D12_SHADER_BYTECODE         PS;
//   D3D12_SHADER_BYTECODE         DS;
//   D3D12_SHADER_BYTECODE         HS;
//   D3D12_SHADER_BYTECODE         GS;
//   D3D12_STREAM_OUTPUT_DESC      StreamOutput;
//   desc.BlendState             = blend->desc;
//   UINT                               SampleMask;
//   D3D12_RASTERIZER_DESC              RasterizerState;
//   D3D12_DEPTH_STENCIL_DESC           DepthStencilState;
//   D3D12_INPUT_LAYOUT_DESC            InputLayout;
//   desc.IBStripCutValue        = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF; // or D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF or D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED
//   desc.PrimitiveTopologyType  = TranslateDrawableType[ /* input drawable type */ ];
//   UINT                               NumRenderTargets;
//   DXGI_FORMAT                        RTVFormats[8];
//   DXGI_FORMAT                        DSVFormat;
//   DXGI_SAMPLE_DESC                   SampleDesc;
//   desc.NodeMask               = 0;  // Only use for multi-GPU
//   D3D12_CACHED_PIPELINE_STATE        CachedPSO;
//   desc.Flags                  = D3D12_PIPELINE_STATE_FLAG_NONE; // or D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG
//
//   // Create pipeline state object
//   ID3D12PipelineState* pipeline;
//   HRESULT ret = device->CreateGraphicsPipelineState( &desc, __uuidof(ID3D12PipelineState), (void**)&pipeline);
//   if (ret == E_OK)
//      {
//      Ptr<dxPipeline> pso = new dxPipeline();
//      pso->id = pipeline;
//      result = ptr_dynamic_cast<Pipeline, dxPipeline>(pipeline);
//      }
////   else
////      {
////      E_OUTOFMEMORY
////      }
//
//   return result;
//   }
//
//
//
//
//
//
//// IA - Input Assembler
////---------------------------------------------------------------------
//
//   // Attributes representing data in fixed point shemes like 16.16, 8.0, 16.0 
//   // were introduced in OpenGL ES for low end devices without HW acceleration. 
//   // Currently all mobile devices has HW acceleration for OpenGL ES which means 
//   // that floating point values will be better choose in almost all cases. 
//   // Therefore fixed attribute formats are not supported by engine.
//
//   // Type of attribute data
//   enum AttributeType
//        {
//        None                      = 0,   
//        Half                         ,
//        Half2                        ,
//        Half3                        ,
//        Half4                        ,
//        Float                        ,
//        Float2                       ,
//        Float3                       ,
//        Float4                       ,
//        Double                       ,
//        Double2                      ,
//        Double3                      ,
//        Double4                      ,
//        Int8                         ,
//        Int8v2                       ,
//        Int8v3                       ,
//        Int8v4                       ,
//        Int16                        ,
//        Int16v2                      ,
//        Int16v3                      ,
//        Int16v4                      ,
//        Int32                        ,
//        Int32v2                      ,
//        Int32v3                      ,
//        Int32v4                      ,
//        Int64                        ,
//        Int64v2                      ,
//        Int64v3                      ,
//        Int64v4                      ,
//        UInt8                        ,
//        UInt8v2                      ,
//        UInt8v3                      ,
//        UInt8v4                      ,
//        UInt16                       ,
//        UInt16v2                     ,
//        UInt16v3                     ,
//        UInt16v4                     ,
//        UInt32                       ,
//        UInt32v2                     ,
//        UInt32v3                     ,
//        UInt32v4                     ,
//        UInt64                       ,
//        UInt64v2                     ,
//        UInt64v3                     ,
//        UInt64v4                     ,
//        Float8_SNorm                 ,
//        Float8v2_SNorm               ,
//        Float8v3_SNorm               ,
//        Float8v4_SNorm               ,
//        Float16_SNorm                ,
//        Float16v2_SNorm              ,
//        Float16v3_SNorm              ,
//        Float16v4_SNorm              ,
//        Float8_Norm                  ,
//        Float8v2_Norm                ,
//        Float8v3_Norm                ,
//        Float8v4_Norm                ,
//        Float16_Norm                 ,
//        Float16v2_Norm               ,
//        Float16v3_Norm               ,
//        Float16v4_Norm               ,
//        Float4_10_10_10_2_SNorm      ,
//        Float4_10_10_10_2_Norm       ,
//        AttributeTypesCount
//        };
//
//   // This value should be queried from GPU settings
//   #define attributeMemoryAlignment  4
//
//   struct Attribute
//      {
//      AttributeType type;        // Data type
//      uint32        buffer;      // Index of buffer to use from buffers array (this is optional, used when passed to IA)
//
//      Attribute();
//      Attribute(const AttributeType type);
//      Attribute(const AttributeType type, const uint32 buffer);
//      };
//
//   struct BufferSettings
//      {
//      AttributeType attribute[MaxInputLayoutAttributesCount]; // Describes each element in the buffer
//      BufferType type;        // Buffer type
//      union {                 // Vertex buffer can store geometry vertices
//            uint32 vertices;  // or like Index buffer "elements" of some 
//            uint32 elements;  // other type, while other buffers just need
//            uint32 size;      // their size specified.
//            };
//      uint32 step;            // Update rate, by default set to 0 - per vertex update, 
//                              // otherwise describes after how many instances it should 
//                              // proceed to next data
//      };
//
//
//
//   Attribute::Attribute() :
//      type(None),
//      buffer(0)
//   {
//   }
//
//   Attribute::Attribute(const AttributeType type) :
//      type(type),
//      buffer(0)
//   {
//   }
//
//   Attribute::Attribute(const AttributeType type, const uint32 buffer) :
//      type(type),
//      buffer(buffer)
//   {
//   assert( buffer < MaxInputLayoutAttributesCount );
//   }
//
//
//// IA - Input Assembler   -   D3D 12
////---------------------------------------------------------------------
//
//
//
//
//   // D3D12:
//
//   // 0-15 (16) input slots for IA
//   D3D12_INPUT_ELEMENT_DESC
//LPCSTR                     SemanticName;             // "name" corresponding to name in the HLSL shader
//  UINT                       SemanticIndex;          // part of element, for e.g. matrix4x4 will have 0 to 3  (will be derived from ColumnType)
//  DXGI_FORMAT                Format;                 // data type format
//  UINT                       InputSlot;              // 0-15
//  UINT                       AlignedByteOffset;
//  D3D12_INPUT_CLASSIFICATION InputSlotClass;        // stepp type
//  UINT                       InstanceDataStepRate;  // 0 -> for D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
//
//
//D3D12_INPUT_CLASSIFICATION 
//  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA    // default, stepRate = 0
//  D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA  // 1+
//
//
//
//
//// single VBO description (can support multiple data elements in IA)
//id;        // VBO index to use
//offset;    // data starting offset
//stride;    // offset to next vertex data
//frequency; // how often update this data
//
//
//
//
//
//
//
//
//// IA - Input Assembler   -   Metal
////---------------------------------------------------------------------
//
//
//   // Metal will add double types in the future to support compute kernels
//   // porting from OpenCL to Metal API. sint64 and uint64 can be added but
//   // it is not verified yet. Other missing types could be added as well but
//   // Epic and Crytek didn't requested them when API was designed so they
//   // were ommited!
//
//   // Type of data in attributes
//   static const MTLVertexFormat TranslateAttributeType[AttributeTypesCount] = 
//      {
//      MTLVertexFormatInvalid,              // None                      
//      MTLVertexFormatInvalid,  // Half                   
//      MTLVertexFormatHalf2,                // Half2                  
//      MTLVertexFormatHalf3,                // Half3                  
//      MTLVertexFormatHalf4,                // Half4                  
//      MTLVertexFormatFloat,                // Float                  
//      MTLVertexFormatFloat2,               // Float2                 
//      MTLVertexFormatFloat3,               // Float3                 
//      MTLVertexFormatFloat4,               // Float4                 
//      MTLVertexFormatInvalid,  // Double                 
//      MTLVertexFormatInvalid,  // Double2                
//      MTLVertexFormatInvalid,  // Double3                
//      MTLVertexFormatInvalid,  // Double4                
//      MTLVertexFormatInvalid,  // Int8                   
//      MTLVertexFormatChar2,                // Int8v2                 
//      MTLVertexFormatChar3,                // Int8v3                 
//      MTLVertexFormatChar4,                // Int8v4                 
//      MTLVertexFormatInvalid,  // Int16                  
//      MTLVertexFormatShort2,               // Int16v2                
//      MTLVertexFormatShort3,               // Int16v3                
//      MTLVertexFormatShort4,               // Int16v4                
//      MTLVertexFormatInt,                  // Int32                  
//      MTLVertexFormatInt2,                 // Int32v2                
//      MTLVertexFormatInt3,                 // Int32v3                
//      MTLVertexFormatInt4,                 // Int32v4                
//      MTLVertexFormatInvalid,  // Int64                  
//      MTLVertexFormatInvalid,  // Int64v2                
//      MTLVertexFormatInvalid,  // Int64v3                
//      MTLVertexFormatInvalid,  // Int64v4                
//      MTLVertexFormatInvalid,  // UInt8                  
//      MTLVertexFormatUChar2,               // UInt8v2                
//      MTLVertexFormatUChar3,               // UInt8v3                
//      MTLVertexFormatUChar4,               // UInt8v4                
//      MTLVertexFormatInvalid,  // UInt16                 
//      MTLVertexFormatUShort2,              // UInt16v2               
//      MTLVertexFormatUShort3,              // UInt16v3               
//      MTLVertexFormatUShort4,              // UInt16v4               
//      MTLVertexFormatUInt,                 // UInt32                 
//      MTLVertexFormatUInt2,                // UInt32v2               
//      MTLVertexFormatUInt3,                // UInt32v3               
//      MTLVertexFormatUInt4,                // UInt32v4               
//      MTLVertexFormatInvalid,  // UInt64                 
//      MTLVertexFormatInvalid,  // UInt64v2               
//      MTLVertexFormatInvalid,  // UInt64v3               
//      MTLVertexFormatInvalid,  // UInt64v4               
//      MTLVertexFormatInvalid,  // Float8_SNorm           
//      MTLVertexFormatChar2Normalized,      // Float8v2_SNorm         
//      MTLVertexFormatChar3Normalized,      // Float8v3_SNorm         
//      MTLVertexFormatChar4Normalized,      // Float8v4_SNorm         
//      MTLVertexFormatInvalid,  // Float16_SNorm          
//      MTLVertexFormatShort2Normalized,     // Float16v2_SNorm        
//      MTLVertexFormatShort3Normalized,     // Float16v3_SNorm        
//      MTLVertexFormatShort4Normalized,     // Float16v4_SNorm        
//      MTLVertexFormatInvalid,  // Float8_Norm            
//      MTLVertexFormatUChar2Normalized,     // Float8v2_Norm          
//      MTLVertexFormatUChar3Normalized,     // Float8v3_Norm          
//      MTLVertexFormatUChar4Normalized,     // Float8v4_Norm          
//      MTLVertexFormatInvalid,  // Float16_Norm           
//      MTLVertexFormatUShort2Normalized,    // Float16v2_Norm         
//      MTLVertexFormatUShort3Normalized,    // Float16v3_Norm         
//      MTLVertexFormatUShort4Normalized,    // Float16v4_Norm         
//      MTLVertexFormatInt1010102Normalized, // Float4_10_10_10_2_SNorm
//      MTLVertexFormatUInt1010102Normalized // Float4_10_10_10_2_Norm 
//      };
//
//   // Size of each attribute in memory taking into notice required padding
//   static const uint8 TranslateAttributeSize[AttributeTypesCount] = 
//      {
//      0,    // None                      
//      0,    // Half                   
//      4,    // Half2                  
//      8,    // Half3               (6 bytes + 2 bytes of padding)
//      8,    // Half4                  
//      4,    // Float                  
//      8,    // Float2                 
//      12,   // Float3                 
//      16,   // Float4                 
//      0,    // Double                 
//      0,    // Double2                
//      0,    // Double3                
//      0,    // Double4                
//      0,    // Int8                   
//      4,    // Int8v2              (2 bytes + 2 bytes of padding)
//      4,    // Int8v3              (3 bytes + 1 byte  of padding)
//      4,    // Int8v4                 
//      0,    // Int16                  
//      4,    // Int16v2                
//      8,    // Int16v3             (6 bytes + 2 bytes of padding)
//      8,    // Int16v4                
//      4,    // Int32                  
//      8,    // Int32v2                
//      12,   // Int32v3                
//      16,   // Int32v4                
//      0,    // Int64                  
//      0,    // Int64v2                
//      0,    // Int64v3                
//      0,    // Int64v4                
//      0,    // UInt8                  
//      4,    // UInt8v2             (2 bytes + 2 bytes of padding)        
//      4,    // UInt8v3             (3 bytes + 1 byte  of padding)
//      4,    // UInt8v4                
//      0,    // UInt16                 
//      4,    // UInt16v2               
//      8,    // UInt16v3            (6 bytes + 2 bytes of padding)
//      8,    // UInt16v4               
//      4,    // UInt32                 
//      8,    // UInt32v2               
//      12,   // UInt32v3               
//      16,   // UInt32v4               
//      0,    // UInt64                 
//      0,    // UInt64v2               
//      0,    // UInt64v3               
//      0,    // UInt64v4               
//      0,    // Float8_SNorm           
//      4,    // Float8v2_SNorm      (2 bytes + 2 bytes of padding)
//      4,    // Float8v3_SNorm      (3 bytes + 1 byte  of padding)
//      4,    // Float8v4_SNorm         
//      0,    // Float16_SNorm          
//      4,    // Float16v2_SNorm        
//      8,    // Float16v3_SNorm     (6 bytes + 2 bytes of padding)
//      8,    // Float16v4_SNorm        
//      0,    // Float8_Norm            
//      4,    // Float8v2_Norm       (2 bytes + 2 bytes of padding)
//      4,    // Float8v3_Norm       (3 bytes + 1 byte  of padding)
//      4,    // Float8v4_Norm          
//      0,    // Float16_Norm           
//      4,    // Float16v2_Norm         
//      8,    // Float16v3_Norm      (6 bytes + 2 bytes of padding)
//      8,    // Float16v4_Norm         
//      4,    // Float4_10_10_10_2_SNorm
//      4     // Float4_10_10_10_2_Norm 
//      };
//
//
//class InputLayout
//   {
//   };
//
//class mtlInputLayout : public InputLayout
//   {
//   private:
//   MTLVertexDescriptor* desc;
//
//   public:
//   mtlInputLayout();
//  ~mtlInputLayout();
//   };
//
//
//mtlInputLayout::mtlInputLayout() :
//   desc([[MTLVertexDescriptor alloc] autorelease])
//{
//}
//
//mtlInputLayout::~mtlInputLayout()
//{
//[desc release];
//}
//
//Ptr<InputLayout> Create(AttributeInfo& attribute[MaxInputLayoutAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
//                           Ptr<Buffer>    buffer[MaxInputLayoutAttributesCount])      // Array of buffer handles that will be used
//{
//Ptr<mtlInputLayout> state = new mtlInputLayout();
//
//// Create array describing buffers used by Vertex Fetch.
//uint32 buffers = 0;
//for(; buffers<MaxInputLayoutAttributesCount; ++buffers)
//   {
//   // Passed array of buffers need to be tightly packed
//   if (!buffer[buffers])
//      break;
//
//   // There is no point in using MTLVertexStepFunctionConstant
//   // as we can pass the same data using regular buffer and sample
//   // it from any shader. There is also no real life case scenario
//   // to set "default" const values for missing buffers.
//   Ptr<mtlBuffer> src = ptr_dynamic_cast<mtlBuffer, Buffer>(buffer[buffers]);
//   if (src->step == 0)
//      {
//      state->desc.layouts[buffers].stepFunction = MTLVertexStepFunctionPerVertex;
//      state->desc.layouts[buffers].stepRate     = 1;
//      }
//   else
//      {
//      state->desc.layouts[buffers].stepFunction = MTLVertexStepFunctionPerInstance;
//      state->desc.layouts[buffers].stepRate     = src->step;
//      }
//
//   state->desc.layouts[buffers].stride = src->rowSize; // distance between data of consecutive vertices in bound buffer, ( multiple of 4 bytes )
//   }
//
//// Describe attributes and their connection with input buffers
//uint32 offset[MaxInputLayoutAttributesCount];
//memset(&offset, 0, sizeof(uint32) * MaxInputLayoutAttributesCount);
//
//for(uint32 i=0; i<MaxInputLayoutAttributesCount; ++i)
//   {
//   assert( attribute[i].buffer < MaxInputLayoutAttributesCount );
//
//   // Calculate new offset in currently used buffer
//   uint32 attributeSize = TranslateAttributeSize[ attribute[i].type ];
//   assert( attributeSize != 0 );
//
//   state->desc.attributes[i].format      = TranslateAttributeType[ attribute[i].type ];
//   state->desc.attributes[i].bufferIndex = attribute[i].buffer;
//   state->desc.attributes[i].offset      = offset[ attribute[i].buffer ];
//
//   // We assume that all attributes sharing the same buffer are 
//   // tightly packed one after another from it's beginning.
//   offset[ attribute[i].buffer ] += attributeSize;
//   }
//
//return ptr_dynamic_cast<InputLayout, mtlInputLayout>(state);
//}
//
//
//
//
//
//
//// Buffers can be rebind dynamically
//MTLRenderCommandEncoder
// setVertexBuffer:offset:atIndex:
// setVertexBuffers:offsets:withRange:
//
//
// drawPrimitives:vertexStart:vertexCount:instanceCount:
// drawIndexedPrimitives:indexCount:indexType:indexBuffer:indexBufferOffset:instanceCount:
//
//
//
//
//
//// Texture   
////---------------------------------------------------------------------
//
//
//   // Texture format
//   //
//   // Texture format describes how data of each texel are located in the memory. 
//   // Components order reflects their location in memory and is independent from
//   // computing unit endiannes and machine word size. Components are described 
//   // from left to right which represent their order from HI to LO bits in texel
//   // (similar to DXGI_FORMAT, and opposed to LO-HI bits order in D3DFMT).
//   // That order is followed by information about amount of bits each component 
//   // occupies and type of quantization in which data are stored.
//   //
//   // Possible quantization types:
//   //
//   // unsigned normalized  - (default, no postfix)
//   // signed normalized    - sn
//   // unsigned integral    - u
//   // signed integral      - s
//   // floating point       - f
//   // unsigned floating p. - uf
//   //
//   // sRGB - RGB channels are in sRGB color space (default is linear color space), Alpha channel if present is in linear color space
//   // pRGB - RGB channels are premultiplied by Alpha
//   //
//   // Examples:
//   //
//   // FormatRGB_3_3_2 :
//   // B - byte 0 - 2 bits [1:0]
//   // G - byte 0 - 3 bits [4:2]
//   // R - byte 0 - 3 bits [7:5]
//   //
//   // FormatRGBA_10_10_10_2:
//   // A - byte 0 - 2 bits [1:0]
//   // B - byte 0 - 6 bits [7:2], byte 1 - 4 bits [3:0]
//   // G - byte 1 - 4 bits [7:4], byte 2 - 6 bits [5:0]
//   // R - byte 2 - 2 bits [7:6], byte 3 - 8 bits [7:0]
//   //
//   // It is important to note that texture data conversion types are not supported
//   // (types that assume different upload format and different storage format) as
//   // well as channels swizzled formats are not exposed here (there are only few
//   // exceptions for loading specific file formats). If channels swizzling is 
//   // possible, it is set up separately.
//   //
//   enum TextureFormat
//      {
//      FormatUnsupported         = 0,
//      FormatR_8                    , // Uncompressed formats
//      FormatR_8_sn                 ,
//      FormatR_8_u                  ,
//      FormatR_8_s                  ,
//      FormatR_16                   ,
//      FormatR_16_sn                ,
//      FormatR_16_u                 ,
//      FormatR_16_s                 ,
//      FormatR_16_hf                ,
//      FormatR_32_u                 ,
//      FormatR_32_s                 ,
//      FormatR_32_f                 ,
//      FormatRG_8                   ,
//      FormatRG_8_sn                ,
//      FormatRG_8_u                 ,
//      FormatRG_8_s                 ,
//      FormatRG_16                  ,
//      FormatRG_16_sn               ,
//      FormatRG_16_u                ,
//      FormatRG_16_s                ,
//      FormatRG_16_hf               ,
//      FormatRG_32_u                ,
//      FormatRG_32_s                ,
//      FormatRG_32_f                ,
//      FormatRGB_8                  ,
//      FormatRGB_8_sn               ,
//      FormatRGB_8_u                ,
//      FormatRGB_8_s                ,
//      FormatRGB_8_sRGB             ,
//      FormatRGB_16                 ,
//      FormatRGB_16_sn              ,
//      FormatRGB_16_u               ,
//      FormatRGB_16_s               ,
//      FormatRGB_16_hf              ,
//      FormatRGB_32_u               ,
//      FormatRGB_32_s               ,
//      FormatRGB_32_f               ,
//      FormatRGBA_8                 ,
//      FormatRGBA_8_sRGB            ,
//      FormatRGBA_8_sn              ,
//      FormatRGBA_8_u               ,
//      FormatRGBA_8_s               ,
//      FormatRGBA_16                ,
//      FormatRGBA_16_sn             ,
//      FormatRGBA_16_u              ,
//      FormatRGBA_16_s              ,
//      FormatRGBA_16_hf             ,
//      FormatRGBA_32_u              ,
//      FormatRGBA_32_s              ,
//      FormatRGBA_32_f              ,
//      FormatD_16                   ,
//      FormatD_24                   ,
//      FormatD_32                   ,
//      FormatD_32_f                 ,
//      FormatS_8                    ,
//      FormatSD_8_24                ,
//      FormatSD_8_32_f              ,
//      FormatRGB_5_6_5              , // Packed/special formats
//      FormatBGR_5_6_5              , 
//      FormatBGR_8                  , // Special swizzled format for PNG (some Windows software use RGB_8)
//      FormatBGR_10_11_11_f         ,
//      FormatEBGR_5_9_9_9_f         ,
//      FormatBGR_16                 , // Special swizzled format for PNG (some Windows software use RGB_16)
//      FormatABGR_1_5_5_5           ,  
//      FormatARGB_1_5_5_5           ,  
//      FormatBGRA_5_5_5_1           ,  
//      FormatRGBA_5_5_5_1           ,  
//      FormatABGR_8                 ,  // Special swizzled format for: PNG
//      FormatARGB_8                 ,  // Special swizzled format for: bmp, TGA
//      FormatBGRA_8                 ,
//      FormatRGBA_10_10_10_2        ,
//      FormatRGBA_10_10_10_2_u      ,
//      FormatBC1_RGB                ,  // Compressed formats
//      FormatBC1_RGB_sRGB           ,  // S3TC DXT1
//      FormatBC1_RGBA               ,  // S3TC DXT1
//      FormatBC1_RGBA_sRGB          ,  // S3TC DXT1
//      FormatBC2_RGBA_pRGB          ,  // S3TC DXT2
//      FormatBC2_RGBA               ,  // S3TC DXT3
//      FormatBC2_RGBA_sRGB          ,  // S3TC DXT3
//      FormatBC3_RGBA_pRGB          ,  // S3TC DXT4
//      FormatBC3_RGBA               ,  // S3TC DXT5
//      FormatBC3_RGBA_sRGB          ,  // S3TC DXT5
//      FormatBC4_R                  ,  // RGTC
//      FormatBC4_R_sn               ,  // RGTC
//      FormatBC5_RG                 ,  // RGTC
//      FormatBC5_RG_sn              ,  // RGTC
//      FormatBC6H_RGB_f             ,  // BPTC
//      FormatBC6H_RGB_uf            ,  // BPTC
//      FormatBC7_RGBA               ,  // BPTC
//      FormatBC7_RGBA_sRGB          ,  // BPTC
//      FormatETC2_R_11              ,  // EAC
//      FormatETC2_R_11_sn           ,  // EAC
//      FormatETC2_RG_11             ,  // EAC
//      FormatETC2_RG_11_sn          ,  // EAC
//      FormatETC2_RGB_8             ,  // ETC1
//      FormatETC2_RGB_8_sRGB        ,  
//      FormatETC2_RGBA_8            ,  // EAC
//      FormatETC2_RGBA_8_sRGB       ,  // EAC
//      FormatETC2_RGB8_A1           ,  
//      FormatETC2_RGB8_A1_sRGB      ,  
//      FormatPVRTC_RGB_2            ,
//      FormatPVRTC_RGB_2_sRGB       ,
//      FormatPVRTC_RGB_4            ,
//      FormatPVRTC_RGB_4_sRGB       ,
//      FormatPVRTC_RGBA_2           ,
//      FormatPVRTC_RGBA_2_sRGB      ,
//      FormatPVRTC_RGBA_4           ,
//      FormatPVRTC_RGBA_4_sRGB      ,
//      FormatASTC_4x4               ,
//      FormatASTC_5x4               ,
//      FormatASTC_5x5               ,
//      FormatASTC_6x5               ,
//      FormatASTC_6x6               ,
//      FormatASTC_8x5               ,
//      FormatASTC_8x6               ,
//      FormatASTC_8x8               ,
//      FormatASTC_10x5              ,
//      FormatASTC_10x6              ,
//      FormatASTC_10x8              ,
//      FormatASTC_10x10             ,
//      FormatASTC_12x10             ,
//      FormatASTC_12x12             ,
//      FormatASTC_4x4_sRGB          ,
//      FormatASTC_5x4_sRGB          ,
//      FormatASTC_5x5_sRGB          ,
//      FormatASTC_6x5_sRGB          ,
//      FormatASTC_6x6_sRGB          ,
//      FormatASTC_8x5_sRGB          ,
//      FormatASTC_8x6_sRGB          ,
//      FormatASTC_8x8_sRGB          ,
//      FormatASTC_10x5_sRGB         ,
//      FormatASTC_10x6_sRGB         ,
//      FormatASTC_10x8_sRGB         ,
//      FormatASTC_10x10_sRGB        ,
//      FormatASTC_12x10_sRGB        ,
//      FormatASTC_12x12_sRGB        ,
//      TextureFormatsCount
//      };
//
//
//
//
//      
//      
//      
//      
//      
//      
//      
//      
//      
//      
//
//
//
//
//// Extremly fast ETC1 compressor:
//// https://bitbucket.org/wolfpld/etcpak/wiki/Home
//
//// EXT_texture_compression_s3tc
//GL_COMPRESSED_RGB_S3TC_DXT1_EXT            FormatBC1_RGB
//GL_COMPRESSED_RGBA_S3TC_DXT1_EXT           FormatBC1_RGBA
//GL_COMPRESSED_RGBA_S3TC_DXT3_EXT           FormatBC2_RGBA
//GL_COMPRESSED_RGBA_S3TC_DXT5_EXT           FormatBC3_RGBA
//
//// EXT_texture_sRGB
//GL_COMPRESSED_SRGB_S3TC_DXT1_EXT           FormatBC1_RGB_sRGB
//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT     FormatBC1_RGBA_sRGB
//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT     FormatBC2_RGBA_sRGB
//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT     FormatBC3_RGBA_sRGB
//
//// 3.0  or  ARB_texture_compression_rgtc
//GL_COMPRESSED_RED_RGTC1                    FormatBC4_R
//GL_COMPRESSED_SIGNED_RED_RGTC1             FormatBC4_R_sn
//GL_COMPRESSED_RG_RGTC2                     FormatBC5_RG
//GL_COMPRESSED_SIGNED_RG_RGTC2              FormatBC5_RG_sn
//// 4.2  or  ARB_texture_compression_bptc                        
//GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT        FormatBC6H_RGB_f
//GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT      FormatBC6H_RGB_uf
//GL_COMPRESSED_RGBA_BPTC_UNORM              FormatBC7_RGBA
//GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM        FormatBC7_RGBA_sRGB
//
///* ETC2 */
//// 4.3  or  ES 3.0
//GL_COMPRESSED_R11_EAC                        FormatEAC_R_11
//GL_COMPRESSED_SIGNED_R11_EAC                 FormatEAC_R_11_sn
//GL_COMPRESSED_RG11_EAC                       FormatEAC_RG_11
//GL_COMPRESSED_SIGNED_RG11_EAC                FormatEAC_RG_11_sn
//GL_COMPRESSED_RGB8_ETC2                      FormatETC2_RGB_8          // ETC1: ETC1_RGB8_OES  -->  OES_compressed_ETC1_RGB8_texture
//GL_COMPRESSED_SRGB8_ETC2                     FormatETC2_sRGB_8
//GL_COMPRESSED_RGBA8_ETC2_EAC                 FormatETC2_RGBA_8
//GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          FormatETC2_sRGB_A_8
//GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  FormatETC2_RGB8_A1
//GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 FormatETC2_sRGB8_A1
//
//// GL_KHR_texture_compression_astc_ldr
//// GL_KHR_texture_compression_astc_hdr
//
//GL_COMPRESSED_RGBA_ASTC_4x4_KHR            
//GL_COMPRESSED_RGBA_ASTC_5x4_KHR            
//GL_COMPRESSED_RGBA_ASTC_5x5_KHR            
//GL_COMPRESSED_RGBA_ASTC_6x5_KHR            
//GL_COMPRESSED_RGBA_ASTC_6x6_KHR            
//GL_COMPRESSED_RGBA_ASTC_8x5_KHR            
//GL_COMPRESSED_RGBA_ASTC_8x6_KHR            
//GL_COMPRESSED_RGBA_ASTC_8x8_KHR            
//GL_COMPRESSED_RGBA_ASTC_10x5_KHR           
//GL_COMPRESSED_RGBA_ASTC_10x6_KHR           
//GL_COMPRESSED_RGBA_ASTC_10x8_KHR           
//GL_COMPRESSED_RGBA_ASTC_10x10_KHR          
//GL_COMPRESSED_RGBA_ASTC_12x10_KHR          
//GL_COMPRESSED_RGBA_ASTC_12x12_KHR          
//
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR    
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR   
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR   
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR   
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR  
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR  
//GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR  
//
//
//
//
//
//   class Texture : public SafeObject
//      {
//      public:
//      virtual TextureType type(void) const = 0;
//      virtual TextureFormat format(void) const = 0;
//      virtual uint32   mipmaps(void) const = 0;
//      virtual uint32   width(const uint8 mipmap = 0) const = 0;
//      virtual uint32   height(const uint8 mipmap = 0) const = 0;
//      virtual uint32   depth(const uint8 mipmap = 0) const = 0;
//      virtual uint32v3 resolution(const uint8 mipmap = 0) const = 0;
//      virtual uint16   layers(void) const = 0;
//      virtual uint16   samples(void) const = 0;
//
//    // TODO:
//    //virtual void*    map(const uint16 layer = 0,
//    //                     const uint8 mipmap = 0) = 0;        // Maps texture array layer (or cubemap array face-layer) mipmap level to client memory
//    //virtual void*    map(const en::gpu::TextureFace face, 
//    //                     const uint8 mipmap = 0) = 0;        // Maps cubemap texture face mipmap to client memory
//    //virtual bool     unmap(void) = 0;                        // Unmaps texture from client memory
//    //
//    //virtual uint32   size(const uint8 mipmap = 0) const = 0; // Mipmap size in bytes
//    //virtual void     read(uint8* buffer, 
//    //                      const uint8 mipmap = 0) const = 0; // Reads texture mipmap to given buffer (app needs to allocate it)
//      };
//
//   class CommonTexture : public Texture
//      {
//      private:
//      TextureState state;
//
//      public:
//      CommonTexture(const TextureState& state);
//      virtual TextureType type(void) const;
//      virtual TextureFormat format(void) const;
//      virtual uint32   mipmaps(void) const;
//      virtual uint32   width(const uint8 mipmap = 0) const;
//      virtual uint32   height(const uint8 mipmap = 0) const;
//      virtual uint32   depth(const uint8 mipmap = 0) const;
//      virtual uint32v3 resolution(const uint8 mipmap = 0) const;
//      virtual uint16   layers(void) const;
//      virtual uint16   samples(void) const;
//
//    // TODO:
//    //virtual void*    map(const uint16 layer = 0,
//    //                     const uint8 mipmap = 0);            // Maps texture array layer (or cubemap array face-layer) mipmap level to client memory
//    //virtual void*    map(const en::gpu::TextureFace face, 
//    //                     const uint8 mipmap = 0);            // Maps cubemap texture face mipmap to client memory
//    //virtual bool     unmap(void);                            // Unmaps texture from client memory
//    //
//    //virtual uint32   size(const uint8 mipmap = 0) const;     // Mipmap size in bytes
//    //virtual void     read(uint8* buffer, 
//    //                      const uint8 mipmap = 0) const;     // Reads texture mipmap to given buffer (app needs to allocate it)
//      };
//
//
//
//// Texture   -   Common
////---------------------------------------------------------------------
//
//
//
//   // Calculate texture mipmaps count
//   uint32 TextureMipMapCount(const TextureState& state)
//   {
//   if (state.type == Texture2DRectangle        ||
//       state.type == Texture2DMultisample      ||
//       state.type == Texture2DMultisampleArray ||
//       state.type == TextureBuffer             )
//      return 1;
//
//   uint32 maxDimmension = state.width > state.height ? state.width : state.height;
//   if (maxDimmension < state.depth)
//      maxDimmension = state.depth;
//   
//   uint32 value = nextPowerOfTwo(maxDimmension);
//   uint32 mipmaps;
//   for(mipmaps = 1; mipmaps<32; ++mipmaps)
//      {
//      if (value & 1)
//         break;
//      value = value >> 1;
//      }
//
//   return mipmaps;
//   }

//// Texture   -   Metal
////---------------------------------------------------------------------
//
//
//   class mtlTexture : public CommonTexture
//      {
//      id <MTLTexture> handle;
//
//      public:
//      mtlTexture(const id<MTLTexture> handle, const TextureState& state);
//      };
//
//
//   // Metal Texturing:
//   //
//   // - is not supporting: 
//   //   - Texture2DRectangle (but that's legacy)
//   //   - TextureBuffer (but that's corner case usage like attrib divisor)
//   //
//   // - do we plan to support these?
//   //   - Texture2DMultisampleArray
//   //   - TextureCubeMapArray - in my opinion we should, this minimizes texture rebinds 
//
//   static const MTLTextureType TranslateTextureType[TextureTypesCount] =
//      {
//      0xFFFF,                        // Unknown
//      MTLTextureType1D,              // Texture1D
//      MTLTextureType1DArray,         // Texture1DArray
//      MTLTextureType2D,              // Texture2D
//      MTLTextureType2DArray,         // Texture2DArray
//      MTLTextureType2D,              // Texture2DRectangle   (we can emulate with 2D)
//      MTLTextureType2DMultisample,   // Texture2DMultisample
//      0xFFFF,                        // Texture2DMultisampleArray
//      MTLTextureType3D,              // Texture3D
//      0xFFFF,                        // TextureBuffer
//      MTLTextureTypeCube,            // TextureCubeMap
//      0xFFFF                         // TextureCubeMapArray
//      };
//
//   static const MTLPixelFormat TranslateTextureFormat[TextureFormatsCount] = 
//      {
//      MTLPixelFormatInvalid,         // Unassigned                           
//      MTLPixelFormatR8Unorm,         // FormatR_8      
//      MTLPixelFormatR8Snorm,         // FormatR_8_sn   
//      MTLPixelFormatR8Uint,          // FormatR_8_u    
//      MTLPixelFormatR8Sint,          // FormatR_8_s    
//      MTLPixelFormatR16Unorm,        // FormatR_16    
//      MTLPixelFormatR16Snorm,        // FormatR_16_sn 
//      MTLPixelFormatR16Uint,         // FormatR_16_u  
//      MTLPixelFormatR16Sint,         // FormatR_16_s  
//      MTLPixelFormatR16Float,        // FormatR_16_hf  
//      MTLPixelFormatR32Uint,         // FormatR_32_u   
//      MTLPixelFormatR32Sint,         // FormatR_32_s   
//      MTLPixelFormatR32Float,        // FormatR_32_f   
//      MTLPixelFormatRG8Unorm,        // FormatRG_8     
//      MTLPixelFormatRG8Snorm,        // FormatRG_8_sn  
//      MTLPixelFormatRG8Uint,         // FormatRG_8_u   
//      MTLPixelFormatRG8Sint,         // FormatRG_8_s   
//      MTLPixelFormatRG16Unorm,       // FormatRG_16    
//      MTLPixelFormatRG16Snorm,       // FormatRG_16_sn 
//      MTLPixelFormatRG16Uint,        // FormatRG_16_u  
//      MTLPixelFormatRG16Sint,        // FormatRG_16_s  
//      MTLPixelFormatRG16Float,       // FormatRG_16_hf 
//      MTLPixelFormatRG32Uint,        // FormatRG_32_u  
//      MTLPixelFormatRG32Sint,        // FormatRG_32_s  
//      MTLPixelFormatRG32Float,       // FormatRG_32_f  
//      0,                             // FormatBGR_2_3_3
//      0,                             // FormatRGB_3_3_2
//      0,                             // FormatRGB_4    
//      0,                             // FormatRGB_5    
//      MTLPixelFormatB5G6R5Unorm,     // FormatBGR_5_6_5
//      0,                             // FormatRGB_5_6_5
//      0,                             // FormatBGR_8    
//      0,                             // FormatRGB_8    
//      0,                             // FormatRGB_8_sn 
//      0,                             // FormatRGB_8_u  
//      0,                             // FormatRGB_8_s  
//      0,                             // Format_sRGB_8  
//      0,                             // FormatRGB_10   
//      MTLPixelFormatRG11B10Float,    // FormatBGR_10_11_11_f
//      MTLPixelFormatRGB9E5Float,     // FormatEBGR_5_9_9_9_f
//      0,                             // FormatRGB_12    
//      0,                             // FormatBGR_16    
//      0,                             // FormatRGB_16    
//      0,                             // FormatRGB_16_sn 
//      0,                             // FormatRGB_16_u  
//      0,                             // FormatRGB_16_s  
//      0,                             // FormatRGB_16_hf   
//      0,                             // FormatRGB_32_u  
//      0,                             // FormatRGB_32_s  
//      0,                             // FormatRGB_32_f  
//      0,                             // FormatRGBA_2
//      MTLPixelFormatABGR4Unorm,      // FormatABGR_4             
//      0,                             // FormatARGB_4
//      0,                             // FormatBGRA_4             
//      0,                             // FormatRGBA_4             
//      MTLPixelFormatA1BGR5Unorm,     // FormatABGR_1_5_5_5
//      0,                             // FormatARGB_1_5_5_5
//      0,                             // FormatBGRA_5_5_5_1
//      0,                             // FormatRGBA_5_5_5_1
//      0,                             // FormatABGR_8      
//      0,                             // FormatARGB_8      
//      MTLPixelFormatBGRA8Unorm,      // FormatBGRA_8      
//      MTLPixelFormatRGBA8Unorm,      // FormatRGBA_8
//      MTLPixelFormatRGBA8Unorm_sRGB, // Format_sRGB_A_8
//      MTLPixelFormatRGBA8Snorm,      // FormatRGBA_8_sn
//      MTLPixelFormatRGBA8Uint,       // FormatRGBA_8_u
//      MTLPixelFormatRGBA8Sint,       // FormatRGBA_8_s
//      MTLPixelFormatRGB10A2Unorm,    // FormatRGBA_10_10_10_2
//      MTLPixelFormatRGB10A2Uint,     // FormatRGBA_10_10_10_2_u
//      MTLPixelFormatRGBA16Unorm,     // FormatRGBA_16   
//      MTLPixelFormatRGBA16Snorm,     // FormatRGBA_16_sn
//      MTLPixelFormatRGBA16Uint,      // FormatRGBA_16_u 
//      MTLPixelFormatRGBA16Sint,      // FormatRGBA_16_s 
//      MTLPixelFormatRGBA16Float,     // FormatRGBA_16_hf
//      MTLPixelFormatRGBA32Uint,      // FormatRGBA_32_u
//      MTLPixelFormatRGBA32Sint,      // FormatRGBA_32_s
//      MTLPixelFormatRGBA32Float,     // FormatRGBA_32_f
//      0,                             // FormatD_16       
//      0,                             // FormatD_24       
//      0,                             // FormatD_32       
//      MTLPixelFormatDepth32Float,    // FormatD_32_f   
//      MTLPixelFormatStencil8,        // FormatS_8
//      0,                             // FormatSD_8_24    
//      0,                             // FormatSD_8_32f   
//      MTLPixelFormatASTC_4x4_LDR,    // FormatASTC_4x4        
//      MTLPixelFormatASTC_5x4_LDR,    // FormatASTC_5x4        
//      MTLPixelFormatASTC_5x5_LDR,    // FormatASTC_5x5        
//      MTLPixelFormatASTC_6x5_LDR,    // FormatASTC_6x5        
//      MTLPixelFormatASTC_6x6_LDR,    // FormatASTC_6x6        
//      MTLPixelFormatASTC_8x5_LDR,    // FormatASTC_8x5        
//      MTLPixelFormatASTC_8x6_LDR,    // FormatASTC_8x6        
//      MTLPixelFormatASTC_8x8_LDR,    // FormatASTC_8x8        
//      MTLPixelFormatASTC_10x5_LDR,   // FormatASTC_10x5       
//      MTLPixelFormatASTC_10x6_LDR,   // FormatASTC_10x6       
//      MTLPixelFormatASTC_10x8_LDR,   // FormatASTC_10x8       
//      MTLPixelFormatASTC_10x10_LDR,  // FormatASTC_10x10      
//      MTLPixelFormatASTC_12x10_LDR,  // FormatASTC_12x10      
//      MTLPixelFormatASTC_12x12_LDR,  // FormatASTC_12x12      
//      MTLPixelFormatASTC_4x4_sRGB,   // FormatASTC_4x4_sRGB   
//      MTLPixelFormatASTC_5x4_sRGB,   // FormatASTC_5x4_sRGB   
//      MTLPixelFormatASTC_5x5_sRGB,   // FormatASTC_5x5_sRGB   
//      MTLPixelFormatASTC_6x5_sRGB,   // FormatASTC_6x5_sRGB   
//      MTLPixelFormatASTC_6x6_sRGB,   // FormatASTC_6x6_sRGB   
//      MTLPixelFormatASTC_8x5_sRGB,   // FormatASTC_8x5_sRGB   
//      MTLPixelFormatASTC_8x6_sRGB,   // FormatASTC_8x6_sRGB   
//      MTLPixelFormatASTC_8x8_sRGB,   // FormatASTC_8x8_sRGB   
//      MTLPixelFormatASTC_10x5_sRGB,  // FormatASTC_10x5_sRGB  
//      MTLPixelFormatASTC_10x6_sRGB,  // FormatASTC_10x6_sRGB  
//      MTLPixelFormatASTC_10x8_sRGB,  // FormatASTC_10x8_sRGB  
//      MTLPixelFormatASTC_10x10_sRGB, // FormatASTC_10x10_sRGB 
//      MTLPixelFormatASTC_12x10_sRGB, // FormatASTC_12x10_sRGB 
//      MTLPixelFormatASTC_12x12_sRGB, // FormatASTC_12x12_sRGB
//      };
//
//   Ptr<Texture> Create(const TextureState& state)
//   {
//   Ptr<Texture> texture = nullptr;
//
//   // Verify if given texture type is supported
//   MTLTextureType type = TranslateTextureType[state.type];
//   if (type == 0xFFFF)
//      return texture;
//
//   // Verify if given texture format is supported
//   MTLPixelFormat format = TranslateTextureFormat[state.format];
//   if (format == MTLPixelFormatInvalid)
//      return texture;
//
//   // Calculate amount of mipmaps texture will have
//   uint32 mipmaps = TextureMipMapCount(state);
//
//   MTLTextureDescriptor* desc = [[MTLTextureDescriptor alloc] autorelease];
//   desc.textureType      = type;
//   desc.pixelFormat      = TranslateFormat[state.format];
//   desc.width            = state.width;
//   desc.height           = state.height;
//   desc.depth            = state.depth;
//   desc.mipmapLevelCount = mipmaps;
//   desc.arrayLength      = state.layers;
//   desc.sampleCount      = state.samples;
// //desc.resourceOptions  = MTLResourceOptionCPUCacheModeWriteCombined;
//   desc.cpuCacheMode     = MTLCPUCacheModeWriteCombined;
//
//   id<MTLTexture> handle = [[device newTextureWithDescriptor:desc] autorelease];   // device <-- we need global here, or getDevice()
//   if (handle != nullptr)
//      {
//      Ptr<mtlTexture> ptr = new mtlTexture(handle, state);
//      ptr->mipmaps = mipmaps;
//      texture = ptr_dynamic_cast<Texture, mtlTexture>(ptr);
//      }
//
//   [desc release];
//   return texture;
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//   MTLPixelFormatA8Unorm       = 1,
//   MTLPixelFormatR8Unorm_sRGB  = 11,
//   MTLPixelFormatRG8Unorm_sRGB = 31,
//   MTLPixelFormatBGRA8Unorm_sRGB  = 81,
//
//// Metal PixelFormat questions:
////
//// PixelFormat notation is inconsistent. 
//// In most cases it represents channel order from HI to LO bits in texel.
//// But there are PixelFormats that don't hold to that and describe components in reversed order - LO to HI bits.
//// Or maybe it describes internal storage layout in GPU/sysmem ?
////
//// MTLPixelFormatRGB9E5Float
//// - what is the order of components?
////   According to OpenGL and Direct3D it should be:  [HI bits] 5 9 9 9 [LO bits]
////   But there is no break down chart for that in the Metal specification.
////
////   Bonus: We can break format description here like OpenGL and Direct3D does, or make one of 2:
////   - rename it to hold format description consistency (HI to LO bits) then it should be: MTLPixelFormatE5BGR9Float (assuming order: [HI] e5 b9 g9 r9 [LO])
////   - explicitly state in documentation that packed pixel formats are not holding to Hi-LO notation but to LO-HI nottion (bits order in memory, rather than in register)
////     but even then there are exceptions from that new rule like: MTLPixelFormatRGB10A2Unorm and MTLPixelFormatRGB10A2Uint
////
//// MTLPixelFormatRG11B10Float
//// - like above, pixed format described from LO to HI bits in register
////
//// MTLPixelFormatRGB10A2Unorm
//// MTLPixelFormatRGB10A2Uint
//// - what is the order of components? HI to LO, or LO to HI ? 
////   I would expect it to be HI to LO like in Direct3D but OpenGL specification allows both.
////
//// MTLPixelFormatRGBA8Unorm_sRGB 
//// - in what color space is alpha channel? SRGB or Linear?
////   It should be linear, but there is no word about it in the spec.
////
//// MTLPixelFormatA1BGR5Unorm
//// - is this [HI] a1 b5 g5 r5 [LO] like in (thumbnails / CGBitmapContext / CGImage) or [HI] a1 r5 g5 b5 [LO] like in Direct3D ?
////   Bonus:
////   As comparison OpenGL ES is not supporting texture upload in this order. It only supports:
////   [HI] r5 g5 b5 a1 [LO] and [HI] b5 g5 r5 a1 [LO] GL_RGB5_A1 GL_RGBA/GL_BGRA and GL_UNSIGNED_SHORT_5_5_5_1
////   ( UNSIGNED_SHORT_4_4_4_4_REV and GL_UNSIGNED_SHORT_1_5_5_5_REV are only supported for ReadPixel, not for uploading textures:
////     https://www.khronos.org/registry/gles/extensions/EXT/EXT_read_format_bgra.txt )
////   This is also legacy format that doesn't benefit anymore in times of compressed pixel formats which give better quality with less bits: https://www.opengl.org/wiki/Image_Format
////
//// MTLPixelFormatABGR4Unorm
//// - similar to above, is this [HI] a4 b4 g4 r4 [LO] ?
////   Bonus:
////   [HI] b4 g4 r4 a4 [LO] Supported by OpenGL and Direct3D.
////   [HI] b4 g4 r4 a4 [LO] Supported by OpenGL only.
////
//      
//         { OpenGL_1_2, false,  16, GL_RGBA4,              GL_BGRA,            GL_UNSIGNED_SHORT_4_4_4_4         },   // FormatBGRA_4                     
//         { OpenGL_1_2, false,  16, GL_RGBA4,              GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4         },   // FormatRGBA_4
//
//         { OpenGL_1_2, false,  16, GL_RGB5_A1,            GL_BGRA,            GL_UNSIGNED_SHORT_5_5_5_1         },   // FormatARGB_1_5_5_5   
//         { OpenGL_1_2, false,  16, GL_RGB5_A1,            GL_RGBA,            GL_UNSIGNED_SHORT_5_5_5_1         },   // FormatABGR_1_5_5_5 
//
//
////
//// In Apple were doing products simple to use and understand, yet still Metal API specification is written with assumption that "everybody knows that stuff".
//// As a result it is not helpfull at all to any developer as it has no detailed descriptions. Just look at OpenGL or Direct3D specifications.
//// The fact that API is simple and easy to use doesn't mean specification should be lacking all that required information (it should contain comprehensive descriptions to make learning API simple, not looking simple).
//
//// I'm seeing here disjoint between making API _simple_ to use and learn and making it look _simple_ (by having general specification not going to the details).
//
//
//
//
//   /* Compressed formats. */
//   /* PVRTC */
//   MTLPixelFormatPVRTC_RGB_2BPP       = 160,
//   MTLPixelFormatPVRTC_RGB_2BPP_sRGB  = 161,
//   MTLPixelFormatPVRTC_RGB_4BPP       = 162,
//   MTLPixelFormatPVRTC_RGB_4BPP_sRGB  = 163,
//   MTLPixelFormatPVRTC_RGBA_2BPP      = 164,
//   MTLPixelFormatPVRTC_RGBA_2BPP_sRGB = 165,
//   MTLPixelFormatPVRTC_RGBA_4BPP      = 166,
//   MTLPixelFormatPVRTC_RGBA_4BPP_sRGB = 167,
//   /* ETC2 */
//   MTLPixelFormatEAC_R11Unorm     = 170,
//   MTLPixelFormatEAC_R11Snorm     = 172,
//   MTLPixelFormatEAC_RG11Unorm    = 174,
//   MTLPixelFormatEAC_RG11Snorm    = 176,
//   MTLPixelFormatEAC_RGBA8        = 178,
//   MTLPixelFormatEAC_RGBA8_sRGB   = 179,
//   MTLPixelFormatETC2_RGB8        = 180,
//   MTLPixelFormatETC2_RGB8_sRGB   = 181,
//   MTLPixelFormatETC2_RGB8A1      = 182,
//   MTLPixelFormatETC2_RGB8A1_sRGB = 183,
//   
//
// 
//   
//
//   
//   MTLPixelFormatGBGR422          = 240,
//   MTLPixelFormatBGRG422          = 241,
// 
//
//// ASTC (Adaptive Scalable Texture Compression) format, with a block size of 4 (width) x 4 (height) pixels - the highest quality, 
//// but lowest compression. The 12 x 12 formats have the greatest compression, and lowest quality. The error color magenta 
//// (1.0, 0.0, 1.0, 1.0) is used when a non-valid ASTC block is decoded. ASTC provides developers with greater control over the 
//// size verses quality tradeoff with textures. ASTC is a lossy format, but one that is designed to provide an inexpensive route 
//// to greater quality textures. The idea is that a developer can choose the optimum format without having to support multiple 
//// compression schemes.
//
//
//
//
//   static const DXGI_FORMAT TranslateTextureFormat[TextureFormatsCount] = 
//      {
//      // Unassigned                           
//      // FormatR_8      
//      // FormatR_8_sn   
//      // FormatR_8_u    
//      // FormatR_8_s    
//      // FormatR_16    
//      // FormatR_16_sn 
//      // FormatR_16_u  
//      // FormatR_16_s  
//      // FormatR_16_hf  
//      // FormatR_32_u   
//      // FormatR_32_s   
//      // FormatR_32_f   
//      // FormatRG_8     
//      // FormatRG_8_sn  
//      // FormatRG_8_u   
//      // FormatRG_8_s   
//      // FormatRG_16    
//      // FormatRG_16_sn 
//      // FormatRG_16_u  
//      // FormatRG_16_s  
//      // FormatRG_16_hf 
//      // FormatRG_32_u  
//      // FormatRG_32_s  
//      // FormatRG_32_f  
//      // FormatBGR_2_3_3
//      // FormatRGB_3_3_2
//      // FormatRGB_4    
//      // FormatRGB_5    
//      // FormatBGR_5_6_5
//      // FormatRGB_5_6_5
//      // FormatBGR_8    
//      // FormatRGB_8    
//      // FormatRGB_8_sn 
//      // FormatRGB_8_u  
//      // FormatRGB_8_s  
//      // Format_sRGB_8  
//      // FormatRGB_10   
//      // FormatBGR_10_11_11_f
//      // FormatEBGR_5_9_9_9_f
//      // FormatRGB_12    
//      // FormatBGR_16    
//      // FormatRGB_16    
//      // FormatRGB_16_sn 
//      // FormatRGB_16_u  
//      // FormatRGB_16_s  
//      // FormatRGB_16_hf 
//      // FormatRGB_16_f  
//      // FormatRGB_32_u  
//      // FormatRGB_32_s  
//      // FormatRGB_32_f  
//      // FormatRGBA_2
//      // FormatABGR_4
//      // FormatARGB_4
//      // FormatBGRA_4
//      // FormatRGBA_4
//      // FormatABGR_1_5_5_5
//      // FormatARGB_1_5_5_5
//      // FormatBGRA_5_5_5_1
//      // FormatRGBA_5_5_5_1
//      // FormatABGR_8      
//      // FormatARGB_8      
//      // FormatBGRA_8      
//      // FormatRGBA_8
//      // Format_sRGB_A_8
//      // FormatRGBA_8_sn
//      // FormatRGBA_8_u
//      // FormatRGBA_8_s
//      // FormatRGBA_10_10_10_2
//      // FormatRGBA_10_10_10_2_u
//      // FormatRGBA_16   
//      // FormatRGBA_16_sn
//      // FormatRGBA_16_u 
//      // FormatRGBA_16_s 
//      // FormatRGBA_16_hf
//      // FormatRGBA_32_u
//      // FormatRGBA_32_s
//      // FormatRGBA_32_f
//      // FormatD_16       
//      // FormatD_24       
//      // FormatD_32       
//      // FormatD_32_f   
//      // FormatS_8
//      // FormatSD_8_24    
//      // FormatSD_8_32f   
//      DXGI_FORMAT_ASTC_4X4_UNORM,        // FormatASTC_4x4        
//      DXGI_FORMAT_ASTC_5X4_UNORM,        // FormatASTC_5x4        
//      DXGI_FORMAT_ASTC_5X5_UNORM,        // FormatASTC_5x5        
//      DXGI_FORMAT_ASTC_6X5_UNORM,        // FormatASTC_6x5        
//      DXGI_FORMAT_ASTC_6X6_UNORM,        // FormatASTC_6x6        
//      DXGI_FORMAT_ASTC_8X5_UNORM,        // FormatASTC_8x5        
//      DXGI_FORMAT_ASTC_8X6_UNORM,        // FormatASTC_8x6        
//      DXGI_FORMAT_ASTC_8X8_UNORM,        // FormatASTC_8x8        
//      DXGI_FORMAT_ASTC_10X5_UNORM,       // FormatASTC_10x5       
//      DXGI_FORMAT_ASTC_10X6_UNORM,       // FormatASTC_10x6       
//      DXGI_FORMAT_ASTC_10X8_UNORM,       // FormatASTC_10x8       
//      DXGI_FORMAT_ASTC_10X10_UNORM,      // FormatASTC_10x10      
//      DXGI_FORMAT_ASTC_12X10_UNORM,      // FormatASTC_12x10      
//      DXGI_FORMAT_ASTC_12X12_UNORM,      // FormatASTC_12x12      
//      DXGI_FORMAT_ASTC_4X4_UNORM_SRGB,   // FormatASTC_4x4_sRGB   
//      DXGI_FORMAT_ASTC_5X4_UNORM_SRGB,   // FormatASTC_5x4_sRGB   
//      DXGI_FORMAT_ASTC_5X5_UNORM_SRGB,   // FormatASTC_5x5_sRGB   
//      DXGI_FORMAT_ASTC_6X5_UNORM_SRGB,   // FormatASTC_6x5_sRGB   
//      DXGI_FORMAT_ASTC_6X6_UNORM_SRGB,   // FormatASTC_6x6_sRGB   
//      DXGI_FORMAT_ASTC_8X5_UNORM_SRGB,   // FormatASTC_8x5_sRGB   
//      DXGI_FORMAT_ASTC_8X6_UNORM_SRGB,   // FormatASTC_8x6_sRGB   
//      DXGI_FORMAT_ASTC_8X8_UNORM_SRGB,   // FormatASTC_8x8_sRGB   
//      DXGI_FORMAT_ASTC_10X5_UNORM_SRGB,  // FormatASTC_10x5_sRGB  
//      DXGI_FORMAT_ASTC_10X6_UNORM_SRGB,  // FormatASTC_10x6_sRGB  
//      DXGI_FORMAT_ASTC_10X8_UNORM_SRGB,  // FormatASTC_10x8_sRGB  
//      DXGI_FORMAT_ASTC_10X10_UNORM_SRGB, // FormatASTC_10x10_sRGB 
//      DXGI_FORMAT_ASTC_12X10_UNORM_SRGB, // FormatASTC_12x10_sRGB 
//      DXGI_FORMAT_ASTC_12X12_UNORM_SRGB, // FormatASTC_12x12_sRGB
//      };
//
//
//

//
//   // METAL COLOR ATTACHMENT
//   //--------------------------------------------------------------------------
//
//   #if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//
//   class mtlColorAttachment : public ColorAttachment
//      {
//      private:
//      MTLRenderPassColorAttachmentDescriptor* desc;
//
//      public:
//      mtlColorAttachment();
//     ~mtlColorAttachment();
//      onLoad(const LoadOperation load, 
//             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
//      onStore(const StoreOperation store);
//      resolve(const Ptr<Texture> texture, 
//              const uint32 mipmap = 0,
//              const uint32 layer = 0);
//      };
//
//   mtlColorAttachment::mtlColorAttachment() :
//      desc([[[MTLRenderPassColorAttachmentDescriptor alloc] init] autorelease])
//   {
//   }
//
//   mtlColorAttachment::~mtlColorAttachment()
//   {
//   [desc release];
//   }
//
//
//   Ptr<ColorAttachment> Create(const Ptr<Texture> texture, 
//                               const TextureFormat format,
//                               const uint32 mipmap,
//                               const uint32 layer,
//                               const uint32 layers)
//   {
//   assert( color );
//
//   Ptr<mtlColorAttachment> attachment = new mtlColorAttachment();
//
//   Ptr<mtlTexture> color = ptr_dynamic_cast<mtlTexture, Texture>(texture);
//
//   attachment->desc.texture       = color->id;
//   attachment->desc.level         = mipmap;
//   attachment->desc.slice         = 0;
//   attachment->desc.depthPlane    = 0;
//
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor
//   // TextureCubeMapArray
//   if (color->state.type == Texture1DArray ||
//       color->state.type == Texture2DArray ||
//       color->state.type == TextureCubeMap)
//      attachment->desc.slice      = layer;
//   else
//   if (color->state.type == Texture3D)
//      attachment->desc.depthPlane = layer;
//
//   // Metal is ignoring "layers" parameter as it has no
//   // way to select RT layer due to missing Geometry Shader.
//   // It is also ignoring "format" parameter for now.
//
//   // TODO: Do we need to store "format" for some reason ?
//
//   return ptr_dynamic_cast<ColorAttachment, mtlColorAttachment>(attachment);
//   }
//
//
//   mtlColorAttachment::onLoad(const LoadOperation load, const float4 clearColor)
//   {
//   desc.loadAction = TranslateLoadOperation[load];
//   desc.clearColor = MTLClearColorMake( static_cast<double>(clearColor.r), 
//                                        static_cast<double>(clearColor.g), 
//                                        static_cast<double>(clearColor.b), 
//                                        static_cast<double>(clearColor.a) );
//   }
//
//   mtlColorAttachment::onStore(const StoreOperation store)
//   {
//   desc.storeAction = TranslateStoreOperation[store];
//   }
//
//   mtlColorAttachment::resolve(const Ptr<Texture> texture, 
//                               const uint32 mipmap,  
//                               const uint32 layer)
//   {
//   assert( color );
//
//   Ptr<mtlTexture> resolve = ptr_dynamic_cast<mtlTexture, Texture>(texture);
//
//   desc.resolveTexture    = resolve->id;
//   desc.resolveLevel      = mipmap;
//   desc.resolveSlice      = 0;
//   desc.resolveDepthPlane = 0; 
//
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor
//   // TextureCubeMapArray
//   if (color->state.type == Texture1DArray ||
//       color->state.type == Texture2DArray ||
//       color->state.type == TextureCubeMap)
//      desc.resolveSlice = layer;
//   else
//   if (color->state.type == Texture3D)
//      sesc.resolveDepthPlane = layer;
//   }
//
//   #endif
//
//   // VULKAN COLOR ATTACHMENT
//   //--------------------------------------------------------------------------
//
//   #if defined(EN_PLATFORM_WINDOWS)
//
//   class vkColorAttachment : public ColorAttachment 
//      {
//      public:
//      VkColorAttachmentView id;
//
//      // Cache state to use by RenderPass
//      LoadOperation  load;
//      float4         clearColor;
//      StoreOperation store;
//      Ptr<Texture>   resolve;
//      uint32         mipmap;
//      uint32         layer;
//
//      onLoad(const LoadOperation load, 
//             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
//      onStore(const StoreOperation store);
//      resolve(const Ptr<Texture> texture, 
//              const uint32 mipmap = 0,
//              const uint32 layer = 0);
//      };
//
//   Ptr<ColorAttachment> Create(const Ptr<Texture> texture, 
//                               const TextureFormat format, 
//                               const uint32 mipmap,
//                               const uint32 layer,
//                               const uint32 layers)
//   {
//   assert( color );
//
//   Ptr<ColorAttachment> result = nullptr;
//
//   Ptr<vkTexture> color = ptr_dynamic_cast<vkTexture, Texture>(texture);
//
//   VkColorAttachmentViewCreateInfo desc;
//   desc.sType            = VK_STRUCTURE_TYPE_COLOR_ATTACHMENT_VIEW_CREATE_INFO;
//   desc.pNext            = nullptr;
//   desc.image            = color->id;
//   desc.format           = TranslateTextureFormat[format];
//   desc.mipLevel         = mipmap;
//   desc.baseArraySlice   = layer;
//   desc.arraySize        = layers;
//   desc.msaaResolveImage = nullptr;
//
//   // Vulkan is ignoring "depth" parameter.
//   // VULKAN TODO: Does vulkan support binding 3D and Cube textures ?
//   //              Should their depth/face be passed through baseArraySlice ?
//   
//   VkColorAttachmentView colorAttView;
//   VkResult res = vkCreateColorAttachmentView( gpu[i].handle, &desc, &colorAttView );              // device handle here !!!
//   if (!res)
//      {
//      Ptr<vkColorAttachment> vkAttachment = new vkColorAttachment();
//      vkAttachment->id = colorAttView;
//      result = ptr_dynamic_cast<ColorAttachment, vkColorAttachment>(vkAttachment);
//      }
//
//   return result;
//   }
//
//   vkColorAttachment::onLoad(const LoadOperation _load, const float4 _clearColor)
//   {
//   load       = _load;
//   clearColor = _clearColor;
//   }
//
//   vkColorAttachment::onStore(const StoreOperation store)
//   {
//   store = _store;
//   }
//
//   vkColorAttachment::resolve(const Ptr<Texture> texture, 
//                              const uint32 mipmap,  
//                              const uint32 layer)
//   {
//   assert( texture );
//
//   texture = _texture;
//   mipmap  = _mipmap;
//   layer   = _layer;
//   }
//
//   //Ptr<vkTexture> vkMsaa  = ptr_dynamic_cast<vkTexture, Texture>(resolveMsaa);
//
//   //VkImageSubresourceRange resourceRange;
//   //resourceRange.aspect                       = VK_IMAGE_ASPECT_COLOR;
//   //resourceRange.baseMipLevel                 = msaaMipMap;
//   //resourceRange.mipLevels                    = 1;
//   //resourceRange.baseArraySlice               = msaaLayer;
//   //resourceRange.arraySize                    = layers;
//
//   //colorAttachmentInfo.msaaResolveImage       = vkMsaa->id;
//   //colorAttachmentInfo.msaaResolveSubResource = resourceRange;
//
//
//   //                            //const Ptr<Texture> resolveMsaa,
//
//   #endif
//
//
//
//
//
//
//
//
//
//   class mtlDepthStencilAttachment : public DepthStencilAttachment
//      {
//      private:
//      MTLRenderPassDepthAttachmentDescriptor*   depth;
//      MTLRenderPassStencilAttachmentDescriptor* stencil;
//
//      public:
//      mtlDepthStencilAttachment();
//     ~mtlDepthStencilAttachment();
//      onLoad(const LoadOperation load, 
//             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
//      onStore(const StoreOperation store);
//      resolve(const Ptr<Texture> color, 
//              const uint32 mipmap = 0,
//              const uint32 depth = 0,
//              const uint32 layer = 0);
//      };
//
//   mtlDepthStencilAttachment::mtlDepthStencilAttachment() :
//      depth([[MTLRenderPassDepthAttachmentDescriptor alloc] autorelease]),
//      stencil([[MTLRenderPassStencilAttachmentDescriptor alloc] autorelease])
//   {
//   }
//
//   mtlDepthStencilAttachment::~mtlDepthStencilAttachment()
//   {
//   [depth   release];
//   [stencil release];
//   }
//
//
//
//
//   Ptr<DepthStencilAttachment> Create(const Ptr<Texture> , 
//                               const TextureFormat format,
//                               const uint32 mipmap,
//                               const uint32 depth,
//                               const uint32 layer,
//                               const uint32 layers
//
//                               const Ptr<Texture> stencil, 
//                               const TextureFormat stencilFormat,
//                               const uint32 stencilMipmap,
//                               const uint32 stencilLayer,
//
//                               const Ptr<Texture> resolveMsaa, 
//                               const uint32 resolveMipmap,
//                               const uint32 resolveLayer,
//)
//   {
//   Ptr<DepthAttachment> result = nullptr;
//
//   Ptr<mtlTexture> mtlDepth = ptr_dynamic_cast<mtlTexture, Texture>(depth);
//   Ptr<mtlTexture> mtlMsaa  = ptr_dynamic_cast<mtlTexture, Texture>(resolveMsaa);
//
//   MTLRenderPassDepthAttachmentDescriptor* depthAttachmentInfo = [[MTLRenderPassDepthAttachmentDescriptor alloc] init];
//   depthAttachmentInfo.texture           = depth ? mtlDepth->id : nil;
//   depthAttachmentInfo.level             = depthMipMap;
//   depthAttachmentInfo.slice             = depthLayer;
//   depthAttachmentInfo.depthPlane        = 0;                   // depth in 3D texture
//   depthAttachmentInfo.loadAction        =
//   depthAttachmentInfo.storeAction       =
//   depthAttachmentInfo.resolveTexture    = resolveMsaa ? mtlMsaa->id : nil;
//   depthAttachmentInfo.resolveLevel      = msaaMipMap;
//   depthAttachmentInfo.resolveSlice      = msaaLayer;
//   depthAttachmentInfo.resolveDepthPlane = 0;                   // depth in 3D texture
//   // Depth Attachment specific
//   depthAttachmentInfo.clearDepth        = clearValue; // double
// 
//
//   }
//
//
//   //#endif
//
//
//
//
//
//   class RenderPass : public SafeObject
//      {
//      };
//
//   class mtlRenderPass : public RenderPass
//      {
//      MTLRenderPassDescriptor* desc;
//
//      mtlRenderPass();
//     ~mtlRenderPass();
//      };
//
//   mtlRenderPass::mtlRenderPass() :
//      desc([[MTLRenderPassDescriptor alloc] autorelease])
//   {
//   }
//
//   mtlRenderPass::~mtlRenderPass()
//   {
//   [desc release];
//   }
//
//
//   Ptr<RenderPass> Create(const uint32v2 resolution,
//                          const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
//                          const Ptr<DepthAttachment> depth,
//                          const Ptr<StencilAttachment> stencil)
//   {
//   Ptr<mtlRenderPass> pass = new mtlRenderPass();
//
//   pass->desc.visibilityResultBuffer = buffer;
//
//   for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
//      {
//      MTLRenderPassColorAttachmentDescriptor* colorAttachment = pass->desc.colorAttachments[i];
//      colorAttachment.texture           = drawable.currentTexture;
//      colorAttachment.level             = 0;
//      colorAttachment.slice             = 0;
//      colorAttachment.depthPlane        = 0;
//      colorAttachment.resolveTexture    = nil;
//      colorAttachment.resolveLevel      = 0;
//      colorAttachment.resolveSlice      = 0;
//      colorAttachment.resolveDepthPlane = 0;
//      colorAttachment.loadAction        = MTLLoadActionClear;
//      colorAttachment.storeAction       = MTLStoreActionDontCare;
//      colorAttachment.clearColor        = MTLClearColorMake(0.2f,0.3f,0.4f,1.0f);
//      }
//
//   // Depth
//   Ptr<mtlDepthAttachment> mtlDepth = ptr_dynamic_cast<mtlDepthAttachment, DepthAttachment>(depth);
//   depthAttachment = mtlDepth->desc;
//
//   // Stencil
//   Ptr<mtlStencilAttachment> mtlStencil = ptr_dynamic_cast<mtlStencilAttachment, StencilAttachment>(stencil);
//   stencilAttachment = mtlStencil->desc;
//
//   return ptr_dynamic_cast<RenderPass, mtlRenderPass>(pass);
//   }
//
//
//
//
//
//   // One encoder at a time in buffer (except of parallel)
//   id <MTLRenderCommandEncoder>
//   id <MTLParallelRenderCommandEncoder>
//   id <MTLComputeRenderCommandEncoder>
//   id <MTLBlitRenderCommandEncoder>
//
//
//   // Vulkan Command Buffer "recycle/reuse" functionality can be emulated on Metal by destroying and recreating the command buffer
//
//
//   Ptr<RenderingQueue> rasterize = gpu.graphicQueue();
//   Ptr<ComputeQueue> compute
//   Ptr<DataQueue> data
//
//   Ptr<CommandBuffer> cmd = render.CreateBuffer();
//
//   // What's the point of exposing command encoder in Metal ?
//   VkCmdBuffer
//
//
// 
//
//
//   // Vulkan has Separate queues for rendering / 3d / compute, executed asynchronously while
//   // Metal has one queue executed in order (async underneath, resources transitions handled by driver)
//   VkQueue
//
//
//   Ptr<CommandEncoder> mtlCommandBuffer::CreateEncoder(Ptr<RenderPass> pass)
//   {
//   assert( pass );
//   Ptr<CommandEncoder> encoder = nullptr;
//   Ptr<mtlRenderPass> mtlPass = ptr_dynamic_cast<mtlRenderPass, RenderPass>(pass);
//
//   id <MTLRenderCommandEncoder> handle = [buffer renderCommandEncoderWithDescriptor: mtlPass->desc];
//   if (handle != nullptr)
//      {
//      Ptr<mtlCommandEncoder> ptr = new mtlCommandEncoder(handle);
//      encoder = ptr_dynamic_cast<CommandEncoder, mtlCommandEncoder>(ptr);
//      }
//
//   return encoder;
//   }
//
//
//
//
//   class CommandEncoder : public SafeObject
//      {
//      public:
//      virtual void Set(Ptr<Pipeline> pipeline) = 0; 
//      };
//
//
//   class mtlCommandEncoder : public CommandEncoder
//      {
//      id <MTLRenderCommandEncoder> encoder;
//
//      public:
//      mtlCommandEncoder(id <MTLRenderCommandEncoder> handle);
//      void Set(Ptr<Pipeline> pipeline) = 0; 
//      };
//
//   
//   mtlCommandEncoder::mtlCommandEncoder(id <MTLRenderCommandEncoder> handle) :
//      encoder(handle)
//   {
//   }
//
//   void mtlCommandEncoder::Set(Ptr<Pipeline> pipeline)
//   {
//   assert( pipeline );
//   Ptr<mtlPipeline> ptr = ptr_dynamic_cast<mtlPipeline, Pipeline>(pipeline);
//
//   [encoder setRenderPipelineState: ptr->state];
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//   Ptr<Pipeline> Create(const Ptr<InputLayout> inputState,
//                        const Ptr<StaticBlendState> blendState)
//   {
//   Ptr<Pipeline> result = nullptr;
//
//   MTLDevice device;
//   MTLRenderPipelineDescriptor desc;
//
//   // TODO: Fill descriptor
//   [desc rasterizationEnabled: /* TODO*/ ];
//   [desc sampleCount: /* TODO*/ ];
//
//
//   // Input Assembler
//   Ptr<mtlInputLayout> input = ptr_dynamic_cast<mtlInputLayout, InputLayout>(inputState);
//   [desc vertexDescriptor: input->desc];
//
//
//   [desc vertexFunction:      ];
//   [desc fragmentFunction:     ];
//
//
//   // Set Color Attachments State
//   Ptr<mtlStaticBlendState> blend = ptr_dynamic_cast<mtlStaticBlendState, StaticBlendState>(blendState);
//   [desc alphaToCoverageEnabled: blend->alphaToCoverage];
//   [desc alphaToOneEnabled: NO];  // Set to Yes if: alpha to coverage == true && disableBlending
//   if (blend->blendInfo[i])
//   for(uint8 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
//      {
//      [desc.colorAttachments setObject: blend->blendInfo[i] atIndexedSubscript:i];
//
//      // Match Blend State attachments Pixel Format
//      [desc.colorAttachments[i] pixelFormat: /* TODO SET PIXEL FORMAT*/ ];
//      //MTLRenderPipelineColorAttachmentDescriptor* attachment = desc.colorAttachments[i];
//      }
//
//   // Set Depth & Stencil Attachments State
//   [desc depthAttachmentPixelFormat: /* MTLPixelFormat */   TranslateTextureFormat[  ] ];
//   [desc stencilAttachmentPixelFormat: /* MTLPixelFormat */ TranslateTextureFormat[  ] ];
//
//   // Create pipeline state object
//   NSError* ret;
//   id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:desc error:&ret];
//   if (ret == nil)
//      {
//      Ptr<mtlPipeline> pso = new mtlPipeline();
//      pso->id = pipeline;
//      result = ptr_dynamic_cast<Pipeline, mtlPipeline>(pipeline);
//      }
//   
//   return result;
//   }
//
//
//   // Context creation :
//   // 
//   // Metal -> Es 3.0 -> ES 2.0
//   //
//
//
//   #import <Metal/Metal.h>
//   #import <QuartzCore/CAMetalLayer.h>
//
//   #import <OpenGLES/ES3/gl.h>
//   #import <OpenGLES/ES3/glext.h>
//
//   EAGLContext* CreateBestEAGLContext()
//   {
//   EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
//   if (context == nil)
//      context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
//
//   return context;
//   }
//
//
//
//   CAMetalLayer *metalLayer;
//   id <CAMetalDrawable> frameDrawable;
//   CADisplayLink *displayLink;
//
//
//@implementation MetalView  // <-- name of your class that inheriths from UIView
//
//+ (id)layerClass  // <--- overload "layerClass" method to return CAMetalLayer instead of standatd CALayer
//{
//    return [CAMetalLayer class];
//}
// 
//@end
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//   struct StaticRasterState
//      {
//
//      ProvokingVertex   provokingVertex;
//      CoordinateOrigin  coordinateOrigin;
//      fillMode;
//      cullMode;
//      frontFace;
//      };
//
//
//   Ptr<Pipeline> Create(const Ptr<StaticBlendState> blendState)
//   {
//   Ptr<Pipeline> result = nullptr;
//
//   VkGraphicsPipelineCreateInfo pipelineInfo;
//   pipelineInfo.sType  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//   pipelineInfo.pNext  = nullptr;
//   pipelineInfo.flags  = 0;      // Pipeline creation flags - VkPipelineCreateFlags: VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT for Debug
//   pipelineInfo.layout = ;      // Interface layout of the pipeline - VkPipelineLayout
//
//
//   // Local copy of all State Descriptors
//   VkPipelineIaStateCreateInfo descInput;
//   VkPipelineRsStateCreateInfo descRaster;
//   VkPipelineCbStateCreateInfo descBlend;
//   VkPipelineCbAttachmentState descBlendAttachment[MaxColorAttachmentsCount];
//   VkPipelineMsStateCreateInfo descMSAA;
//   VkPipelineVpStateCreateInfo descView;
//   VkPipelineDsStateCreateInfo ds;
//   VkPipelineShaderStageCreateInfo vs;
//   VkPipelineShaderStageCreateInfo fs;
//   VkPipelineVertexInputCreateInfo vi;
//
//   // Init to null all descriptors
//   memset(&descInput,  0, sizeof(descInput));
//   memset(&descRaster, 0, sizeof(descRaster));
//   memset(&descBlend,  0, sizeof(descBlend));
//   for(uint8 i=0; i<MaxColorAttachmentsCount; ++i)
//      memset(&descBlendAttachment[i],  0, sizeof(descBlendAttachment[i]));
//   memset(&descMSAA, 0, sizeof(descMSAA));
//   memset(&descView, 0, sizeof(descView));
//
//   // Input Assembly
//   descInput.sType                  = VK_STRUCTURE_TYPE_PIPELINE_IA_STATE_CREATE_INFO;
//   descInput.pNext                  = nullptr;
//   descInput.topology               = TranslateDrawableType[];
//   descInput.disableVertexReuse     = false;                     // optional - disable Post Vertex Transform Cache ?
//   descInput.primitiveRestartEnable = true;
//   descInput.primitiveRestartIndex  = 0xFFFFFFFF;                // optional GL45
//
//   // Raster State
//   descRaster.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RS_STATE_CREATE_INFO;
//   descRaster.pNext                   = nullptr;
//   descRaster.depthClipEnable         = false;
//   descRaster.rasterizerDiscardEnable = false;
//   descRaster.programPointSize        = false;                                 // optional (GL45)
//   descRaster.pointOrigin             = TranslateCoordinateOrigin[];           // optional (GL45)
//   descRaster.provokingVertex         = TranslateProvokingVertex[];            // optional (GL45)
//   descRaster.fillMode                = TranslateFillMode[];                   // optional (GL45)
//   descRaster.cullMode                = TranslateCullingMethod[];
//   descRaster.frontFace               = TranslateNormalCalculationMethod[];
//
//   // Create local copy of Static Blend Descriptor 
//   Ptr<vkStaticBlendState> blend = ptr_dynamic_cast<vkStaticBlendState, StaticBlendState>(blendState);
//   memcpy(&descBlend, &blend->desc, sizeof(VkPipelineCbStateCreateInfo));
//   for(uint8 i=0; i<MaxColorAttachmentsCount; ++i)
//      {
//      memcpy(&descBlendAttachment[i], &blend->blendInfo[i], sizeof(VkPipelineCbAttachmentState));
//      descBlendAttachment[i].format = ; // TODO: Match pixel format of RT !!!!
//      }
//   descBlend.pAttachments = &descBlendAttachment[0];
//
//   // Multisampling state
//   descMSAA.sType               = VK_STRUCTURE_TYPE_PIPELINE_MS_STATE_CREATE_INFO;
//   descMSAA.pNext               = nullptr;
//   descMSAA.samples             = 1;
//   descMSAA.multisampleEnable   = false;    // optional (GL45)
//   descMSAA.sampleShadingEnable = false;    // optional (GL45)
//   descMSAA.minSampleShading    = 0.0f;     // optional (GL45)
//   descMSAA.sampleMask          = 1;
//
//   // Viewport state
//   descView.sType               = VK_STRUCTURE_TYPE_PIPELINE_VP_STATE_CREATE_INFO;
//   descView.pNext               = nullptr;
//   descView.viewportCount       = 1;
//   descView.clipOrigin          = VK_COORDINATE_ORIGIN_LOWER_LEFT;   // optional (GL45) - Can specify Direct3D way: VK_COORDINATE_ORIGIN_UPPER_LEFT 
//   descView.depthMode           = VK_DEPTH_MODE_ZERO_TO_ONE;         // optional (GL45) - Can specify Direct3D way: VK_DEPTH_MODE_NEGATIVE_ONE_TO_ONE
//
//   // Depth-Stencil state
//   descDepth.sType              = VK_STRUCTURE_TYPE_PIPELINE_DS_STATE_CREATE_INFO;
//   descDepth.pNext              = nullptr;
//    VkFormat                                    format;
//   descDepth.depthTestEnable    = true;
//   descDepth.depthWriteEnable   = true;
//    VkCompareOp                                 depthCompareOp;
//   descDepth.depthBoundsEnable  = false;          // optional (depth_bounds_test)
//   descDepth.stencilTestEnable  = false;
//    VkStencilOpState                            front;
//    VkStencilOpState                            back;
//
//
//
//   // Create chain of descriptors
//   pipelineInfo.pNext = (const void *) &vi;
//   vi.pNext = (void *) &descInput;
//   descInput.pNext  = (const void *) &descRaster;
//   descRaster.pNext = (const void *) &descBlend;
//   descBlend.pNext  = (const void *) &ms;
//
//   ms.pNext = (const void *) &vp;
//   vp.pNext = (const void *) &ds;
//   ds.pNext = (const void *) &vs;
//   vs.pNext = (const void *) &fs;
//
//  // TODO: Who is the owner of the chain of descriptors ?
//  //       Will Vulkan driver automatically deallocate them after use?
//
//
//   // Create pipeline state object
//   VkPipeline pipeline;
//   VkResult res = vkCreateGraphicsPipeline( /* DEVICE */, &pipelineInfo, &pipeline );
//   if (!res)
//      {
//      Ptr<vkPipeline> pso = new vkPipeline();
//      pso->id = pipeline;
//      result = ptr_dynamic_cast<Pipeline, vkPipeline>(pso);
//      }
//
//   return result;
//   }
//
//
//
//
//
//
//    
//
//
//
//
//
//
//    pipeline.layout = demo->pipeline_layout;
//
//    vi = demo->vertices.vi;
//
//
//
//
//
//
//
//    memset(&vp, 0, sizeof(vp));
//    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VP_STATE_CREATE_INFO;
//    vp.viewportCount = 1;
//    vp.clipOrigin = VK_COORDINATE_ORIGIN_UPPER_LEFT;
//
//    memset(&ds, 0, sizeof(ds));
//    ds.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DS_STATE_CREATE_INFO;
//    ds.format                = demo->depth.format;
//    ds.depthTestEnable       = VK_TRUE;
//    ds.depthWriteEnable      = VK_TRUE;
//    ds.depthCompareOp        = VK_COMPARE_OP_LESS_EQUAL;
//    ds.depthBoundsEnable     = VK_FALSE;
//    ds.back.stencilFailOp    = VK_STENCIL_OP_KEEP;
//    ds.back.stencilPassOp    = VK_STENCIL_OP_KEEP;
//    ds.back.stencilCompareOp = VK_COMPARE_OP_ALWAYS;
//    ds.stencilTestEnable     = VK_FALSE;
//    ds.front = ds.back;
//
//    memset(&vs, 0, sizeof(vs));
//    vs.sType                       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    vs.shader.stage                = VK_SHADER_STAGE_VERTEX;
//    vs.shader.shader               = demo_prepare_vs(demo);
//    vs.shader.linkConstBufferCount = 0;
//
//    memset(&fs, 0, sizeof(fs));
//    fs.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    fs.shader.stage = VK_SHADER_STAGE_FRAGMENT;
//    fs.shader.shader = demo_prepare_fs(demo);
//
//    memset(&ms, 0, sizeof(ms));
//    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MS_STATE_CREATE_INFO;
//    ms.sampleMask = 1;
//    ms.multisampleEnable = VK_FALSE;
//    ms.samples = 1;
//
//    pipeline.pNext = (const void *) &vi;
//    vi.pNext = (void *) &ia;
//    ia.pNext = (const void *) &rs;
//    rs.pNext = (const void *) &cb;
//    cb.pNext = (const void *) &ms;
//    ms.pNext = (const void *) &vp;
//    vp.pNext = (const void *) &ds;
//    ds.pNext = (const void *) &vs;
//    vs.pNext = (const void *) &fs;
//
//    err = vkCreateGraphicsPipeline(demo->device, &pipeline, &demo->pipeline);
//    assert(!err);
//
//    vkDestroyObject(demo->device, VK_OBJECT_TYPE_SHADER, vs.shader.shader);
//    vkDestroyObject(demo->device, VK_OBJECT_TYPE_SHADER, fs.shader.shader);
//
//
//
//   }
//}
