/*

 Ngine v5.0
 
 Module      : State interface.
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
#include "utilities/utilities.h"
#include "utilities/gpcpu/gpcpu.h" // for clamp(...)

#include "core/rendering/device.h"

#include "core/rendering/opengl/glState.h" 
#include "core/rendering/opengl/gl33Sampler.h" 
#include "core/rendering/opengl/gl43Texture.h"
#include "core/rendering/opengl/gl45Framebuffer.h"

#if !defined(EN_PLATFORM_OSX)

namespace en
{
   namespace gpu
   {
   Ptr<Sampler> Interface::ISampler::create(const SamplerState& state)
   {
   // API independent debug validation
   assert( TextureWrapingSupported[state.coordU] );
   assert( TextureWrapingSupported[state.coordV] );
   assert( TextureWrapingSupported[state.coordW] );

   // Calculate sampler state hash
   SamplerHash hash;
   hash.minification  = underlyingType(state.minification);
   hash.magnification = underlyingType(state.magnification);
   hash.mipmap        = underlyingType(state.mipmap);
   hash.anisotropy    = static_cast<uint32>(state.anisotropy);
   hash.coordU        = state.coordU;
   hash.coordV        = state.coordV;
   hash.coordW        = state.coordW;
   hash.borderColor   = state.borderColor;
   hash.depthCompare  = underlyingType(state.compare);
   hash.bias          = state.LodBias;
   hash.minLOD        = state.minLod;
   hash.maxLOD        = state.maxLod;

   // Check if sampler is not already in the cache
   for(uint32 i=0; i<GpuContext.sampler.cacheSize; ++i)
      if (memcmp(&GpuContext.sampler.cache[i].hash, &hash, sizeof(SamplerHash)) == 0)
         return GpuContext.sampler.cache[i].sampler;

   // Create new sampler
   Ptr<gpu::Sampler> sampler = nullptr;
#ifdef EN_OPENGL_DESKTOP
   if ( GpuContext.screen.support(OpenGL_3_3) )
      {
      //SamplerGL33* ptr = allocate<SamplerGL33>();
      //sampler = ptr_dynamic_cast<gpu::Sampler, SamplerGL33>(new(ptr) SamplerGL33(state));            // <<<<< - This is circular dependience now, need to move "Create" to separate file
      sampler = ptr_dynamic_cast<gpu::Sampler, SamplerGL33>(new SamplerGL33(state));
      }
#endif

   // Add sampler to chache
   assert( sampler );
   for(uint32 i=0; i<GpuContext.sampler.cacheSize; ++i)
      if (!GpuContext.sampler.cache[i].sampler)
         {
         GpuContext.sampler.cache[i].sampler = sampler;
         GpuContext.sampler.cache[i].hash    = hash;
         return sampler;
         }

   // Try to free unused sampler from cache and replace it
   for(uint32 i=0; i<GpuContext.sampler.cacheSize; ++i)
      if (GpuContext.sampler.cache[i].sampler.references() == 1)
         {
         GpuContext.sampler.cache[i].sampler = sampler;
         GpuContext.sampler.cache[i].hash    = hash;
         return sampler;
         }

   // Return sampler interface
   Log << "WARNING: Samplers cache overflow!\n";
   return sampler;
   }


   Ptr<Texture> Interface::ITexture::create(const TextureState& state)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   // Check if texture type and format are supported
   assert( TextureTypeSupported[underlyingType(state.type)] );
   assert( TextureCapabilities[underlyingType(state.format)].supported );

   // Check if given texture type supports compressed storage
   bool compressed = TextureCompressionInfo[underlyingType(state.format)].compressed;
   if ( compressed &&
        ( //(state.type == TextureType::Texture2DRectangle)        ||
          (state.type == TextureType::Texture2DMultisample)      ||
          (state.type == TextureType::Texture2DMultisampleArray) //||
          /*(state.type == TextureType::TextureBuffer)*/ ) )
      return nullptr;

   // Check if format is color, depth or stencil renderable
   if ( ( (state.type == TextureType::Texture2DMultisample)      ||
          (state.type == TextureType::Texture2DMultisampleArray) ) &&
        !TextureCapabilities[underlyingType(state.format)].rendertarget )
      return nullptr;


   // HW dependent validation

   // Check if texture dimmensions are correct 
   if ( (state.type == TextureType::Texture1D) ||
        (state.type == TextureType::Texture2DArray) )
      assert( state.width <= GpuContext.support.maxTextureSize );
   else
   if ( (state.type == TextureType::Texture2D)             ||
        (state.type == TextureType::Texture2DArray)        ||
        (state.type == TextureType::Texture2DMultisample ) ||
        (state.type == TextureType::Texture2DMultisampleArray ) )
      {
      assert( state.width  <= GpuContext.support.maxTextureSize );
      assert( state.height <= GpuContext.support.maxTextureSize );
      }
   else
   //if (state.type == TextureType::Texture2DRectangle)
   //   {
   //   assert( state.width  <= GpuContext.support.maxTextureRectSize );
   //   assert( state.height <= GpuContext.support.maxTextureRectSize );
   //   }
   //else
   if (state.type == TextureType::Texture3D)
      {
      assert( state.width  <= GpuContext.support.maxTexture3DSize );
      assert( state.height <= GpuContext.support.maxTexture3DSize );
      assert( state.depth  <= GpuContext.support.maxTexture3DSize );
      }
   else
   if ( (state.type == TextureType::TextureCubeMap)      ||
        (state.type == TextureType::TextureCubeMapArray) )
      {
      assert( state.width == state.height );
      assert( state.width <= GpuContext.support.maxTextureCubeSize );
      }
   //else
   //if (state.type == TextureType::TextureBuffer)
   //   {
   //   assert( state.width <= GpuContext.support.maxTextureBufferSize );
   //   }

   // Check if layers count is set propery
   if ( (state.type == TextureType::Texture1DArray) ||
        (state.type == TextureType::Texture2DArray) ||
        (state.type == TextureType::Texture2DMultisampleArray ) )
      assert( state.layers <= GpuContext.support.maxTextureLayers );
   else
   if (state.type == TextureType::TextureCubeMapArray)
      {
      assert( state.layers % 6 == 0 );
      assert( state.layers <= GpuContext.support.maxTextureLayers );
      }
