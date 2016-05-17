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

#include "core/defines.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/opengl/glBuffer.h"
#include "core/rendering/opengl/glInputAssembler.h"
#include "core/rendering/opengl/glDevice.h"

namespace en
{
   namespace gpu
   {
   // In OSX define missing enums as 0 for easier debugging.
   #ifndef GL_ATOMIC_COUNTER_BUFFER
   #define GL_ATOMIC_COUNTER_BUFFER        0
   #endif
   #ifndef GL_DISPATCH_INDIRECT_BUFFER
   #define GL_DISPATCH_INDIRECT_BUFFER     0
   #endif
   #ifndef GL_SHADER_STORAGE_BUFFER
   #define GL_SHADER_STORAGE_BUFFER        0
   #endif
   #ifndef GL_QUERY_BUFFER
   #define GL_QUERY_BUFFER                 0
   #endif
      
   static const uint16 TranslateBufferType[underlyingType(BufferType::Count)] =
      {
      GL_ARRAY_BUFFER              , // Vertex   (OpenGL 1.5)
      GL_ELEMENT_ARRAY_BUFFER      , // Index    (OpenGL 1.5)
      GL_UNIFORM_BUFFER            , // Uniform  (OpenGL 3.1)
      GL_SHADER_STORAGE_BUFFER     , // Storage  (OpenGL 4.3)
      GL_DISPATCH_INDIRECT_BUFFER  , // Indirect (OpenGL 4.3)
      };
   
      // TODO: Distinguish Draw and Dispatch Indirect commands
      //
      //GL_DRAW_INDIRECT_BUFFER      , // Draw     (OpenGL 4.0)
      //GL_TRANSFORM_FEEDBACK_BUFFER , // Feedback (OpenGL 3.1)
      //GL_ATOMIC_COUNTER_BUFFER     , // Counter  (OpenGL 4.2)
      //GL_QUERY_BUFFER              , // Query    (OpenGL 4.4)

   static const uint16 TranslateDataAccess[DataAccessTypes] =
      {
      GL_READ_ONLY   ,
      GL_WRITE_ONLY  ,
      GL_READ_WRITE  
      };
      
      
   BufferGL::BufferGL(const BufferType type, const uint32 size, const void* data) :
      vao(0),
      BufferCommon(type)
   {
   glType = TranslateBufferType[underlyingType(type)];
   
   Profile( glGenBuffers(1, static_cast<GLuint *>(&handle)) )
   Profile( glBindBuffer(glType, handle) )
   Profile( glBufferData(glType, size, data, GL_STATIC_DRAW) )
   Profile( glBindBuffer(glType, 0) )
   }
   
   BufferGL::~BufferGL()
   {
   Profile( glDeleteBuffers(1, static_cast<GLuint *>(&handle)) );
   if (vao)
      Profile( glDeleteVertexArrays(1, &vao) )
   }
   
#ifdef EN_OPENGL_DESKTOP
   void* BufferGL::map(const DataAccess access)
   {
   void* pointer = nullptr;
   Profile( glBindBuffer(glType, handle) );
   Profile( pointer = glMapBuffer(glType, TranslateDataAccess[access]) )
   return pointer;
   }

   bool BufferGL::unmap(void)
   {
   // We don't care if it was mapped or not
   bool success = false;
   Profile( glBindBuffer(glType, handle) )
   Profile( success = static_cast<bool>(glUnmapBuffer(glType)) )
   return success;
   }
#endif

#ifdef EN_OPENGL_MOBILE
   void* BufferGL::map(const DataAccess access)
   {
   void* pointer = nullptr;
   Profile( glBindBuffer(glType, handle) );
   Profile( pointer = glMapBuffer(glType, TranslateDataAccess[access]) )
   return pointer;
   }

   bool BufferGL::unmap(void)
   {
   // We don't care if it was mapped or not
   bool success = false;
   Profile( glBindBuffer(glType, handle) )
   Profile( success = static_cast<bool>(glUnmapBuffer(glType)) )
   return success;
   }
#endif
   
   
   void OpenGLDevice::init(void)
   {
   // Gather information about supported API capabilites
   
   // Attribute formats
   for(uint16 i=0; i<underlyingType(Attribute::Count); ++i)
      if (api.release >= AttributeVersion[i].release)
         support.attribute.set(i);
   }

   Ptr<Buffer> OpenGLDevice::create(const BufferType type, const uint32 size, const void* data)
   {
   assert( size );
   assert( type != BufferType::Count );
   
   // TODO: OpenGL Context needs to be tied to a window
   // assert(GpuContext.screen.created);
   
   Ptr<BufferGL> buffer = new BufferGL(type, size, data);
   if (buffer)
      {
      if (api.better(OpenGL_3_0))
         Profile( glGenVertexArrays(1, &buffer->vao) )
      }
      
   return ptr_dynamic_cast<Buffer, BufferGL>(buffer);
   }
      
   }
}
#endif



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
   . . .
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
   
   void BufferGL15::invalidate(void)
   {
   assert( GpuContext.screen.created );
   Profile( glBindBuffer(glType, id) );
   Profile( glBufferData(glType, capacity, NULL, glUsage) );
   allocated = false;
   }



   uint32 VertexBufferGL15::columns(void)
   {
   return columnsCount;
   }

//*/



