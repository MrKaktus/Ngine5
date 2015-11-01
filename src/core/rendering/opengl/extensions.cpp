/*

 Ngine v5.0
 
 Module      : Extensions declarations.
 Visibility  : Engine only.
 Requirements: none
 Description : Holds declarations of functions that
               are supported via extensions of OpenGL
               and Windows OpenGL extensions.

*/

#include "core/defines.h"
#include "core/rendering/opengl/extensions.h" 

#ifdef EN_PLATFORM_WINDOWS

/******************************************************************************

 OpenGL Core Extensions

******************************************************************************/


//
//   GL_ARB_clip_control
//
string                                      ARB_clip_control                     = "GL_ARB_clip_control";
// 
//   GL_ARB_cull_distance
//
string                                      ARB_cull_distance                    = "GL_ARB_cull_distance";
// 
//   GL_ARB_ES3_1_compatibility
//
string                                      ARB_ES3_1_compatibility              = "GL_ARB_ES3_1_compatibility";
// 
//   GL_ARB_conditional_render_inverted
//
string                                      ARB_conditional_render_inverted      = "GL_ARB_conditional_render_inverted";
// 
//   GL_KHR_context_flush_control
//
string                                      KHR_context_flush_control            = "GL_KHR_context_flush_control";
// 
//   GL_ARB_derivative_control (OpenGL Shading Language only)
//
string                                      ARB_derivative_control               = "GL_ARB_derivative_control";
// 
//   GL_ARB_direct_state_access
//
string                                      ARB_direct_state_access              = "GL_ARB_direct_state_access";
// 
//   GL_ARB_get_texture_sub_image
//
string                                      ARB_get_texture_sub_image            = "GL_ARB_get_texture_sub_image";
// 
//   GL_KHR_robustness
//
string                                      KHR_robustness                       = "GL_KHR_robustness";
// 
//   GL_ARB_shader_texture_image_samples (OpenGL Shading Language only)
//
string                                      ARB_shader_texture_image_samples     = "GL_ARB_shader_texture_image_samples";
// 
//   GL_ARB_texture_barrier
//
string                                      ARB_texture_barrier                  = "GL_ARB_texture_barrier";
//
//   GL_ARB_buffer_storage
//
string                                      ARB_buffer_storage                   = "GL_ARB_buffer_storage";
PFNGLNAMEDBUFFERSTORAGEEXTPROC              glNamedBufferStorageEXT              = nullptr;
//  
//   GL_ARB_clear_texture
//
string                                      ARB_clear_texture                    = "GL_ARB_clear_texture";
//  
//   GL_ARB_enhanced_layouts
//
string                                      ARB_enhanced_layouts                 = "GL_ARB_enhanced_layouts";
//  
//   GL_ARB_multi_bind
//
string                                      ARB_multi_bind                       = "GL_ARB_multi_bind";
//  
//   GL_ARB_query_buffer_object
//
string                                      ARB_query_buffer_object              = "GL_ARB_query_buffer_object";
//  
//   GL_ARB_texture_mirror_clamp_to_edge
//
string                                      ARB_texture_mirror_clamp_to_edge     = "GL_ARB_texture_mirror_clamp_to_edge";
//  
//   GL_ARB_texture_stencil8
//
string                                      ARB_texture_stencil8                 = "GL_ARB_texture_stencil8";
//  
//   GL_ARB_vertex_type_10f_11f_11f_rev
//
string                                      ARB_vertex_type_10f_11f_11f_rev      = "GL_ARB_vertex_type_10f_11f_11f_rev";
//
//   GL_ARB_arrays_of_arrays (OpenGL Shading Language only)
//
string                                      ARB_arrays_of_arrays                 = "GL_ARB_arrays_of_arrays";
//                                         
//   GL_ARB_ES3_compatibility              
//                                         
string                                      ARB_ES3_compatibility                = "GL_ARB_ES3_compatibility";
//                                         
//   GL_ARB_clear_buffer_object            
//                                         
string                                      ARB_clear_buffer_object              = "GL_ARB_clear_buffer_object";
PFNGLCLEARNAMEDBUFFERDATAEXTPROC            glClearNamedBufferDataEXT            = nullptr;
PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC         glClearNamedBufferSubDataEXT         = nullptr;
//                                         
//   GL_ARB_compute_shader                 
//                                         
string                                      ARB_compute_shader                   = "GL_ARB_compute_shader";
//                                         
//   GL_ARB_copy_image                     
//                                         
string                                      ARB_copy_image                       = "GL_ARB_copy_image";
//                                         
//   GL_ARB_explicit_uniform_location      
//                                         
string                                      ARB_explicit_uniform_location        = "GL_ARB_explicit_uniform_location";
//
//   GL_ARB_fragment_layer_viewport (OpenGL Shading Language only)
//
string                                      ARB_fragment_layer_viewport          = "GL_ARB_fragment_layer_viewport";
//                                          
//   GL_ARB_framebuffer_no_attachments      
//                                          
string                                      ARB_framebuffer_no_attachments       = "GL_ARB_framebuffer_no_attachments";
PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC      glNamedFramebufferParameteriEXT      = nullptr;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC  glGetNamedFramebufferParameterivEXT  = nullptr;
//                                          
//   GL_ARB_internalformat_query2                                                
//                                                                               
string                                      ARB_internalformat_query2            = "GL_ARB_internalformat_query2";
//                                                                               
//   GL_ARB_invalidate_subdata                                                   
//                                                                               
string                                      ARB_invalidate_subdata               = "GL_ARB_invalidate_subdata";
//                                                                               
//   GL_ARB_multi_draw_indirect                                                  
//                                                                               
string                                      ARB_multi_draw_indirect              = "GL_ARB_multi_draw_indirect";
//                                                                               
//   GL_ARB_program_interface_query                                              
//                                                                               
string                                      ARB_program_interface_query          = "GL_ARB_program_interface_query";
//                                                                               
//   GL_ARB_robust_buffer_access_behavior                                        
//                                                                               
string                                      ARB_robust_buffer_access_behavior    = "GL_ARB_robust_buffer_access_behavior";
//
//   GL_ARB_shader_image_size (OpenGL Shading Language only)
//
string                                      ARB_shader_image_size                = "GL_ARB_shader_image_size";
//                                          
//   GL_ARB_shader_storage_buffer_object    
//                                          
string                                      ARB_shader_storage_buffer_object     = "GL_ARB_shader_storage_buffer_object";
//                                          
//   GL_ARB_stencil_texturing               
//                                          
string                                      ARB_stencil_texturing                = "GL_ARB_stencil_texturing";
//                                          
//   GL_ARB_texture_buffer_range            
//                                          
string                                      ARB_texture_buffer_range             = "GL_ARB_texture_buffer_range";
PFNGLTEXTUREBUFFERRANGEEXTPROC              glTextureBufferRangeEXT              = nullptr;
//                                          
//   GL_ARB_texture_query_levels            
//                                          
string                                      ARB_texture_query_levels             = "GL_ARB_texture_query_levels";
//                                          
//   GL_ARB_texture_storage_multisample     
//                                          
string                                      ARB_texture_storage_multisample      = "GL_ARB_texture_storage_multisample";
PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC     glTextureStorage2DMultisampleEXT     = nullptr;
PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC     glTextureStorage3DMultisampleEXT     = nullptr;
//                                          
//   GL_ARB_texture_view                    
//                                          
string                                      ARB_texture_view                     = "GL_ARB_texture_view";
//
//   GL_ARB_vertex_attrib_binding
//
string                                      ARB_vertex_attrib_binding            = "GL_ARB_vertex_attrib_binding";
PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC     glVertexArrayBindVertexBufferEXT     = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC   glVertexArrayVertexAttribFormatEXT   = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC  glVertexArrayVertexAttribIFormatEXT  = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC  glVertexArrayVertexAttribLFormatEXT  = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC  glVertexArrayVertexAttribBindingEXT  = nullptr;
PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC glVertexArrayVertexBindingDivisorEXT = nullptr;
//
//   GL_KHR_debug
//
string                                      KHR_debug                            = "GL_KHR_debug";
//
// GL_ARB_base_instance
//
string                                     ARB_base_instance                   = "GL_ARB_base_instance";
//
// GL_ARB_conservative_depth
//
string                                     ARB_conservative_depth              = "GL_ARB_conservative_depth";
//
// GL_ARB_internalformat_query
//
string                                     ARB_internalformat_query            = "GL_ARB_internalformat_query";
//
// GL_ARB_map_buffer_aligment
//
string                                     ARB_map_buffer_aligment             = "GL_ARB_map_buffer_aligment";
//
// GL_ARB_shader_atomic_counters
//
string                                     ARB_shader_atomic_counters          = "GL_ARB_shader_atomic_counters";
//
// GL_ARB_shader_image_load_store
//
string                                     ARB_shader_image_load_store         = "GL_ARB_shader_image_load_store";
//
// GL_ARB_shading_language_420pack
//
string                                     ARB_shading_language_420pack        = "GL_ARB_shading_language_420pack";
//
// GL_ARB_shading_language_packing
//
string                                     ARB_shading_language_packing        = "GL_ARB_shading_language_packing";
//
// GL_ARB_texture_compression_bptc
//
string                                     ARB_texture_compression_bptc        = "GL_ARB_texture_compression_bptc";
//
// GL_ARB_texture_storage
//
string                                     ARB_texture_storage                 = "GL_ARB_texture_storage";
//
// GL_ARB_transform_feedback_instanced
//
string                                     ARB_transform_feedback_instanced    = "GL_ARB_transform_feedback_instanced";
//
// GL_ARB_debug_output
//
string                                     ARB_debug_output                    = "GL_ARB_debug_output";
PFNGLDEBUGMESSAGECALLBACKARBPROC           glDebugMessageCallbackARB           = NULL;
PFNGLDEBUGMESSAGECONTROLARBPROC            glDebugMessageControlARB            = NULL;
PFNGLDEBUGMESSAGEINSERTARBPROC             glDebugMessageInsertARB             = NULL;
PFNGLGETDEBUGMESSAGELOGARBPROC             glGetDebugMessageLogARB             = NULL;
//
// GL_ARB_ES2_compatibility
//
string                                     ARB_ES2_compatibility               = "GL_ARB_ES2_compatibility";
//
// GL_ARB_get_program_binary
//
string                                     ARB_get_program_binary              = "GL_ARB_get_program_binary";
//
// GL_ARB_separate_shader_objects
//
string                                     ARB_separate_shader_objects         = "GL_ARB_separate_shader_objects";
//
// GL_ARB_vertex_attrib_64bit
//
string                                     ARB_vertex_attrib_64bit             = "GL_ARB_vertex_attrib_64bit";
PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glVertexArrayVertexAttribLOffsetEXT = NULL;
//
// GL_ARB_viewport_array
//
string                                     ARB_viewport_array                  = "GL_ARB_viewport_array";
//
// GL_ARB_draw_buffers_blend
//
string                                     ARB_draw_buffers_blend              = "GL_ARB_draw_buffers_blend";
PFNGLBLENDEQUATIONIARBPROC                 glBlendEquationiARB                 = NULL;
PFNGLBLENDEQUATIONSEPARATEIARBPROC         glBlendEquationSeparateiARB         = NULL;
PFNGLBLENDFUNCIARBPROC                     glBlendFunciARB                     = NULL;
PFNGLBLENDFUNCSEPARATEIARBPROC             glBlendFuncSeparateiARB             = NULL;
//   
// GL_ARB_draw_indirect
//
string                                     ARB_draw_indirect                   = "GL_ARB_draw_indirect";
//   
// GL_ARB_gpu_shader_fp64   
//
string                                     ARB_gpu_shader_fp64                 = "GL_ARB_gpu_shader_fp64";
PFNGLPROGRAMUNIFORM1DEXTPROC               glProgramUniform1dEXT               = NULL;
PFNGLPROGRAMUNIFORM1DVEXTPROC              glProgramUniform1dvEXT              = NULL;
PFNGLPROGRAMUNIFORM2DEXTPROC               glProgramUniform2dEXT               = NULL;
PFNGLPROGRAMUNIFORM2DVEXTPROC              glProgramUniform2dvEXT              = NULL;
PFNGLPROGRAMUNIFORM3DEXTPROC               glProgramUniform3dEXT               = NULL;
PFNGLPROGRAMUNIFORM3DVEXTPROC              glProgramUniform3dvEXT              = NULL;
PFNGLPROGRAMUNIFORM4DEXTPROC               glProgramUniform4dEXT               = NULL;
PFNGLPROGRAMUNIFORM4DVEXTPROC              glProgramUniform4dvEXT              = NULL;
PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC        glProgramUniformMatrix2dvEXT        = NULL;
PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC        glProgramUniformMatrix3dvEXT        = NULL;
PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC        glProgramUniformMatrix4dvEXT        = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC      glProgramUniformMatrix2x3dvEXT      = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC      glProgramUniformMatrix2x4dvEXT      = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC      glProgramUniformMatrix3x2dvEXT      = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC      glProgramUniformMatrix3x4dvEXT      = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC      glProgramUniformMatrix4x2dvEXT      = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC      glProgramUniformMatrix4x3dvEXT      = NULL;
//
// GL_ARB_sample_shading
//
string                                     ARB_sample_shading                  = "GL_ARB_sample_shading";
PFNGLMINSAMPLESHADINGARBPROC               glMinSampleShadingARB               = NULL;
//   
// GL_ARB_shader_subroutine
//
string                                     ARB_shader_subroutine               = "GL_ARB_shader_subroutine";
//   
// GL_ARB_tessellation_shader
//
string                                     ARB_tessellation_shader             = "GL_ARB_tessellation_shader";
//   
// GL_ARB_transform_feedback2
//
string                                     ARB_transform_feedback2             = "GL_ARB_transform_feedback2";
//   
// GL_ARB_blend_func_extended
//
string                                     ARB_blend_func_extended             = "GL_ARB_blend_func_extended";
//   
// GL_ARB_sampler_objects
//
string                                     ARB_sampler_objects                 = "GL_ARB_sampler_objects";
//   
// GL_ARB_timer_query
//
string                                     ARB_timer_query                     = "GL_ARB_timer_query";
//   
// GL_ARB_vertex_type_2_10_10_10_rev
//
string                                     ARB_vertex_type_2_10_10_10_rev      = "GL_ARB_vertex_type_2_10_10_10_rev";
//   
// GL_ARB_draw_elements_base_vertex
//
string                                     ARB_draw_elements_base_vertex       = "GL_ARB_draw_elements_base_vertex";
//
// GL_ARB_geometry_shader4
//
string                                     ARB_geometry_shader4                = "GL_ARB_geometry_shader4";
PFNGLPROGRAMPARAMETERIARBPROC              glProgramParameteriARB              = NULL;
PFNGLFRAMEBUFFERTEXTUREARBPROC             glFramebufferTextureARB             = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERARBPROC        glFramebufferTextureLayerARB        = NULL;
PFNGLFRAMEBUFFERTEXTUREFACEARBPROC         glFramebufferTextureFaceARB         = NULL;
//   
// GL_ARB_provoking_vertex
//
string                                     ARB_provoking_vertex                = "GL_ARB_provoking_vertex";
//   
// GL_ARB_sync
//
string                                     ARB_sync                            = "GL_ARB_sync";
//   
// GL_ARB_texture_multisample
//
string                                     ARB_texture_multisample             = "GL_ARB_texture_multisample";
//   
// GL_ARB_copy_buffer
//
string                                     ARB_copy_buffer                     = "GL_ARB_copy_buffer";
//
// GL_ARB_draw_instanced
//
string                                     ARB_draw_instanced                  = "GL_ARB_draw_instanced";
PFNGLDRAWARRAYSINSTANCEDARBPROC            glDrawArraysInstancedARB            = NULL;
PFNGLDRAWELEMENTSINSTANCEDARBPROC          glDrawElementsInstancedARB          = NULL;
//
// GL_ARB_texture_buffer_object
//
string                                     ARB_texture_buffer_object           = "GL_ARB_texture_buffer_object";
PFNGLTEXBUFFERARBPROC                      glTexBufferARB                      = NULL;
//   
// GL_ARB_uniform_buffer_object
//
string                                     ARB_uniform_buffer_object           = "GL_ARB_uniform_buffer_object";
//
// GL_NV_primitive_restart
//
string                                     NV_primitive_restart                = "GL_NV_primitive_restart";
PFNGLPRIMITIVERESTARTNVPROC                glPrimitiveRestartNV                = NULL;
PFNGLPRIMITIVERESTARTINDEXNVPROC           glPrimitiveRestartIndexNV           = NULL;
//
// GL_ARB_color_buffer_float
//
string                                     ARB_color_buffer_float              = "GL_ARB_color_buffer_float";
PFNGLCLAMPCOLORARBPROC                     glClampColorARB                     = NULL;
//
// GL_APPLE_flush_buffer_range          ( predecessor of GL_ARB_map_buffer_range )
//
string                                     APPLE_flush_buffer_range            = "GL_APPLE_flush_buffer_range";
PFNGLBUFFERPARAMETERIAPPLEPROC             glBufferParameteriAPPLE             = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC       glFlushMappedBufferRangeAPPLE       = NULL;
//
// GL_ARB_framebuffer_object
//
string                                     ARB_framebuffer_object              = "GL_ARB_framebuffer_object";
//
// GL_ARB_map_buffer_range
//
string                                     ARB_map_buffer_range                = "GL_ARB_map_buffer_range";
//
// GL_ARB_vertex_array_object
//
string                                     ARB_vertex_array_object             = "GL_ARB_vertex_array_object";
//
// GL_APPLE_vertex_array_object         ( predecessor of GL_ARB_vertex_array_object )
//
string                                     APPLE_vertex_array_object           = "GL_APPLE_vertex_array_object";
PFNGLBINDVERTEXARRAYAPPLEPROC              glBindVertexArrayAPPLE              = NULL;    
PFNGLDELETEVERTEXARRAYSAPPLEPROC           glDeleteVertexArraysAPPLE           = NULL; 
PFNGLGENVERTEXARRAYSAPPLEPROC              glGenVertexArraysAPPLE              = NULL;    
PFNGLISVERTEXARRAYAPPLEPROC                glIsVertexArrayAPPLE                = NULL;      
//
// GL_EXT_draw_buffers2
//
string                                     EXT_draw_buffers2                   = "GL_EXT_draw_buffers2";
PFNGLCOLORMASKINDEXEDEXTPROC               glColorMaskIndexedEXT               = NULL;   
PFNGLDISABLEINDEXEDEXTPROC                 glDisableIndexedEXT                 = NULL;     
PFNGLENABLEINDEXEDEXTPROC                  glEnableIndexedEXT                  = NULL;      
PFNGLGETBOOLEANINDEXEDVEXTPROC             glGetBooleanIndexedvEXT             = NULL; 
PFNGLGETINTEGERINDEXEDVEXTPROC             glGetIntegerIndexedvEXT             = NULL; 
PFNGLISENABLEDINDEXEDEXTPROC               glIsEnabledIndexedEXT               = NULL;   
//
// GL_EXT_framebuffer_blit
//
string                                     EXT_framebuffer_blit                = "GL_EXT_framebuffer_blit";
PFNGLBLITFRAMEBUFFEREXTPROC                glBlitFramebufferEXT                = NULL;
//
// GL_EXT_framebuffer_multisample
//
string                                     EXT_framebuffer_multisample         = "GL_EXT_framebuffer_multisample";
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT = NULL; 
//
// GL_EXT_gpu_shader4
//
string                                     EXT_gpu_shader4                     = "GL_EXT_gpu_shader4";
PFNGLBINDFRAGDATALOCATIONEXTPROC           glBindFragDataLocationEXT           = NULL;
PFNGLGETFRAGDATALOCATIONEXTPROC            glGetFragDataLocationEXT            = NULL;
PFNGLGETUNIFORMUIVEXTPROC                  glGetUniformuivEXT                  = NULL;
PFNGLGETVERTEXATTRIBIIVEXTPROC             glGetVertexAttribIivEXT             = NULL;
PFNGLGETVERTEXATTRIBIUIVEXTPROC            glGetVertexAttribIuivEXT            = NULL;
PFNGLUNIFORM1UIEXTPROC                     glUniform1uiEXT                     = NULL;
PFNGLUNIFORM1UIVEXTPROC                    glUniform1uivEXT                    = NULL;
PFNGLUNIFORM2UIEXTPROC                     glUniform2uiEXT                     = NULL;
PFNGLUNIFORM2UIVEXTPROC                    glUniform2uivEXT                    = NULL;
PFNGLUNIFORM3UIEXTPROC                     glUniform3uiEXT                     = NULL;
PFNGLUNIFORM3UIVEXTPROC                    glUniform3uivEXT                    = NULL;
PFNGLUNIFORM4UIEXTPROC                     glUniform4uiEXT                     = NULL;
PFNGLUNIFORM4UIVEXTPROC                    glUniform4uivEXT                    = NULL;
PFNGLVERTEXATTRIBI1IEXTPROC                glVertexAttribI1iEXT                = NULL;
PFNGLVERTEXATTRIBI1IVEXTPROC               glVertexAttribI1ivEXT               = NULL;
PFNGLVERTEXATTRIBI1UIEXTPROC               glVertexAttribI1uiEXT               = NULL;
PFNGLVERTEXATTRIBI1UIVEXTPROC              glVertexAttribI1uivEXT              = NULL;
PFNGLVERTEXATTRIBI2IEXTPROC                glVertexAttribI2iEXT                = NULL;
PFNGLVERTEXATTRIBI2IVEXTPROC               glVertexAttribI2ivEXT               = NULL;
PFNGLVERTEXATTRIBI2UIEXTPROC               glVertexAttribI2uiEXT               = NULL;
PFNGLVERTEXATTRIBI2UIVEXTPROC              glVertexAttribI2uivEXT              = NULL;
PFNGLVERTEXATTRIBI3IEXTPROC                glVertexAttribI3iEXT                = NULL;
PFNGLVERTEXATTRIBI3IVEXTPROC               glVertexAttribI3ivEXT               = NULL;
PFNGLVERTEXATTRIBI3UIEXTPROC               glVertexAttribI3uiEXT               = NULL;
PFNGLVERTEXATTRIBI3UIVEXTPROC              glVertexAttribI3uivEXT              = NULL;
PFNGLVERTEXATTRIBI4BVEXTPROC               glVertexAttribI4bvEXT               = NULL;
PFNGLVERTEXATTRIBI4IEXTPROC                glVertexAttribI4iEXT                = NULL;
PFNGLVERTEXATTRIBI4IVEXTPROC               glVertexAttribI4ivEXT               = NULL;
PFNGLVERTEXATTRIBI4SVEXTPROC               glVertexAttribI4svEXT               = NULL;
PFNGLVERTEXATTRIBI4UBVEXTPROC              glVertexAttribI4ubvEXT              = NULL;
PFNGLVERTEXATTRIBI4UIEXTPROC               glVertexAttribI4uiEXT               = NULL;
PFNGLVERTEXATTRIBI4UIVEXTPROC              glVertexAttribI4uivEXT              = NULL;
PFNGLVERTEXATTRIBI4USVEXTPROC              glVertexAttribI4usvEXT              = NULL;
PFNGLVERTEXATTRIBIPOINTEREXTPROC           glVertexAttribIPointerEXT           = NULL;
//
// GL_EXT_texture_array
//
string                                     EXT_texture_array                   = "GL_EXT_texture_array";
PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC        glFramebufferTextureLayerEXT        = NULL;
//
// GL_EXT_texture_integer
//
string                                     EXT_texture_integer                 = "GL_EXT_texture_integer";
PFNGLCLEARCOLORIIEXTPROC                   glClearColorIiEXT                   = NULL;
PFNGLCLEARCOLORIUIEXTPROC                  glClearColorIuiEXT                  = NULL;
PFNGLGETTEXPARAMETERIIVEXTPROC             glGetTexParameterIivEXT             = NULL;
PFNGLGETTEXPARAMETERIUIVEXTPROC            glGetTexParameterIuivEXT            = NULL;
PFNGLTEXPARAMETERIIVEXTPROC                glTexParameterIivEXT                = NULL;
PFNGLTEXPARAMETERIUIVEXTPROC               glTexParameterIuivEXT               = NULL;
//
// GL_EXT_transform_feedback 
//
string                                     EXT_transform_feedback              = "GL_EXT_transform_feedback";
PFNGLBEGINTRANSFORMFEEDBACKEXTPROC         glBeginTransformFeedbackEXT         = NULL;
PFNGLBINDBUFFERBASEEXTPROC                 glBindBufferBaseEXT                 = NULL;
PFNGLBINDBUFFEROFFSETEXTPROC               glBindBufferOffsetEXT               = NULL;
PFNGLBINDBUFFERRANGEEXTPROC                glBindBufferRangeEXT                = NULL;
PFNGLENDTRANSFORMFEEDBACKEXTPROC           glEndTransformFeedbackEXT           = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC    glGetTransformFeedbackVaryingEXT    = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC      glTransformFeedbackVaryingsEXT      = NULL;
//
// GL_NV_transform_feedback             ( predecessor of GL_EXT_transform_feedback )
//
string                                     NV_transform_feedback               = "GL_NV_transform_feedback";
PFNGLACTIVEVARYINGNVPROC                   glActiveVaryingNV                   = NULL;
PFNGLBEGINTRANSFORMFEEDBACKNVPROC          glBeginTransformFeedbackNV          = NULL;
PFNGLBINDBUFFERBASENVPROC                  glBindBufferBaseNV                  = NULL;
PFNGLBINDBUFFEROFFSETNVPROC                glBindBufferOffsetNV                = NULL;
PFNGLBINDBUFFERRANGENVPROC                 glBindBufferRangeNV                 = NULL;
PFNGLENDTRANSFORMFEEDBACKNVPROC            glEndTransformFeedbackNV            = NULL;
PFNGLGETACTIVEVARYINGNVPROC                glGetActiveVaryingNV                = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC     glGetTransformFeedbackVaryingNV     = NULL;
PFNGLGETVARYINGLOCATIONNVPROC              glGetVaryingLocationNV              = NULL;
PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC        glTransformFeedbackAttribsNV        = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC       glTransformFeedbackVaryingsNV       = NULL;
//                                                    
// GL_NV_conditional_render                           
//   
string                                     NV_conditional_render               = "GL_NV_conditional_render";
PFNGLBEGINCONDITIONALRENDERNVPROC          glBeginConditionalRenderNV          = NULL;
PFNGLENDCONDITIONALRENDERNVPROC            glEndConditionalRenderNV            = NULL;
//
// GL_ARB_draw_buffers
//
string                                     ARB_draw_buffers                    = "GL_ARB_draw_buffers";
PFNGLDRAWBUFFERSARBPROC                    glDrawBuffersARB                    = NULL;
//
// GL_ARB_shader_objects
//
string                                     ARB_shader_objects                  = "GL_ARB_shader_objects";
PFNGLATTACHOBJECTARBPROC                   glAttachObjectARB                   = NULL;             
PFNGLCOMPILESHADERARBPROC                  glCompileShaderARB                  = NULL;            
PFNGLCREATEPROGRAMOBJECTARBPROC            glCreateProgramObjectARB            = NULL;      
PFNGLCREATESHADEROBJECTARBPROC             glCreateShaderObjectARB             = NULL;       
PFNGLDELETEOBJECTARBPROC                   glDeleteObjectARB                   = NULL;             
PFNGLDETACHOBJECTARBPROC                   glDetachObjectARB                   = NULL;             
PFNGLGETACTIVEUNIFORMARBPROC               glGetActiveUniformARB               = NULL;         
PFNGLGETATTACHEDOBJECTSARBPROC             glGetAttachedObjectsARB             = NULL;       
PFNGLGETHANDLEARBPROC                      glGetHandleARB                      = NULL;                
PFNGLGETINFOLOGARBPROC                     glGetInfoLogARB                     = NULL;               
PFNGLGETOBJECTPARAMETERFVARBPROC           glGetObjectParameterfvARB           = NULL;     
PFNGLGETOBJECTPARAMETERIVARBPROC           glGetObjectParameterivARB           = NULL;     
PFNGLGETUNIFORMFVARBPROC                   glGetUniformfvARB                   = NULL;             
PFNGLGETUNIFORMIVARBPROC                   glGetUniformivARB                   = NULL;             
PFNGLGETUNIFORMLOCATIONARBPROC             glGetUniformLocationARB             = NULL;       
PFNGLGETSHADERSOURCEARBPROC                glGetShaderSourceARB                = NULL;          
PFNGLLINKPROGRAMARBPROC                    glLinkProgramARB                    = NULL;              
PFNGLSHADERSOURCEARBPROC                   glShaderSourceARB                   = NULL;             
PFNGLUNIFORM1FARBPROC                      glUniform1fARB                      = NULL;                
PFNGLUNIFORM1FVARBPROC                     glUniform1fvARB                     = NULL;               
PFNGLUNIFORM1IARBPROC                      glUniform1iARB                      = NULL;                
PFNGLUNIFORM1IVARBPROC                     glUniform1ivARB                     = NULL;               
PFNGLUNIFORM2FARBPROC                      glUniform2fARB                      = NULL;                
PFNGLUNIFORM2FVARBPROC                     glUniform2fvARB                     = NULL;               
PFNGLUNIFORM2IARBPROC                      glUniform2iARB                      = NULL;                
PFNGLUNIFORM2IVARBPROC                     glUniform2ivARB                     = NULL;               
PFNGLUNIFORM3FARBPROC                      glUniform3fARB                      = NULL;                
PFNGLUNIFORM3FVARBPROC                     glUniform3fvARB                     = NULL;               
PFNGLUNIFORM3IARBPROC                      glUniform3iARB                      = NULL;                
PFNGLUNIFORM3IVARBPROC                     glUniform3ivARB                     = NULL;               
PFNGLUNIFORM4FARBPROC                      glUniform4fARB                      = NULL;                
PFNGLUNIFORM4FVARBPROC                     glUniform4fvARB                     = NULL;               
PFNGLUNIFORM4IARBPROC                      glUniform4iARB                      = NULL;                
PFNGLUNIFORM4IVARBPROC                     glUniform4ivARB                     = NULL;               
PFNGLUNIFORMMATRIX2FVARBPROC               glUniformMatrix2fvARB               = NULL;         
PFNGLUNIFORMMATRIX3FVARBPROC               glUniformMatrix3fvARB               = NULL;         
PFNGLUNIFORMMATRIX4FVARBPROC               glUniformMatrix4fvARB               = NULL;         
PFNGLUSEPROGRAMOBJECTARBPROC               glUseProgramObjectARB               = NULL;         
PFNGLVALIDATEPROGRAMARBPROC                glValidateProgramARB                = NULL;          
//
// GL_ARB_vertex_shader
//
string                                     ARB_vertex_shader                   = "GL_ARB_vertex_shader";
PFNGLBINDATTRIBLOCATIONARBPROC             glBindAttribLocationARB             = NULL;       
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC       glDisableVertexAttribArrayARB       = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC        glEnableVertexAttribArrayARB        = NULL;
PFNGLGETACTIVEATTRIBARBPROC                glGetActiveAttribARB                = NULL;          
PFNGLGETATTRIBLOCATIONARBPROC              glGetAttribLocationARB              = NULL;        
PFNGLGETVERTEXATTRIBDVARBPROC              glGetVertexAttribdvARB              = NULL;        
PFNGLGETVERTEXATTRIBFVARBPROC              glGetVertexAttribfvARB              = NULL;       
PFNGLGETVERTEXATTRIBIVARBPROC              glGetVertexAttribivARB              = NULL;        
PFNGLGETVERTEXATTRIBPOINTERVARBPROC        glGetVertexAttribPointervARB        = NULL;
PFNGLVERTEXATTRIB1DARBPROC                 glVertexAttrib1dARB                 = NULL;           
PFNGLVERTEXATTRIB1DVARBPROC                glVertexAttrib1dvARB                = NULL;          
PFNGLVERTEXATTRIB1FARBPROC                 glVertexAttrib1fARB                 = NULL;           
PFNGLVERTEXATTRIB1FVARBPROC                glVertexAttrib1fvARB                = NULL;          
PFNGLVERTEXATTRIB1SARBPROC                 glVertexAttrib1sARB                 = NULL;           
PFNGLVERTEXATTRIB1SVARBPROC                glVertexAttrib1svARB                = NULL;          
PFNGLVERTEXATTRIB2DARBPROC                 glVertexAttrib2dARB                 = NULL;           
PFNGLVERTEXATTRIB2DVARBPROC                glVertexAttrib2dvARB                = NULL;          
PFNGLVERTEXATTRIB2FARBPROC                 glVertexAttrib2fARB                 = NULL;           
PFNGLVERTEXATTRIB2FVARBPROC                glVertexAttrib2fvARB                = NULL;          
PFNGLVERTEXATTRIB2SARBPROC                 glVertexAttrib2sARB                 = NULL;          
PFNGLVERTEXATTRIB2SVARBPROC                glVertexAttrib2svARB                = NULL;          
PFNGLVERTEXATTRIB3DARBPROC                 glVertexAttrib3dARB                 = NULL;           
PFNGLVERTEXATTRIB3DVARBPROC                glVertexAttrib3dvARB                = NULL;          
PFNGLVERTEXATTRIB3FARBPROC                 glVertexAttrib3fARB                 = NULL;           
PFNGLVERTEXATTRIB3FVARBPROC                glVertexAttrib3fvARB                = NULL;          
PFNGLVERTEXATTRIB3SARBPROC                 glVertexAttrib3sARB                 = NULL;           
PFNGLVERTEXATTRIB3SVARBPROC                glVertexAttrib3svARB                = NULL;          
PFNGLVERTEXATTRIB4NBVARBPROC               glVertexAttrib4NbvARB               = NULL;         
PFNGLVERTEXATTRIB4NIVARBPROC               glVertexAttrib4NivARB               = NULL;         
PFNGLVERTEXATTRIB4NSVARBPROC               glVertexAttrib4NsvARB               = NULL;         
PFNGLVERTEXATTRIB4NUBARBPROC               glVertexAttrib4NubARB               = NULL;         
PFNGLVERTEXATTRIB4NUBVARBPROC              glVertexAttrib4NubvARB              = NULL;        
PFNGLVERTEXATTRIB4NUIVARBPROC              glVertexAttrib4NuivARB              = NULL;        
PFNGLVERTEXATTRIB4NUSVARBPROC              glVertexAttrib4NusvARB              = NULL;       
PFNGLVERTEXATTRIB4BVARBPROC                glVertexAttrib4bvARB                = NULL;          
PFNGLVERTEXATTRIB4DARBPROC                 glVertexAttrib4dARB                 = NULL;           
PFNGLVERTEXATTRIB4DVARBPROC                glVertexAttrib4dvARB                = NULL;          
PFNGLVERTEXATTRIB4FARBPROC                 glVertexAttrib4fARB                 = NULL;           
PFNGLVERTEXATTRIB4FVARBPROC                glVertexAttrib4fvARB                = NULL;          
PFNGLVERTEXATTRIB4IVARBPROC                glVertexAttrib4ivARB                = NULL;          
PFNGLVERTEXATTRIB4SARBPROC                 glVertexAttrib4sARB                 = NULL;           
PFNGLVERTEXATTRIB4SVARBPROC                glVertexAttrib4svARB                = NULL;          
PFNGLVERTEXATTRIB4UBVARBPROC               glVertexAttrib4ubvARB               = NULL;         
PFNGLVERTEXATTRIB4UIVARBPROC               glVertexAttrib4uivARB               = NULL;         
PFNGLVERTEXATTRIB4USVARBPROC               glVertexAttrib4usvARB               = NULL;         
PFNGLVERTEXATTRIBPOINTERARBPROC            glVertexAttribPointerARB            = NULL;      
//
// GL_EXT_blend_equation_separate
//
string                                     EXT_blend_equation_separate         = "GL_EXT_blend_equation_separate";
PFNGLBLENDEQUATIONSEPARATEEXTPROC          glBlendEquationSeparateEXT          = NULL;
//
// GL_EXT_stencil_two_side
//
string                                     EXT_stencil_two_side                = "GL_EXT_stencil_two_side";
PFNGLACTIVESTENCILFACEEXTPROC              glActiveStencilFaceEXT              = NULL;
//                                                                     
// GL_ARB_occlusion_query                                              
//  
string                                     ARB_occlusion_query                 = "GL_ARB_occlusion_query";
PFNGLBEGINQUERYARBPROC                     glBeginQueryARB                     = NULL;
PFNGLDELETEQUERIESARBPROC                  glDeleteQueriesARB                  = NULL;
PFNGLENDQUERYARBPROC                       glEndQueryARB                       = NULL;
PFNGLGENQUERIESARBPROC                     glGenQueriesARB                     = NULL;
PFNGLGETQUERYIVARBPROC                     glGetQueryivARB                     = NULL;
PFNGLGETQUERYOBJECTIVARBPROC               glGetQueryObjectivARB               = NULL;
PFNGLGETQUERYOBJECTUIVARBPROC              glGetQueryObjectuivARB              = NULL;
PFNGLISQUERYARBPROC                        glIsQueryARB                        = NULL;
//                                                                     
// GL_ARB_vertex_buffer_object                                         
//   
string                                     ARB_vertex_buffer_object            = "GL_ARB_vertex_buffer_object";
PFNGLBINDBUFFERARBPROC                     glBindBufferARB                     = NULL;          
PFNGLBUFFERDATAARBPROC                     glBufferDataARB                     = NULL;          
PFNGLBUFFERSUBDATAARBPROC                  glBufferSubDataARB                  = NULL;       
PFNGLDELETEBUFFERSARBPROC                  glDeleteBuffersARB                  = NULL;       
PFNGLGENBUFFERSARBPROC                     glGenBuffersARB                     = NULL;          
PFNGLGETBUFFERPARAMETERIVARBPROC           glGetBufferParameterivARB           = NULL;
PFNGLGETBUFFERPOINTERVARBPROC              glGetBufferPointervARB              = NULL;   
PFNGLGETBUFFERSUBDATAARBPROC               glGetBufferSubDataARB               = NULL;    
PFNGLISBUFFERARBPROC                       glIsBufferARB                       = NULL;            
PFNGLMAPBUFFERARBPROC                      glMapBufferARB                      = NULL;           
PFNGLUNMAPBUFFERARBPROC                    glUnmapBufferARB                    = NULL;         
//
// GL_ARB_point_parameters
//
string                                     ARB_point_parameters                = "GL_ARB_point_parameters";
PFNGLPOINTPARAMETERFARBPROC                glPointParameterfARB                = NULL; 
PFNGLPOINTPARAMETERFVARBPROC               glPointParameterfvARB               = NULL;
//
// GL_ARB_window_pos            ( Deprecated in OpenGL 3.0 Core )
//
string                                     ARB_window_pos                      = "GL_ARB_window_pos";
PFNGLWINDOWPOS2DARBPROC                    glWindowPos2dARB                    = NULL; 
PFNGLWINDOWPOS2DVARBPROC                   glWindowPos2dvARB                   = NULL;
PFNGLWINDOWPOS2FARBPROC                    glWindowPos2fARB                    = NULL;
PFNGLWINDOWPOS2FVARBPROC                   glWindowPos2fvARB                   = NULL;
PFNGLWINDOWPOS2IARBPROC                    glWindowPos2iARB                    = NULL;
PFNGLWINDOWPOS2IVARBPROC                   glWindowPos2ivARB                   = NULL;
PFNGLWINDOWPOS2SARBPROC                    glWindowPos2sARB                    = NULL; 
PFNGLWINDOWPOS2SVARBPROC                   glWindowPos2svARB                   = NULL;
PFNGLWINDOWPOS3DARBPROC                    glWindowPos3dARB                    = NULL; 
PFNGLWINDOWPOS3DVARBPROC                   glWindowPos3dvARB                   = NULL;
PFNGLWINDOWPOS3FARBPROC                    glWindowPos3fARB                    = NULL; 
PFNGLWINDOWPOS3FVARBPROC                   glWindowPos3fvARB                   = NULL;
PFNGLWINDOWPOS3IARBPROC                    glWindowPos3iARB                    = NULL; 
PFNGLWINDOWPOS3IVARBPROC                   glWindowPos3ivARB                   = NULL;
PFNGLWINDOWPOS3SARBPROC                    glWindowPos3sARB                    = NULL;
PFNGLWINDOWPOS3SVARBPROC                   glWindowPos3svARB                   = NULL;
//
// GL_EXT_blend_color
//
string                                     EXT_blend_color                     = "GL_EXT_blend_color";
PFNGLBLENDCOLOREXTPROC                     glBlendColorEXT                     = NULL;
//
// GL_EXT_blend_func_separate
//
string                                     EXT_blend_func_separate             = "GL_EXT_blend_func_separate";
PFNGLBLENDFUNCSEPARATEEXTPROC              glBlendFuncSeparateEXT              = NULL;
//
// GL_EXT_fog_coord             ( Deprecated in OpenGL 3.0 Core )
//
string                                     EXT_fog_coord                       = "GL_EXT_fog_coord";
PFNGLFOGCOORDFEXTPROC                      glFogCoordfEXT                      = NULL;     
PFNGLFOGCOORDFVEXTPROC                     glFogCoordfvEXT                     = NULL;     
PFNGLFOGCOORDDEXTPROC                      glFogCoorddEXT                      = NULL;      
PFNGLFOGCOORDDVEXTPROC                     glFogCoorddvEXT                     = NULL;     
PFNGLFOGCOORDPOINTEREXTPROC                glFogCoordPointerEXT                = NULL;
//
// GL_EXT_multi_draw_arrays
//
string                                     EXT_multi_draw_arrays               = "GL_EXT_multi_draw_arrays";
PFNGLMULTIDRAWARRAYSEXTPROC                glMultiDrawArraysEXT                = NULL;  
PFNGLMULTIDRAWELEMENTSEXTPROC              glMultiDrawElementsEXT              = NULL;
//
// GL_EXT_secondary_color       ( Deprecated in OpenGL 3.0 Core )
//
string                                     EXT_secondary_color                 = "GL_EXT_secondary_color";
PFNGLSECONDARYCOLOR3BEXTPROC               glSecondaryColor3bEXT               = NULL;     
PFNGLSECONDARYCOLOR3BVEXTPROC              glSecondaryColor3bvEXT              = NULL;    
PFNGLSECONDARYCOLOR3DEXTPROC               glSecondaryColor3dEXT               = NULL;     
PFNGLSECONDARYCOLOR3DVEXTPROC              glSecondaryColor3dvEXT              = NULL;    
PFNGLSECONDARYCOLOR3FEXTPROC               glSecondaryColor3fEXT               = NULL;     
PFNGLSECONDARYCOLOR3FVEXTPROC              glSecondaryColor3fvEXT              = NULL;    
PFNGLSECONDARYCOLOR3IEXTPROC               glSecondaryColor3iEXT               = NULL;     
PFNGLSECONDARYCOLOR3IVEXTPROC              glSecondaryColor3ivEXT              = NULL;    
PFNGLSECONDARYCOLOR3SEXTPROC               glSecondaryColor3sEXT               = NULL;     
PFNGLSECONDARYCOLOR3SVEXTPROC              glSecondaryColor3svEXT              = NULL;    
PFNGLSECONDARYCOLOR3UBEXTPROC              glSecondaryColor3ubEXT              = NULL;    
PFNGLSECONDARYCOLOR3UBVEXTPROC             glSecondaryColor3ubvEXT             = NULL;   
PFNGLSECONDARYCOLOR3UIEXTPROC              glSecondaryColor3uiEXT              = NULL;    
PFNGLSECONDARYCOLOR3UIVEXTPROC             glSecondaryColor3uivEXT             = NULL;   
PFNGLSECONDARYCOLOR3USEXTPROC              glSecondaryColor3usEXT              = NULL;    
PFNGLSECONDARYCOLOR3USVEXTPROC             glSecondaryColor3usvEXT             = NULL;   
PFNGLSECONDARYCOLORPOINTEREXTPROC          glSecondaryColorPointerEXT          = NULL;
//
// GL_ARB_multisample
//
string                                     ARB_multisample                     = "GL_ARB_multisample";
PFNGLSAMPLECOVERAGEARBPROC                 glSampleCoverageARB                 = NULL;
//
// GL_ARB_multitexture         ( Deprecated in OpenGL 3.0 Core )
// 
string                                     ARB_multitexture                    = "GL_ARB_multitexture";
PFNGLACTIVETEXTUREARBPROC                  glActiveTextureARB                  = NULL;   
PFNGLCLIENTACTIVETEXTUREARBPROC            glClientActiveTextureARB            = NULL;
PFNGLMULTITEXCOORD1DARBPROC                glMultiTexCoord1dARB                = NULL;    
PFNGLMULTITEXCOORD1DVARBPROC               glMultiTexCoord1dvARB               = NULL;  
PFNGLMULTITEXCOORD1FARBPROC                glMultiTexCoord1fARB                = NULL;    
PFNGLMULTITEXCOORD1FVARBPROC               glMultiTexCoord1fvARB               = NULL;   
PFNGLMULTITEXCOORD1IARBPROC                glMultiTexCoord1iARB                = NULL;    
PFNGLMULTITEXCOORD1IVARBPROC               glMultiTexCoord1ivARB               = NULL;   
PFNGLMULTITEXCOORD1SARBPROC                glMultiTexCoord1sARB                = NULL;    
PFNGLMULTITEXCOORD1SVARBPROC               glMultiTexCoord1svARB               = NULL;   
PFNGLMULTITEXCOORD2DARBPROC                glMultiTexCoord2dARB                = NULL;    
PFNGLMULTITEXCOORD2DVARBPROC               glMultiTexCoord2dvARB               = NULL;   
PFNGLMULTITEXCOORD2FARBPROC                glMultiTexCoord2fARB                = NULL;    
PFNGLMULTITEXCOORD2FVARBPROC               glMultiTexCoord2fvARB               = NULL;  
PFNGLMULTITEXCOORD2IARBPROC                glMultiTexCoord2iARB                = NULL;    
PFNGLMULTITEXCOORD2IVARBPROC               glMultiTexCoord2ivARB               = NULL;   
PFNGLMULTITEXCOORD2SARBPROC                glMultiTexCoord2sARB                = NULL;    
PFNGLMULTITEXCOORD2SVARBPROC               glMultiTexCoord2svARB               = NULL;   
PFNGLMULTITEXCOORD3DARBPROC                glMultiTexCoord3dARB                = NULL;    
PFNGLMULTITEXCOORD3DVARBPROC               glMultiTexCoord3dvARB               = NULL;   
PFNGLMULTITEXCOORD3FARBPROC                glMultiTexCoord3fARB                = NULL;    
PFNGLMULTITEXCOORD3FVARBPROC               glMultiTexCoord3fvARB               = NULL;   
PFNGLMULTITEXCOORD3IARBPROC                glMultiTexCoord3iARB                = NULL;    
PFNGLMULTITEXCOORD3IVARBPROC               glMultiTexCoord3ivARB               = NULL;   
PFNGLMULTITEXCOORD3SARBPROC                glMultiTexCoord3sARB                = NULL;    
PFNGLMULTITEXCOORD3SVARBPROC               glMultiTexCoord3svARB               = NULL;  
PFNGLMULTITEXCOORD4DARBPROC                glMultiTexCoord4dARB                = NULL;    
PFNGLMULTITEXCOORD4DVARBPROC               glMultiTexCoord4dvARB               = NULL;   
PFNGLMULTITEXCOORD4FARBPROC                glMultiTexCoord4fARB                = NULL;    
PFNGLMULTITEXCOORD4FVARBPROC               glMultiTexCoord4fvARB               = NULL;   
PFNGLMULTITEXCOORD4IARBPROC                glMultiTexCoord4iARB                = NULL;    
PFNGLMULTITEXCOORD4IVARBPROC               glMultiTexCoord4ivARB               = NULL;   
PFNGLMULTITEXCOORD4SARBPROC                glMultiTexCoord4sARB                = NULL;    
PFNGLMULTITEXCOORD4SVARBPROC               glMultiTexCoord4svARB               = NULL;   
//
// GL_ARB_texture_compression
//
string                                     ARB_texture_compression             = "GL_ARB_texture_compression";
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC           glCompressedTexImage1DARB           = NULL;   
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC           glCompressedTexImage2DARB           = NULL;   
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC           glCompressedTexImage3DARB           = NULL;   
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC        glCompressedTexSubImage1DARB        = NULL; 
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC        glCompressedTexSubImage2DARB        = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC        glCompressedTexSubImage3DARB        = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC          glGetCompressedTexImageARB          = NULL; 
//
// GL_ARB_transpose_matrix     ( Deprecated in OpenGL 3.0 Core )
//
string                                     ARB_transpose_matrix                = "GL_ARB_transpose_matrix";
PFNGLLOADTRANSPOSEMATRIXFARBPROC           glLoadTransposeMatrixfARB           = NULL;
PFNGLLOADTRANSPOSEMATRIXDARBPROC           glLoadTransposeMatrixdARB           = NULL;
PFNGLMULTTRANSPOSEMATRIXFARBPROC           glMultTransposeMatrixfARB           = NULL;
PFNGLMULTTRANSPOSEMATRIXDARBPROC           glMultTransposeMatrixdARB           = NULL;
//
// GL_EXT_draw_range_elements
//
string                                     EXT_draw_range_elements             = "GL_EXT_draw_range_elements";
PFNGLDRAWRANGEELEMENTSEXTPROC              glDrawRangeElementsEXT              = NULL;
//
// GL_EXT_texture3D
//
string                                     EXT_texture3D                       = "GL_EXT_texture3D";
PFNGLTEXIMAGE3DEXTPROC                     glTexImage3DEXT                     = NULL;

