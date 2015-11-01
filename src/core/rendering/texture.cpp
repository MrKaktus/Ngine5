///*
//
// Ngine v4.0
// 
// Module      : Texture.
// Requirements: none
// Description : Rendering context supports window
//               creation and management of graphics
//               resources. It allows programmer to
//               use easy abstraction layer that 
//               removes from him platform dependent
//               implementation of graphic routines.
//
//*/
//
//#include "Ngine4/core/log/log.h"
//#include "Ngine4/core/rendering/context.h"
//
//namespace en
//{
//   namespace gpu
//   {
//
//   TextureState::TextureState() :
//      type(Texture1D),
//      format(Unassigned),
//      width(0),
//      height(1),
//      depth(1),
//      layers(1),
//      samples(1),
//      mipmaps(1)
//   {
//   }     
//   
//   TextureState::TextureState(const TextureType _type, 
//      const TextureFormat _format,
//      const uint16 _width,
//      const uint16 _height,
//      const uint16 _depth,
//      const uint16 _layers,
//      const uint16 _samples )
//   {
//   type    = _type;
//   format  = _format;
//   width   = _width;
//   height  = _height;
//   depth   = _depth;
//   layers  = _layers;
//   samples = _samples;
//   mipmaps = 1;
//   }
//   
//   bool TextureState::operator !=(const TextureState& b)
//   {
//   // TextureState structure can be aligned to
//   // specified memory size. Therefore it is possible 
//   // that there will be unused bytes at its end. To
//   // perform proper memory comparison, only used
//   // bytes need to be compared.
//   return memcmp(this, &b, ((uint64)&samples - (uint64)this + sizeof(samples)) ) & 1;
//   }
//
//   Texture::Texture() :
//       ProxyInterface<class TextureDescriptor>(NULL)
//   {
//   }
//
//   Texture::Texture(const Texture& src) :
//      ProxyInterface<class TextureDescriptor>(src)
//   {
//   }
//              
//   Texture::Texture(TextureDescriptor* src) :
//       ProxyInterface<class TextureDescriptor>(src)
//   {
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//   void* Texture::map(const uint8 mipmap, const uint16 layer)
//   {
//   return pointer == NULL ? NULL : gl20::TextureMap(pointer, mipmap, layer);
//   }
//   
//   void* Texture::map(const en::gpu::TextureFace face, const uint8 mipmap, const uint16 layer)
//   {
//   return pointer == NULL ? NULL : gl20::TextureMap(pointer, face, mipmap, layer);
//   }
//       
//   bool Texture::unmap(void)
//   {
//#ifdef EN_OPENGL_DESKTOP
//   return pointer == NULL ? false : gl20::TextureUnmap(pointer);
//#endif
//#ifdef EN_OPENGL_MOBILE
//   return pointer == NULL ? false : es20::TextureUnmap(pointer);
//#endif
//   }
//
//   uint16 Texture::mipmaps(void) const
//   {
//   assert( pointer );
//   return pointer->mipmaps;
//   }
//
//   uint32 Texture::width(const uint8 mipmap) const
//   {
//   assert( pointer );
//   assert( pointer->mipmaps > mipmap );
//   return pointer->width >> mipmap;
//   //return nextPowerOfTwo(pointer->width) >> mipmap;
//   }
//
//   uint32 Texture::height(const uint8 mipmap) const
//   {
//   assert( pointer );
//   assert( pointer->mipmaps > mipmap );
//   return pointer->height >> mipmap;
//   //return nextPowerOfTwo(pointer->height) >> mipmap;
//   }
//
//   uint32 Texture::size(const uint8 mipmap) const
//   {
//   assert( pointer );
//   return gl20::TextureSize(pointer, mipmap);
//   }
//
//   void Texture::read(uint8* buffer, const uint8 mipmap) const
//   {
//   assert( pointer );
//   gl20::TextureRead(pointer, buffer, mipmap);
//   }
//
//   TextureFormat Texture::format(void) const
//   {
//   assert( pointer );
//   return pointer->format;
//   }
//
//   TextureType Texture::type(void) const
//   {
//   assert( pointer );
//   return pointer->type;
//   }
//
//   uint16 Texture::layers(void) const
//   {
//   assert( pointer );
//   return pointer->layers;
//   }
//  
//   Sampler::Sampler(class SamplerDescriptor* src) :
//       ProxyInterface<class SamplerDescriptor>(src)
//   {
//   }
//
//   //Sampler::~Sampler()
//   //{
//   //}
//
//   const char* Sampler::name(void)
//   {
//   assert(pointer);
//   return pointer->name;
//   }
//
//   bool Sampler::set(const Texture& texture)
//   {
//   if (!pointer)    // <- this can be resolved by always returning some dummy sampler
//      return false;
//   if (!texture)    // <- this can be resolved by always binding some dummy texture
//      return false;
//
//   // If texture is already assigned to other sampler unit it cannot
//   // be assigned to this one (in OpenGL 3.3 it can).
//   for(uint32 i=0; i<pointer->program->samplers.count; ++i)
//      if (pointer->program->samplers.list[i].unit->texture == texture)
//         return false;
//   
//   // TODO: Assure thread safety here as few threads at the same
//   //       time can try to assign the same texture to different
//   //       samplers of thesame program!
//   
//   pointer->unit->texture = texture;
//   return true;
//   }
//   
//   bool Sampler::magnification(const TextureFiltering filtering)
//   {
//   // Check if specified filtering can be assigned to magnification
//   if (!gl::TextureFiltering[filtering].magnification)
//      return false;
//   
//   // Set new filtering method and return
//   pointer->unit->magFilter = filtering;
//   pointer->unit->dirty.magFilter = true;
//   return true;
//   }
//   
//   bool Sampler::minification(const TextureFiltering filtering)
//   {
//   assert(pointer);
//   assert(pointer->unit);
//
//   // Set new filtering method and return
//   pointer->unit->minFilter = filtering;
//   pointer->unit->dirty.minFilter = true;
//   return true;
//   }
//   
//   bool Sampler::wraping(const TextureWraping coordU, const TextureWraping coordV, const TextureWraping coordW )
//   {
//   assert(pointer);
//   assert(pointer->unit);
//
//   pointer->unit->wrapS = coordU;
//   pointer->unit->wrapT = coordV;
//   pointer->unit->wrapR = coordW;
//   pointer->unit->dirty.wrapS = true;
//   pointer->unit->dirty.wrapT = true;
//   pointer->unit->dirty.wrapR = true;
//   return true;
//   }
//
//   Texture Interface::Texture::create(const TextureState& TextureState)
//   {
//#ifdef EN_OPENGL_DESKTOP
//   return gl20::TextureCreate(TextureState);
//#endif
//#ifdef EN_OPENGL_MOBILE
//   return es20::TextureCreate(TextureState);
//#endif
//   }
//
//   uint16 Interface::Texture::bitsPerTexel(const en::gpu::TextureFormat format)
//   {
//   if (format >= TextureFormatsCount)
//      return 0;
//
//   return en::gpu::gl::TextureFormat[format].srcBpp;
//   }
//
//   SurfaceDescriptor::SurfaceDescriptor() :
//      size(0),
//      face(0),
//      mipmap(0),
//      width(0),
//      height(0),
//      layer(0),
//      pointer(nullptr)
//   {
//   }
//
//   SurfaceDescriptor::~SurfaceDescriptor()
//   {
//   // Pointer or PBO needs to be released externally
//   // depending on supported OpenGL version.
//   }
//
//   TextureDescriptor::TextureDescriptor() :
//      type(Texture1D),
//      format(Unassigned),
//      width(0),
//      height(1),
//      depth(1),
//      layers(1),
//      samples(1),
//      mipmaps(1),
//      surface(nullptr),
//      id(0),
//      pbo(0)
//   {
//   }
//
//   TextureDescriptor::~TextureDescriptor()
//   {
//   // Surface needs to be freed externally to
//   // properly free memory or PBO depending on 
//   // supported OpenGL version.
//   }
//
//   SamplerUnitDescriptor::SamplerUnitDescriptor() :
//       texture(nullptr),
//       magFilter(Linear),
//       minFilter(Linear),
//       wrapS(Repeat),
//       wrapT(Repeat),
//       wrapR(Repeat)
//   {
//   }
//
//   SamplerUnitDescriptor::~SamplerUnitDescriptor()
//   {
//   }
//
//   SamplerDescriptor::SamplerDescriptor() :
//       program(NULL),
//       unit(NULL),
//       name(NULL),
//       elements(0),
//       location(0),
//       type(0),
//       dirty(true)
//   {
//   }
//
//   SamplerDescriptor::~SamplerDescriptor()
//   {
//    delete unit;
//    delete name;
//   }
//
//      namespace gl
//      {
//      bool TextureTypeSupported[TextureTypesCount];
//      bool TextureFormatSupported[TextureFormatsCount];
//
//      const uint16 TextureFace[TextureFacesCount] = 
//         { 
//         GL_TEXTURE_CUBE_MAP_POSITIVE_X,   // Right
//         GL_TEXTURE_CUBE_MAP_NEGATIVE_X,   // Left
//         GL_TEXTURE_CUBE_MAP_POSITIVE_Y,   // Top
//         GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,   // Bottom
//         GL_TEXTURE_CUBE_MAP_POSITIVE_Z,   // Front
//         GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    // Back
//         };
//
//      #ifdef EN_OPENGL_DESKTOP
//
//      const TextureTypeTranslation TextureType[TextureTypesCount] = 
//         { 
//         { OpenGL_1_1, GL_TEXTURE_1D                   },   // Texture1D                  
//         { OpenGL_3_0, GL_TEXTURE_1D_ARRAY             },   // Texture1DArray             
//         { OpenGL_1_0, GL_TEXTURE_2D                   },   // Texture2D                  
//         { OpenGL_3_0, GL_TEXTURE_2D_ARRAY             },   // Texture2DArray             
//         { OpenGL_3_1, GL_TEXTURE_RECTANGLE            },   // Texture2DRectangle         
//         { OpenGL_3_2, GL_TEXTURE_2D_MULTISAMPLE       },   // Texture2DMultisample       
//         { OpenGL_3_2, GL_TEXTURE_2D_MULTISAMPLE_ARRAY },   // Texture2DMultisampleArray  
//         { OpenGL_1_2, GL_TEXTURE_3D                   },   // Texture3D                  
//         { OpenGL_3_1, GL_TEXTURE_BUFFER               },   // TextureBuffer              
//         { OpenGL_1_3, GL_TEXTURE_CUBE_MAP             },   // TextureCubeMap             
//         { OpenGL_4_0, GL_TEXTURE_CUBE_MAP_ARRAY       }    // TextureCubeMapArray  
//         };
//           
//      const TextureFormatTranslation TextureFormat[TextureFormatsCount] = 
//         { 
//         { 0,                  false,   0, 0,                     0,                  0                                 },   // Unassigned   
//         { OpenGL_3_0,         false,   8, GL_R8,                 GL_RED,             GL_UNSIGNED_BYTE                  },   // FormatR_8  
//         { OpenGL_3_1,         false,   8, GL_R8_SNORM,           GL_RED,             GL_BYTE                           },   // FormatR_8_sn
//         { OpenGL_3_0,         false,   8, GL_R8UI,               GL_RED_INTEGER,     GL_UNSIGNED_BYTE                  },   // FormatR_8_u
//         { OpenGL_3_0,         false,   8, GL_R8I,                GL_RED_INTEGER,     GL_BYTE                           },   // FormatR_8_s
//         { OpenGL_3_0,         false,  16, GL_R16,                GL_RED,             GL_UNSIGNED_SHORT                 },   // FormatR_16 
//         { OpenGL_3_1,         false,  16, GL_R16_SNORM,          GL_RED,             GL_SHORT                          },   // FormatR_16_sn 
//         { OpenGL_3_0,         false,  16, GL_R16UI,              GL_RED_INTEGER,     GL_UNSIGNED_SHORT                 },   // FormatR_16_u 
//         { OpenGL_3_0,         false,  16, GL_R16I,               GL_RED_INTEGER,     GL_SHORT                          },   // FormatR_16_s 
//         { OpenGL_3_0,         false,  16, GL_R16F,               GL_RED,             GL_HALF_FLOAT                     },   // FormatR_16_hf
//         { OpenGL_3_0,         false,  16, GL_R16F,               GL_RED,             GL_FLOAT                          },   // FormatR_16_f ( conversion: float -> f16 )
//         { OpenGL_3_0,         false,  32, GL_R32UI,              GL_RED_INTEGER,     GL_UNSIGNED_INT                   },   // FormatR_32_u 
//         { OpenGL_3_0,         false,  32, GL_R32I,               GL_RED_INTEGER,     GL_INT                            },   // FormatR_32_s 
//         { OpenGL_3_0,         false,  32, GL_R32F,               GL_RED,             GL_FLOAT                          },   // FormatR_32_f  
//         { OpenGL_3_0,         false,  16, GL_RG8,                GL_RG,              GL_UNSIGNED_BYTE                  },   // FormatGR_8 
//         { OpenGL_3_1,         false,  16, GL_RG8_SNORM,          GL_RG,              GL_BYTE                           },   // FormatGR_8_sn
//         { OpenGL_3_0,         false,  16, GL_RG8UI,              GL_RG_INTEGER,      GL_UNSIGNED_BYTE                  },   // FormatGR_8_u
//         { OpenGL_3_0,         false,  16, GL_RG8I,               GL_RG_INTEGER,      GL_BYTE                           },   // FormatGR_8_s
//         { OpenGL_3_0,         false,  32, GL_RG16,               GL_RG,              GL_UNSIGNED_SHORT                 },   // FormatGR_16 
//         { OpenGL_3_1,         false,  32, GL_RG16_SNORM,         GL_RG,              GL_SHORT                          },   // FormatGR_16_sn
//         { OpenGL_3_0,         false,  32, GL_RG16UI,             GL_RG_INTEGER,      GL_UNSIGNED_SHORT                 },   // FormatGR_16_u
//         { OpenGL_3_0,         false,  32, GL_RG16I,              GL_RG_INTEGER,      GL_SHORT                          },   // FormatGR_16_s
//         { OpenGL_3_0,         false,  32, GL_RG16F,              GL_RG,              GL_HALF_FLOAT                     },   // FormatGR_16_hf
//         { OpenGL_3_0,         false,  32, GL_RG16F,              GL_RG,              GL_FLOAT                          },   // FormatGR_16_f ( conversion: float -> f16 )
//         { OpenGL_3_0,         false,  64, GL_RG32UI,             GL_RG_INTEGER,      GL_UNSIGNED_INT                   },   // FormatGR_32_u
//         { OpenGL_3_0,         false,  64, GL_RG32I,              GL_RG_INTEGER,      GL_INT                            },   // FormatGR_32_s
//         { OpenGL_3_0,         false,  64, GL_RG32F,              GL_RG,              GL_FLOAT                          },   // FormatGR_32_f 
//         { OpenGL_1_2,         false,   8, GL_R3_G3_B2,           GL_RGB,             GL_UNSIGNED_BYTE_2_3_3_REV        },   // FormatRGB_3_3_2       
//         { OpenGL_1_2,         false,   8, GL_R3_G3_B2,           GL_RGB,             GL_UNSIGNED_BYTE_3_3_2            },   // FormatBGR_2_3_3  
//         { OpenGL_1_3,         false,  12, GL_RGB4,               GL_RGB,             GL_UNSIGNED_BYTE                  },   // FormatBGR_4    ( conversion: u8 -> u4 ? or packed 2 in one byte? )
//         { OpenGL_1_3,         false,  15, GL_RGB5,               GL_RGB,             GL_UNSIGNED_BYTE                  },   // FormatBGR_5    ( conversion: u8 -> u5 )     
//         { OpenGL_1_2,         false,  16, GL_RGB565,             GL_RGB,             GL_UNSIGNED_SHORT_5_6_5_REV       },   // FormatRGB_5_6_5    // OpenGL, Direct3D, Metal
//         { OpenGL_1_2,         false,  16, GL_RGB565,             GL_RGB,             GL_UNSIGNED_SHORT_5_6_5           },   // FormatBGR_5_6_5    // OpenGL, OpenGL ES
//         { OpenGL_1_2,         false,  24, GL_RGB8,               GL_BGR,             GL_UNSIGNED_BYTE                  },   // FormatRGB_8 
//         { OpenGL_1_0,         false,  24, GL_RGB8,               GL_RGB,             GL_UNSIGNED_BYTE                  },   // FormatBGR_8 
//         { OpenGL_3_1,         false,  24, GL_RGB8_SNORM,         GL_RGB,             GL_BYTE                           },   // FormatBGR_8_sn
//         { OpenGL_3_0,         false,  24, GL_RGB8UI,             GL_RGB_INTEGER,     GL_UNSIGNED_BYTE                  },   // FormatBGR_8_u
//         { OpenGL_3_0,         false,  24, GL_RGB8I,              GL_RGB_INTEGER,     GL_BYTE                           },   // FormatBGR_8_s 
//         { OpenGL_3_0,         false,  24, GL_SRGB8,              GL_RGB,             GL_UNSIGNED_BYTE                  },   // Format_sBGR_8 
//         { OpenGL_1_3,         false,  30, GL_RGB10,              GL_RGB,             GL_UNSIGNED_SHORT                 },   // FormatBGR_10    ( conversion: u16 -> u10 ) this is rendertarget ?? X2 ??
//         { OpenGL_3_0,         false,  32, GL_R11F_G11F_B10F,     GL_RGB,             GL_UNSIGNED_INT_10F_11F_11F_REV   },   // FormatRGB_11_11_10_f   
//         { OpenGL_3_0,         false,  32, GL_RGB9_E5,            GL_RGBA,            GL_UNSIGNED_INT_5_9_9_9_REV       },   // FormatRGBe_9_9_9_5_f   
//         { OpenGL_1_3,         false,  36, GL_RGB12,              GL_RGB,             GL_UNSIGNED_SHORT                 },   // FormatBGR_12    ( conversion: u16 -> u12 )
//         { OpenGL_1_2,         false,  48, GL_RGB16,              GL_BGR,             GL_UNSIGNED_SHORT                 },   // FormatRGB_16 
//         { OpenGL_1_0,         false,  48, GL_RGB16,              GL_RGB,             GL_UNSIGNED_SHORT                 },   // FormatBGR_16 
//         { OpenGL_3_1,         false,  48, GL_RGB16_SNORM,        GL_RGB,             GL_SHORT                          },   // FormatBGR_16_sn 
//         { OpenGL_3_0,         false,  48, GL_RGB16UI,            GL_RGB_INTEGER,     GL_UNSIGNED_SHORT                 },   // FormatBGR_16_u
//         { OpenGL_3_0,         false,  48, GL_RGB16I,             GL_RGB_INTEGER,     GL_SHORT                          },   // FormatBGR_16_s 
//         { OpenGL_3_0,         false,  48, GL_RGB16F,             GL_RGB,             GL_HALF_FLOAT                     },   // FormatBGR_16_hf
//         { OpenGL_3_0,         false,  48, GL_RGB16F,             GL_RGB,             GL_FLOAT                          },   // FormatBGR_16_f ( conversion: float -> f16 )
//         { OpenGL_3_0,         false,  96, GL_RGB32UI,            GL_RGB_INTEGER,     GL_UNSIGNED_INT                   },   // FormatBGR_32_u
//         { OpenGL_3_0,         false,  96, GL_RGB32I,             GL_RGB_INTEGER,     GL_INT                            },   // FormatBGR_32_s
//         { OpenGL_3_0,         false,  96, GL_RGB32F,             GL_RGB,             GL_FLOAT                          },   // FormatBGR_32_f  
//         { OpenGL_1_3,         false,   8, GL_RGBA2,              GL_RGBA,            GL_UNSIGNED_BYTE                  },   // FormatABGR_2    ( conversion: u8 -> u2 ? or packed 4 in one byte?) 
//         { OpenGL_1_2,         false,  16, GL_RGBA4,              GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4_REV     },   // FormatRGBA_4    // Metal only.
//         { OpenGL_1_2,         false,  16, GL_RGBA4,              GL_BGRA,            GL_UNSIGNED_SHORT_4_4_4_4_REV     },   // FormatBGRA_4          
//         { OpenGL_1_2,         false,  16, GL_RGBA4,              GL_BGRA,            GL_UNSIGNED_SHORT_4_4_4_4         },   // FormatARGB_4    // OpenGL and Direct3D            
//         { OpenGL_1_2,         false,  16, GL_RGBA4,              GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4         },   // FormatABGR_4    // OpenGL
//         { OpenGL_1_2,         false,  16, GL_RGB5_A1,            GL_RGBA,            GL_UNSIGNED_SHORT_1_5_5_5_REV     },   // FormatRGBA_5_5_5_1 // OpenGL, Metal.
//         { OpenGL_1_2,         false,  16, GL_RGB5_A1,            GL_BGRA,            GL_UNSIGNED_SHORT_1_5_5_5_REV     },   // FormatBGRA_5_5_5_1 // OpenGL, Direct3D.   
//         { OpenGL_1_2,         false,  16, GL_RGB5_A1,            GL_BGRA,            GL_UNSIGNED_SHORT_5_5_5_1         },   // FormatARGB_1_5_5_5 // OpenGL, OpenGL ES, Direct3D 11.1 from Win8+  
//         { OpenGL_1_2,         false,  16, GL_RGB5_A1,            GL_RGBA,            GL_UNSIGNED_SHORT_5_5_5_1         },   // FormatABGR_1_5_5_5 // OpenGL, OpenGL ES.
//         { OpenGL_1_2,         false,  32, GL_RGBA8,              GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV       },   // FormatRGBA_8
//         { OpenGL_1_2,         false,  32, GL_RGBA8,              GL_BGRA,            GL_UNSIGNED_INT_8_8_8_8_REV       },   // FormatBGRA_8
//         { OpenGL_1_2,         false,  32, GL_RGBA8,              GL_BGRA,            GL_UNSIGNED_BYTE                  },   // FormatARGB_8 
//// or      OpenGL_1_2,         false,  32, GL_RGBA8,              GL_BGRA,            GL_UNSIGNED_INT_8_8_8_8           
//         { OpenGL_1_0,         false,  32, GL_RGBA8,              GL_RGBA,            GL_UNSIGNED_BYTE                  },   // FormatABGR_8 
//// or 	   OpenGL_1_0,         false,  32, GL_RGBA8,              GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8             <- type GL_UNSIGNED_INT_8_8_8_8 was not supported in OpenGL 1.0
//         { OpenGL_3_0,         false,  32, GL_SRGB8_ALPHA8,       GL_RGBA,            GL_UNSIGNED_BYTE                  },   // Format_sABGR_8 
//         { OpenGL_3_1,         false,  32, GL_RGBA8_SNORM,        GL_RGBA,            GL_BYTE                           },   // FormatABGR_8_sn
//         { OpenGL_3_0,         false,  32, GL_RGBA8UI,            GL_RGBA_INTEGER,    GL_UNSIGNED_BYTE                  },   // FormatABGR_8_u  
//         { OpenGL_3_0,         false,  32, GL_RGBA8I,             GL_RGBA_INTEGER,    GL_BYTE                           },   // FormatABGR_8_s
//         { OpenGL_1_2,         false,  32, GL_RGB10_A2,           GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV    },   // FormatRGBA_10_10_10_2 <- OpenGL ES 3.0
//         { OpenGL_1_2,         false,  32, GL_RGB10_A2,           GL_BGRA,            GL_UNSIGNED_INT_2_10_10_10_REV    },   // FormatBGRA_10_10_10_2
//         { OpenGL_1_2,         false,  32, GL_RGB10_A2,           GL_BGRA,            GL_UNSIGNED_INT_10_10_10_2        },   // FormatARGB_2_10_10_10
//         { OpenGL_1_2,         false,  32, GL_RGB10_A2,           GL_RGBA,            GL_UNSIGNED_INT_10_10_10_2        },   // FormatABGR_2_10_10_10 
//         { OpenGL_3_3,         false,  32, GL_RGB10_A2UI,         GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV    },   // FormatRGBA_10_10_10_2_u <- OpenGL ES 3.0
//         { OpenGL_3_3,         false,  32, GL_RGB10_A2UI,         GL_BGRA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV    },   // FormatBGRA_10_10_10_2_u
//         { OpenGL_3_3,         false,  32, GL_RGB10_A2UI,         GL_BGRA_INTEGER,    GL_UNSIGNED_INT_10_10_10_2        },   // FormatARGB_2_10_10_10_u
//         { OpenGL_3_3,         false,  32, GL_RGB10_A2UI,         GL_RGBA_INTEGER,    GL_UNSIGNED_INT_10_10_10_2        },   // FormatABGR_2_10_10_10_u 
//         { OpenGL_1_3,         false,  48, GL_RGBA12,             GL_RGBA,            GL_UNSIGNED_SHORT                 },   // FormatABGR_12    ( conversion: u16 -> u12 ) 
//         { OpenGL_1_2,         false,  64, GL_RGBA16,             GL_BGRA,            GL_UNSIGNED_SHORT                 },   // FormatARGB_16   
//         { OpenGL_1_0,         false,  64, GL_RGBA16,             GL_RGBA,            GL_UNSIGNED_SHORT                 },   // FormatABGR_16 
//         { OpenGL_3_1,         false,  64, GL_RGBA16_SNORM,       GL_RGBA,            GL_SHORT                          },   // FormatABGR_16_sn
//         { OpenGL_3_0,         false,  64, GL_RGBA16UI,           GL_RGBA_INTEGER,    GL_UNSIGNED_SHORT                 },   // FormatABGR_16_u  
//         { OpenGL_3_0,         false,  64, GL_RGBA16I,            GL_RGBA_INTEGER,    GL_SHORT                          },   // FormatABGR_16_s
//         { OpenGL_3_0,         false,  64, GL_RGBA16F,            GL_RGBA,            GL_HALF_FLOAT                     },   // FormatABGR_16_hf 
//         { OpenGL_3_0,         false,  64, GL_RGBA16F,            GL_RGBA,            GL_FLOAT                          },   // FormatABGR_16_f ( conversion: f32 -> f16 )
//         { OpenGL_3_0,         false, 128, GL_RGBA32UI,           GL_RGBA_INTEGER,    GL_UNSIGNED_INT                   },   // FormatABGR_32_u  
//         { OpenGL_3_0,         false, 128, GL_RGBA32I,            GL_RGBA_INTEGER,    GL_INT                            },   // FormatABGR_32_s  
//         { OpenGL_3_0,         false, 128, GL_RGBA32F,            GL_RGBA,            GL_FLOAT                          },   // FormatABGR_32_f                           
//         { OpenGL_1_4,         false,  16, GL_DEPTH_COMPONENT16,  GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT                 },   // FormatD_16            
//         { OpenGL_1_4,         false,  24, GL_DEPTH_COMPONENT24,  GL_DEPTH_COMPONENT, GL_UNSIGNED_INT                   },   // FormatD_24 ( uses lo24bits from 32bits )
//         { OpenGL_1_4,         false,  32, GL_DEPTH_COMPONENT32,  GL_DEPTH_COMPONENT, GL_UNSIGNED_INT                   },   // FormatD_32    
//         { OpenGL_3_0,         false,  32, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT                          },   // FormatD_32_f
//         { OpenGL_3_0,         false,  32, GL_DEPTH24_STENCIL8,   GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8              },   // FormatDS_24_8
//         { OpenGL_3_0,         false,  64, GL_DEPTH32F_STENCIL8,  GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV },   // FormatDS_32f_8
//         { OpenGL_3_0, /*EXT*/ true,    8, 0,                     GL_COMPRESSED_RGB_S3TC_DXT1_EXT,                    0 },   // FormatBC1_RGB                // S3TC DXT1 EXT_texture_compression_s3tc
//         { OpenGL_3_0, /*EXT*/ true,    8, 0,                     GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,                   0 },   // FormatBC1_RGB_sRGB           // S3TC DXT1 EXT_texture_sRGB
//         { OpenGL_3_0, /*EXT*/ true,    8, 0,                     GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,                   0 },   // FormatBC1_RGBA               // S3TC DXT1 EXT_texture_compression_s3tc
//         { OpenGL_3_0, /*EXT*/ true,    8, 0,                     GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,             0 },   // FormatBC1_RGBA_sRGB          // S3TC DXT1 EXT_texture_sRGB
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatBC2_RGBA_pRGB          // S3TC DXT2 
//         { OpenGL_3_0, /*EXT*/ true,   16, 0,                     GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,                   0 },   // FormatBC2_RGBA               // S3TC DXT3 EXT_texture_compression_s3tc
//         { OpenGL_3_0, /*EXT*/ true,   16, 0,                     GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,             0 },   // FormatBC2_RGBA_sRGB          // S3TC DXT3 EXT_texture_sRGB 
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatBC3_RGBA_pRGB          // S3TC DXT4
//         { OpenGL_3_0, /*EXT*/ true,   16, 0,                     GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,                   0 },   // FormatBC3_RGBA               // S3TC DXT5 EXT_texture_compression_s3tc
//         { OpenGL_3_0, /*EXT*/ true,   16, 0,                     GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,             0 },   // FormatBC3_RGBA_sRGB          // S3TC DXT5 EXT_texture_sRGB
//         { OpenGL_3_0,         true,    8, 0,                     GL_COMPRESSED_RED_RGTC1,                            0 },   // FormatBC4_R                  // RGTC OpenGL 3.0  or  ARB_texture_compression_rgtc
//         { OpenGL_3_0,         true,    8, 0,                     GL_COMPRESSED_SIGNED_RED_RGTC1,                     0 },   // FormatBC4_R_sn               // RGTC
//         { OpenGL_3_0,         true,   16, 0,                     GL_COMPRESSED_RG_RGTC2,                             0 },   // FormatBC5_RG                 // RGTC
//         { OpenGL_3_0,         true,   16, 0,                     GL_COMPRESSED_SIGNED_RG_RGTC2,                      0 },   // FormatBC5_RG_sn              // RGTC
//         { OpenGL_4_2,         true,   16, 0,                     GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,                0 },   // FormatBC6H_RGB_f             // BPTC OpenGL 4.2  or  ARB_texture_compression_bptc 
//         { OpenGL_4_2,         true,   16, 0,                     GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,              0 },   // FormatBC6H_RGB_uf            // BPTC
//         { OpenGL_4_2,         true,   16, 0,                     GL_COMPRESSED_RGBA_BPTC_UNORM,                      0 },   // FormatBC7_RGBA               // BPTC
//         { OpenGL_4_2,         true,   16, 0,                     GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,                0 },   // FormatBC7_RGBA_sRGB          // BPTC
//         { OpenGL_4_3,         true,    8, 0,                     GL_COMPRESSED_R11_EAC,                              0 },   // FormatETC2_R_11              // EAC  OpenGL 4.3  or  OpenGL ES 3.0
//         { OpenGL_4_3,         true,    8, 0,                     GL_COMPRESSED_SIGNED_R11_EAC,                       0 },   // FormatETC2_R_11_sn           // EAC
//         { OpenGL_4_3,         true,   16, 0,                     GL_COMPRESSED_RG11_EAC,                             0 },   // FormatETC2_RG_11             // EAC
//         { OpenGL_4_3,         true,   16, 0,                     GL_COMPRESSED_SIGNED_RG11_EAC,                      0 },   // FormatETC2_RG_11_sn          // EAC
//         { OpenGL_4_3,         true,    8, 0,                     GL_COMPRESSED_RGB8_ETC2,                            0 },   // FormatETC2_RGB_8             // ETC1
//         { OpenGL_4_3,         true,    8, 0,                     GL_COMPRESSED_SRGB8_ETC2,                           0 },   // FormatETC2_RGB_8_sRGB        
//         { OpenGL_4_3,         true,   16, 0,                     GL_COMPRESSED_RGBA8_ETC2_EAC,                       0 },   // FormatETC2_RGBA_8            // EAC
//         { OpenGL_4_3,         true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,                0 },   // FormatETC2_RGBA_8_sRGB       // EAC
//         { OpenGL_4_3,         true,    8, 0,                     GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,        0 },   // FormatETC2_RGB8_A1           
//         { OpenGL_4_3,         true,    8, 0,                     GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,       0 },   // FormatETC2_RGB8_A1_sRGB      
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGB_2            
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGB_2_sRGB       
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGB_4            
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGB_4_sRGB       
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGBA_2           
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGBA_2_sRGB      
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGBA_4           
//         { OpenGL_Unsupported, true,    0, 0,                     0,                                                  0 },   // FormatPVRTC_RGBA_4_sRGB      
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_4x4_KHR,                    0 },   // FormatASTC_4x4                // GL_KHR_texture_compression_astc_ldr             
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_5x4_KHR,                    0 },   // FormatASTC_5x4                // GL_KHR_texture_compression_astc_hdr
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_5x5_KHR,                    0 },   // FormatASTC_5x5               
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_6x5_KHR,                    0 },   // FormatASTC_6x5               
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_6x6_KHR,                    0 },   // FormatASTC_6x6               
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_8x5_KHR,                    0 },   // FormatASTC_8x5               
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_8x6_KHR,                    0 },   // FormatASTC_8x6               
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_8x8_KHR,                    0 },   // FormatASTC_8x8               
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_10x5_KHR,                   0 },   // FormatASTC_10x5              
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_10x6_KHR,                   0 },   // FormatASTC_10x6              
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_10x8_KHR,                   0 },   // FormatASTC_10x8              
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_10x10_KHR,                  0 },   // FormatASTC_10x10             
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_12x10_KHR,                  0 },   // FormatASTC_12x10             
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_RGBA_ASTC_12x12_KHR,                  0 },   // FormatASTC_12x12             
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,            0 },   // FormatASTC_4x4_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,            0 },   // FormatASTC_5x4_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,            0 },   // FormatASTC_5x5_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,            0 },   // FormatASTC_6x5_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,            0 },   // FormatASTC_6x6_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,            0 },   // FormatASTC_8x5_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,            0 },   // FormatASTC_8x6_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,            0 },   // FormatASTC_8x8_sRGB          
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,           0 },   // FormatASTC_10x5_sRGB         
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,           0 },   // FormatASTC_10x6_sRGB         
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,           0 },   // FormatASTC_10x8_sRGB         
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,          0 },   // FormatASTC_10x10_sRGB        
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,          0 },   // FormatASTC_12x10_sRGB        
//         { OpenGL_Unsupported, true,   16, 0,                     GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,          0 }    // FormatASTC_12x12_sRGB  
//         };
//
//         // 1) What is the component order for GL_R11F_G11F_B10F when using: GL_UNSIGNED_INT_10F_11F_11F_REV and what when using GL_HALF_FLOAT or GL_FLOAT ?
//         // 1.b) The same with : GL_RGB9_E5 or GL_RGB5_A1?
//         // 2) What is the meaning of "GL_UNSIGNED_INT_8_8_8_8" if we can just use "GL_UNSIGNED_BYTE".
//         //    It should be deprecated in OpenGL 5.0.  -> endianness 
//         // 3) How we transfer data in cases like RGBA2? Do we have all 4 channels packed in one GL_UNSIGNED_BYTE? 
//         //    Or do we convert u8 to u2 which doesn't make much sense?
//         // 4) In OpenGL 3.3 Core Spec (2010/03/11), in table "Table 3.5: Packed pixel formats." there was added pixel formats:
//         //    RGB_INTEGER, RGBA_INTEGER, BGRA_INTEGER as supported for types:
//         //    GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_BYTE_3_3_2,
//         //    GL_UNSIGNED_BYTE_5_6_5_REV, GL_UNSIGNED_BYTE_5_6_5,
//         //    GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_4_4_4_4, 
//         //    GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_SHORT_5_5_5_1, 
//         //    GL_UNSIGNED_SHORT_8_8_8_8_REV, GL_UNSIGNED_SHORT_8_8_8_8, 
//         //    GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10_10_10_2
//         //    but in "Table 3.12" only one new signed internal format was added: GL_RGB10_A2UI which relates to GL_ARB_texture_rgb10_a2ui .
//         //    What is the purpose of rest of this new declarations ?
//         // 5) Was "GL_RGB565" signed internal format introduced in OpenGL 1.2 or 4.2 ?
//         //    It is present in glext.h since opengl 4.2.
//
//         // For OpenGL ES 3.0 see: http://www.khronos.org/opengles/sdk/docs/man3/xhtml/glTexImage2D.xml
//         // See also: http://www.opengl.org/wiki/Image_Format
//
//      const TextureFilteringTranslation TextureFiltering[TextureFilteringMethodsCount] = 
//         {
//         { OpenGL_1_0,         true,  1,  GL_NEAREST                },   // Nearest
//         { OpenGL_1_0,         false, 1,  GL_NEAREST_MIPMAP_NEAREST },   // NearestMipmaped
//         { OpenGL_1_0,         false, 1,  GL_NEAREST_MIPMAP_LINEAR  },   // NearestMipmapedSmooth
//         { OpenGL_1_0,         true,  1,  GL_LINEAR                 },   // Linear 
//         { OpenGL_1_0,         false, 1,  GL_LINEAR_MIPMAP_NEAREST  },   // Bilinear
//         { OpenGL_1_0,         false, 1,  GL_LINEAR_MIPMAP_LINEAR   },   // Trilinear   
//         { OpenGL_Unsupported, false, 2,  GL_LINEAR_MIPMAP_LINEAR   },   // Anisotropic2x  
//         { OpenGL_Unsupported, false, 4,  GL_LINEAR_MIPMAP_LINEAR   },   // Anisotropic4x  
//         { OpenGL_Unsupported, false, 8,  GL_LINEAR_MIPMAP_LINEAR   },   // Anisotropic8x  
//         { OpenGL_Unsupported, false, 16, GL_LINEAR_MIPMAP_LINEAR   }    // Anisotropic16x 
//         };
//
//      const TextureWrapingTranslation TextureWraping[TextureWrapingMethodsCount] = 
//         {
//         { OpenGL_1_0, GL_CLAMP                },   // Clamp         
//         { OpenGL_1_0, GL_REPEAT               },   // Repeat         
//         { OpenGL_1_4, GL_MIRRORED_REPEAT      },   // RepeatMirrored 
//         { OpenGL_4_4, GL_MIRROR_CLAMP_TO_EDGE },   // ClampMirrored
//         { OpenGL_Unsupported, 0               }    // ClampToBorder
//         };
//
//      #endif
//      #ifdef EN_OPENGL_MOBILE
//
//      const TextureTypeTranslation TextureType[TextureTypesCount] = 
//         { 
//         { OpenGL_ES_Unsupported, 0 /*GL_TEXTURE_1D*/                   },   // Texture1D                  
//         { OpenGL_ES_Unsupported, 0 /*GL_TEXTURE_1D_ARRAY*/             },   // Texture1DArray             
//         { OpenGL_ES_1_0,         GL_TEXTURE_2D                         },   // Texture2D                  
//         { OpenGL_ES_3_0,         GL_TEXTURE_2D_ARRAY                   },   // Texture2DArray             
//         { OpenGL_ES_Unsupported, 0 /*GL_TEXTURE_RECTANGLE*/            },   // Texture2DRectangle         
//         { OpenGL_ES_3_1,         GL_TEXTURE_2D_MULTISAMPLE             },   // Texture2DMultisample       
//         { OpenGL_ES_Unsupported, 0 /*GL_TEXTURE_2D_MULTISAMPLE_ARRAY*/ },   // Texture2DMultisampleArray  
//         { OpenGL_ES_3_0,         GL_TEXTURE_3D                         },   // Texture3D                  
//         { OpenGL_ES_Unsupported, 0 /*GL_TEXTURE_BUFFER*/               },   // TextureBuffer              
//         { OpenGL_ES_2_0,         GL_TEXTURE_CUBE_MAP                   },   // TextureCubeMap             
//         { OpenGL_ES_Unsupported, 0 /*GL_TEXTURE_CUBE_MAP_ARRAY*/       }    // TextureCubeMapArray  
//         };
//
//      const TextureFormatTranslation TextureFormat[TextureFormatsCount] = 
//         { 
//         { 0,                     false, 0,  0,                          0,                        0                                    },   // Unassigned            
//         { OpenGL_ES_Unsupported, false, 8,  0 /*GL_R3_G3_B2*/,          GL_RGB,                   0 /*GL_UNSIGNED_BYTE_2_3_3_REV*/     },   // FormatRGB_3_3_2       
//         { OpenGL_ES_Unsupported, false, 8,  0 /*GL_R3_G3_B2*/,          GL_RGB,                   0 /*GL_UNSIGNED_BYTE_3_3_2*/         },   // FormatBGR_2_3_3       
//         { OpenGL_ES_1_0,         false, 24, GL_RGB,                     GL_RGB,                   GL_UNSIGNED_BYTE                     },   // FormatRGB_8  
//         { OpenGL_ES_Unsupported, false, 24, 0 /*GL_RGB8*/,              0 /*GL_BGR*/,             GL_UNSIGNED_BYTE                     },   // FormatBGR_8         
//         { OpenGL_ES_Unsupported, false, 48, 0 /*GL_RGB16*/,             GL_RGB,                   GL_UNSIGNED_SHORT                    },   // FormatRGB_16  
//         { OpenGL_ES_Unsupported, false, 48, 0 /*GL_RGB16*/,             0 /*GL_BGR*/,             GL_UNSIGNED_SHORT                    },   // FormatBGR_16                  
//         { OpenGL_ES_Unsupported, false, 16, GL_RGBA4,                   GL_RGBA,                  0 /*GL_UNSIGNED_SHORT_4_4_4_4_REV*/  },   // FormatRGBA_4
//         { OpenGL_ES_Unsupported, false, 16, GL_RGBA4,                   0 /*GL_BGRA*/,            0 /*GL_UNSIGNED_SHORT_4_4_4_4_REV*/  },   // FormatBGRA_4          
//         { OpenGL_ES_1_0,         false, 16, GL_RGBA,                    GL_RGBA,                  GL_UNSIGNED_SHORT_4_4_4_4            },   // FormatABGR_4 
//         { OpenGL_ES_Unsupported, false, 16, GL_RGBA4,                   0 /*GL_BGRA*/,            GL_UNSIGNED_SHORT_4_4_4_4            },   // FormatARGB_4                   
//         { OpenGL_ES_Unsupported, false, 16, GL_RGB5_A1,                 GL_RGBA,                  0 /*GL_UNSIGNED_SHORT_1_5_5_5_REV*/  },   // FormatRGBA_5_5_5_1 
//         { OpenGL_ES_Unsupported, false, 16, GL_RGB5_A1,                 0 /*GL_BGRA*/,            0 /*GL_UNSIGNED_SHORT_1_5_5_5_REV*/  },   // FormatBGRA_5_5_5_1    
//         { OpenGL_ES_1_0,         false, 16, GL_RGBA,                    GL_RGBA,                  GL_UNSIGNED_SHORT_5_5_5_1            },   // FormatABGR_1_5_5_5 
//         { OpenGL_ES_Unsupported, false, 16, GL_RGB5_A1,                 0 /*GL_BGRA*/,            GL_UNSIGNED_SHORT_5_5_5_1            },   // FormatARGB_1_5_5_5   
//         { OpenGL_ES_1_0,         false, 32, GL_RGBA,                    GL_RGBA,                  GL_UNSIGNED_BYTE                     },   // FormatRGBA_8 
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGBA8*/,             0 /*GL_BGRA*/,            GL_UNSIGNED_BYTE                     },   // FormatBGRA_8              
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGBA8*/,             GL_RGBA,                  0 /*GL_UNSIGNED_INT_8_8_8_8*/        },   // FormatABGR_8
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGBA8*/,             0 /*GL_BGRA*/,            0 /*GL_UNSIGNED_INT_8_8_8_8*/        },   // FormatARGB_8
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGB10_A2*/,          GL_RGBA,                  0 /*GL_UNSIGNED_INT_2_10_10_10_REV*/ },   // FormatRGBA_10_10_10_2 
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGB10_A2*/,          0 /*GL_BGRA*/,            0 /*GL_UNSIGNED_INT_2_10_10_10_REV*/ },   // FormatBGRA_10_10_10_2
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGB10_A2*/,          GL_RGBA,                  0 /*GL_UNSIGNED_INT_10_10_10_2*/     },   // FormatABGR_2_10_10_10 
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_RGB10_A2*/,          0 /*GL_BGRA*/,            0 /*GL_UNSIGNED_INT_10_10_10_2*/     },   // FormatARGB_2_10_10_10
//         { OpenGL_ES_Unsupported, false, 64, 0 /*GL_RGBA16*/,            GL_RGBA,                  GL_UNSIGNED_SHORT                    },   // FormatRGBA_16 
//         { OpenGL_ES_Unsupported, false, 64, 0 /*GL_RGBA16*/,            0 /*GL_BGRA*/,            GL_UNSIGNED_SHORT                    },   // FormatBGRA_16        
//         { OpenGL_ES_Unsupported, false, 16, GL_DEPTH_COMPONENT16,       GL_DEPTH_COMPONENT,       GL_UNSIGNED_SHORT                    },   // FormatD_16            
//         { OpenGL_ES_Unsupported, false, 32, 0 /*GL_DEPTH_COMPONENT32*/, GL_DEPTH_COMPONENT,       GL_UNSIGNED_INT                      }    // FormatD_32    
//         };
//
//      const TextureFilteringTranslation TextureFiltering[TextureFilteringMethodsCount] = 
//         {
//         { OpenGL_ES_1_0,         true,  1,  GL_NEAREST                },   // Nearest
//         { OpenGL_ES_1_0,         false, 1,  GL_NEAREST_MIPMAP_NEAREST },   // NearestMipmaped
//         { OpenGL_ES_1_0,         false, 1,  GL_NEAREST_MIPMAP_LINEAR  },   // NearestMipmapedSmooth
//         { OpenGL_ES_1_0,         true,  1,  GL_LINEAR                 },   // Linear 
//         { OpenGL_ES_1_0,         false, 1,  GL_LINEAR_MIPMAP_NEAREST  },   // Bilinear
//         { OpenGL_ES_1_0,         false, 1,  GL_LINEAR_MIPMAP_LINEAR   },   // Trilinear    
//         { OpenGL_ES_Unsupported, false, 2,  GL_LINEAR_MIPMAP_LINEAR   },   // Anisotropic2x  
//         { OpenGL_ES_Unsupported, false, 4,  GL_LINEAR_MIPMAP_LINEAR   },   // Anisotropic4x  
//         { OpenGL_ES_Unsupported, false, 8,  GL_LINEAR_MIPMAP_LINEAR   },   // Anisotropic8x  
//         { OpenGL_ES_Unsupported, false, 16, GL_LINEAR_MIPMAP_LINEAR   }    // Anisotropic16x 
//         };
//
//      const TextureWrapingTranslation TextureWraping[TextureWrapingMethodsCount] = 
//         {
//         { OpenGL_ES_1_0, GL_CLAMP_TO_EDGE   },   // Clamp         
//         { OpenGL_ES_1_0, GL_REPEAT          },   // Repeat         
//         { OpenGL_ES_2_0, GL_MIRRORED_REPEAT },   // RepeatMirrored 
//         { OpenGL_Unsupported, 0             },   // ClampMirrored
//         { OpenGL_Unsupported, 0             }    // ClampToBorder
//         };
//
//      #endif
//      #ifdef EN_DEBUG
//
//      #endif
//      }
//
//      namespace gl20
//      {
//      void TexturesInit(void)
//      {
//      // Init information about currently supported texture types
//      for(uint16 i=0; i<TextureTypesCount; ++i)
//         {
//         gl::TextureTypeSupported[i] = false;
//         if (GpuContext.screen.api.release >= gl::TextureType[i].opengl.release)
//            gl::TextureTypeSupported[i] = true;
//         }
//
//      // Init information about currently supported texture formats
//      for(uint16 i=0; i<TextureFormatsCount; ++i)
//         {
//         gl::TextureFormatSupported[i] = false;
//         if (GpuContext.screen.api.release >= gl::TextureFormat[i].opengl.release)
//            gl::TextureFormatSupported[i] = true;
//         }
//      }
//
//      void TexturesClear(void)
//      {
//      // Clear information 
//      for(uint16 i=0; i<TextureTypesCount; ++i)
//         gl::TextureTypeSupported[i] = false;
//      for(uint16 i=0; i<TextureFormatsCount; ++i)
//         gl::TextureFormatSupported[i] = false;
//      }
//
//      // Checks if texture type is supported in current OpenGL Context
//      bool SupportTextureType(const TextureType type)
//      {
//      return gl::TextureTypeSupported[type];
//      }
//
//      // Checks if texture format is supported in current OpenGL Context
//      bool SupportTextureFormat(const TextureFormat type)
//      {
//      return gl::TextureFormatSupported[type];
//      }
//
//      #ifdef EN_OPENGL_DESKTOP
//      // Creates texture 
//      Texture TextureCreate(const TextureState& textureState)
//      {
//      // Check if texture type and format are supported in current Context
//      if ( !gl::TextureTypeSupported[textureState.type] ||
//           !gl::TextureFormatSupported[textureState.format] )
//         return Texture(NULL);
//
//      // Try to add new texture descriptor 
//      TextureDescriptor* texture = GpuContext.textures.allocate();
//      if (!texture)
//         return Texture(NULL);
//
//      // Fill texture descriptor with data
//      memcpy(&texture->type, &textureState, sizeof(TextureState));
//      texture->mipmaps = 0;
//      texture->surface = NULL;
//      texture->id      = 0;
//
//      // Extract information needed for texture creation in OpenGL
//      uint16 texType    = gl::TextureType[textureState.type].type;
//      uint16 dstFormat  = gl::TextureFormat[textureState.format].dstFormat;
//      uint16 srcFormat  = gl::TextureFormat[textureState.format].srcFormat;
//      uint16 srcType    = gl::TextureFormat[textureState.format].srcType; 
//
//      // Generate empty texture in driver
//      Profile( glGenTextures(1, (GLuint*)&texture->id) );
//      Profile( glBindTexture(texType, texture->id) );
//      if (textureState.type != Texture2DRectangle)
//         {
//         Profile( glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT) );
//         Profile( glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT) );
//         Profile( glTexParameteri(texType, GL_TEXTURE_WRAP_R, GL_REPEAT) );
//         }
//      Profile( glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
//      Profile( glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
//           
//      Profile( glGenBuffers(1, &texture->pbo) );
//    
//      // Reserve memory for texture completness (OpenGL 2.0+)
//      if (textureState.type == Texture1D)
//         {
//         uint16 width = textureState.width;
//         for (uint8 i=0; width != 0; ++i)
//             {
//             Profile( glTexImage1D(GL_TEXTURE_1D, i, dstFormat, width, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//             width = width >> 1;
//             }
//         }
//      if (textureState.type == Texture2D)
//         {
//         uint16 width  = textureState.width;
//         uint16 height = textureState.height;
//         for (uint8 i=0; ; ++i)
//             {
//             Profile( glTexImage2D(GL_TEXTURE_2D, i, dstFormat, width, height, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//      
//             if ( (width  == 1) && 
//                  (height == 1) )
//                break;
//             if (width  > 1) width  = width  >> 1;
//             if (height > 1) height = height >> 1;
//             }
//         }
//      if (textureState.type == Texture3D)
//         {
//         uint16 width  = textureState.width;
//         uint16 height = textureState.height;
//         uint16 depth  = textureState.depth;
//         for (uint8 i=0; ; ++i)
//             {
//             Profile( glTexImage3D(GL_TEXTURE_3D, i, dstFormat, width, height, depth, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//      
//             if ( (width  == 1) && 
//                  (height == 1) &&
//                  (depth  == 1) )
//                break;
//             if (width  > 1) width  = width  >> 1;
//             if (height > 1) height = height >> 1;
//             if (depth  > 1) depth  = depth  >> 1;
//             }
//         }
//      if (textureState.type == TextureCubeMap)
//         {
//         uint16 width = textureState.width;
//         for (uint8 i=0; width != 0; ++i)
//             {
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//             width = width >> 1;
//             }
//         }
//      
//      // Reserve memory for texture completness (OpenGL 3.0+)
//      if (textureState.type == Texture1DArray)
//         {
//         uint16 width  = textureState.width;
//         for (uint8 i=0; width != 0; ++i)
//             {
//             Profile( glTexImage2D(GL_TEXTURE_1D_ARRAY, i, dstFormat, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//             width = width >> 1;
//             }
//         }
//      if (textureState.type == Texture2DArray)
//         {
//         uint16 width  = textureState.width;
//         uint16 height = textureState.height;
//         for (uint8 i=0; ; ++i)
//             {
//             Profile( glTexImage3D(GL_TEXTURE_2D_ARRAY, i, dstFormat, width, height, textureState.layers, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//                 
//             if ( (width  == 1) && 
//                  (height == 1) )
//                break;
//             if (width  > 1) width  = width  >> 1;
//             if (height > 1) height = height >> 1;
//             }
//         }
//      
//      // Reserve memory for texture completness (OpenGL 3.1+)
//      if (textureState.type == Texture2DRectangle)
//         {
//         Profile( glTexImage2D(GL_TEXTURE_RECTANGLE, 0, dstFormat, textureState.width, textureState.height, 0, srcFormat, srcType, NULL) );
//         texture->mipmaps = 1;
//         }
//      
//      if (textureState.type == TextureBuffer)
//         {
//         // TODO: Finish !
//         // Should be supported through 'Buffer' bindable to sampler
//         }
//
//      // Reserve memory for texture completness (OpenGL 3.2+)
//      if (textureState.type == Texture2DMultisample)
//         {
//         // TODO: Check samples count (depth/color/integer), destination format must be (color/depth/stencil)-renderable
//         Profile( glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, textureState.samples, dstFormat, textureState.width, textureState.height, GL_TRUE) );
//         texture->mipmaps = 1;
//         }
//      
//      if (textureState.type == Texture2DMultisampleArray)
//         {
//         // TODO: Check samples count (depth/color/integer), destination format must be (color/depth/stencil)-renderable
//         Profile( glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, textureState.samples, dstFormat, textureState.width, textureState.height, textureState.layers, GL_TRUE) );
//         texture->mipmaps = 1;
//         }
//      
//      // Reserve memory for texture completness (OpenGL 4.0+)
//      if (textureState.type == TextureCubeMapArray)
//         {
//         uint16 width = textureState.width;
//         for (uint8 i=0; width != 0; ++i)
//             {
//             Profile( glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, dstFormat, width, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, dstFormat, width, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, dstFormat, width, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, dstFormat, width, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, dstFormat, width, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, dstFormat, width, width, textureState.layers, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//             width = width >> 1;
//             }
//         }
//
//      // Return texture interface
//      return Texture(texture);
//      }
//      #endif
//
//      //// Returns size of texture
//      //uint32 TextureSize(const TextureDescriptor* texture)
//      //{
//      //assert(texture);
//
//      ////if (texture->type == Texture1D)
//      ////   return texture->width;
//
//      ////if (texture->type == Texture1DArray)
//      ////   return texture->width * texture->layers;
//
//      ////if (texture->type == Texture2D)
//      ////   return texture->width * texture->height;
//
//      ////if (texture->type == Texture2DArray)
//      ////   return texture->width * texture->height * texture->layers;
//
//      ////if (texture->type == Texture2DRectangle)
//      ////   return texture->width * texture->height;
//
//      ////if (texture->type == Texture2DMultisample)
//      ////   return texture->width * texture->height * texture->samples;
//
//      ////if (texture->type == Texture2DMultisampleArray)
//      ////   return texture->width * texture->height * texture->samples * texture->layers;
//
//      ////if (texture->type == Texture3D)
//      ////   return texture->width * texture->height * texture->depth;
//
//      ////// TODO: if (texture->type == TextureBuffer)
//
//      ////if (texture->type == TextureCubeMap)
//      ////if (texture->type == TextureCubeMapArray)
//      //return 0;
//      //}
//
//      //// Returns size of specified mipmap
//      //uint32 TextureSize(const TextureDescriptor* texture, const uint8 mipmap)
//      //{
//      //assert(texture);
//
//      //// Check if specified mipmap is correct
//      //if (texture->mipmaps <= mipmap)
//      //   return 0;
//
//      ////uint32 size = texture->width * 
//      ////              texture->height * 
//      ////              texture->depth * 
//      ////              texture->layers * 
//      ////              texture->samples *
//      ////              texture->
//      //return 0;
//      //}
//
//      // Maps texture memory
//      void* TextureMap(TextureDescriptor* texture, const uint8 mipmap, const uint16 layer)
//      {
//      assert(texture);
//      
//      // Check if specified mipmap and layer are correct
//      if (texture->mipmaps <= mipmap)
//         return NULL;
//      if (texture->type == Texture3D)
//         if (texture->depth <= layer)
//            return NULL;
//      else
//         if (texture->layers <= layer)
//            return NULL;
//      
//      // Check if texture isn't already mapped
//      if (texture->surface)
//         return NULL;
//      
//      // Checks if specified texture type is supported
//      if (!gl::TextureTypeSupported[texture->type])
//         return NULL;
//      
//      // Allocate descriptor for mapped texture surface
//      SurfaceDescriptor* surface = new SurfaceDescriptor;
//      if (!surface)
//         return NULL;
//
//      surface->width   = texture->width  >> surface->mipmap;
//      surface->height  = texture->height >> surface->mipmap;
//      surface->size    = surface->width * surface->height * (gl::TextureFormat[texture->format].srcBpp >> 3);
//      surface->mipmap  = mipmap;
//      surface->layer   = layer;
//      surface->face    = 0;
//      
//      if (GpuContext.screen.support(OpenGL_2_1))
//         {
//         // Discard previous PBO data to prevent CPU-GPU synchronization.
//         void* ptr;
//         Profile( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, texture->pbo) );
//         Profile( glBufferData( GL_PIXEL_UNPACK_BUFFER, surface->size, 0, GL_STREAM_DRAW) );
//         Profile( ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY) );
//
//         // Mark texture as mapped
//         texture->surface = surface;
//         return ptr;
//         }
//
//      // In OpenGL 2.0 there is no PBO implementation 
//      // so it needs to be emulated. Instead of using
//      // PBO, local memory is allocated.
//      surface->pointer = new uint8[surface->size];
//      if (surface->pointer == NULL)
//         {
//         delete surface;
//         return NULL;
//         }
//
//      // Mark texture as mapped
//      texture->surface = surface;
//      return surface->pointer;
//      }
//
//      // Maps cubemap texture memory
//      void* TextureMap(TextureDescriptor* texture, const TextureFace face, const uint8 mipmap, const uint16 layer)
//      {
//      assert(texture);
//      
//      // Check if specified face, mipmap and layer are correct
//      assert(texture->mipmaps > mipmap);
//      assert(texture->layers > layer);
//      assert(face < 6);
//      
//      // Check if texture is CubeMap
//      assert( ((texture->type == TextureCubeMap) && (layer == 0)) ||
//               (texture->type == TextureCubeMapArray) );
//
//      // Check if texture isn't already mapped
//      if (texture->surface)
//         return nullptr;
//    
//      // Checks if specified texture type is supported
//      if (!gl::TextureTypeSupported[texture->type])
//         return nullptr;
//      
//      // Allocate descriptor for mapped texture surface
//      SurfaceDescriptor* surface = new SurfaceDescriptor;
//      if (!surface)
//         return nullptr;
//      
//      surface->width   = texture->width  >> surface->mipmap;
//      surface->height  = texture->height >> surface->mipmap;
//      surface->size    = surface->width * surface->height * (gl::TextureFormat[texture->format].srcBpp >> 3);
//      surface->mipmap  = mipmap;
//      surface->layer   = layer;
//      surface->face    = face;
//      
//      if (GpuContext.screen.support(OpenGL_2_1))
//         {
//         // Discard previous PBO data to prevent CPU-GPU synchronization.
//         void* ptr;
//         Profile( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, texture->pbo) );
//         Profile( glBufferData( GL_PIXEL_UNPACK_BUFFER, surface->size, 0, GL_STREAM_DRAW) );
//         Profile( ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY) );
//
//         // Mark texture as mapped
//         texture->surface = surface;
//         return ptr;
//         }
//
//      // In OpenGL 2.0 there is no PBO implementation 
//      // so it needs to be emulated. Instead of using
//      // PBO, local memory is allocated.
//      surface->pointer = new uint8[surface->size];
//      if (surface->pointer == NULL)
//         {
//         delete surface;
//         return NULL;
//         }
//      
//      // Mark texture as mapped
//      texture->surface = surface;
//      return surface->pointer;
//      }
//
//      #ifdef EN_OPENGL_DESKTOP
//      bool TextureUnmap(TextureDescriptor* texture)
//      {
//      assert(texture);
//      
//      // Check if texture is mapped. There is no need
//      // to check if texture type is supported, because
//      // it was checked already when it was mapped.
//      if (!texture->surface)
//         return false;
//      
//      // Extract information needed for texture update in OpenGL
//      SurfaceDescriptor* surface = texture->surface;
//      uint16 texType    = gl::TextureType[texture->type].type;
//      bool   compressed = gl::TextureFormat[texture->format].compressed;
//      uint16 srcFormat  = gl::TextureFormat[texture->format].srcFormat;
//      uint16 srcType    = gl::TextureFormat[texture->format].srcType; 
//      
//      // Unmount PBO after all data is written to it
//      if (GpuContext.screen.support(OpenGL_2_1))
//         Profile( glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER) );
//
//      // Send texture to GPU. If PBO is supported, texture
//      // data will be transferred from it using DMA. Otherwise
//      // it wil be copied in traditional way from client memory.
//      // In both ways it is correct to pass surface->pointer
//      // as data source, as in PBO case it will resolve to 0 offset.
//      Profile( glBindTexture(texType, texture->id) );
//      
//      if (texture->type == Texture1D)
//         {
//         if (compressed)
//            Profile( glCompressedTexSubImage1D(GL_TEXTURE_1D, surface->mipmap, 0, surface->width, srcFormat, surface->size, surface->pointer) )
//         else
//            Profile( glTexSubImage1D(GL_TEXTURE_1D, surface->mipmap, 0, surface->width, srcFormat, srcType, surface->pointer) );
//         }
//      
//      if (texture->type == Texture2D)
//         {
//         if (compressed)
//            Profile( glCompressedTexSubImage2D(GL_TEXTURE_2D, surface->mipmap, 0, 0, surface->width, surface->height, srcFormat, surface->size, surface->pointer) )
//         else
//            Profile( glTexSubImage2D(GL_TEXTURE_2D, surface->mipmap, 0, 0, surface->width, surface->height, srcFormat, srcType, surface->pointer) );
//         }
//      
//      if (texture->type == Texture3D)
//         {
//         if (compressed)
//            Profile( glCompressedTexSubImage3D(GL_TEXTURE_3D, surface->mipmap, 0, 0, 0, surface->width, surface->height, surface->depth, srcFormat, surface->size, surface->pointer) )
//         else
//            Profile( glTexSubImage3D(GL_TEXTURE_3D, surface->mipmap, 0, 0, 0, surface->width, surface->height, surface->depth, srcFormat, srcType, surface->pointer) );    
//         }
//      
//      if (texture->type == TextureCubeMap)
//         {
//         uint16 texFace = gl::TextureFace[surface->face];
//         if (compressed)
//            Profile( glCompressedTexSubImage2D(texFace, surface->mipmap, 0, 0, surface->width, surface->width, srcFormat, surface->size, surface->pointer) )
//         else    
//            Profile( glTexSubImage2D(texFace, surface->mipmap, 0, 0, surface->width, surface->width, srcFormat, srcType, surface->pointer) );
//         }
//
//      // Other texture types are not supported in OpenGL 2.0.
//      // Future versions of OpenGL uses PBO implementation for
//      // support of textures update.
//      if (texture->type == Texture2DArray)
//         {
//         if (compressed)
//            Profile( glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, 
//                                               surface->mipmap, 
//                                               0, 0,                  // x,y,z - location
//                                               surface->layer, 
//                                               surface->width,        // w,h,d - size
//                                               surface->height, 
//                                               1, 
//                                               srcFormat, 
//                                               surface->size, 
//                                               surface->pointer) )
//         else
//            Profile( glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 
//                                     surface->mipmap, 
//                                     0, 0,                 // x,y,z - location
//                                     surface->layer, 
//                                     surface->width,       // w,h,d - size
//                                     surface->height, 
//                                     1, 
//                                     srcFormat, 
//                                     srcType, 
//                                     surface->pointer) )
//         }
//    
//      // Unbind PBO restoring standard pixel operations.
//      if (GpuContext.screen.support(OpenGL_2_1))
//         Profile( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0) );
//
//      // Free surface descriptor and finish
//      if (GpuContext.screen.support(OpenGL_2_1))
//         delete surface->pointer;
//      delete surface;
//      texture->surface = nullptr;
//      return true;
//      }
//      #endif
//
//
//
//      struct TextureCompressedBlockInfo
//         {
//         uint32 width      : 8;   // Block width
//         uint32 height     : 8;   // Block height
//         uint32 size       : 8;   // Block size or texel size in bytes
//         uint32 compressed : 1;   // Validation flag
//         uint32            : 7;   // Padding
//         };
//
//      static const TextureCompressedBlockInfo TextureCompressionInfo[TextureFormatsCount] = 
//         {
//         {  0,  0,  0, false },   // FormatUnsupported         
//         {  0,  0,  1, false },   // FormatR_8                 
//         {  0,  0,  1, false },   // FormatR_8_sn              
//         {  0,  0,  1, false },   // FormatR_8_u               
//         {  0,  0,  1, false },   // FormatR_8_s               
//         {  0,  0,  2, false },   // FormatR_16                
//         {  0,  0,  2, false },   // FormatR_16_sn             
//         {  0,  0,  2, false },   // FormatR_16_u              
//         {  0,  0,  2, false },   // FormatR_16_s              
//         {  0,  0,  2, false },   // FormatR_16_hf             
//         {  0,  0,  4, false },   // FormatR_32_u              
//         {  0,  0,  4, false },   // FormatR_32_s              
//         {  0,  0,  4, false },   // FormatR_32_f              
//         {  0,  0,  2, false },   // FormatRG_8                
//         {  0,  0,  2, false },   // FormatRG_8_sn             
//         {  0,  0,  2, false },   // FormatRG_8_u              
//         {  0,  0,  2, false },   // FormatRG_8_s              
//         {  0,  0,  4, false },   // FormatRG_16               
//         {  0,  0,  4, false },   // FormatRG_16_sn            
//         {  0,  0,  4, false },   // FormatRG_16_u             
//         {  0,  0,  4, false },   // FormatRG_16_s             
//         {  0,  0,  4, false },   // FormatRG_16_hf            
//         {  0,  0,  8, false },   // FormatRG_32_u             
//         {  0,  0,  8, false },   // FormatRG_32_s             
//         {  0,  0,  8, false },   // FormatRG_32_f             
//         {  0,  0,  1, false },   // FormatBGR_2_3_3       ---- remove ???    
//         {  0,  0,  1, false },   // FormatRGB_3_3_2       ---- remove ???  
//         {  0,  0,  0, false },   // FormatRGB_4           ---- remove ???  size ????
//         {  0,  0,  0, false },   // FormatRGB_5           ---- remove ???  size ????
//         {  0,  0,  2, false },   // FormatBGR_5_6_5           
//         {  0,  0,  2, false },   // FormatRGB_5_6_5           
//         {  0,  0,  3, false },   // FormatBGR_8               
//         {  0,  0,  3, false },   // FormatRGB_8               
//         {  0,  0,  3, false },   // FormatRGB_8_sn            
//         {  0,  0,  3, false },   // FormatRGB_8_u             
//         {  0,  0,  3, false },   // FormatRGB_8_s             
//         {  0,  0,  3, false },   // FormatRGB_8_sRGB          
//         {  0,  0,  4, false },   // FormatRGB_10           ---- X2 at the end ??? size is 4 ???           
//         {  0,  0,  4, false },   // FormatBGR_10_11_11_f      
//         {  0,  0,  4, false },   // FormatEBGR_5_9_9_9_f      
//         {  0,  0,  0, false },   // FormatRGB_12           ---- remove ???   size ???    
//         {  0,  0,  6, false },   // FormatBGR_16              
//         {  0,  0,  6, false },   // FormatRGB_16              
//         {  0,  0,  6, false },   // FormatRGB_16_sn           
//         {  0,  0,  6, false },   // FormatRGB_16_u            
//         {  0,  0,  6, false },   // FormatRGB_16_s            
//         {  0,  0,  6, false },   // FormatRGB_16_hf           
//         {  0,  0,  6, false },   // FormatRGB_16_f            
//         {  0,  0, 12, false },   // FormatRGB_32_u            
//         {  0,  0, 12, false },   // FormatRGB_32_s            
//         {  0,  0, 12, false },   // FormatRGB_32_f            
//         {  0,  0,  1, false },   // FormatRGBA_2        ---- remove ??? size ???     
//         {  0,  0,  2, false },   // FormatABGR_4        ---- remove ??? size ???      
//         {  0,  0,  2, false },   // FormatARGB_4        ---- remove ??? size ???      
//         {  0,  0,  2, false },   // FormatBGRA_4        ---- remove ??? size ???      
//         {  0,  0,  2, false },   // FormatRGBA_4        ---- remove ??? size ???      
//         {  0,  0,  2, false },   // FormatABGR_1_5_5_5        
//         {  0,  0,  2, false },   // FormatARGB_1_5_5_5        
//         {  0,  0,  2, false },   // FormatBGRA_5_5_5_1        
//         {  0,  0,  2, false },   // FormatRGBA_5_5_5_1        
//         {  0,  0,  4, false },   // FormatABGR_8              
//         {  0,  0,  4, false },   // FormatARGB_8              
//         {  0,  0,  4, false },   // FormatBGRA_8              
//         {  0,  0,  4, false },   // FormatRGBA_8              
//         {  0,  0,  4, false },   // FormatRGBA_8_sRGB         
//         {  0,  0,  4, false },   // FormatRGBA_8_sn           
//         {  0,  0,  4, false },   // FormatRGBA_8_u            
//         {  0,  0,  4, false },   // FormatRGBA_8_s            
//         {  0,  0,  4, false },   // FormatRGBA_10_10_10_2     
//         {  0,  0,  4, false },   // FormatRGBA_10_10_10_2_u   
//         {  0,  0,  8, false },   // FormatRGBA_16             
//         {  0,  0,  8, false },   // FormatRGBA_16_sn          
//         {  0,  0,  8, false },   // FormatRGBA_16_u           
//         {  0,  0,  8, false },   // FormatRGBA_16_s           
//         {  0,  0,  8, false },   // FormatRGBA_16_hf          
//         {  0,  0, 16, false },   // FormatRGBA_32_u           
//         {  0,  0, 16, false },   // FormatRGBA_32_s           
//         {  0,  0, 16, false },   // FormatRGBA_32_f           
//         {  0,  0,  2, false },   // FormatD_16                
//         {  0,  0,  3, false },   // FormatD_24          ---- size 3 or 4 bytes ?????           
//         {  0,  0,  4, false },   // FormatD_32                
//         {  0,  0,  4, false },   // FormatD_32_f              
//         {  0,  0,  1, false },   // FormatS_8                 
//         {  0,  0,  4, false },   // FormatSD_8_24             
//         {  0,  0,  8, false },   // FormatSD_8_32_f     ---- size 5 or 8 bytes ???? 24bit's of padding to 8 bytes ???      
//         {  4,  4,  8, true  },   // FormatBC1_RGB                
//         {  4,  4,  8, true  },   // FormatBC1_RGB_sRGB        
//         {  4,  4,  8, true  },   // FormatBC1_RGBA            
//         {  4,  4,  8, true  },   // FormatBC1_RGBA_sRGB       
//         {  4,  4, 16, true  },   // FormatBC2_RGBA_pRGB       
//         {  4,  4, 16, true  },   // FormatBC2_RGBA            
//         {  4,  4, 16, true  },   // FormatBC2_RGBA_sRGB       
//         {  4,  4, 16, true  },   // FormatBC3_RGBA_pRGB       
//         {  4,  4, 16, true  },   // FormatBC3_RGBA            
//         {  4,  4, 16, true  },   // FormatBC3_RGBA_sRGB       
//         {  4,  4,  8, true  },   // FormatBC4_R               
//         {  4,  4,  8, true  },   // FormatBC4_R_sn            
//         {  4,  4, 16, true  },   // FormatBC5_RG              
//         {  4,  4, 16, true  },   // FormatBC5_RG_sn           
//         {  4,  4, 16, true  },   // FormatBC6H_RGB_f          
//         {  4,  4, 16, true  },   // FormatBC6H_RGB_uf         
//         {  4,  4, 16, true  },   // FormatBC7_RGBA            
//         {  4,  4, 16, true  },   // FormatBC7_RGBA_sRGB       
//         {  4,  4,  8, true  },   // FormatETC2_R_11           
//         {  4,  4,  8, true  },   // FormatETC2_R_11_sn        
//         {  4,  4, 16, true  },   // FormatETC2_RG_11          
//         {  4,  4, 16, true  },   // FormatETC2_RG_11_sn       
//         {  4,  4,  8, true  },   // FormatETC2_RGB_8          
//         {  4,  4,  8, true  },   // FormatETC2_RGB_8_sRGB     
//         {  4,  4, 16, true  },   // FormatETC2_RGBA_8         
//         {  4,  4, 16, true  },   // FormatETC2_RGBA_8_sRGB    
//         {  4,  4,  8, true  },   // FormatETC2_RGB8_A1        
//         {  4,  4,  8, true  },   // FormatETC2_RGB8_A1_sRGB   
//         {  4,  4,  8, true  },   // FormatPVRTC_RGB_2         
//         {  4,  4,  8, true  },   // FormatPVRTC_RGB_2_sRGB    
//         {  4,  4,  8, true  },   // FormatPVRTC_RGB_4         
//         {  4,  4,  8, true  },   // FormatPVRTC_RGB_4_sRGB    
//         {  4,  4,  8, true  },   // FormatPVRTC_RGBA_2        
//         {  4,  4,  8, true  },   // FormatPVRTC_RGBA_2_sRGB   
//         {  4,  4,  8, true  },   // FormatPVRTC_RGBA_4        
//         {  4,  4,  8, true  },   // FormatPVRTC_RGBA_4_sRGB   
//         {  4,  4, 16, true  },   // FormatASTC_4x4            
//         {  5,  4, 16, true  },   // FormatASTC_5x4            
//         {  5,  5, 16, true  },   // FormatASTC_5x5            
//         {  6,  5, 16, true  },   // FormatASTC_6x5            
//         {  6,  6, 16, true  },   // FormatASTC_6x6            
//         {  8,  5, 16, true  },   // FormatASTC_8x5            
//         {  8,  6, 16, true  },   // FormatASTC_8x6            
//         {  8,  8, 16, true  },   // FormatASTC_8x8            
//         { 10,  5, 16, true  },   // FormatASTC_10x5           
//         { 10,  6, 16, true  },   // FormatASTC_10x6           
//         { 10,  8, 16, true  },   // FormatASTC_10x8           
//         { 10, 10, 16, true  },   // FormatASTC_10x10          
//         { 12, 10, 16, true  },   // FormatASTC_12x10          
//         { 12, 12, 16, true  },   // FormatASTC_12x12          
//         {  4,  4, 16, true  },   // FormatASTC_4x4_sRGB       
//         {  5,  4, 16, true  },   // FormatASTC_5x4_sRGB       
//         {  5,  5, 16, true  },   // FormatASTC_5x5_sRGB       
//         {  6,  5, 16, true  },   // FormatASTC_6x5_sRGB       
//         {  6,  6, 16, true  },   // FormatASTC_6x6_sRGB       
//         {  8,  5, 16, true  },   // FormatASTC_8x5_sRGB       
//         {  8,  6, 16, true  },   // FormatASTC_8x6_sRGB       
//         {  8,  8, 16, true  },   // FormatASTC_8x8_sRGB       
//         { 10,  5, 16, true  },   // FormatASTC_10x5_sRGB      
//         { 10,  6, 16, true  },   // FormatASTC_10x6_sRGB      
//         { 10,  8, 16, true  },   // FormatASTC_10x8_sRGB      
//         { 10, 10, 16, true  },   // FormatASTC_10x10_sRGB     
//         { 12, 10, 16, true  },   // FormatASTC_12x10_sRGB     
//         { 12, 12, 16, true  }    // FormatASTC_12x12_sRGB
//         };
//
//
//      
//      uint32 TextureSize(const TextureDescriptor* texture, const uint8 mipmap)
//      {
//      assert(texture);
//
//      TextureCompressedBlockInfo block = TextureCompressionInfo[texture->format];
//      uint32 mipWidth  = max(1, texture->width  >> mipmap);
//      uint32 mipHeight = max(1, texture->height >> mipmap);
//
//      if (block.compressed)
//         return ((mipWidth  + (block.width - 1)) / block.width) * 
//                ((mipHeight + (block.height - 1)) / block.height) *
//                block.size;
//
//      return mipWidth * mipHeight * block.size;
//      }
//
//      void TextureRead(const TextureDescriptor* texture, uint8* buffer, const uint8 mipmap)
//      {
//      assert(texture);
//
//      // Extract information needed for texture update in OpenGL
//      uint16 texType    = gl::TextureType[texture->type].type;
//      bool   compressed = gl::TextureFormat[texture->format].compressed;
//      uint16 srcFormat  = gl::TextureFormat[texture->format].srcFormat;
//      uint16 srcType    = gl::TextureFormat[texture->format].srcType; 
//
//      // Read texture content to RAM
//      Profile( glBindTexture(texType, texture->id) )
//      Profile( glGetTexImage(texType, mipmap, srcFormat, srcType, buffer) )
//      }
//
//      bool TextureDestroy(TextureDescriptor* const texture)
//      {    
//      // Frees texture in driver
//      Profile( glDeleteTextures(1, &texture->id) )
//      texture->id = 0;
//      
//      // Fill program with null pointer
//      GpuContext.textures.free(texture);
//      return true; 
//      }
//
//      #ifdef EN_OPENGL_DESKTOP
//      bool SamplerUnitUpdate(SamplerUnitDescriptor* sampler)
//      {
//      assert(sampler);
//     
//      // Check if update is required   TODO: Fix-it properly implementing array of sampler states in GPU and comparing to required state by program
//      //if (!sampler->dirty.value)
//      //   return true;
//     
//      // Update sampler state in driver
//      TextureDescriptor* textureDescriptor = *(TextureDescriptor**)&sampler->texture;
//      uint16 type = gl::TextureType[textureDescriptor->type].type;
//      if (textureDescriptor->type != Texture2DRectangle)
//         {
//         //if (sampler->dirty.wrapS)
//            Profile( glTexParameteri(type, GL_TEXTURE_WRAP_S, gl::TextureWraping[sampler->wrapS].wraping) );           
//         //if (sampler->dirty.wrapT)                                                                         
//            Profile( glTexParameteri(type, GL_TEXTURE_WRAP_T, gl::TextureWraping[sampler->wrapT].wraping) );           
//         //if (sampler->dirty.wrapR)                                                                         
//            Profile( glTexParameteri(type, GL_TEXTURE_WRAP_R, gl::TextureWraping[sampler->wrapR].wraping) );            
//         }     
//     // if (sampler->dirty.magFilter)
//         Profile( glTexParameteri(type, GL_TEXTURE_MAG_FILTER, gl::TextureFiltering[sampler->magFilter].filtering) );  
//     // if (sampler->dirty.magFilter)
//         Profile( glTexParameteri(type, GL_TEXTURE_MIN_FILTER, gl::TextureFiltering[sampler->minFilter].filtering) );  
//     
//      // Sampler state updated
//    //  sampler->dirty.value = 0;
//      return true;
//      }
//      #endif
//
//      bool SamplerDestroy(SamplerDescriptor* const sampler)
//      {
//      // Samplers are destroyed together with program.
//      return true;
//      }
//
//      }
//
//      namespace es20
//      {
//      #ifdef EN_OPENGL_MOBILE
//
//      // Creates texture 
//      Texture TextureCreate(const TextureState& textureState)
//      {
//      // Check if texture type and format are supported in current Context
//      if ( !gl::TextureTypeSupported[textureState.type] ||
//           !gl::TextureFormatSupported[textureState.format] )
//         return Texture(NULL);
//
//      // Try to add new texture descriptor 
//      TextureDescriptor* texture = GpuContext.textures.allocate();
//      if (!texture)
//         return Texture(NULL);
//
//      // Fill texture descriptor with data
//      memcpy(&texture->type, &textureState, sizeof(TextureState));
//      texture->mipmaps = 0;
//      texture->surface = NULL;
//      texture->id      = 0;
//
//      // Extract information needed for texture creation in OpenGL
//      uint16 texType    = gl::TextureType[textureState.type].type;
//      uint16 dstFormat  = gl::TextureFormat[textureState.format].dstFormat;
//      uint16 srcFormat  = gl::TextureFormat[textureState.format].srcFormat;
//      uint16 srcType    = gl::TextureFormat[textureState.format].srcType; 
//
//      // Generate empty texture in driver
//      Profile( glGenTextures(1, (GLuint*)&texture->id) );
//      Profile( glBindTexture(texType, texture->id) );
//      Profile( glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT) );
//      Profile( glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT) );
//      Profile( glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
//      Profile( glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
//      
//      // Reserve memory for texture completness (OpenGL ES 2.0+)
//      if (textureState.type == Texture2D)
//         {
//         uint16 width  = textureState.width;
//         uint16 height = textureState.height;
//         for (uint8 i=0; ; ++i)
//             {
//             Profile( glTexImage2D(GL_TEXTURE_2D, i, dstFormat, width, height, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//      
//             if ( (width  == 1) && 
//                  (height == 1) )
//                break;
//             if (width  > 1) width  = width  >> 1;
//             if (height > 1) height = height >> 1;
//             }
//         }
//      if (textureState.type == TextureCubeMap)
//         {
//         uint16 width = textureState.width;
//         for (uint8 i=0; width != 0; ++i)
//             {
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             Profile( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, dstFormat, width, width, 0, srcFormat, srcType, NULL) );
//             texture->mipmaps++;
//             width = width >> 1;
//             }
//         }
//
//      // Return texture interface
//      return Texture(texture);
//      }
//
//      bool TextureUnmap(TextureDescriptor* texture)
//      {
//      assert(texture);
//      
//      // Check if texture is mapped. There is no need
//      // to check if texture type is supported, because
//      // it was checked already when it was mapped.
//      if (!texture->surface)
//         return false;
//      if (!texture->surface->pointer)
//         return false;
//      
//      // Extract information needed for texture update in OpenGL
//      SurfaceDescriptor* surface = texture->surface;
//      uint16 texType    = gl::TextureType[texture->type].type;
//      bool   compressed = gl::TextureFormat[texture->format].compressed;
//      uint16 srcFormat  = gl::TextureFormat[texture->format].srcFormat;
//      uint16 srcType    = gl::TextureFormat[texture->format].srcType; 
//      
//      // Send texture to gpu using traditional methods
//      Profile( glBindTexture(texType, texture->id) )
//      
//      if (texture->type == Texture2D)
//         {
//         if (compressed)
//            Profile( glCompressedTexSubImage2D(GL_TEXTURE_2D, surface->mipmap, 0, 0, texture->width, texture->height, srcFormat, surface->size, surface->pointer) )
//         else
//            Profile( glTexSubImage2D(GL_TEXTURE_2D, surface->mipmap, 0, 0, texture->width, texture->height, srcFormat, srcType, surface->pointer) )
//         }
//      
//      if (texture->type == TextureCubeMap)
//         {
//         uint16 texFace = gl::TextureFace[surface->face];
//         if (compressed)
//            Profile( glCompressedTexSubImage2D(texFace, surface->mipmap, 0, 0, texture->width, texture->width, srcFormat, surface->size, surface->pointer) )
//         else    
//            Profile( glTexSubImage2D(texFace, surface->mipmap, 0, 0, texture->width, texture->width, srcFormat, srcType, surface->pointer) )
//         }
//      // Other texture types are not supported in OpenGL ES 2.0.
//      // Future versions of OpenGL ES uses PBO implementation for
//      // support of textures update.
//      
//      // Free surface descriptor and finish
//      delete [] static_cast<uint8*>(texture->surface->pointer);
//      delete texture->surface;
//      texture->surface = NULL;
//      return true;
//      }
//
//      bool SamplerUnitUpdate(SamplerUnitDescriptor* sampler)
//      {
//      assert(sampler);
//     
//      // Check if update is required
//      if (!sampler->dirty.value)
//         return true;
//     
//      // Update sampler state in driver
//      TextureDescriptor* textureDescriptor = *(TextureDescriptor**)&sampler->texture;
//      uint16 type = gl::TextureType[textureDescriptor->type].type;
//      if (sampler->dirty.wrapS)
//         Profile( glTexParameteri(type, GL_TEXTURE_WRAP_S, gl::TextureWraping[sampler->wrapS].wraping) )
//      if (sampler->dirty.wrapT)
//         Profile( glTexParameteri(type, GL_TEXTURE_WRAP_T, gl::TextureWraping[sampler->wrapT].wraping) )       
//      if (sampler->dirty.magFilter)
//         Profile( glTexParameteri(type, GL_TEXTURE_MAG_FILTER, gl::TextureFiltering[sampler->magFilter].filtering) )
//      if (sampler->dirty.magFilter)
//         Profile( glTexParameteri(type, GL_TEXTURE_MIN_FILTER, gl::TextureFiltering[sampler->minFilter].filtering) )
//     
//      // Sampler state updated
//      sampler->dirty.value = 0;
//      return true;
//      }
//
//      #endif
//      }
//
//   }
//}
//
//template<> bool (*ProxyInterface<en::gpu::TextureDescriptor>::destroy)(en::gpu::TextureDescriptor* const) = en::gpu::gl20::TextureDestroy;
//template<> bool (*ProxyInterface<en::gpu::SamplerDescriptor>::destroy)(en::gpu::SamplerDescriptor* const) = en::gpu::gl20::SamplerDestroy;
//
