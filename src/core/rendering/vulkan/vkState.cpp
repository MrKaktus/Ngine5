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
//
//   // Framebuffer creation
//
//
//   shared_ptr<ColorAttachment> Create(const shared_ptr<Texture> color, 
//                               const shared_ptr<Texture> resolveMsaa  = nullptr, 
//                               const uint32 colorMipMap        = 0,
//                               const uint32 msaaMipMap         = 0,
//                               const uint32 colorLayer         = 0,
//                               const uint32 msaaLayer          = 0,
//                               const uint32 layers             = 1);   // On iOS/OSX only one layer is supported
//
//   shared_ptr<ColorAttachment> Create(const shared_ptr<Texture> color, 
//                               const TextureFormat renderFormat,
//                               const shared_ptr<Texture> resolveMsaa = nullptr, 
//                               const uint32 colorMipMap       = 0,
//                               const uint32 msaaMipMap        = 0,
//                               const uint32 colorLayer        = 0,
//                               const uint32 msaaLayer         = 0,
//                               const uint32 layers            = 1);   // On iOS/OSX only one layer is supported 
//
//   shared_ptr<DepthStencilAttachment> Create(const shared_ptr<Texture> depthStencil, 
//                                      const shared_ptr<Texture> resolveMsaa  = nullptr, 
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
//   PFN_vkCreateColorAttachmentView  vkCreateColorAttachmentView  = nullptr;
//   PFN_vkCreateDepthStencilView     vkCreateDepthStencilView     = nullptr;
//   PFN_vkCreateFramebuffer          vkCreateFramebuffer          = nullptr;
//   PFN_vkCreateDynamicViewportState vkCreateDynamicViewportState = nullptr;
//

//
//   // DX12 Doesn't support Tiled Renderers, and as such, don't have load and store operations.
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
//   shared_ptr<RasterState> Create(const RasterStateInfo raster)
//   {
//   shared_ptr<RasterState> result = nullptr;
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
//      shared_ptr<vkRasterState> vkRaster = make_shared<vkRasterState>();
//      vkRaster->id = rasterState;
//      result = vkRaster;
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
//   shared_ptr<ViewportScissorState> Create(const uint32 count, 
//                                    const ViewportStateInfo* viewports,
//                                    const ScissorStateInfo* scissors)
//   {
//   shared_ptr<ViewportScissorState> result = nullptr;
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
//      shared_ptr<vkViewportScissorState> vkViewportScissor = make_shared<vkViewportScissorState>(fbo);
//      vkViewportScissor->id = viewportScissorState;
//      result = vkFbo;
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
//   void vkCommandBuffer::bind(const shared_ptr<RasterState> raster)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_RASTER, 
//                                reinterpret_cast<vkRasterState*>(raster.get())->id );
//   }
//
//   void vkCommandBuffer::bind(const shared_ptr<ViewportScissorState> viewportScissor)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_VIEWPORT, 
//                                reinterpret_cast<vkViewportScissorState*>(viewportScissor.get())->id );
//   }
//
//   void vkCommandBuffer::bind(const shared_ptr<DepthStencilState> depthStencil)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_DEPTH_STENCIL, 
//                                reinterpret_cast<vkDepthStencilState*>(depthStencil.get())->id );
//   }
//
//   void vkCommandBuffer::bind(const shared_ptr<BlendState> blend)
//   {
//   vkCmdBindDynamicStateObject( id, VK_STATE_BIND_COLOR_BLEND, 
//                                reinterpret_cast<vkBlendState*>(blend.get())->id );
//   }
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
//   shared_ptr<StaticBlendState> Create(const BlendInfo& state,
//                                const uint32 attachments, 
//                                const BlendAttachmentInfo* color)
//   {
//   shared_ptr<vkStaticBlendState> vkState = new vkStaticBlendState();
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
//   return vkState;
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
