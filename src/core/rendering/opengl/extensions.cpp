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
#if defined(EN_PLATFORM_WINDOWS)
PFNGLNAMEDBUFFERSTORAGEEXTPROC              glNamedBufferStorageEXT              = nullptr;
#endif
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
#if defined(EN_PLATFORM_WINDOWS)
PFNGLCLEARNAMEDBUFFERDATAEXTPROC            glClearNamedBufferDataEXT            = nullptr;
PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC         glClearNamedBufferSubDataEXT         = nullptr;
#endif
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
#if defined(EN_PLATFORM_WINDOWS)
PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC      glNamedFramebufferParameteriEXT      = nullptr;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC  glGetNamedFramebufferParameterivEXT  = nullptr;
#endif
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
#if defined(EN_PLATFORM_WINDOWS)
PFNGLTEXTUREBUFFERRANGEEXTPROC              glTextureBufferRangeEXT              = nullptr;
#endif
//                                          
//   GL_ARB_texture_query_levels            
//                                          
string                                      ARB_texture_query_levels             = "GL_ARB_texture_query_levels";
//                                          
//   GL_ARB_texture_storage_multisample     
//                                          
string                                      ARB_texture_storage_multisample      = "GL_ARB_texture_storage_multisample";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC     glTextureStorage2DMultisampleEXT     = nullptr;
PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC     glTextureStorage3DMultisampleEXT     = nullptr;
#endif
//                                          
//   GL_ARB_texture_view                    
//                                          
string                                      ARB_texture_view                     = "GL_ARB_texture_view";
//
//   GL_ARB_vertex_attrib_binding
//
string                                      ARB_vertex_attrib_binding            = "GL_ARB_vertex_attrib_binding";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC     glVertexArrayBindVertexBufferEXT     = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC   glVertexArrayVertexAttribFormatEXT   = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC  glVertexArrayVertexAttribIFormatEXT  = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC  glVertexArrayVertexAttribLFormatEXT  = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC  glVertexArrayVertexAttribBindingEXT  = nullptr;
PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC glVertexArrayVertexBindingDivisorEXT = nullptr;
#endif
//
//   GL_KHR_debug
//
string                                      KHR_debug                            = "GL_KHR_debug";
//
// GL_ARB_base_instance
//
string                                     ARB_base_instance                     = "GL_ARB_base_instance";
//
// GL_ARB_conservative_depth
//
string                                     ARB_conservative_depth                = "GL_ARB_conservative_depth";
//
// GL_ARB_internalformat_query
//
string                                     ARB_internalformat_query              = "GL_ARB_internalformat_query";
//
// GL_ARB_map_buffer_aligment
//
string                                     ARB_map_buffer_aligment               = "GL_ARB_map_buffer_aligment";
//
// GL_ARB_shader_atomic_counters
//
string                                     ARB_shader_atomic_counters            = "GL_ARB_shader_atomic_counters";
//
// GL_ARB_shader_image_load_store
//
string                                     ARB_shader_image_load_store           = "GL_ARB_shader_image_load_store";
//
// GL_ARB_shading_language_420pack
//
string                                     ARB_shading_language_420pack          = "GL_ARB_shading_language_420pack";
//
// GL_ARB_shading_language_packing
//
string                                     ARB_shading_language_packing          = "GL_ARB_shading_language_packing";
//
// GL_ARB_texture_compression_bptc
//
string                                     ARB_texture_compression_bptc          = "GL_ARB_texture_compression_bptc";
//
// GL_ARB_texture_storage
//
string                                     ARB_texture_storage                   = "GL_ARB_texture_storage";
//
// GL_ARB_transform_feedback_instanced
//
string                                     ARB_transform_feedback_instanced      = "GL_ARB_transform_feedback_instanced";
//
// GL_ARB_debug_output
//
string                                     ARB_debug_output                      = "GL_ARB_debug_output";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLDEBUGMESSAGECALLBACKARBPROC           glDebugMessageCallbackARB             = nullptr;
PFNGLDEBUGMESSAGECONTROLARBPROC            glDebugMessageControlARB              = nullptr;
PFNGLDEBUGMESSAGEINSERTARBPROC             glDebugMessageInsertARB               = nullptr;
PFNGLGETDEBUGMESSAGELOGARBPROC             glGetDebugMessageLogARB               = nullptr;
#endif
//
// GL_ARB_ES2_compatibility
//
string                                     ARB_ES2_compatibility                 = "GL_ARB_ES2_compatibility";
//
// GL_ARB_get_program_binary
//
string                                     ARB_get_program_binary                = "GL_ARB_get_program_binary";
//
// GL_ARB_separate_shader_objects
//
string                                     ARB_separate_shader_objects           = "GL_ARB_separate_shader_objects";
//
// GL_ARB_vertex_attrib_64bit
//
string                                     ARB_vertex_attrib_64bit               = "GL_ARB_vertex_attrib_64bit";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glVertexArrayVertexAttribLOffsetEXT   = nullptr;
#endif
//
// GL_ARB_viewport_array
//
string                                     ARB_viewport_array                    = "GL_ARB_viewport_array";
//
// GL_ARB_draw_buffers_blend
//
string                                     ARB_draw_buffers_blend                = "GL_ARB_draw_buffers_blend";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBLENDEQUATIONIARBPROC                 glBlendEquationiARB                   = nullptr;
PFNGLBLENDEQUATIONSEPARATEIARBPROC         glBlendEquationSeparateiARB           = nullptr;
PFNGLBLENDFUNCIARBPROC                     glBlendFunciARB                       = nullptr;
PFNGLBLENDFUNCSEPARATEIARBPROC             glBlendFuncSeparateiARB               = nullptr;
#endif
//   
// GL_ARB_draw_indirect
//
string                                     ARB_draw_indirect                     = "GL_ARB_draw_indirect";
//   
// GL_ARB_gpu_shader_fp64   
//
string                                     ARB_gpu_shader_fp64                   = "GL_ARB_gpu_shader_fp64";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLPROGRAMUNIFORM1DEXTPROC               glProgramUniform1dEXT                 = nullptr;
PFNGLPROGRAMUNIFORM1DVEXTPROC              glProgramUniform1dvEXT                = nullptr;
PFNGLPROGRAMUNIFORM2DEXTPROC               glProgramUniform2dEXT                 = nullptr;
PFNGLPROGRAMUNIFORM2DVEXTPROC              glProgramUniform2dvEXT                = nullptr;
PFNGLPROGRAMUNIFORM3DEXTPROC               glProgramUniform3dEXT                 = nullptr;
PFNGLPROGRAMUNIFORM3DVEXTPROC              glProgramUniform3dvEXT                = nullptr;
PFNGLPROGRAMUNIFORM4DEXTPROC               glProgramUniform4dEXT                 = nullptr;
PFNGLPROGRAMUNIFORM4DVEXTPROC              glProgramUniform4dvEXT                = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC        glProgramUniformMatrix2dvEXT          = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC        glProgramUniformMatrix3dvEXT          = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC        glProgramUniformMatrix4dvEXT          = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC      glProgramUniformMatrix2x3dvEXT        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC      glProgramUniformMatrix2x4dvEXT        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC      glProgramUniformMatrix3x2dvEXT        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC      glProgramUniformMatrix3x4dvEXT        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC      glProgramUniformMatrix4x2dvEXT        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC      glProgramUniformMatrix4x3dvEXT        = nullptr;
#endif
//
// GL_ARB_sample_shading
//
string                                     ARB_sample_shading                    = "GL_ARB_sample_shading";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLMINSAMPLESHADINGARBPROC               glMinSampleShadingARB                 = nullptr;
#endif
//   
// GL_ARB_shader_subroutine
//
string                                     ARB_shader_subroutine                 = "GL_ARB_shader_subroutine";
//   
// GL_ARB_tessellation_shader
//
string                                     ARB_tessellation_shader               = "GL_ARB_tessellation_shader";
//   
// GL_ARB_transform_feedback2
//
string                                     ARB_transform_feedback2               = "GL_ARB_transform_feedback2";
//   
// GL_ARB_blend_func_extended
//
string                                     ARB_blend_func_extended               = "GL_ARB_blend_func_extended";
//   
// GL_ARB_sampler_objects
//
string                                     ARB_sampler_objects                   = "GL_ARB_sampler_objects";
//   
// GL_ARB_timer_query
//
string                                     ARB_timer_query                       = "GL_ARB_timer_query";
//   
// GL_ARB_vertex_type_2_10_10_10_rev
//
string                                     ARB_vertex_type_2_10_10_10_rev        = "GL_ARB_vertex_type_2_10_10_10_rev";
//   
// GL_ARB_draw_elements_base_vertex
//
string                                     ARB_draw_elements_base_vertex         = "GL_ARB_draw_elements_base_vertex";
//
// GL_ARB_geometry_shader4
//
string                                     ARB_geometry_shader4                  = "GL_ARB_geometry_shader4";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLPROGRAMPARAMETERIARBPROC              glProgramParameteriARB                = nullptr;
PFNGLFRAMEBUFFERTEXTUREARBPROC             glFramebufferTextureARB               = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERARBPROC        glFramebufferTextureLayerARB          = nullptr;
PFNGLFRAMEBUFFERTEXTUREFACEARBPROC         glFramebufferTextureFaceARB           = nullptr;
#endif
//   
// GL_ARB_provoking_vertex
//
string                                     ARB_provoking_vertex                  = "GL_ARB_provoking_vertex";
//   
// GL_ARB_sync
//
string                                     ARB_sync                              = "GL_ARB_sync";
//   
// GL_ARB_texture_multisample
//
string                                     ARB_texture_multisample               = "GL_ARB_texture_multisample";
//   
// GL_ARB_copy_buffer
//
string                                     ARB_copy_buffer                       = "GL_ARB_copy_buffer";
//
// GL_ARB_draw_instanced
//
string                                     ARB_draw_instanced                    = "GL_ARB_draw_instanced";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLDRAWARRAYSINSTANCEDARBPROC            glDrawArraysInstancedARB              = nullptr;
PFNGLDRAWELEMENTSINSTANCEDARBPROC          glDrawElementsInstancedARB            = nullptr;
#endif
//
// GL_ARB_texture_buffer_object
//
string                                     ARB_texture_buffer_object             = "GL_ARB_texture_buffer_object";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLTEXBUFFERARBPROC                      glTexBufferARB                        = nullptr;
#endif
//   
// GL_ARB_uniform_buffer_object
//
string                                     ARB_uniform_buffer_object             = "GL_ARB_uniform_buffer_object";
//
// GL_NV_primitive_restart
//
string                                     NV_primitive_restart                  = "GL_NV_primitive_restart";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLPRIMITIVERESTARTNVPROC                glPrimitiveRestartNV                  = nullptr;
PFNGLPRIMITIVERESTARTINDEXNVPROC           glPrimitiveRestartIndexNV             = nullptr;
#endif
//
// GL_ARB_color_buffer_float
//
string                                     ARB_color_buffer_float                = "GL_ARB_color_buffer_float";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLCLAMPCOLORARBPROC                     glClampColorARB                       = nullptr;
#endif
//
// GL_APPLE_flush_buffer_range          ( predecessor of GL_ARB_map_buffer_range )
//
string                                     APPLE_flush_buffer_range              = "GL_APPLE_flush_buffer_range";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBUFFERPARAMETERIAPPLEPROC             glBufferParameteriAPPLE               = nullptr;
PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC       glFlushMappedBufferRangeAPPLE         = nullptr;
#endif
//
// GL_ARB_framebuffer_object
//
string                                     ARB_framebuffer_object                = "GL_ARB_framebuffer_object";
//
// GL_ARB_map_buffer_range
//
string                                     ARB_map_buffer_range                  = "GL_ARB_map_buffer_range";
//
// GL_ARB_vertex_array_object
//
string                                     ARB_vertex_array_object               = "GL_ARB_vertex_array_object";
//
// GL_APPLE_vertex_array_object         ( predecessor of GL_ARB_vertex_array_object )
//
string                                     APPLE_vertex_array_object             = "GL_APPLE_vertex_array_object";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBINDVERTEXARRAYAPPLEPROC              glBindVertexArrayAPPLE                = nullptr;    
PFNGLDELETEVERTEXARRAYSAPPLEPROC           glDeleteVertexArraysAPPLE             = nullptr; 
PFNGLGENVERTEXARRAYSAPPLEPROC              glGenVertexArraysAPPLE                = nullptr;    
PFNGLISVERTEXARRAYAPPLEPROC                glIsVertexArrayAPPLE                  = nullptr; 
#endif     
//
// GL_EXT_draw_buffers2
//
string                                     EXT_draw_buffers2                     = "GL_EXT_draw_buffers2";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLCOLORMASKINDEXEDEXTPROC               glColorMaskIndexedEXT                 = nullptr;   
PFNGLDISABLEINDEXEDEXTPROC                 glDisableIndexedEXT                   = nullptr;     
PFNGLENABLEINDEXEDEXTPROC                  glEnableIndexedEXT                    = nullptr;      
PFNGLGETBOOLEANINDEXEDVEXTPROC             glGetBooleanIndexedvEXT               = nullptr; 
PFNGLGETINTEGERINDEXEDVEXTPROC             glGetIntegerIndexedvEXT               = nullptr; 
PFNGLISENABLEDINDEXEDEXTPROC               glIsEnabledIndexedEXT                 = nullptr;   
#endif
//
// GL_EXT_framebuffer_blit
//
string                                     EXT_framebuffer_blit                  = "GL_EXT_framebuffer_blit";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBLITFRAMEBUFFEREXTPROC                glBlitFramebufferEXT                  = nullptr;
#endif
//
// GL_EXT_framebuffer_multisample
//
string                                     EXT_framebuffer_multisample           = "GL_EXT_framebuffer_multisample";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT   = nullptr; 
#endif
//
// GL_EXT_gpu_shader4
//
string                                     EXT_gpu_shader4                       = "GL_EXT_gpu_shader4";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBINDFRAGDATALOCATIONEXTPROC           glBindFragDataLocationEXT             = nullptr;
PFNGLGETFRAGDATALOCATIONEXTPROC            glGetFragDataLocationEXT              = nullptr;
PFNGLGETUNIFORMUIVEXTPROC                  glGetUniformuivEXT                    = nullptr;
PFNGLGETVERTEXATTRIBIIVEXTPROC             glGetVertexAttribIivEXT               = nullptr;
PFNGLGETVERTEXATTRIBIUIVEXTPROC            glGetVertexAttribIuivEXT              = nullptr;
PFNGLUNIFORM1UIEXTPROC                     glUniform1uiEXT                       = nullptr;
PFNGLUNIFORM1UIVEXTPROC                    glUniform1uivEXT                      = nullptr;
PFNGLUNIFORM2UIEXTPROC                     glUniform2uiEXT                       = nullptr;
PFNGLUNIFORM2UIVEXTPROC                    glUniform2uivEXT                      = nullptr;
PFNGLUNIFORM3UIEXTPROC                     glUniform3uiEXT                       = nullptr;
PFNGLUNIFORM3UIVEXTPROC                    glUniform3uivEXT                      = nullptr;
PFNGLUNIFORM4UIEXTPROC                     glUniform4uiEXT                       = nullptr;
PFNGLUNIFORM4UIVEXTPROC                    glUniform4uivEXT                      = nullptr;
PFNGLVERTEXATTRIBI1IEXTPROC                glVertexAttribI1iEXT                  = nullptr;
PFNGLVERTEXATTRIBI1IVEXTPROC               glVertexAttribI1ivEXT                 = nullptr;
PFNGLVERTEXATTRIBI1UIEXTPROC               glVertexAttribI1uiEXT                 = nullptr;
PFNGLVERTEXATTRIBI1UIVEXTPROC              glVertexAttribI1uivEXT                = nullptr;
PFNGLVERTEXATTRIBI2IEXTPROC                glVertexAttribI2iEXT                  = nullptr;
PFNGLVERTEXATTRIBI2IVEXTPROC               glVertexAttribI2ivEXT                 = nullptr;
PFNGLVERTEXATTRIBI2UIEXTPROC               glVertexAttribI2uiEXT                 = nullptr;
PFNGLVERTEXATTRIBI2UIVEXTPROC              glVertexAttribI2uivEXT                = nullptr;
PFNGLVERTEXATTRIBI3IEXTPROC                glVertexAttribI3iEXT                  = nullptr;
PFNGLVERTEXATTRIBI3IVEXTPROC               glVertexAttribI3ivEXT                 = nullptr;
PFNGLVERTEXATTRIBI3UIEXTPROC               glVertexAttribI3uiEXT                 = nullptr;
PFNGLVERTEXATTRIBI3UIVEXTPROC              glVertexAttribI3uivEXT                = nullptr;
PFNGLVERTEXATTRIBI4BVEXTPROC               glVertexAttribI4bvEXT                 = nullptr;
PFNGLVERTEXATTRIBI4IEXTPROC                glVertexAttribI4iEXT                  = nullptr;
PFNGLVERTEXATTRIBI4IVEXTPROC               glVertexAttribI4ivEXT                 = nullptr;
PFNGLVERTEXATTRIBI4SVEXTPROC               glVertexAttribI4svEXT                 = nullptr;
PFNGLVERTEXATTRIBI4UBVEXTPROC              glVertexAttribI4ubvEXT                = nullptr;
PFNGLVERTEXATTRIBI4UIEXTPROC               glVertexAttribI4uiEXT                 = nullptr;
PFNGLVERTEXATTRIBI4UIVEXTPROC              glVertexAttribI4uivEXT                = nullptr;
PFNGLVERTEXATTRIBI4USVEXTPROC              glVertexAttribI4usvEXT                = nullptr;
PFNGLVERTEXATTRIBIPOINTEREXTPROC           glVertexAttribIPointerEXT             = nullptr;
#endif
//
// GL_EXT_texture_array
//
string                                     EXT_texture_array                     = "GL_EXT_texture_array";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC        glFramebufferTextureLayerEXT          = nullptr;
#endif
//
// GL_EXT_texture_integer
//
string                                     EXT_texture_integer                   = "GL_EXT_texture_integer";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLCLEARCOLORIIEXTPROC                   glClearColorIiEXT                     = nullptr;
PFNGLCLEARCOLORIUIEXTPROC                  glClearColorIuiEXT                    = nullptr;
PFNGLGETTEXPARAMETERIIVEXTPROC             glGetTexParameterIivEXT               = nullptr;
PFNGLGETTEXPARAMETERIUIVEXTPROC            glGetTexParameterIuivEXT              = nullptr;
PFNGLTEXPARAMETERIIVEXTPROC                glTexParameterIivEXT                  = nullptr;
PFNGLTEXPARAMETERIUIVEXTPROC               glTexParameterIuivEXT                 = nullptr;
#endif
//                                                                               
// GL_EXT_transform_feedback                                                     
//                                                                               
string                                     EXT_transform_feedback                = "GL_EXT_transform_feedback";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBEGINTRANSFORMFEEDBACKEXTPROC         glBeginTransformFeedbackEXT           = nullptr;
PFNGLBINDBUFFERBASEEXTPROC                 glBindBufferBaseEXT                   = nullptr;
PFNGLBINDBUFFEROFFSETEXTPROC               glBindBufferOffsetEXT                 = nullptr;
PFNGLBINDBUFFERRANGEEXTPROC                glBindBufferRangeEXT                  = nullptr;
PFNGLENDTRANSFORMFEEDBACKEXTPROC           glEndTransformFeedbackEXT             = nullptr;
PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC    glGetTransformFeedbackVaryingEXT      = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC      glTransformFeedbackVaryingsEXT        = nullptr;
#endif
//
// GL_NV_transform_feedback             ( predecessor of GL_EXT_transform_feedback )
//
string                                     NV_transform_feedback                 = "GL_NV_transform_feedback";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLACTIVEVARYINGNVPROC                   glActiveVaryingNV                     = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKNVPROC          glBeginTransformFeedbackNV            = nullptr;
PFNGLBINDBUFFERBASENVPROC                  glBindBufferBaseNV                    = nullptr;
PFNGLBINDBUFFEROFFSETNVPROC                glBindBufferOffsetNV                  = nullptr;
PFNGLBINDBUFFERRANGENVPROC                 glBindBufferRangeNV                   = nullptr;
PFNGLENDTRANSFORMFEEDBACKNVPROC            glEndTransformFeedbackNV              = nullptr;
PFNGLGETACTIVEVARYINGNVPROC                glGetActiveVaryingNV                  = nullptr;
PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC     glGetTransformFeedbackVaryingNV       = nullptr;
PFNGLGETVARYINGLOCATIONNVPROC              glGetVaryingLocationNV                = nullptr;
PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC        glTransformFeedbackAttribsNV          = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC       glTransformFeedbackVaryingsNV         = nullptr;
#endif
//                                                    
// GL_NV_conditional_render                           
//   
string                                     NV_conditional_render                 = "GL_NV_conditional_render";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLBEGINCONDITIONALRENDERNVPROC          glBeginConditionalRenderNV            = nullptr;
PFNGLENDCONDITIONALRENDERNVPROC            glEndConditionalRenderNV              = nullptr;
#endif
//
// GL_ARB_draw_buffers
//
string                                     ARB_draw_buffers                      = "GL_ARB_draw_buffers";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLDRAWBUFFERSARBPROC                    glDrawBuffersARB                      = nullptr;
#endif
//
// GL_ARB_shader_objects
//
string                                     ARB_shader_objects                    = "GL_ARB_shader_objects";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLATTACHOBJECTARBPROC                   glAttachObjectARB                     = nullptr;             
PFNGLCOMPILESHADERARBPROC                  glCompileShaderARB                    = nullptr;            
PFNGLCREATEPROGRAMOBJECTARBPROC            glCreateProgramObjectARB              = nullptr;      
PFNGLCREATESHADEROBJECTARBPROC             glCreateShaderObjectARB               = nullptr;       
PFNGLDELETEOBJECTARBPROC                   glDeleteObjectARB                     = nullptr;             
PFNGLDETACHOBJECTARBPROC                   glDetachObjectARB                     = nullptr;             
PFNGLGETACTIVEUNIFORMARBPROC               glGetActiveUniformARB                 = nullptr;         
PFNGLGETATTACHEDOBJECTSARBPROC             glGetAttachedObjectsARB               = nullptr;       
PFNGLGETHANDLEARBPROC                      glGetHandleARB                        = nullptr;                
PFNGLGETINFOLOGARBPROC                     glGetInfoLogARB                       = nullptr;               
PFNGLGETOBJECTPARAMETERFVARBPROC           glGetObjectParameterfvARB             = nullptr;     
PFNGLGETOBJECTPARAMETERIVARBPROC           glGetObjectParameterivARB             = nullptr;     
PFNGLGETUNIFORMFVARBPROC                   glGetUniformfvARB                     = nullptr;             
PFNGLGETUNIFORMIVARBPROC                   glGetUniformivARB                     = nullptr;             
PFNGLGETUNIFORMLOCATIONARBPROC             glGetUniformLocationARB               = nullptr;       
PFNGLGETSHADERSOURCEARBPROC                glGetShaderSourceARB                  = nullptr;          
PFNGLLINKPROGRAMARBPROC                    glLinkProgramARB                      = nullptr;              
PFNGLSHADERSOURCEARBPROC                   glShaderSourceARB                     = nullptr;             
PFNGLUNIFORM1FARBPROC                      glUniform1fARB                        = nullptr;                
PFNGLUNIFORM1FVARBPROC                     glUniform1fvARB                       = nullptr;               
PFNGLUNIFORM1IARBPROC                      glUniform1iARB                        = nullptr;                
PFNGLUNIFORM1IVARBPROC                     glUniform1ivARB                       = nullptr;               
PFNGLUNIFORM2FARBPROC                      glUniform2fARB                        = nullptr;                
PFNGLUNIFORM2FVARBPROC                     glUniform2fvARB                       = nullptr;               
PFNGLUNIFORM2IARBPROC                      glUniform2iARB                        = nullptr;                
PFNGLUNIFORM2IVARBPROC                     glUniform2ivARB                       = nullptr;               
PFNGLUNIFORM3FARBPROC                      glUniform3fARB                        = nullptr;                
PFNGLUNIFORM3FVARBPROC                     glUniform3fvARB                       = nullptr;               
PFNGLUNIFORM3IARBPROC                      glUniform3iARB                        = nullptr;                
PFNGLUNIFORM3IVARBPROC                     glUniform3ivARB                       = nullptr;               
PFNGLUNIFORM4FARBPROC                      glUniform4fARB                        = nullptr;                
PFNGLUNIFORM4FVARBPROC                     glUniform4fvARB                       = nullptr;               
PFNGLUNIFORM4IARBPROC                      glUniform4iARB                        = nullptr;                
PFNGLUNIFORM4IVARBPROC                     glUniform4ivARB                       = nullptr;               
PFNGLUNIFORMMATRIX2FVARBPROC               glUniformMatrix2fvARB                 = nullptr;         
PFNGLUNIFORMMATRIX3FVARBPROC               glUniformMatrix3fvARB                 = nullptr;         
PFNGLUNIFORMMATRIX4FVARBPROC               glUniformMatrix4fvARB                 = nullptr;         
PFNGLUSEPROGRAMOBJECTARBPROC               glUseProgramObjectARB                 = nullptr;         
PFNGLVALIDATEPROGRAMARBPROC                glValidateProgramARB                  = nullptr;
#endif
//
// GL_ARB_vertex_shader
//
string                                     ARB_vertex_shader                     = "GL_ARB_vertex_shader";
#if defined(EN_PLATFORM_WINDOWS)                                                
PFNGLBINDATTRIBLOCATIONARBPROC             glBindAttribLocationARB               = nullptr;       
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC       glDisableVertexAttribArrayARB         = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC        glEnableVertexAttribArrayARB          = nullptr;
PFNGLGETACTIVEATTRIBARBPROC                glGetActiveAttribARB                  = nullptr;          
PFNGLGETATTRIBLOCATIONARBPROC              glGetAttribLocationARB                = nullptr;        
PFNGLGETVERTEXATTRIBDVARBPROC              glGetVertexAttribdvARB                = nullptr;        
PFNGLGETVERTEXATTRIBFVARBPROC              glGetVertexAttribfvARB                = nullptr;       
PFNGLGETVERTEXATTRIBIVARBPROC              glGetVertexAttribivARB                = nullptr;        
PFNGLGETVERTEXATTRIBPOINTERVARBPROC        glGetVertexAttribPointervARB          = nullptr;
PFNGLVERTEXATTRIB1DARBPROC                 glVertexAttrib1dARB                   = nullptr;           
PFNGLVERTEXATTRIB1DVARBPROC                glVertexAttrib1dvARB                  = nullptr;          
PFNGLVERTEXATTRIB1FARBPROC                 glVertexAttrib1fARB                   = nullptr;           
PFNGLVERTEXATTRIB1FVARBPROC                glVertexAttrib1fvARB                  = nullptr;          
PFNGLVERTEXATTRIB1SARBPROC                 glVertexAttrib1sARB                   = nullptr;           
PFNGLVERTEXATTRIB1SVARBPROC                glVertexAttrib1svARB                  = nullptr;          
PFNGLVERTEXATTRIB2DARBPROC                 glVertexAttrib2dARB                   = nullptr;           
PFNGLVERTEXATTRIB2DVARBPROC                glVertexAttrib2dvARB                  = nullptr;          
PFNGLVERTEXATTRIB2FARBPROC                 glVertexAttrib2fARB                   = nullptr;           
PFNGLVERTEXATTRIB2FVARBPROC                glVertexAttrib2fvARB                  = nullptr;          
PFNGLVERTEXATTRIB2SARBPROC                 glVertexAttrib2sARB                   = nullptr;          
PFNGLVERTEXATTRIB2SVARBPROC                glVertexAttrib2svARB                  = nullptr;          
PFNGLVERTEXATTRIB3DARBPROC                 glVertexAttrib3dARB                   = nullptr;           
PFNGLVERTEXATTRIB3DVARBPROC                glVertexAttrib3dvARB                  = nullptr;          
PFNGLVERTEXATTRIB3FARBPROC                 glVertexAttrib3fARB                   = nullptr;           
PFNGLVERTEXATTRIB3FVARBPROC                glVertexAttrib3fvARB                  = nullptr;          
PFNGLVERTEXATTRIB3SARBPROC                 glVertexAttrib3sARB                   = nullptr;           
PFNGLVERTEXATTRIB3SVARBPROC                glVertexAttrib3svARB                  = nullptr;          
PFNGLVERTEXATTRIB4NBVARBPROC               glVertexAttrib4NbvARB                 = nullptr;         
PFNGLVERTEXATTRIB4NIVARBPROC               glVertexAttrib4NivARB                 = nullptr;         
PFNGLVERTEXATTRIB4NSVARBPROC               glVertexAttrib4NsvARB                 = nullptr;         
PFNGLVERTEXATTRIB4NUBARBPROC               glVertexAttrib4NubARB                 = nullptr;         
PFNGLVERTEXATTRIB4NUBVARBPROC              glVertexAttrib4NubvARB                = nullptr;        
PFNGLVERTEXATTRIB4NUIVARBPROC              glVertexAttrib4NuivARB                = nullptr;        
PFNGLVERTEXATTRIB4NUSVARBPROC              glVertexAttrib4NusvARB                = nullptr;       
PFNGLVERTEXATTRIB4BVARBPROC                glVertexAttrib4bvARB                  = nullptr;          
PFNGLVERTEXATTRIB4DARBPROC                 glVertexAttrib4dARB                   = nullptr;           
PFNGLVERTEXATTRIB4DVARBPROC                glVertexAttrib4dvARB                  = nullptr;          
PFNGLVERTEXATTRIB4FARBPROC                 glVertexAttrib4fARB                   = nullptr;           
PFNGLVERTEXATTRIB4FVARBPROC                glVertexAttrib4fvARB                  = nullptr;          
PFNGLVERTEXATTRIB4IVARBPROC                glVertexAttrib4ivARB                  = nullptr;          
PFNGLVERTEXATTRIB4SARBPROC                 glVertexAttrib4sARB                   = nullptr;           
PFNGLVERTEXATTRIB4SVARBPROC                glVertexAttrib4svARB                  = nullptr;          
PFNGLVERTEXATTRIB4UBVARBPROC               glVertexAttrib4ubvARB                 = nullptr;         
PFNGLVERTEXATTRIB4UIVARBPROC               glVertexAttrib4uivARB                 = nullptr;         
PFNGLVERTEXATTRIB4USVARBPROC               glVertexAttrib4usvARB                 = nullptr;         
PFNGLVERTEXATTRIBPOINTERARBPROC            glVertexAttribPointerARB              = nullptr;
#endif      
//
// GL_EXT_blend_equation_separate
//
string                                     EXT_blend_equation_separate           = "GL_EXT_blend_equation_separate";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLBLENDEQUATIONSEPARATEEXTPROC          glBlendEquationSeparateEXT            = nullptr;
#endif
//
// GL_EXT_stencil_two_side
//
string                                     EXT_stencil_two_side                  = "GL_EXT_stencil_two_side";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLACTIVESTENCILFACEEXTPROC              glActiveStencilFaceEXT                = nullptr;
#endif
//                                                                     
// GL_ARB_occlusion_query                                              
//  
string                                     ARB_occlusion_query                   = "GL_ARB_occlusion_query";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLBEGINQUERYARBPROC                     glBeginQueryARB                       = nullptr;
PFNGLDELETEQUERIESARBPROC                  glDeleteQueriesARB                    = nullptr;
PFNGLENDQUERYARBPROC                       glEndQueryARB                         = nullptr;
PFNGLGENQUERIESARBPROC                     glGenQueriesARB                       = nullptr;
PFNGLGETQUERYIVARBPROC                     glGetQueryivARB                       = nullptr;
PFNGLGETQUERYOBJECTIVARBPROC               glGetQueryObjectivARB                 = nullptr;
PFNGLGETQUERYOBJECTUIVARBPROC              glGetQueryObjectuivARB                = nullptr;
PFNGLISQUERYARBPROC                        glIsQueryARB                          = nullptr;
#endif
//                                                                     
// GL_ARB_vertex_buffer_object                                         
//   
string                                     ARB_vertex_buffer_object              = "GL_ARB_vertex_buffer_object";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLBINDBUFFERARBPROC                     glBindBufferARB                       = nullptr;          
PFNGLBUFFERDATAARBPROC                     glBufferDataARB                       = nullptr;          
PFNGLBUFFERSUBDATAARBPROC                  glBufferSubDataARB                    = nullptr;       
PFNGLDELETEBUFFERSARBPROC                  glDeleteBuffersARB                    = nullptr;       
PFNGLGENBUFFERSARBPROC                     glGenBuffersARB                       = nullptr;          
PFNGLGETBUFFERPARAMETERIVARBPROC           glGetBufferParameterivARB             = nullptr;
PFNGLGETBUFFERPOINTERVARBPROC              glGetBufferPointervARB                = nullptr;   
PFNGLGETBUFFERSUBDATAARBPROC               glGetBufferSubDataARB                 = nullptr;    
PFNGLISBUFFERARBPROC                       glIsBufferARB                         = nullptr;            
PFNGLMAPBUFFERARBPROC                      glMapBufferARB                        = nullptr;           
PFNGLUNMAPBUFFERARBPROC                    glUnmapBufferARB                      = nullptr;  
#endif       
//
// GL_ARB_point_parameters
//
string                                     ARB_point_parameters                  = "GL_ARB_point_parameters";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLPOINTPARAMETERFARBPROC                glPointParameterfARB                  = nullptr; 
PFNGLPOINTPARAMETERFVARBPROC               glPointParameterfvARB                 = nullptr;
#endif
//
// GL_ARB_window_pos            ( Deprecated in OpenGL 3.0 Core )
//
string                                     ARB_window_pos                        = "GL_ARB_window_pos";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLWINDOWPOS2DARBPROC                    glWindowPos2dARB                      = nullptr; 
PFNGLWINDOWPOS2DVARBPROC                   glWindowPos2dvARB                     = nullptr;
PFNGLWINDOWPOS2FARBPROC                    glWindowPos2fARB                      = nullptr;
PFNGLWINDOWPOS2FVARBPROC                   glWindowPos2fvARB                     = nullptr;
PFNGLWINDOWPOS2IARBPROC                    glWindowPos2iARB                      = nullptr;
PFNGLWINDOWPOS2IVARBPROC                   glWindowPos2ivARB                     = nullptr;
PFNGLWINDOWPOS2SARBPROC                    glWindowPos2sARB                      = nullptr; 
PFNGLWINDOWPOS2SVARBPROC                   glWindowPos2svARB                     = nullptr;
PFNGLWINDOWPOS3DARBPROC                    glWindowPos3dARB                      = nullptr; 
PFNGLWINDOWPOS3DVARBPROC                   glWindowPos3dvARB                     = nullptr;
PFNGLWINDOWPOS3FARBPROC                    glWindowPos3fARB                      = nullptr; 
PFNGLWINDOWPOS3FVARBPROC                   glWindowPos3fvARB                     = nullptr;
PFNGLWINDOWPOS3IARBPROC                    glWindowPos3iARB                      = nullptr; 
PFNGLWINDOWPOS3IVARBPROC                   glWindowPos3ivARB                     = nullptr;
PFNGLWINDOWPOS3SARBPROC                    glWindowPos3sARB                      = nullptr;
PFNGLWINDOWPOS3SVARBPROC                   glWindowPos3svARB                     = nullptr;
#endif                                                                           
//
// GL_EXT_blend_color
//
string                                     EXT_blend_color                       = "GL_EXT_blend_color";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLBLENDCOLOREXTPROC                     glBlendColorEXT                       = nullptr;
#endif                                                                           
//                                                                               
// GL_EXT_blend_func_separate                                                    
//                                                                               
string                                     EXT_blend_func_separate               = "GL_EXT_blend_func_separate";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLBLENDFUNCSEPARATEEXTPROC              glBlendFuncSeparateEXT                = nullptr;
#endif
//
// GL_EXT_fog_coord             ( Deprecated in OpenGL 3.0 Core )
//
string                                     EXT_fog_coord                         = "GL_EXT_fog_coord";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLFOGCOORDFEXTPROC                      glFogCoordfEXT                        = nullptr;     
PFNGLFOGCOORDFVEXTPROC                     glFogCoordfvEXT                       = nullptr;     
PFNGLFOGCOORDDEXTPROC                      glFogCoorddEXT                        = nullptr;      
PFNGLFOGCOORDDVEXTPROC                     glFogCoorddvEXT                       = nullptr;     
PFNGLFOGCOORDPOINTEREXTPROC                glFogCoordPointerEXT                  = nullptr;
#endif
//
// GL_EXT_multi_draw_arrays
//
string                                     EXT_multi_draw_arrays                 = "GL_EXT_multi_draw_arrays";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLMULTIDRAWARRAYSEXTPROC                glMultiDrawArraysEXT                  = nullptr;  
PFNGLMULTIDRAWELEMENTSEXTPROC              glMultiDrawElementsEXT                = nullptr;
#endif
//
// GL_EXT_secondary_color       ( Deprecated in OpenGL 3.0 Core )
//
string                                     EXT_secondary_color                   = "GL_EXT_secondary_color";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLSECONDARYCOLOR3BEXTPROC               glSecondaryColor3bEXT                 = nullptr;     
PFNGLSECONDARYCOLOR3BVEXTPROC              glSecondaryColor3bvEXT                = nullptr;    
PFNGLSECONDARYCOLOR3DEXTPROC               glSecondaryColor3dEXT                 = nullptr;     
PFNGLSECONDARYCOLOR3DVEXTPROC              glSecondaryColor3dvEXT                = nullptr;    
PFNGLSECONDARYCOLOR3FEXTPROC               glSecondaryColor3fEXT                 = nullptr;     
PFNGLSECONDARYCOLOR3FVEXTPROC              glSecondaryColor3fvEXT                = nullptr;    
PFNGLSECONDARYCOLOR3IEXTPROC               glSecondaryColor3iEXT                 = nullptr;     
PFNGLSECONDARYCOLOR3IVEXTPROC              glSecondaryColor3ivEXT                = nullptr;    
PFNGLSECONDARYCOLOR3SEXTPROC               glSecondaryColor3sEXT                 = nullptr;     
PFNGLSECONDARYCOLOR3SVEXTPROC              glSecondaryColor3svEXT                = nullptr;    
PFNGLSECONDARYCOLOR3UBEXTPROC              glSecondaryColor3ubEXT                = nullptr;    
PFNGLSECONDARYCOLOR3UBVEXTPROC             glSecondaryColor3ubvEXT               = nullptr;   
PFNGLSECONDARYCOLOR3UIEXTPROC              glSecondaryColor3uiEXT                = nullptr;    
PFNGLSECONDARYCOLOR3UIVEXTPROC             glSecondaryColor3uivEXT               = nullptr;   
PFNGLSECONDARYCOLOR3USEXTPROC              glSecondaryColor3usEXT                = nullptr;    
PFNGLSECONDARYCOLOR3USVEXTPROC             glSecondaryColor3usvEXT               = nullptr;   
PFNGLSECONDARYCOLORPOINTEREXTPROC          glSecondaryColorPointerEXT            = nullptr;
#endif
//
// GL_ARB_multisample
//
string                                     ARB_multisample                       = "GL_ARB_multisample";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLSAMPLECOVERAGEARBPROC                 glSampleCoverageARB                   = nullptr;
#endif
//
// GL_ARB_multitexture         ( Deprecated in OpenGL 3.0 Core )
// 
string                                     ARB_multitexture                      = "GL_ARB_multitexture";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLACTIVETEXTUREARBPROC                  glActiveTextureARB                    = nullptr;   
PFNGLCLIENTACTIVETEXTUREARBPROC            glClientActiveTextureARB              = nullptr;
PFNGLMULTITEXCOORD1DARBPROC                glMultiTexCoord1dARB                  = nullptr;    
PFNGLMULTITEXCOORD1DVARBPROC               glMultiTexCoord1dvARB                 = nullptr;  
PFNGLMULTITEXCOORD1FARBPROC                glMultiTexCoord1fARB                  = nullptr;    
PFNGLMULTITEXCOORD1FVARBPROC               glMultiTexCoord1fvARB                 = nullptr;   
PFNGLMULTITEXCOORD1IARBPROC                glMultiTexCoord1iARB                  = nullptr;    
PFNGLMULTITEXCOORD1IVARBPROC               glMultiTexCoord1ivARB                 = nullptr;   
PFNGLMULTITEXCOORD1SARBPROC                glMultiTexCoord1sARB                  = nullptr;    
PFNGLMULTITEXCOORD1SVARBPROC               glMultiTexCoord1svARB                 = nullptr;   
PFNGLMULTITEXCOORD2DARBPROC                glMultiTexCoord2dARB                  = nullptr;    
PFNGLMULTITEXCOORD2DVARBPROC               glMultiTexCoord2dvARB                 = nullptr;   
PFNGLMULTITEXCOORD2FARBPROC                glMultiTexCoord2fARB                  = nullptr;    
PFNGLMULTITEXCOORD2FVARBPROC               glMultiTexCoord2fvARB                 = nullptr;  
PFNGLMULTITEXCOORD2IARBPROC                glMultiTexCoord2iARB                  = nullptr;    
PFNGLMULTITEXCOORD2IVARBPROC               glMultiTexCoord2ivARB                 = nullptr;   
PFNGLMULTITEXCOORD2SARBPROC                glMultiTexCoord2sARB                  = nullptr;    
PFNGLMULTITEXCOORD2SVARBPROC               glMultiTexCoord2svARB                 = nullptr;   
PFNGLMULTITEXCOORD3DARBPROC                glMultiTexCoord3dARB                  = nullptr;    
PFNGLMULTITEXCOORD3DVARBPROC               glMultiTexCoord3dvARB                 = nullptr;   
PFNGLMULTITEXCOORD3FARBPROC                glMultiTexCoord3fARB                  = nullptr;    
PFNGLMULTITEXCOORD3FVARBPROC               glMultiTexCoord3fvARB                 = nullptr;   
PFNGLMULTITEXCOORD3IARBPROC                glMultiTexCoord3iARB                  = nullptr;    
PFNGLMULTITEXCOORD3IVARBPROC               glMultiTexCoord3ivARB                 = nullptr;   
PFNGLMULTITEXCOORD3SARBPROC                glMultiTexCoord3sARB                  = nullptr;    
PFNGLMULTITEXCOORD3SVARBPROC               glMultiTexCoord3svARB                 = nullptr;  
PFNGLMULTITEXCOORD4DARBPROC                glMultiTexCoord4dARB                  = nullptr;    
PFNGLMULTITEXCOORD4DVARBPROC               glMultiTexCoord4dvARB                 = nullptr;   
PFNGLMULTITEXCOORD4FARBPROC                glMultiTexCoord4fARB                  = nullptr;    
PFNGLMULTITEXCOORD4FVARBPROC               glMultiTexCoord4fvARB                 = nullptr;   
PFNGLMULTITEXCOORD4IARBPROC                glMultiTexCoord4iARB                  = nullptr;    
PFNGLMULTITEXCOORD4IVARBPROC               glMultiTexCoord4ivARB                 = nullptr;   
PFNGLMULTITEXCOORD4SARBPROC                glMultiTexCoord4sARB                  = nullptr;    
PFNGLMULTITEXCOORD4SVARBPROC               glMultiTexCoord4svARB                 = nullptr;  
#endif 
//
// GL_ARB_texture_compression
//
string                                     ARB_texture_compression               = "GL_ARB_texture_compression";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC           glCompressedTexImage1DARB             = nullptr;   
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC           glCompressedTexImage2DARB             = nullptr;   
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC           glCompressedTexImage3DARB             = nullptr;   
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC        glCompressedTexSubImage1DARB          = nullptr; 
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC        glCompressedTexSubImage2DARB          = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC        glCompressedTexSubImage3DARB          = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC          glGetCompressedTexImageARB            = nullptr; 
#endif
//
// GL_ARB_transpose_matrix     ( Deprecated in OpenGL 3.0 Core )
//
string                                     ARB_transpose_matrix                  = "GL_ARB_transpose_matrix";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLLOADTRANSPOSEMATRIXFARBPROC           glLoadTransposeMatrixfARB             = nullptr;
PFNGLLOADTRANSPOSEMATRIXDARBPROC           glLoadTransposeMatrixdARB             = nullptr;
PFNGLMULTTRANSPOSEMATRIXFARBPROC           glMultTransposeMatrixfARB             = nullptr;
PFNGLMULTTRANSPOSEMATRIXDARBPROC           glMultTransposeMatrixdARB             = nullptr;
#endif
//
// GL_EXT_draw_range_elements
//
string                                     EXT_draw_range_elements               = "GL_EXT_draw_range_elements";
#if defined(EN_PLATFORM_WINDOWS)                                                 
PFNGLDRAWRANGEELEMENTSEXTPROC              glDrawRangeElementsEXT                = nullptr;
#endif
//
// GL_EXT_texture3D
//
string                                     EXT_texture3D                         = "GL_EXT_texture3D";
#if defined(EN_PLATFORM_WINDOWS)
PFNGLTEXIMAGE3DEXTPROC                     glTexImage3DEXT                       = nullptr;
#endif

