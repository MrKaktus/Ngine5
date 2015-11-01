/*

 Ngine v5.0
 
 Module      : Extensions declarations.
 Visibility  : Engine only.
 Requirements: none
 Description : Holds declarations of functions that
               are supported via extensions of OpenGL
               and Windows OpenGL extensions.

*/

#ifndef ENG_RENDERING_EXTENSIONS
#define ENG_RENDERING_EXTENSIONS 

#include "core/defines.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>  
#include <GL/gl.h>                  // OpenGL
#include "opengl/include/glext.h"   // OpenGL Extensions 
#include "opengl/include/wglext.h"  // OpenGL Windows Extensions 

#include <string>
using namespace std;


/******************************************************************************

 OpenGL Core Extensions

******************************************************************************/

//
//   GL_ARB_clip_control
//
extern string                                      ARB_clip_control;
// 
//   GL_ARB_cull_distance
//
extern string                                      ARB_cull_distance;
// 
//   GL_ARB_ES3_1_compatibility
//
extern string                                      ARB_ES3_1_compatibility;
// 
//   GL_ARB_conditional_render_inverted
//
extern string                                      ARB_conditional_render_inverted;
// 
//   GL_KHR_context_flush_control
//
extern string                                      KHR_context_flush_control;
// 
//   GL_ARB_derivative_control (OpenGL Shading Language only)
//
extern string                                      ARB_derivative_control;
// 
//   GL_ARB_direct_state_access
//
extern string                                      ARB_direct_state_access;
// 
//   GL_ARB_get_texture_sub_image
//
extern string                                      ARB_get_texture_sub_image;
// 
//   GL_KHR_robustness
//
extern string                                      KHR_robustness;
// 
//   GL_ARB_shader_texture_image_samples (OpenGL Shading Language only)
//
extern string                                      ARB_shader_texture_image_samples;
// 
//   GL_ARB_texture_barrier
//
extern string                                      ARB_texture_barrier;
// 
//   GL_ARB_buffer_storage
//
extern string                                      ARB_buffer_storage;
extern PFNGLNAMEDBUFFERSTORAGEEXTPROC              glNamedBufferStorageEXT;
//  
//   GL_ARB_clear_texture
//
extern string                                      ARB_clear_texture;
//  
//   GL_ARB_enhanced_layouts
//
extern string                                      ARB_enhanced_layouts;
//  
//   GL_ARB_multi_bind
//
extern string                                      ARB_multi_bind;
//  
//   GL_ARB_query_buffer_object
//
extern string                                      ARB_query_buffer_object;
//  
//   GL_ARB_texture_mirror_clamp_to_edge
//
extern string                                      ARB_texture_mirror_clamp_to_edge;
//  
//   GL_ARB_texture_stencil8
//
extern string                                      ARB_texture_stencil8;
//  
//   GL_ARB_vertex_type_10f_11f_11f_rev
//
extern string                                      ARB_vertex_type_10f_11f_11f_rev;
//
//   GL_ARB_arrays_of_arrays (OpenGL Shading Language only)
//
extern string                                      ARB_arrays_of_arrays;
//                                                 
//   GL_ARB_ES3_compatibility                      
//                                                 
extern string                                      ARB_ES3_compatibility;
//                                                 
//   GL_ARB_clear_buffer_object                    
//                                                 
extern string                                      ARB_clear_buffer_object;
extern PFNGLCLEARNAMEDBUFFERDATAEXTPROC            glClearNamedBufferDataEXT;
extern PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC         glClearNamedBufferSubDataEXT;
//                                                 
//   GL_ARB_compute_shader                         
//                                                 
extern string                                      ARB_compute_shader;
//                                                 
//   GL_ARB_copy_image                             
//                                                 
extern string                                      ARB_copy_image;
//                                                 
//   GL_ARB_explicit_uniform_location              
//                                                 
extern string                                      ARB_explicit_uniform_location;
//
//   GL_ARB_fragment_layer_viewport (OpenGL Shading Language only)
//
extern string                                      ARB_fragment_layer_viewport;
//                                                 
//   GL_ARB_framebuffer_no_attachments             
//                                                 
extern string                                      ARB_framebuffer_no_attachments;
extern PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC      glNamedFramebufferParameteriEXT;
extern PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC  glGetNamedFramebufferParameterivEXT;
//                                                 
//   GL_ARB_internalformat_query2                  
//                                                 
extern string                                      ARB_internalformat_query2;
//                                                 
//   GL_ARB_invalidate_subdata                     
//                                                 
extern string                                      ARB_invalidate_subdata;
//                                                 
//   GL_ARB_multi_draw_indirect                    
//                                                 
extern string                                      ARB_multi_draw_indirect;
//                                                 
//   GL_ARB_program_interface_query                
//                                                 
extern string                                      ARB_program_interface_query;
//                                                 
//   GL_ARB_robust_buffer_access_behavior          
//                                                 
extern string                                      ARB_robust_buffer_access_behavior;
//
//   GL_ARB_shader_image_size (OpenGL Shading Language only)
//
extern string                                      ARB_shader_image_size;
//                                                 
//   GL_ARB_shader_storage_buffer_object           
//                                                 
extern string                                      ARB_shader_storage_buffer_object;
//                                                 
//   GL_ARB_stencil_texturing                      
//                                                 
extern string                                      ARB_stencil_texturing;
//                                                 
//   GL_ARB_texture_buffer_range                   
//                                                 
extern string                                      ARB_texture_buffer_range;
extern PFNGLTEXTUREBUFFERRANGEEXTPROC              glTextureBufferRangeEXT;
//                                                 
//   GL_ARB_texture_query_levels                   
//                                                 
extern string                                      ARB_texture_query_levels;
//                                                 
//   GL_ARB_texture_storage_multisample            
//                                                 
extern string                                      ARB_texture_storage_multisample;
extern PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC     glTextureStorage2DMultisampleEXT;
extern PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC     glTextureStorage3DMultisampleEXT;
//                                                 
//   GL_ARB_texture_view                           
//                                                 
extern string                                      ARB_texture_view;
//                                                 
//   GL_ARB_vertex_attrib_binding                  
//                                                 
extern string                                      ARB_vertex_attrib_binding;
extern PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC     glVertexArrayBindVertexBufferEXT;
extern PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC   glVertexArrayVertexAttribFormatEXT;
extern PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC  glVertexArrayVertexAttribIFormatEXT;
extern PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC  glVertexArrayVertexAttribLFormatEXT;
extern PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC  glVertexArrayVertexAttribBindingEXT;
extern PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC glVertexArrayVertexBindingDivisorEXT;
//
//   GL_KHR_debug
//
extern string                                      KHR_debug;
//
// GL_ARB_base_instance
//
extern string                                     ARB_base_instance;                  
//
// GL_ARB_conservative_depth
//
extern string                                     ARB_conservative_depth;             
//
// GL_ARB_internalformat_query
//
extern string                                     ARB_internalformat_query;           
//
// GL_ARB_map_buffer_aligment
//
extern string                                     ARB_map_buffer_aligment;            
//
// GL_ARB_shader_atomic_counters
//
extern string                                     ARB_shader_atomic_counters;         
//
// GL_ARB_shader_image_load_store
//
extern string                                     ARB_shader_image_load_store;        
//
// GL_ARB_shading_language_420pack
//
extern string                                     ARB_shading_language_420pack;      
//
// GL_ARB_shading_language_packing
//
extern string                                     ARB_shading_language_packing;       
//
// GL_ARB_texture_compression_bptc
//
extern string                                     ARB_texture_compression_bptc;       
//
// GL_ARB_texture_storage
//
extern string                                     ARB_texture_storage;                
//
// GL_ARB_transform_feedback_instanced
//
extern string                                     ARB_transform_feedback_instanced;   
//
// GL_ARB_debug_output
//
extern string                                     ARB_debug_output;
extern PFNGLDEBUGMESSAGECALLBACKARBPROC           glDebugMessageCallbackARB;
extern PFNGLDEBUGMESSAGECONTROLARBPROC            glDebugMessageControlARB;
extern PFNGLDEBUGMESSAGEINSERTARBPROC             glDebugMessageInsertARB;
extern PFNGLGETDEBUGMESSAGELOGARBPROC             glGetDebugMessageLogARB;
//
// GL_ARB_ES2_compatibility
//
extern string                                     ARB_ES2_compatibility;
//
// GL_ARB_get_program_binary
//
extern string                                     ARB_get_program_binary;
//
// GL_ARB_separate_shader_objects
//
extern string                                     ARB_separate_shader_objects;
//
// GL_ARB_vertex_attrib_64bit
//
extern string                                     ARB_vertex_attrib_64bit;
extern PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glVertexArrayVertexAttribLOffsetEXT;
//
// GL_ARB_viewport_array
//
extern string                                     ARB_viewport_array;
//
// GL_ARB_draw_buffers_blend
//
extern string                                     ARB_draw_buffers_blend;
extern PFNGLBLENDEQUATIONIARBPROC                 glBlendEquationiARB;
extern PFNGLBLENDEQUATIONSEPARATEIARBPROC         glBlendEquationSeparateiARB;
extern PFNGLBLENDFUNCIARBPROC                     glBlendFunciARB;
extern PFNGLBLENDFUNCSEPARATEIARBPROC             glBlendFuncSeparateiARB;
//   
// GL_ARB_draw_indirect
//
extern string                                     ARB_draw_indirect;
//   
// GL_ARB_gpu_shader_fp64   
//
extern string                                     ARB_gpu_shader_fp64;
extern PFNGLPROGRAMUNIFORM1DEXTPROC               glProgramUniform1dEXT;
extern PFNGLPROGRAMUNIFORM1DVEXTPROC              glProgramUniform1dvEXT;
extern PFNGLPROGRAMUNIFORM2DEXTPROC               glProgramUniform2dEXT;
extern PFNGLPROGRAMUNIFORM2DVEXTPROC              glProgramUniform2dvEXT;
extern PFNGLPROGRAMUNIFORM3DEXTPROC               glProgramUniform3dEXT;
extern PFNGLPROGRAMUNIFORM3DVEXTPROC              glProgramUniform3dvEXT;
extern PFNGLPROGRAMUNIFORM4DEXTPROC               glProgramUniform4dEXT;
extern PFNGLPROGRAMUNIFORM4DVEXTPROC              glProgramUniform4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC        glProgramUniformMatrix2dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC        glProgramUniformMatrix3dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC        glProgramUniformMatrix4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC      glProgramUniformMatrix2x3dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC      glProgramUniformMatrix2x4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC      glProgramUniformMatrix3x2dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC      glProgramUniformMatrix3x4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC      glProgramUniformMatrix4x2dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC      glProgramUniformMatrix4x3dvEXT;
//
// GL_ARB_sample_shading
//
extern string                                     ARB_sample_shading;
extern PFNGLMINSAMPLESHADINGARBPROC               glMinSampleShadingARB;
//   
// GL_ARB_shader_subroutine
//
extern string                                     ARB_shader_subroutine;
//   
// GL_ARB_tessellation_shader
//
extern string                                     ARB_tessellation_shader;
//   
// GL_ARB_transform_feedback2
//
extern string                                     ARB_transform_feedback2;
//   
// GL_ARB_blend_func_extended
//
extern string                                     ARB_blend_func_extended;
//   
// GL_ARB_sampler_objects
//
extern string                                     ARB_sampler_objects;
//   
// GL_ARB_timer_query
//
extern string                                     ARB_timer_query;
//   
// GL_ARB_vertex_type_2_10_10_10_rev
//
extern string                                     ARB_vertex_type_2_10_10_10_rev;
//   
// GL_ARB_draw_elements_base_vertex
//
extern string                                     ARB_draw_elements_base_vertex;
//
// GL_ARB_geometry_shader4
//
extern string                                     ARB_geometry_shader4;
extern PFNGLPROGRAMPARAMETERIARBPROC              glProgramParameteriARB;
extern PFNGLFRAMEBUFFERTEXTUREARBPROC             glFramebufferTextureARB;
extern PFNGLFRAMEBUFFERTEXTURELAYERARBPROC        glFramebufferTextureLayerARB;
extern PFNGLFRAMEBUFFERTEXTUREFACEARBPROC         glFramebufferTextureFaceARB;
//   
// GL_ARB_provoking_vertex
//
extern string                                     ARB_provoking_vertex;
//   
// GL_ARB_sync
//
extern string                                     ARB_sync;
//   
// GL_ARB_texture_multisample
//
extern string                                     ARB_texture_multisample;
//   
// GL_ARB_copy_buffer
//
extern string                                     ARB_copy_buffer;
//
// GL_ARB_draw_instanced
//
extern string                                     ARB_draw_instanced;
extern PFNGLDRAWARRAYSINSTANCEDARBPROC            glDrawArraysInstancedARB;
extern PFNGLDRAWELEMENTSINSTANCEDARBPROC          glDrawElementsInstancedARB;
//
// GL_ARB_texture_buffer_object
//
extern string                                     ARB_texture_buffer_object;
extern PFNGLTEXBUFFERARBPROC                      glTexBufferARB;
//   
// GL_ARB_uniform_buffer_object
//
extern string                                     ARB_uniform_buffer_object;
//
// GL_NV_primitive_restart
//
extern string                                     NV_primitive_restart;
extern PFNGLPRIMITIVERESTARTNVPROC                glPrimitiveRestartNV;
extern PFNGLPRIMITIVERESTARTINDEXNVPROC           glPrimitiveRestartIndexNV;
//
// GL_ARB_color_buffer_float
//
extern string                                     ARB_color_buffer_float;
extern PFNGLCLAMPCOLORARBPROC                     glClampColorARB;
//
// GL_APPLE_flush_buffer_range                    ( predecessor of GL_ARB_map_buffer_range )
//
extern string                                     APPLE_flush_buffer_range;
extern PFNGLBUFFERPARAMETERIAPPLEPROC             glBufferParameteriAPPLE;
extern PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC       glFlushMappedBufferRangeAPPLE;
//
// GL_ARB_framebuffer_object
//
extern string                                     ARB_framebuffer_object;
//
// GL_ARB_map_buffer_range
//
extern string                                     ARB_map_buffer_range;
//
// GL_ARB_vertex_array_object
//
extern string                                     ARB_vertex_array_object;
//
// GL_APPLE_vertex_array_object                   ( predecessor of GL_ARB_vertex_array_object )
//
extern string                                     APPLE_vertex_array_object;
extern PFNGLBINDVERTEXARRAYAPPLEPROC              glBindVertexArrayAPPLE;    
extern PFNGLDELETEVERTEXARRAYSAPPLEPROC           glDeleteVertexArraysAPPLE; 
extern PFNGLGENVERTEXARRAYSAPPLEPROC              glGenVertexArraysAPPLE;    
extern PFNGLISVERTEXARRAYAPPLEPROC                glIsVertexArrayAPPLE;      
//
// GL_EXT_draw_buffers2
//
extern string                                     EXT_draw_buffers2;
extern PFNGLCOLORMASKINDEXEDEXTPROC               glColorMaskIndexedEXT;   
extern PFNGLDISABLEINDEXEDEXTPROC                 glDisableIndexedEXT;     
extern PFNGLENABLEINDEXEDEXTPROC                  glEnableIndexedEXT;      
extern PFNGLGETBOOLEANINDEXEDVEXTPROC             glGetBooleanIndexedvEXT; 
extern PFNGLGETINTEGERINDEXEDVEXTPROC             glGetIntegerIndexedvEXT; 
extern PFNGLISENABLEDINDEXEDEXTPROC               glIsEnabledIndexedEXT;   
//
// GL_EXT_framebuffer_blit
//
extern string                                     EXT_framebuffer_blit;
extern PFNGLBLITFRAMEBUFFEREXTPROC                glBlitFramebufferEXT;
//
// GL_EXT_framebuffer_multisample
//
extern string                                     EXT_framebuffer_multisample;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT; 
//
// GL_EXT_gpu_shader4
//
extern string                                     EXT_gpu_shader4;
extern PFNGLBINDFRAGDATALOCATIONEXTPROC           glBindFragDataLocationEXT;
extern PFNGLGETFRAGDATALOCATIONEXTPROC            glGetFragDataLocationEXT;
extern PFNGLGETUNIFORMUIVEXTPROC                  glGetUniformuivEXT;
extern PFNGLGETVERTEXATTRIBIIVEXTPROC             glGetVertexAttribIivEXT;
extern PFNGLGETVERTEXATTRIBIUIVEXTPROC            glGetVertexAttribIuivEXT;
extern PFNGLUNIFORM1UIEXTPROC                     glUniform1uiEXT;
extern PFNGLUNIFORM1UIVEXTPROC                    glUniform1uivEXT;
extern PFNGLUNIFORM2UIEXTPROC                     glUniform2uiEXT;
extern PFNGLUNIFORM2UIVEXTPROC                    glUniform2uivEXT;
extern PFNGLUNIFORM3UIEXTPROC                     glUniform3uiEXT;
extern PFNGLUNIFORM3UIVEXTPROC                    glUniform3uivEXT;
extern PFNGLUNIFORM4UIEXTPROC                     glUniform4uiEXT;
extern PFNGLUNIFORM4UIVEXTPROC                    glUniform4uivEXT;
extern PFNGLVERTEXATTRIBI1IEXTPROC                glVertexAttribI1iEXT;
extern PFNGLVERTEXATTRIBI1IVEXTPROC               glVertexAttribI1ivEXT;
extern PFNGLVERTEXATTRIBI1UIEXTPROC               glVertexAttribI1uiEXT;
extern PFNGLVERTEXATTRIBI1UIVEXTPROC              glVertexAttribI1uivEXT;
extern PFNGLVERTEXATTRIBI2IEXTPROC                glVertexAttribI2iEXT;
extern PFNGLVERTEXATTRIBI2IVEXTPROC               glVertexAttribI2ivEXT;
extern PFNGLVERTEXATTRIBI2UIEXTPROC               glVertexAttribI2uiEXT;
extern PFNGLVERTEXATTRIBI2UIVEXTPROC              glVertexAttribI2uivEXT;
extern PFNGLVERTEXATTRIBI3IEXTPROC                glVertexAttribI3iEXT;
extern PFNGLVERTEXATTRIBI3IVEXTPROC               glVertexAttribI3ivEXT;
extern PFNGLVERTEXATTRIBI3UIEXTPROC               glVertexAttribI3uiEXT;
extern PFNGLVERTEXATTRIBI3UIVEXTPROC              glVertexAttribI3uivEXT;
extern PFNGLVERTEXATTRIBI4BVEXTPROC               glVertexAttribI4bvEXT;
extern PFNGLVERTEXATTRIBI4IEXTPROC                glVertexAttribI4iEXT;
extern PFNGLVERTEXATTRIBI4IVEXTPROC               glVertexAttribI4ivEXT;
extern PFNGLVERTEXATTRIBI4SVEXTPROC               glVertexAttribI4svEXT;
extern PFNGLVERTEXATTRIBI4UBVEXTPROC              glVertexAttribI4ubvEXT;
extern PFNGLVERTEXATTRIBI4UIEXTPROC               glVertexAttribI4uiEXT;
extern PFNGLVERTEXATTRIBI4UIVEXTPROC              glVertexAttribI4uivEXT;
extern PFNGLVERTEXATTRIBI4USVEXTPROC              glVertexAttribI4usvEXT;
extern PFNGLVERTEXATTRIBIPOINTEREXTPROC           glVertexAttribIPointerEXT;
//
// GL_EXT_texture_array
//
extern string                                     EXT_texture_array;
extern PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC        glFramebufferTextureLayerEXT;
//
// GL_EXT_texture_integer
//
extern string                                     EXT_texture_integer;
extern PFNGLCLEARCOLORIIEXTPROC                   glClearColorIiEXT;
extern PFNGLCLEARCOLORIUIEXTPROC                  glClearColorIuiEXT;
extern PFNGLGETTEXPARAMETERIIVEXTPROC             glGetTexParameterIivEXT;
extern PFNGLGETTEXPARAMETERIUIVEXTPROC            glGetTexParameterIuivEXT;
extern PFNGLTEXPARAMETERIIVEXTPROC                glTexParameterIivEXT;
extern PFNGLTEXPARAMETERIUIVEXTPROC               glTexParameterIuivEXT;
//
// GL_EXT_transform_feedback 
//
extern string                                     EXT_transform_feedback;
extern PFNGLBEGINTRANSFORMFEEDBACKEXTPROC         glBeginTransformFeedbackEXT;
extern PFNGLBINDBUFFERBASEEXTPROC                 glBindBufferBaseEXT;
extern PFNGLBINDBUFFEROFFSETEXTPROC               glBindBufferOffsetEXT;
extern PFNGLBINDBUFFERRANGEEXTPROC                glBindBufferRangeEXT;
extern PFNGLENDTRANSFORMFEEDBACKEXTPROC           glEndTransformFeedbackEXT;
extern PFNGLGETBOOLEANINDEXEDVEXTPROC             glGetBooleanIndexedvEXT;
extern PFNGLGETINTEGERINDEXEDVEXTPROC             glGetIntegerIndexedvEXT;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC    glGetTransformFeedbackVaryingEXT;
extern PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC      glTransformFeedbackVaryingsEXT;
//
// GL_NV_transform_feedback                       ( predecessor of GL_EXT_transform_feedback )
//
extern string                                     NV_transform_feedback;
extern PFNGLACTIVEVARYINGNVPROC                   glActiveVaryingNV;
extern PFNGLBEGINTRANSFORMFEEDBACKNVPROC          glBeginTransformFeedbackNV;
extern PFNGLBINDBUFFERBASENVPROC                  glBindBufferBaseNV;
extern PFNGLBINDBUFFEROFFSETNVPROC                glBindBufferOffsetNV;
extern PFNGLBINDBUFFERRANGENVPROC                 glBindBufferRangeNV;
extern PFNGLENDTRANSFORMFEEDBACKNVPROC            glEndTransformFeedbackNV;
extern PFNGLGETACTIVEVARYINGNVPROC                glGetActiveVaryingNV;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC     glGetTransformFeedbackVaryingNV;
extern PFNGLGETVARYINGLOCATIONNVPROC              glGetVaryingLocationNV;
extern PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC        glTransformFeedbackAttribsNV;
extern PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC       glTransformFeedbackVaryingsNV;
//                                                    
// GL_NV_conditional_render                           
//    
extern string                                     NV_conditional_render;
extern PFNGLBEGINCONDITIONALRENDERNVPROC          glBeginConditionalRenderNV;
extern PFNGLENDCONDITIONALRENDERNVPROC            glEndConditionalRenderNV;
//
// GL_ARB_draw_buffers
//
extern string                                     ARB_draw_buffers;
extern PFNGLDRAWBUFFERSARBPROC                    glDrawBuffersARB;
//
// GL_ARB_shader_objects
//
extern string                                     ARB_shader_objects;
extern PFNGLATTACHOBJECTARBPROC                   glAttachObjectARB;             
extern PFNGLCOMPILESHADERARBPROC                  glCompileShaderARB;            
extern PFNGLCREATEPROGRAMOBJECTARBPROC            glCreateProgramObjectARB;      
extern PFNGLCREATESHADEROBJECTARBPROC             glCreateShaderObjectARB;       
extern PFNGLDELETEOBJECTARBPROC                   glDeleteObjectARB;             
extern PFNGLDETACHOBJECTARBPROC                   glDetachObjectARB;             
extern PFNGLGETACTIVEUNIFORMARBPROC               glGetActiveUniformARB;         
extern PFNGLGETATTACHEDOBJECTSARBPROC             glGetAttachedObjectsARB;       
extern PFNGLGETHANDLEARBPROC                      glGetHandleARB;                
extern PFNGLGETINFOLOGARBPROC                     glGetInfoLogARB;               
extern PFNGLGETOBJECTPARAMETERFVARBPROC           glGetObjectParameterfvARB;     
extern PFNGLGETOBJECTPARAMETERIVARBPROC           glGetObjectParameterivARB;     
extern PFNGLGETUNIFORMFVARBPROC                   glGetUniformfvARB;             
extern PFNGLGETUNIFORMIVARBPROC                   glGetUniformivARB;             
extern PFNGLGETUNIFORMLOCATIONARBPROC             glGetUniformLocationARB;       
extern PFNGLGETSHADERSOURCEARBPROC                glGetShaderSourceARB;          
extern PFNGLLINKPROGRAMARBPROC                    glLinkProgramARB;              
extern PFNGLSHADERSOURCEARBPROC                   glShaderSourceARB;             
extern PFNGLUNIFORM1FARBPROC                      glUniform1fARB;                
extern PFNGLUNIFORM1FVARBPROC                     glUniform1fvARB;               
extern PFNGLUNIFORM1IARBPROC                      glUniform1iARB;                
extern PFNGLUNIFORM1IVARBPROC                     glUniform1ivARB;               
extern PFNGLUNIFORM2FARBPROC                      glUniform2fARB;                
extern PFNGLUNIFORM2FVARBPROC                     glUniform2fvARB;               
extern PFNGLUNIFORM2IARBPROC                      glUniform2iARB;                
extern PFNGLUNIFORM2IVARBPROC                     glUniform2ivARB;               
extern PFNGLUNIFORM3FARBPROC                      glUniform3fARB;                
extern PFNGLUNIFORM3FVARBPROC                     glUniform3fvARB;               
extern PFNGLUNIFORM3IARBPROC                      glUniform3iARB;                
extern PFNGLUNIFORM3IVARBPROC                     glUniform3ivARB;               
extern PFNGLUNIFORM4FARBPROC                      glUniform4fARB;                
extern PFNGLUNIFORM4FVARBPROC                     glUniform4fvARB;               
extern PFNGLUNIFORM4IARBPROC                      glUniform4iARB;                
extern PFNGLUNIFORM4IVARBPROC                     glUniform4ivARB;               
extern PFNGLUNIFORMMATRIX2FVARBPROC               glUniformMatrix2fvARB;         
extern PFNGLUNIFORMMATRIX3FVARBPROC               glUniformMatrix3fvARB;         
extern PFNGLUNIFORMMATRIX4FVARBPROC               glUniformMatrix4fvARB;         
extern PFNGLUSEPROGRAMOBJECTARBPROC               glUseProgramObjectARB;         
extern PFNGLVALIDATEPROGRAMARBPROC                glValidateProgramARB;          
//
// GL_ARB_vertex_shader
//
extern string                                     ARB_vertex_shader;
extern PFNGLBINDATTRIBLOCATIONARBPROC             glBindAttribLocationARB;       
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC       glDisableVertexAttribArrayARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC        glEnableVertexAttribArrayARB;
extern PFNGLGETACTIVEATTRIBARBPROC                glGetActiveAttribARB;          
extern PFNGLGETATTRIBLOCATIONARBPROC              glGetAttribLocationARB;        
extern PFNGLGETVERTEXATTRIBDVARBPROC              glGetVertexAttribdvARB;        
extern PFNGLGETVERTEXATTRIBFVARBPROC              glGetVertexAttribfvARB;        
extern PFNGLGETVERTEXATTRIBIVARBPROC              glGetVertexAttribivARB;        
extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC        glGetVertexAttribPointervARB;
extern PFNGLVERTEXATTRIB1DARBPROC                 glVertexAttrib1dARB;           
extern PFNGLVERTEXATTRIB1DVARBPROC                glVertexAttrib1dvARB;          
extern PFNGLVERTEXATTRIB1FARBPROC                 glVertexAttrib1fARB;           
extern PFNGLVERTEXATTRIB1FVARBPROC                glVertexAttrib1fvARB;          
extern PFNGLVERTEXATTRIB1SARBPROC                 glVertexAttrib1sARB;           
extern PFNGLVERTEXATTRIB1SVARBPROC                glVertexAttrib1svARB;          
extern PFNGLVERTEXATTRIB2DARBPROC                 glVertexAttrib2dARB;           
extern PFNGLVERTEXATTRIB2DVARBPROC                glVertexAttrib2dvARB;          
extern PFNGLVERTEXATTRIB2FARBPROC                 glVertexAttrib2fARB;           
extern PFNGLVERTEXATTRIB2FVARBPROC                glVertexAttrib2fvARB;          
extern PFNGLVERTEXATTRIB2SARBPROC                 glVertexAttrib2sARB;           
extern PFNGLVERTEXATTRIB2SVARBPROC                glVertexAttrib2svARB;          
extern PFNGLVERTEXATTRIB3DARBPROC                 glVertexAttrib3dARB;           
extern PFNGLVERTEXATTRIB3DVARBPROC                glVertexAttrib3dvARB;          
extern PFNGLVERTEXATTRIB3FARBPROC                 glVertexAttrib3fARB;           
extern PFNGLVERTEXATTRIB3FVARBPROC                glVertexAttrib3fvARB;          
extern PFNGLVERTEXATTRIB3SARBPROC                 glVertexAttrib3sARB;           
extern PFNGLVERTEXATTRIB3SVARBPROC                glVertexAttrib3svARB;          
extern PFNGLVERTEXATTRIB4NBVARBPROC               glVertexAttrib4NbvARB;         
extern PFNGLVERTEXATTRIB4NIVARBPROC               glVertexAttrib4NivARB;         
extern PFNGLVERTEXATTRIB4NSVARBPROC               glVertexAttrib4NsvARB;         
extern PFNGLVERTEXATTRIB4NUBARBPROC               glVertexAttrib4NubARB;         
extern PFNGLVERTEXATTRIB4NUBVARBPROC              glVertexAttrib4NubvARB;        
extern PFNGLVERTEXATTRIB4NUIVARBPROC              glVertexAttrib4NuivARB;        
extern PFNGLVERTEXATTRIB4NUSVARBPROC              glVertexAttrib4NusvARB;        
extern PFNGLVERTEXATTRIB4BVARBPROC                glVertexAttrib4bvARB;          
extern PFNGLVERTEXATTRIB4DARBPROC                 glVertexAttrib4dARB;           
extern PFNGLVERTEXATTRIB4DVARBPROC                glVertexAttrib4dvARB;          
extern PFNGLVERTEXATTRIB4FARBPROC                 glVertexAttrib4fARB;           
extern PFNGLVERTEXATTRIB4FVARBPROC                glVertexAttrib4fvARB;          
extern PFNGLVERTEXATTRIB4IVARBPROC                glVertexAttrib4ivARB;          
extern PFNGLVERTEXATTRIB4SARBPROC                 glVertexAttrib4sARB;           
extern PFNGLVERTEXATTRIB4SVARBPROC                glVertexAttrib4svARB;          
extern PFNGLVERTEXATTRIB4UBVARBPROC               glVertexAttrib4ubvARB;         
extern PFNGLVERTEXATTRIB4UIVARBPROC               glVertexAttrib4uivARB;         
extern PFNGLVERTEXATTRIB4USVARBPROC               glVertexAttrib4usvARB;         
extern PFNGLVERTEXATTRIBPOINTERARBPROC            glVertexAttribPointerARB;      
//
// GL_EXT_blend_equation_separate
//
extern string                                     EXT_blend_equation_separate;
extern PFNGLBLENDEQUATIONSEPARATEEXTPROC          glBlendEquationSeparateEXT;
//
// GL_EXT_stencil_two_side
//
extern string                                     EXT_stencil_two_side;
extern PFNGLACTIVESTENCILFACEEXTPROC              glActiveStencilFaceEXT;
//                                                                     
// GL_ARB_occlusion_query                                              
//   
extern string                                     ARB_occlusion_query;
extern PFNGLBEGINQUERYARBPROC                     glBeginQueryARB;       
extern PFNGLDELETEQUERIESARBPROC                  glDeleteQueriesARB;    
extern PFNGLENDQUERYARBPROC                       glEndQueryARB;         
extern PFNGLGENQUERIESARBPROC                     glGenQueriesARB;       
extern PFNGLGETQUERYIVARBPROC                     glGetQueryivARB;       
extern PFNGLGETQUERYOBJECTIVARBPROC               glGetQueryObjectivARB; 
extern PFNGLGETQUERYOBJECTUIVARBPROC              glGetQueryObjectuivARB;
extern PFNGLISQUERYARBPROC                        glIsQueryARB;          
//                                                                     
// GL_ARB_vertex_buffer_object                                         
//   
extern string                                     ARB_vertex_buffer_object;
extern PFNGLBINDBUFFERARBPROC                     glBindBufferARB;          
extern PFNGLBUFFERDATAARBPROC                     glBufferDataARB;          
extern PFNGLBUFFERSUBDATAARBPROC                  glBufferSubDataARB;       
extern PFNGLDELETEBUFFERSARBPROC                  glDeleteBuffersARB;       
extern PFNGLGENBUFFERSARBPROC                     glGenBuffersARB;          
extern PFNGLGETBUFFERPARAMETERIVARBPROC           glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC              glGetBufferPointervARB;   
extern PFNGLGETBUFFERSUBDATAARBPROC               glGetBufferSubDataARB;    
extern PFNGLISBUFFERARBPROC                       glIsBufferARB;            
extern PFNGLMAPBUFFERARBPROC                      glMapBufferARB;           
extern PFNGLUNMAPBUFFERARBPROC                    glUnmapBufferARB;         
//
// GL_ARB_point_parameters
//
extern string                                     ARB_point_parameters;
extern PFNGLPOINTPARAMETERFARBPROC                glPointParameterfARB; 
extern PFNGLPOINTPARAMETERFVARBPROC               glPointParameterfvARB;
//
// GL_ARB_window_pos                              ( Deprecated in OpenGL 3.0 Core )
//
extern string                                     ARB_window_pos;
extern PFNGLWINDOWPOS2DARBPROC                    glWindowPos2dARB; 
extern PFNGLWINDOWPOS2DVARBPROC                   glWindowPos2dvARB;
extern PFNGLWINDOWPOS2FARBPROC                    glWindowPos2fARB; 
extern PFNGLWINDOWPOS2FVARBPROC                   glWindowPos2fvARB;
extern PFNGLWINDOWPOS2IARBPROC                    glWindowPos2iARB; 
extern PFNGLWINDOWPOS2IVARBPROC                   glWindowPos2ivARB;
extern PFNGLWINDOWPOS2SARBPROC                    glWindowPos2sARB; 
extern PFNGLWINDOWPOS2SVARBPROC                   glWindowPos2svARB;
extern PFNGLWINDOWPOS3DARBPROC                    glWindowPos3dARB; 
extern PFNGLWINDOWPOS3DVARBPROC                   glWindowPos3dvARB;
extern PFNGLWINDOWPOS3FARBPROC                    glWindowPos3fARB; 
extern PFNGLWINDOWPOS3FVARBPROC                   glWindowPos3fvARB;
extern PFNGLWINDOWPOS3IARBPROC                    glWindowPos3iARB; 
extern PFNGLWINDOWPOS3IVARBPROC                   glWindowPos3ivARB;
extern PFNGLWINDOWPOS3SARBPROC                    glWindowPos3sARB; 
extern PFNGLWINDOWPOS3SVARBPROC                   glWindowPos3svARB;
//
// GL_EXT_blend_color
//
extern string                                     EXT_blend_color;
extern PFNGLBLENDCOLOREXTPROC                     glBlendColorEXT;
//
// GL_EXT_blend_func_separate
//
extern string                                     EXT_blend_func_separate;
extern PFNGLBLENDFUNCSEPARATEEXTPROC              glBlendFuncSeparateEXT;
//
// GL_EXT_fog_coord                               ( Deprecated in OpenGL 3.0 Core )
//
extern string                                     EXT_fog_coord;
extern PFNGLFOGCOORDFEXTPROC                      glFogCoordfEXT;      
extern PFNGLFOGCOORDFVEXTPROC                     glFogCoordfvEXT;     
extern PFNGLFOGCOORDDEXTPROC                      glFogCoorddEXT;      
extern PFNGLFOGCOORDDVEXTPROC                     glFogCoorddvEXT;     
extern PFNGLFOGCOORDPOINTEREXTPROC                glFogCoordPointerEXT;
//
// GL_EXT_multi_draw_arrays
//
extern string                                     EXT_multi_draw_arrays;
extern PFNGLMULTIDRAWARRAYSEXTPROC                glMultiDrawArraysEXT;  
extern PFNGLMULTIDRAWELEMENTSEXTPROC              glMultiDrawElementsEXT;
//
// GL_EXT_secondary_color                         ( Deprecated in OpenGL 3.0 Core )
//
extern string                                     EXT_secondary_color;
extern PFNGLSECONDARYCOLOR3BEXTPROC               glSecondaryColor3bEXT;     
extern PFNGLSECONDARYCOLOR3BVEXTPROC              glSecondaryColor3bvEXT;    
extern PFNGLSECONDARYCOLOR3DEXTPROC               glSecondaryColor3dEXT;     
extern PFNGLSECONDARYCOLOR3DVEXTPROC              glSecondaryColor3dvEXT;    
extern PFNGLSECONDARYCOLOR3FEXTPROC               glSecondaryColor3fEXT;     
extern PFNGLSECONDARYCOLOR3FVEXTPROC              glSecondaryColor3fvEXT;    
extern PFNGLSECONDARYCOLOR3IEXTPROC               glSecondaryColor3iEXT;     
extern PFNGLSECONDARYCOLOR3IVEXTPROC              glSecondaryColor3ivEXT;    
extern PFNGLSECONDARYCOLOR3SEXTPROC               glSecondaryColor3sEXT;     
extern PFNGLSECONDARYCOLOR3SVEXTPROC              glSecondaryColor3svEXT;    
extern PFNGLSECONDARYCOLOR3UBEXTPROC              glSecondaryColor3ubEXT;    
extern PFNGLSECONDARYCOLOR3UBVEXTPROC             glSecondaryColor3ubvEXT;   
extern PFNGLSECONDARYCOLOR3UIEXTPROC              glSecondaryColor3uiEXT;    
extern PFNGLSECONDARYCOLOR3UIVEXTPROC             glSecondaryColor3uivEXT;   
extern PFNGLSECONDARYCOLOR3USEXTPROC              glSecondaryColor3usEXT;    
extern PFNGLSECONDARYCOLOR3USVEXTPROC             glSecondaryColor3usvEXT;   
extern PFNGLSECONDARYCOLORPOINTEREXTPROC          glSecondaryColorPointerEXT;
//
// GL_ARB_multisample
//
extern string                                     ARB_multisample;
extern PFNGLSAMPLECOVERAGEARBPROC                 glSampleCoverageARB;
//
// GL_ARB_multitexture                            ( Deprecated in OpenGL 3.0 Core )
// 
extern string                                     ARB_multitexture;
extern PFNGLACTIVETEXTUREARBPROC                  glActiveTextureARB;   
extern PFNGLCLIENTACTIVETEXTUREARBPROC            glClientActiveTextureARB;
extern PFNGLMULTITEXCOORD1DARBPROC                glMultiTexCoord1dARB;    
extern PFNGLMULTITEXCOORD1DVARBPROC               glMultiTexCoord1dvARB;  
extern PFNGLMULTITEXCOORD1FARBPROC                glMultiTexCoord1fARB;    
extern PFNGLMULTITEXCOORD1FVARBPROC               glMultiTexCoord1fvARB;   
extern PFNGLMULTITEXCOORD1IARBPROC                glMultiTexCoord1iARB;    
extern PFNGLMULTITEXCOORD1IVARBPROC               glMultiTexCoord1ivARB;   
extern PFNGLMULTITEXCOORD1SARBPROC                glMultiTexCoord1sARB;    
extern PFNGLMULTITEXCOORD1SVARBPROC               glMultiTexCoord1svARB;   
extern PFNGLMULTITEXCOORD2DARBPROC                glMultiTexCoord2dARB;    
extern PFNGLMULTITEXCOORD2DVARBPROC               glMultiTexCoord2dvARB;   
extern PFNGLMULTITEXCOORD2FARBPROC                glMultiTexCoord2fARB;    
extern PFNGLMULTITEXCOORD2FVARBPROC               glMultiTexCoord2fvARB;  
extern PFNGLMULTITEXCOORD2IARBPROC                glMultiTexCoord2iARB;    
extern PFNGLMULTITEXCOORD2IVARBPROC               glMultiTexCoord2ivARB;   
extern PFNGLMULTITEXCOORD2SARBPROC                glMultiTexCoord2sARB;    
extern PFNGLMULTITEXCOORD2SVARBPROC               glMultiTexCoord2svARB;   
extern PFNGLMULTITEXCOORD3DARBPROC                glMultiTexCoord3dARB;    
extern PFNGLMULTITEXCOORD3DVARBPROC               glMultiTexCoord3dvARB;   
extern PFNGLMULTITEXCOORD3FARBPROC                glMultiTexCoord3fARB;    
extern PFNGLMULTITEXCOORD3FVARBPROC               glMultiTexCoord3fvARB;   
extern PFNGLMULTITEXCOORD3IARBPROC                glMultiTexCoord3iARB;    
extern PFNGLMULTITEXCOORD3IVARBPROC               glMultiTexCoord3ivARB;   
extern PFNGLMULTITEXCOORD3SARBPROC                glMultiTexCoord3sARB;    
extern PFNGLMULTITEXCOORD3SVARBPROC               glMultiTexCoord3svARB;  
extern PFNGLMULTITEXCOORD4DARBPROC                glMultiTexCoord4dARB;    
extern PFNGLMULTITEXCOORD4DVARBPROC               glMultiTexCoord4dvARB;   
extern PFNGLMULTITEXCOORD4FARBPROC                glMultiTexCoord4fARB;    
extern PFNGLMULTITEXCOORD4FVARBPROC               glMultiTexCoord4fvARB;   
extern PFNGLMULTITEXCOORD4IARBPROC                glMultiTexCoord4iARB;    
extern PFNGLMULTITEXCOORD4IVARBPROC               glMultiTexCoord4ivARB;   
extern PFNGLMULTITEXCOORD4SARBPROC                glMultiTexCoord4sARB;    
extern PFNGLMULTITEXCOORD4SVARBPROC               glMultiTexCoord4svARB;   
//
// GL_ARB_texture_compression
//
extern string                                     ARB_texture_compression;
extern PFNGLCOMPRESSEDTEXIMAGE1DARBPROC           glCompressedTexImage1DARB;   
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC           glCompressedTexImage2DARB;   
extern PFNGLCOMPRESSEDTEXIMAGE3DARBPROC           glCompressedTexImage3DARB;   
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC        glCompressedTexSubImage1DARB; 
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC        glCompressedTexSubImage2DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC        glCompressedTexSubImage3DARB;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC          glGetCompressedTexImageARB;  
//
// GL_ARB_transpose_matrix                        ( Deprecated in OpenGL 3.0 Core )
//
extern string                                     ARB_transpose_matrix;
extern PFNGLLOADTRANSPOSEMATRIXFARBPROC           glLoadTransposeMatrixfARB;
extern PFNGLLOADTRANSPOSEMATRIXDARBPROC           glLoadTransposeMatrixdARB;
extern PFNGLMULTTRANSPOSEMATRIXFARBPROC           glMultTransposeMatrixfARB;
extern PFNGLMULTTRANSPOSEMATRIXDARBPROC           glMultTransposeMatrixdARB;
//
// GL_EXT_draw_range_elements
//
extern string                                     EXT_draw_range_elements;
extern PFNGLDRAWRANGEELEMENTSEXTPROC              glDrawRangeElementsEXT;
//
// GL_EXT_texture3D
//
extern string                                     EXT_texture3D;
extern PFNGLTEXIMAGE3DEXTPROC                     glTexImage3DEXT;