#endif

   Ptr<gpu::Texture> texture = nullptr;

#ifdef EN_OPENGL_DESKTOP
   if ( GpuContext.screen.support(OpenGL_4_3) )
      texture = ptr_dynamic_cast<gpu::Texture, TextureGL43>(new TextureGL43(state));
   else
      texture = ptr_dynamic_cast<gpu::Texture, TextureGL>(new TextureGL(state));
#elif  EN_OPENGL_MOBILE
   texture = ptr_dynamic_cast<gpu::Texture, TextureGL>(new TextureGL(state));    // TextureGL implementation should be universal for both GL and ES
#endif

   // Return texture interface
   return texture;
   }

   uint16 Interface::ITexture::bitsPerTexel(const Format format)
   { 
   return TextureCompressionInfo[underlyingType(format)].size * 8;
   }

   bool Interface::Support::Texture::type(const en::gpu::TextureType type)
   {
   return TextureTypeSupported[underlyingType(type)];
   }

   bool Interface::Support::Texture::format(const en::gpu::Format format)
   {
   return TextureCapabilities[underlyingType(format)].supported;
   }




   void Interface::Culling::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glEnable(GL_CULL_FACE) )
#else
   GpuContext.state.culling.enabled = true;
   
   // Add to deffered list
   if (!GpuContext.state.dirtyBits.culling)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::CullingUpdate;
      GpuContext.state.dirtyBits.culling = true;
      }
#endif
   }

   void Interface::Culling::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDisable(GL_CULL_FACE) )
#else
   GpuContext.state.culling.enabled = false;
   
   // Add to deffered list
   if (!GpuContext.state.dirtyBits.culling)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::CullingUpdate;
      GpuContext.state.dirtyBits.culling = true;
      }
#endif
   }

   void Interface::Culling::front(const NormalCalculationMethod function)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glFrontFace(gl::NormalCalculationMethod[function]) ) 
#else
   GpuContext.state.culling.function = gl::NormalCalculationMethod[function];
 
   // Add to deffered list  
   if (!GpuContext.state.dirtyBits.cullingFunction)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::CullingFunctionUpdate;
      GpuContext.state.dirtyBits.cullingFunction = true;
      }
#endif
   }

   void Interface::Culling::face(const Face face)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glCullFace(gl::Face[face]) ) 
#else
   GpuContext.state.culling.face = gl::Face[face];

   // Add to deffered list     
   if (!GpuContext.state.dirtyBits.cullingFace)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::CullingFaceUpdate;
      GpuContext.state.dirtyBits.cullingFace = true;
      }
#endif
   }

   void Interface::Scissor::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glEnable(GL_SCISSOR_TEST) )
#else
   GpuContext.state.scissor.enabled = true;

   // Add to deffered list     
   if (!GpuContext.state.dirtyBits.scissor)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ScissorUpdate;
      GpuContext.state.dirtyBits.scissor = true;
      }
#endif
   }

   void Interface::Scissor::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDisable(GL_SCISSOR_TEST) )
#else
   GpuContext.state.scissor.enabled = false;

   // Add to deffered list     
   if (!GpuContext.state.dirtyBits.scissor)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ScissorUpdate;
      GpuContext.state.dirtyBits.scissor = true;
      }
#endif
   }

   void Interface::Scissor::rect(const uint32 left, const uint32 bottom, const uint32 width, const uint32 height)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glScissor(left, bottom, width, height) )
#else
   GpuContext.state.scissor.left    = left;
   GpuContext.state.scissor.bottom  = bottom;
   GpuContext.state.scissor.width   = width;
   GpuContext.state.scissor.height  = height;
  
   // Add to deffered list     
   if (!GpuContext.state.dirtyBits.scissorRect)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ScissorRectUpdate;  
      GpuContext.state.dirtyBits.scissorRect = true;
      }
#endif
   }

   void Interface::Depth::Buffer::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDepthMask(true) )
#else
   GpuContext.state.depth.writing = true;

   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.depthWriting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthWritingUpdate;
      GpuContext.state.dirtyBits.depthWriting = true;
      }
#endif
   }

   void Interface::Depth::Buffer::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDepthMask(false) )
#else
   GpuContext.state.depth.writing = false;

   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.depthWriting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthWritingUpdate;
      GpuContext.state.dirtyBits.depthWriting = true;
      }
#endif
   }

   void Interface::Depth::Buffer::clearValue(const double depth)
   {
#ifdef GPU_IMMEDIATE_MODE
   #ifdef EN_OPENGL_DESKTOP
   Profile( glClearDepth(clamp(depth, 0.0, 1.0)) )
   #endif
   #ifdef EN_OPENGL_MOBILE
   Profile( glClearDepthf((float)clamp(depth, 0.0, 1.0)) )
   #endif
#else
   GpuContext.state.depth.clear = clamp(depth, 0.0, 1.0);
   GpuContext.state.dirtyBits.depthClearValue = true;
#endif
   }
   
   void Interface::Depth::Buffer::clearValue(const float depth)
   {
#ifdef GPU_IMMEDIATE_MODE
   #ifdef EN_OPENGL_DESKTOP
   Profile( glClearDepth((double)clamp(depth, 0.0f, 1.0f)) )
   #endif
   #ifdef EN_OPENGL_MOBILE
   Profile( glClearDepthf(clamp(depth, 0.0f, 1.0f)) )
   #endif
#else
   GpuContext.state.depth.clear = (double)clamp(depth, 0.0f, 1.0f);
   GpuContext.state.dirtyBits.depthClearValue = true;
#endif
   }

   void Interface::Depth::Buffer::clear(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glClear(GL_DEPTH_BUFFER_BIT) )
