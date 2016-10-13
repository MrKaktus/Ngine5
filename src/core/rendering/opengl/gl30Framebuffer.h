/*

 Ngine v5.0
 
 Module      : OpenGL Framebuffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_30_FRAMEBUFFER
#define ENG_CORE_RENDERING_OPENGL_30_FRAMEBUFFER

#include "core/rendering/framebuffer.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

namespace en
{
   namespace gpu
   {
   class FramebufferGL30 : public Framebuffer
      {
      public:
      uint32 id;                      // OpenGL handle

      FramebufferGL30();
      virtual ~FramebufferGL30();

                                      // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
      virtual void color(             //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D, TextureCubeMap, TextureCubeMapArray (all layers)
         const DataAccess access,     // Type of action that can be performed on this texture 
         const uint8 index,           // Color attachment
         const Ptr<Texture> texture,  // Texture
         const uint8 mipmap = 0);     // Mipmap
      
                                      // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
                                      //      Texture3D, TextureCubeMap (specific depth, face)   
      virtual void color(             //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, TextureCubeMapArray (all layers)
         const DataAccess access,     // Type of action that can be performed on this texture 
         const uint8 index,           // Color attachment
         const Ptr<Texture> texture,  // Texture
         const uint16 layer,          // Depth of 3D texture or face of CubeMap
         const uint8 mipmap = 0);     // Mipmap

      virtual void depth(             // Use: Texture2D, Texture2DRectangle
         const DataAccess access,     // Type of action that can be performed on this texture 
         const Ptr<Texture> texture); // Depth Texture

      virtual void stencil(           // Use: Texture2D, Texture2DRectangle
         const DataAccess access,     // Type of action that can be performed on this texture 
         const Ptr<Texture> texture); // Stencil Texture

      virtual void depthStencil(      // Use: Texture2D, Texture2DRectangle
         const DataAccess access,     // Type of action that can be performed on this texture 
         const Ptr<Texture> texture); // Depth-Stencil Texture

      virtual void defaultColor(const uint8 index); // Detach texture/renderbuffer from given color attachment
      virtual void defaultDepth(void);              // Detach texture/renderbuffer from depth attachment
      virtual void defaultStencil(void);            // Detach texture/renderbuffer from stencil attachment
      virtual void defaultDepthStencil(void);       // Detach texture/renderbuffer from deth/stencil attachment

      virtual void defaultColor(const DataAccess access, const uint8 index); // Detach texture/renderbuffer from given color attachment
      virtual void defaultDepth(const DataAccess access);              // Detach texture/renderbuffer from depth attachment
      virtual void defaultStencil(const DataAccess access);            // Detach texture/renderbuffer from stencil attachment
      virtual void defaultDepthStencil(const DataAccess access);       // Detach texture/renderbuffer from deth/stencil attachment

      };
   }
}
#endif

#endif
