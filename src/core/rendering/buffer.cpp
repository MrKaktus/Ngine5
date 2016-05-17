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
#include "core/rendering/device.h"

namespace en
{
   namespace gpu
   {
//   void* Buffer::map(void)
//   {
//   return pointer == NULL ? NULL : gl20::BufferMap(pointer);
//   }
//   
//   bool Buffer::unmap(void)
//   {
//   return pointer == NULL ? false : gl20::BufferUnmap(pointer);
//   }
//
//   uint32 Buffer::columns(void)
//   {
//   return pointer == NULL ? 0 : pointer->columns;
//   }
//
//   // Creates buffer with optional passing of data
//   Buffer Interface::Buffer::create(const BufferSettings& bufferSettings, void* data)
//   {
//   // TODO: Dynamic linking in this point
//   return gl20::BufferCreate(bufferSettings, data);
//   }
//
//   BufferColumnDescriptor::BufferColumnDescriptor() :
//      type(None),
//      name(NULL)
//   {
//   }
//
//   BufferColumnDescriptor::~BufferColumnDescriptor()
//   {
//   delete [] name;
//   }
//
//   DataBlockDescriptor::DataBlockDescriptor() :
//      size(0),
//      pointer(NULL)
//   {
//   }
//
//   DataBlockDescriptor::~DataBlockDescriptor()
//   {
//   // Pointer needs to be released externally
//   // depending on supported OpenGL ES version.
//   }
//
//   BufferDescriptor::BufferDescriptor() :
//      elements(0),
//      columns(0),
//      rowSize(0),
//      size(0),
//      block(NULL),
//      id(0),
//      vao(0),
//      named(false)
//   {
//   }





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






















//      // Creates input buffer 
//      Buffer BufferCreate(const BufferSettings& bufferSettings, void* data)
//      {
//      // Screen needs to be created
//      assert(GpuContext.screen.created);
//
//      // Buffer Settings need to make sense
//      assert(bufferSettings.type == VertexBuffer  || 
//             bufferSettings.type == IndexBuffer   ||
//             bufferSettings.type == UniformBuffer ||
//             bufferSettings.type == StorageBuffer);
//      assert(bufferSettings.elements != 0);
//
//      // Check if column types are supported 
//      uint8 columns = 0;
//      if ( bufferSettings.type == VertexBuffer )
//         { 
//         for(uint8 i=0; i<16; i++)
//            {
//            if (bufferSettings.column[i].type == None)
//               break;
//            if (!gl::BufferColumnSupported[bufferSettings.column[i].type])
//               {
//#ifdef EN_DEBUG
//               Log << "ERROR: Buffer column of type: \"";
//               Log << gl::BufferColumnName[bufferSettings.column[i].type];
//               Log << "\" is not supported in this context!" << endl;
//#endif
//               return Buffer(NULL);
//               }
//            columns++;
//            }
//         }
//
//      if ( bufferSettings.type == IndexBuffer )
//         {
//         if (bufferSettings.column[1].type != None)
//            return Buffer(NULL);
//         if (bufferSettings.column[0].type != UByte  &&
//             bufferSettings.column[0].type != UShort &&
//             bufferSettings.column[0].type != UInt)
//            {
//#ifdef EN_DEBUG
//            Log << "ERROR: Buffer column of type: \"";
//            Log << gl::BufferColumnName[bufferSettings.column[0].type];
//            Log << "\" cannot be used in IndexBuffer!" << endl;
//            return Buffer(NULL);
//#endif
//            }
//         columns = 1;
//         }
//    
//      // Try to add new buffer descriptor 
//      BufferDescriptor* buffer = GpuContext.buffers.allocate();
//      if (!buffer)
//         return Buffer(NULL);
//  
//      // Fill buffer descriptor with data
//      buffer->type     = bufferSettings.type;  
//      if (buffer->type == VertexBuffer ||
//          buffer->type == IndexBuffer)
//         {
//         buffer->rowSize = 0;
//         for(uint8 i=0; i<columns; ++i)
//            {
//            buffer->column[i].type = bufferSettings.column[i].type;
//            buffer->column[i].name = NULL;
//            if (bufferSettings.column[i].name.size() > 0)
//               {
//               // Create local copy of column name
//               buffer->column[i].name = new char[bufferSettings.column[i].name.size() + 1];
//               memcpy(buffer->column[i].name, bufferSettings.column[i].name.c_str(), bufferSettings.column[i].name.size());
//               buffer->column[i].name[ bufferSettings.column[i].name.size() ] = 0;
//               buffer->named = true;
//               }    
//            buffer->offset[i] = buffer->rowSize;  
//            buffer->rowSize  += gl::BufferColumn[buffer->column[i].type].size;
//            }
//
//         if (buffer->type == VertexBuffer)
//            if (GpuContext.screen.support(OpenGL_3_0))            
//               Profile( glGenVertexArrays(1, &buffer->vao) )
//
//         buffer->elements = bufferSettings.elements;             
//         buffer->columns  = columns; 
//         buffer->size     = buffer->elements * buffer->rowSize;
//         }
//      else
//         buffer->size     = bufferSettings.size;
//                
//      // Extract information needed for buffer creation in OpenGL
//      uint16 bufferType = gl::BufferType[bufferSettings.type].type;
//      
//      // Generate corresponding VBO or IBO in driver and reserve memory
//      Profile( glGenBuffers(1, &buffer->id) )
//      Profile( glBindBuffer(bufferType, buffer->id) )
//      Profile( glBufferData(bufferType, buffer->size, data, GL_STATIC_DRAW) )
//      Profile( glBindBuffer(bufferType, 0) )
//
//      // Return buffers interface
//      return Buffer(buffer);
//      }
      

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

      if (buffer->type == BufferType::Vertex)
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