#else
   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.depthClear)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ClearUpdate;
      GpuContext.state.dirtyBits.depthClear = true;
      }
#endif
   }

   void Interface::Depth::Test::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glEnable(GL_DEPTH_TEST) )
#else
   GpuContext.state.depth.testing = true;

   // Add to deffered list    
   if (!GpuContext.state.dirtyBits.depthTesting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthTestingUpdate;
      GpuContext.state.dirtyBits.depthTesting = true;
      }
#endif
   }

   void Interface::Depth::Test::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDisable(GL_DEPTH_TEST) )
#else
   GpuContext.state.depth.testing = false;

   // Add to deffered list    
   if (!GpuContext.state.dirtyBits.depthTesting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthTestingUpdate;
      GpuContext.state.dirtyBits.depthTesting = true;
      }
#endif
   }

   void Interface::Depth::Test::function(const CompareMethod function)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDepthFunc(en::gpu::TranslateCompareMethod[function]) )
#else
   GpuContext.state.depth.function = gl::TranslateCompareMethod[function];
  
   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.depthFunction)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthFunctionUpdate;
      GpuContext.state.dirtyBits.depthFunction = true;
      }
#endif
   }

   void Interface::Depth::Test::operator() (const CompareMethod function)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDepthFunc(TranslateCompareMethod[function]) )
   Profile( glEnable(GL_DEPTH_TEST) )
#else
   GpuContext.state.depth.function = gl::TranslateCompareMethod[function];
   GpuContext.state.depth.testing  = true;  

   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.depthFunction)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthFunctionUpdate;
      GpuContext.state.dirtyBits.depthFunction = true;
      }  
   if (!GpuContext.state.dirtyBits.depthTesting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::DepthTestingUpdate;
      GpuContext.state.dirtyBits.depthTesting = true;
      }
#endif
   }

   void Interface::Stencil::Buffer::clearValue(const uint8 stencil)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glClearStencil(stencil) )
#else
   GpuContext.state.stencil.clear = stencil;
   GpuContext.state.dirtyBits.stencilClearValue = true;
#endif
   }

   void Interface::Stencil::Buffer::clear(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glClear(GL_STENCIL_BUFFER_BIT) )
#else
   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.stencilClear)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ClearUpdate;
      GpuContext.state.dirtyBits.stencilClear = true;
      }
#endif
   }

   void Interface::Stencil::Buffer::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xFFFFFFFF) )
#else
   GpuContext.state.stencil.writing = true;

   // Add to deffered list   
   if (!GpuContext.state.dirtyBits.stencilWriting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilWritingUpdate;
      GpuContext.state.dirtyBits.stencilWriting = true;
      }
#endif
   }

   void Interface::Stencil::Buffer::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x00000000) )
#else
   GpuContext.state.stencil.writing = false;

   // Add to deffered list   
   if (!GpuContext.state.dirtyBits.stencilWriting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilWritingUpdate;
      GpuContext.state.dirtyBits.stencilWriting = true;
      }
#endif
   }

   void Interface::Stencil::Test::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glEnable(GL_STENCIL_TEST) )
#else
   GpuContext.state.stencil.testing = true;

   // Add to deffered list   
   if (!GpuContext.state.dirtyBits.stencilTesting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilTestingUpdate;
      GpuContext.state.dirtyBits.stencilTesting = true;
      }
#endif
   }

   void Interface::Stencil::Test::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDisable(GL_STENCIL_TEST) )
#else
   GpuContext.state.stencil.testing = false;

   // Add to deffered list   
   if (!GpuContext.state.dirtyBits.stencilTesting)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilTestingUpdate;
      GpuContext.state.dirtyBits.stencilTesting = true;
      }
#endif
   }

   void Interface::Stencil::Test::operation(const StencilModification whenStencilTestFailed,
                                            const StencilModification whenDepthTestFailed,
                                            const StencilModification whenDepthTestPassed)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glStencilOpSeparate(GL_FRONT, 
                                gl::StencilModification[whenStencilTestFailed],
                                gl::StencilModification[whenDepthTestFailed],
                                gl::StencilModification[whenDepthTestPassed]) )
   Profile( glStencilOpSeparate(GL_BACK, 
                                gl::StencilModification[whenStencilTestFailed],
                                gl::StencilModification[whenDepthTestFailed],
                                gl::StencilModification[whenDepthTestPassed]) )
#else
   GpuContext.state.stencil.front.operation[0] = gl::StencilModification[whenStencilTestFailed];
   GpuContext.state.stencil.front.operation[1] = gl::StencilModification[whenDepthTestFailed];
   GpuContext.state.stencil.front.operation[2] = gl::StencilModification[whenDepthTestPassed];
   GpuContext.state.stencil.back.operation[0]  = gl::StencilModification[whenStencilTestFailed];
   GpuContext.state.stencil.back.operation[1]  = gl::StencilModification[whenDepthTestFailed];
   GpuContext.state.stencil.back.operation[2]  = gl::StencilModification[whenDepthTestPassed];

   // Add to deffered list   
   if ( !GpuContext.state.dirtyBits.stencilFrontOps ||
        !GpuContext.state.dirtyBits.stencilBackOps )
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilOpsUpdate;

      if (!GpuContext.state.dirtyBits.stencilFrontOps)
         GpuContext.state.dirtyBits.stencilFrontOps = true;
      if (!GpuContext.state.dirtyBits.stencilBackOps)
         GpuContext.state.dirtyBits.stencilBackOps = true;
      }
