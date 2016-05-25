/*

 Ngine v5.0
 
 Module      : OpenGL Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/opengl/glInputAssembler.h"
#include "core/rendering/opengl/glDevice.h"

namespace en
{
   namespace gpu
   {      
#ifdef EN_OPENGL_DESKTOP
   
   // OpenGL Vertex Attribute Formats:     
   // https://www.opengl.org/sdk/docs/man/html/glVertexAttribPointer.xhtml
   // (last verified for OpenGL 4.5)
   //
   // GL_BGRA can be replaced by 4 is formatting can be deduced from type.
   const AttributeTranslation TranslateAttribute[underlyingType(Attribute::Count)] =
      { // Size Col Norm Int    64bit  Format
      { 0,  0, 0,        0,     0,     0                               }, // None
      { 1,  1, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // u8_norm
      { 1,  1, GL_TRUE,  false, false, GL_BYTE                         }, // s8_norm
      { 1,  1, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // u8
      { 1,  1, GL_FALSE, true,  false, GL_BYTE                         }, // s8
      { 2,  1, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // u16_norm
      { 2,  1, GL_TRUE,  false, false, GL_SHORT                        }, // s16_norm
      { 2,  1, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // u16
      { 2,  1, GL_FALSE, true,  false, GL_SHORT                        }, // s16
      { 2,  1, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // f16
      { 4,  1, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // u32
      { 4,  1, GL_FALSE, true,  false, GL_INT                          }, // s32
      { 4,  1, GL_FALSE, false, false, GL_FLOAT                        }, // f32
      { 8,  1, GL_FALSE, true,  true,  0                               }, // u64
      { 8,  1, GL_FALSE, true,  true,  0                               }, // s64
      { 8,  1, GL_FALSE, false, true,  GL_DOUBLE                       }, // f64
      { 2,  2, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // v2u8_norm
      { 2,  2, GL_TRUE,  false, false, GL_BYTE                         }, // v2s8_norm
      { 2,  2, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // v2u8
      { 2,  2, GL_FALSE, true,  false, GL_BYTE                         }, // v2s8
      { 4,  2, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // v2u16_norm
      { 4,  2, GL_TRUE,  false, false, GL_SHORT                        }, // v2s16_norm
      { 4,  2, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // v2u16
      { 4,  2, GL_FALSE, true,  false, GL_SHORT                        }, // v2s16
      { 4,  2, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // v2f16
      { 8,  2, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // v2u32
      { 8,  2, GL_FALSE, true,  false, GL_INT                          }, // v2s32
      { 8,  2, GL_FALSE, false, false, GL_FLOAT                        }, // v2f32
      { 16, 2, GL_FALSE, true,  true,  0                               }, // v2u64
      { 16, 2, GL_FALSE, true,  true,  0                               }, // v2s64
      { 16, 2, GL_FALSE, false, true,  GL_DOUBLE                       }, // v2f64
      { 3,  3, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // v3u8_norm
      { 3,  3, GL_TRUE,  false, false, 0                               }, // v3u8_srgb
      { 3,  3, GL_TRUE,  false, false, GL_BYTE                         }, // v3s8_norm
      { 3,  3, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // v3u8
      { 3,  3, GL_FALSE, true,  false, GL_BYTE                         }, // v3s8
      { 6,  3, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // v3u16_norm
      { 6,  3, GL_TRUE,  false, false, GL_SHORT                        }, // v3s16_norm
      { 6,  3, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // v3u16
      { 6,  3, GL_FALSE, true,  false, GL_SHORT                        }, // v3s16
      { 6,  3, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // v3f16
      { 12, 3, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // v3u32
      { 12, 3, GL_FALSE, true,  false, GL_INT                          }, // v3s32
      { 12, 3, GL_FALSE, false, false, GL_FLOAT                        }, // v3f32
      { 24, 3, GL_FALSE, true,  true,  0                               }, // v3u64
      { 24, 3, GL_FALSE, true,  true,  0                               }, // v3s64
      { 24, 3, GL_FALSE, false, true,  GL_DOUBLE                       }, // v3f64
      { 4,  4, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // v4u8_norm
      { 4,  4, GL_TRUE,  false, false, GL_BYTE                         }, // v4s8_norm
      { 4,  4, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // v4u8
      { 4,  4, GL_FALSE, true,  false, GL_BYTE                         }, // v4s8
      { 8,  4, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // v4u16_norm
      { 8,  4, GL_TRUE,  false, false, GL_SHORT                        }, // v4s16_norm
      { 8,  4, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // v4u16
      { 8,  4, GL_FALSE, true,  false, GL_SHORT                        }, // v4s16
      { 8,  4, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // v4f16
      { 16, 4, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // v4u32
      { 16, 4, GL_FALSE, true,  false, GL_INT                          }, // v4s32
      { 16, 4, GL_FALSE, false, false, GL_FLOAT                        }, // v4f32
      { 32, 4, GL_FALSE, true,  true,  0                               }, // v4u64
      { 32, 4, GL_FALSE, true,  true,  0                               }, // v4s64
      { 32, 4, GL_FALSE, false, true,  GL_DOUBLE                       }, // v4f64
      { 4,  3, GL_TRUE,  false, false, GL_UNSIGNED_INT_10F_11F_11F_REV }, // v3f11_11_10
      { 4,  4, GL_TRUE,  false, false, 0                               }, // v4u10_10_10_2_norm
      { 4,  4, GL_TRUE,  false, false, 0                               }, // v4s10_10_10_2_norm
      { 4,  4, GL_FALSE, true,  false, 0                               }, // v4u10_10_10_2
      { 4,  4, GL_FALSE, true,  false, 0                               }, // v4s10_10_10_2
      { 4,  GL_BGRA, GL_TRUE,  false, false, GL_UNSIGNED_INT_2_10_10_10_REV  }, // v4u10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW (see ARB_vertex_array_bgra)
      { 4,  GL_BGRA, GL_TRUE,  false, false, GL_INT_2_10_10_10_REV           }, // v4s10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW 
      { 4,  4, GL_FALSE, true,  false, 0                               }, // v4u10_10_10_2_rev
      { 4,  4, GL_FALSE, true,  false, 0                               }  // v4s10_10_10_2_rev
      };   

// Vertex Fetch fixed point formats deliberately not supported:
//  
// cf - integer cast to float (also known as USCALED/SSCALED)
//
// It's better to use unsigned/signed integer formats and cast to float manually if needed.
//
//    { OpenGL_2_0,         1,  1, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // u8_cf
//    { OpenGL_2_0,         1,  1, GL_FALSE, false, false, GL_BYTE                         }, // s8_cf
//    { OpenGL_2_0,         2,  1, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // u16_cf
//    { OpenGL_2_0,         2,  1, GL_FALSE, false, false, GL_SHORT                        }, // s16_cf
//    { OpenGL_2_0,         2,  2, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // v2u8_cf            
//    { OpenGL_2_0,         2,  2, GL_FALSE, false, false, GL_BYTE                         }, // v2s8_cf  
//    { OpenGL_2_0,         4,  2, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // v2u16_cf
//    { OpenGL_2_0,         4,  2, GL_FALSE, false, false, GL_SHORT                        }, // v2s16_cf 
//    { OpenGL_2_0,         3,  3, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // v3u8_cf 
//    { OpenGL_2_0,         3,  3, GL_FALSE, false, false, GL_BYTE                         }, // v3s8_cf  
//    { OpenGL_2_0,         6,  3, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // v3u16_cf
//    { OpenGL_2_0,         6,  3, GL_FALSE, false, false, GL_SHORT                        }, // v3s16_cf 
//    { OpenGL_2_0,         4,  4, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // v4u8_cf 
//    { OpenGL_2_0,         4,  4, GL_FALSE, false, false, GL_BYTE                         }, // v4s8_cf
//    { OpenGL_2_0,         8,  4, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // v4u16_cf 
//    { OpenGL_2_0,         8,  4, GL_FALSE, false, false, GL_SHORT                        }, // v4s16_cf         

   // First OpenGL version that supports it
   const Nversion AttributeVersion[underlyingType(Attribute::Count)] =
      {
      OpenGL_Unsupported,  // None
      OpenGL_2_0,          // u8_norm
      OpenGL_2_0,          // s8_norm
      OpenGL_3_0,          // u8
      OpenGL_3_0,          // s8
      OpenGL_2_0,          // u16_norm
      OpenGL_2_0,          // s16_norm
      OpenGL_3_0,          // u16
      OpenGL_3_0,          // s16
      OpenGL_3_0,          // f16
      OpenGL_3_0,          // u32
      OpenGL_3_0,          // s32
      OpenGL_2_0,          // f32
      OpenGL_Unsupported,  // u64
      OpenGL_Unsupported,  // s64
      OpenGL_4_1,          // f64
      OpenGL_2_0,          // v2u8_norm
      OpenGL_2_0,          // v2s8_norm
      OpenGL_3_0,          // v2u8
      OpenGL_3_0,          // v2s8
      OpenGL_2_0,          // v2u16_norm
      OpenGL_2_0,          // v2s16_norm
      OpenGL_3_0,          // v2u16
      OpenGL_3_0,          // v2s16
      OpenGL_3_0,          // v2f16
      OpenGL_3_0,          // v2u32
      OpenGL_3_0,          // v2s32
      OpenGL_2_0,          // v2f32
      OpenGL_Unsupported,  // v2u64
      OpenGL_Unsupported,  // v2s64
      OpenGL_4_1,          // v2f64
      OpenGL_2_0,          // v3u8_norm
      OpenGL_Unsupported,  // v3u8_srgb
      OpenGL_2_0,          // v3s8_norm
      OpenGL_3_0,          // v3u8
      OpenGL_3_0,          // v3s8
      OpenGL_2_0,          // v3u16_norm
      OpenGL_2_0,          // v3s16_norm
      OpenGL_3_0,          // v3u16
      OpenGL_3_0,          // v3s16
      OpenGL_3_0,          // v3f16
      OpenGL_3_0,          // v3u32
      OpenGL_3_0,          // v3s32
      OpenGL_2_0,          // v3f32
      OpenGL_Unsupported,  // v3u64
      OpenGL_Unsupported,  // v3s64
      OpenGL_4_1,          // v3f64
      OpenGL_2_0,          // v4u8_norm
      OpenGL_2_0,          // v4s8_norm
      OpenGL_3_0,          // v4u8
      OpenGL_3_0,          // v4s8
      OpenGL_2_0,          // v4u16_norm
      OpenGL_2_0,          // v4s16_norm
      OpenGL_3_0,          // v4u16
      OpenGL_3_0,          // v4s16
      OpenGL_3_0,          // v4f16
      OpenGL_3_0,          // v4u32
      OpenGL_3_0,          // v4s32
      OpenGL_2_0,          // v4f32
      OpenGL_Unsupported,  // v4u64
      OpenGL_Unsupported,  // v4s64
      OpenGL_4_1,          // v4f64
      OpenGL_4_4,          // v3f11_11_10
      OpenGL_Unsupported,  // v4u10_10_10_2_norm
      OpenGL_Unsupported,  // v4s10_10_10_2_norm
      OpenGL_Unsupported,  // v4u10_10_10_2
      OpenGL_Unsupported,  // v4s10_10_10_2
      OpenGL_3_3,          // v4u10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW (see ARB_vertex_array_bgra)
      OpenGL_3_3,          // v4s10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW 
      OpenGL_Unsupported,  // v4u10_10_10_2_rev
      OpenGL_Unsupported   // v4s10_10_10_2_rev
      };

#endif
#ifdef EN_OPENGL_MOBILE

   // TODO: Rework this:
   
   const BufferTypeTranslation BufferType[BufferTypesCount] = 
      { 
      { OpenGL_ES_1_1, GL_ARRAY_BUFFER                 },   // Vertex                  
      { OpenGL_ES_1_1, GL_ELEMENT_ARRAY_BUFFER         }    // Index  
      };

   // OpenGL ES 3.0 Vertex Attribute Formats:
   // http://www.khronos.org/registry/gles/specs/1.0/opengles_spec_1_0.pdf
   // https://www.khronos.org/registry/gles/specs/1.1/es_full_spec_1.1.12.pdf
   // https://www.khronos.org/registry/gles/specs/2.0/es_full_spec_2.0.25.pdf
   // https://www.khronos.org/registry/gles/specs/3.0/es_spec_3.0.0.pdf
   // (last verified for OpenGL ES 3.0)
   //
   const AttributeTranslation TranslateAttribute[underlyingType(Attribute::Count)] =
      { // Size Col Norm Int    64bit  Format
      { 0,  0, 0,        0,     0,     0                               }, // None
      { 1,  1, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // u8_norm
      { 1,  1, GL_TRUE,  false, false, GL_BYTE                         }, // s8_norm
      { 1,  1, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // u8
      { 1,  1, GL_FALSE, true,  false, GL_BYTE                         }, // s8
      { 2,  1, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // u16_norm
      { 2,  1, GL_TRUE,  false, false, GL_SHORT                        }, // s16_norm
      { 2,  1, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // u16
      { 2,  1, GL_FALSE, true,  false, GL_SHORT                        }, // s16
      { 2,  1, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // f16
      { 4,  1, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // u32
      { 4,  1, GL_FALSE, true,  false, GL_INT                          }, // s32
      { 4,  1, GL_FALSE, false, false, GL_FLOAT                        }, // f32
      { 8,  1, GL_FALSE, true,  true,  0                               }, // u64
      { 8,  1, GL_FALSE, true,  true,  0                               }, // s64
      { 8,  1, GL_FALSE, false, true,  /*GL_DOUBLE*/                   }, // f64
      { 2,  2, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // v2u8_norm
      { 2,  2, GL_TRUE,  false, false, GL_BYTE                         }, // v2s8_norm
      { 2,  2, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // v2u8
      { 2,  2, GL_FALSE, true,  false, GL_BYTE                         }, // v2s8
      { 4,  2, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // v2u16_norm
      { 4,  2, GL_TRUE,  false, false, GL_SHORT                        }, // v2s16_norm
      { 4,  2, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // v2u16
      { 4,  2, GL_FALSE, true,  false, GL_SHORT                        }, // v2s16
      { 4,  2, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // v2f16
      { 8,  2, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // v2u32
      { 8,  2, GL_FALSE, true,  false, GL_INT                          }, // v2s32
      { 8,  2, GL_FALSE, false, false, GL_FLOAT                        }, // v2f32
      { 16, 2, GL_FALSE, true,  true,  0                               }, // v2u64
      { 16, 2, GL_FALSE, true,  true,  0                               }, // v2s64
      { 16, 2, GL_FALSE, false, true,  /*GL_DOUBLE*/                   }, // v2f64
      { 3,  3, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // v3u8_norm
      { 3,  3, GL_TRUE,  false, false, 0                               }, // v3u8_srgb
      { 3,  3, GL_TRUE,  false, false, GL_BYTE                         }, // v3s8_norm
      { 3,  3, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // v3u8
      { 3,  3, GL_FALSE, true,  false, GL_BYTE                         }, // v3s8
      { 6,  3, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // v3u16_norm
      { 6,  3, GL_TRUE,  false, false, GL_SHORT                        }, // v3s16_norm
      { 6,  3, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // v3u16
      { 6,  3, GL_FALSE, true,  false, GL_SHORT                        }, // v3s16
      { 6,  3, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // v3f16
      { 12, 3, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // v3u32
      { 12, 3, GL_FALSE, true,  false, GL_INT                          }, // v3s32
      { 12, 3, GL_FALSE, false, false, GL_FLOAT                        }, // v3f32
      { 24, 3, GL_FALSE, true,  true,  0                               }, // v3u64
      { 24, 3, GL_FALSE, true,  true,  0                               }, // v3s64
      { 24, 3, GL_FALSE, false, true,  /*GL_DOUBLE*/                   }, // v3f64
      { 4,  4, GL_TRUE,  false, false, GL_UNSIGNED_BYTE                }, // v4u8_norm
      { 4,  4, GL_TRUE,  false, false, GL_BYTE                         }, // v4s8_norm
      { 4,  4, GL_FALSE, true,  false, GL_UNSIGNED_BYTE                }, // v4u8
      { 4,  4, GL_FALSE, true,  false, GL_BYTE                         }, // v4s8
      { 8,  4, GL_TRUE,  false, false, GL_UNSIGNED_SHORT               }, // v4u16_norm
      { 8,  4, GL_TRUE,  false, false, GL_SHORT                        }, // v4s16_norm
      { 8,  4, GL_FALSE, true,  false, GL_UNSIGNED_SHORT               }, // v4u16
      { 8,  4, GL_FALSE, true,  false, GL_SHORT                        }, // v4s16
      { 8,  4, GL_FALSE, false, false, GL_HALF_FLOAT                   }, // v4f16
      { 16, 4, GL_FALSE, true,  false, GL_UNSIGNED_INT                 }, // v4u32
      { 16, 4, GL_FALSE, true,  false, GL_INT                          }, // v4s32
      { 16, 4, GL_FALSE, false, false, GL_FLOAT                        }, // v4f32
      { 32, 4, GL_FALSE, true,  true,  0                               }, // v4u64
      { 32, 4, GL_FALSE, true,  true,  0                               }, // v4s64
      { 32, 4, GL_FALSE, false, true,  /*GL_DOUBLE*/                   }, // v4f64
      { 4,  3, GL_TRUE,  false, false, /*GL_UNSIGNED_INT_10F_11F_11F_REV*/ }, // v3f11_11_10
      { 4,  4, GL_TRUE,  false, false, 0                               }, // v4u10_10_10_2_norm
      { 4,  4, GL_TRUE,  false, false, 0                               }, // v4s10_10_10_2_norm
      { 4,  4, GL_FALSE, true,  false, 0                               }, // v4u10_10_10_2
      { 4,  4, GL_FALSE, true,  false, 0                               }, // v4s10_10_10_2
      { 4,  4, GL_TRUE,  false, false, GL_UNSIGNED_INT_2_10_10_10_REV  }, // v4u10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW (see ARB_vertex_array_bgra)
      { 4,  4, GL_TRUE,  false, false, GL_INT_2_10_10_10_REV           }, // v4s10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW 
      { 4,  4, GL_FALSE, true,  false, 0                               }, // v4u10_10_10_2_rev
      { 4,  4, GL_FALSE, true,  false, 0                               }  // v4s10_10_10_2_rev
      };   

// Vertex Fetch fixed point formats deliberately not supported:
//  
// cf - integer cast to float (also known as USCALED/SSCALED)
//
// Unsigned formats were added in ES 2.0, as well as single channel ones.
//
//    { OpenGL_ES_2_0,         1,  1, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // u8_cf
//    { OpenGL_ES_2_0,         1,  1, GL_FALSE, false, false, GL_BYTE                         }, // s8_cf
//    { OpenGL_ES_2_0,         2,  1, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // u16_cf
//    { OpenGL_ES_2_0,         2,  1, GL_FALSE, false, false, GL_SHORT                        }, // s16_cf
//    { OpenGL_ES_2_0,         2,  2, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // v2u8_cf            
//    { OpenGL_ES_1_0,         2,  2, GL_FALSE, false, false, GL_BYTE                         }, // v2s8_cf  
//    { OpenGL_ES_2_0,         4,  2, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // v2u16_cf
//    { OpenGL_ES_1_0,         4,  2, GL_FALSE, false, false, GL_SHORT                        }, // v2s16_cf 
//    { OpenGL_ES_2_0,         3,  3, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // v3u8_cf 
//    { OpenGL_ES_1_0,         3,  3, GL_FALSE, false, false, GL_BYTE                         }, // v3s8_cf  
//    { OpenGL_ES_2_0,         6,  3, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // v3u16_cf
//    { OpenGL_ES_1_0,         6,  3, GL_FALSE, false, false, GL_SHORT                        }, // v3s16_cf 
//    { OpenGL_ES_2_0,         4,  4, GL_FALSE, false, false, GL_UNSIGNED_BYTE                }, // v4u8_cf 
//    { OpenGL_ES_1_0,         4,  4, GL_FALSE, false, false, GL_BYTE                         }, // v4s8_cf
//    { OpenGL_ES_2_0,         8,  4, GL_FALSE, false, false, GL_UNSIGNED_SHORT               }, // v4u16_cf 
//    { OpenGL_ES_1_0,         8,  4, GL_FALSE, false, false, GL_SHORT                        }, // v4s16_cf         

   // First OpenGL ES version that supports it
   const Nversion AttributeVersion[underlyingType(Attribute::Count)] =
      {
      OpenGL_ES_Unsupported, // None
      OpenGL_ES_2_0,         // u8_norm
      OpenGL_ES_2_0,         // s8_norm
      OpenGL_ES_3_0,         // u8
      OpenGL_ES_3_0,         // s8
      OpenGL_ES_2_0,         // u16_norm
      OpenGL_ES_2_0,         // s16_norm
      OpenGL_ES_3_0,         // u16
      OpenGL_ES_3_0,         // s16
      OpenGL_ES_3_0,         // f16
      OpenGL_ES_3_0,         // u32
      OpenGL_ES_3_0,         // s32
      OpenGL_ES_2_0,         // f32
      OpenGL_ES_Unsupported, // u64
      OpenGL_ES_Unsupported, // s64
      OpenGL_ES_Unsupported, // f64
      OpenGL_ES_2_0,         // v2u8_norm
      OpenGL_ES_1_1,         // v2s8_norm
      OpenGL_ES_3_0,         // v2u8
      OpenGL_ES_3_0,         // v2s8
      OpenGL_ES_2_0,         // v2u16_norm
      OpenGL_ES_1_1,         // v2s16_norm
      OpenGL_ES_3_0,         // v2u16
      OpenGL_ES_3_0,         // v2s16
      OpenGL_ES_3_0,         // v2f16
      OpenGL_ES_3_0,         // v2u32
      OpenGL_ES_3_0,         // v2s32
      OpenGL_ES_1_0,         // v2f32
      OpenGL_ES_Unsupported, // v2u64
      OpenGL_ES_Unsupported, // v2s64
      OpenGL_ES_Unsupported, // v2f64
      OpenGL_ES_2_0,         // v3u8_norm
      OpenGL_ES_Unsupported, // v3u8_srgb
      OpenGL_ES_1_1,         // v3s8_norm
      OpenGL_ES_3_0,         // v3u8
      OpenGL_ES_3_0,         // v3s8
      OpenGL_ES_2_0,         // v3u16_norm
      OpenGL_ES_1_1,         // v3s16_norm
      OpenGL_ES_3_0,         // v3u16
      OpenGL_ES_3_0,         // v3s16
      OpenGL_ES_3_0,         // v3f16
      OpenGL_ES_3_0,         // v3u32
      OpenGL_ES_3_0,         // v3s32
      OpenGL_ES_1_0,         // v3f32
      OpenGL_ES_Unsupported, // v3u64
      OpenGL_ES_Unsupported, // v3s64
      OpenGL_ES_Unsupported, // v3f64
      OpenGL_ES_2_0,         // v4u8_norm
      OpenGL_ES_1_1,         // v4s8_norm
      OpenGL_ES_3_0,         // v4u8
      OpenGL_ES_3_0,         // v4s8
      OpenGL_ES_2_0,         // v4u16_norm
      OpenGL_ES_1_1,         // v4s16_norm
      OpenGL_ES_3_0,         // v4u16
      OpenGL_ES_3_0,         // v4s16
      OpenGL_ES_3_0,         // v4f16
      OpenGL_ES_3_0,         // v4u32
      OpenGL_ES_3_0,         // v4s32
      OpenGL_ES_1_0,         // v4f32
      OpenGL_ES_Unsupported, // v4u64
      OpenGL_ES_Unsupported, // v4s64
      OpenGL_ES_Unsupported, // v4f64
      OpenGL_ES_Unsupported, // v3f11_11_10
      OpenGL_ES_Unsupported, // v4u10_10_10_2_norm
      OpenGL_ES_Unsupported, // v4s10_10_10_2_norm
      OpenGL_ES_Unsupported, // v4u10_10_10_2
      OpenGL_ES_Unsupported, // v4s10_10_10_2
      OpenGL_ES_3_0,         // v4u10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW (see ARB_vertex_array_bgra)
      OpenGL_ES_3_0,         // v4s10_10_10_2_norm_rev  - Comaptiblity with D3D format ZYXW 
      OpenGL_ES_Unsupported, // v4u10_10_10_2_rev
      OpenGL_ES_Unsupported  // v4s10_10_10_2_rev
      };
#endif

   }
}
#endif