/******************************************************************************

 OpenGL Extensions

******************************************************************************/

//
// GL_AMD_vertex_shader_viewport_index
//
string                                     AMD_vertex_shader_viewport_index    = "GL_AMD_vertex_shader_viewport_index";
//
// GL_EXT_direct_state_access
//
string                                     EXT_direct_state_access             = "GL_EXT_direct_state_access";
//
// GL_EXT_texture_filter_anisotropic
//
string                                     EXT_texture_filter_anisotropic      = "GL_EXT_texture_filter_anisotropic";
//
// NV_geometry_shader_passthrough
//
string                                     NV_geometry_shader_passthrough      = "GL_NV_geometry_shader_passthrough";
//
// GL_NV_viewport_array
//
string                                     NV_viewport_array                   = "GL_NV_viewport_array";
//
// GL_NV_viewport_array2
//
string                                     NV_viewport_array2                  = "GL_NV_viewport_array2";

/******************************************************************************

 Windows Extensions for OpenGL 

******************************************************************************/

#if defined(EN_PLATFORM_WINDOWS)
//
// WGL_ARB_create_context
//
string                                     ARB_create_context                  = "WGL_ARB_create_context";
PFNWGLCREATECONTEXTATTRIBSARBPROC          wglCreateContextAttribsARB          = nullptr;
//
// WGL_ARB_pixel_format
//
string                                     ARB_pixel_format                    = "WGL_ARB_pixel_format";
PFNWGLGETPIXELFORMATATTRIBIVARBPROC        wglGetPixelFormatAttribivARB        = nullptr;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC        wglGetPixelFormatAttribfvARB        = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC             wglChoosePixelFormatARB             = nullptr;
//
// WGL_EXT_pixel_format
//
string                                     EXT_pixel_format                    = "WGL_EXT_pixel_format";
PFNWGLGETPIXELFORMATATTRIBIVEXTPROC        wglGetPixelFormatAttribivEXT        = nullptr;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC        wglGetPixelFormatAttribfvEXT        = nullptr;
PFNWGLCHOOSEPIXELFORMATEXTPROC             wglChoosePixelFormatEXT             = nullptr;
//
// WGL_ARB_extensions_string 
//
string                                     ARB_extensions_string               = "WGL_ARB_extensions_string";
PFNWGLGETEXTENSIONSSTRINGARBPROC           wglGetExtensionsStringARB           = nullptr;
//
// WGL_EXT_extensions_string 
//
string                                     EXT_extensions_string               = "WGL_EXT_extensions_string";
PFNWGLGETEXTENSIONSSTRINGEXTPROC           wglGetExtensionsStringEXT           = nullptr;
//
// WGL_EXT_swap_control
//
string                                     EXT_swap_control                    = "WGL_EXT_swap_control";
PFNWGLSWAPINTERVALEXTPROC                  wglSwapIntervalEXT                  = nullptr;
PFNWGLGETSWAPINTERVALEXTPROC               wglGetSwapIntervalEXT               = nullptr;
#endif