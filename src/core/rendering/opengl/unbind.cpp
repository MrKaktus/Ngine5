/*

 Ngine v5.0
 
 Module      : Extensions deinitiation.
 Visibility  : Engine only.
 Requirements: none
 Description : In Windows, engine needs to get 
               functions pointers from OpenGL ICD 
               driver, because Microsoft's opengl32.dll
               does not support functions introduced 
               after OpenGL v1.1 . Functions in this
               file deinitiates OpenGL API bindings.

*/

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/context.h"

namespace en
{
   namespace gpu
   {
      namespace wgl
      {
      #ifdef EN_PLATFORM_WINDOWS

      void UnbindFunctionsPointers(void)
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
      glClipControl                                 = nullptr;
      glMemoryBarrierByRegion                       = nullptr;
      glCreateTransformFeedbacks                    = nullptr;
      glTransformFeedbackBufferBase                 = nullptr;
      glTransformFeedbackBufferRange                = nullptr;
      glGetTransformFeedbackiv                      = nullptr;
      glGetTransformFeedbacki_v                     = nullptr;
      glGetTransformFeedbacki64_v                   = nullptr;
      glCreateBuffers                               = nullptr;
      glNamedBufferStorage                          = nullptr;
      glNamedBufferData                             = nullptr;
      glNamedBufferSubData                          = nullptr;
      glCopyNamedBufferSubData                      = nullptr;
      glClearNamedBufferData                        = nullptr;
      glClearNamedBufferSubData                     = nullptr;
      glMapNamedBuffer                              = nullptr;
      glMapNamedBufferRange                         = nullptr;
      glUnmapNamedBuffer                            = nullptr;
      glFlushMappedNamedBufferRange                 = nullptr;
      glGetNamedBufferParameteriv                   = nullptr;
      glGetNamedBufferParameteri64v                 = nullptr;
      glGetNamedBufferPointerv                      = nullptr;
      glGetNamedBufferSubData                       = nullptr;
      glCreateFramebuffers                          = nullptr;
      glNamedFramebufferRenderbuffer                = nullptr;
      glNamedFramebufferParameteri                  = nullptr;
      glNamedFramebufferTexture                     = nullptr;
      glNamedFramebufferTextureLayer                = nullptr;
      glNamedFramebufferDrawBuffer                  = nullptr;
      glNamedFramebufferDrawBuffers                 = nullptr;
      glNamedFramebufferReadBuffer                  = nullptr;
      glInvalidateNamedFramebufferData              = nullptr;
      glInvalidateNamedFramebufferSubData           = nullptr;
      glClearNamedFramebufferiv                     = nullptr;
      glClearNamedFramebufferuiv                    = nullptr;
      glClearNamedFramebufferfv                     = nullptr;
      glClearNamedFramebufferfi                     = nullptr;
      glBlitNamedFramebuffer                        = nullptr;
      glCheckNamedFramebufferStatus                 = nullptr;
      glGetNamedFramebufferParameteriv              = nullptr;
      glGetNamedFramebufferAttachmentParameteriv    = nullptr;
      glCreateRenderbuffers                         = nullptr;
      glNamedRenderbufferStorage                    = nullptr;
      glNamedRenderbufferStorageMultisample         = nullptr;
      glGetNamedRenderbufferParameteriv             = nullptr;
      glCreateTextures                              = nullptr;
      glTextureBuffer                               = nullptr;
      glTextureBufferRange                          = nullptr;
      glTextureStorage1D                            = nullptr;
      glTextureStorage2D                            = nullptr;
      glTextureStorage3D                            = nullptr;
      glTextureStorage2DMultisample                 = nullptr;
      glTextureStorage3DMultisample                 = nullptr;
      glTextureSubImage1D                           = nullptr;
      glTextureSubImage2D                           = nullptr;
      glTextureSubImage3D                           = nullptr;
      glCompressedTextureSubImage1D                 = nullptr;
      glCompressedTextureSubImage2D                 = nullptr;
      glCompressedTextureSubImage3D                 = nullptr;
      glCopyTextureSubImage1D                       = nullptr;
      glCopyTextureSubImage2D                       = nullptr;
      glCopyTextureSubImage3D                       = nullptr;
      glTextureParameterf                           = nullptr;
      glTextureParameterfv                          = nullptr;
      glTextureParameteri                           = nullptr;
      glTextureParameterIiv                         = nullptr;
      glTextureParameterIuiv                        = nullptr;
      glTextureParameteriv                          = nullptr;
      glGenerateTextureMipmap                       = nullptr;
      glBindTextureUnit                             = nullptr;
      glGetTextureImage                             = nullptr;
      glGetCompressedTextureImage                   = nullptr;
      glGetTextureLevelParameterfv                  = nullptr;
      glGetTextureLevelParameteriv                  = nullptr;
      glGetTextureParameterfv                       = nullptr;
      glGetTextureParameterIiv                      = nullptr;
      glGetTextureParameterIuiv                     = nullptr;
      glGetTextureParameteriv                       = nullptr;
      glCreateVertexArrays                          = nullptr;
      glDisableVertexArrayAttrib                    = nullptr;
      glEnableVertexArrayAttrib                     = nullptr;
      glVertexArrayElementBuffer                    = nullptr;
      glVertexArrayVertexBuffer                     = nullptr;
      glVertexArrayVertexBuffers                    = nullptr;
      glVertexArrayAttribFormat                     = nullptr;
      glVertexArrayAttribIFormat                    = nullptr;
      glVertexArrayAttribLFormat                    = nullptr;
      glVertexArrayAttribBinding                    = nullptr;
      glVertexArrayBindingDivisor                   = nullptr;
      glGetVertexArrayiv                            = nullptr;
      glGetVertexArrayIndexediv                     = nullptr;
      glGetVertexArrayIndexed64iv                   = nullptr;
      glCreateSamplers                              = nullptr;
      glCreateProgramPipelines                      = nullptr;
      glCreateQueries                               = nullptr;
      glGetQueryBufferObjectiv                      = nullptr;
      glGetQueryBufferObjectuiv                     = nullptr;
      glGetQueryBufferObjecti64v                    = nullptr;
      glGetQueryBufferObjectui64v                   = nullptr;
      glGetTextureSubImage                          = nullptr;
      glGetCompressedTextureSubImage                = nullptr;
      glGetGraphicsResetStatus                      = nullptr;
      glReadnPixels                                 = nullptr;
      glGetnUniformfv                               = nullptr;
      glGetnUniformiv                               = nullptr;
      glGetnUniformuiv                              = nullptr;
      glTextureBarrier                              = nullptr;
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
      glBufferStorage                               = nullptr;
      glNamedBufferStorageEXT                       = nullptr;
      glClearTexImage                               = nullptr;
      glClearTexSubImage                            = nullptr;
      glBindBuffersBase                             = nullptr;
      glBindBuffersRange                            = nullptr;
      glBindTextures                                = nullptr;
      glBindSamplers                                = nullptr;
      glBindImageTextures                           = nullptr;
      glBindVertexBuffers                           = nullptr;
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
      glClearBufferData                             = nullptr;
      glClearBufferSubData                          = nullptr;
      glClearNamedBufferDataEXT                     = nullptr;
      glClearNamedBufferSubDataEXT                  = nullptr;
      glDispatchCompute                             = nullptr;
      glDispatchComputeIndirect                     = nullptr;
      glCopyImageSubData                            = nullptr;
      glDebugMessageControl                         = nullptr;
      glDebugMessageInsert                          = nullptr;
      glDebugMessageCallback                        = nullptr;
      glGetDebugMessageLog                          = nullptr;
      glFramebufferParameteri                       = nullptr;
      glGetFramebufferParameteriv                   = nullptr;
      glNamedFramebufferParameteriEXT               = nullptr;
      glGetNamedFramebufferParameterivEXT           = nullptr;
      glGetInternalformati64v                       = nullptr;
      glInvalidateTexSubImage                       = nullptr;
      glInvalidateTexImage                          = nullptr;
      glInvalidateBufferSubData                     = nullptr;
      glInvalidateBufferData                        = nullptr;
      glInvalidateFramebuffer                       = nullptr;
      glInvalidateSubFramebuffer                    = nullptr;
      glMultiDrawArraysIndirect                     = nullptr;
      glMultiDrawElementsIndirect                   = nullptr;
      glGetProgramInterfaceiv                       = nullptr;
      glGetProgramResourceIndex                     = nullptr;
      glGetProgramResourceName                      = nullptr;
      glGetProgramResourceiv                        = nullptr;
      glGetProgramResourceLocation                  = nullptr;
      glGetProgramResourceLocationIndex             = nullptr;
      glShaderStorageBlockBinding                   = nullptr;
      glTexBufferRange                              = nullptr;
      glTextureBufferRangeEXT                       = nullptr;
      glTexStorage2DMultisample                     = nullptr;
      glTexStorage3DMultisample                     = nullptr;
      glTextureStorage2DMultisampleEXT              = nullptr;
      glTextureStorage3DMultisampleEXT              = nullptr;
      glTextureView                                 = nullptr;
      glBindVertexBuffer                            = nullptr;
      glVertexAttribFormat                          = nullptr;
      glVertexAttribIFormat                         = nullptr;
      glVertexAttribLFormat                         = nullptr;
      glVertexAttribBinding                         = nullptr;
      glVertexBindingDivisor                        = nullptr;
      glVertexArrayBindVertexBufferEXT              = nullptr;
      glVertexArrayVertexAttribFormatEXT            = nullptr;
      glVertexArrayVertexAttribIFormatEXT           = nullptr;
      glVertexArrayVertexAttribLFormatEXT           = nullptr;
      glVertexArrayVertexAttribBindingEXT           = nullptr;
      glVertexArrayVertexBindingDivisorEXT          = nullptr;
      glDebugMessageControl                         = nullptr;
      glDebugMessageInsert                          = nullptr;
      glDebugMessageCallback                        = nullptr;
      glGetDebugMessageLog                          = nullptr;
    //glGetPointerv                                 = nullptr;
      glPushDebugGroup                              = nullptr;
      glPopDebugGroup                               = nullptr;
      glObjectLabel                                 = nullptr;
      glGetObjectLabel                              = nullptr;
      glObjectPtrLabel                              = nullptr;
      glGetObjectPtrLabel                           = nullptr;
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
      glBindImageTexture                            = nullptr;
      glDrawArraysInstancedBaseInstance             = nullptr;
      glDrawElementsInstancedBaseInstance           = nullptr;
      glDrawElementsInstancedBaseVertexBaseInstance = nullptr;
      glDrawTransformFeedbackInstanced              = nullptr;
      glDrawTransformFeedbackStreamInstanced        = nullptr;
      glGetActiveAtomicCounterBufferiv              = nullptr;
      glGetInternalformativ                         = nullptr;
      glMemoryBarrier                               = nullptr;
      glTexStorage1D                                = nullptr;
      glTexStorage2D                                = nullptr;
      glTexStorage3D                                = nullptr;
      glTextureStorage1DEXT                         = nullptr;
      glTextureStorage2DEXT                         = nullptr;
      glTextureStorage3DEXT                         = nullptr;
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
      glActiveShaderProgram                         = nullptr;
      glBindProgramPipeline                         = nullptr;
      glClearDepthf                                 = nullptr;
      glCreateShaderProgramv                        = nullptr;
      glDeleteProgramPipelines                      = nullptr;
      glDepthRangef                                 = nullptr;
      glDepthRangeArrayv                            = nullptr;
      glDepthRangeIndexed                           = nullptr;
      glGetDoublei_v                                = nullptr;
      glGetFloati_v                                 = nullptr;
      glGetProgramBinary                            = nullptr;
      glGenProgramPipelines                         = nullptr;
      glGetProgramPipelineiv                        = nullptr;
      glGetProgramPipelineInfoLog                   = nullptr;
      glGetShaderPrecisionFormat                    = nullptr;
      glGetVertexAttribLdv                          = nullptr;
      glIsProgramPipeline                           = nullptr;
      glProgramBinary                               = nullptr;
      glProgramParameteri                           = nullptr;
      glProgramUniform1d                            = nullptr;
      glProgramUniform1dv                           = nullptr;
      glProgramUniform1f                            = nullptr;
      glProgramUniform1fv                           = nullptr;
      glProgramUniform1i                            = nullptr;
      glProgramUniform1iv                           = nullptr;
      glProgramUniform1ui                           = nullptr;
      glProgramUniform1uiv                          = nullptr;
      glProgramUniform2d                            = nullptr;
      glProgramUniform2dv                           = nullptr;
      glProgramUniform2f                            = nullptr;
      glProgramUniform2fv                           = nullptr;
      glProgramUniform2i                            = nullptr;
      glProgramUniform2iv                           = nullptr;
      glProgramUniform2ui                           = nullptr;
      glProgramUniform2uiv                          = nullptr;
      glProgramUniform3d                            = nullptr;
      glProgramUniform3dv                           = nullptr;
      glProgramUniform3f                            = nullptr;
      glProgramUniform3fv                           = nullptr;
      glProgramUniform3i                            = nullptr;
      glProgramUniform3iv                           = nullptr;
      glProgramUniform3ui                           = nullptr;
      glProgramUniform3uiv                          = nullptr;
      glProgramUniform4d                            = nullptr;
      glProgramUniform4dv                           = nullptr;
      glProgramUniform4f                            = nullptr;
      glProgramUniform4fv                           = nullptr;
      glProgramUniform4i                            = nullptr;
      glProgramUniform4iv                           = nullptr;
      glProgramUniform4ui                           = nullptr;
      glProgramUniform4uiv                          = nullptr;
      glProgramUniformMatrix2dv                     = nullptr;
      glProgramUniformMatrix2fv                     = nullptr;
      glProgramUniformMatrix3dv                     = nullptr;
      glProgramUniformMatrix3fv                     = nullptr;
      glProgramUniformMatrix4dv                     = nullptr;
      glProgramUniformMatrix4fv                     = nullptr;
      glProgramUniformMatrix2x3dv                   = nullptr;
      glProgramUniformMatrix2x3fv                   = nullptr;
      glProgramUniformMatrix2x4dv                   = nullptr;
      glProgramUniformMatrix2x4fv                   = nullptr;
      glProgramUniformMatrix3x2dv                   = nullptr;
      glProgramUniformMatrix3x2fv                   = nullptr;
      glProgramUniformMatrix3x4dv                   = nullptr;
      glProgramUniformMatrix3x4fv                   = nullptr;
      glProgramUniformMatrix4x2dv                   = nullptr;
      glProgramUniformMatrix4x2fv                   = nullptr;
      glProgramUniformMatrix4x3dv                   = nullptr;
      glProgramUniformMatrix4x3fv                   = nullptr;
      glReleaseShaderCompiler                       = nullptr;
      glScissorArrayv                               = nullptr;
      glScissorIndexed                              = nullptr;
      glScissorIndexedv                             = nullptr;
      glShaderBinary                                = nullptr;
      glUseProgramStages                            = nullptr;
      glValidateProgramPipeline                     = nullptr;
      glVertexAttribL1d                             = nullptr;
      glVertexAttribL1dv                            = nullptr;
      glVertexAttribL2d                             = nullptr;
      glVertexAttribL2dv                            = nullptr;
      glVertexAttribL3d                             = nullptr;
      glVertexAttribL3dv                            = nullptr;
      glVertexAttribL4d                             = nullptr;
      glVertexAttribL4dv                            = nullptr;
      glVertexAttribLPointer                        = nullptr;
      glViewportArrayv                              = nullptr;
      glViewportIndexedf                            = nullptr;
      glViewportIndexedfv                           = nullptr;
      //                                            
      // GL_ARB_debug_output                        
      //                                            
      glDebugMessageCallbackARB                     = nullptr;
      glDebugMessageControlARB                      = nullptr;
      glDebugMessageInsertARB                       = nullptr;
      glGetDebugMessageLogARB                       = nullptr;
      //                                            
      // GL_ARB_vertex_attrib_64bit                 
      //                                            
      glVertexArrayVertexAttribLOffsetEXT           = nullptr;
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
      glBindTransformFeedback                       = nullptr;
      glBlendEquationi                              = nullptr;
      glBlendEquationSeparatei                      = nullptr;
      glBlendFunci                                  = nullptr;
      glBlendFuncSeparatei                          = nullptr; 
      glDeleteTransformFeedbacks                    = nullptr; 
      glDrawArraysIndirect                          = nullptr;
      glDrawElementsIndirect                        = nullptr;
      glDrawTransformFeedback                       = nullptr;
      glGenTransformFeedbacks                       = nullptr;
      glGetActiveSubroutineName                     = nullptr;
      glGetActiveSubroutineUniformiv                = nullptr;
      glGetActiveSubroutineUniformName              = nullptr;
      glGetProgramStageiv                           = nullptr;
      glGetSubroutineIndex                          = nullptr;
      glGetSubroutineUniformLocation                = nullptr;
      glGetUniformdv                                = nullptr;
      glGetUniformSubroutineuiv                     = nullptr;
      glIsTransformFeedback                         = nullptr;
      glMinSampleShading                            = nullptr;
      glPatchParameteri                             = nullptr;
      glPatchParameterfv                            = nullptr;
      glPauseTransformFeedback                      = nullptr;
      glResumeTransformFeedback                     = nullptr;
      glUniform1d                                   = nullptr;
      glUniform1dv                                  = nullptr;
      glUniform2d                                   = nullptr;
      glUniform2dv                                  = nullptr;
      glUniform3d                                   = nullptr;
      glUniform3dv                                  = nullptr;
      glUniform4d                                   = nullptr;
      glUniform4dv                                  = nullptr;
      glUniformMatrix2dv                            = nullptr;
      glUniformMatrix3dv                            = nullptr;
      glUniformMatrix4dv                            = nullptr;
      glUniformMatrix2x3dv                          = nullptr;
      glUniformMatrix2x4dv                          = nullptr;
      glUniformMatrix3x2dv                          = nullptr;
      glUniformMatrix3x4dv                          = nullptr;
      glUniformMatrix4x2dv                          = nullptr;
      glUniformMatrix4x3dv                          = nullptr;
      glUniformSubroutinesuiv                       = nullptr;
      //                                                             
      // GL_ARB_draw_buffers_blend                                   
      //                                            
      glBlendEquationiARB                           = nullptr;
      glBlendEquationSeparateiARB                   = nullptr;
      glBlendFunciARB                               = nullptr;
      glBlendFuncSeparateiARB                       = nullptr;
      //   
      // GL_ARB_gpu_shader_fp64   
      //
      glProgramUniform1dEXT                         = nullptr;
      glProgramUniform1dvEXT                        = nullptr;
      glProgramUniform2dEXT                         = nullptr;
      glProgramUniform2dvEXT                        = nullptr;
      glProgramUniform3dEXT                         = nullptr;
      glProgramUniform3dvEXT                        = nullptr;
      glProgramUniform4dEXT                         = nullptr;
      glProgramUniform4dvEXT                        = nullptr;
      glProgramUniformMatrix2dvEXT                  = nullptr;
      glProgramUniformMatrix3dvEXT                  = nullptr;
      glProgramUniformMatrix4dvEXT                  = nullptr;
      glProgramUniformMatrix2x3dvEXT                = nullptr;
      glProgramUniformMatrix2x4dvEXT                = nullptr;
      glProgramUniformMatrix3x2dvEXT                = nullptr;
      glProgramUniformMatrix3x4dvEXT                = nullptr;
      glProgramUniformMatrix4x2dvEXT                = nullptr;
      glProgramUniformMatrix4x3dvEXT                = nullptr;
      //                                            
      // GL_ARB_sample_shading                      
      //                                            
      glMinSampleShadingARB                         = nullptr;
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
      glBindFragDataLocationIndexed                 = nullptr;
      glBindSampler                                 = nullptr;
      glDeleteSamplers                              = nullptr;
      glGenSamplers                                 = nullptr;
      glGetFragDataIndex                            = nullptr;
      glGetQueryObjecti64v                          = nullptr;
      glGetQueryObjectui64v                         = nullptr;
      glGetSamplerParameteriv                       = nullptr;
      glGetSamplerParameterfv                       = nullptr;
      glGetSamplerParameterIiv                      = nullptr;
      glGetSamplerParameterIuiv                     = nullptr;
      glIsSampler                                   = nullptr;
      glQueryCounter                                = nullptr;
      glSamplerParameteri                           = nullptr;
      glSamplerParameterf                           = nullptr;
      glSamplerParameteriv                          = nullptr;
      glSamplerParameterfv                          = nullptr;
      glSamplerParameterIiv                         = nullptr;
      glSamplerParameterIuiv                        = nullptr;
      glVertexAttribP1ui                            = nullptr;
      glVertexAttribP1uiv                           = nullptr;
      glVertexAttribP2ui                            = nullptr;
      glVertexAttribP2uiv                           = nullptr;
      glVertexAttribP3ui                            = nullptr;
      glVertexAttribP3uiv                           = nullptr;
      glVertexAttribP4ui                            = nullptr;
      glVertexAttribP4uiv                           = nullptr;
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
      glClientWaitSync                              = nullptr;
      glDeleteSync                                  = nullptr;
      glDrawElementsBaseVertex                      = nullptr;
      glDrawElementsInstancedBaseVertex             = nullptr;
      glDrawRangeElementsBaseVertex                 = nullptr;
      glFenceSync                                   = nullptr;
      glGetBufferParameteri64v                      = nullptr;
      glGetInteger64i_v                             = nullptr;
      glGetInteger64v                               = nullptr;
      glGetMultisamplefv                            = nullptr;
      glGetSynciv                                   = nullptr;
      glIsSync                                      = nullptr;
      glMultiDrawElementsBaseVertex                 = nullptr;
      glFramebufferTexture                          = nullptr;
      glProgramParameteri                           = nullptr;
      glProvokingVertex                             = nullptr;
      glSampleMaski                                 = nullptr;
      glTexImage2DMultisample                       = nullptr;
      glTexImage3DMultisample                       = nullptr;
      glWaitSync                                    = nullptr;
      //                                                                                             
      // GL_ARB_geometry_shader4                                                                     
      //                                            
      glProgramParameteriARB                        = nullptr;
      glFramebufferTextureARB                       = nullptr;
      glFramebufferTextureLayerARB                  = nullptr;
      glFramebufferTextureFaceARB                   = nullptr;
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
      glCopyBufferSubData                           = nullptr;
      glDrawArraysInstanced                         = nullptr;
      glDrawElementsInstanced                       = nullptr;
      glGetActiveUniformBlockiv                     = nullptr;
      glGetActiveUniformBlockName                   = nullptr;
      glGetActiveUniformsiv                         = nullptr;
      glGetActiveUniformName                        = nullptr;
      glGetUniformBlockIndex                        = nullptr;
      glGetUniformIndices                           = nullptr;
      glPrimitiveRestartIndex                       = nullptr;
      glTexBuffer                                   = nullptr;
      glUniformBlockBinding                         = nullptr;
      //                                                                                             
      // GL_ARB_draw_instanced                                                                       
      //                                            
      glDrawArraysInstancedARB                      = nullptr;
      glDrawElementsInstancedARB                    = nullptr;
      //                                                                                             
      // GL_ARB_texture_buffer_object                                                                
      //                                            
      glTexBufferARB                                = nullptr;
      //                                            
      // GL_NV_primitive_restart                    
      //                                            
      glPrimitiveRestartNV                          = nullptr;
      glPrimitiveRestartIndexNV                     = nullptr;
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
      glBeginConditionalRender                      = nullptr;
      glBeginTransformFeedback                      = nullptr;
      glBindBufferBase                              = nullptr;
      glBindBufferRange                             = nullptr;
      glBindFragDataLocation                        = nullptr;
      glBindFramebuffer                             = nullptr;
      glBindRenderbuffer                            = nullptr;
      glBindVertexArray                             = nullptr;
      glBlitFramebuffer                             = nullptr;
      glCheckFramebufferStatus                      = nullptr;
      glClampColor                                  = nullptr;
      glClearBufferfi                               = nullptr;
      glClearBufferfv                               = nullptr;
      glClearBufferiv                               = nullptr;
      glClearBufferuiv                              = nullptr;
      glColorMaski                                  = nullptr;
      glDeleteFramebuffers                          = nullptr;
      glDeleteRenderbuffers                         = nullptr;
      glDeleteVertexArrays                          = nullptr;
      glDisablei                                    = nullptr;
      glEnablei                                     = nullptr;
      glEndConditionalRender                        = nullptr;
      glEndTransformFeedback                        = nullptr;
      glFlushMappedBufferRange                      = nullptr;
      glFramebufferRenderbuffer                     = nullptr;
      glFramebufferTexture1D                        = nullptr;
      glFramebufferTexture2D                        = nullptr;
      glFramebufferTexture3D                        = nullptr;
      glFramebufferTextureLayer                     = nullptr;
      glGenerateMipmap                              = nullptr;
      glGenFramebuffers                             = nullptr;
      glGenRenderbuffers                            = nullptr;
      glGenVertexArrays                             = nullptr;
      glGetBooleani_v                               = nullptr;
      glGetFragDataLocation                         = nullptr;
      glGetFramebufferAttachmentParameteriv         = nullptr;
      glGetIntegeri_v                               = nullptr;
      glGetRenderbufferParameteriv                  = nullptr;
      glGetStringi                                  = nullptr;
      glGetTexParameterIiv                          = nullptr;
      glGetTexParameterIuiv                         = nullptr;
      glGetTransformFeedbackVarying                 = nullptr;
      glGetUniformuiv                               = nullptr;
      glGetVertexAttribIiv                          = nullptr;
      glGetVertexAttribIuiv                         = nullptr;
      glIsEnabledi                                  = nullptr;
      glIsFramebuffer                               = nullptr;
      glIsRenderbuffer                              = nullptr;
      glIsVertexArray                               = nullptr;
      glMapBufferRange                              = nullptr;
      glRenderbufferStorage                         = nullptr;
      glRenderbufferStorageMultisample              = nullptr;
      glTexParameterIiv                             = nullptr;
      glTexParameterIuiv                            = nullptr;
      glTransformFeedbackVaryings                   = nullptr;
      glUniform1ui                                  = nullptr;
      glUniform1uiv                                 = nullptr;
      glUniform2ui                                  = nullptr;
      glUniform2uiv                                 = nullptr;
      glUniform3ui                                  = nullptr;
      glUniform3uiv                                 = nullptr;
      glUniform4ui                                  = nullptr;
      glUniform4uiv                                 = nullptr;
      glVertexAttribI1i                             = nullptr;
      glVertexAttribI1iv                            = nullptr;
      glVertexAttribI1ui                            = nullptr;
      glVertexAttribI1uiv                           = nullptr;
      glVertexAttribI2i                             = nullptr;
      glVertexAttribI2iv                            = nullptr;
      glVertexAttribI2ui                            = nullptr;
      glVertexAttribI2uiv                           = nullptr;
      glVertexAttribI3i                             = nullptr;
      glVertexAttribI3iv                            = nullptr;
      glVertexAttribI3ui                            = nullptr;
      glVertexAttribI3uiv                           = nullptr;
      glVertexAttribI4bv                            = nullptr;
      glVertexAttribI4i                             = nullptr;
      glVertexAttribI4iv                            = nullptr;
      glVertexAttribI4sv                            = nullptr;
      glVertexAttribI4ubv                           = nullptr;
      glVertexAttribI4ui                            = nullptr;
      glVertexAttribI4uiv                           = nullptr;
      glVertexAttribI4usv                           = nullptr;
      glVertexAttribIPointer                        = nullptr;
      //
      // GL_ARB_color_buffer_float
      //
      glClampColorARB                               = nullptr;
      //                                    
      // GL_APPLE_flush_buffer_range        ( predecessor of GL_ARB_map_buffer_range )
      //                                    
      glBufferParameteriAPPLE                       = nullptr;
      glFlushMappedBufferRangeAPPLE                 = nullptr;
      //                                    
      // GL_APPLE_vertex_array_object       ( predecessor of GL_ARB_vertex_array_object )
      //                                    
      glBindVertexArrayAPPLE                        = nullptr;
      glDeleteVertexArraysAPPLE                     = nullptr;
      glGenVertexArraysAPPLE                        = nullptr;
      glIsVertexArrayAPPLE                          = nullptr;
      //                                            
      // GL_EXT_draw_buffers2                       
      //                                            
      glColorMaskIndexedEXT                         = nullptr;
      glDisableIndexedEXT                           = nullptr;
      glEnableIndexedEXT                            = nullptr;
      glGetBooleanIndexedvEXT                       = nullptr;
      glGetIntegerIndexedvEXT                       = nullptr;
      glIsEnabledIndexedEXT                         = nullptr;
      //                                            
      // GL_EXT_framebuffer_blit                    
      //                                            
      glBlitFramebufferEXT                          = nullptr;
      //                                    
      // GL_EXT_framebuffer_multisample     
      //                                    
      glRenderbufferStorageMultisampleEXT           = nullptr; 
      //
      // GL_EXT_gpu_shader4
      //
      glBindFragDataLocationEXT                     = nullptr;
      glGetFragDataLocationEXT                      = nullptr;
      glGetUniformuivEXT                            = nullptr;
      glGetVertexAttribIivEXT                       = nullptr;
      glGetVertexAttribIuivEXT                      = nullptr;
      glUniform1uiEXT                               = nullptr;
      glUniform1uivEXT                              = nullptr;
      glUniform2uiEXT                               = nullptr;
      glUniform2uivEXT                              = nullptr;
      glUniform3uiEXT                               = nullptr;
      glUniform3uivEXT                              = nullptr;
      glUniform4uiEXT                               = nullptr;
      glUniform4uivEXT                              = nullptr;
      glVertexAttribI1iEXT                          = nullptr;
      glVertexAttribI1ivEXT                         = nullptr;
      glVertexAttribI1uiEXT                         = nullptr;
      glVertexAttribI1uivEXT                        = nullptr;
      glVertexAttribI2iEXT                          = nullptr;
      glVertexAttribI2ivEXT                         = nullptr;
      glVertexAttribI2uiEXT                         = nullptr;
      glVertexAttribI2uivEXT                        = nullptr;
      glVertexAttribI3iEXT                          = nullptr;
      glVertexAttribI3ivEXT                         = nullptr;
      glVertexAttribI3uiEXT                         = nullptr;
      glVertexAttribI3uivEXT                        = nullptr;
      glVertexAttribI4bvEXT                         = nullptr;
      glVertexAttribI4iEXT                          = nullptr;
      glVertexAttribI4ivEXT                         = nullptr;
      glVertexAttribI4svEXT                         = nullptr;
      glVertexAttribI4ubvEXT                        = nullptr;
      glVertexAttribI4uiEXT                         = nullptr;
      glVertexAttribI4uivEXT                        = nullptr;
      glVertexAttribI4usvEXT                        = nullptr;
      glVertexAttribIPointerEXT                     = nullptr;
      //                                    
      // GL_EXT_texture_array               
      //                                    
      glFramebufferTextureLayerEXT                  = nullptr;
      //                                            
      // GL_EXT_texture_integer                     
      //                                            
      glClearColorIiEXT                             = nullptr;
      glClearColorIuiEXT                            = nullptr;
      glGetTexParameterIivEXT                       = nullptr;
      glGetTexParameterIuivEXT                      = nullptr;
      glTexParameterIivEXT                          = nullptr;
      glTexParameterIuivEXT                         = nullptr;
      //                                            
      // GL_EXT_transform_feedback                  
      //                                            
      glBeginTransformFeedbackEXT                   = nullptr;
      glBindBufferBaseEXT                           = nullptr;
      glBindBufferOffsetEXT                         = nullptr;
      glBindBufferRangeEXT                          = nullptr;
      glEndTransformFeedbackEXT                     = nullptr;
      glGetTransformFeedbackVaryingEXT              = nullptr;
      glTransformFeedbackVaryingsEXT                = nullptr;
      //                                    
      // GL_NV_transform_feedback           ( predecessor of GL_EXT_transform_feedback )
      //                                    
      glActiveVaryingNV                             = nullptr;
      glBeginTransformFeedbackNV                    = nullptr;
      glBindBufferBaseNV                            = nullptr;
      glBindBufferOffsetNV                          = nullptr;
      glBindBufferRangeNV                           = nullptr;
      glEndTransformFeedbackNV                      = nullptr;
      glGetActiveVaryingNV                          = nullptr;
      glGetTransformFeedbackVaryingNV               = nullptr;
      glGetVaryingLocationNV                        = nullptr;
      glTransformFeedbackAttribsNV                  = nullptr;
      glTransformFeedbackVaryingsNV                 = nullptr;
      //                                            
      // GL_NV_conditional_render                   
      //                                            
      glBeginConditionalRenderNV                    = nullptr;
      glEndConditionalRenderNV                      = nullptr;
      //
      // OpenGL 2.1
      //
      //   GLSL 1.20
      //
      //   GL_ARB_pixel_buffer_object
      //   GL_EXT_texture_sRGB
      //
      glUniformMatrix2x3fv                          = nullptr;
      glUniformMatrix3x2fv                          = nullptr;
      glUniformMatrix2x4fv                          = nullptr;
      glUniformMatrix4x2fv                          = nullptr;
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
      glAttachShader                                = nullptr;
      glBindAttribLocation                          = nullptr;
      glCompileShader                               = nullptr;
      glCreateProgram                               = nullptr;
      glCreateShader                                = nullptr;
      glDeleteProgram                               = nullptr;
      glDeleteShader                                = nullptr;
      glDetachShader                                = nullptr;
      glDisableVertexAttribArray                    = nullptr;
      glDrawBuffers                                 = nullptr;
      glEnableVertexAttribArray                     = nullptr;
      glGetActiveAttrib                             = nullptr;
      glGetActiveUniform                            = nullptr;
      glGetAttachedShaders                          = nullptr;
      glGetAttribLocation                           = nullptr;
      glGetProgramiv                                = nullptr;
      glGetProgramInfoLog                           = nullptr;
      glGetShaderiv                                 = nullptr;
      glGetShaderInfoLog                            = nullptr;
      glGetShaderSource                             = nullptr;
      glGetUniformLocation                          = nullptr;
      glGetUniformfv                                = nullptr;
      glGetUniformiv                                = nullptr;
      glGetVertexAttribdv                           = nullptr;
      glGetVertexAttribfv                           = nullptr;
      glGetVertexAttribiv                           = nullptr;
      glGetVertexAttribPointerv                     = nullptr;
      glIsProgram                                   = nullptr;
      glIsShader                                    = nullptr;
      glLinkProgram                                 = nullptr;
      glShaderSource                                = nullptr;
      glStencilFuncSeparate                         = nullptr;
      glStencilMaskSeparate                         = nullptr;
      glStencilOpSeparate                           = nullptr;
      glUniform1f                                   = nullptr;
      glUniform1fv                                  = nullptr;
      glUniform1i                                   = nullptr;
      glUniform1iv                                  = nullptr;
      glUniform2f                                   = nullptr;
      glUniform2fv                                  = nullptr;
      glUniform2i                                   = nullptr;
      glUniform2iv                                  = nullptr;
      glUniform3f                                   = nullptr;
      glUniform3fv                                  = nullptr;
      glUniform3i                                   = nullptr;
      glUniform3iv                                  = nullptr;
      glUniform4f                                   = nullptr;
      glUniform4fv                                  = nullptr;
      glUniform4i                                   = nullptr;
      glUniform4iv                                  = nullptr;
      glUniformMatrix2fv                            = nullptr;
      glUniformMatrix3fv                            = nullptr;
      glUniformMatrix4fv                            = nullptr;
      glUseProgram                                  = nullptr;
      glValidateProgram                             = nullptr;
      glVertexAttrib1d                              = nullptr;
      glVertexAttrib1dv                             = nullptr;
      glVertexAttrib1f                              = nullptr;
      glVertexAttrib1fv                             = nullptr;
      glVertexAttrib1s                              = nullptr;
      glVertexAttrib1sv                             = nullptr;
      glVertexAttrib2d                              = nullptr;
      glVertexAttrib2dv                             = nullptr;
      glVertexAttrib2f                              = nullptr;
      glVertexAttrib2fv                             = nullptr;
      glVertexAttrib2s                              = nullptr;
      glVertexAttrib2sv                             = nullptr;
      glVertexAttrib3d                              = nullptr;
      glVertexAttrib3dv                             = nullptr;
      glVertexAttrib3f                              = nullptr;
      glVertexAttrib3fv                             = nullptr;
      glVertexAttrib3s                              = nullptr;
      glVertexAttrib3sv                             = nullptr;
      glVertexAttrib4Nbv                            = nullptr;
      glVertexAttrib4Niv                            = nullptr;
      glVertexAttrib4Nsv                            = nullptr;
      glVertexAttrib4Nub                            = nullptr;
      glVertexAttrib4Nubv                           = nullptr;
      glVertexAttrib4Nuiv                           = nullptr;
      glVertexAttrib4Nusv                           = nullptr;
      glVertexAttrib4bv                             = nullptr;
      glVertexAttrib4d                              = nullptr;
      glVertexAttrib4dv                             = nullptr;
      glVertexAttrib4f                              = nullptr;
      glVertexAttrib4fv                             = nullptr;
      glVertexAttrib4iv                             = nullptr;
      glVertexAttrib4s                              = nullptr;
      glVertexAttrib4sv                             = nullptr;
      glVertexAttrib4ubv                            = nullptr;
      glVertexAttrib4uiv                            = nullptr;
      glVertexAttrib4usv                            = nullptr;
      glVertexAttribPointer                         = nullptr;
      //                                            
      // GL_ARB_draw_buffers                        
      //                                            
      glDrawBuffersARB                              = nullptr;
      //                                    
      // GL_ARB_shader_objects              
      //                                    
      glAttachObjectARB                             = nullptr;
      glCompileShaderARB                            = nullptr;
      glCreateProgramObjectARB                      = nullptr;
      glCreateShaderObjectARB                       = nullptr;
      glDeleteObjectARB                             = nullptr;
      glDetachObjectARB                             = nullptr;
      glGetActiveUniformARB                         = nullptr;
      glGetAttachedObjectsARB                       = nullptr;
      glGetHandleARB                                = nullptr;
      glGetInfoLogARB                               = nullptr;
      glGetObjectParameterfvARB                     = nullptr;
      glGetObjectParameterivARB                     = nullptr;
      glGetUniformfvARB                             = nullptr;
      glGetUniformivARB                             = nullptr;
      glGetUniformLocationARB                       = nullptr;
      glGetShaderSourceARB                          = nullptr;
      glLinkProgramARB                              = nullptr;
      glShaderSourceARB                             = nullptr;
      glUniform1fARB                                = nullptr;
      glUniform1fvARB                               = nullptr;
      glUniform1iARB                                = nullptr;
      glUniform1ivARB                               = nullptr;
      glUniform2fARB                                = nullptr;
      glUniform2fvARB                               = nullptr;
      glUniform2iARB                                = nullptr;
      glUniform2ivARB                               = nullptr;
      glUniform3fARB                                = nullptr;
      glUniform3fvARB                               = nullptr;
      glUniform3iARB                                = nullptr;
      glUniform3ivARB                               = nullptr;
      glUniform4fARB                                = nullptr;
      glUniform4fvARB                               = nullptr;
      glUniform4iARB                                = nullptr;
      glUniform4ivARB                               = nullptr;
      glUniformMatrix2fvARB                         = nullptr;
      glUniformMatrix3fvARB                         = nullptr;
      glUniformMatrix4fvARB                         = nullptr;
      glUseProgramObjectARB                         = nullptr;
      glValidateProgramARB                          = nullptr;
      //                                    
      // GL_ARB_vertex_shader               
      //                                    
      glBindAttribLocationARB                       = nullptr;
      glDisableVertexAttribArrayARB                 = nullptr;
      glEnableVertexAttribArrayARB                  = nullptr;
      glGetActiveAttribARB                          = nullptr;
      glGetAttribLocationARB                        = nullptr;
      glGetVertexAttribdvARB                        = nullptr;
      glGetVertexAttribfvARB                        = nullptr;
      glGetVertexAttribivARB                        = nullptr;
      glGetVertexAttribPointervARB                  = nullptr;
      glVertexAttrib1dARB                           = nullptr;
      glVertexAttrib1dvARB                          = nullptr;
      glVertexAttrib1fARB                           = nullptr;
      glVertexAttrib1fvARB                          = nullptr;
      glVertexAttrib1sARB                           = nullptr;
      glVertexAttrib1svARB                          = nullptr;
      glVertexAttrib2dARB                           = nullptr;
      glVertexAttrib2dvARB                          = nullptr;
      glVertexAttrib2fARB                           = nullptr;
      glVertexAttrib2fvARB                          = nullptr;
      glVertexAttrib2sARB                           = nullptr;
      glVertexAttrib2svARB                          = nullptr;
      glVertexAttrib3dARB                           = nullptr;
      glVertexAttrib3dvARB                          = nullptr;
      glVertexAttrib3fARB                           = nullptr;
      glVertexAttrib3fvARB                          = nullptr;
      glVertexAttrib3sARB                           = nullptr;
      glVertexAttrib3svARB                          = nullptr;
      glVertexAttrib4NbvARB                         = nullptr;
      glVertexAttrib4NivARB                         = nullptr;
      glVertexAttrib4NsvARB                         = nullptr;
      glVertexAttrib4NubARB                         = nullptr;
      glVertexAttrib4NubvARB                        = nullptr;
      glVertexAttrib4NuivARB                        = nullptr;
      glVertexAttrib4NusvARB                        = nullptr;
      glVertexAttrib4bvARB                          = nullptr;
      glVertexAttrib4dARB                           = nullptr;
      glVertexAttrib4dvARB                          = nullptr;
      glVertexAttrib4fARB                           = nullptr;
      glVertexAttrib4fvARB                          = nullptr;
      glVertexAttrib4ivARB                          = nullptr;
      glVertexAttrib4sARB                           = nullptr;
      glVertexAttrib4svARB                          = nullptr;
      glVertexAttrib4ubvARB                         = nullptr;
      glVertexAttrib4uivARB                         = nullptr;
      glVertexAttrib4usvARB                         = nullptr;
      glVertexAttribPointerARB                      = nullptr;
      //                                            
      // GL_EXT_blend_equation_separate             
      //                                            
      glBlendEquationSeparateEXT                    = nullptr;
      //                                    
      // GL_EXT_stencil_two_side            
      //                                    
      glActiveStencilFaceEXT                        = nullptr;
      //
      // OpenGL 1.5
      //
      //   GL_ARB_occlusion_query        or    GL_HP_occlusion_test
      //   GL_ARB_vertex_buffer_object
      //   GL_EXT_shadow_funcs
      //
      glBeginQuery                                  = nullptr;
      glBindBuffer                                  = nullptr;
      glBufferData                                  = nullptr;
      glBufferSubData                               = nullptr;
      glDeleteBuffers                               = nullptr;
      glDeleteQueries                               = nullptr;
      glEndQuery                                    = nullptr;
      glGenBuffers                                  = nullptr;
      glGenQueries                                  = nullptr;
      glGetBufferParameteriv                        = nullptr;
      glGetBufferPointerv                           = nullptr;
      glGetBufferSubData                            = nullptr;
      glGetQueryiv                                  = nullptr;
      glGetQueryObjectiv                            = nullptr;
      glGetQueryObjectuiv                           = nullptr;
      glIsBuffer                                    = nullptr;
      glIsQuery                                     = nullptr;
      glMapBuffer                                   = nullptr;
      glUnmapBuffer                                 = nullptr;
      //                                                                     
      // GL_ARB_occlusion_query                                              
      //  
      glBeginQueryARB                               = nullptr;
      glDeleteQueriesARB                            = nullptr;
      glEndQueryARB                                 = nullptr;
      glGenQueriesARB                               = nullptr;
      glGetQueryivARB                               = nullptr;
      glGetQueryObjectivARB                         = nullptr;
      glGetQueryObjectuivARB                        = nullptr;
      glIsQueryARB                                  = nullptr;
      //                                                                               
      // GL_ARB_vertex_buffer_object                                                   
      //                                            
      glBindBufferARB                               = nullptr;
      glBufferDataARB                               = nullptr;
      glBufferSubDataARB                            = nullptr;
      glDeleteBuffersARB                            = nullptr;
      glGenBuffersARB                               = nullptr;
      glGetBufferParameterivARB                     = nullptr;
      glGetBufferPointervARB                        = nullptr;
      glGetBufferSubDataARB                         = nullptr;
      glIsBufferARB                                 = nullptr;
      glMapBufferARB                                = nullptr;
      glUnmapBufferARB                              = nullptr;
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
      glBlendColor                                  = nullptr;
      glBlendFuncSeparate                           = nullptr;
      glFogCoordf                                   = nullptr;
      glFogCoordfv                                  = nullptr;
      glFogCoordd                                   = nullptr;
      glFogCoorddv                                  = nullptr;
      glFogCoordPointer                             = nullptr;
      glMultiDrawArrays                             = nullptr;
      glMultiDrawElements                           = nullptr;
      glPointParameterf                             = nullptr;
      glPointParameterfv                            = nullptr;
      glPointParameteri                             = nullptr;
      glPointParameteriv                            = nullptr;
      glSecondaryColor3b                            = nullptr;
      glSecondaryColor3bv                           = nullptr;
      glSecondaryColor3d                            = nullptr;
      glSecondaryColor3dv                           = nullptr;
      glSecondaryColor3f                            = nullptr;
      glSecondaryColor3fv                           = nullptr;
      glSecondaryColor3i                            = nullptr;
      glSecondaryColor3iv                           = nullptr;
      glSecondaryColor3s                            = nullptr;
      glSecondaryColor3sv                           = nullptr;
      glSecondaryColor3ub                           = nullptr;
      glSecondaryColor3ubv                          = nullptr;
      glSecondaryColor3ui                           = nullptr;
      glSecondaryColor3uiv                          = nullptr;
      glSecondaryColor3us                           = nullptr;
      glSecondaryColor3usv                          = nullptr;
      glSecondaryColorPointer                       = nullptr;
      glWindowPos2d                                 = nullptr;
      glWindowPos2dv                                = nullptr;
      glWindowPos2f                                 = nullptr;
      glWindowPos2fv                                = nullptr;
      glWindowPos2i                                 = nullptr;
      glWindowPos2iv                                = nullptr;
      glWindowPos2s                                 = nullptr;
      glWindowPos2sv                                = nullptr;
      glWindowPos3d                                 = nullptr;
      glWindowPos3dv                                = nullptr;
      glWindowPos3f                                 = nullptr;
      glWindowPos3fv                                = nullptr;
      glWindowPos3i                                 = nullptr;
      glWindowPos3iv                                = nullptr;
      glWindowPos3s                                 = nullptr;
      glWindowPos3sv                                = nullptr;
      //                                            
      // GL_ARB_point_parameters                    
      //                                            
      glPointParameterfARB                          = nullptr;
      glPointParameterfvARB                         = nullptr;
      //                                    
      // GL_ARB_window_pos                  ( Deprecated in OpenGL 3.0 Core )
      //                                    
      glWindowPos2dARB                              = nullptr;
      glWindowPos2dvARB                             = nullptr;
      glWindowPos2fARB                              = nullptr;
      glWindowPos2fvARB                             = nullptr;
      glWindowPos2iARB                              = nullptr;
      glWindowPos2ivARB                             = nullptr;
      glWindowPos2sARB                              = nullptr;
      glWindowPos2svARB                             = nullptr;
      glWindowPos3dARB                              = nullptr;
      glWindowPos3dvARB                             = nullptr;
      glWindowPos3fARB                              = nullptr;
      glWindowPos3fvARB                             = nullptr;
      glWindowPos3iARB                              = nullptr;
      glWindowPos3ivARB                             = nullptr;
      glWindowPos3sARB                              = nullptr;
      glWindowPos3svARB                             = nullptr;
      //                                            
      // GL_EXT_blend_color                         
      //                                            
      glBlendColorEXT                               = nullptr;
      //                                            
      // GL_EXT_blend_func_separate                 
      //                                            
      glBlendFuncSeparateEXT                        = nullptr;
      //                                    
      // GL_EXT_fog_coord                   ( Deprecated in OpenGL 3.0 Core )
      //                                    
      glFogCoordfEXT                                = nullptr;
      glFogCoordfvEXT                               = nullptr;
      glFogCoorddEXT                                = nullptr;
      glFogCoorddvEXT                               = nullptr;
      glFogCoordPointerEXT                          = nullptr;                                                                                                               
      //                                                                       
      // GL_EXT_multi_draw_arrays                                              
      //                                                                       
      glMultiDrawArraysEXT                          = nullptr;
      glMultiDrawElementsEXT                        = nullptr;
      //
      // GL_EXT_secondary_color             ( Deprecated in OpenGL 3.0 Core )
      //
      glSecondaryColor3bEXT                         = nullptr;
      glSecondaryColor3bvEXT                        = nullptr;
      glSecondaryColor3dEXT                         = nullptr;
      glSecondaryColor3dvEXT                        = nullptr;
      glSecondaryColor3fEXT                         = nullptr;
      glSecondaryColor3fvEXT                        = nullptr;
      glSecondaryColor3iEXT                         = nullptr;
      glSecondaryColor3ivEXT                        = nullptr;
      glSecondaryColor3sEXT                         = nullptr;
      glSecondaryColor3svEXT                        = nullptr;
      glSecondaryColor3ubEXT                        = nullptr;
      glSecondaryColor3ubvEXT                       = nullptr;
      glSecondaryColor3uiEXT                        = nullptr;
      glSecondaryColor3uivEXT                       = nullptr;
      glSecondaryColor3usEXT                        = nullptr;
      glSecondaryColor3usvEXT                       = nullptr;
      glSecondaryColorPointerEXT                    = nullptr;
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
      glActiveTexture                               = nullptr;
      glClientActiveTexture                         = nullptr;
      glCompressedTexImage1D                        = nullptr;
      glCompressedTexImage2D                        = nullptr;
      glCompressedTexImage3D                        = nullptr;
      glCompressedTexSubImage1D                     = nullptr;
      glCompressedTexSubImage2D                     = nullptr;
      glCompressedTexSubImage3D                     = nullptr;
      glGetCompressedTexImage                       = nullptr;
      glLoadTransposeMatrixf                        = nullptr;
      glLoadTransposeMatrixd                        = nullptr;
      glMultTransposeMatrixf                        = nullptr;
      glMultTransposeMatrixd                        = nullptr;
      glMultiTexCoord1d                             = nullptr;
      glMultiTexCoord1dv                            = nullptr;
      glMultiTexCoord1f                             = nullptr;
      glMultiTexCoord1fv                            = nullptr;
      glMultiTexCoord1i                             = nullptr;
      glMultiTexCoord1iv                            = nullptr;
      glMultiTexCoord1s                             = nullptr;
      glMultiTexCoord1sv                            = nullptr;
      glMultiTexCoord2d                             = nullptr;
      glMultiTexCoord2dv                            = nullptr;
      glMultiTexCoord2f                             = nullptr;
      glMultiTexCoord2fv                            = nullptr;
      glMultiTexCoord2i                             = nullptr;
      glMultiTexCoord2iv                            = nullptr;
      glMultiTexCoord2s                             = nullptr;
      glMultiTexCoord2sv                            = nullptr;
      glMultiTexCoord3d                             = nullptr;
      glMultiTexCoord3dv                            = nullptr;
      glMultiTexCoord3f                             = nullptr;
      glMultiTexCoord3fv                            = nullptr;
      glMultiTexCoord3i                             = nullptr;
      glMultiTexCoord3iv                            = nullptr;
      glMultiTexCoord3s                             = nullptr;
      glMultiTexCoord3sv                            = nullptr;
      glMultiTexCoord4d                             = nullptr;
      glMultiTexCoord4dv                            = nullptr;
      glMultiTexCoord4f                             = nullptr;
      glMultiTexCoord4fv                            = nullptr;
      glMultiTexCoord4i                             = nullptr;
      glMultiTexCoord4iv                            = nullptr;
      glMultiTexCoord4s                             = nullptr;
      glMultiTexCoord4sv                            = nullptr;
      glSampleCoverage                              = nullptr;
      //                                            
      // GL_ARB_multisample                         
      //                                            
      glSampleCoverageARB                           = nullptr;
      //
      // GL_ARB_multitexture                ( Deprecated in OpenGL 3.0 Core )
      // 
      glActiveTextureARB                            = nullptr;
      glClientActiveTextureARB                      = nullptr;
      glMultiTexCoord1dARB                          = nullptr;
      glMultiTexCoord1dvARB                         = nullptr;
      glMultiTexCoord1fARB                          = nullptr;
      glMultiTexCoord1fvARB                         = nullptr;
      glMultiTexCoord1iARB                          = nullptr;
      glMultiTexCoord1ivARB                         = nullptr;
      glMultiTexCoord1sARB                          = nullptr;
      glMultiTexCoord1svARB                         = nullptr;
      glMultiTexCoord2dARB                          = nullptr;
      glMultiTexCoord2dvARB                         = nullptr;
      glMultiTexCoord2fARB                          = nullptr;
      glMultiTexCoord2fvARB                         = nullptr;
      glMultiTexCoord2iARB                          = nullptr;
      glMultiTexCoord2ivARB                         = nullptr;
      glMultiTexCoord2sARB                          = nullptr;
      glMultiTexCoord2svARB                         = nullptr;
      glMultiTexCoord3dARB                          = nullptr;
      glMultiTexCoord3dvARB                         = nullptr;
      glMultiTexCoord3fARB                          = nullptr;
      glMultiTexCoord3fvARB                         = nullptr;
      glMultiTexCoord3iARB                          = nullptr;
      glMultiTexCoord3ivARB                         = nullptr;
      glMultiTexCoord3sARB                          = nullptr;
      glMultiTexCoord3svARB                         = nullptr;
      glMultiTexCoord4dARB                          = nullptr;
      glMultiTexCoord4dvARB                         = nullptr;
      glMultiTexCoord4fARB                          = nullptr;
      glMultiTexCoord4fvARB                         = nullptr;
      glMultiTexCoord4iARB                          = nullptr;
      glMultiTexCoord4ivARB                         = nullptr;
      glMultiTexCoord4sARB                          = nullptr;
      glMultiTexCoord4svARB                         = nullptr;
      //                                            
      // GL_ARB_texture_compression                 
      //                                            
      glCompressedTexImage1DARB                     = nullptr;
      glCompressedTexImage2DARB                     = nullptr;
      glCompressedTexImage3DARB                     = nullptr;
      glCompressedTexSubImage1DARB                  = nullptr;
      glCompressedTexSubImage2DARB                  = nullptr;
      glCompressedTexSubImage3DARB                  = nullptr;
      glGetCompressedTexImageARB                    = nullptr;
      //
      // GL_ARB_transpose_matrix            ( Deprecated in OpenGL 3.0 Core )
      //
      glLoadTransposeMatrixfARB                     = nullptr;
      glLoadTransposeMatrixdARB                     = nullptr;
      glMultTransposeMatrixfARB                     = nullptr;
      glMultTransposeMatrixdARB                     = nullptr;
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
      glCopyTexSubImage3D                           = nullptr;
      glDrawRangeElements                           = nullptr;
      glTexImage3D                                  = nullptr;
      glTexSubImage3D                               = nullptr;
      //                                                                     
      // GL_EXT_draw_range_elements                                          
      //                                                                     
      glDrawRangeElementsEXT                        = nullptr;
      //                                            
      // GL_EXT_texture3D                           
      //                                            
      glTexImage3DEXT                               = nullptr;
      }
      
      void UnbindWglFunctionsPointers(void)
      {
      //
      // WGL_ARB_create_context
      //
      wglCreateContextAttribsARB                    = nullptr;
      //                                            
      // WGL_ARB_pixel_format                       
      //                                            
      wglGetPixelFormatAttribivARB                  = nullptr;
      wglGetPixelFormatAttribfvARB                  = nullptr;
      wglChoosePixelFormatARB                       = nullptr;
      //                                            
      // WGL_EXT_pixel_format                       
      //                                            
      wglGetPixelFormatAttribivEXT                  = nullptr;
      wglGetPixelFormatAttribfvEXT                  = nullptr;
      wglChoosePixelFormatEXT                       = nullptr;
      //                                            
      // WGL_ARB_extensions_string                  
      //                                            
      wglGetExtensionsStringARB                     = nullptr;
      //                                            
      // WGL_EXT_extensions_string                  
      //                                            
      wglGetExtensionsStringEXT                     = nullptr;
      }

      #endif
      }
   }
}