/******************************************************************************

 OpenGL Extensions

******************************************************************************/

//
// GL_EXT_direct_state_access
//
extern string                                     EXT_direct_state_access;
//
// GL_EXT_texture_filter_anisotropic
//
extern string                                     EXT_texture_filter_anisotropic;

/******************************************************************************

 Windows Extensions for OpenGL 

******************************************************************************/

//
// WGL_ARB_create_context
//
extern string                                     ARB_create_context;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC          wglCreateContextAttribsARB;
//
// WGL_ARB_pixel_format
//
extern string                                     ARB_pixel_format;
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC        wglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC        wglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC             wglChoosePixelFormatARB;
//
// WGL_EXT_pixel_format
//
extern string                                     EXT_pixel_format;
extern PFNWGLGETPIXELFORMATATTRIBIVEXTPROC        wglGetPixelFormatAttribivEXT;
extern PFNWGLGETPIXELFORMATATTRIBFVEXTPROC        wglGetPixelFormatAttribfvEXT;
extern PFNWGLCHOOSEPIXELFORMATEXTPROC             wglChoosePixelFormatEXT;
//
// WGL_ARB_extensions_string 
//
extern string                                     ARB_extensions_string;
extern PFNWGLGETEXTENSIONSSTRINGARBPROC           wglGetExtensionsStringARB;
//
// WGL_EXT_extensions_string 
//
extern string                                     EXT_extensions_string;
extern PFNWGLGETEXTENSIONSSTRINGEXTPROC           wglGetExtensionsStringEXT;
//
// WGL_EXT_swap_control
//
extern string                                     EXT_swap_control;
extern PFNWGLSWAPINTERVALEXTPROC                  wglSwapIntervalEXT;
extern PFNWGLGETSWAPINTERVALEXTPROC               wglGetSwapIntervalEXT;
#endif  

#endif