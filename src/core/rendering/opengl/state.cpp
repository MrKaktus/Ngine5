/*

 Ngine v5.0
 
 Module      : State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/log/log.h"
#include "core/rendering/context.h"
#include "core/rendering/opengl/opengl.h"

#if 0 //!defined(EN_PLATFORM_OSX)

namespace en
{
   namespace gpu
   {
      namespace gl
      {
      bool BlendSourceFunctionSupported[BlendFunctionsCount];
      bool BlendDestinationFunctionSupported[BlendFunctionsCount];

      // Comparison Method 
      const uint16 CompareMethod[CompareMethodsCount] = 
         {
         GL_NEVER   ,   // Never          
         GL_ALWAYS  ,   // Always         
         GL_LESS    ,   // Less           
         GL_LEQUAL  ,   // LessOrEqual    
         GL_EQUAL   ,   // Equal          
         GL_GREATER ,   // Greater        
         GL_GEQUAL  ,   // GreaterOrEqual 
         GL_NOTEQUAL    // NotEqual   
         };    

      // Stencil Buffer modification methods
      const uint16 StencilModification[StencilModificationsCount] =
         {
         GL_KEEP     ,  // Keep
         GL_ZERO     ,  // Clear
         GL_REPLACE  ,  // Reference
         GL_INCR     ,  // Increase
         GL_DECR     ,  // Decrease
         GL_INVERT   ,  // InvertBits
         GL_INCR_WRAP,  // IncreaseWrap
         GL_DECR_WRAP   // DecreaseWrap
         };

      // Color Buffer blend equations
      const uint16 BlendEquation[BlendEquationsCount] =
         {
         GL_FUNC_ADD             ,    // Add                    
         GL_FUNC_SUBTRACT        ,    // Subtract               
         GL_FUNC_REVERSE_SUBTRACT,    // DestinationMinusSource 
         GL_MIN                  ,    // Min                    
         GL_MAX                       // Max                    
         };

      // Surface Face 
      const uint16 Face[FaceChoosesCount] = 
         {
         GL_FRONT         ,   // Front
         GL_BACK          ,   // Back 
         GL_FRONT_AND_BACK    // Both 
         };

      // Normal Calculation Method
      const uint16 NormalCalculationMethod[NormalCalculationMethodsCount] = 
         {
         GL_CW ,   // ClockWise       
         GL_CCW    // CounterClockWise
         };

      #ifdef EN_OPENGL_DESKTOP

      const DrawableTypeTranslation Drawable[DrawableTypesCount] = 
         {
         { OpenGL_1_0, GL_POINTS         },   // Points            
         { OpenGL_1_0, GL_LINES          },   // Lines             
         { OpenGL_1_0, GL_LINE_LOOP      },   // LineLoops         
         { OpenGL_1_0, GL_LINE_STRIP     },   // LineStripes        
         { OpenGL_1_0, GL_TRIANGLES      },   // Triangles         
         { OpenGL_1_0, GL_TRIANGLE_FAN   },   // TriangleFans      
         { OpenGL_1_0, GL_TRIANGLE_STRIP },   // TriangleStripes  
         { OpenGL_4_0, GL_PATCHES        }    // Patches    
         };

      // Color Buffer blend functions
      const BlendFunctionTranslation BlendFunction[BlendFunctionsCount] = 
         {
         { OpenGL_1_0, OpenGL_1_0, GL_ZERO                     },   // Zero
         { OpenGL_1_0, OpenGL_1_0, GL_ONE                      },   // One
         { OpenGL_1_4, OpenGL_1_0, GL_SRC_COLOR                },   // Source
         { OpenGL_1_4, OpenGL_1_0, GL_ONE_MINUS_SRC_COLOR      },   // OneMinusSource
         { OpenGL_1_0, OpenGL_1_4, GL_DST_COLOR                },   // Destination
         { OpenGL_1_0, OpenGL_1_4, GL_ONE_MINUS_DST_COLOR      },   // OneMinusDestination
         { OpenGL_1_0, OpenGL_1_0, GL_SRC_ALPHA                },   // SourceAlpha
         { OpenGL_1_0, OpenGL_1_0, GL_ONE_MINUS_SRC_ALPHA      },   // OneMinusSourceAlpha
         { OpenGL_1_0, OpenGL_1_0, GL_DST_ALPHA                },   // DestinationAlpha
         { OpenGL_1_0, OpenGL_1_0, GL_ONE_MINUS_DST_ALPHA      },   // OneMinusDestinationAlpha
         { OpenGL_1_2, OpenGL_1_2, GL_CONSTANT_COLOR           },   // ConstantColor
         { OpenGL_1_2, OpenGL_1_2, GL_ONE_MINUS_CONSTANT_COLOR },   // OneMinusConst
         { OpenGL_1_2, OpenGL_1_2, GL_CONSTANT_ALPHA           },   // ConstantAlpha
         { OpenGL_1_2, OpenGL_1_2, GL_ONE_MINUS_CONSTANT_ALPHA },   // OneMinusConstantAlpha
         { OpenGL_1_0, OpenGL_3_3, GL_SRC_ALPHA_SATURATE       },   // SourceAlphaSaturate
         { OpenGL_3_3, OpenGL_3_3, GL_SRC1_COLOR               },   // SecondSource
         { OpenGL_3_3, OpenGL_3_3, GL_ONE_MINUS_SRC1_COLOR     },   // OneMinusSecondSource
         { OpenGL_3_3, OpenGL_3_3, GL_SRC1_ALPHA               },   // SecondSourceAlpha
         { OpenGL_3_3, OpenGL_3_3, GL_ONE_MINUS_SRC1_ALPHA     }    // OneMinusSecondSourceAlpha
         };

      #endif
      #ifdef EN_OPENGL_MOBILE

      const DrawableTypeTranslation Drawable[DrawableTypesCount] = 
         {
         { OpenGL_ES_1_0,         GL_POINTS         },   // Points            
         { OpenGL_ES_1_0,         GL_LINES          },   // Lines             
         { OpenGL_ES_1_0,         GL_LINE_LOOP      },   // LineLoops         
         { OpenGL_ES_1_0,         GL_LINE_STRIP     },   // LineStripes        
         { OpenGL_ES_1_0,         GL_TRIANGLES      },   // Triangles         
         { OpenGL_ES_1_0,         GL_TRIANGLE_FAN   },   // TriangleFans      
         { OpenGL_ES_1_0,         GL_TRIANGLE_STRIP },   // TriangleStripes   
         { OpenGL_ES_Unsupported, 0 /*GL_PATCHES*/  }    // Patches   
         };

      // Color Buffer blend functions
      const BlendFunctionTranslation BlendFunction[BlendFunctionsCount] = 
         {
         { OpenGL_ES_1_0,         OpenGL_ES_1_0,         GL_ZERO                       },   // Zero
         { OpenGL_ES_1_0,         OpenGL_ES_1_0,         GL_ONE                        },   // One
         { OpenGL_ES_1_1,         OpenGL_ES_1_0,         GL_SRC_COLOR                  },   // Source
         { OpenGL_ES_1_1,         OpenGL_ES_1_0,         GL_ONE_MINUS_SRC_COLOR        },   // OneMinusSource
         { OpenGL_ES_1_0,         OpenGL_ES_1_1,         GL_DST_COLOR                  },   // Destination
         { OpenGL_ES_1_0,         OpenGL_ES_1_1,         GL_ONE_MINUS_DST_COLOR        },   // OneMinusDestination
         { OpenGL_ES_1_0,         OpenGL_ES_1_0,         GL_SRC_ALPHA                  },   // SourceAlpha
         { OpenGL_ES_1_0,         OpenGL_ES_1_0,         GL_ONE_MINUS_SRC_ALPHA        },   // OneMinusSourceAlpha
         { OpenGL_ES_1_0,         OpenGL_ES_1_0,         GL_DST_ALPHA                  },   // DestinationAlpha
         { OpenGL_ES_1_0,         OpenGL_ES_1_0,         GL_ONE_MINUS_DST_ALPHA        },   // OneMinusDestinationAlpha
         { OpenGL_ES_2_0,         OpenGL_ES_2_0,         GL_CONSTANT_COLOR             },   // ConstantColor
         { OpenGL_ES_2_0,         OpenGL_ES_2_0,         GL_ONE_MINUS_CONSTANT_COLOR   },   // OneMinusConst
         { OpenGL_ES_2_0,         OpenGL_ES_2_0,         GL_CONSTANT_ALPHA             },   // ConstantAlpha
         { OpenGL_ES_2_0,         OpenGL_ES_2_0,         GL_ONE_MINUS_CONSTANT_ALPHA   },   // OneMinusConstantAlpha
         { OpenGL_ES_1_1,         OpenGL_ES_3_0,         GL_SRC_ALPHA_SATURATE         },   // SourceAlphaSaturate
         { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported, 0 /*GL_SRC1_COLOR*/           },   // SecondSource
         { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported, 0 /*GL_ONE_MINUS_SRC1_COLOR*/ },   // OneMinusSecondSource
         { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported, 0 /*GL_SRC1_ALPHA*/           },   // SecondSourceAlpha
         { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported, 0 /*GL_ONE_MINUS_SRC1_ALPHA*/ }    // OneMinusSecondSourceAlpha
         };

      #endif
      #ifdef EN_DEBUG

      #endif

      // Gather information about features supported by this context
      void GatherContextCapabilities(void)
      {
#ifdef EN_OPENGL_DESKTOP
      // Input Assembler
      GpuContext.support.maxInputLayoutAttributesCount = 16;

      // Pipeline State - Misc
      Validate( glGetIntegerv(GL_MAX_DRAW_BUFFERS,        (GLint*)&GpuContext.support.maxRenderTargets) );  // Maximum supported Render Targets count  
      Validate( glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,   (GLint*)&GpuContext.support.maxFramebufferColorAttachments) ); // Maximum supported Render Targets in FBO count
      Validate( glGetIntegerv(GL_MAX_PATCH_VERTICES,      (GLint*)&GpuContext.support.maxPatchSize) );	
      Validate( glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&GpuContext.support.maxTextureUnits) )

      // Buffer
