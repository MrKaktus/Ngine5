/*

 Ngine v5.0
 
 Module      : Extensions initiation.
 Visibility  : Engine only.
 Requirements: none
 Description : In Windows, engine needs to get 
               functions pointers from OpenGL ICD 
               driver, because Microsoft's opengl32.dll
               does not support functions introduced 
               after OpenGL v1.1 .

*/

#include "core/defines.h"
#include "core/types.h"
#include "core/log/log.h"
#include "core/rendering/context.h"

namespace en
{
   namespace gpu
   {
      namespace wgl
      {
      #if 0
      //#ifdef EN_PLATFORM_WINDOWS

      // - binds only pointers to functions allowed in given API and version
      // - returns error logs if cannot bind
      // - should be called after RC is created
      // = (PFNPROC) wglGetProcAddress("");
      
      void unbindedOpenGLFunctionHandler(...)
      {
      Log << "Error: Called unbinded OpenGL function.\n";
      }

      // MACRO: Safe OpenGL function binding with fallback
      #define bindFunction(y,x) \
      { \
      x = (y) wglGetProcAddress(#x); \
      if (!x) \
         { \
         x = (y) &unbindedOpenGLFunctionHandler; \
         Log << "Error: Cannot bind function "; \
         Log << #x << endl; \
         }; \
      }

      void BindFunctionsPointers(void)
      {
      //
      // OpenGL 4.5
      //
      //   GLSL 4.50
      //
      //   GL_ARB_clip_control
      //   GL_ARB_cull_distance
      //   GL_ARB_ES3_1_compatibility
      //   GL_ARB_conditional_render_inverted
      //   GL_KHR_context_flush_control
      //   GL_ARB_derivative_control (OpenGL Shading Language only)
      //   GL_ARB_direct_state_access
      //   GL_ARB_get_texture_sub_image
      //   GL_KHR_robustness
      //   GL_ARB_shader_texture_image_samples (OpenGL Shading Language only)
      //   GL_ARB_texture_barrier
      //
      if ( GpuContext.screen.support(OpenGL_4_5) || 
           GpuContext.support.extension(ARB_clip_control) )
         {
         bindFunction( PFNGLCLIPCONTROLPROC,                                 glClipControl );
         }
      if ( GpuContext.screen.support(OpenGL_4_5) || 
           GpuContext.support.extension(ARB_ES3_1_compatibility) )
         {
         bindFunction( PFNGLMEMORYBARRIERBYREGIONPROC,                       glMemoryBarrierByRegion );
         }
      if ( GpuContext.screen.support(OpenGL_4_5) || 
           GpuContext.support.extension(ARB_direct_state_access) )
         {
         bindFunction( PFNGLCREATETRANSFORMFEEDBACKSPROC,                    glCreateTransformFeedbacks );
         bindFunction( PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC,                 glTransformFeedbackBufferBase );
         bindFunction( PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC,                glTransformFeedbackBufferRange );
         bindFunction( PFNGLGETTRANSFORMFEEDBACKIVPROC,                      glGetTransformFeedbackiv );
         bindFunction( PFNGLGETTRANSFORMFEEDBACKI_VPROC,                     glGetTransformFeedbacki_v );
         bindFunction( PFNGLGETTRANSFORMFEEDBACKI64_VPROC,                   glGetTransformFeedbacki64_v );
         bindFunction( PFNGLCREATEBUFFERSPROC,                               glCreateBuffers );
         bindFunction( PFNGLNAMEDBUFFERSTORAGEPROC,                          glNamedBufferStorage );
         bindFunction( PFNGLNAMEDBUFFERDATAPROC,                             glNamedBufferData );
         bindFunction( PFNGLNAMEDBUFFERSUBDATAPROC,                          glNamedBufferSubData );
         bindFunction( PFNGLCOPYNAMEDBUFFERSUBDATAPROC,                      glCopyNamedBufferSubData );
         bindFunction( PFNGLCLEARNAMEDBUFFERDATAPROC,                        glClearNamedBufferData );
         bindFunction( PFNGLCLEARNAMEDBUFFERSUBDATAPROC,                     glClearNamedBufferSubData );
         bindFunction( PFNGLMAPNAMEDBUFFERPROC,                              glMapNamedBuffer );
         bindFunction( PFNGLMAPNAMEDBUFFERRANGEPROC,                         glMapNamedBufferRange );
         bindFunction( PFNGLUNMAPNAMEDBUFFERPROC,                            glUnmapNamedBuffer );
         bindFunction( PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC,                 glFlushMappedNamedBufferRange );
         bindFunction( PFNGLGETNAMEDBUFFERPARAMETERIVPROC,                   glGetNamedBufferParameteriv );
         bindFunction( PFNGLGETNAMEDBUFFERPARAMETERI64VPROC,                 glGetNamedBufferParameteri64v );
         bindFunction( PFNGLGETNAMEDBUFFERPOINTERVPROC,                      glGetNamedBufferPointerv );
         bindFunction( PFNGLGETNAMEDBUFFERSUBDATAPROC,                       glGetNamedBufferSubData );
         bindFunction( PFNGLCREATEFRAMEBUFFERSPROC,                          glCreateFramebuffers );
         bindFunction( PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC,                glNamedFramebufferRenderbuffer );
         bindFunction( PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC,                  glNamedFramebufferParameteri );
         bindFunction( PFNGLNAMEDFRAMEBUFFERTEXTUREPROC,                     glNamedFramebufferTexture );
         bindFunction( PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC,                glNamedFramebufferTextureLayer );
         bindFunction( PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC,                  glNamedFramebufferDrawBuffer );
         bindFunction( PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC,                 glNamedFramebufferDrawBuffers );
         bindFunction( PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC,                  glNamedFramebufferReadBuffer );
         bindFunction( PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC,              glInvalidateNamedFramebufferData );
         bindFunction( PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC,           glInvalidateNamedFramebufferSubData );
         bindFunction( PFNGLCLEARNAMEDFRAMEBUFFERIVPROC,                     glClearNamedFramebufferiv );
         bindFunction( PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC,                    glClearNamedFramebufferuiv );
         bindFunction( PFNGLCLEARNAMEDFRAMEBUFFERFVPROC,                     glClearNamedFramebufferfv );
         bindFunction( PFNGLCLEARNAMEDFRAMEBUFFERFIPROC,                     glClearNamedFramebufferfi );
         bindFunction( PFNGLBLITNAMEDFRAMEBUFFERPROC,                        glBlitNamedFramebuffer );
         bindFunction( PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC,                 glCheckNamedFramebufferStatus );
         bindFunction( PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC,              glGetNamedFramebufferParameteriv );
         bindFunction( PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC,    glGetNamedFramebufferAttachmentParameteriv );
         bindFunction( PFNGLCREATERENDERBUFFERSPROC,                         glCreateRenderbuffers );
         bindFunction( PFNGLNAMEDRENDERBUFFERSTORAGEPROC,                    glNamedRenderbufferStorage );
         bindFunction( PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC,         glNamedRenderbufferStorageMultisample );
         bindFunction( PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC,             glGetNamedRenderbufferParameteriv );
         bindFunction( PFNGLCREATETEXTURESPROC,                              glCreateTextures );
         bindFunction( PFNGLTEXTUREBUFFERPROC,                               glTextureBuffer );
         bindFunction( PFNGLTEXTUREBUFFERRANGEPROC,                          glTextureBufferRange );
         bindFunction( PFNGLTEXTURESTORAGE1DPROC,                            glTextureStorage1D );
         bindFunction( PFNGLTEXTURESTORAGE2DPROC,                            glTextureStorage2D );
         bindFunction( PFNGLTEXTURESTORAGE3DPROC,                            glTextureStorage3D );
         bindFunction( PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC,                 glTextureStorage2DMultisample );
         bindFunction( PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC,                 glTextureStorage3DMultisample );
         bindFunction( PFNGLTEXTURESUBIMAGE1DPROC,                           glTextureSubImage1D );
         bindFunction( PFNGLTEXTURESUBIMAGE2DPROC,                           glTextureSubImage2D );
         bindFunction( PFNGLTEXTURESUBIMAGE3DPROC,                           glTextureSubImage3D );
         bindFunction( PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC,                 glCompressedTextureSubImage1D );
         bindFunction( PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC,                 glCompressedTextureSubImage2D );
         bindFunction( PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC,                 glCompressedTextureSubImage3D );
         bindFunction( PFNGLCOPYTEXTURESUBIMAGE1DPROC,                       glCopyTextureSubImage1D );
         bindFunction( PFNGLCOPYTEXTURESUBIMAGE2DPROC,                       glCopyTextureSubImage2D );
         bindFunction( PFNGLCOPYTEXTURESUBIMAGE3DPROC,                       glCopyTextureSubImage3D );
         bindFunction( PFNGLTEXTUREPARAMETERFPROC,                           glTextureParameterf );
         bindFunction( PFNGLTEXTUREPARAMETERFVPROC,                          glTextureParameterfv );
         bindFunction( PFNGLTEXTUREPARAMETERIPROC,                           glTextureParameteri );
         bindFunction( PFNGLTEXTUREPARAMETERIIVPROC,                         glTextureParameterIiv );
         bindFunction( PFNGLTEXTUREPARAMETERIUIVPROC,                        glTextureParameterIuiv );
         bindFunction( PFNGLTEXTUREPARAMETERIVPROC,                          glTextureParameteriv );
         bindFunction( PFNGLGENERATETEXTUREMIPMAPPROC,                       glGenerateTextureMipmap );
         bindFunction( PFNGLBINDTEXTUREUNITPROC,                             glBindTextureUnit );
         bindFunction( PFNGLGETTEXTUREIMAGEPROC,                             glGetTextureImage );
         bindFunction( PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC,                   glGetCompressedTextureImage );
         bindFunction( PFNGLGETTEXTURELEVELPARAMETERFVPROC,                  glGetTextureLevelParameterfv );
         bindFunction( PFNGLGETTEXTURELEVELPARAMETERIVPROC,                  glGetTextureLevelParameteriv );
         bindFunction( PFNGLGETTEXTUREPARAMETERFVPROC,                       glGetTextureParameterfv );
         bindFunction( PFNGLGETTEXTUREPARAMETERIIVPROC,                      glGetTextureParameterIiv );
         bindFunction( PFNGLGETTEXTUREPARAMETERIUIVPROC,                     glGetTextureParameterIuiv );
         bindFunction( PFNGLGETTEXTUREPARAMETERIVPROC,                       glGetTextureParameteriv );
         bindFunction( PFNGLCREATEVERTEXARRAYSPROC,                          glCreateVertexArrays );
         bindFunction( PFNGLDISABLEVERTEXARRAYATTRIBPROC,                    glDisableVertexArrayAttrib );
         bindFunction( PFNGLENABLEVERTEXARRAYATTRIBPROC,                     glEnableVertexArrayAttrib );
         bindFunction( PFNGLVERTEXARRAYELEMENTBUFFERPROC,                    glVertexArrayElementBuffer );
         bindFunction( PFNGLVERTEXARRAYVERTEXBUFFERPROC,                     glVertexArrayVertexBuffer );
         bindFunction( PFNGLVERTEXARRAYVERTEXBUFFERSPROC,                    glVertexArrayVertexBuffers );
         bindFunction( PFNGLVERTEXARRAYATTRIBFORMATPROC,                     glVertexArrayAttribFormat );
         bindFunction( PFNGLVERTEXARRAYATTRIBIFORMATPROC,                    glVertexArrayAttribIFormat );
         bindFunction( PFNGLVERTEXARRAYATTRIBLFORMATPROC,                    glVertexArrayAttribLFormat );
         bindFunction( PFNGLVERTEXARRAYATTRIBBINDINGPROC,                    glVertexArrayAttribBinding );
         bindFunction( PFNGLVERTEXARRAYBINDINGDIVISORPROC,                   glVertexArrayBindingDivisor );
         bindFunction( PFNGLGETVERTEXARRAYIVPROC,                            glGetVertexArrayiv );
         bindFunction( PFNGLGETVERTEXARRAYINDEXEDIVPROC,                     glGetVertexArrayIndexediv );
         bindFunction( PFNGLGETVERTEXARRAYINDEXED64IVPROC,                   glGetVertexArrayIndexed64iv );
         bindFunction( PFNGLCREATESAMPLERSPROC,                              glCreateSamplers );
         bindFunction( PFNGLCREATEPROGRAMPIPELINESPROC,                      glCreateProgramPipelines );
         bindFunction( PFNGLCREATEQUERIESPROC,                               glCreateQueries );
         bindFunction( PFNGLGETQUERYBUFFEROBJECTIVPROC,                      glGetQueryBufferObjectiv );
         bindFunction( PFNGLGETQUERYBUFFEROBJECTUIVPROC,                     glGetQueryBufferObjectuiv );
         bindFunction( PFNGLGETQUERYBUFFEROBJECTI64VPROC,                    glGetQueryBufferObjecti64v );
         bindFunction( PFNGLGETQUERYBUFFEROBJECTUI64VPROC,                   glGetQueryBufferObjectui64v );
         }
      if ( GpuContext.screen.support(OpenGL_4_5) || 
           GpuContext.support.extension(ARB_get_texture_sub_image) )
         {
         bindFunction( PFNGLGETTEXTURESUBIMAGEPROC,                          glGetTextureSubImage );
         bindFunction( PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC,                glGetCompressedTextureSubImage );
         }
      if ( GpuContext.screen.support(OpenGL_4_5) || 
           GpuContext.support.extension(KHR_robustness) )
         {
         // NOTE: when implemented in an OpenGL ES context, all entry points defined
         // by this extension must have a "KHR" suffix. 
         bindFunction( PFNGLGETGRAPHICSRESETSTATUSPROC,                      glGetGraphicsResetStatus );
         bindFunction( PFNGLREADNPIXELSPROC,                                 glReadnPixels );
         bindFunction( PFNGLGETNUNIFORMFVPROC,                               glGetnUniformfv );
         bindFunction( PFNGLGETNUNIFORMIVPROC,                               glGetnUniformiv );
         bindFunction( PFNGLGETNUNIFORMUIVPROC,                              glGetnUniformuiv );
         }
      if ( GpuContext.screen.support(OpenGL_4_5) || 
           GpuContext.support.extension(ARB_texture_barrier) )
         {
         bindFunction( PFNGLTEXTUREBARRIERPROC,                              glTextureBarrier );
         }
      //
      // OpenGL 4.4
      //
      //   GLSL 4.40
      //
      //   GL_ARB_buffer_storage
      //   GL_ARB_clear_texture
      //   GL_ARB_enhanced_layouts
      //   GL_ARB_multi_bind
      //   GL_ARB_query_buffer_object
      //   GL_ARB_texture_mirror_clamp_to_edge
      //   GL_ARB_texture_stencil8
      //   GL_ARB_vertex_type_10f_11f_11f_rev
      //
      if ( GpuContext.screen.support(OpenGL_4_4) || 
           GpuContext.support.extension(ARB_buffer_storage) )
         {
         bindFunction( PFNGLBUFFERSTORAGEPROC,                               glBufferStorage );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_4) || 
             GpuContext.support.extension(ARB_buffer_storage) ) &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLNAMEDBUFFERSTORAGEEXTPROC,                       glNamedBufferStorageEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_4) || 
           GpuContext.support.extension(ARB_clear_texture) )
         {
         bindFunction( PFNGLCLEARTEXIMAGEPROC,                               glClearTexImage );
         bindFunction( PFNGLCLEARTEXSUBIMAGEPROC,                            glClearTexSubImage );
         }
      if ( GpuContext.screen.support(OpenGL_4_4) || 
           GpuContext.support.extension(ARB_multi_bind) )
         {
         bindFunction( PFNGLBINDBUFFERSBASEPROC,                             glBindBuffersBase );
         bindFunction( PFNGLBINDBUFFERSRANGEPROC,                            glBindBuffersRange );
         bindFunction( PFNGLBINDTEXTURESPROC,                                glBindTextures );
         bindFunction( PFNGLBINDSAMPLERSPROC,                                glBindSamplers );
         bindFunction( PFNGLBINDIMAGETEXTURESPROC,                           glBindImageTextures );
         bindFunction( PFNGLBINDVERTEXBUFFERSPROC,                           glBindVertexBuffers );
         }
      //
      // OpenGL 4.3
      //
      //   GLSL 4.30
      //
      //   GL_ARB_arrays_of_arrays (OpenGL Shading Language only)
      //   GL_ARB_ES3_compatibility
      //   GL_ARB_clear_buffer_object
      //   GL_ARB_compute_shader
      //   GL_ARB_copy_image
      //   GL_ARB_debug_output
      //   GL_ARB_explicit_uniform_location
      //   GL_ARB_fragment_layer_viewport (OpenGL Shading Language only)
      //   GL_ARB_framebuffer_no_attachments
      //   GL_ARB_internalformat_query2
      //   GL_ARB_invalidate_subdata
      //   GL_ARB_multi_draw_indirect
      //   GL_ARB_program_interface_query
      //   GL_ARB_robust_buffer_access_behavior
      //   GL_ARB_shader_image_size (OpenGL Shading Language only)
      //   GL_ARB_shader_storage_buffer_object
      //   GL_ARB_stencil_texturing
      //   GL_ARB_texture_buffer_range
      //   GL_ARB_texture_query_levels
      //   GL_ARB_texture_storage_multisample
      //   GL_ARB_texture_view
      //   GL_ARB_vertex_attrib_binding
      //   GL_KHR_debug
      //
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_clear_buffer_object) )
         {
         bindFunction( PFNGLCLEARBUFFERDATAPROC,                             glClearBufferData );
         bindFunction( PFNGLCLEARBUFFERSUBDATAPROC,                          glClearBufferSubData );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_3) || 
             GpuContext.support.extension(ARB_clear_buffer_object) ) &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLCLEARNAMEDBUFFERDATAEXTPROC,                     glClearNamedBufferDataEXT );
         bindFunction( PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC,                  glClearNamedBufferSubDataEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_compute_shader) )
         {
         bindFunction( PFNGLDISPATCHCOMPUTEPROC,                             glDispatchCompute );
         bindFunction( PFNGLDISPATCHCOMPUTEINDIRECTPROC,                     glDispatchComputeIndirect );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_copy_image) )
         {
         bindFunction( PFNGLCOPYIMAGESUBDATAPROC,                            glCopyImageSubData );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_debug_output) )
         {
         bindFunction( PFNGLDEBUGMESSAGECONTROLPROC,                         glDebugMessageControl );
         bindFunction( PFNGLDEBUGMESSAGEINSERTPROC,                          glDebugMessageInsert );
         bindFunction( PFNGLDEBUGMESSAGECALLBACKPROC,                        glDebugMessageCallback );
         bindFunction( PFNGLGETDEBUGMESSAGELOGPROC,                          glGetDebugMessageLog );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_framebuffer_no_attachments) )
         {
         bindFunction( PFNGLFRAMEBUFFERPARAMETERIPROC,                       glFramebufferParameteri );
         bindFunction( PFNGLGETFRAMEBUFFERPARAMETERIVPROC,                   glGetFramebufferParameteriv );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_3) || 
             GpuContext.support.extension(ARB_framebuffer_no_attachments) ) &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC,               glNamedFramebufferParameteriEXT );
         bindFunction( PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC,           glGetNamedFramebufferParameterivEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_internalformat_query2) )
         {
         bindFunction( PFNGLGETINTERNALFORMATI64VPROC,                       glGetInternalformati64v );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_invalidate_subdata) )
         {
         bindFunction( PFNGLINVALIDATETEXSUBIMAGEPROC,                       glInvalidateTexSubImage );
         bindFunction( PFNGLINVALIDATETEXIMAGEPROC,                          glInvalidateTexImage );
         bindFunction( PFNGLINVALIDATEBUFFERSUBDATAPROC,                     glInvalidateBufferSubData );
         bindFunction( PFNGLINVALIDATEBUFFERDATAPROC,                        glInvalidateBufferData );
         bindFunction( PFNGLINVALIDATEFRAMEBUFFERPROC,                       glInvalidateFramebuffer );
         bindFunction( PFNGLINVALIDATESUBFRAMEBUFFERPROC,                    glInvalidateSubFramebuffer );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_multi_draw_indirect) )
         {
         bindFunction( PFNGLMULTIDRAWARRAYSINDIRECTPROC,                     glMultiDrawArraysIndirect );
         bindFunction( PFNGLMULTIDRAWELEMENTSINDIRECTPROC,                   glMultiDrawElementsIndirect );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_program_interface_query) )
         {
         bindFunction( PFNGLGETPROGRAMINTERFACEIVPROC,                       glGetProgramInterfaceiv );
         bindFunction( PFNGLGETPROGRAMRESOURCEINDEXPROC,                     glGetProgramResourceIndex );
         bindFunction( PFNGLGETPROGRAMRESOURCENAMEPROC,                      glGetProgramResourceName );
         bindFunction( PFNGLGETPROGRAMRESOURCEIVPROC,                        glGetProgramResourceiv );
         bindFunction( PFNGLGETPROGRAMRESOURCELOCATIONPROC,                  glGetProgramResourceLocation );
         bindFunction( PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC,             glGetProgramResourceLocationIndex );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_shader_storage_buffer_object) )
         {
         bindFunction( PFNGLSHADERSTORAGEBLOCKBINDINGPROC,                   glShaderStorageBlockBinding );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_texture_buffer_range) )
         {
         bindFunction( PFNGLTEXBUFFERRANGEPROC,                              glTexBufferRange );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_3) || 
             GpuContext.support.extension(ARB_texture_buffer_range) ) &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLTEXTUREBUFFERRANGEEXTPROC,                       glTextureBufferRangeEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_texture_storage_multisample) )
         {
         bindFunction( PFNGLTEXSTORAGE2DMULTISAMPLEPROC,                     glTexStorage2DMultisample );
         bindFunction( PFNGLTEXSTORAGE3DMULTISAMPLEPROC,                     glTexStorage3DMultisample );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_3) || 
             GpuContext.support.extension(ARB_texture_storage_multisample) ) &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC,              glTextureStorage2DMultisampleEXT );
         bindFunction( PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC,              glTextureStorage3DMultisampleEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_texture_view) )
         {
         bindFunction( PFNGLTEXTUREVIEWPROC,                                 glTextureView );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(ARB_vertex_attrib_binding) )
         {
         bindFunction( PFNGLBINDVERTEXBUFFERPROC,                            glBindVertexBuffer );
         bindFunction( PFNGLVERTEXATTRIBFORMATPROC,                          glVertexAttribFormat );
         bindFunction( PFNGLVERTEXATTRIBIFORMATPROC,                         glVertexAttribIFormat );
         bindFunction( PFNGLVERTEXATTRIBLFORMATPROC,                         glVertexAttribLFormat );
         bindFunction( PFNGLVERTEXATTRIBBINDINGPROC,                         glVertexAttribBinding );
         bindFunction( PFNGLVERTEXBINDINGDIVISORPROC,                        glVertexBindingDivisor );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_3) || 
             GpuContext.support.extension(ARB_vertex_attrib_binding) ) &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC,              glVertexArrayBindVertexBufferEXT );
         bindFunction( PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC,            glVertexArrayVertexAttribFormatEXT );
         bindFunction( PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC,           glVertexArrayVertexAttribIFormatEXT );
         bindFunction( PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC,           glVertexArrayVertexAttribLFormatEXT );
         bindFunction( PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC,           glVertexArrayVertexAttribBindingEXT );
         bindFunction( PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC,          glVertexArrayVertexBindingDivisorEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_3) || 
           GpuContext.support.extension(KHR_debug) )
         {
         bindFunction( PFNGLDEBUGMESSAGECONTROLPROC,                         glDebugMessageControl );
         bindFunction( PFNGLDEBUGMESSAGEINSERTPROC,                          glDebugMessageInsert );
         bindFunction( PFNGLDEBUGMESSAGECALLBACKPROC,                        glDebugMessageCallback );
         bindFunction( PFNGLGETDEBUGMESSAGELOGPROC,                          glGetDebugMessageLog );
       //bindFunction( PFNGLGETPOINTERVPROC,                                 glGetPointerv );
         bindFunction( PFNGLPUSHDEBUGGROUPPROC,                              glPushDebugGroup );
         bindFunction( PFNGLPOPDEBUGGROUPPROC,                               glPopDebugGroup );
         bindFunction( PFNGLOBJECTLABELPROC,                                 glObjectLabel );
         bindFunction( PFNGLGETOBJECTLABELPROC,                              glGetObjectLabel );
         bindFunction( PFNGLOBJECTPTRLABELPROC,                              glObjectPtrLabel );
         bindFunction( PFNGLGETOBJECTPTRLABELPROC,                           glGetObjectPtrLabel );
         }
      // 
      // OpenGL 4.2
      //
      //   GLSL 4.20
      //
      //   GL_ARB_base_instance
      //   GL_ARB_conservative_depth
      //   GL_ARB_internalformat_query
      //   GL_ARB_map_buffer_aligment
      //   GL_ARB_shader_atomic_counters
      //   GL_ARB_shader_image_load_store
      //   GL_ARB_shading_language_420pack
      //   GL_ARB_shading_language_packing
      //   GL_ARB_texture_compression_bptc
      //   GL_ARB_texture_storage
      //   GL_ARB_transform_feedback_instanced
      //
      if ( GpuContext.screen.support(OpenGL_4_2) || 
           GpuContext.support.extension(ARB_base_instance) )
         {
         bindFunction( PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC,             glDrawArraysInstancedBaseInstance );
         bindFunction( PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC,           glDrawElementsInstancedBaseInstance );
         bindFunction( PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC, glDrawElementsInstancedBaseVertexBaseInstance );
         }
      if ( GpuContext.screen.support(OpenGL_4_2) || 
           GpuContext.support.extension(ARB_internalformat_query) )
         bindFunction( PFNGLGETINTERNALFORMATIVPROC,                         glGetInternalformativ );
      if ( GpuContext.screen.support(OpenGL_4_2) || 
           GpuContext.support.extension(ARB_shader_atomic_counters) )
         bindFunction( PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC,              glGetActiveAtomicCounterBufferiv );
      if ( GpuContext.screen.support(OpenGL_4_2) || 
           GpuContext.support.extension(ARB_shader_image_load_store) )
         {
         bindFunction( PFNGLBINDIMAGETEXTUREPROC,                            glBindImageTexture );
         bindFunction( PFNGLMEMORYBARRIERPROC,                               glMemoryBarrier );
         }
      if ( GpuContext.screen.support(OpenGL_4_2) || 
           GpuContext.support.extension(ARB_texture_storage) )
         {
         bindFunction( PFNGLTEXSTORAGE1DPROC,                                glTexStorage1D );
         bindFunction( PFNGLTEXSTORAGE2DPROC,                                glTexStorage2D );
         bindFunction( PFNGLTEXSTORAGE3DPROC,                                glTexStorage3D );
         }
      if ( ( GpuContext.screen.support(OpenGL_4_2) || 
             GpuContext.support.extension(ARB_texture_storage) )  &&
             GpuContext.support.extension(EXT_direct_state_access) )
         {
         bindFunction( PFNGLTEXTURESTORAGE1DEXTPROC,                         glTextureStorage1DEXT );
         bindFunction( PFNGLTEXTURESTORAGE2DEXTPROC,                         glTextureStorage2DEXT );
         bindFunction( PFNGLTEXTURESTORAGE3DEXTPROC,                         glTextureStorage3DEXT );
         }
      if ( GpuContext.screen.support(OpenGL_4_2) || 
           GpuContext.support.extension(ARB_transform_feedback_instanced) )
         {
         bindFunction( PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC,              glDrawTransformFeedbackInstanced );
         bindFunction( PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC,        glDrawTransformFeedbackStreamInstanced );
         }
      //
      // OpenGL 4.1
      //
      //   GLSL 4.10
      //
      //   GL_ARB_debug_output
      //   GL_ARB_ES2_compatibility
      //   GL_ARB_get_program_binary
      //   GL_ARB_separate_shader_objects
      //   GL_ARB_shader_precision
      //   GL_ARB_vertex_attrib_64bit
      //   GL_ARB_viewport_array
      //
      if ( GpuContext.screen.support(OpenGL_4_1) || 
           GpuContext.support.extension(ARB_ES2_compatibility) )
         {
         bindFunction( PFNGLCLEARDEPTHFPROC,                                 glClearDepthf );
         bindFunction( PFNGLDEPTHRANGEFPROC,                                 glDepthRangef );
         bindFunction( PFNGLGETSHADERPRECISIONFORMATPROC,                    glGetShaderPrecisionFormat );
         bindFunction( PFNGLRELEASESHADERCOMPILERPROC,                       glReleaseShaderCompiler );
         bindFunction( PFNGLSHADERBINARYPROC,                                glShaderBinary );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_1) ||                                     
           GpuContext.support.extension(ARB_get_program_binary) )                   
         {                                                                   
         bindFunction( PFNGLGETPROGRAMBINARYPROC,                            glGetProgramBinary );
         bindFunction( PFNGLPROGRAMBINARYPROC,                               glProgramBinary );
         bindFunction( PFNGLPROGRAMPARAMETERIPROC,                           glProgramParameteri );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_1) ||                                     
           GpuContext.support.extension(ARB_separate_shader_objects) )              
         {                                                                   
         bindFunction( PFNGLACTIVESHADERPROGRAMPROC,                         glActiveShaderProgram );
         bindFunction( PFNGLBINDPROGRAMPIPELINEPROC,                         glBindProgramPipeline );
         bindFunction( PFNGLCREATESHADERPROGRAMVPROC,                        glCreateShaderProgramv );
         bindFunction( PFNGLDELETEPROGRAMPIPELINESPROC,                      glDeleteProgramPipelines );
         bindFunction( PFNGLGENPROGRAMPIPELINESPROC,                         glGenProgramPipelines );
         bindFunction( PFNGLGETPROGRAMPIPELINEIVPROC,                        glGetProgramPipelineiv );
         bindFunction( PFNGLGETPROGRAMPIPELINEINFOLOGPROC,                   glGetProgramPipelineInfoLog );
         bindFunction( PFNGLISPROGRAMPIPELINEPROC,                           glIsProgramPipeline );
         bindFunction( PFNGLPROGRAMPARAMETERIPROC,                           glProgramParameteri );
         bindFunction( PFNGLPROGRAMUNIFORM1DPROC,                            glProgramUniform1d );
         bindFunction( PFNGLPROGRAMUNIFORM1DVPROC,                           glProgramUniform1dv );
         bindFunction( PFNGLPROGRAMUNIFORM1FPROC,                            glProgramUniform1f );
         bindFunction( PFNGLPROGRAMUNIFORM1FVPROC,                           glProgramUniform1fv );
         bindFunction( PFNGLPROGRAMUNIFORM1IPROC,                            glProgramUniform1i );
         bindFunction( PFNGLPROGRAMUNIFORM1IVPROC,                           glProgramUniform1iv );
         bindFunction( PFNGLPROGRAMUNIFORM1UIPROC,                           glProgramUniform1ui );
         bindFunction( PFNGLPROGRAMUNIFORM1UIVPROC,                          glProgramUniform1uiv );
         bindFunction( PFNGLPROGRAMUNIFORM2DPROC,                            glProgramUniform2d );
         bindFunction( PFNGLPROGRAMUNIFORM2DVPROC,                           glProgramUniform2dv );
         bindFunction( PFNGLPROGRAMUNIFORM2FPROC,                            glProgramUniform2f );
         bindFunction( PFNGLPROGRAMUNIFORM2FVPROC,                           glProgramUniform2fv );
         bindFunction( PFNGLPROGRAMUNIFORM2IPROC,                            glProgramUniform2i );
         bindFunction( PFNGLPROGRAMUNIFORM2IVPROC,                           glProgramUniform2iv );
         bindFunction( PFNGLPROGRAMUNIFORM2UIPROC,                           glProgramUniform2ui );
         bindFunction( PFNGLPROGRAMUNIFORM2UIVPROC,                          glProgramUniform2uiv );
         bindFunction( PFNGLPROGRAMUNIFORM3DPROC,                            glProgramUniform3d );
         bindFunction( PFNGLPROGRAMUNIFORM3DVPROC,                           glProgramUniform3dv );
         bindFunction( PFNGLPROGRAMUNIFORM3FPROC,                            glProgramUniform3f );
         bindFunction( PFNGLPROGRAMUNIFORM3FVPROC,                           glProgramUniform3fv );
         bindFunction( PFNGLPROGRAMUNIFORM3IPROC,                            glProgramUniform3i );
         bindFunction( PFNGLPROGRAMUNIFORM3IVPROC,                           glProgramUniform3iv );
         bindFunction( PFNGLPROGRAMUNIFORM3UIPROC,                           glProgramUniform3ui );
         bindFunction( PFNGLPROGRAMUNIFORM3UIVPROC,                          glProgramUniform3uiv );
         bindFunction( PFNGLPROGRAMUNIFORM4DPROC,                            glProgramUniform4d );
         bindFunction( PFNGLPROGRAMUNIFORM4DVPROC,                           glProgramUniform4dv );
         bindFunction( PFNGLPROGRAMUNIFORM4FPROC,                            glProgramUniform4f );
         bindFunction( PFNGLPROGRAMUNIFORM4FVPROC,                           glProgramUniform4fv );
         bindFunction( PFNGLPROGRAMUNIFORM4IPROC,                            glProgramUniform4i );
         bindFunction( PFNGLPROGRAMUNIFORM4IVPROC,                           glProgramUniform4iv );
         bindFunction( PFNGLPROGRAMUNIFORM4UIPROC,                           glProgramUniform4ui );
         bindFunction( PFNGLPROGRAMUNIFORM4UIVPROC,                          glProgramUniform4uiv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2DVPROC,                     glProgramUniformMatrix2dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2FVPROC,                     glProgramUniformMatrix2fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3DVPROC,                     glProgramUniformMatrix3dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3FVPROC,                     glProgramUniformMatrix3fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4DVPROC,                     glProgramUniformMatrix4dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4FVPROC,                     glProgramUniformMatrix4fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC,                   glProgramUniformMatrix2x3dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC,                   glProgramUniformMatrix2x3fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC,                   glProgramUniformMatrix2x4dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC,                   glProgramUniformMatrix2x4fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC,                   glProgramUniformMatrix3x2dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC,                   glProgramUniformMatrix3x2fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC,                   glProgramUniformMatrix3x4dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC,                   glProgramUniformMatrix3x4fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC,                   glProgramUniformMatrix4x2dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC,                   glProgramUniformMatrix4x2fv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC,                   glProgramUniformMatrix4x3dv );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC,                   glProgramUniformMatrix4x3fv );
         bindFunction( PFNGLUSEPROGRAMSTAGESPROC,                            glUseProgramStages );
         bindFunction( PFNGLVALIDATEPROGRAMPIPELINEPROC,                     glValidateProgramPipeline );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_1) ||                                     
           GpuContext.support.extension(ARB_vertex_attrib_64bit) )                  
         {                                                                   
         bindFunction( PFNGLGETVERTEXATTRIBLDVPROC,                          glGetVertexAttribLdv );
         bindFunction( PFNGLVERTEXATTRIBL1DPROC,                             glVertexAttribL1d );
         bindFunction( PFNGLVERTEXATTRIBL1DVPROC,                            glVertexAttribL1dv );
         bindFunction( PFNGLVERTEXATTRIBL2DPROC,                             glVertexAttribL2d );
         bindFunction( PFNGLVERTEXATTRIBL2DVPROC,                            glVertexAttribL2dv );
         bindFunction( PFNGLVERTEXATTRIBL3DPROC,                             glVertexAttribL3d );
         bindFunction( PFNGLVERTEXATTRIBL3DVPROC,                            glVertexAttribL3dv );
         bindFunction( PFNGLVERTEXATTRIBL4DPROC,                             glVertexAttribL4d );
         bindFunction( PFNGLVERTEXATTRIBL4DVPROC,                            glVertexAttribL4dv );
         bindFunction( PFNGLVERTEXATTRIBLPOINTERPROC,                        glVertexAttribLPointer );
         }
      // TODO: Wait until bug resolved by Khronos
      //
      //if ( ( GpuContext.screen.support(OpenGL_4_1) || 
      //       GpuContext.support.extension(ARB_vertex_attrib_64bit) ) &&
      //     GpuContext.support.extension(EXT_direct_state_access_memory) )
      //   {
      //   bindFunction( PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC,   glVertexArrayVertexAttribLOffsetEXT );
      //   }
      if ( GpuContext.screen.support(OpenGL_4_1) || 
           GpuContext.support.extension(ARB_viewport_array) )
         {
         bindFunction( PFNGLDEPTHRANGEARRAYVPROC,                            glDepthRangeArrayv );
         bindFunction( PFNGLDEPTHRANGEINDEXEDPROC,                           glDepthRangeIndexed );
         bindFunction( PFNGLGETDOUBLEI_VPROC,                                glGetDoublei_v );
         bindFunction( PFNGLGETFLOATI_VPROC,                                 glGetFloati_v );
         bindFunction( PFNGLSCISSORARRAYVPROC,                               glScissorArrayv );
         bindFunction( PFNGLSCISSORINDEXEDPROC,                              glScissorIndexed );
         bindFunction( PFNGLSCISSORINDEXEDVPROC,                             glScissorIndexedv );
         bindFunction( PFNGLVIEWPORTARRAYVPROC,                              glViewportArrayv );
         bindFunction( PFNGLVIEWPORTINDEXEDFPROC,                            glViewportIndexedf );
         bindFunction( PFNGLVIEWPORTINDEXEDFVPROC,                           glViewportIndexedfv );
         }                                                                   
      //                                                                     
      // GL_ARB_debug_output                                                 
      //                                                                     
      if ( GpuContext.support.extension(ARB_debug_output) )                         
         {                                                                   
         bindFunction( PFNGLDEBUGMESSAGECALLBACKARBPROC,                     glDebugMessageCallbackARB );
         bindFunction( PFNGLDEBUGMESSAGECONTROLARBPROC,                      glDebugMessageControlARB );
         bindFunction( PFNGLDEBUGMESSAGEINSERTARBPROC,                       glDebugMessageInsertARB );
         bindFunction( PFNGLGETDEBUGMESSAGELOGARBPROC,                       glGetDebugMessageLogARB );
         }                                                                   
      //                                                                     
      // OpenGL 4.0                                                          
      //                                                                     
      //   GLSL 4.00                                                         
      //                                                                     
      //   GL_ARB_draw_buffers_blend                                         
      //   GL_ARB_draw_indirect                                              
      //   GL_ARB_gpu_shader5                                                
      //   GL_ARB_gpu_shader_fp64                                            
      //   GL_ARB_sample_shading                                             
      //   GL_ARB_shader_subroutine                                          
      //   GL_ARB_tessellation_shader                                        
      //   GL_ARB_transform_feedback2                                        
      //   GL_ARB_texture_buffer_object_rgb32                                
      //   GL_ARB_texture_cube_map_array                                     
      //   GL_ARB_texture_gather                                             
      //   GL_ARB_texture_query_lod                                          
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_4_0) )                                      
         {                                                                   
         bindFunction( PFNGLBLENDEQUATIONSEPARATEIPROC,                      glBlendEquationSeparatei );
         bindFunction( PFNGLBLENDEQUATIONIPROC,                              glBlendEquationi );
         bindFunction( PFNGLBLENDFUNCSEPARATEIPROC,                          glBlendFuncSeparatei );
         bindFunction( PFNGLBLENDFUNCIPROC,                                  glBlendFunci );
         bindFunction( PFNGLMINSAMPLESHADINGPROC,                            glMinSampleShading );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_0) ||                                     
           GpuContext.support.extension(ARB_draw_indirect) )                        
         {                                                                   
         bindFunction( PFNGLDRAWARRAYSINDIRECTPROC,                          glDrawArraysIndirect );
         bindFunction( PFNGLDRAWELEMENTSINDIRECTPROC,                        glDrawElementsIndirect );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_0) ||                                     
           GpuContext.support.extension(ARB_gpu_shader_fp64) )                      
         {                                                                   
         bindFunction( PFNGLGETUNIFORMDVPROC,                                glGetUniformdv );
         bindFunction( PFNGLUNIFORM1DPROC,                                   glUniform1d );
         bindFunction( PFNGLUNIFORM1DVPROC,                                  glUniform1dv );
         bindFunction( PFNGLUNIFORM2DPROC,                                   glUniform2d );
         bindFunction( PFNGLUNIFORM2DVPROC,                                  glUniform2dv );
         bindFunction( PFNGLUNIFORM3DPROC,                                   glUniform3d );
         bindFunction( PFNGLUNIFORM3DVPROC,                                  glUniform3dv );
         bindFunction( PFNGLUNIFORM4DPROC,                                   glUniform4d );
         bindFunction( PFNGLUNIFORM4DVPROC,                                  glUniform4dv );
         bindFunction( PFNGLUNIFORMMATRIX2DVPROC,                            glUniformMatrix2dv );
         bindFunction( PFNGLUNIFORMMATRIX3DVPROC,                            glUniformMatrix3dv );
         bindFunction( PFNGLUNIFORMMATRIX4DVPROC,                            glUniformMatrix4dv );
         bindFunction( PFNGLUNIFORMMATRIX2X3DVPROC,                          glUniformMatrix2x3dv );
         bindFunction( PFNGLUNIFORMMATRIX2X4DVPROC,                          glUniformMatrix2x4dv );
         bindFunction( PFNGLUNIFORMMATRIX3X2DVPROC,                          glUniformMatrix3x2dv );
         bindFunction( PFNGLUNIFORMMATRIX3X4DVPROC,                          glUniformMatrix3x4dv );
         bindFunction( PFNGLUNIFORMMATRIX4X2DVPROC,                          glUniformMatrix4x2dv );
         bindFunction( PFNGLUNIFORMMATRIX4X3DVPROC,                          glUniformMatrix4x3dv );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_0) ||                                     
           GpuContext.support.extension(ARB_shader_subroutine) )                    
         {                                                                   
         bindFunction( PFNGLGETACTIVESUBROUTINENAMEPROC,                     glGetActiveSubroutineName );
         bindFunction( PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC,                glGetActiveSubroutineUniformiv );
         bindFunction( PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC,              glGetActiveSubroutineUniformName );
         bindFunction( PFNGLGETPROGRAMSTAGEIVPROC,                           glGetProgramStageiv );
         bindFunction( PFNGLGETSUBROUTINEINDEXPROC,                          glGetSubroutineIndex );
         bindFunction( PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC,                glGetSubroutineUniformLocation );
         bindFunction( PFNGLGETUNIFORMSUBROUTINEUIVPROC,                     glGetUniformSubroutineuiv );
         bindFunction( PFNGLUNIFORMSUBROUTINESUIVPROC,                       glUniformSubroutinesuiv );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_0) ||                                     
           GpuContext.support.extension(ARB_tessellation_shader) )                  
         {                                                                   
         bindFunction( PFNGLPATCHPARAMETERIPROC,                             glPatchParameteri );
         bindFunction( PFNGLPATCHPARAMETERFVPROC,                            glPatchParameterfv );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_4_0) ||                                     
           GpuContext.support.extension(ARB_transform_feedback2) )                  
         {                                                                   
         bindFunction( PFNGLBINDTRANSFORMFEEDBACKPROC,                       glBindTransformFeedback );
         bindFunction( PFNGLDELETETRANSFORMFEEDBACKSPROC,                    glDeleteTransformFeedbacks );
         bindFunction( PFNGLDRAWTRANSFORMFEEDBACKPROC,                       glDrawTransformFeedback );
         bindFunction( PFNGLGENTRANSFORMFEEDBACKSPROC,                       glGenTransformFeedbacks );
         bindFunction( PFNGLISTRANSFORMFEEDBACKPROC,                         glIsTransformFeedback );
         bindFunction( PFNGLPAUSETRANSFORMFEEDBACKPROC,                      glPauseTransformFeedback );
         bindFunction( PFNGLRESUMETRANSFORMFEEDBACKPROC,                     glResumeTransformFeedback );
         }                                                                   
      //                                                                     
      // GL_ARB_draw_buffers_blend                                           
      //                                                                     
      if ( GpuContext.support.extension(ARB_draw_buffers_blend) )                   
         {                                                                   
         bindFunction( PFNGLBLENDEQUATIONIARBPROC,                           glBlendEquationiARB );
         bindFunction( PFNGLBLENDEQUATIONSEPARATEIARBPROC,                   glBlendEquationSeparateiARB );
         bindFunction( PFNGLBLENDFUNCIARBPROC,                               glBlendFunciARB );
         bindFunction( PFNGLBLENDFUNCSEPARATEIARBPROC,                       glBlendFuncSeparateiARB );
         }                                                                   
      //                                                                     
      // GL_ARB_gpu_shader_fp64                                              
      //                                                                     
      if ( GpuContext.support.extension(ARB_gpu_shader_fp64) &&                     
           GpuContext.support.extension(EXT_direct_state_access) )                  
         {                                                                   
         bindFunction( PFNGLPROGRAMUNIFORM1DEXTPROC,                         glProgramUniform1dEXT );
         bindFunction( PFNGLPROGRAMUNIFORM1DVEXTPROC,                        glProgramUniform1dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORM2DEXTPROC,                         glProgramUniform2dEXT );
         bindFunction( PFNGLPROGRAMUNIFORM2DVEXTPROC,                        glProgramUniform2dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORM3DEXTPROC,                         glProgramUniform3dEXT );
         bindFunction( PFNGLPROGRAMUNIFORM3DVEXTPROC,                        glProgramUniform3dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORM4DEXTPROC,                         glProgramUniform4dEXT );
         bindFunction( PFNGLPROGRAMUNIFORM4DVEXTPROC,                        glProgramUniform4dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC,                  glProgramUniformMatrix2dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC,                  glProgramUniformMatrix3dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC,                  glProgramUniformMatrix4dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC,                glProgramUniformMatrix2x3dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC,                glProgramUniformMatrix2x4dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC,                glProgramUniformMatrix3x2dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC,                glProgramUniformMatrix3x4dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC,                glProgramUniformMatrix4x2dvEXT );
         bindFunction( PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC,                glProgramUniformMatrix4x3dvEXT );
         }                                                                   
      //                                                                     
      // GL_ARB_sample_shading                                               
      //                                                                     
      if ( GpuContext.support.extension(ARB_sample_shading) )                       
         bindFunction( PFNGLMINSAMPLESHADINGARBPROC,                         glMinSampleShadingARB );
      //                                                                     
      // OpenGL 3.3                                                          
      //                                                                     
      //   GLSL 3.30                                                         
      //                                                                     
      //   GL_ARB_blend_func_extended                                        
      //   GL_ARB_explicit_attrib_location                                   
      //   GL_ARB_occlusion_query2                                           
      //   GL_ARB_sampler_objects                                            
      //   GL_ARB_texture_rgb10_a2ui                                         
      //   GL_ARB_texture_swizzle                                            
      //   GL_ARB_timer_query                                                
      //   GL_ARB_vertex_type_2_10_10_10_rev                                 
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_3_3) ||                                     
           GpuContext.support.extension(ARB_blend_func_extended) )                  
         {                                                                   
         bindFunction( PFNGLBINDFRAGDATALOCATIONINDEXEDPROC,                 glBindFragDataLocationIndexed );
         bindFunction( PFNGLGETFRAGDATAINDEXPROC,                            glGetFragDataIndex );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_3) ||                                     
           GpuContext.support.extension(ARB_sampler_objects) )                      
         {                                                                   
         bindFunction( PFNGLBINDSAMPLERPROC,                                 glBindSampler );
         bindFunction( PFNGLDELETESAMPLERSPROC,                              glDeleteSamplers );
         bindFunction( PFNGLGENSAMPLERSPROC,                                 glGenSamplers );
         bindFunction( PFNGLISSAMPLERPROC,                                   glIsSampler );
         bindFunction( PFNGLGETSAMPLERPARAMETERIVPROC,                       glGetSamplerParameteriv );
         bindFunction( PFNGLGETSAMPLERPARAMETERFVPROC,                       glGetSamplerParameterfv );
         bindFunction( PFNGLGETSAMPLERPARAMETERIIVPROC,                      glGetSamplerParameterIiv );
         bindFunction( PFNGLGETSAMPLERPARAMETERIUIVPROC,                     glGetSamplerParameterIuiv );
         bindFunction( PFNGLSAMPLERPARAMETERIPROC,                           glSamplerParameteri );
         bindFunction( PFNGLSAMPLERPARAMETERFPROC,                           glSamplerParameterf );
         bindFunction( PFNGLSAMPLERPARAMETERIVPROC,                          glSamplerParameteriv );
         bindFunction( PFNGLSAMPLERPARAMETERFVPROC,                          glSamplerParameterfv );
         bindFunction( PFNGLSAMPLERPARAMETERIIVPROC,                         glSamplerParameterIiv );
         bindFunction( PFNGLSAMPLERPARAMETERIUIVPROC,                        glSamplerParameterIuiv );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_3) ||                                     
           GpuContext.support.extension(ARB_timer_query) )                          
         {                                                                   
         bindFunction( PFNGLGETQUERYOBJECTI64VPROC,                          glGetQueryObjecti64v );
         bindFunction( PFNGLGETQUERYOBJECTUI64VPROC,                         glGetQueryObjectui64v );
         bindFunction( PFNGLQUERYCOUNTERPROC,                                glQueryCounter );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_3) ||                                     
           GpuContext.support.extension(ARB_vertex_type_2_10_10_10_rev) )           
         {                                                                   
         bindFunction( PFNGLVERTEXATTRIBP1UIPROC,                            glVertexAttribP1ui );
         bindFunction( PFNGLVERTEXATTRIBP1UIVPROC,                           glVertexAttribP1uiv );
         bindFunction( PFNGLVERTEXATTRIBP2UIPROC,                            glVertexAttribP2ui );
         bindFunction( PFNGLVERTEXATTRIBP2UIVPROC,                           glVertexAttribP2uiv );
         bindFunction( PFNGLVERTEXATTRIBP3UIPROC,                            glVertexAttribP3ui );
         bindFunction( PFNGLVERTEXATTRIBP3UIVPROC,                           glVertexAttribP3uiv );
         bindFunction( PFNGLVERTEXATTRIBP4UIPROC,                            glVertexAttribP4ui );
         bindFunction( PFNGLVERTEXATTRIBP4UIVPROC,                           glVertexAttribP4uiv );
         }                                                                   
      //                                                                     
      // OpenGL 3.2                                                          
      //                                                                     
      //   GLSL 1.50                                                         
      //                                                                     
      //   GL_ARB_draw_elements_base_vertex                                  
      //   GL_ARB_fragment_coord_conventions                                 
      //   GL_ARB_geometry_shader4                                           
      //   GL_ARB_provoking_vertex                                           
      //   GL_ARB_seamless_cube_map                                          
      //   GL_ARB_sync                                                       
      //   GL_ARB_texture_multisample                                        
      //   GL_ARB_vertex_array_bgra                                          
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_3_2) )                                      
         {                                                                   
         bindFunction( PFNGLFRAMEBUFFERTEXTUREPROC,                          glFramebufferTexture );
         bindFunction( PFNGLGETBUFFERPARAMETERI64VPROC,                      glGetBufferParameteri64v );
         bindFunction( PFNGLGETINTEGER64I_VPROC,                             glGetInteger64i_v );
         bindFunction( PFNGLPROGRAMPARAMETERIPROC,                           glProgramParameteri );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_2) ||                                     
           GpuContext.support.extension(ARB_draw_elements_base_vertex) )            
         {                                                                   
         bindFunction( PFNGLDRAWELEMENTSBASEVERTEXPROC,                      glDrawElementsBaseVertex );
         bindFunction( PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC,             glDrawElementsInstancedBaseVertex );
         bindFunction( PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC,                 glDrawRangeElementsBaseVertex );
         bindFunction( PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC,                 glMultiDrawElementsBaseVertex );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_2) ||                                     
           GpuContext.support.extension(ARB_provoking_vertex) )                     
         bindFunction( PFNGLPROVOKINGVERTEXPROC,                             glProvokingVertex );
      if ( GpuContext.screen.support(OpenGL_3_2) ||                                     
           GpuContext.support.extension(ARB_sync) )                                 
         {                                                                   
         bindFunction( PFNGLCLIENTWAITSYNCPROC,                              glClientWaitSync );
         bindFunction( PFNGLDELETESYNCPROC,                                  glDeleteSync );
         bindFunction( PFNGLFENCESYNCPROC,                                   glFenceSync );
         bindFunction( PFNGLISSYNCPROC,                                      glIsSync );
         bindFunction( PFNGLGETINTEGER64VPROC,                               glGetInteger64v );
         bindFunction( PFNGLGETSYNCIVPROC,                                   glGetSynciv );
         bindFunction( PFNGLWAITSYNCPROC,                                    glWaitSync );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_2) ||                                     
           GpuContext.support.extension(ARB_texture_multisample) )                  
         {                                                                   
         bindFunction( PFNGLGETMULTISAMPLEFVPROC,                            glGetMultisamplefv );
         bindFunction( PFNGLSAMPLEMASKIPROC,                                 glSampleMaski );
         bindFunction( PFNGLTEXIMAGE2DMULTISAMPLEPROC,                       glTexImage2DMultisample );
         bindFunction( PFNGLTEXIMAGE3DMULTISAMPLEPROC,                       glTexImage3DMultisample );
         }                                                                   
      //                                                                                             
      // GL_ARB_geometry_shader4                                                                     
      //                                                                     
      if ( GpuContext.support.extension(ARB_geometry_shader4) )                     
         {                                                                                           
         bindFunction( PFNGLPROGRAMPARAMETERIARBPROC,                        glProgramParameteriARB );
         bindFunction( PFNGLFRAMEBUFFERTEXTUREARBPROC,                       glFramebufferTextureARB );
         bindFunction( PFNGLFRAMEBUFFERTEXTURELAYERARBPROC,                  glFramebufferTextureLayerARB );
         bindFunction( PFNGLFRAMEBUFFERTEXTUREFACEARBPROC,                   glFramebufferTextureFaceARB );
         }                                                                   
      //                                                                     
      // OpenGL 3.1                                                          
      //                                                                     
      //   GLSL 1.40                                                         
      //                                                                     
      //   GL_ARB_copy_buffer                                                
      //   GL_ARB_draw_instanced                                             
      //   GL_ARB_texture_buffer_object                                      
      //   GL_ARB_texture_rectangle                                          
      //   GL_ARB_texture_snorm                                              
      //   GL_ARB_uniform_buffer_object                                      
      //   GL_NV_primitive_restart                                           
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_3_1) )                                      
         {                                                                   
         bindFunction( PFNGLDRAWARRAYSINSTANCEDPROC,                         glDrawArraysInstanced );
         bindFunction( PFNGLDRAWELEMENTSINSTANCEDPROC,                       glDrawElementsInstanced );
         bindFunction( PFNGLTEXBUFFERPROC,                                   glTexBuffer );
         bindFunction( PFNGLPRIMITIVERESTARTINDEXPROC,                       glPrimitiveRestartIndex );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_1) ||                                     
           GpuContext.support.extension(ARB_copy_buffer) )                          
         bindFunction( PFNGLCOPYBUFFERSUBDATAPROC,                           glCopyBufferSubData );
      if ( GpuContext.screen.support(OpenGL_3_1) ||                                     
           GpuContext.support.extension(ARB_uniform_buffer_object) )                
         {                                                                   
         bindFunction( PFNGLGETACTIVEUNIFORMBLOCKIVPROC,                     glGetActiveUniformBlockiv );
         bindFunction( PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC,                   glGetActiveUniformBlockName );
         bindFunction( PFNGLGETACTIVEUNIFORMSIVPROC,                         glGetActiveUniformsiv );
         bindFunction( PFNGLGETACTIVEUNIFORMNAMEPROC,                        glGetActiveUniformName );
         bindFunction( PFNGLGETUNIFORMBLOCKINDEXPROC,                        glGetUniformBlockIndex );
         bindFunction( PFNGLGETUNIFORMINDICESPROC,                           glGetUniformIndices );
         bindFunction( PFNGLUNIFORMBLOCKBINDINGPROC,                         glUniformBlockBinding );
         }                                                                   
      //                                                                                             
      // GL_ARB_draw_instanced                                                                       
      //                                                                     
      if ( GpuContext.support.extension(ARB_draw_instanced) )                       
         {                                                                                          
         bindFunction( PFNGLDRAWARRAYSINSTANCEDARBPROC,                      glDrawArraysInstancedARB );
         bindFunction( PFNGLDRAWELEMENTSINSTANCEDARBPROC,                    glDrawElementsInstancedARB );
         }                                                                   
      //                                                                                             
      // GL_ARB_texture_buffer_object                                                                
      //                                                                     
      if ( GpuContext.support.extension(ARB_texture_buffer_object) )                                                                                         
         bindFunction( PFNGLTEXBUFFERARBPROC,                                glTexBufferARB );
      //                                                                     
      // GL_NV_primitive_restart                                             
      //                                                                     
      if ( GpuContext.support.extension(NV_primitive_restart) )                     
         {                                                                   
         bindFunction( PFNGLPRIMITIVERESTARTNVPROC,                          glPrimitiveRestartNV );
         bindFunction( PFNGLPRIMITIVERESTARTINDEXNVPROC,                     glPrimitiveRestartIndexNV );
         }
      //
      // OpenGL 3.0
      //
      //   GLSL 1.30
      //
      //   GL_ARB_color_buffer_float
      //   GL_ARB_depth_buffer_float
      //   GL_ARB_framebuffer_object
      //   GL_ARB_framebuffer_sRGB
      //   GL_ARB_half_float_pixel
      //   GL_ARB_map_buffer_range           or    GL_APPLE_flush_buffer_range 
      //   GL_ARB_texture_compression_rgtc
      //   GL_ARB_texture_float
      //   GL_ARB_texture_rg
      //   GL_ARB_vertex_array_object        or    GL_APPLE_vertex_array_object
      //   GL_EXT_draw_buffers2
      //   GL_EXT_framebuffer_blit
      //   GL_EXT_framebuffer_multisample
      //   GL_EXT_gpu_shader4
      //   GL_EXT_packed_depth_stencil
      //   GL_EXT_packed_float
      //   GL_EXT_texture_array
      //   GL_EXT_texture_integer
      //   GL_EXT_texture_shared_exponent
      //   GL_EXT_transform_feedback         or    GL_NV_transform_feedback
      //   GL_NV_conditional_render
      //   GL_NV_half_float    
      //
      if ( GpuContext.screen.support(OpenGL_3_0) )
         {
         bindFunction( PFNGLBEGINCONDITIONALRENDERPROC,                      glBeginConditionalRender );
         bindFunction( PFNGLBEGINTRANSFORMFEEDBACKPROC,                      glBeginTransformFeedback );
         bindFunction( PFNGLBINDBUFFERBASEPROC,                              glBindBufferBase );
         bindFunction( PFNGLBINDBUFFERRANGEPROC,                             glBindBufferRange );
         bindFunction( PFNGLBINDFRAGDATALOCATIONPROC,                        glBindFragDataLocation );
         bindFunction( PFNGLCLAMPCOLORPROC,                                  glClampColor );
         bindFunction( PFNGLCLEARBUFFERFIPROC,                               glClearBufferfi );
         bindFunction( PFNGLCLEARBUFFERFVPROC,                               glClearBufferfv );
         bindFunction( PFNGLCLEARBUFFERIVPROC,                               glClearBufferiv );
         bindFunction( PFNGLCLEARBUFFERUIVPROC,                              glClearBufferuiv );
         bindFunction( PFNGLCOLORMASKIPROC,                                  glColorMaski );
         bindFunction( PFNGLDISABLEIPROC,                                    glDisablei );
         bindFunction( PFNGLENABLEIPROC,                                     glEnablei );
         bindFunction( PFNGLENDCONDITIONALRENDERPROC,                        glEndConditionalRender );
         bindFunction( PFNGLENDTRANSFORMFEEDBACKPROC,                        glEndTransformFeedback );
         bindFunction( PFNGLGENVERTEXARRAYSPROC,                             glGenVertexArrays );
         bindFunction( PFNGLGETBOOLEANI_VPROC,                               glGetBooleani_v );
         bindFunction( PFNGLGETFRAGDATALOCATIONPROC,                         glGetFragDataLocation );
         bindFunction( PFNGLGETINTEGERI_VPROC,                               glGetIntegeri_v );
         bindFunction( PFNGLGETSTRINGIPROC,                                  glGetStringi );
         bindFunction( PFNGLGETTEXPARAMETERIIVPROC,                          glGetTexParameterIiv );
         bindFunction( PFNGLGETTEXPARAMETERIUIVPROC,                         glGetTexParameterIuiv );
         bindFunction( PFNGLGETTRANSFORMFEEDBACKVARYINGPROC,                 glGetTransformFeedbackVarying );
         bindFunction( PFNGLGETUNIFORMUIVPROC,                               glGetUniformuiv );
         bindFunction( PFNGLGETVERTEXATTRIBIIVPROC,                          glGetVertexAttribIiv );
         bindFunction( PFNGLGETVERTEXATTRIBIUIVPROC,                         glGetVertexAttribIuiv );
         bindFunction( PFNGLISENABLEDIPROC,                                  glIsEnabledi );
         bindFunction( PFNGLTEXPARAMETERIIVPROC,                             glTexParameterIiv );
         bindFunction( PFNGLTEXPARAMETERIUIVPROC,                            glTexParameterIuiv );
         bindFunction( PFNGLTRANSFORMFEEDBACKVARYINGSPROC,                   glTransformFeedbackVaryings );
         bindFunction( PFNGLUNIFORM1UIPROC,                                  glUniform1ui );
         bindFunction( PFNGLUNIFORM1UIVPROC,                                 glUniform1uiv );
         bindFunction( PFNGLUNIFORM2UIPROC,                                  glUniform2ui );
         bindFunction( PFNGLUNIFORM2UIVPROC,                                 glUniform2uiv );
         bindFunction( PFNGLUNIFORM3UIPROC,                                  glUniform3ui );
         bindFunction( PFNGLUNIFORM3UIVPROC,                                 glUniform3uiv );
         bindFunction( PFNGLUNIFORM4UIPROC,                                  glUniform4ui );
         bindFunction( PFNGLUNIFORM4UIVPROC,                                 glUniform4uiv );
         bindFunction( PFNGLVERTEXATTRIBI1IPROC,                             glVertexAttribI1i );
         bindFunction( PFNGLVERTEXATTRIBI1IVPROC,                            glVertexAttribI1iv );
         bindFunction( PFNGLVERTEXATTRIBI1UIPROC,                            glVertexAttribI1ui );
         bindFunction( PFNGLVERTEXATTRIBI1UIVPROC,                           glVertexAttribI1uiv );
         bindFunction( PFNGLVERTEXATTRIBI2IPROC,                             glVertexAttribI2i );
         bindFunction( PFNGLVERTEXATTRIBI2IVPROC,                            glVertexAttribI2iv );
         bindFunction( PFNGLVERTEXATTRIBI2UIPROC,                            glVertexAttribI2ui );
         bindFunction( PFNGLVERTEXATTRIBI2UIVPROC,                           glVertexAttribI2uiv );
         bindFunction( PFNGLVERTEXATTRIBI3IPROC,                             glVertexAttribI3i );
         bindFunction( PFNGLVERTEXATTRIBI3IVPROC,                            glVertexAttribI3iv );
         bindFunction( PFNGLVERTEXATTRIBI3UIPROC,                            glVertexAttribI3ui );
         bindFunction( PFNGLVERTEXATTRIBI3UIVPROC,                           glVertexAttribI3uiv );
         bindFunction( PFNGLVERTEXATTRIBI4BVPROC,                            glVertexAttribI4bv );
         bindFunction( PFNGLVERTEXATTRIBI4IPROC,                             glVertexAttribI4i );
         bindFunction( PFNGLVERTEXATTRIBI4IVPROC,                            glVertexAttribI4iv );
         bindFunction( PFNGLVERTEXATTRIBI4SVPROC,                            glVertexAttribI4sv );
         bindFunction( PFNGLVERTEXATTRIBI4UBVPROC,                           glVertexAttribI4ubv );
         bindFunction( PFNGLVERTEXATTRIBI4UIPROC,                            glVertexAttribI4ui );
         bindFunction( PFNGLVERTEXATTRIBI4UIVPROC,                           glVertexAttribI4uiv );
         bindFunction( PFNGLVERTEXATTRIBI4USVPROC,                           glVertexAttribI4usv );
         bindFunction( PFNGLVERTEXATTRIBIPOINTERPROC,                        glVertexAttribIPointer );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_0) ||                                     
           GpuContext.support.extension(ARB_framebuffer_object) )                   
         {                                                                   
         bindFunction( PFNGLBINDFRAMEBUFFERPROC,                             glBindFramebuffer );
         bindFunction( PFNGLBINDRENDERBUFFERPROC,                            glBindRenderbuffer );
         bindFunction( PFNGLBLITFRAMEBUFFERPROC,                             glBlitFramebuffer );
         bindFunction( PFNGLCHECKFRAMEBUFFERSTATUSPROC,                      glCheckFramebufferStatus );
         bindFunction( PFNGLDELETEFRAMEBUFFERSPROC,                          glDeleteFramebuffers );
         bindFunction( PFNGLDELETERENDERBUFFERSPROC,                         glDeleteRenderbuffers );
         bindFunction( PFNGLFRAMEBUFFERRENDERBUFFERPROC,                     glFramebufferRenderbuffer );
         bindFunction( PFNGLFRAMEBUFFERTEXTURE1DPROC,                        glFramebufferTexture1D );
         bindFunction( PFNGLFRAMEBUFFERTEXTURE2DPROC,                        glFramebufferTexture2D );
         bindFunction( PFNGLFRAMEBUFFERTEXTURE3DPROC,                        glFramebufferTexture3D );
         bindFunction( PFNGLFRAMEBUFFERTEXTURELAYERPROC,                     glFramebufferTextureLayer );
         bindFunction( PFNGLGENERATEMIPMAPPROC,                              glGenerateMipmap );
         bindFunction( PFNGLGENFRAMEBUFFERSPROC,                             glGenFramebuffers );
         bindFunction( PFNGLGENRENDERBUFFERSPROC,                            glGenRenderbuffers );
         bindFunction( PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC,         glGetFramebufferAttachmentParameteriv );
         bindFunction( PFNGLGETRENDERBUFFERPARAMETERIVPROC,                  glGetRenderbufferParameteriv );
         bindFunction( PFNGLISFRAMEBUFFERPROC,                               glIsFramebuffer );
         bindFunction( PFNGLISRENDERBUFFERPROC,                              glIsRenderbuffer );
         bindFunction( PFNGLRENDERBUFFERSTORAGEPROC,                         glRenderbufferStorage );
         bindFunction( PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC,              glRenderbufferStorageMultisample );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_0) ||                                     
           GpuContext.support.extension(ARB_map_buffer_range) )                     
         {                                                                   
         bindFunction( PFNGLFLUSHMAPPEDBUFFERRANGEPROC,                      glFlushMappedBufferRange );
         bindFunction( PFNGLMAPBUFFERRANGEPROC,                              glMapBufferRange );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_3_0) ||                                     
           GpuContext.support.extension(ARB_vertex_array_object) )                  
         {                                                                   
         bindFunction( PFNGLBINDVERTEXARRAYPROC,                             glBindVertexArray );
         bindFunction( PFNGLDELETEVERTEXARRAYSPROC,                          glDeleteVertexArrays );
         bindFunction( PFNGLGENVERTEXARRAYSPROC,                             glGenVertexArrays );
         bindFunction( PFNGLISVERTEXARRAYPROC,                               glIsVertexArray );
         }                                                                   
      //                                                                     
      // GL_ARB_color_buffer_float                                           
      //                                                                     
      if ( GpuContext.support.extension(ARB_color_buffer_float) )                   
         bindFunction( PFNGLCLAMPCOLORARBPROC,                               glClampColorARB );
      //
      // GL_APPLE_flush_buffer_range          ( predecessor of GL_ARB_map_buffer_range )
      //
      if ( GpuContext.support.extension(APPLE_flush_buffer_range) )
         {
         bindFunction( PFNGLBUFFERPARAMETERIAPPLEPROC,                       glBufferParameteriAPPLE );
         bindFunction( PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC,                 glFlushMappedBufferRangeAPPLE );
         }
      //
      // GL_APPLE_vertex_array_object         ( predecessor of GL_ARB_vertex_array_object )
      //
      if ( GpuContext.support.extension(APPLE_vertex_array_object) )
         {
         bindFunction( PFNGLBINDVERTEXARRAYAPPLEPROC,                        glBindVertexArrayAPPLE );
         bindFunction( PFNGLDELETEVERTEXARRAYSAPPLEPROC,                     glDeleteVertexArraysAPPLE );
         bindFunction( PFNGLGENVERTEXARRAYSAPPLEPROC,                        glGenVertexArraysAPPLE );
         bindFunction( PFNGLISVERTEXARRAYAPPLEPROC,                          glIsVertexArrayAPPLE );
         }                                                                   
      //                                                                     
      // GL_EXT_draw_buffers2                                                
      //                                                                     
      if ( GpuContext.support.extension(EXT_draw_buffers2) )                        
         {                                                                   
         bindFunction( PFNGLCOLORMASKINDEXEDEXTPROC,                         glColorMaskIndexedEXT );
         bindFunction( PFNGLDISABLEINDEXEDEXTPROC,                           glDisableIndexedEXT );
         bindFunction( PFNGLENABLEINDEXEDEXTPROC,                            glEnableIndexedEXT );
         bindFunction( PFNGLGETBOOLEANINDEXEDVEXTPROC,                       glGetBooleanIndexedvEXT );
         bindFunction( PFNGLGETINTEGERINDEXEDVEXTPROC,                       glGetIntegerIndexedvEXT );
         bindFunction( PFNGLISENABLEDINDEXEDEXTPROC,                         glIsEnabledIndexedEXT );
         }                                                                   
      //                                                                     
      // GL_EXT_framebuffer_blit                                             
      //                                                                     
      if ( GpuContext.support.extension(EXT_framebuffer_blit) )                     
         bindFunction( PFNGLBLITFRAMEBUFFEREXTPROC,                          glBlitFramebufferEXT );
      //                                                                     
      // GL_EXT_framebuffer_multisample                                      
      //                                                                     
      if ( GpuContext.support.extension(EXT_framebuffer_multisample) )              
         bindFunction( PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC,           glRenderbufferStorageMultisampleEXT ); 
      //                                                                     
      // GL_EXT_gpu_shader4                                                  
      //                                                                     
      if ( GpuContext.support.extension(EXT_gpu_shader4) )                          
         {                                                                   
         bindFunction( PFNGLBINDFRAGDATALOCATIONEXTPROC,                     glBindFragDataLocationEXT );
         bindFunction( PFNGLGETFRAGDATALOCATIONEXTPROC,                      glGetFragDataLocationEXT );
         bindFunction( PFNGLGETUNIFORMUIVEXTPROC,                            glGetUniformuivEXT );
         bindFunction( PFNGLGETVERTEXATTRIBIIVEXTPROC,                       glGetVertexAttribIivEXT );
         bindFunction( PFNGLGETVERTEXATTRIBIUIVEXTPROC,                      glGetVertexAttribIuivEXT );
         bindFunction( PFNGLUNIFORM1UIEXTPROC,                               glUniform1uiEXT );
         bindFunction( PFNGLUNIFORM1UIVEXTPROC,                              glUniform1uivEXT );
         bindFunction( PFNGLUNIFORM2UIEXTPROC,                               glUniform2uiEXT );
         bindFunction( PFNGLUNIFORM2UIVEXTPROC,                              glUniform2uivEXT );
         bindFunction( PFNGLUNIFORM3UIEXTPROC,                               glUniform3uiEXT );
         bindFunction( PFNGLUNIFORM3UIVEXTPROC,                              glUniform3uivEXT );
         bindFunction( PFNGLUNIFORM4UIEXTPROC,                               glUniform4uiEXT );
         bindFunction( PFNGLUNIFORM4UIVEXTPROC,                              glUniform4uivEXT );
         bindFunction( PFNGLVERTEXATTRIBI1IEXTPROC,                          glVertexAttribI1iEXT );
         bindFunction( PFNGLVERTEXATTRIBI1IVEXTPROC,                         glVertexAttribI1ivEXT );
         bindFunction( PFNGLVERTEXATTRIBI1UIEXTPROC,                         glVertexAttribI1uiEXT );
         bindFunction( PFNGLVERTEXATTRIBI1UIVEXTPROC,                        glVertexAttribI1uivEXT );
         bindFunction( PFNGLVERTEXATTRIBI2IEXTPROC,                          glVertexAttribI2iEXT );
         bindFunction( PFNGLVERTEXATTRIBI2IVEXTPROC,                         glVertexAttribI2ivEXT );
         bindFunction( PFNGLVERTEXATTRIBI2UIEXTPROC,                         glVertexAttribI2uiEXT );
         bindFunction( PFNGLVERTEXATTRIBI2UIVEXTPROC,                        glVertexAttribI2uivEXT );
         bindFunction( PFNGLVERTEXATTRIBI3IEXTPROC,                          glVertexAttribI3iEXT );
         bindFunction( PFNGLVERTEXATTRIBI3IVEXTPROC,                         glVertexAttribI3ivEXT );
         bindFunction( PFNGLVERTEXATTRIBI3UIEXTPROC,                         glVertexAttribI3uiEXT );
         bindFunction( PFNGLVERTEXATTRIBI3UIVEXTPROC,                        glVertexAttribI3uivEXT );
         bindFunction( PFNGLVERTEXATTRIBI4BVEXTPROC,                         glVertexAttribI4bvEXT );
         bindFunction( PFNGLVERTEXATTRIBI4IEXTPROC,                          glVertexAttribI4iEXT );
         bindFunction( PFNGLVERTEXATTRIBI4IVEXTPROC,                         glVertexAttribI4ivEXT );
         bindFunction( PFNGLVERTEXATTRIBI4SVEXTPROC,                         glVertexAttribI4svEXT );
         bindFunction( PFNGLVERTEXATTRIBI4UBVEXTPROC,                        glVertexAttribI4ubvEXT );
         bindFunction( PFNGLVERTEXATTRIBI4UIEXTPROC,                         glVertexAttribI4uiEXT );
         bindFunction( PFNGLVERTEXATTRIBI4UIVEXTPROC,                        glVertexAttribI4uivEXT );
         bindFunction( PFNGLVERTEXATTRIBI4USVEXTPROC,                        glVertexAttribI4usvEXT );
         bindFunction( PFNGLVERTEXATTRIBIPOINTEREXTPROC,                     glVertexAttribIPointerEXT );
         }                                                                   
      //                                                                     
      // GL_EXT_texture_array                                                
      //                                                                     
      if ( GpuContext.support.extension(EXT_texture_array) )                        
         bindFunction( PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC,                  glFramebufferTextureLayerEXT );
      //                                                                     
      // GL_EXT_texture_integer                                              
      //                                                                     
      if ( GpuContext.support.extension(EXT_texture_integer) )                      
         {                                                                   
         bindFunction( PFNGLCLEARCOLORIIEXTPROC,                             glClearColorIiEXT );
         bindFunction( PFNGLCLEARCOLORIUIEXTPROC,                            glClearColorIuiEXT );
         bindFunction( PFNGLGETTEXPARAMETERIIVEXTPROC,                       glGetTexParameterIivEXT );
         bindFunction( PFNGLGETTEXPARAMETERIUIVEXTPROC,                      glGetTexParameterIuivEXT );
         bindFunction( PFNGLTEXPARAMETERIIVEXTPROC,                          glTexParameterIivEXT );
         bindFunction( PFNGLTEXPARAMETERIUIVEXTPROC,                         glTexParameterIuivEXT );
         }                                                                   
      //                                                                     
      // GL_EXT_transform_feedback                                           
      //                                                                     
      if ( GpuContext.support.extension(EXT_transform_feedback) )                   
         {                                                                   
         bindFunction( PFNGLBEGINTRANSFORMFEEDBACKEXTPROC,                   glBeginTransformFeedbackEXT );
         bindFunction( PFNGLBINDBUFFERBASEEXTPROC,                           glBindBufferBaseEXT );
         bindFunction( PFNGLBINDBUFFEROFFSETEXTPROC,                         glBindBufferOffsetEXT );
         bindFunction( PFNGLBINDBUFFERRANGEEXTPROC,                          glBindBufferRangeEXT );
         bindFunction( PFNGLENDTRANSFORMFEEDBACKEXTPROC,                     glEndTransformFeedbackEXT );
         bindFunction( PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC,              glGetTransformFeedbackVaryingEXT );
         bindFunction( PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC,                glTransformFeedbackVaryingsEXT );
         }
      //
      // GL_NV_transform_feedback             ( predecessor of GL_EXT_transform_feedback )
      //
      if ( GpuContext.support.extension(NV_transform_feedback) )
         {
         bindFunction( PFNGLACTIVEVARYINGNVPROC,                             glActiveVaryingNV );
         bindFunction( PFNGLBEGINTRANSFORMFEEDBACKNVPROC,                    glBeginTransformFeedbackNV );
         bindFunction( PFNGLBINDBUFFERBASENVPROC,                            glBindBufferBaseNV );
         bindFunction( PFNGLBINDBUFFEROFFSETNVPROC,                          glBindBufferOffsetNV );
         bindFunction( PFNGLBINDBUFFERRANGENVPROC,                           glBindBufferRangeNV );
         bindFunction( PFNGLENDTRANSFORMFEEDBACKNVPROC,                      glEndTransformFeedbackNV );
         bindFunction( PFNGLGETACTIVEVARYINGNVPROC,                          glGetActiveVaryingNV );
         bindFunction( PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC,               glGetTransformFeedbackVaryingNV );
         bindFunction( PFNGLGETVARYINGLOCATIONNVPROC,                        glGetVaryingLocationNV );
         bindFunction( PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC,                  glTransformFeedbackAttribsNV );
         bindFunction( PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC,                 glTransformFeedbackVaryingsNV );
         }                                                                   
      //                                                                     
      // GL_NV_conditional_render                                            
      //                                                                     
      if ( GpuContext.support.extension(NV_conditional_render) )                    
         {                                                                   
         bindFunction( PFNGLBEGINCONDITIONALRENDERNVPROC,                    glBeginConditionalRenderNV );
         bindFunction( PFNGLENDCONDITIONALRENDERNVPROC,                      glEndConditionalRenderNV );
         }                                                                   
      //                                                                     
      // OpenGL 2.1                                                          
      //                                                                     
      //   GLSL 1.20                                                         
      //                                                                     
      //   GL_ARB_pixel_buffer_object                                        
      //   GL_EXT_texture_sRGB                                               
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_2_1) )                                      
         {                                                                   
         bindFunction( PFNGLUNIFORMMATRIX2X3FVPROC,                          glUniformMatrix2x3fv );
         bindFunction( PFNGLUNIFORMMATRIX3X2FVPROC,                          glUniformMatrix3x2fv );
         bindFunction( PFNGLUNIFORMMATRIX3X4FVPROC,                          glUniformMatrix3x4fv );
         bindFunction( PFNGLUNIFORMMATRIX2X4FVPROC,                          glUniformMatrix2x4fv );
         bindFunction( PFNGLUNIFORMMATRIX4X2FVPROC,                          glUniformMatrix4x2fv );
         bindFunction( PFNGLUNIFORMMATRIX4X3FVPROC,                          glUniformMatrix4x3fv );
         }                                                                   
      //                                                                     
      // OpenGL 2.0                                                          
      //                                                                     
      //   GLSL 1.10                                                         
      //                                                                     
      //   GL_ARB_draw_buffers                                               
      //   GL_ARB_fragment_shader                                            
      //   GL_ARB_point_sprite                                               
      //   GL_ARB_shader_objects                                             
      //   GL_ARB_shading_language_100                                       
      //   GL_ARB_texture_non_power_of_two                                   
      //   GL_ARB_vertex_shader                                              
      //   GL_EXT_blend_equation_separate                                    
      //   GL_EXT_stencil_two_side                                           
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_2_0) )                                      
         {
         bindFunction( PFNGLATTACHSHADERPROC,                                glAttachShader );
         bindFunction( PFNGLBINDATTRIBLOCATIONPROC,                          glBindAttribLocation );
         bindFunction( PFNGLCOMPILESHADERPROC,                               glCompileShader );
         bindFunction( PFNGLCREATEPROGRAMPROC,                               glCreateProgram );
         bindFunction( PFNGLCREATESHADERPROC,                                glCreateShader );
         bindFunction( PFNGLDELETEPROGRAMPROC,                               glDeleteProgram );
         bindFunction( PFNGLDELETESHADERPROC,                                glDeleteShader );
         bindFunction( PFNGLDETACHSHADERPROC,                                glDetachShader );
         bindFunction( PFNGLDISABLEVERTEXATTRIBARRAYPROC,                    glDisableVertexAttribArray );
         bindFunction( PFNGLDRAWBUFFERSPROC,                                 glDrawBuffers );
         bindFunction( PFNGLENABLEVERTEXATTRIBARRAYPROC,                     glEnableVertexAttribArray );
         bindFunction( PFNGLGETACTIVEATTRIBPROC,                             glGetActiveAttrib );
         bindFunction( PFNGLGETACTIVEUNIFORMPROC,                            glGetActiveUniform );
         bindFunction( PFNGLGETATTACHEDSHADERSPROC,                          glGetAttachedShaders );
         bindFunction( PFNGLGETATTRIBLOCATIONPROC,                           glGetAttribLocation );
         bindFunction( PFNGLGETPROGRAMIVPROC,                                glGetProgramiv );
         bindFunction( PFNGLGETPROGRAMINFOLOGPROC,                           glGetProgramInfoLog );
         bindFunction( PFNGLGETSHADERSOURCEPROC,                             glGetShaderSource );
         bindFunction( PFNGLGETSHADERIVPROC,                                 glGetShaderiv );
         bindFunction( PFNGLGETSHADERINFOLOGPROC,                            glGetShaderInfoLog );
         bindFunction( PFNGLGETUNIFORMLOCATIONPROC,                          glGetUniformLocation );
         bindFunction( PFNGLGETUNIFORMFVPROC,                                glGetUniformfv );
         bindFunction( PFNGLGETUNIFORMIVPROC,                                glGetUniformiv );
         bindFunction( PFNGLGETVERTEXATTRIBDVPROC,                           glGetVertexAttribdv );
         bindFunction( PFNGLGETVERTEXATTRIBFVPROC,                           glGetVertexAttribfv );
         bindFunction( PFNGLGETVERTEXATTRIBIVPROC,                           glGetVertexAttribiv );
         bindFunction( PFNGLGETVERTEXATTRIBPOINTERVPROC,                     glGetVertexAttribPointerv );
         bindFunction( PFNGLISPROGRAMPROC,                                   glIsProgram );
         bindFunction( PFNGLISSHADERPROC,                                    glIsShader );
         bindFunction( PFNGLLINKPROGRAMPROC,                                 glLinkProgram );
         bindFunction( PFNGLSHADERSOURCEPROC,                                glShaderSource );
         bindFunction( PFNGLSTENCILFUNCSEPARATEPROC,                         glStencilFuncSeparate );
         bindFunction( PFNGLSTENCILMASKSEPARATEPROC,                         glStencilMaskSeparate );
         bindFunction( PFNGLSTENCILOPSEPARATEPROC,                           glStencilOpSeparate );
         bindFunction( PFNGLUNIFORM1FPROC,                                   glUniform1f );
         bindFunction( PFNGLUNIFORM1FVPROC,                                  glUniform1fv );
         bindFunction( PFNGLUNIFORM1IPROC,                                   glUniform1i );
         bindFunction( PFNGLUNIFORM1IVPROC,                                  glUniform1iv );
         bindFunction( PFNGLUNIFORM2FPROC,                                   glUniform2f );
         bindFunction( PFNGLUNIFORM2FVPROC,                                  glUniform2fv );
         bindFunction( PFNGLUNIFORM2IPROC,                                   glUniform2i );
         bindFunction( PFNGLUNIFORM2IVPROC,                                  glUniform2iv );
         bindFunction( PFNGLUNIFORM3FPROC,                                   glUniform3f );
         bindFunction( PFNGLUNIFORM3FVPROC,                                  glUniform3fv );
         bindFunction( PFNGLUNIFORM3IPROC,                                   glUniform3i );
         bindFunction( PFNGLUNIFORM3IVPROC,                                  glUniform3iv );
         bindFunction( PFNGLUNIFORM4FPROC,                                   glUniform4f );
         bindFunction( PFNGLUNIFORM4FVPROC,                                  glUniform4fv );
         bindFunction( PFNGLUNIFORM4IPROC,                                   glUniform4i );
         bindFunction( PFNGLUNIFORM4IVPROC,                                  glUniform4iv );
         bindFunction( PFNGLUNIFORMMATRIX2FVPROC,                            glUniformMatrix2fv );
         bindFunction( PFNGLUNIFORMMATRIX3FVPROC,                            glUniformMatrix3fv );
         bindFunction( PFNGLUNIFORMMATRIX4FVPROC,                            glUniformMatrix4fv );
         bindFunction( PFNGLUSEPROGRAMPROC,                                  glUseProgram );
         bindFunction( PFNGLVALIDATEPROGRAMPROC,                             glValidateProgram );
         bindFunction( PFNGLVERTEXATTRIB1DPROC,                              glVertexAttrib1d );
         bindFunction( PFNGLVERTEXATTRIB1DVPROC,                             glVertexAttrib1dv );
         bindFunction( PFNGLVERTEXATTRIB1FPROC,                              glVertexAttrib1f );
         bindFunction( PFNGLVERTEXATTRIB1FVPROC,                             glVertexAttrib1fv );
         bindFunction( PFNGLVERTEXATTRIB1SPROC,                              glVertexAttrib1s );
         bindFunction( PFNGLVERTEXATTRIB1SVPROC,                             glVertexAttrib1sv );
         bindFunction( PFNGLVERTEXATTRIB2DPROC,                              glVertexAttrib2d );
         bindFunction( PFNGLVERTEXATTRIB2DVPROC,                             glVertexAttrib2dv );
         bindFunction( PFNGLVERTEXATTRIB2FPROC,                              glVertexAttrib2f );
         bindFunction( PFNGLVERTEXATTRIB2FVPROC,                             glVertexAttrib2fv );
         bindFunction( PFNGLVERTEXATTRIB2SPROC,                              glVertexAttrib2s );
         bindFunction( PFNGLVERTEXATTRIB2SVPROC,                             glVertexAttrib2sv );
         bindFunction( PFNGLVERTEXATTRIB3DPROC,                              glVertexAttrib3d );
         bindFunction( PFNGLVERTEXATTRIB3DVPROC,                             glVertexAttrib3dv );
         bindFunction( PFNGLVERTEXATTRIB3FPROC,                              glVertexAttrib3f );
         bindFunction( PFNGLVERTEXATTRIB3FVPROC,                             glVertexAttrib3fv );
         bindFunction( PFNGLVERTEXATTRIB3SPROC,                              glVertexAttrib3s );
         bindFunction( PFNGLVERTEXATTRIB3SVPROC,                             glVertexAttrib3sv );
         bindFunction( PFNGLVERTEXATTRIB4NBVPROC,                            glVertexAttrib4Nbv );
         bindFunction( PFNGLVERTEXATTRIB4NIVPROC,                            glVertexAttrib4Niv );
         bindFunction( PFNGLVERTEXATTRIB4NSVPROC,                            glVertexAttrib4Nsv );
         bindFunction( PFNGLVERTEXATTRIB4NUBPROC,                            glVertexAttrib4Nub );
         bindFunction( PFNGLVERTEXATTRIB4NUBVPROC,                           glVertexAttrib4Nubv );
         bindFunction( PFNGLVERTEXATTRIB4NUIVPROC,                           glVertexAttrib4Nuiv );
         bindFunction( PFNGLVERTEXATTRIB4NUSVPROC,                           glVertexAttrib4Nusv );
         bindFunction( PFNGLVERTEXATTRIB4BVPROC,                             glVertexAttrib4bv );
         bindFunction( PFNGLVERTEXATTRIB4DPROC,                              glVertexAttrib4d );
         bindFunction( PFNGLVERTEXATTRIB4DVPROC,                             glVertexAttrib4dv );
         bindFunction( PFNGLVERTEXATTRIB4FPROC,                              glVertexAttrib4f );
         bindFunction( PFNGLVERTEXATTRIB4FVPROC,                             glVertexAttrib4fv );
         bindFunction( PFNGLVERTEXATTRIB4IVPROC,                             glVertexAttrib4iv );
         bindFunction( PFNGLVERTEXATTRIB4SPROC,                              glVertexAttrib4s );
         bindFunction( PFNGLVERTEXATTRIB4SVPROC,                             glVertexAttrib4sv );
         bindFunction( PFNGLVERTEXATTRIB4UBVPROC,                            glVertexAttrib4ubv );
         bindFunction( PFNGLVERTEXATTRIB4UIVPROC,                            glVertexAttrib4uiv );
         bindFunction( PFNGLVERTEXATTRIB4USVPROC,                            glVertexAttrib4usv );
         bindFunction( PFNGLVERTEXATTRIBPOINTERPROC,                         glVertexAttribPointer );
         }                                                                   
      //                                                                     
      // GL_ARB_draw_buffers                                                 
      //                                                                     
      if ( GpuContext.support.extension(ARB_draw_buffers) )                         
         bindFunction( PFNGLDRAWBUFFERSARBPROC,                              glDrawBuffersARB );
      //                                                                     
      // GL_ARB_shader_objects                                               
      //                                                                     
      if ( GpuContext.support.extension(ARB_shader_objects) )                       
         {                                                                   
         bindFunction( PFNGLATTACHOBJECTARBPROC,                             glAttachObjectARB );
         bindFunction( PFNGLCOMPILESHADERARBPROC,                            glCompileShaderARB );
         bindFunction( PFNGLCREATEPROGRAMOBJECTARBPROC,                      glCreateProgramObjectARB );
         bindFunction( PFNGLCREATESHADEROBJECTARBPROC,                       glCreateShaderObjectARB );
         bindFunction( PFNGLDELETEOBJECTARBPROC,                             glDeleteObjectARB );
         bindFunction( PFNGLDETACHOBJECTARBPROC,                             glDetachObjectARB );
         bindFunction( PFNGLGETACTIVEUNIFORMARBPROC,                         glGetActiveUniformARB );
         bindFunction( PFNGLGETATTACHEDOBJECTSARBPROC,                       glGetAttachedObjectsARB );
         bindFunction( PFNGLGETHANDLEARBPROC,                                glGetHandleARB );
         bindFunction( PFNGLGETINFOLOGARBPROC,                               glGetInfoLogARB );
         bindFunction( PFNGLGETOBJECTPARAMETERFVARBPROC,                     glGetObjectParameterfvARB );
         bindFunction( PFNGLGETOBJECTPARAMETERIVARBPROC,                     glGetObjectParameterivARB );
         bindFunction( PFNGLGETUNIFORMFVARBPROC,                             glGetUniformfvARB );
         bindFunction( PFNGLGETUNIFORMIVARBPROC,                             glGetUniformivARB );
         bindFunction( PFNGLGETUNIFORMLOCATIONARBPROC,                       glGetUniformLocationARB );
         bindFunction( PFNGLGETSHADERSOURCEARBPROC,                          glGetShaderSourceARB );
         bindFunction( PFNGLLINKPROGRAMARBPROC,                              glLinkProgramARB );
         bindFunction( PFNGLSHADERSOURCEARBPROC,                             glShaderSourceARB );
         bindFunction( PFNGLUNIFORM1FARBPROC,                                glUniform1fARB );
         bindFunction( PFNGLUNIFORM1FVARBPROC,                               glUniform1fvARB );
         bindFunction( PFNGLUNIFORM1IARBPROC,                                glUniform1iARB );
         bindFunction( PFNGLUNIFORM1IVARBPROC,                               glUniform1ivARB );
         bindFunction( PFNGLUNIFORM2FARBPROC,                                glUniform2fARB );
         bindFunction( PFNGLUNIFORM2FVARBPROC,                               glUniform2fvARB );
         bindFunction( PFNGLUNIFORM2IARBPROC,                                glUniform2iARB );
         bindFunction( PFNGLUNIFORM2IVARBPROC,                               glUniform2ivARB );
         bindFunction( PFNGLUNIFORM3FARBPROC,                                glUniform3fARB );
         bindFunction( PFNGLUNIFORM3FVARBPROC,                               glUniform3fvARB );
         bindFunction( PFNGLUNIFORM3IARBPROC,                                glUniform3iARB );
         bindFunction( PFNGLUNIFORM3IVARBPROC,                               glUniform3ivARB );
         bindFunction( PFNGLUNIFORM4FARBPROC,                                glUniform4fARB );
         bindFunction( PFNGLUNIFORM4FVARBPROC,                               glUniform4fvARB );
         bindFunction( PFNGLUNIFORM4IARBPROC,                                glUniform4iARB );
         bindFunction( PFNGLUNIFORM4IVARBPROC,                               glUniform4ivARB );
         bindFunction( PFNGLUNIFORMMATRIX2FVARBPROC,                         glUniformMatrix2fvARB );
         bindFunction( PFNGLUNIFORMMATRIX3FVARBPROC,                         glUniformMatrix3fvARB );
         bindFunction( PFNGLUNIFORMMATRIX4FVARBPROC,                         glUniformMatrix4fvARB );
         bindFunction( PFNGLUSEPROGRAMOBJECTARBPROC,                         glUseProgramObjectARB );
         bindFunction( PFNGLVALIDATEPROGRAMARBPROC,                          glValidateProgramARB );
         }                                                                   
      //                                                                     
      // GL_ARB_vertex_shader                                                
      //                                                                     
      if ( GpuContext.support.extension(ARB_vertex_shader) )                        
         {                                                                   
         bindFunction( PFNGLBINDATTRIBLOCATIONARBPROC,                       glBindAttribLocationARB );
         bindFunction( PFNGLDISABLEVERTEXATTRIBARRAYARBPROC,                 glDisableVertexAttribArrayARB );
         bindFunction( PFNGLENABLEVERTEXATTRIBARRAYARBPROC,                  glEnableVertexAttribArrayARB );
         bindFunction( PFNGLGETACTIVEATTRIBARBPROC,                          glGetActiveAttribARB );
         bindFunction( PFNGLGETATTRIBLOCATIONARBPROC,                        glGetAttribLocationARB );
         bindFunction( PFNGLGETVERTEXATTRIBDVARBPROC,                        glGetVertexAttribdvARB );
         bindFunction( PFNGLGETVERTEXATTRIBFVARBPROC,                        glGetVertexAttribfvARB );
         bindFunction( PFNGLGETVERTEXATTRIBIVARBPROC,                        glGetVertexAttribivARB );
         bindFunction( PFNGLGETVERTEXATTRIBPOINTERVARBPROC,                  glGetVertexAttribPointervARB );
         bindFunction( PFNGLVERTEXATTRIB1DARBPROC,                           glVertexAttrib1dARB );
         bindFunction( PFNGLVERTEXATTRIB1DVARBPROC,                          glVertexAttrib1dvARB );
         bindFunction( PFNGLVERTEXATTRIB1FARBPROC,                           glVertexAttrib1fARB );
         bindFunction( PFNGLVERTEXATTRIB1FVARBPROC,                          glVertexAttrib1fvARB );
         bindFunction( PFNGLVERTEXATTRIB1SARBPROC,                           glVertexAttrib1sARB );
         bindFunction( PFNGLVERTEXATTRIB1SVARBPROC,                          glVertexAttrib1svARB );
         bindFunction( PFNGLVERTEXATTRIB2DARBPROC,                           glVertexAttrib2dARB );
         bindFunction( PFNGLVERTEXATTRIB2DVARBPROC,                          glVertexAttrib2dvARB );
         bindFunction( PFNGLVERTEXATTRIB2FARBPROC,                           glVertexAttrib2fARB );
         bindFunction( PFNGLVERTEXATTRIB2FVARBPROC,                          glVertexAttrib2fvARB );
         bindFunction( PFNGLVERTEXATTRIB2SARBPROC,                           glVertexAttrib2sARB );
         bindFunction( PFNGLVERTEXATTRIB2SVARBPROC,                          glVertexAttrib2svARB );
         bindFunction( PFNGLVERTEXATTRIB3DARBPROC,                           glVertexAttrib3dARB );
         bindFunction( PFNGLVERTEXATTRIB3DVARBPROC,                          glVertexAttrib3dvARB );
         bindFunction( PFNGLVERTEXATTRIB3FARBPROC,                           glVertexAttrib3fARB );
         bindFunction( PFNGLVERTEXATTRIB3FVARBPROC,                          glVertexAttrib3fvARB );
         bindFunction( PFNGLVERTEXATTRIB3SARBPROC,                           glVertexAttrib3sARB );
         bindFunction( PFNGLVERTEXATTRIB3SVARBPROC,                          glVertexAttrib3svARB );
         bindFunction( PFNGLVERTEXATTRIB4NBVARBPROC,                         glVertexAttrib4NbvARB );
         bindFunction( PFNGLVERTEXATTRIB4NIVARBPROC,                         glVertexAttrib4NivARB );
         bindFunction( PFNGLVERTEXATTRIB4NSVARBPROC,                         glVertexAttrib4NsvARB );
         bindFunction( PFNGLVERTEXATTRIB4NUBARBPROC,                         glVertexAttrib4NubARB );
         bindFunction( PFNGLVERTEXATTRIB4NUBVARBPROC,                        glVertexAttrib4NubvARB );
         bindFunction( PFNGLVERTEXATTRIB4NUIVARBPROC,                        glVertexAttrib4NuivARB );
         bindFunction( PFNGLVERTEXATTRIB4NUSVARBPROC,                        glVertexAttrib4NusvARB );
         bindFunction( PFNGLVERTEXATTRIB4BVARBPROC,                          glVertexAttrib4bvARB );
         bindFunction( PFNGLVERTEXATTRIB4DARBPROC,                           glVertexAttrib4dARB );
         bindFunction( PFNGLVERTEXATTRIB4DVARBPROC,                          glVertexAttrib4dvARB );
         bindFunction( PFNGLVERTEXATTRIB4FARBPROC,                           glVertexAttrib4fARB );
         bindFunction( PFNGLVERTEXATTRIB4FVARBPROC,                          glVertexAttrib4fvARB );
         bindFunction( PFNGLVERTEXATTRIB4IVARBPROC,                          glVertexAttrib4ivARB );
         bindFunction( PFNGLVERTEXATTRIB4SARBPROC,                           glVertexAttrib4sARB );
         bindFunction( PFNGLVERTEXATTRIB4SVARBPROC,                          glVertexAttrib4svARB );
         bindFunction( PFNGLVERTEXATTRIB4UBVARBPROC,                         glVertexAttrib4ubvARB );
         bindFunction( PFNGLVERTEXATTRIB4UIVARBPROC,                         glVertexAttrib4uivARB );
         bindFunction( PFNGLVERTEXATTRIB4USVARBPROC,                         glVertexAttrib4usvARB );
         bindFunction( PFNGLVERTEXATTRIBPOINTERARBPROC,                      glVertexAttribPointerARB );
         }                                                                   
      //                                                                     
      // GL_EXT_blend_equation_separate                                      
      //                                                                     
      if ( GpuContext.support.extension(EXT_blend_equation_separate) )              
         bindFunction( PFNGLBLENDEQUATIONSEPARATEEXTPROC,                    glBlendEquationSeparateEXT );
      //                                                                     
      // GL_EXT_stencil_two_side                                             
      //                                                                     
      if ( GpuContext.support.extension(EXT_stencil_two_side) )                     
         bindFunction( PFNGLACTIVESTENCILFACEEXTPROC,                        glActiveStencilFaceEXT );
      //                                                                     
      // OpenGL 1.5                                                          
      //                                                                     
      //   GL_ARB_occlusion_query        or    GL_HP_occlusion_test          
      //   GL_ARB_vertex_buffer_object                                       
      //   GL_EXT_shadow_funcs                                               
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_1_5) )                                      
         {                                                                   
         bindFunction( PFNGLBEGINQUERYPROC,                                  glBeginQuery );
         bindFunction( PFNGLBINDBUFFERPROC,                                  glBindBuffer );
         bindFunction( PFNGLBUFFERDATAPROC,                                  glBufferData );
         bindFunction( PFNGLBUFFERSUBDATAPROC,                               glBufferSubData );
         bindFunction( PFNGLDELETEBUFFERSPROC,                               glDeleteBuffers );
         bindFunction( PFNGLDELETEQUERIESPROC,                               glDeleteQueries );
         bindFunction( PFNGLENDQUERYPROC,                                    glEndQuery );
         bindFunction( PFNGLGENBUFFERSPROC,                                  glGenBuffers );
         bindFunction( PFNGLGENQUERIESPROC,                                  glGenQueries );
         bindFunction( PFNGLGETBUFFERPARAMETERIVPROC,                        glGetBufferParameteriv );
         bindFunction( PFNGLGETBUFFERPOINTERVPROC,                           glGetBufferPointerv );
         bindFunction( PFNGLGETBUFFERSUBDATAPROC,                            glGetBufferSubData );
         bindFunction( PFNGLGETQUERYIVPROC,                                  glGetQueryiv );
         bindFunction( PFNGLGETQUERYOBJECTIVPROC,                            glGetQueryObjectiv );
         bindFunction( PFNGLGETQUERYOBJECTUIVPROC,                           glGetQueryObjectuiv );
         bindFunction( PFNGLISBUFFERPROC,                                    glIsBuffer );
         bindFunction( PFNGLISQUERYPROC,                                     glIsQuery );
         bindFunction( PFNGLMAPBUFFERPROC,                                   glMapBuffer );
         bindFunction( PFNGLUNMAPBUFFERPROC,                                 glUnmapBuffer );
         }                                                                   
      //                                                                             
      // GL_ARB_occlusion_query                                                      
      //                                                                     
      if ( GpuContext.support.extension(ARB_occlusion_query) )                      
         {                                                                           
         bindFunction( PFNGLBEGINQUERYARBPROC,                               glBeginQueryARB );
         bindFunction( PFNGLDELETEQUERIESARBPROC,                            glDeleteQueriesARB );
         bindFunction( PFNGLENDQUERYARBPROC,                                 glEndQueryARB );
         bindFunction( PFNGLGENQUERIESARBPROC,                               glGenQueriesARB );
         bindFunction( PFNGLGETQUERYIVARBPROC,                               glGetQueryivARB );
         bindFunction( PFNGLGETQUERYOBJECTIVARBPROC,                         glGetQueryObjectivARB );
         bindFunction( PFNGLGETQUERYOBJECTUIVARBPROC,                        glGetQueryObjectuivARB );
         bindFunction( PFNGLISQUERYARBPROC,                                  glIsQueryARB );
         }                                                                   
      //                                                                             
      // GL_ARB_vertex_buffer_object                                                 
      //                                                                     
      if ( GpuContext.support.extension(ARB_vertex_buffer_object) )                 
         {                                                                            
         bindFunction( PFNGLBINDBUFFERARBPROC,                               glBindBufferARB );
         bindFunction( PFNGLBUFFERDATAARBPROC,                               glBufferDataARB );
         bindFunction( PFNGLBUFFERSUBDATAARBPROC,                            glBufferSubDataARB );
         bindFunction( PFNGLDELETEBUFFERSARBPROC,                            glDeleteBuffersARB );
         bindFunction( PFNGLGENBUFFERSARBPROC,                               glGenBuffersARB );
         bindFunction( PFNGLGETBUFFERPARAMETERIVARBPROC,                     glGetBufferParameterivARB );
         bindFunction( PFNGLGETBUFFERPOINTERVARBPROC,                        glGetBufferPointervARB );
         bindFunction( PFNGLGETBUFFERSUBDATAARBPROC,                         glGetBufferSubDataARB );
         bindFunction( PFNGLISBUFFERARBPROC,                                 glIsBufferARB );
         bindFunction( PFNGLMAPBUFFERARBPROC,                                glMapBufferARB );
         bindFunction( PFNGLUNMAPBUFFERARBPROC,                              glUnmapBufferARB );
         }                                                                   
      //                                                                     
      // OpenGL 1.4                                                          
      //                                                                     
      //   GL_ARB_depth_texture                                              
      //   GL_ARB_point_parameters                                           
      //   GL_ARB_shadow                                                     
      //   GL_ARB_texture_env_crossbar                                       
      //   GL_ARB_texture_mirrored_repeat                                    
      //   GL_ARB_window_pos                                                 
      //   GL_EXT_blend_color                                                
      //   GL_EXT_blend_func_separate                                        
      //   GL_EXT_fog_coord                                                  
      //   GL_EXT_multi_draw_arrays                                          
      //   GL_EXT_secondary_color                                            
      //   GL_EXT_stencil_wrap                                               
      //   GL_EXT_texture_lod_bias                                           
      //   GL_NV_blend_square                                                
      //   GL_SGIS_generate_mipmap                                           
      //                                                                     
      if ( GpuContext.screen.support(OpenGL_1_4) )                                      
         {                                                                   
         bindFunction( PFNGLBLENDCOLORPROC,                                  glBlendColor );
         bindFunction( PFNGLBLENDFUNCSEPARATEPROC,                           glBlendFuncSeparate );
         bindFunction( PFNGLMULTIDRAWARRAYSPROC,                             glMultiDrawArrays );
         bindFunction( PFNGLMULTIDRAWELEMENTSPROC,                           glMultiDrawElements );
         bindFunction( PFNGLPOINTPARAMETERFPROC,                             glPointParameterf );
         bindFunction( PFNGLPOINTPARAMETERFVPROC,                            glPointParameterfv );
         bindFunction( PFNGLPOINTPARAMETERIPROC,                             glPointParameteri );
         bindFunction( PFNGLPOINTPARAMETERIVPROC,                            glPointParameteriv );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_1_4) &&                                     
          !GpuContext.screen.support(OpenGL_3_0) )                                      
         {                                                                   
         bindFunction( PFNGLFOGCOORDDPROC,                                   glFogCoordd  );
         bindFunction( PFNGLFOGCOORDDVPROC,                                  glFogCoorddv );
         bindFunction( PFNGLFOGCOORDFPROC,                                   glFogCoordf );
         bindFunction( PFNGLFOGCOORDFVPROC,                                  glFogCoordfv );
         bindFunction( PFNGLFOGCOORDPOINTERPROC,                             glFogCoordPointer );
         bindFunction( PFNGLSECONDARYCOLOR3BPROC,                            glSecondaryColor3b );
         bindFunction( PFNGLSECONDARYCOLOR3BVPROC,                           glSecondaryColor3bv );
         bindFunction( PFNGLSECONDARYCOLOR3DPROC,                            glSecondaryColor3d );
         bindFunction( PFNGLSECONDARYCOLOR3DVPROC,                           glSecondaryColor3dv );
         bindFunction( PFNGLSECONDARYCOLOR3FPROC,                            glSecondaryColor3f );
         bindFunction( PFNGLSECONDARYCOLOR3FVPROC,                           glSecondaryColor3fv );
         bindFunction( PFNGLSECONDARYCOLOR3IPROC,                            glSecondaryColor3i );
         bindFunction( PFNGLSECONDARYCOLOR3IVPROC,                           glSecondaryColor3iv );
         bindFunction( PFNGLSECONDARYCOLOR3SPROC,                            glSecondaryColor3s );
         bindFunction( PFNGLSECONDARYCOLOR3SVPROC,                           glSecondaryColor3sv );
         bindFunction( PFNGLSECONDARYCOLOR3UBPROC,                           glSecondaryColor3ub );
         bindFunction( PFNGLSECONDARYCOLOR3UBVPROC,                          glSecondaryColor3ubv );
         bindFunction( PFNGLSECONDARYCOLOR3UIPROC,                           glSecondaryColor3ui );
         bindFunction( PFNGLSECONDARYCOLOR3UIVPROC,                          glSecondaryColor3uiv );
         bindFunction( PFNGLSECONDARYCOLOR3USPROC,                           glSecondaryColor3us );
         bindFunction( PFNGLSECONDARYCOLOR3USVPROC,                          glSecondaryColor3usv );
         bindFunction( PFNGLSECONDARYCOLORPOINTERPROC,                       glSecondaryColorPointer );
         bindFunction( PFNGLWINDOWPOS2DPROC,                                 glWindowPos2d );
         bindFunction( PFNGLWINDOWPOS2DVPROC,                                glWindowPos2dv );
         bindFunction( PFNGLWINDOWPOS2FPROC,                                 glWindowPos2f );
         bindFunction( PFNGLWINDOWPOS2FVPROC,                                glWindowPos2fv );
         bindFunction( PFNGLWINDOWPOS2IPROC,                                 glWindowPos2i );
         bindFunction( PFNGLWINDOWPOS2IVPROC,                                glWindowPos2iv );
         bindFunction( PFNGLWINDOWPOS2SPROC,                                 glWindowPos2s );
         bindFunction( PFNGLWINDOWPOS2SVPROC,                                glWindowPos2sv );
         bindFunction( PFNGLWINDOWPOS3DPROC,                                 glWindowPos3d );
         bindFunction( PFNGLWINDOWPOS3DVPROC,                                glWindowPos3dv );
         bindFunction( PFNGLWINDOWPOS3FPROC,                                 glWindowPos3f );
         bindFunction( PFNGLWINDOWPOS3FVPROC,                                glWindowPos3fv );
         bindFunction( PFNGLWINDOWPOS3IPROC,                                 glWindowPos3i );
         bindFunction( PFNGLWINDOWPOS3IVPROC,                                glWindowPos3iv );
         bindFunction( PFNGLWINDOWPOS3SPROC,                                 glWindowPos3s );
         bindFunction( PFNGLWINDOWPOS3SVPROC,                                glWindowPos3sv );
         }
      //
      // GL_ARB_point_parameters
      //
      if ( GpuContext.support.extension(ARB_point_parameters) )
         {
         bindFunction( PFNGLPOINTPARAMETERFARBPROC,                          glPointParameterfARB );
         bindFunction( PFNGLPOINTPARAMETERFVARBPROC,                         glPointParameterfvARB );
         }
      //
      // GL_ARB_window_pos             ( Deprecated in OpenGL 3.0 Core )
      //
      if ( GpuContext.support.extension(ARB_window_pos) &&
          !GpuContext.screen.support(OpenGL_3_0) )
         {
         bindFunction( PFNGLWINDOWPOS2DARBPROC,                              glWindowPos2dARB );
         bindFunction( PFNGLWINDOWPOS2DVARBPROC,                             glWindowPos2dvARB );
         bindFunction( PFNGLWINDOWPOS2FARBPROC,                              glWindowPos2fARB );
         bindFunction( PFNGLWINDOWPOS2FVARBPROC,                             glWindowPos2fvARB );
         bindFunction( PFNGLWINDOWPOS2IARBPROC,                              glWindowPos2iARB );
         bindFunction( PFNGLWINDOWPOS2IVARBPROC,                             glWindowPos2ivARB );
         bindFunction( PFNGLWINDOWPOS2SARBPROC,                              glWindowPos2sARB );
         bindFunction( PFNGLWINDOWPOS2SVARBPROC,                             glWindowPos2svARB );
         bindFunction( PFNGLWINDOWPOS3DARBPROC,                              glWindowPos3dARB );
         bindFunction( PFNGLWINDOWPOS3DVARBPROC,                             glWindowPos3dvARB );
         bindFunction( PFNGLWINDOWPOS3FARBPROC,                              glWindowPos3fARB );
         bindFunction( PFNGLWINDOWPOS3FVARBPROC,                             glWindowPos3fvARB );
         bindFunction( PFNGLWINDOWPOS3IARBPROC,                              glWindowPos3iARB );
         bindFunction( PFNGLWINDOWPOS3IVARBPROC,                             glWindowPos3ivARB );
         bindFunction( PFNGLWINDOWPOS3SARBPROC,                              glWindowPos3sARB );
         bindFunction( PFNGLWINDOWPOS3SVARBPROC,                             glWindowPos3svARB );
         }                                                                   
      //                                                                     
      // GL_EXT_blend_color                                                  
      //                                                                     
      if ( GpuContext.support.extension(EXT_blend_color) )                          
         bindFunction( PFNGLBLENDCOLOREXTPROC,                               glBlendColorEXT );
      //                                                                     
      // GL_EXT_blend_func_separate                                          
      //                                                                     
      if ( GpuContext.support.extension(EXT_blend_func_separate) )                  
         bindFunction( PFNGLBLENDFUNCSEPARATEEXTPROC,                        glBlendFuncSeparateEXT );
      //
      // GL_EXT_fog_coord             ( Deprecated in OpenGL 3.0 Core )
      //
      if ( GpuContext.support.extension(EXT_fog_coord) &&
          !GpuContext.screen.support(OpenGL_3_0) )
         {
         bindFunction( PFNGLFOGCOORDFEXTPROC,                                glFogCoordfEXT );
         bindFunction( PFNGLFOGCOORDFVEXTPROC,                               glFogCoordfvEXT );
         bindFunction( PFNGLFOGCOORDDEXTPROC,                                glFogCoorddEXT );
         bindFunction( PFNGLFOGCOORDDVEXTPROC,                               glFogCoorddvEXT );
         bindFunction( PFNGLFOGCOORDPOINTEREXTPROC,                          glFogCoordPointerEXT );
         }                                                                                                                            
      //                                                                     
      // GL_EXT_multi_draw_arrays                                            
      //                                                                     
      if ( GpuContext.support.extension(EXT_multi_draw_arrays) )                    
         {                                                                   
         bindFunction( PFNGLMULTIDRAWARRAYSEXTPROC,                          glMultiDrawArraysEXT );
         bindFunction( PFNGLMULTIDRAWELEMENTSEXTPROC,                        glMultiDrawElementsEXT );
         }
      //
      // GL_EXT_secondary_color       ( Deprecated in OpenGL 3.0 Core )
      //
      if ( GpuContext.support.extension(EXT_secondary_color) &&
          !GpuContext.screen.support(OpenGL_3_0) )
         {
         bindFunction( PFNGLSECONDARYCOLOR3BEXTPROC,                         glSecondaryColor3bEXT );
         bindFunction( PFNGLSECONDARYCOLOR3BVEXTPROC,                        glSecondaryColor3bvEXT );
         bindFunction( PFNGLSECONDARYCOLOR3DEXTPROC,                         glSecondaryColor3dEXT );
         bindFunction( PFNGLSECONDARYCOLOR3DVEXTPROC,                        glSecondaryColor3dvEXT );
         bindFunction( PFNGLSECONDARYCOLOR3FEXTPROC,                         glSecondaryColor3fEXT );
         bindFunction( PFNGLSECONDARYCOLOR3FVEXTPROC,                        glSecondaryColor3fvEXT );
         bindFunction( PFNGLSECONDARYCOLOR3IEXTPROC,                         glSecondaryColor3iEXT );
         bindFunction( PFNGLSECONDARYCOLOR3IVEXTPROC,                        glSecondaryColor3ivEXT );
         bindFunction( PFNGLSECONDARYCOLOR3SEXTPROC,                         glSecondaryColor3sEXT );
         bindFunction( PFNGLSECONDARYCOLOR3SVEXTPROC,                        glSecondaryColor3svEXT );
         bindFunction( PFNGLSECONDARYCOLOR3UBEXTPROC,                        glSecondaryColor3ubEXT );
         bindFunction( PFNGLSECONDARYCOLOR3UBVEXTPROC,                       glSecondaryColor3ubvEXT );
         bindFunction( PFNGLSECONDARYCOLOR3UIEXTPROC,                        glSecondaryColor3uiEXT );
         bindFunction( PFNGLSECONDARYCOLOR3UIVEXTPROC,                       glSecondaryColor3uivEXT );
         bindFunction( PFNGLSECONDARYCOLOR3USEXTPROC,                        glSecondaryColor3usEXT );
         bindFunction( PFNGLSECONDARYCOLOR3USVEXTPROC,                       glSecondaryColor3usvEXT );
         bindFunction( PFNGLSECONDARYCOLORPOINTEREXTPROC,                    glSecondaryColorPointerEXT );
         }
      //
      // OpenGL 1.3
      //
      //   GL_ARB_texture_border_clamp
      //   GL_ARB_texture_cube_map
      //   GL_ARB_texture_env_add
      //   GL_ARB_texture_env_combine
      //   GL_ARB_texture_env_dot3
      //   GL_ARB_multisample
      //   GL_ARB_multitexture
      //   GL_ARB_texture_compression
      //   GL_ARB_transpose_matrix
      //
      if ( GpuContext.screen.support(OpenGL_1_3) )
         {
         bindFunction( PFNGLACTIVETEXTUREPROC,                               glActiveTexture );
         bindFunction( PFNGLCOMPRESSEDTEXIMAGE1DPROC,                        glCompressedTexImage1D );
         bindFunction( PFNGLCOMPRESSEDTEXIMAGE2DPROC,                        glCompressedTexImage2D );
         bindFunction( PFNGLCOMPRESSEDTEXIMAGE3DPROC,                        glCompressedTexImage3D );
         bindFunction( PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC,                     glCompressedTexSubImage1D ); 
         bindFunction( PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC,                     glCompressedTexSubImage2D );
         bindFunction( PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC,                     glCompressedTexSubImage3D );
         bindFunction( PFNGLGETCOMPRESSEDTEXIMAGEPROC,                       glGetCompressedTexImage );
         bindFunction( PFNGLSAMPLECOVERAGEPROC,                              glSampleCoverage );
         }                                                                   
      if ( GpuContext.screen.support(OpenGL_1_3) &&                                     
          !GpuContext.screen.support(OpenGL_3_0) )                                      
         {                                                                   
         bindFunction( PFNGLCLIENTACTIVETEXTUREPROC,                         glClientActiveTexture );
         bindFunction( PFNGLLOADTRANSPOSEMATRIXDPROC,                        glLoadTransposeMatrixd );
         bindFunction( PFNGLLOADTRANSPOSEMATRIXFPROC,                        glLoadTransposeMatrixf );
         bindFunction( PFNGLMULTTRANSPOSEMATRIXDPROC,                        glMultTransposeMatrixd );
         bindFunction( PFNGLMULTTRANSPOSEMATRIXFPROC,                        glMultTransposeMatrixf );
         bindFunction( PFNGLMULTITEXCOORD1DPROC,                             glMultiTexCoord1d );
         bindFunction( PFNGLMULTITEXCOORD1DVPROC,                            glMultiTexCoord1dv );
         bindFunction( PFNGLMULTITEXCOORD1FPROC,                             glMultiTexCoord1f );
         bindFunction( PFNGLMULTITEXCOORD1FVPROC,                            glMultiTexCoord1fv );
         bindFunction( PFNGLMULTITEXCOORD1IPROC,                             glMultiTexCoord1i );
         bindFunction( PFNGLMULTITEXCOORD1IVPROC,                            glMultiTexCoord1iv );
         bindFunction( PFNGLMULTITEXCOORD1SPROC,                             glMultiTexCoord1s );
         bindFunction( PFNGLMULTITEXCOORD1SVPROC,                            glMultiTexCoord1sv );
         bindFunction( PFNGLMULTITEXCOORD2DPROC,                             glMultiTexCoord2d );
         bindFunction( PFNGLMULTITEXCOORD2DVPROC,                            glMultiTexCoord2dv );
         bindFunction( PFNGLMULTITEXCOORD2FPROC,                             glMultiTexCoord2f );
         bindFunction( PFNGLMULTITEXCOORD2FVPROC,                            glMultiTexCoord2fv );
         bindFunction( PFNGLMULTITEXCOORD2IPROC,                             glMultiTexCoord2i );
         bindFunction( PFNGLMULTITEXCOORD2IVPROC,                            glMultiTexCoord2iv );
         bindFunction( PFNGLMULTITEXCOORD2SPROC,                             glMultiTexCoord2s );
         bindFunction( PFNGLMULTITEXCOORD2SVPROC,                            glMultiTexCoord2sv );
         bindFunction( PFNGLMULTITEXCOORD3DPROC,                             glMultiTexCoord3d );
         bindFunction( PFNGLMULTITEXCOORD3DVPROC,                            glMultiTexCoord3dv );
         bindFunction( PFNGLMULTITEXCOORD3FPROC,                             glMultiTexCoord3f );
         bindFunction( PFNGLMULTITEXCOORD3FVPROC,                            glMultiTexCoord3fv );
         bindFunction( PFNGLMULTITEXCOORD3IPROC,                             glMultiTexCoord3i );
         bindFunction( PFNGLMULTITEXCOORD3IVPROC,                            glMultiTexCoord3iv );
         bindFunction( PFNGLMULTITEXCOORD3SPROC,                             glMultiTexCoord3s );
         bindFunction( PFNGLMULTITEXCOORD3SVPROC,                            glMultiTexCoord3sv );
         bindFunction( PFNGLMULTITEXCOORD4DPROC,                             glMultiTexCoord4d );
         bindFunction( PFNGLMULTITEXCOORD4DVPROC,                            glMultiTexCoord4dv );
         bindFunction( PFNGLMULTITEXCOORD4FPROC,                             glMultiTexCoord4f );
         bindFunction( PFNGLMULTITEXCOORD4FVPROC,                            glMultiTexCoord4fv );
         bindFunction( PFNGLMULTITEXCOORD4IPROC,                             glMultiTexCoord4i );
         bindFunction( PFNGLMULTITEXCOORD4IVPROC,                            glMultiTexCoord4iv );
         bindFunction( PFNGLMULTITEXCOORD4SPROC,                             glMultiTexCoord4s );
         bindFunction( PFNGLMULTITEXCOORD4SVPROC,                            glMultiTexCoord4sv );
         }                                                                   
      //                                                                     
      // GL_ARB_multisample                                                  
      //                                                                     
      if ( GpuContext.support.extension(ARB_multisample) )                          
         bindFunction( PFNGLSAMPLECOVERAGEARBPROC,                           glSampleCoverageARB );
      //
      // GL_ARB_multitexture         ( Deprecated in OpenGL 3.0 Core )
      // 
      if ( GpuContext.support.extension(ARB_multitexture) )
         bindFunction( PFNGLACTIVETEXTUREARBPROC,                            glActiveTextureARB );   
      if ( GpuContext.support.extension(ARB_multitexture) &&                        
          !GpuContext.screen.support(OpenGL_3_0) )                                      
         {                                                                   
         bindFunction( PFNGLCLIENTACTIVETEXTUREARBPROC,                      glClientActiveTextureARB );
         bindFunction( PFNGLMULTITEXCOORD1DARBPROC,                          glMultiTexCoord1dARB );
         bindFunction( PFNGLMULTITEXCOORD1DVARBPROC,                         glMultiTexCoord1dvARB );
         bindFunction( PFNGLMULTITEXCOORD1FARBPROC,                          glMultiTexCoord1fARB );
         bindFunction( PFNGLMULTITEXCOORD1FVARBPROC,                         glMultiTexCoord1fvARB );
         bindFunction( PFNGLMULTITEXCOORD1IARBPROC,                          glMultiTexCoord1iARB );
         bindFunction( PFNGLMULTITEXCOORD1IVARBPROC,                         glMultiTexCoord1ivARB );
         bindFunction( PFNGLMULTITEXCOORD1SARBPROC,                          glMultiTexCoord1sARB );
         bindFunction( PFNGLMULTITEXCOORD1SVARBPROC,                         glMultiTexCoord1svARB );
         bindFunction( PFNGLMULTITEXCOORD2DARBPROC,                          glMultiTexCoord2dARB );
         bindFunction( PFNGLMULTITEXCOORD2DVARBPROC,                         glMultiTexCoord2dvARB );
         bindFunction( PFNGLMULTITEXCOORD2FARBPROC,                          glMultiTexCoord2fARB );
         bindFunction( PFNGLMULTITEXCOORD2FVARBPROC,                         glMultiTexCoord2fvARB );
         bindFunction( PFNGLMULTITEXCOORD2IARBPROC,                          glMultiTexCoord2iARB );
         bindFunction( PFNGLMULTITEXCOORD2IVARBPROC,                         glMultiTexCoord2ivARB );
         bindFunction( PFNGLMULTITEXCOORD2SARBPROC,                          glMultiTexCoord2sARB );
         bindFunction( PFNGLMULTITEXCOORD2SVARBPROC,                         glMultiTexCoord2svARB );
         bindFunction( PFNGLMULTITEXCOORD3DARBPROC,                          glMultiTexCoord3dARB );
         bindFunction( PFNGLMULTITEXCOORD3DVARBPROC,                         glMultiTexCoord3dvARB );
         bindFunction( PFNGLMULTITEXCOORD3FARBPROC,                          glMultiTexCoord3fARB );
         bindFunction( PFNGLMULTITEXCOORD3FVARBPROC,                         glMultiTexCoord3fvARB );
         bindFunction( PFNGLMULTITEXCOORD3IARBPROC,                          glMultiTexCoord3iARB );
         bindFunction( PFNGLMULTITEXCOORD3IVARBPROC,                         glMultiTexCoord3ivARB );
         bindFunction( PFNGLMULTITEXCOORD3SARBPROC,                          glMultiTexCoord3sARB );
         bindFunction( PFNGLMULTITEXCOORD3SVARBPROC,                         glMultiTexCoord3svARB );
         bindFunction( PFNGLMULTITEXCOORD4DARBPROC,                          glMultiTexCoord4dARB );
         bindFunction( PFNGLMULTITEXCOORD4DVARBPROC,                         glMultiTexCoord4dvARB );
         bindFunction( PFNGLMULTITEXCOORD4FARBPROC,                          glMultiTexCoord4fARB );
         bindFunction( PFNGLMULTITEXCOORD4FVARBPROC,                         glMultiTexCoord4fvARB );
         bindFunction( PFNGLMULTITEXCOORD4IARBPROC,                          glMultiTexCoord4iARB );
         bindFunction( PFNGLMULTITEXCOORD4IVARBPROC,                         glMultiTexCoord4ivARB );
         bindFunction( PFNGLMULTITEXCOORD4SARBPROC,                          glMultiTexCoord4sARB );
         bindFunction( PFNGLMULTITEXCOORD4SVARBPROC,                         glMultiTexCoord4svARB );
         }                                                                   
      //                                                                     
      // GL_ARB_texture_compression                                          
      //                                                                     
      if ( GpuContext.support.extension(ARB_texture_compression) )                  
         {                                                                   
         bindFunction( PFNGLCOMPRESSEDTEXIMAGE1DARBPROC,                     glCompressedTexImage1DARB );
         bindFunction( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC,                     glCompressedTexImage2DARB );
         bindFunction( PFNGLCOMPRESSEDTEXIMAGE3DARBPROC,                     glCompressedTexImage3DARB );
         bindFunction( PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC,                  glCompressedTexSubImage1DARB ); 
         bindFunction( PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC,                  glCompressedTexSubImage2DARB );
         bindFunction( PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC,                  glCompressedTexSubImage3DARB );
         bindFunction( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC,                    glGetCompressedTexImageARB );
         }
      //
      // GL_ARB_transpose_matrix     ( Deprecated in OpenGL 3.0 Core )
      //
      if ( GpuContext.support.extension(ARB_transpose_matrix) &&
          !GpuContext.screen.support(OpenGL_3_0) )
         {
         bindFunction( PFNGLLOADTRANSPOSEMATRIXFARBPROC,                     glLoadTransposeMatrixfARB );
         bindFunction( PFNGLLOADTRANSPOSEMATRIXDARBPROC,                     glLoadTransposeMatrixdARB );
         bindFunction( PFNGLMULTTRANSPOSEMATRIXFARBPROC,                     glMultTransposeMatrixfARB );
         bindFunction( PFNGLMULTTRANSPOSEMATRIXDARBPROC,                     glMultTransposeMatrixdARB );
         }
      //
      // OpenGL 1.2
      //
      //   GL_EXT_bgra
      //   GL_EXT_draw_range_elements
      //   GL_EXT_packed_pixels  
      //   GL_EXT_rescale_normal
      //   GL_EXT_separate_specular_color
      //   GL_EXT_texture3D
      //   GL_SGIS_texture_edge_clamp  /  GL_EXT_texture_edge_clamp
      //   GL_SGIS_texture_lod
      //
      if ( GpuContext.screen.support(OpenGL_1_2) )
         {
         bindFunction( PFNGLCOPYTEXSUBIMAGE3DPROC,                           glCopyTexSubImage3D );
         bindFunction( PFNGLDRAWRANGEELEMENTSPROC,                           glDrawRangeElements );
         bindFunction( PFNGLTEXIMAGE3DPROC,                                  glTexImage3D );
         bindFunction( PFNGLTEXSUBIMAGE3DPROC,                               glTexSubImage3D );
         }                                                                   
      //                                                                     
      // GL_EXT_draw_range_elements                                          
      //                                                                     
      if ( GpuContext.support.extension(EXT_draw_range_elements) )                  
         bindFunction( PFNGLDRAWRANGEELEMENTSEXTPROC,                        glDrawRangeElementsEXT );
      //                                                                     
      // GL_EXT_texture3D                                                    
      //                                                                     
      if ( GpuContext.support.extension(EXT_texture3D) )                            
         bindFunction( PFNGLTEXIMAGE3DEXTPROC,                               glTexImage3DEXT );
      }

      void BindWglFunctionsPointers(void)
      {
      //
      // WGL_ARB_create_context
      //
      if ( GpuContext.support.extension(ARB_create_context) )
         bindFunction( PFNWGLCREATECONTEXTATTRIBSARBPROC,                    wglCreateContextAttribsARB );
      //                                                                     
      // WGL_ARB_pixel_format                                                
      //                                                                     
      if ( GpuContext.support.extension(ARB_pixel_format) )                         
         {                                                                   
         bindFunction( PFNWGLGETPIXELFORMATATTRIBIVARBPROC,                  wglGetPixelFormatAttribivARB );
         bindFunction( PFNWGLGETPIXELFORMATATTRIBFVARBPROC,                  wglGetPixelFormatAttribfvARB );
         bindFunction( PFNWGLCHOOSEPIXELFORMATARBPROC,                       wglChoosePixelFormatARB );
         }                                                                   
      //                                                                     
      // WGL_EXT_pixel_format                                                
      //                                                                     
      if ( GpuContext.support.extension(EXT_pixel_format) )                         
         {                                                                   
         bindFunction( PFNWGLGETPIXELFORMATATTRIBIVEXTPROC,                  wglGetPixelFormatAttribivEXT );
         bindFunction( PFNWGLGETPIXELFORMATATTRIBFVEXTPROC,                  wglGetPixelFormatAttribfvEXT );
         bindFunction( PFNWGLCHOOSEPIXELFORMATEXTPROC,                       wglChoosePixelFormatEXT );
         }                                                                   
      //                                                                     
      // WGL_ARB_extensions_string                                           
      //                                                                     
      if ( GpuContext.support.extension(ARB_extensions_string) )                    
         bindFunction( PFNWGLGETEXTENSIONSSTRINGARBPROC,                     wglGetExtensionsStringARB );
      //
      // WGL_EXT_extensions_string 
      //
      if ( GpuContext.support.extension(EXT_extensions_string) )
         bindFunction( PFNWGLGETEXTENSIONSSTRINGEXTPROC,                     wglGetExtensionsStringEXT );
      //
      // WGL_EXT_swap_control
      //
      if ( GpuContext.support.extension(EXT_swap_control) )
         {
         bindFunction( PFNWGLSWAPINTERVALEXTPROC,                            wglSwapIntervalEXT );
         bindFunction( PFNWGLGETSWAPINTERVALEXTPROC,                         wglGetSwapIntervalEXT );
         }
      }

      #endif
      }
   }
}