#endif
   }

   void Interface::Stencil::Test::operation(const Face face,
                                            const StencilModification whenStencilTestFailed,
                                            const StencilModification whenDepthTestFailed,
                                            const StencilModification whenDepthTestPassed)
   {
#ifdef GPU_IMMEDIATE_MODE
   if ( face == FrontFace || face == BothFaces )
      Profile( glStencilOpSeparate(GL_FRONT, 
                                   gl::StencilModification[whenStencilTestFailed],
                                   gl::StencilModification[whenDepthTestFailed],
                                   gl::StencilModification[whenDepthTestPassed]) )
   if ( face == BackFace || face == BothFaces )
      Profile( glStencilOpSeparate(GL_BACK, 
                                   gl::StencilModification[whenStencilTestFailed],
                                   gl::StencilModification[whenDepthTestFailed],
                                   gl::StencilModification[whenDepthTestPassed]) )
#else
   if ( face == FrontFace || 
        face == BothFaces )
      {
      GpuContext.state.stencil.front.operation[0] = gl::StencilModification[whenStencilTestFailed];
      GpuContext.state.stencil.front.operation[1] = gl::StencilModification[whenDepthTestFailed];
      GpuContext.state.stencil.front.operation[2] = gl::StencilModification[whenDepthTestPassed];

      // Add to deffered list   
      if ( !GpuContext.state.dirtyBits.stencilFrontOps &&
           !GpuContext.state.dirtyBits.stencilBackOps )
         GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilOpsUpdate;
      if (!GpuContext.state.dirtyBits.stencilFrontOps)
         GpuContext.state.dirtyBits.stencilFrontOps = true;
      }
   if ( face == BackFace || 
        face == BothFaces )
      {
      GpuContext.state.stencil.back.operation[0]  = gl::StencilModification[whenStencilTestFailed];
      GpuContext.state.stencil.back.operation[1]  = gl::StencilModification[whenDepthTestFailed];
      GpuContext.state.stencil.back.operation[2]  = gl::StencilModification[whenDepthTestPassed];

      // Add to deffered list   
      if ( !GpuContext.state.dirtyBits.stencilFrontOps &&
           !GpuContext.state.dirtyBits.stencilBackOps )
         GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilOpsUpdate;
      if (!GpuContext.state.dirtyBits.stencilBackOps)
         GpuContext.state.dirtyBits.stencilBackOps = true;
      }
#endif
   }

   void Interface::Stencil::Test::function(const CompareMethod compare, const sint32 reference, const uint8 mask)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glStencilFuncSeparate(GL_FRONT, TranslateCompareMethod[compare], reference, mask) )
   Profile( glStencilFuncSeparate(GL_BACK,  TranslateCompareMethod[compare], reference, mask) )
#else
   GpuContext.state.stencil.front.function  = gl::TranslateCompareMethod[compare];
   GpuContext.state.stencil.front.reference = reference;
   GpuContext.state.stencil.front.mask      = mask;
   GpuContext.state.stencil.back.function   = gl::TranslateCompareMethod[compare];
   GpuContext.state.stencil.back.reference  = reference;
   GpuContext.state.stencil.back.mask       = mask;

   // Add to deffered list   
   if ( !GpuContext.state.dirtyBits.stencilFrontFunc || 
        !GpuContext.state.dirtyBits.stencilBackFunc )
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilFuncUpdate;

      if (!GpuContext.state.dirtyBits.stencilFrontFunc)
         GpuContext.state.dirtyBits.stencilFrontFunc = true;  
      if (!GpuContext.state.dirtyBits.stencilBackFunc)
         GpuContext.state.dirtyBits.stencilBackFunc = true;
      }
#endif
   }

   void Interface::Stencil::Test::function(const Face face, const CompareMethod compare, const sint32 reference, const uint8 mask)
   {
#ifdef GPU_IMMEDIATE_MODE
   if ( face == FrontFace || face == BothFaces )
      Profile( glStencilFuncSeparate(GL_FRONT, TranslateCompareMethod[compare], reference, mask) )
   if ( face == BackFace  || face == BothFaces )
      Profile( glStencilFuncSeparate(GL_BACK,  TranslateCompareMethod[compare], reference, mask) )
#else
   if ( face == FrontFace || 
        face == BothFaces )
      {
      GpuContext.state.stencil.front.function  = gl::TranslateCompareMethod[compare];
      GpuContext.state.stencil.front.reference = reference;
      GpuContext.state.stencil.front.mask      = mask;

      // Add to deffered list   
      if ( !GpuContext.state.dirtyBits.stencilFrontFunc && 
           !GpuContext.state.dirtyBits.stencilBackFunc )
         GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilFuncUpdate;
      if (!GpuContext.state.dirtyBits.stencilFrontFunc)
         GpuContext.state.dirtyBits.stencilFrontFunc = true; 
      }
   if ( face == BackFace || 
        face == BothFaces )
      {
      GpuContext.state.stencil.back.function   = gl::TranslateCompareMethod[compare];
      GpuContext.state.stencil.back.reference  = reference;
      GpuContext.state.stencil.back.mask       = mask;

      // Add to deffered list   
      if ( !GpuContext.state.dirtyBits.stencilFrontFunc && 
           !GpuContext.state.dirtyBits.stencilBackFunc )
         GpuContext.state.update[GpuContext.state.dirty++] = gl::StencilFuncUpdate;
      if (!GpuContext.state.dirtyBits.stencilBackFunc)
         GpuContext.state.dirtyBits.stencilBackFunc = true;
      }
#endif
   }

   void Interface::Color::Buffer::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glColorMask(true, true, true, true) )
#else
   GpuContext.state.color.enabled[0] = true;
   GpuContext.state.color.enabled[1] = true;
   GpuContext.state.color.enabled[2] = true;
   GpuContext.state.color.enabled[3] = true;

   // Add to deffered list    
   if (!GpuContext.state.dirtyBits.color)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ColorWritingUpdate;
      GpuContext.state.dirtyBits.color = true;
      }
#endif
   }

   void Interface::Color::Buffer::mask(const bool r, const bool g, const bool b, const bool a)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glColorMask(r, g, b, a) )
#else
   GpuContext.state.color.enabled[0] = r;
   GpuContext.state.color.enabled[1] = g;
   GpuContext.state.color.enabled[2] = b;
   GpuContext.state.color.enabled[3] = a;
   
   // Add to deffered list    
   if (!GpuContext.state.dirtyBits.color)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ColorWritingUpdate;
      GpuContext.state.dirtyBits.color = true;
      }