// glGetIntegerv(GL_MAX_ELEMENTS_INDICES,           (GLint*)&m_maxElementIndices);
// glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,          (GLint*)&m_maxElementVerices);

      // Texture
      Validate( glGetIntegerv(GL_MAX_TEXTURE_SIZE,           (GLint*)&GpuContext.support.maxTextureSize) )
    //Validate( glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, (GLint*)&GpuContext.support.maxTextureRectSize) )
      Validate( glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,  (GLint*)&GpuContext.support.maxTextureCubeSize) )
      Validate( glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,        (GLint*)&GpuContext.support.maxTexture3DSize) )
      Validate( glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,   (GLint*)&GpuContext.support.maxTextureLayers) )
      Validate( glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,    (GLint*)&GpuContext.support.maxTextureBufferSize) )
      Validate( glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS,         (GLfloat*)&GpuContext.support.maxTextureLodBias) )
#endif

      // Maximum anisotropic filtering samples count
      GpuContext.support.maxAnisotropy = 0.0f;
      if (GpuContext.support.extension(EXT_texture_filter_anisotropic))
         Validate( glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat*)&GpuContext.support.maxAnisotropy) )


      // TODO: Finish !

// // Sets device implementation dependent variables
// glGetIntegerv(GL_SUBPIXEL_BITS,                  (GLint*)&m_subpixelBits);       


// glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,          (GLint*)&m_maxRenderbufferSize);
// glGetIntegerv(GL_MAX_VIEWPORT_DIMS,              (GLint*)&temp[0]);
//                                                           m_maxViewportWidth  = temp[0];
//                                                           m_maxViewportHeight = temp[1];
// glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,       (GLfloat*)&tempf[0]);
//                                                           m_aliasedLineWidthMin = tempf[0];
//                                                           m_aliasedLineWidthMax = tempf[1];
// glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, (GLint*)&m_compressedTextureFormats);
//
//
// // TODO: This is quick WA for missing declarations in MacOS.
// //       It is suspected that they are not supported in 
// //       OpenGL ES.
//#ifdef EN_PLATFORM_WINDOWS
// glGetIntegerv(GL_MAX_CLIP_DISTANCES,             (GLint*)&m_maxClipDistances);    
  


// glGetFloatv(GL_POINT_SIZE_RANGE,               (GLfloat*)&tempf[0]);
//                                                           m_pointSizeMin = tempf[0];
//                                                           m_pointSizeMax = tempf[1];
// glGetFloatv(GL_POINT_SIZE_GRANULARITY,         (GLfloat*)&m_pointSizeGranularity);
// glGetFloatv(GL_LINE_WIDTH_RANGE,               (GLfloat*)&tempf[0]); 
//                                                           m_antialiasedLineWidthMin = tempf[0];
//                                                           m_antialiasedLineWidthMax = tempf[1];
// glGetFloatv(GL_LINE_WIDTH_GRANULARITY,         (GLfloat*)&m_antialiasedLineGranularity);

