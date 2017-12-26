/*

 Ngine v5.0
 
 Module      : OpenGL Blend.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

#include "core/rendering/opengl/glBlend.h"
#include "core/rendering/opengl/glDevice.h"

namespace en
{
   namespace gpu
   {
   static const uint16 TranslateBlend[BlendFunctionsCount] =
      {
      GL_ZERO,                     // Zero
      GL_ONE,                      // One
      GL_SRC_COLOR,                // Source
      GL_ONE_MINUS_SRC_COLOR,      // OneMinusSource
      GL_DST_COLOR,                // Destination
      GL_ONE_MINUS_DST_COLOR,      // OneMinusDestination
      GL_SRC_ALPHA,                // SourceAlpha
      GL_ONE_MINUS_SRC_ALPHA,      // OneMinusSourceAlpha
      GL_DST_ALPHA,                // DestinationAlpha
      GL_ONE_MINUS_DST_ALPHA,      // OneMinusDestinationAlpha
      GL_CONSTANT_COLOR,           // ConstantColor
      GL_ONE_MINUS_CONSTANT_COLOR, // OneMinusConstantColor
      GL_CONSTANT_ALPHA,           // ConstantAlpha
      GL_ONE_MINUS_CONSTANT_ALPHA, // OneMinusConstantAlpha
      GL_SRC_ALPHA_SATURATE,       // SourceAlphaSaturate
      GL_SRC1_COLOR,               // SecondSource
      GL_ONE_MINUS_SRC1_COLOR,     // OneMinusSecondSource
      GL_SRC1_ALPHA,               // SecondSourceAlpha
      GL_ONE_MINUS_SRC1_ALPHA,     // OneMinusSecondSourceAlpha
      };

   static const uint16 TranslateBlendFunc[BlendEquationsCount] =
      {
      GL_FUNC_ADD,                 // Add
      GL_FUNC_SUBTRACT,            // Subtract
      GL_FUNC_REVERSE_SUBTRACT,    // DestinationMinusSource
      GL_MIN,                      // Min
      GL_MAX                       // Max
      };

   // Logical Operation is alternative to blending (currently not supported)
   static const uint16 TranslateLogicOperation[LogicOperationsCount] = 
      {
      GL_CLEAR,                    // ClearDestination
      GL_SET,                      // Set             
      GL_COPY,                     // Copy            
      GL_COPY_INVERTED,            // CopyInverted    
      GL_NOOP,                     // NoOperation     
      GL_INVERT,                   // Invert          
      GL_AND,                      // And             
      GL_NAND,                     // NAnd            
      GL_OR,                       // Or              
      GL_NOR,                      // Nor             
      GL_XOR,                      // Xor             
      GL_EQUIV,                    // Equiv           
      GL_AND_REVERSE,              // AndReverse      
      GL_AND_INVERTED,             // AndInverted     
      GL_OR_REVERSE,               // OrReverse       
      GL_OR_INVERTED	           // OrInverted      
      };

#if 0
   // Blend Function 
   struct BlendFunctionSupport
          {
          Nversion source;                // First OpenGL version that supports it as source function
          Nversion destination;           // First OpenGL version that supports it as destination function
          };
   
   // Color Buffer blend functions
   const BlendFunctionSupport BlendFunction[BlendFunctionsCount] = 
      {
      { OpenGL_1_0, OpenGL_1_0 },   // Zero
      { OpenGL_1_0, OpenGL_1_0 },   // One
      { OpenGL_1_4, OpenGL_1_0 },   // Source
      { OpenGL_1_4, OpenGL_1_0 },   // OneMinusSource
      { OpenGL_1_0, OpenGL_1_4 },   // Destination
      { OpenGL_1_0, OpenGL_1_4 },   // OneMinusDestination
      { OpenGL_1_0, OpenGL_1_0 },   // SourceAlpha
      { OpenGL_1_0, OpenGL_1_0 },   // OneMinusSourceAlpha
      { OpenGL_1_0, OpenGL_1_0 },   // DestinationAlpha
      { OpenGL_1_0, OpenGL_1_0 },   // OneMinusDestinationAlpha
      { OpenGL_1_2, OpenGL_1_2 },   // ConstantColor
      { OpenGL_1_2, OpenGL_1_2 },   // OneMinusConst
      { OpenGL_1_2, OpenGL_1_2 },   // ConstantAlpha
      { OpenGL_1_2, OpenGL_1_2 },   // OneMinusConstantAlpha
      { OpenGL_1_0, OpenGL_3_3 },   // SourceAlphaSaturate
      { OpenGL_3_3, OpenGL_3_3 },   // SecondSource
      { OpenGL_3_3, OpenGL_3_3 },   // OneMinusSecondSource
      { OpenGL_3_3, OpenGL_3_3 },   // SecondSourceAlpha
      { OpenGL_3_3, OpenGL_3_3 }    // OneMinusSecondSourceAlpha
      };

   // Color Buffer blend functions
   const BlendFunctionSupport BlendFunction[BlendFunctionsCount] = 
      {
      { OpenGL_ES_1_0,         OpenGL_ES_1_0         },   // Zero
      { OpenGL_ES_1_0,         OpenGL_ES_1_0         },   // One
      { OpenGL_ES_1_1,         OpenGL_ES_1_0         },   // Source
      { OpenGL_ES_1_1,         OpenGL_ES_1_0         },   // OneMinusSource
      { OpenGL_ES_1_0,         OpenGL_ES_1_1         },   // Destination
      { OpenGL_ES_1_0,         OpenGL_ES_1_1         },   // OneMinusDestination
      { OpenGL_ES_1_0,         OpenGL_ES_1_0         },   // SourceAlpha
      { OpenGL_ES_1_0,         OpenGL_ES_1_0         },   // OneMinusSourceAlpha
      { OpenGL_ES_1_0,         OpenGL_ES_1_0         },   // DestinationAlpha
      { OpenGL_ES_1_0,         OpenGL_ES_1_0         },   // OneMinusDestinationAlpha
      { OpenGL_ES_2_0,         OpenGL_ES_2_0         },   // ConstantColor
      { OpenGL_ES_2_0,         OpenGL_ES_2_0         },   // OneMinusConst
      { OpenGL_ES_2_0,         OpenGL_ES_2_0         },   // ConstantAlpha
      { OpenGL_ES_2_0,         OpenGL_ES_2_0         },   // OneMinusConstantAlpha
      { OpenGL_ES_1_1,         OpenGL_ES_3_0         },   // SourceAlphaSaturate
      { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported },   // SecondSource
      { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported },   // OneMinusSecondSource
      { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported },   // SecondSourceAlpha
      { OpenGL_ES_Unsupported, OpenGL_ES_Unsupported }    // OneMinusSecondSourceAlpha
      };
#endif


   BlendStateGL::BlendStateGL(const BlendStateInfo& state,
                              const uint32 _attachments, 
                              const BlendAttachmentInfo* _color) :
      attachments(min(_attachments, static_cast<uint32>(MaxColorAttachmentsCount))),
      color(new BlendAttachmentStateGL[attachments])
   {
   blendColor = state.blendColor;
   for(uint8 i=0; i<attachments; ++i)
      {
      assert( !(_color[0].logicOperation && _color[i].blending) );

      color[i].blending       = _color[i].blending;
      color[i].logicOperation = _color[i].logicOperation;
      color[i].srcRGB         = TranslateBlend[_color[i].srcRGB];
      color[i].dstRGB         = TranslateBlend[_color[i].dstRGB];
      color[i].srcAlpha       = TranslateBlend[_color[i].srcAlpha];
      color[i].dstAlpha       = TranslateBlend[_color[i].dstAlpha];
      color[i].rgbFunc        = TranslateBlendFunc[_color[i].rgbFunc];
      color[i].alphaFunc      = TranslateBlendFunc[_color[i].alphaFunc];
      color[i].logic          = TranslateLogicOperation[_color[i].logic];
      }
   }
 
   BlendStateGL::~BlendStateGL()
   {
   delete [] color;
   }


   void setBlendState(const OpenGLDevice& device, const BlendStateGL& state)
   {
   // Default color buffer
   if (device.cache.blend[0].blending != state.cachedBlend[0].blending)
      {
      if (state.cachedBlend[0].blending)
         Validate( glEnable(GL_BLEND) )
      else
         Validate( glDisable(GL_BLEND) )
      }

   if (bitsChanged(device.cache.blend[0].vector, state.cachedBlend[0].vector, CacheBlendFunctionMask))
      {
      Validate( glBlendFuncSeparate(state.color[0].srcRGB, 
                                   state.color[0].dstRGB, 
                                   state.color[0].srcAlpha,
                                   state.color[0].dstAlpha) )
      }

   if (bitsChanged(device.cache.blend[0].vector, state.cachedBlend[0].vector, CacheBlendEquationMask))
      {
#if defined(EN_DISCRETE_GPU) && !defined(EN_PLATFORM_OSX)
      Validate( glBlendEquationSeparateEXT(state.color[0].rgbFunc,
                                          state.color[0].alphaFunc) )
#else // defined(EN_MOBILE_GPU)
      Validate( glBlendEquationSeparate(state.color[0].rgbFunc,
                                       state.color[0].alphaFunc) )
#endif
      }

   // State matches now
   device.cache.blend[0] = state.cachedBlend[0];


   // Additional color buffers
#ifdef EN_OPENGL_DESKTOP                       // TODO: Verify latest OpenGL ES !
   if (device.support.separateBlendingModes)   // TODO: Add this query !
      for(uint8 i=1; i<device.support.maxColorAttachments; ++i)
         {
         if (device.cache.blend[i].blending != state.cachedBlend[i].blending)
            {
            if (state.cachedBlend[i].blending)
               Validate( glEnablei(GL_BLEND, i) )
            else
               Validate( glDisablei(GL_BLEND, i) )
            }

         if (bitsChanged(device.cache.blend[0].vector, state.cachedBlend[0].vector, CacheBlendFunctionMask))
            {
            if (device.api.better(OpenGL_4_0))   
               {      
               Validate( glBlendFuncSeparatei(i, state.color[i].srcRGB,  
                                                state.color[i].dstRGB,  
                                                state.color[i].srcAlpha,
                                                state.color[i].dstAlpha ) )
               }
            else
            if (device.support.extension(ARB_draw_buffers_blend))  
               {
               Validate( glBlendFuncSeparateiARB(i, state.color[i].srcRGB,  
                                                   state.color[i].dstRGB,  
                                                   state.color[i].srcAlpha,
                                                   state.color[i].dstAlpha ) )
               }
            }

         if (bitsChanged(device.cache.blend[0].vector, state.cachedBlend[0].vector, CacheBlendEquationMask))
            {
            if (device.api.better(OpenGL_4_0))  
               {   
               Validate( glBlendEquationSeparatei(i, state.color[i].rgbFunc, 
                                                    state.color[i].alphaFunc) )
               }
            else
            if (device.support.extension(ARB_draw_buffers_blend)) 
               {
               Validate( glBlendEquationSeparateiARB(i, state.color[i].rgbFunc, 
                                                       state.color[i].alphaFunc) )
               }
            }

         // State matches now
         device.cache.blend[i] = state.cachedBlend[i];
         }
#endif


     // TODO: Blend Mask & Blend Color, & Logic Operation !!!

      if (GpuContext.state.dirtyBits.blendColor)
         {
         Validate( glBlendColor(state.color.r,
                               GpuContext.state.output.blendColor.g,
                               GpuContext.state.output.blendColor.b,
                               GpuContext.state.output.blendColor.a) )
         GpuContext.state.dirtyBits.blendColor = false;
         }
   }

   }
}
#endif