#endif
   }

   void Interface::Color::Buffer::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glColorMask(false, false, false, false) )
#else
   GpuContext.state.color.enabled[0] = false;
   GpuContext.state.color.enabled[1] = false;
   GpuContext.state.color.enabled[2] = false;
   GpuContext.state.color.enabled[3] = false;

   // Add to deffered list    
   if (!GpuContext.state.dirtyBits.color)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ColorWritingUpdate;
      GpuContext.state.dirtyBits.color = true;
      }
#endif
   }

   void Interface::Color::Buffer::clearValue(const float4 color)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glClearColor(color.r, color.g, color.b, color.a) )
#else
   GpuContext.state.color.clear.r = color.r;
   GpuContext.state.color.clear.g = color.g;
   GpuContext.state.color.clear.b = color.b;
   GpuContext.state.color.clear.a = color.a;
   
   GpuContext.state.dirtyBits.colorClearValue = true;
#endif
   }

   void Interface::Color::Buffer::clearValue(const float r, const float g, const float b, const float a)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glClearColor(r, g, b, a) )
#else
   GpuContext.state.color.clear.r = r;
   GpuContext.state.color.clear.g = g;
   GpuContext.state.color.clear.b = b;
   GpuContext.state.color.clear.a = a;
   
   GpuContext.state.dirtyBits.colorClearValue = true;
#endif
   }

   void Interface::Color::Buffer::clear(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glClear(GL_COLOR_BUFFER_BIT) )
#else
   // Add to deffered list 
   if (!GpuContext.state.dirtyBits.colorClear)
      {
      GpuContext.state.update[GpuContext.state.dirty++] = gl::ClearUpdate;
      GpuContext.state.dirtyBits.colorClear = true;
      }
#endif
   } 




   void Interface::Output::Blend::on(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glEnable(GL_BLEND) )
#else
   GpuContext.state.output.color[0].blend.on = true;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blend, 0))
      return;
   if (GpuContext.state.dirtyBits.blend == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendUpdate;
   setBit(GpuContext.state.dirtyBits.blend, 0);
#endif
   }
 
   bool Interface::Output::Blend::function(const BlendFunction srcRGBA,
      const BlendFunction dstRGBA)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glBlendFuncSeparate(gl::BlendFunction[srcRGBA].type, 
                                gl::BlendFunction[dstRGBA].type, 
                                gl::BlendFunction[srcRGBA].type,
                                gl::BlendFunction[dstRGBA].type) )
#else
   if (!gl::BlendSourceFunctionSupported[srcRGBA])
      return false;
   if (!gl::BlendDestinationFunctionSupported[dstRGBA])
      return false;

   GpuContext.state.output.color[0].blend.srcFuncRGB = gl::BlendFunction[srcRGBA].type;
   GpuContext.state.output.color[0].blend.srcFuncA   = gl::BlendFunction[srcRGBA].type;
   GpuContext.state.output.color[0].blend.dstFuncRGB = gl::BlendFunction[dstRGBA].type;
   GpuContext.state.output.color[0].blend.dstFuncA   = gl::BlendFunction[dstRGBA].type;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendFunction, 0))
      return true;
   if (GpuContext.state.dirtyBits.blendFunction == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendFuncUpdate;
   setBit(GpuContext.state.dirtyBits.blendFunction, 0);
#endif
   return true;
   }
                                           
   bool Interface::Output::Blend::function(const BlendFunction srcRGB,
      const BlendFunction srcA,
      const BlendFunction dstRGB,
      const BlendFunction dstA)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glBlendFuncSeparate(gl::BlendFunction[srcRGB].type, 
                                gl::BlendFunction[dstRGB].type,
                                gl::BlendFunction[srcA].type, 
                                gl::BlendFunction[dstA].type) )
#else
   if (!gl::BlendSourceFunctionSupported[srcRGB])
      return false;
   if (!gl::BlendSourceFunctionSupported[srcA])
      return false;
   if (!gl::BlendDestinationFunctionSupported[dstRGB])
      return false;
   if (!gl::BlendDestinationFunctionSupported[dstA])
      return false;

   GpuContext.state.output.color[0].blend.srcFuncRGB = gl::BlendFunction[srcRGB].type;
   GpuContext.state.output.color[0].blend.srcFuncA   = gl::BlendFunction[srcA].type;
   GpuContext.state.output.color[0].blend.dstFuncRGB = gl::BlendFunction[dstRGB].type;
   GpuContext.state.output.color[0].blend.dstFuncA   = gl::BlendFunction[dstA].type;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendFunction, 0))
      return true;
   if (GpuContext.state.dirtyBits.blendFunction == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendFuncUpdate;
   setBit(GpuContext.state.dirtyBits.blendFunction, 0);
#endif
   return true;
   }

   bool Interface::Output::Blend::equation(const BlendEquation rgba)
   {
   #ifdef EN_OPENGL_MOBILE
   // OpenGL ES 2.0 Doesn't support Min and Max blending equations
   if ((rgba == Min) ||
       (rgba == Max))
      return false;
   #endif
#ifdef GPU_IMMEDIATE_MODE
#if defined(EN_DISCRETE_GPU) && !defined(EN_PLATFORM_OSX)
   Profile( glBlendEquationSeparateEXT(gl::BlendEquation[rgba], gl::BlendEquation[rgba]) )
#else // EN_OPENGL_MOBILE
   Profile( glBlendEquationSeparate(gl::BlendEquation[rgba], gl::BlendEquation[rgba]) )
#endif
#else
   GpuContext.state.output.color[0].blend.equationRGB = gl::BlendEquation[rgba];
   GpuContext.state.output.color[0].blend.equationA   = gl::BlendEquation[rgba];

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendEquation, 0))
      return true;
   if (GpuContext.state.dirtyBits.blendEquation == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendEquationUpdate;
   setBit(GpuContext.state.dirtyBits.blendEquation, 0);
#endif
   return true;
   }

   bool Interface::Output::Blend::equation(const BlendEquation rgb,
      const BlendEquation a)
   {
   #ifdef EN_OPENGL_MOBILE
   // OpenGL ES 2.0 Doesn't support Min and Max blending equations
   if ((rgb == Min) ||
       (rgb == Max) ||
       (a == Min) ||
       (a == Max))
      return false;
   #endif
#ifdef GPU_IMMEDIATE_MODE
#if defined(EN_DISCRETE_GPU) && !defined(EN_PLATFORM_OSX)
   Profile( glBlendEquationSeparateEXT(gl::BlendEquation[rgb], gl::BlendEquation[a]) )
#else // EN_OPENGL_MOBILE
   Profile( glBlendEquationSeparate(gl::BlendEquation[rgb], gl::BlendEquation[a]) )
#endif
#else
   GpuContext.state.output.color[0].blend.equationRGB = gl::BlendEquation[rgb];
   GpuContext.state.output.color[0].blend.equationA   = gl::BlendEquation[a];

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendEquation, 0))
      return true;
   if (GpuContext.state.dirtyBits.blendEquation == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendEquationUpdate;
   setBit(GpuContext.state.dirtyBits.blendEquation, 0);
#endif
   return true;
   }

   void Interface::Output::Blend::off(void)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glDisable(GL_BLEND) )