//#endif
      }

      void StateInit(void)
      {
      // By default nothing is supported
      memset(&BlendSourceFunctionSupported[0], 0, sizeof(BlendSourceFunctionSupported));
      memset(&BlendDestinationFunctionSupported[0], 0, sizeof(BlendSourceFunctionSupported));

      // Init information about currently supported blending functions
      for(uint16 i=0; i<BlendFunctionsCount; ++i)
         {
         // Source blend functions
         if (GpuContext.screen.api.release >= gl::BlendFunction[i].source.release)
            BlendSourceFunctionSupported[i] = true;
         
         // Destination blend functions
         if (GpuContext.screen.api.release >= gl::BlendFunction[i].destination.release)
            BlendDestinationFunctionSupported[i] = true;
         }
      }

      void BlendUpdate(void)
      {
      // Default color buffer 
      if (checkBit(GpuContext.state.dirtyBits.blend, 0))
         {
         if (GpuContext.state.output.color[0].blend.on)
            Validate( glEnable(GL_BLEND) )
         else
            Validate( glDisable(GL_BLEND) )
         clearBit(GpuContext.state.dirtyBits.blend, 0);
         }

      // Additional color buffers
#ifdef EN_OPENGL_DESKTOP
      for(uint8 i=1; i<GpuContext.support.maxRenderTargets; ++i)
         if (checkBit(GpuContext.state.dirtyBits.blend, i))
            {
            if (GpuContext.state.output.color[i].blend.on)
               Validate( glEnablei(GL_BLEND, i) )
            else
               Validate( glDisablei(GL_BLEND, i) )
            }  
#endif

      GpuContext.state.dirtyBits.blend = 0;
      }

      void BlendFuncUpdate(void)
      {
      // Default color buffer 
      if (checkBit(GpuContext.state.dirtyBits.blendFunction, 0))
         {
         Validate( glBlendFuncSeparate(GpuContext.state.output.color[0].blend.srcFuncRGB, 
                                      GpuContext.state.output.color[0].blend.dstFuncRGB, 
                                      GpuContext.state.output.color[0].blend.srcFuncA,
                                      GpuContext.state.output.color[0].blend.dstFuncA) )
         clearBit(GpuContext.state.dirtyBits.blendFunction, 0);
         }

      // Additional color buffers
#ifdef EN_OPENGL_DESKTOP
      for(uint8 i=1; i<GpuContext.support.maxRenderTargets; ++i)
         if (checkBit(GpuContext.state.dirtyBits.blendFunction, i))
            {
            if (GpuContext.screen.support(OpenGL_4_0))   
               {      
               Validate( glBlendFuncSeparatei(i, GpuContext.state.output.color[i].blend.srcFuncRGB, 
                                                GpuContext.state.output.color[i].blend.dstFuncRGB, 
                                                GpuContext.state.output.color[i].blend.srcFuncA,
                                                GpuContext.state.output.color[i].blend.dstFuncA) )
               }
            else
            if (GpuContext.support.extension(ARB_draw_buffers_blend))  
               {
               Validate( glBlendFuncSeparateiARB(i, GpuContext.state.output.color[i].blend.srcFuncRGB, 
                                                   GpuContext.state.output.color[i].blend.dstFuncRGB, 
                                                   GpuContext.state.output.color[i].blend.srcFuncA,
                                                   GpuContext.state.output.color[i].blend.dstFuncA) )
               }
            }
#endif

      GpuContext.state.dirtyBits.blendFunction = 0;
      }

      void BlendEquationUpdate(void)
      {
      // Default color buffer 
      if (checkBit(GpuContext.state.dirtyBits.blendEquation, 0))
         {
#if defined(EN_DISCRETE_GPU) && !defined(EN_PLATFORM_OSX)
         Validate( glBlendEquationSeparateEXT(GpuContext.state.output.color[0].blend.equationRGB,
                                             GpuContext.state.output.color[0].blend.equationA) )
#else // defined(EN_MOBILE_GPU)
         Validate( glBlendEquationSeparate(GpuContext.state.output.color[0].blend.equationRGB, 
                                          GpuContext.state.output.color[0].blend.equationA) )
#endif
         clearBit(GpuContext.state.dirtyBits.blendEquation, 0);
         }

      // Additional color buffers
#ifdef EN_OPENGL_DESKTOP
      for(uint8 i=1; i<GpuContext.support.maxRenderTargets; ++i)
         if (checkBit(GpuContext.state.dirtyBits.blendEquation, i))
            {
            if (GpuContext.screen.support(OpenGL_4_0))  
               {   
               Validate( glBlendEquationSeparatei(i, GpuContext.state.output.color[i].blend.equationRGB, 
                                                    GpuContext.state.output.color[i].blend.equationA) )
               }
            else
            if (GpuContext.support.extension(ARB_draw_buffers_blend)) 
               {
               Validate( glBlendEquationSeparateiARB(i, GpuContext.state.output.color[i].blend.equationRGB, 
                                                       GpuContext.state.output.color[i].blend.equationA) )
               }
            }
#endif

      GpuContext.state.dirtyBits.blendEquation = 0;
      }

      void BlendColorUpdate(void)
      {
      if (GpuContext.state.dirtyBits.blendColor)
         {
         Validate( glBlendColor(GpuContext.state.output.blendColor.r,
                               GpuContext.state.output.blendColor.g,
                               GpuContext.state.output.blendColor.b,
                               GpuContext.state.output.blendColor.a) )
         GpuContext.state.dirtyBits.blendColor = false;
         }
      }

      void CullingUpdate(void)
      {
      if (GpuContext.state.culling.enabled)
         Validate( glEnable(GL_CULL_FACE) )
      else
         Validate( glDisable(GL_CULL_FACE) )
      
      GpuContext.state.dirtyBits.culling = false;
      }

      void CullingFunctionUpdate(void)
      {
      Validate( glFrontFace(GpuContext.state.culling.function) ) 
      GpuContext.state.dirtyBits.cullingFunction = false;
      }

      void CullingFaceUpdate(void)
      {
      Validate( glCullFace(GpuContext.state.culling.face) ) 
      GpuContext.state.dirtyBits.cullingFace = false;
      }

      void ScissorRectUpdate(void)
      {
      Validate( glScissor(GpuContext.state.scissor.left, 
                         GpuContext.state.scissor.bottom, 
                         GpuContext.state.scissor.width, 
                         GpuContext.state.scissor.height) )
      GpuContext.state.dirtyBits.scissorRect = false;
      }

      void ScissorUpdate(void)
      {
      if (GpuContext.state.scissor.enabled)
         Validate( glEnable(GL_SCISSOR_TEST) )
      else
         Validate( glDisable(GL_SCISSOR_TEST) )
      
      GpuContext.state.dirtyBits.scissor = false;
      }

      void DepthWritingUpdate(void)
      {
      Validate( glDepthMask(GpuContext.state.depth.writing) )
      GpuContext.state.dirtyBits.depthWriting = false;
      }

      void DepthTestingUpdate(void)
      {
      if (GpuContext.state.depth.testing)
         Validate( glEnable(GL_DEPTH_TEST) )
      else
         Validate( glDisable(GL_DEPTH_TEST) )
      
      GpuContext.state.dirtyBits.depthTesting = false;
      }

      void DepthFunctionUpdate(void)
      {
      Validate( glDepthFunc(GpuContext.state.depth.function) )
      GpuContext.state.dirtyBits.depthFunction = false;
      } 

      void StencilWritingUpdate(void)
      {
      if (GpuContext.state.stencil.writing)
         Validate( glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xFFFFFFFF) )
      else
         Validate( glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x00000000) )

      GpuContext.state.dirtyBits.stencilWriting = false;
      }

      void StencilTestingUpdate(void)
      {
      if (GpuContext.state.stencil.testing)
         Validate( glEnable(GL_STENCIL_TEST) )
      else
         Validate( glDisable(GL_STENCIL_TEST) )
      
      GpuContext.state.dirtyBits.stencilTesting = false;
      }
  
      void StencilOpsUpdate(void)
      {
      if (GpuContext.state.dirtyBits.stencilFrontOps)
         {
         Validate( glStencilOpSeparate(GL_FRONT, 
                     GpuContext.state.stencil.front.operation[0],
                     GpuContext.state.stencil.front.operation[1],
                     GpuContext.state.stencil.front.operation[2]) )

         GpuContext.state.dirtyBits.stencilFrontOps = false;
         }

      if (GpuContext.state.dirtyBits.stencilBackOps)
         {
         Validate( glStencilOpSeparate(GL_BACK, 
                     GpuContext.state.stencil.back.operation[0],
                     GpuContext.state.stencil.back.operation[1],
                     GpuContext.state.stencil.back.operation[2]) )

         GpuContext.state.dirtyBits.stencilBackOps = false;
         }
      }

      void StencilFuncUpdate(void)
      {
      if (GpuContext.state.dirtyBits.stencilFrontFunc)
         {
         Validate( glStencilFuncSeparate(GL_FRONT, 
                     GpuContext.state.stencil.front.function, 
                     GpuContext.state.stencil.front.reference, 
                     GpuContext.state.stencil.front.mask) )

         GpuContext.state.dirtyBits.stencilFrontFunc = false;
         }

      if (GpuContext.state.dirtyBits.stencilBackFunc)
         {
         Validate( glStencilFuncSeparate(GL_BACK, 
                     GpuContext.state.stencil.back.function, 
                     GpuContext.state.stencil.back.reference, 
                     GpuContext.state.stencil.back.mask) )

         GpuContext.state.dirtyBits.stencilBackFunc = false;
         }
      }
  
      void ColorWritingUpdate(void)
      {   
      Validate( glColorMask(GpuContext.state.color.enabled[0],
                           GpuContext.state.color.enabled[1],
                           GpuContext.state.color.enabled[2],
                           GpuContext.state.color.enabled[3]) )
      GpuContext.state.dirtyBits.color = false;
      }   
      
      void ClearUpdate(void)
      {
      uint32 mask = 0;
      
      // Set new color clear value only when finally
      // need to clear color buffer
      if (GpuContext.state.dirtyBits.colorClearValue &&
          GpuContext.state.dirtyBits.colorClear)
         {
         Validate( glClearColor(GpuContext.state.color.clear.r,
                               GpuContext.state.color.clear.g,
                               GpuContext.state.color.clear.b,
                               GpuContext.state.color.clear.a) )
         GpuContext.state.dirtyBits.colorClearValue = false;
         }
      
      // Set new depth clear value only when finally 
      // need to clear depth buffer
      if (GpuContext.state.dirtyBits.depthClearValue &&
          GpuContext.state.dirtyBits.depthClear)
         {
         #ifdef EN_OPENGL_DESKTOP
         Validate( glClearDepth(GpuContext.state.depth.clear) )
         #endif
         #ifdef EN_OPENGL_MOBILE
         Validate( glClearDepthf((float)GpuContext.state.depth.clear) )
         #endif

         GpuContext.state.dirtyBits.depthClearValue = false;
         }

      // Set new depth clear value only when finally 
      // need to clear depth buffer
      if (GpuContext.state.dirtyBits.stencilClearValue &&
          GpuContext.state.dirtyBits.stencilClear)
         {
         Validate( glClearStencil(GpuContext.state.stencil.clear) )
         GpuContext.state.dirtyBits.stencilClearValue = false;
         }
               
      // Decide which buffers are waiting for clearing
      if (GpuContext.state.dirtyBits.colorClear)
         {
         mask |= GL_COLOR_BUFFER_BIT;
         GpuContext.state.dirtyBits.colorClear = false;
         }
      if (GpuContext.state.dirtyBits.depthClear)
         {
         mask |= GL_DEPTH_BUFFER_BIT;
         GpuContext.state.dirtyBits.depthClear = false;
         }
      if (GpuContext.state.dirtyBits.stencilClear)
         {
         mask |= GL_STENCIL_BUFFER_BIT;
         GpuContext.state.dirtyBits.stencilClear = false;
         }
      
      // Clear buffers 
      if (mask)
         Validate( glClear(mask) )
      }
  
      }
   }
}

#endif
