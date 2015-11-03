/*

 Ngine v5.0
 
 Module      : Buffer.
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

namespace en
{
   namespace gpu
   {

   ColumnInfo::ColumnInfo() :
      type(None),
      name()
   {
   }

   ColumnInfo::ColumnInfo(const ColumnType type) :
      type(type),
      name()
   {
   }

   ColumnInfo::ColumnInfo(const ColumnType type, const string name ) :
      type(type),
      name(name)
   {
   }

   BufferSettings::BufferSettings(
      const BufferType type,
      const uint32 vertices,
      const ColumnType col0,
      const ColumnType col1,
      const ColumnType col2,
      const ColumnType col3,
      const ColumnType col4,
      const ColumnType col5,
      const ColumnType col6,
      const ColumnType col7,
      const ColumnType col8,
      const ColumnType col9,
      const ColumnType col10,
      const ColumnType col11,
      const ColumnType col12,
      const ColumnType col13,
      const ColumnType col14,
      const ColumnType col15 ) :
      type(type),
      vertices(vertices)
   {
   column[0].type  = col0;
   column[1].type  = col1;
   column[2].type  = col2;
   column[3].type  = col3;
   column[4].type  = col4;
   column[5].type  = col5;
   column[6].type  = col6;
   column[7].type  = col7;
   column[8].type  = col8;
   column[9].type  = col9;
   column[10].type = col10;
   column[11].type = col11;
   column[12].type = col12;
   column[13].type = col13;
   column[14].type = col14;
   column[15].type = col15;
   }
       
   BufferSettings::BufferSettings(
      const BufferType type,
      const uint32 vertices, 
      const ColumnInfo col0,
      const ColumnInfo col1,
      const ColumnInfo col2,
      const ColumnInfo col3,
      const ColumnInfo col4,
      const ColumnInfo col5,
      const ColumnInfo col6,
      const ColumnInfo col7,
      const ColumnInfo col8,
      const ColumnInfo col9,
      const ColumnInfo col10,
      const ColumnInfo col11,
      const ColumnInfo col12,
      const ColumnInfo col13,
      const ColumnInfo col14,
      const ColumnInfo col15 ) :
      type(type),
      vertices(vertices)
   {
   column[0]  = col0;
   column[1]  = col1;
   column[2]  = col2;
   column[3]  = col3;
   column[4]  = col4;
   column[5]  = col5;
   column[6]  = col6;
   column[7]  = col7;
   column[8]  = col8;
   column[9]  = col9;
   column[10] = col10;
   column[11] = col11;
   column[12] = col12;
   column[13] = col13;
   column[14] = col14;
   column[15] = col15;
   }

   Buffer::Buffer() :
       ProxyInterface<class BufferDescriptor>(NULL)
   {
   }

   Buffer::Buffer(class BufferDescriptor* src) :
       ProxyInterface<class BufferDescriptor>(src)
   {
   }

   //Buffer::~Buffer()
   //{
   //}

   void* Buffer::map(void)
   {
   return pointer == NULL ? NULL : gl20::BufferMap(pointer);
   }
   
   bool Buffer::unmap(void)
   {
   return pointer == NULL ? false : gl20::BufferUnmap(pointer);
   }

   uint32 Buffer::columns(void)
   {
   return pointer == NULL ? 0 : pointer->columns;
   }

   // Creates buffer with optional passing of data
   Buffer Interface::Buffer::create(const BufferSettings& bufferSettings, void* data)
   {
   // TODO: Dynamic linking in this point
   return gl20::BufferCreate(bufferSettings, data);
   }

   BufferColumnDescriptor::BufferColumnDescriptor() :
      type(None),
      name(NULL)
   {
   }

   BufferColumnDescriptor::~BufferColumnDescriptor()
   {
   delete [] name;
   }

   DataBlockDescriptor::DataBlockDescriptor() :
      size(0),
      pointer(NULL)
   {
   }

   DataBlockDescriptor::~DataBlockDescriptor()
   {
   // Pointer needs to be released externally
   // depending on supported OpenGL ES version.
   }

   BufferDescriptor::BufferDescriptor() :
      elements(0),
      columns(0),
      rowSize(0),
      size(0),
      block(NULL),
      id(0),
      vao(0),
      named(false)
   {
   }












/*

   // INTERFACE

   class Buffer : public SafeObject
         {
         public:
         virtual BufferType type(void) = 0;
         virtual void       update(void* src, uint32 size = 0, uint32 offset = 0) = 0; 
         virtual void*      map(DataAccess access) = 0;
         virtual void       unmap(void) = 0;
         virtual void       invalidate(void) = 0;

         virtual ~Buffer();         // Polymorphic deletes require a virtual base destructor     
         };

   // IMPLEMENTATION
   
   const uint16 TranslateBufferUsage[9] = 
      {
      GL_STATIC_READ,   // Static    Read
      GL_STATIC_DRAW,   //           Write
      GL_STATIC_COPY,   //           ReadWrite
      GL_DYNAMIC_READ,  // Dynamic   Read
      GL_DYNAMIC_DRAW,  //           Write
      GL_DYNAMIC_COPY,  //           ReadWrite
      GL_STREAM_READ,   // Streaming Read
      GL_STREAM_DRAW,   //           Write
      GL_STREAM_COPY    //           ReadWrite
      };

   const uint16 TranslateDataAccess[DataAccessTypes] = 
      {
      GL_READ_ONLY   ,
      GL_WRITE_ONLY  ,
      GL_READ_WRITE  
      };

   VertexBufferSettings::VertexBufferSettings() :
      transfer(Static),
      access(Write),
      vertices(0)
   {
   }

   VertexBufferSettings::VertexBufferSettings(
      const uint32 vertices,  
      const ColumnInfo col0,
      const ColumnInfo col1,
      const ColumnInfo col2,
      const ColumnInfo col3,
      const ColumnInfo col4,
      const ColumnInfo col5,
      const ColumnInfo col6,
      const ColumnInfo col7,
      const ColumnInfo col8,
      const ColumnInfo col9,
      const ColumnInfo col10,
      const ColumnInfo col11,
      const ColumnInfo col12,
      const ColumnInfo col13,
      const ColumnInfo col14,
      const ColumnInfo col15
      ) :
      transfer(Static),
      access(Write),
      vertices(vertices)
   {
   column[0]  = col0;
   column[1]  = col1;
   column[2]  = col2;
   column[3]  = col3;
   column[4]  = col4;
   column[5]  = col5;
   column[6]  = col6;
   column[7]  = col7;
   column[8]  = col8;
   column[9]  = col9;
   column[10] = col10;
   column[11] = col11;
   column[12] = col12;
   column[13] = col13;
   column[14] = col14;
   column[15] = col15;
   }

   VertexBufferSettings::VertexBufferSettings(
      const DataTransfer transfer, 
      const DataAccess access,
      const uint32 vertices,  
      const ColumnInfo col0,
      const ColumnInfo col1,
      const ColumnInfo col2,
      const ColumnInfo col3,
      const ColumnInfo col4,
      const ColumnInfo col5,
      const ColumnInfo col6,
      const ColumnInfo col7,
      const ColumnInfo col8,
      const ColumnInfo col9,
      const ColumnInfo col10,
      const ColumnInfo col11,
      const ColumnInfo col12,
      const ColumnInfo col13,
      const ColumnInfo col14,
      const ColumnInfo col15
      ) :
      transfer(transfer),
      access(access),
      vertices(vertices)
   {
   column[0]  = col0;
   column[1]  = col1;
   column[2]  = col2;
   column[3]  = col3;
   column[4]  = col4;
   column[5]  = col5;
   column[6]  = col6;
   column[7]  = col7;
   column[8]  = col8;
   column[9]  = col9;
   column[10] = col10;
   column[11] = col11;
   column[12] = col12;
   column[13] = col13;
   column[14] = col14;
   column[15] = col15;
   }

   IndexBufferSettings::IndexBufferSettings() :
      type(UInt),
      indices(0)
   {
   }

   IndexBufferSettings::IndexBufferSettings(const uint32 indices, const ColumnType type) :
      type(type),
      indices(indices)
   {
   }

   UniformBufferSettings::UniformBufferSettings(const string name) :
      name(name)
   {
   }


   class BufferGL15 : public en::gpu::Buffer
         {
         public:
         uint32     capacity;          // [4]  Size in bytes
         uint32     id;                // [8]  OpenGL handle
         uint32     glType    : 16;    // [12] OpenGL type
         uint32     glUsage   : 16;    // [14] OpenGL usage

         BufferType apiType;           // [16] API type
         uint32     elements;          // [20] Rows/elements in buffer 
         uint32     rowSize;           // [24] Size in bytes of one row/element
         uint32     columns   : 8;     // [28] Active columns
         uint32     allocated : 1;     //      Buffer is valid
         uint32     named     : 1;     //      Buffer has named columns
         uint32               : 22;    //
         ColumnType apiColumnType[16]; // Column Type
         char*      columnName[16];    // Column Name
         uint32     columnOffset[16];  // Column Offset in row

         BufferGL15();
        ~BufferGL15();

         BufferType type(void);
         void  update(void* src, uint32 size = 0, uint32 offset = 0);
         void* map(DataAccess access);
         void  unmap(void);
         void  invalidate(void);
         };

   BufferGL15::BufferGL15() :
      capacity(0),
      id(0),
      glType(GL_ARRAY_BUFFER),
      glUsage(GL_STATIC_DRAW),
      apiType(VertexBuffer),
      elements(0),
      rowSize(0),
      columns(0),
      allocated(false),
      named(false)
   {
   memset(&apiColumnType[0], 0, 64);
   memset(&columnName[0], 0, 16 * sizeof(char*));
   memset(&columnOffset[0], 0, 64);
   }
  
   BufferGL15::~BufferGL15()
   {
   assert( GpuContext.screen.created );
   Profile( glBindBuffer(glType, id) );
   Profile( glDeleteBuffers(1, &id) );
   }

   Ptr<Buffer> create(const VertexBufferSettings& settings, const void* data)
   {
   assert( GpuContext.screen.created );
   assert( GpuContext.screen.support(OpenGL_1_5) );
   assert( data );
   assert( settings.vertices );
   assert( settings.column[0].type != None );

   // Check if columns are grouped together
   uint8 columnsCount = 0;
   for(; columnsCount<16; ++columnsCount)
      if (settings.column[columnsCount].type == None)
         for(uint8 j=columnsCount+1; j<16; ++j)
            if (settings.column[j].type != None)
               {
               DebugLog( "ERROR: Vertex Buffer cannot have gaps between columns!" << endl );
               return Ptr<Buffer>(NULL);
               }

   // Check if column types are supported
   for(uint8 i=0; i<columnsCount; ++i)
      if (!gl::BufferColumnSupported[settings.column[i].type])
         {
         DebugLog( "ERROR: Buffer column of type: \"" << gl::BufferColumnName[settings.column[i].type] << "\" is not supported in this context!" << endl );
         return Ptr<Buffer>(NULL);
         }
 
   BufferGL15* buffer = new BufferGL15();
   if (!buffer)
      return Ptr<Buffer>(NULL);

   buffer->capacity  = size;
   buffer->glType    = GL_ARRAY_BUFFER;
   buffer->glUsage   = TranslateBufferUsage[ (static_cast<uint32>(settings.transfer) * 3) + static_cast<uint32>(settings.access) ];
   buffer->apiType   = VertexBuffer;
   buffer->elements  = settings.elements;
   buffer->columns   = columnsCount;
   buffer->allocated = true;
 
   // Fill buffer descriptor with data
   for(uint8 i=0; i<columnsCount; ++i)
      {
      buffer->apiColumnType[i] = settings.column[i].type;
      if (!settings.column[i].name.empty())
         {
         // Create local copy of column name
         buffer->column[i].name = new char[settings.column[i].name.size() + 1];
         memcpy(buffer->column[i].name, settings.column[i].name.c_str(), settings.column[i].name.size());
         buffer->column[i].name[ settings.column[i].name.size() ] = 0;
         buffer->named = true;
         }    
      buffer->columnOffset[i] = buffer->rowSize;  
      buffer->rowSize        += gl::BufferColumn[buffer->column[i].type].size;
      } 

   Profile( glGenBuffers(1, &id) );
   Profile( glBindBuffer(buffer->glType, id) );
   Profile( glBufferData(buffer->glType, size, data, buffer->glUsage) );

   return Ptr<Buffer>(static_cast<gpu::Buffer*>(buffer));
   };



         if (bufferSettings.column[1].type != None)
            return Buffer(NULL);
         if (bufferSettings.column[0].type != UByte  &&
             bufferSettings.column[0].type != UShort &&
             bufferSettings.column[0].type != UInt)
            {
#ifdef EN_DEBUG
            Log << "ERROR: Buffer column of type: \"";
            Log << gl::BufferColumnName[bufferSettings.column[0].type];
            Log << "\" cannot be used in IndexBuffer!" << endl;
            return Buffer(NULL);
#endif
            }
         columns = 1;

    

   BufferType BufferGL15::type(void)
   {
   return apiType;
   }

   void BufferGL15::update(void* src, uint32 size, uint32 offset)
   {
   assert( GpuContext.screen.created );
   assert( src );

   Profile( glBindBuffer(glType, id) );
   if (allocated)
      {
      assert( (offset + size) <= capacity );
      Profile( glBufferSubData(glType, offset, size ? size : capacity, src) );
      }
   else
      {
      Profile( glBufferData(glType, size , src, glUsage) );
      capacity = size;
      allocated = true;
      }
   }

   void BufferGL15::update(void* src, uint32 size, DataTransfer transfer, DataAccess access)
   {
   assert( GpuContext.screen.created );
   assert( size );
   assert( src );

   Profile( glBindBuffer(glType, id) );
   if (allocated)
      Profile( glBufferData(glType, capacity, NULL, glUsage) );



   Profile( glBufferData(glType, size , src, glUsage) );
   capacity = size;
   allocated = true;
   }

   void* BufferGL15::map(DataAccess access)
   {
   assert( GpuContext.screen.created );
// if not mapped
   Profile( glBindBuffer(glType, id) );
   return glMapBuffer(glType, TranslateDataAccess[access]);
   }

   void BufferGL15::unmap(void)
   {
   assert( GpuContext.screen.created );
   Profile( glBindBuffer(glType, id) );
   Profile( glUnmapBuffer(glType) );
   }
   
   void BufferGL15::invalidate(void)
   {
   assert( GpuContext.screen.created );
   Profile( glBindBuffer(glType, id) );
   Profile( glBufferData(glType, capacity, NULL, glUsage) );
   allocated = false;
   }

   VertexBufferGL15::VertexBufferGL15()
   {
   }

   VertexBufferGL15::~VertexBufferGL15()
   {
   }

   uint32 VertexBufferGL15::columns(void)
   {
   return columnsCount;
   }

//*/






      namespace gl
      {
      bool BufferColumnSupported[ColumnTypesCount];

      #ifdef EN_OPENGL_DESKTOP
  
      const BufferTypeTranslation BufferType[BufferTypesCount] = 
         { 
         { OpenGL_1_5, GL_ARRAY_BUFFER                 },   // Vertex                  
         { OpenGL_1_5, GL_ELEMENT_ARRAY_BUFFER         },   // Index  
         { OpenGL_3_1, GL_UNIFORM_BUFFER               },   // Uniform
         { OpenGL_3_1, GL_TRANSFORM_FEEDBACK_BUFFER    },   // Feedback
         { OpenGL_4_0, GL_DRAW_INDIRECT_BUFFER         },   // Draw
         { OpenGL_4_2, GL_ATOMIC_COUNTER_BUFFER        },   // Counter
         { OpenGL_4_3, GL_DISPATCH_INDIRECT_BUFFER     },   // Dispatch
         { OpenGL_4_3, GL_SHADER_STORAGE_BUFFER        },   // Storage
         { OpenGL_4_4, GL_QUERY_BUFFER                 },   // Query
         };
             
      const BufferColumnTranslation BufferColumn[ColumnTypesCount] = 
         { 
         { 0,          0,  0,       0,        0,     0,     0                              },   // None  
         { OpenGL_2_0, 1,  1,       GL_FALSE, false, false, GL_BYTE                        },   // Float_8   
         { OpenGL_2_0, 2,  2,       GL_FALSE, false, false, GL_BYTE                        },   // Float2_8  
         { OpenGL_2_0, 3,  3,       GL_FALSE, false, false, GL_BYTE                        },   // Float3_8  
         { OpenGL_2_0, 4,  4,       GL_FALSE, false, false, GL_BYTE                        },   // Float4_8       
         { OpenGL_2_0, 2,  1,       GL_FALSE, false, false, GL_SHORT                       },   // Float_16  
         { OpenGL_2_0, 4,  2,       GL_FALSE, false, false, GL_SHORT                       },   // Float2_16 
         { OpenGL_2_0, 6,  3,       GL_FALSE, false, false, GL_SHORT                       },   // Float3_16 
         { OpenGL_2_0, 8,  4,       GL_FALSE, false, false, GL_SHORT                       },   // Float4_16         
         { OpenGL_2_0, 1,  1,       GL_FALSE, false, false, GL_UNSIGNED_BYTE               },   // UFloat_8  
         { OpenGL_2_0, 2,  2,       GL_FALSE, false, false, GL_UNSIGNED_BYTE               },   // UFloat2_8 
         { OpenGL_2_0, 3,  3,       GL_FALSE, false, false, GL_UNSIGNED_BYTE               },   // UFloat3_8 
         { OpenGL_2_0, 4,  4,       GL_FALSE, false, false, GL_UNSIGNED_BYTE               },   // UFloat4_8        
         { OpenGL_2_0, 2,  1,       GL_FALSE, false, false, GL_UNSIGNED_SHORT              },   // UFloat_16 
         { OpenGL_2_0, 4,  2,       GL_FALSE, false, false, GL_UNSIGNED_SHORT              },   // UFloat2_16
         { OpenGL_2_0, 6,  3,       GL_FALSE, false, false, GL_UNSIGNED_SHORT              },   // UFloat3_16
         { OpenGL_2_0, 8,  4,       GL_FALSE, false, false, GL_UNSIGNED_SHORT              },   // UFloat4_16 
         { OpenGL_3_0, 2,  1,       GL_FALSE, false, false, GL_HALF_FLOAT                  },   // Half   
         { OpenGL_3_0, 4,  2,       GL_FALSE, false, false, GL_HALF_FLOAT                  },   // Half2  
         { OpenGL_3_0, 6,  3,       GL_FALSE, false, false, GL_HALF_FLOAT                  },   // Half3  
         { OpenGL_3_0, 8,  4,       GL_FALSE, false, false, GL_HALF_FLOAT                  },   // Half4  
         { OpenGL_2_0, 4,  1,       GL_FALSE, false, false, GL_FLOAT                       },   // Float  
         { OpenGL_2_0, 8,  2,       GL_FALSE, false, false, GL_FLOAT                       },   // Float2 
         { OpenGL_2_0, 12, 3,       GL_FALSE, false, false, GL_FLOAT                       },   // Float3 
         { OpenGL_2_0, 16, 4,       GL_FALSE, false, false, GL_FLOAT                       },   // Float4 
         { OpenGL_4_1, 8,  1,       GL_FALSE, false, true,  GL_DOUBLE                      },   // Double 
         { OpenGL_4_1, 16, 2,       GL_FALSE, false, true,  GL_DOUBLE                      },   // Double2
         { OpenGL_4_1, 24, 3,       GL_FALSE, false, true,  GL_DOUBLE                      },   // Double3
         { OpenGL_4_1, 32, 4,       GL_FALSE, false, true,  GL_DOUBLE                      },   // Double4
         { OpenGL_2_0, 1,  1,       GL_TRUE,  false, false, GL_BYTE                        },   // Float_8_SNorm
         { OpenGL_2_0, 2,  2,       GL_TRUE,  false, false, GL_BYTE                        },   // Float2_8_SNorm
         { OpenGL_2_0, 3,  3,       GL_TRUE,  false, false, GL_BYTE                        },   // Float3_8_SNorm
         { OpenGL_2_0, 4,  4,       GL_TRUE,  false, false, GL_BYTE                        },   // Float4_8_SNorm
         { OpenGL_2_0, 2,  1,       GL_TRUE,  false, false, GL_SHORT                       },   // Half_SNorm
         { OpenGL_2_0, 4,  2,       GL_TRUE,  false, false, GL_SHORT                       },   // Half2_SNorm
         { OpenGL_2_0, 6,  3,       GL_TRUE,  false, false, GL_SHORT                       },   // Half3_SNorm
         { OpenGL_2_0, 8,  4,       GL_TRUE,  false, false, GL_SHORT                       },   // Half4_SNorm
         { OpenGL_2_0, 4,  1,       GL_TRUE,  false, false, GL_INT                         },   // Float_SNorm
         { OpenGL_2_0, 8,  2,       GL_TRUE,  false, false, GL_INT                         },   // Float2_SNorm
         { OpenGL_2_0, 12, 3,       GL_TRUE,  false, false, GL_INT                         },   // Float3_SNorm
         { OpenGL_2_0, 16, 4,       GL_TRUE,  false, false, GL_INT                         },   // Float4_SNorm
         { OpenGL_2_0, 1,  1,       GL_TRUE,  false, false, GL_UNSIGNED_BYTE               },   // Float_8_Norm
         { OpenGL_2_0, 2,  2,       GL_TRUE,  false, false, GL_UNSIGNED_BYTE               },   // Float2_8_Norm
         { OpenGL_2_0, 3,  3,       GL_TRUE,  false, false, GL_UNSIGNED_BYTE               },   // Float3_8_Norm
         { OpenGL_2_0, 4,  4,       GL_TRUE,  false, false, GL_UNSIGNED_BYTE               },   // Float4_8_Norm
         { OpenGL_2_0, 2,  1,       GL_TRUE,  false, false, GL_UNSIGNED_SHORT              },   // Half_Norm
         { OpenGL_2_0, 4,  2,       GL_TRUE,  false, false, GL_UNSIGNED_SHORT              },   // Half2_Norm
         { OpenGL_2_0, 6,  3,       GL_TRUE,  false, false, GL_UNSIGNED_SHORT              },   // Half3_Norm
         { OpenGL_2_0, 8,  4,       GL_TRUE,  false, false, GL_UNSIGNED_SHORT              },   // Half4_Norm
         { OpenGL_2_0, 4,  1,       GL_TRUE,  false, false, GL_UNSIGNED_INT                },   // Float_Norm
         { OpenGL_2_0, 8,  2,       GL_TRUE,  false, false, GL_UNSIGNED_INT                },   // Float2_Norm
         { OpenGL_2_0, 12, 3,       GL_TRUE,  false, false, GL_UNSIGNED_INT                },   // Float3_Norm
         { OpenGL_2_0, 16, 4,       GL_TRUE,  false, false, GL_UNSIGNED_INT                },   // Float4_Norm
         { OpenGL_3_0, 1,  1,       GL_FALSE, true,  false, GL_BYTE                        },   // Byte   
         { OpenGL_3_0, 2,  2,       GL_FALSE, true,  false, GL_BYTE                        },   // Byte2  
         { OpenGL_3_0, 3,  3,       GL_FALSE, true,  false, GL_BYTE                        },   // Byte3  
         { OpenGL_3_0, 4,  4,       GL_FALSE, true,  false, GL_BYTE                        },   // Byte4  
         { OpenGL_3_0, 2,  1,       GL_FALSE, true,  false, GL_SHORT                       },   // Short  
         { OpenGL_3_0, 4,  2,       GL_FALSE, true,  false, GL_SHORT                       },   // Short2 
         { OpenGL_3_0, 6,  3,       GL_FALSE, true,  false, GL_SHORT                       },   // Short3 
         { OpenGL_3_0, 8,  4,       GL_FALSE, true,  false, GL_SHORT                       },   // Short4 
         { OpenGL_3_0, 4,  1,       GL_FALSE, true,  false, GL_INT                         },   // Int    
         { OpenGL_3_0, 8,  2,       GL_FALSE, true,  false, GL_INT                         },   // Int2   
         { OpenGL_3_0, 12, 3,       GL_FALSE, true,  false, GL_INT                         },   // Int3   
         { OpenGL_3_0, 16, 4,       GL_FALSE, true,  false, GL_INT                         },   // Int4   
         { OpenGL_3_0, 1,  1,       GL_FALSE, true,  false, GL_UNSIGNED_BYTE               },   // UByte  
         { OpenGL_3_0, 2,  2,       GL_FALSE, true,  false, GL_UNSIGNED_BYTE               },   // UByte2 
         { OpenGL_3_0, 3,  3,       GL_FALSE, true,  false, GL_UNSIGNED_BYTE               },   // UByte3 
         { OpenGL_3_0, 4,  4,       GL_FALSE, true,  false, GL_UNSIGNED_BYTE               },   // UByte4 
         { OpenGL_3_0, 2,  1,       GL_FALSE, true,  false, GL_UNSIGNED_SHORT              },   // UShort 
         { OpenGL_3_0, 4,  2,       GL_FALSE, true,  false, GL_UNSIGNED_SHORT              },   // UShort2
         { OpenGL_3_0, 6,  3,       GL_FALSE, true,  false, GL_UNSIGNED_SHORT              },   // UShort3
         { OpenGL_3_0, 8,  4,       GL_FALSE, true,  false, GL_UNSIGNED_SHORT              },   // UShort4
         { OpenGL_3_0, 4,  1,       GL_FALSE, true,  false, GL_UNSIGNED_INT                },   // UInt   
         { OpenGL_3_0, 8,  2,       GL_FALSE, true,  false, GL_UNSIGNED_INT                },   // UInt2  
         { OpenGL_3_0, 12, 3,       GL_FALSE, true,  false, GL_UNSIGNED_INT                },   // UInt3  
         { OpenGL_3_0, 16, 4,       GL_FALSE, true,  false, GL_UNSIGNED_INT                },   // UInt4  
         { OpenGL_3_3, 4,  GL_BGRA, GL_TRUE,  false, false, GL_INT_2_10_10_10_REV          },   // Float4_10_10_10_2_SNorm -> D3D comaptiblity format ZYXW see ARB_vertex_array_bgra
         { OpenGL_3_3, 4,  GL_BGRA, GL_TRUE,  false, false, GL_UNSIGNED_INT_2_10_10_10_REV }    // Float4_10_10_10_2_Norm  -> D3D comaptiblity format ZYXW
         };
     
      #endif
      #ifdef EN_OPENGL_MOBILE

      const BufferTypeTranslation BufferType[BufferTypesCount] = 
         { 
         { OpenGL_ES_1_1, GL_ARRAY_BUFFER                 },   // Vertex                  
         { OpenGL_ES_1_1, GL_ELEMENT_ARRAY_BUFFER         }    // Index  
         };

      const BufferColumnTranslation BufferColumn[ColumnTypesCount] = 
         { 
         { 0,                     0,  0,             0,        0,     0,     0                                    },   // None    
         { OpenGL_ES_2_0,         1,  1,             GL_FALSE, false, false, GL_BYTE                              },   // Float_8   
         { OpenGL_ES_1_0,         2,  2,             GL_FALSE, false, false, GL_BYTE                              },   // Float2_8  
         { OpenGL_ES_1_0,         3,  3,             GL_FALSE, false, false, GL_BYTE                              },   // Float3_8  
         { OpenGL_ES_1_0,         4,  4,             GL_FALSE, false, false, GL_BYTE                              },   // Float4_8        
         { OpenGL_ES_2_0,         2,  1,             GL_FALSE, false, false, GL_SHORT                             },   // Float_16  
         { OpenGL_ES_1_0,         4,  2,             GL_FALSE, false, false, GL_SHORT                             },   // Float2_16 
         { OpenGL_ES_1_0,         6,  3,             GL_FALSE, false, false, GL_SHORT                             },   // Float3_16 
         { OpenGL_ES_1_0,         8,  4,             GL_FALSE, false, false, GL_SHORT                             },   // Float4_16          
         { OpenGL_ES_2_0,         1,  1,             GL_FALSE, false, false, GL_UNSIGNED_BYTE                     },   // UFloat_8  
         { OpenGL_ES_2_0,         2,  2,             GL_FALSE, false, false, GL_UNSIGNED_BYTE                     },   // UFloat2_8 
         { OpenGL_ES_2_0,         3,  3,             GL_FALSE, false, false, GL_UNSIGNED_BYTE                     },   // UFloat3_8 
         { OpenGL_ES_2_0,         4,  4,             GL_FALSE, false, false, GL_UNSIGNED_BYTE                     },   // UFloat4_8         
         { OpenGL_ES_2_0,         2,  1,             GL_FALSE, false, false, GL_UNSIGNED_SHORT                    },   // UFloat_16 
         { OpenGL_ES_2_0,         4,  2,             GL_FALSE, false, false, GL_UNSIGNED_SHORT                    },   // UFloat2_16
         { OpenGL_ES_2_0,         6,  3,             GL_FALSE, false, false, GL_UNSIGNED_SHORT                    },   // UFloat3_16
         { OpenGL_ES_2_0,         8,  4,             GL_FALSE, false, false, GL_UNSIGNED_SHORT                    },   // UFloat4_16
         { OpenGL_ES_Unsupported, 2,  1,             GL_FALSE, false, false, 0 /*GL_HALF_FLOAT*/                  },   // Half   
         { OpenGL_ES_Unsupported, 4,  2,             GL_FALSE, false, false, 0 /*GL_HALF_FLOAT*/                  },   // Half2  
         { OpenGL_ES_Unsupported, 6,  3,             GL_FALSE, false, false, 0 /*GL_HALF_FLOAT*/                  },   // Half3  
         { OpenGL_ES_Unsupported, 8,  4,             GL_FALSE, false, false, 0 /*GL_HALF_FLOAT*/                  },   // Half4  
         { OpenGL_ES_2_0,         4,  1,             GL_FALSE, false, false, GL_FLOAT                             },   // Float  
         { OpenGL_ES_1_0,         8,  2,             GL_FALSE, false, false, GL_FLOAT                             },   // Float2 
         { OpenGL_ES_1_0,         12, 3,             GL_FALSE, false, false, GL_FLOAT                             },   // Float3 
         { OpenGL_ES_1_0,         16, 4,             GL_FALSE, false, false, GL_FLOAT                             },   // Float4 
         { OpenGL_ES_Unsupported, 8,  1,             GL_FALSE, false, true,  0 /*GL_DOUBLE*/                      },   // Double 
         { OpenGL_ES_Unsupported, 16, 2,             GL_FALSE, false, true,  0 /*GL_DOUBLE*/                      },   // Double2
         { OpenGL_ES_Unsupported, 24, 3,             GL_FALSE, false, true,  0 /*GL_DOUBLE*/                      },   // Double3
         { OpenGL_ES_Unsupported, 32, 4,             GL_FALSE, false, true,  0 /*GL_DOUBLE*/                      },   // Double4
         { OpenGL_ES_Unsupported, 1,  1,             GL_TRUE,  false, false, GL_BYTE                              },   // Float_8_SNorm
         { OpenGL_ES_Unsupported, 2,  2,             GL_TRUE,  false, false, GL_BYTE                              },   // Float2_8_SNorm
         { OpenGL_ES_Unsupported, 3,  3,             GL_TRUE,  false, false, GL_BYTE                              },   // Float3_8_SNorm
         { OpenGL_ES_Unsupported, 4,  4,             GL_TRUE,  false, false, GL_BYTE                              },   // Float4_8_SNorm
         { OpenGL_ES_Unsupported, 2,  1,             GL_TRUE,  false, false, GL_SHORT                             },   // Half_SNorm
         { OpenGL_ES_Unsupported, 4,  2,             GL_TRUE,  false, false, GL_SHORT                             },   // Half2_SNorm
         { OpenGL_ES_Unsupported, 6,  3,             GL_TRUE,  false, false, GL_SHORT                             },   // Half3_SNorm
         { OpenGL_ES_Unsupported, 8,  4,             GL_TRUE,  false, false, GL_SHORT                             },   // Half4_SNorm
         { OpenGL_ES_Unsupported, 4,  1,             GL_TRUE,  false, false, GL_INT                               },   // Float_SNorm
         { OpenGL_ES_Unsupported, 8,  2,             GL_TRUE,  false, false, GL_INT                               },   // Float2_SNorm
         { OpenGL_ES_Unsupported, 12, 3,             GL_TRUE,  false, false, GL_INT                               },   // Float3_SNorm
         { OpenGL_ES_Unsupported, 16, 4,             GL_TRUE,  false, false, GL_INT                               },   // Float4_SNorm
         { OpenGL_ES_Unsupported, 1,  1,             GL_TRUE,  false, false, GL_UNSIGNED_BYTE                     },   // Float_8_Norm
         { OpenGL_ES_Unsupported, 2,  2,             GL_TRUE,  false, false, GL_UNSIGNED_BYTE                     },   // Float2_8_Norm
         { OpenGL_ES_Unsupported, 3,  3,             GL_TRUE,  false, false, GL_UNSIGNED_BYTE                     },   // Float3_8_Norm
         { OpenGL_ES_Unsupported, 4,  4,             GL_TRUE,  false, false, GL_UNSIGNED_BYTE                     },   // Float4_8_Norm
         { OpenGL_ES_Unsupported, 2,  1,             GL_TRUE,  false, false, GL_UNSIGNED_SHORT                    },   // Half_Norm
         { OpenGL_ES_Unsupported, 4,  2,             GL_TRUE,  false, false, GL_UNSIGNED_SHORT                    },   // Half2_Norm
         { OpenGL_ES_Unsupported, 6,  3,             GL_TRUE,  false, false, GL_UNSIGNED_SHORT                    },   // Half3_Norm
         { OpenGL_ES_Unsupported, 8,  4,             GL_TRUE,  false, false, GL_UNSIGNED_SHORT                    },   // Half4_Norm
         { OpenGL_ES_Unsupported, 4,  1,             GL_TRUE,  false, false, GL_UNSIGNED_INT                      },   // Float_Norm
         { OpenGL_ES_Unsupported, 8,  2,             GL_TRUE,  false, false, GL_UNSIGNED_INT                      },   // Float2_Norm
         { OpenGL_ES_Unsupported, 12, 3,             GL_TRUE,  false, false, GL_UNSIGNED_INT                      },   // Float3_Norm
         { OpenGL_ES_Unsupported, 16, 4,             GL_TRUE,  false, false, GL_UNSIGNED_INT                      },   // Float4_Norm
         { OpenGL_ES_Unsupported, 1,  1,             GL_FALSE, true,  false, GL_BYTE                              },   // Byte   
         { OpenGL_ES_Unsupported, 2,  2,             GL_FALSE, true,  false, GL_BYTE                              },   // Byte2  
         { OpenGL_ES_Unsupported, 3,  3,             GL_FALSE, true,  false, GL_BYTE                              },   // Byte3  
         { OpenGL_ES_Unsupported, 4,  4,             GL_FALSE, true,  false, GL_BYTE                              },   // Byte4  
         { OpenGL_ES_Unsupported, 2,  1,             GL_FALSE, true,  false, GL_SHORT                             },   // Short  
         { OpenGL_ES_Unsupported, 4,  2,             GL_FALSE, true,  false, GL_SHORT                             },   // Short2 
         { OpenGL_ES_Unsupported, 6,  3,             GL_FALSE, true,  false, GL_SHORT                             },   // Short3 
         { OpenGL_ES_Unsupported, 8,  4,             GL_FALSE, true,  false, GL_SHORT                             },   // Short4 
         { OpenGL_ES_Unsupported, 4,  1,             GL_FALSE, true,  false, GL_INT                               },   // Int    
         { OpenGL_ES_Unsupported, 8,  2,             GL_FALSE, true,  false, GL_INT                               },   // Int2   
         { OpenGL_ES_Unsupported, 12, 3,             GL_FALSE, true,  false, GL_INT                               },   // Int3   
         { OpenGL_ES_Unsupported, 16, 4,             GL_FALSE, true,  false, GL_INT                               },   // Int4   
         { OpenGL_ES_Unsupported, 1,  1,             GL_FALSE, true,  false, GL_UNSIGNED_BYTE                     },   // UByte  
         { OpenGL_ES_Unsupported, 2,  2,             GL_FALSE, true,  false, GL_UNSIGNED_BYTE                     },   // UByte2 
         { OpenGL_ES_Unsupported, 3,  3,             GL_FALSE, true,  false, GL_UNSIGNED_BYTE                     },   // UByte3 
         { OpenGL_ES_Unsupported, 4,  4,             GL_FALSE, true,  false, GL_UNSIGNED_BYTE                     },   // UByte4 
         { OpenGL_ES_Unsupported, 2,  1,             GL_FALSE, true,  false, GL_UNSIGNED_SHORT                    },   // UShort 
         { OpenGL_ES_Unsupported, 4,  2,             GL_FALSE, true,  false, GL_UNSIGNED_SHORT                    },   // UShort2
         { OpenGL_ES_Unsupported, 6,  3,             GL_FALSE, true,  false, GL_UNSIGNED_SHORT                    },   // UShort3
         { OpenGL_ES_Unsupported, 8,  4,             GL_FALSE, true,  false, GL_UNSIGNED_SHORT                    },   // UShort4
         { OpenGL_ES_Unsupported, 4,  1,             GL_FALSE, true,  false, GL_UNSIGNED_INT                      },   // UInt   
         { OpenGL_ES_Unsupported, 8,  2,             GL_FALSE, true,  false, GL_UNSIGNED_INT                      },   // UInt2  
         { OpenGL_ES_Unsupported, 12, 3,             GL_FALSE, true,  false, GL_UNSIGNED_INT                      },   // UInt3  
         { OpenGL_ES_Unsupported, 16, 4,             GL_FALSE, true,  false, GL_UNSIGNED_INT                      },   // UInt4  
         { OpenGL_ES_Unsupported, 4,  0 /*GL_BGRA*/, GL_TRUE,  false, false, 0 /*GL_INT_2_10_10_10_REV*/          },   // Float4_10_10_10_2_SNorm
         { OpenGL_ES_Unsupported, 4,  0 /*GL_BGRA*/, GL_TRUE,  false, false, 0 /*GL_UNSIGNED_INT_2_10_10_10_REV*/ }    // Float4_10_10_10_2_Norm
         };

      #endif
      #ifdef EN_DEBUG

      const char* BufferColumnName[ColumnTypesCount] = 
         {
         "None",  
         "Float_8",  
         "Float2_8",  
         "Float3_8",  
         "Float4_8",       
         "Float_16",  
         "Float2_16", 
         "Float3_16", 
         "Float4_16",         
         "UFloat_8",  
         "UFloat2_8", 
         "UFloat3_8", 
         "UFloat4_8",        
         "UFloat_16", 
         "UFloat2_16",
         "UFloat3_16",
         "UFloat4_16", 
         "Half",   
         "Half2",  
         "Half3",  
         "Half4",  
         "Float",  
         "Float2", 
         "Float3", 
         "Float4", 
         "Double", 
         "Double2",
         "Double3",
         "Double4",
         "Float_8_SNorm",
         "Float2_8_SNorm",
         "Float3_8_SNorm",
         "Float4_8_SNorm",
         "Half_SNorm",
         "Half2_SNorm",
         "Half3_SNorm",
         "Half4_SNorm",
         "Float_SNorm",
         "Float2_SNorm",
         "Float3_SNorm",
         "Float4_SNorm",
         "Float_8_Norm",
         "Float2_8_Norm",
         "Float3_8_Norm",
         "Float4_8_Norm",
         "Half_Norm",
         "Half2_Norm",
         "Half3_Norm",
         "Half4_Norm",
         "Float_Norm",
         "Float2_Norm",
         "Float3_Norm",
         "Float4_Norm",
         "Byte",   
         "Byte2", 
         "Byte3", 
         "Byte4",  
         "Short", 
         "Short2", 
         "Short3", 
         "Short4", 
         "Int",    
         "Int2",   
         "Int3",   
         "Int4",   
         "UByte",  
         "UByte2", 
         "UByte3", 
         "UByte4", 
         "UShort", 
         "UShort2",
         "UShort3",
         "UShort4",
         "UInt",   
         "UInt2", 
         "UInt3",  
         "UInt4",  
         "Float4_10_10_10_2_SNorm",
         "Float4_10_10_10_2_Norm"
         };

      #endif
      }

      namespace gl20
      {



//   // Vertex assembly input column description
//   class InputColumn
//         {
//         ColumnType type;
//         InputColumn();
//         };
//
//      class Input // VertexAssemble
//            {
//            Ptr<Buffer> bind[16];    // OpenGL vbo bindings
//            uint32      divisor[16]; // OpenGL attribute divisor
//            uint8       column[16];  // OpenGL vbo column to bind and it's format to use
//            uint32      vao;         // OpenGL vao
//
//            Input(
//                  ColumnType col0  = None,
//                  ColumnType col1  = None,
//                  ColumnType col2  = None,
//                  ColumnType col3  = None,
//                  ColumnType col4  = None,
//                  ColumnType col5  = None,
//                  ColumnType col6  = None,
//                  ColumnType col7  = None,
//                  ColumnType col8  = None,
//                  ColumnType col9  = None,
//                  ColumnType col10 = None,
//                  ColumnType col11 = None,
//                  ColumnType col12 = None,
//                  ColumnType col13 = None,
//                  ColumnType col14 = None,
//                  ColumnType col15 = None
//)
//            };
//
//      Input::Input()
//      {
//      for(uint8 i=0; i<16; ++i)
//         {
//         bind[i]    = nullptr;
//         divisor[i] = 0;
//         column[i]  = 0;
//         }
//      vao = 0;
//      }
//
//      Profile( glGetProgramiv(program->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, (GLint*)&maxAttribLength) );
//      Profile( glGetProgramiv(program->id, GL_ACTIVE_ATTRIBUTES, (GLint*)&program->inputs.count) );
//      // Describe inputs
//      InputDescriptor* input;
//      for(uint32 i=0; i<program->inputs.count; ++i)
//         {
//         Profile( glGetActiveAttrib(program->id, i, nameMaxLength, (GLsizei*)&nameLength, (GLint*)&elements, (GLenum*)&type, name) );
//         name[nameLength] = 0;
//      
//         // Input parameters can be returned out of order.
//         // They can also have location index higher than
//         // their total count.   
//         input = &program->inputs.list[i];
//         input->program  = program;
//         input->elements = elements;
//         input->name     = new char[nameLength+1];
//         memcpy(input->name, name, nameLength+1);
//         Profile( input->location = glGetAttribLocation(program->id, name) );
//         input->type     = type;
//      
//         // Assing input to it's name 
//         program->inputs.names[string(name, nameLength)] = input;
//         }
//
//
//      // For each active program input, search proper buffer
//      // column by name or by location. If column cannot be
//      // found, try to use default value specified for
//      // input parameter name or return error (TODO).
//      Profile( glBindVertexArray(buffer->vao) )
//      Profile( glBindBuffer(GL_ARRAY_BUFFER, buffer->id) )
//      uint32 offset = 0;
//      for(uint8 i=0; i<program->inputs.count; ++i)
//         {
//         InputDescriptor& input = program->inputs.list[i];
//
//         // Find buffer column that match by name input parameter
//         if (buffer->named)
//            {
//            bool found = false;
//            for(uint8 j=0; j<buffer->columns; ++j)
//               if (strcmp(buffer->column[j].name, input.name) == 0)
//                  {
//                  ColumnType columnType = buffer->column[j].type;
//                  uint16 type       = gl::BufferColumn[columnType].type;
//                  //uint8  size       = gl::BufferColumn[columnType].size;
//                  uint16 channels   = gl::BufferColumn[columnType].channels;
//                  bool   normalized = gl::BufferColumn[columnType].normalized;
//                  
//                  Profile( glEnableVertexAttribArray( input.location ) );
//                  Profile( glVertexAttribPointer(input.location, channels, type, normalized, buffer->rowSize, (const GLvoid*)buffer->offset[j]) );
//   
//                  found = true;
//                  break;
//                  }
//      
//     //       if (!found)
//     //          log << "Error! Can't find column matching programs input attribute named: " << input.name << endl; 
//            }
//         // Match columns by locations
//         else
//            {
//            ColumnType columnType = buffer->column[input.location].type;
//            uint16 type       = gl::BufferColumn[columnType].type;
//            uint16 size       = gl::BufferColumn[columnType].size;
//            uint16 channels   = gl::BufferColumn[columnType].channels;
//            bool   normalized = gl::BufferColumn[columnType].normalized;
//            
//            Profile( glEnableVertexAttribArray( input.location ) );
//            Profile( glVertexAttribPointer(input.location, channels, type, normalized, buffer->rowSize, (const GLvoid*)offset) );
//            offset += size;
//            }
//         }


















      void BuffersInit(void)
      {
      // Init information about currently supported column types
      for(uint16 i=0; i<ColumnTypesCount; ++i)
         {
         gl::BufferColumnSupported[i] = false;
         if (GpuContext.screen.api.release >= gl::BufferColumn[i].opengl.release)
            gl::BufferColumnSupported[i] = true;
         }
      }

      void BuffersClear(void)
      {
      // Clear information about currently supported columns
      for(uint16 i=0; i<ColumnTypesCount; ++i)
         gl::BufferColumnSupported[i] = false;
      }

      // Checks if column type is supported in current OpenGL Context
      bool SupportColumnType(const ColumnType type)
      {
      return gl::BufferColumnSupported[type];
      }

      // Creates input buffer 
      Buffer BufferCreate(const BufferSettings& bufferSettings, void* data)
      {
      // Screen needs to be created
      assert(GpuContext.screen.created);

      // Buffer Settings need to make sense
      assert(bufferSettings.type == VertexBuffer  || 
             bufferSettings.type == IndexBuffer   ||
             bufferSettings.type == UniformBuffer ||
             bufferSettings.type == StorageBuffer);
      assert(bufferSettings.elements != 0);

      // Check if column types are supported 
      uint8 columns = 0;
      if ( bufferSettings.type == VertexBuffer )
         { 
         for(uint8 i=0; i<16; i++)
            {
            if (bufferSettings.column[i].type == None)
               break;
            if (!gl::BufferColumnSupported[bufferSettings.column[i].type])
               {
#ifdef EN_DEBUG
               Log << "ERROR: Buffer column of type: \"";
               Log << gl::BufferColumnName[bufferSettings.column[i].type];
               Log << "\" is not supported in this context!" << endl;
#endif
               return Buffer(NULL);
               }
            columns++;
            }
         }

      if ( bufferSettings.type == IndexBuffer )
         {
         if (bufferSettings.column[1].type != None)
            return Buffer(NULL);
         if (bufferSettings.column[0].type != UByte  &&
             bufferSettings.column[0].type != UShort &&
             bufferSettings.column[0].type != UInt)
            {
#ifdef EN_DEBUG
            Log << "ERROR: Buffer column of type: \"";
            Log << gl::BufferColumnName[bufferSettings.column[0].type];
            Log << "\" cannot be used in IndexBuffer!" << endl;
            return Buffer(NULL);
#endif
            }
         columns = 1;
         }
    
      // Try to add new buffer descriptor 
      BufferDescriptor* buffer = GpuContext.buffers.allocate();
      if (!buffer)
         return Buffer(NULL);
  
      // Fill buffer descriptor with data
      buffer->type     = bufferSettings.type;  
      if (buffer->type == VertexBuffer ||
          buffer->type == IndexBuffer)
         {
         buffer->rowSize = 0;
         for(uint8 i=0; i<columns; ++i)
            {
            buffer->column[i].type = bufferSettings.column[i].type;
            buffer->column[i].name = NULL;
            if (bufferSettings.column[i].name.size() > 0)
               {
               // Create local copy of column name
               buffer->column[i].name = new char[bufferSettings.column[i].name.size() + 1];
               memcpy(buffer->column[i].name, bufferSettings.column[i].name.c_str(), bufferSettings.column[i].name.size());
               buffer->column[i].name[ bufferSettings.column[i].name.size() ] = 0;
               buffer->named = true;
               }    
            buffer->offset[i] = buffer->rowSize;  
            buffer->rowSize  += gl::BufferColumn[buffer->column[i].type].size;
            }

         if (buffer->type == VertexBuffer)
            if (GpuContext.screen.support(OpenGL_3_0))            
               Profile( glGenVertexArrays(1, &buffer->vao) )

         buffer->elements = bufferSettings.elements;             
         buffer->columns  = columns; 
         buffer->size     = buffer->elements * buffer->rowSize;
         }
      else
         buffer->size     = bufferSettings.size;
                
      // Extract information needed for buffer creation in OpenGL
      uint16 bufferType = gl::BufferType[bufferSettings.type].type;
      
      // Generate corresponding VBO or IBO in driver and reserve memory
      Profile( glGenBuffers(1, &buffer->id) )
      Profile( glBindBuffer(bufferType, buffer->id) )
      Profile( glBufferData(bufferType, buffer->size, data, GL_STATIC_DRAW) )
      Profile( glBindBuffer(bufferType, 0) )

      // Return buffers interface
      return Buffer(buffer);
      }
      
      #ifdef EN_OPENGL_DESKTOP
      // Maps VBO or IBO buffer to memory
      void* BufferMap(BufferDescriptor* buffer)
      {
      // Screen needs to be created
      assert(GpuContext.screen.created);
      assert(buffer);
      
      // Maps VBO or IBO
      uint16 type = gl::BufferType[buffer->type].type;
      void* pointer = nullptr;
      Profile( glBindBuffer(type, buffer->id) )
      Profile( pointer = glMapBuffer(type, GL_READ_WRITE) )
      return pointer;
      }
      
      bool BufferUnmap(BufferDescriptor* buffer)
      {
      // Screen needs to be created
      assert(GpuContext.screen.created);
      assert(buffer);
     
      // Unmaps VBO or IBO 
      uint16 type = gl::BufferType[buffer->type].type;
      bool success = false;
      Profile( glBindBuffer(type, buffer->id) )
      Profile( success = static_cast<bool>(glUnmapBuffer(type)) )
      return success;
      }

      #endif
      #ifdef EN_OPENGL_MOBILE

      // Maps VBO or IBO buffer to memory
      void* BufferMap(BufferDescriptor* buffer)
      {
      // Screen needs to be created
      assert(GpuContext.screen.created);
      assert(buffer);

      // Allocate DataBlockDescriptor and its local memory 
      buffer->block = new DataBlockDescriptor;
      buffer->block->size = buffer->rowSize * buffer->elements;
      buffer->block->pointer = new uint8[buffer->block->size];

      return buffer->block->pointer;
      }

      bool BufferUnmap(BufferDescriptor* buffer)
      {
      // Screen needs to be created
      assert(GpuContext.screen.created);
      assert(buffer);

      // Maps VBO or IBO
      uint16 type = gl::BufferType[buffer->type].type;
      if ( buffer->type == VertexBuffer ||
           buffer->type == IndexBuffer )
         {
         Profile( glBindBuffer(type, buffer->id) )
         Profile( glBufferData(type, buffer->elements * buffer->rowSize, buffer->block->pointer, GL_STATIC_DRAW) )
         Profile( glBindBuffer(type, 0) )

         // Free DataBlockDescriptor and its local memory
         delete [] static_cast<uint8*>(buffer->block->pointer);
         delete buffer->block;
         return true;        
         }

      return false;
      }
      #endif

      bool BufferDestroy(BufferDescriptor* const buffer)
      {         
      // Frees corresponding buffer in driver
      Profile( glDeleteBuffers(1, &buffer->id) )

      if (buffer->type == VertexBuffer)
         if (GpuContext.screen.support(OpenGL_3_0))            
            Profile( glDeleteVertexArrays(1, &buffer->vao) )

      // Fill program with null pointer
      GpuContext.buffers.free(buffer);
      return true; 
      }

      }

      namespace es20
      {
      #ifdef EN_OPENGL_MOBILE

      #endif
      }
   }
}

template<> bool (*ProxyInterface<en::gpu::BufferDescriptor>::destroy)(en::gpu::BufferDescriptor* const) = en::gpu::gl20::BufferDestroy;