#else
   GpuContext.state.output.color[0].blend.on = false;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blend, 0))
      return;
   if (GpuContext.state.dirtyBits.blend == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendUpdate;
   setBit(GpuContext.state.dirtyBits.blend, 0);
#endif
   }

   bool Interface::Output::Blend::on(const uint8 n)
   {
   // Choosed render target is not supported
   if (n >= GpuContext.support.maxRenderTargets)
      return false;

#ifdef GPU_IMMEDIATE_MODE
   Profile( glEnablei(GL_BLEND, n) )
#else
   GpuContext.state.output.color[n].blend.on = true;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blend, n))
      return true;
   if (GpuContext.state.dirtyBits.blend == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendUpdate;
   setBit(GpuContext.state.dirtyBits.blend, n);
#endif
   return true;
   }
 
   bool Interface::Output::Blend::function(const uint8 n,
      const BlendFunction srcRGBA,
      const BlendFunction dstRGBA)
   {
#ifdef GPU_IMMEDIATE_MODE
   if (GpuContext.screen.support(OpenGL_4_0))   
       {      
       Profile( glBlendFuncSeparatei(n, gl::BlendFunction[srcRGBA].type, 
                                        gl::BlendFunction[dstRGBA].type, 
                                        gl::BlendFunction[srcRGBA].type,
                                        gl::BlendFunction[dstRGBA].type) )
       }
    else
    if (GpuContext.support.extension(ARB_draw_buffers_blend))  
       {
       Profile( glBlendFuncSeparateiARB(n, gl::BlendFunction[srcRGBA].type, 
                                           gl::BlendFunction[dstRGBA].type, 
                                           gl::BlendFunction[srcRGBA].type,
                                           gl::BlendFunction[dstRGBA].type) )
       }
#else
   // Choosed render target is not supported
   if (n >= GpuContext.support.maxRenderTargets)
      return false;

   if (!gl::BlendSourceFunctionSupported[srcRGBA])
      return false;
   if (!gl::BlendDestinationFunctionSupported[dstRGBA])
      return false;

   GpuContext.state.output.color[n].blend.srcFuncRGB  = gl::BlendFunction[srcRGBA].type;
   GpuContext.state.output.color[n].blend.srcFuncA    = gl::BlendFunction[srcRGBA].type;
   GpuContext.state.output.color[n].blend.dstFuncRGB  = gl::BlendFunction[dstRGBA].type;
   GpuContext.state.output.color[n].blend.dstFuncA    = gl::BlendFunction[dstRGBA].type;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendFunction, n))
      return true;
   if (GpuContext.state.dirtyBits.blendFunction == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendFuncUpdate;
   setBit(GpuContext.state.dirtyBits.blendFunction, n);
#endif
   return true;
   }
          
   bool Interface::Output::Blend::function(const uint8 n,
      const BlendFunction srcRGB,
      const BlendFunction srcA,
      const BlendFunction dstRGB,
      const BlendFunction dstA)
   {
   // Choosed render target is not supported
   if (n >= GpuContext.support.maxRenderTargets)
      return false;

   if (!gl::BlendSourceFunctionSupported[srcRGB])
      return false;
   if (!gl::BlendSourceFunctionSupported[srcA])
      return false;
   if (!gl::BlendDestinationFunctionSupported[dstRGB])
      return false;
   if (!gl::BlendDestinationFunctionSupported[dstA])
      return false;

#ifdef GPU_IMMEDIATE_MODE
   if (GpuContext.screen.support(OpenGL_4_0))   
       {      
       Profile( glBlendFuncSeparatei(n, gl::BlendFunction[srcRGB].type, 
                                        gl::BlendFunction[dstRGB].type,
                                        gl::BlendFunction[srcA].type, 
                                        gl::BlendFunction[dstA].type) )
       }
    else
    if (GpuContext.support.extension(ARB_draw_buffers_blend))  
       {
       Profile( glBlendFuncSeparateiARB(n, gl::BlendFunction[srcRGB].type,  
                                           gl::BlendFunction[dstRGB].type,
                                           gl::BlendFunction[srcA].type,
                                           gl::BlendFunction[dstA].type) )
       }
#else
   GpuContext.state.output.color[n].blend.srcFuncRGB = gl::BlendFunction[srcRGB].type;
   GpuContext.state.output.color[n].blend.srcFuncA   = gl::BlendFunction[srcA].type;
   GpuContext.state.output.color[n].blend.dstFuncRGB = gl::BlendFunction[dstRGB].type;
   GpuContext.state.output.color[n].blend.dstFuncA   = gl::BlendFunction[dstA].type;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendFunction, n))
      return true;
   if (GpuContext.state.dirtyBits.blendFunction == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendFuncUpdate;
   setBit(GpuContext.state.dirtyBits.blendFunction, n);
#endif
   return true;
   }

   bool Interface::Output::Blend::equation(const uint8 n,
      const BlendEquation rgba)
   {
   // Choosed render target is not supported
   if (n >= GpuContext.support.maxRenderTargets)
      return false;

   #ifdef EN_OPENGL_MOBILE
   // OpenGL ES 2.0 Doesn't support Min and Max blending equations
   if ((rgba == Min) ||
       (rgba == Max))
      return false;
   #endif

#ifdef GPU_IMMEDIATE_MODE
#ifdef EN_OPENGL_DESKTOP
   if (GpuContext.screen.support(OpenGL_4_0))  
      {   
      Profile( glBlendEquationSeparatei(n, gl::BlendEquation[rgba], 
                                           gl::BlendEquation[rgba]) )
      }
   else
   if (GpuContext.support.extension(ARB_draw_buffers_blend)) 
      {
      Profile( glBlendEquationSeparateiARB(n, gl::BlendEquation[rgba], 
                                              gl::BlendEquation[rgba]) )
      }
#endif
#else
   GpuContext.state.output.color[n].blend.equationRGB = gl::BlendEquation[rgba];
   GpuContext.state.output.color[n].blend.equationA   = gl::BlendEquation[rgba];

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendEquation, n))
      return true;
   if (GpuContext.state.dirtyBits.blendEquation == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendEquationUpdate;
   setBit(GpuContext.state.dirtyBits.blendEquation, n);
#endif
   return true;
   }

   bool Interface::Output::Blend::equation(const uint8 n,
      const BlendEquation rgb,
      const BlendEquation a)
   {
   // Choosed render target is not supported
   if (n >= GpuContext.support.maxRenderTargets)
      return false;

   #ifdef EN_OPENGL_MOBILE
   // OpenGL ES 2.0 Doesn't support Min and Max blending equations
   if ((rgb == Min) ||
       (rgb == Max) ||
       (a == Min) ||
       (a == Max))
      return false;
   #endif

#ifdef GPU_IMMEDIATE_MODE
#ifdef EN_OPENGL_DESKTOP
   if (GpuContext.screen.support(OpenGL_4_0))  
      {   
      Profile( glBlendEquationSeparatei(n, gl::BlendEquation[rgb], 
                                           gl::BlendEquation[a]) )
      }
   else
   if (GpuContext.support.extension(ARB_draw_buffers_blend)) 
      {
      Profile( glBlendEquationSeparateiARB(n, gl::BlendEquation[rgb], 
                                              gl::BlendEquation[a]) )
      }
#endif
#else
   GpuContext.state.output.color[n].blend.equationRGB = gl::BlendEquation[rgb];
   GpuContext.state.output.color[n].blend.equationA   = gl::BlendEquation[a];

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blendEquation, n))
      return true;
   if (GpuContext.state.dirtyBits.blendEquation == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendEquationUpdate;
   setBit(GpuContext.state.dirtyBits.blendEquation, n);
#endif
   return true;
   }

   bool Interface::Output::Blend::off(const uint8 n)
   {
   // Choosed render target is not supported
   if (n >= GpuContext.support.maxRenderTargets)
      return false;

#ifdef GPU_IMMEDIATE_MODE
#ifdef EN_OPENGL_DESKTOP
   Profile( glDisablei(GL_BLEND, n) )
#endif
#else
   GpuContext.state.output.color[n].blend.on = false;

   // Mark for deffered update
   if (checkBit(GpuContext.state.dirtyBits.blend, n))
      return true;
   if (GpuContext.state.dirtyBits.blend == 0)
      GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendUpdate;
   setBit(GpuContext.state.dirtyBits.blend, n);
#endif
   return true;
   }

   void Interface::Output::Blend::color(const float4 color)    
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glBlendColor(color.r, color.g, color.b, color.a) )
#else
   GpuContext.state.output.blendColor = color;

   // Mark for deffered update
   if (GpuContext.state.dirtyBits.blendColor)
      return;
   GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendColorUpdate;
   GpuContext.state.dirtyBits.blendColor = true;