/******************************************************************************

 OpenGL Extensions

******************************************************************************/

//
// GL_EXT_direct_state_access
//
string                                     EXT_direct_state_access             = "GL_EXT_direct_state_access";
//
// GL_EXT_texture_filter_anisotropic
//
string                                     EXT_texture_filter_anisotropic      = "GL_EXT_texture_filter_anisotropic";

/******************************************************************************

 Windows Extensions for OpenGL 

******************************************************************************/

//
// WGL_ARB_create_context
//
string                                     ARB_create_context                  = "WGL_ARB_create_context";
PFNWGLCREATECONTEXTATTRIBSARBPROC          wglCreateContextAttribsARB          = NULL;
//
// WGL_ARB_pixel_format
//
string                                     ARB_pixel_format                    = "WGL_ARB_pixel_format";
PFNWGLGETPIXELFORMATATTRIBIVARBPROC        wglGetPixelFormatAttribivARB        = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC        wglGetPixelFormatAttribfvARB        = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC             wglChoosePixelFormatARB             = NULL;
//
// WGL_EXT_pixel_format
//
string                                     EXT_pixel_format                    = "WGL_EXT_pixel_format";
PFNWGLGETPIXELFORMATATTRIBIVEXTPROC        wglGetPixelFormatAttribivEXT        = NULL;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC        wglGetPixelFormatAttribfvEXT        = NULL;
PFNWGLCHOOSEPIXELFORMATEXTPROC             wglChoosePixelFormatEXT             = NULL;
//
// WGL_ARB_extensions_string 
//
string                                     ARB_extensions_string               = "WGL_ARB_extensions_string";
PFNWGLGETEXTENSIONSSTRINGARBPROC           wglGetExtensionsStringARB           = NULL;
//
// WGL_EXT_extensions_string 
//
string                                     EXT_extensions_string               = "WGL_EXT_extensions_string";
PFNWGLGETEXTENSIONSSTRINGEXTPROC           wglGetExtensionsStringEXT           = NULL;
//
// WGL_EXT_swap_control
//
string                                     EXT_swap_control                    = "WGL_EXT_swap_control";
PFNWGLSWAPINTERVALEXTPROC                  wglSwapIntervalEXT                  = NULL;
PFNWGLGETSWAPINTERVALEXTPROC               wglGetSwapIntervalEXT               = NULL;
#endif