#endif
   }

   void Interface::Output::Blend::color(const float r,      
      const float g,  
      const float b,
      const float a)
   {
#ifdef GPU_IMMEDIATE_MODE
   Profile( glBlendColor(r, g, b, a) )
#else
   GpuContext.state.output.blendColor.r = r;
   GpuContext.state.output.blendColor.g = g;
   GpuContext.state.output.blendColor.b = b;
   GpuContext.state.output.blendColor.a = a;

   // Mark for deffered update
   if (GpuContext.state.dirtyBits.blendColor)
      return;
   GpuContext.state.update[GpuContext.state.dirty++] = gl::BlendColorUpdate;
   GpuContext.state.dirtyBits.blendColor = true;
#endif
   }

   Ptr<Framebuffer> Interface::Output::Buffer::create(void)
   {
   assert( GpuContext.screen.created );

#ifdef EN_OPENGL_DESKTOP
#if !defined(EN_PLATFORM_OSX)
   if ( GpuContext.screen.support(OpenGL_4_5) )
      return ptr_dynamic_cast<Framebuffer, FramebufferGL45>(new FramebufferGL45());
#endif
   if ( GpuContext.screen.support(OpenGL_4_0) )
      return ptr_dynamic_cast<Framebuffer, FramebufferGL40>(new FramebufferGL40());
   if ( GpuContext.screen.support(OpenGL_3_2) )
      return ptr_dynamic_cast<Framebuffer, FramebufferGL32>(new FramebufferGL32());
   if ( GpuContext.screen.support(OpenGL_3_1) )
      return ptr_dynamic_cast<Framebuffer, FramebufferGL31>(new FramebufferGL31());
   return ptr_dynamic_cast<Framebuffer, FramebufferGL30>(new FramebufferGL30());
#elif  EN_OPENGL_MOBILE
   // TODO: OpenGL ES 3.0 Framebuffer creation
   return Ptr<Framebuffer>(nullptr);
#endif

   //if (GpuContext.device.api.better(OpenGL_3_0))
   //   return Ptr<FramebufferGL45>((gpu::Framebuffer*) new gpu::gl30::Framebuffer()); 
   //return Ptr<gpu::Framebuffer>(NULL);
   }

   bool Interface::Output::Buffer::set(const Ptr<gpu::Framebuffer> framebuffer)
   {
   assert( GpuContext.screen.created );
   assert( framebuffer );
  
   if ( GpuContext.screen.support(OpenGL_3_0) )
      {
      Ptr<FramebufferGL30> fbo = ptr_dynamic_cast<FramebufferGL30, Framebuffer>(framebuffer);
      Profile( glBindFramebuffer(GL_FRAMEBUFFER, fbo->id) );
      return true;
      }

//   if (GpuContext.device.api.better(OpenGL_3_0))
//      {
//      gpu::gl30::Framebuffer* fbo = *((gpu::gl30::Framebuffer**)(&framebuffer)); 
//
//#ifdef GPU_IMMEDIATE_MODE
//      Profile( glBindFramebuffer(GL_FRAMEBUFFER, fbo->id) );
//#else
//      if (GpuContext.state.output.framebuffer.read  != fbo->id ||
//          GpuContext.state.output.framebuffer.write != fbo->id)
//         {
//         GpuContext.state.output.framebuffer.read  = fbo->id;
//         GpuContext.state.output.framebuffer.write = fbo->id;
//         Profile( glBindFramebuffer(GL_FRAMEBUFFER, fbo->id) );
//         }
//#endif
//      return true;
//      }

   return false;
   }

   bool Interface::Output::Buffer::setRead(const Ptr<gpu::Framebuffer> framebuffer)
   {
   assert( GpuContext.screen.created );

   if ( GpuContext.screen.support(OpenGL_3_0) )
      {
      Ptr<FramebufferGL30> fbo = ptr_dynamic_cast<FramebufferGL30, Framebuffer>(framebuffer);
      Profile( glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->id) );
      return true;
      }

//   if (GpuContext.device.api.better(OpenGL_3_0))
//      {
//      gpu::gl30::Framebuffer* fbo = *((gpu::gl30::Framebuffer**)(&framebuffer)); 
//
//#ifdef GPU_IMMEDIATE_MODE
//      Profile( glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->id) );
//#else
//      if (GpuContext.state.output.framebuffer.read != fbo->id)
//         {
//         GpuContext.state.output.framebuffer.read = fbo->id;
//         Profile( glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->id) )
//         }
//#endif
//      return true;
//      }

   return false;
   }

   bool Interface::Output::Buffer::setWrite(const Ptr<gpu::Framebuffer> framebuffer)
   {
   assert( GpuContext.screen.created );

   if ( GpuContext.screen.support(OpenGL_3_0) )
      {
      Ptr<FramebufferGL30> fbo = ptr_dynamic_cast<FramebufferGL30, Framebuffer>(framebuffer);
      Profile( glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id) );
      return true;
      }

//   if (GpuContext.device.api.better(OpenGL_3_0))
//      {
//      gpu::gl30::Framebuffer* fbo = *((gpu::gl30::Framebuffer**)(&framebuffer)); 
//
//#ifdef GPU_IMMEDIATE_MODE
//      Profile( glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id) );
//#else
//      if (GpuContext.state.output.framebuffer.write != fbo->id)
//         {
//         GpuContext.state.output.framebuffer.write = fbo->id;
//         Profile( glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id) )
//         }
//#endif
//      return true;
//      }

   return false;
   }

   void Interface::Output::Buffer::setDefault(void)
   {
   assert( GpuContext.screen.created );
   Profile( glBindFramebuffer(GL_FRAMEBUFFER, 0) )
   GpuContext.state.output.framebuffer.read  = 0;
   GpuContext.state.output.framebuffer.write = 0;
   }

   void Interface::Output::Buffer::setDefaultRead(void)
   {
   assert( GpuContext.screen.created );
   Profile( glBindFramebuffer(GL_READ_FRAMEBUFFER, 0) )
   GpuContext.state.output.framebuffer.read  = 0;
   }

   void Interface::Output::Buffer::setDefaultWrite(void)
   { 
   assert( GpuContext.screen.created );
   Profile( glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0) )
   GpuContext.state.output.framebuffer.write = 0;
   }

   bool Interface::vsync(bool state)
   {
   assert( GpuContext.screen.created );
#ifdef EN_PLATFORM_WINDOWS
   if (!GpuContext.support.extension(EXT_swap_control))
      return false;

   wglSwapIntervalEXT(state ? 1 : 0);
   return true;
#endif
   return false;
   }

   void Interface::Output::Buffer::copy(const uint32v4 srcRange, const uint32v4 dstRange, const SamplerFilter filtering)
   {
   assert( GpuContext.screen.created );
   assert( GpuContext.device.api.better(OpenGL_3_0) );

   Profile(  glBlitFramebuffer(srcRange.x, srcRange.y, srcRange.z, srcRange.w,
                               dstRange.x, dstRange.y, dstRange.z, dstRange.w,
                               GL_COLOR_BUFFER_BIT, 
                               TranslateSamplerMagnification[underlyingType(filtering)]) );
   }

   void Interface::Output::mode(const ColorSpace mode)
   {
   assert( GpuContext.screen.created );
   if (mode == ColorSpaceLinear) Profile( glEnable(GL_FRAMEBUFFER_SRGB) )
   if (mode == ColorSpaceSRGB)   Profile( glDisable(GL_FRAMEBUFFER_SRGB) )
   }

   void Interface::waitForIdle(void)
   {
   assert( GpuContext.screen.created );
   Profile( glFlush() )
   Profile( glFinish() )
   }

   }
}

#endif
