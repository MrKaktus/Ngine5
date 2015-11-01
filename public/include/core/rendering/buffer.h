/*

 Ngine v5.0
 
 Module      : Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_BUFFER
#define ENG_CORE_RENDERING_BUFFER

#include <string>
using namespace std;

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/state.h"
#include "core/utilities/Tproxy.h"

namespace en
{
   namespace gpu
   {
   // Columns representing data in fixed point sheme 16.16 were introduced 
   // in OpenGL ES for low end devices without HW acceleration. Currently
   // almost all mobile devices has HW acceleration for OpenGL ES which means 
   // that floating point values will be better choose in almost all cases. 
   // Therefore fixed column formats are not supported by engine.

   // Type of data in columns
   enum ColumnType
        {
        None                      = 0,   
        Float_8                      ,
        Float2_8                     ,
        Float3_8                     ,
        Float4_8                     ,
        Float_16                     ,
        Float2_16                    ,
        Float3_16                    ,
        Float4_16                    ,
        UFloat_8                     ,
        UFloat2_8                    ,
        UFloat3_8                    ,
        UFloat4_8                    ,
        UFloat_16                    ,
        UFloat2_16                   ,
        UFloat3_16                   ,
        UFloat4_16                   ,
        Half                         ,
        Half2                        ,
        Half3                        ,
        Half4                        ,
        Float                        ,
        Float2                       ,
        Float3                       ,
        Float4                       ,
        Double                       ,
        Double2                      ,
        Double3                      ,
        Double4                      ,
        Float_8_SNorm                ,
        Float2_8_SNorm               ,
        Float3_8_SNorm               ,
        Float4_8_SNorm               ,
        Half_SNorm                   ,
        Half2_SNorm                  ,
        Half3_SNorm                  ,
        Half4_SNorm                  ,
        Float_SNorm                  ,
        Float2_SNorm                 ,
        Float3_SNorm                 ,
        Float4_SNorm                 ,
        Float_8_Norm                 ,
        Float2_8_Norm                ,
        Float3_8_Norm                ,
        Float4_8_Norm                ,
        Half_Norm                    ,
        Half2_Norm                   ,
        Half3_Norm                   ,
        Half4_Norm                   ,
        Float_Norm                   ,
        Float2_Norm                  ,
        Float3_Norm                  ,
        Float4_Norm                  ,
        Byte                         ,
        Byte2                        ,
        Byte3                        ,
        Byte4                        ,
        Short                        ,
        Short2                       ,
        Short3                       ,
        Short4                       ,
        Int                          ,
        Int2                         ,
        Int3                         ,
        Int4                         ,
        UByte                        ,
        UByte2                       ,
        UByte3                       ,
        UByte4                       ,
        UShort                       ,
        UShort2                      ,
        UShort3                      ,
        UShort4                      ,
        UInt                         ,
        UInt2                        ,
        UInt3                        ,
        UInt4                        ,
        Float4_10_10_10_2_SNorm      ,
        Float4_10_10_10_2_Norm       ,
        ColumnTypesCount
        };

   // Buffer type
   enum BufferType                      // OpenGL:
        {
        VertexBuffer              = 0,  // Vertex Buffer Object
        IndexBuffer                  ,  // Index Buffer Object
        UniformBuffer                ,  // Uniform Buffer Object
        FeedbackBuffer               ,  // Transform Feedback Buffer Object
        DrawBuffer                   ,  // Draw Indirect Buffer Object
        CounterBuffer                ,  // Atomic Counters Buffer Object
        DispatchBuffer               ,  // Dispatch Indirect Buffer Object
        StorageBuffer                ,  // Shader Storage Buffer Object
        QueryBuffer                  ,  // Query Buffer Object
        BufferTypesCount
        };

   // Buffer data transfer type
   enum DataTransfer
        {
        Static                    = 0,  // Data will be static, set once, used whole the time
        Dynamic                      ,  // Data will be updated frequently
        Streaming                    ,  // Data in most cases will be used only once and updated all the time
        DataTransferTypes
        };
   
   // Buffer column description
   class ColumnInfo
         {
         public:
         ColumnType type;       // Column type
         string     name;       // Column name

         ColumnInfo();
         ColumnInfo(const ColumnType type);
         ColumnInfo(const ColumnType type,
                    const string     name );
         };

   // Buffer Settings structure
   class BufferSettings
         {
         public:
         ColumnInfo column[16];  // Vector holding types of each column
         BufferType type;        // Buffer type
         union {                 // Vertex buffer can store geometry vertices
               uint32 vertices;  // or like Index buffer "elements" of some 
               uint32 elements;  // other type, while other buffers just need
               uint32 size;      // their size specified.
               };

         BufferSettings(const BufferType type  = VertexBuffer, // Default Constructor   
                        const uint32 vertices  = 0,  
                        const ColumnType col0  = None,
                        const ColumnType col1  = None,
                        const ColumnType col2  = None,
                        const ColumnType col3  = None,
                        const ColumnType col4  = None,
                        const ColumnType col5  = None,
                        const ColumnType col6  = None,
                        const ColumnType col7  = None,
                        const ColumnType col8  = None,
                        const ColumnType col9  = None,
                        const ColumnType col10 = None,
                        const ColumnType col11 = None,
                        const ColumnType col12 = None,
                        const ColumnType col13 = None,
                        const ColumnType col14 = None,
                        const ColumnType col15 = None
                        );

         BufferSettings(const BufferType type,                 // Constructor 
                        const uint32 vertices  = 0,  
                        const ColumnInfo col0  = ColumnInfo(None),
                        const ColumnInfo col1  = ColumnInfo(None),
                        const ColumnInfo col2  = ColumnInfo(None),
                        const ColumnInfo col3  = ColumnInfo(None),
                        const ColumnInfo col4  = ColumnInfo(None),
                        const ColumnInfo col5  = ColumnInfo(None),
                        const ColumnInfo col6  = ColumnInfo(None),
                        const ColumnInfo col7  = ColumnInfo(None),
                        const ColumnInfo col8  = ColumnInfo(None),
                        const ColumnInfo col9  = ColumnInfo(None),
                        const ColumnInfo col10 = ColumnInfo(None),
                        const ColumnInfo col11 = ColumnInfo(None),
                        const ColumnInfo col12 = ColumnInfo(None),
                        const ColumnInfo col13 = ColumnInfo(None),
                        const ColumnInfo col14 = ColumnInfo(None),
                        const ColumnInfo col15 = ColumnInfo(None)
                        );
         };

   // Buffer
   class Buffer : public ProxyInterface<class BufferDescriptor>
         {
         public:
         Buffer();
         Buffer(class BufferDescriptor* src);

         void*  map(void);           // Maps buffer to clients memory
         bool   unmap(void);         // Unmaps buffer from client memory
         uint32 columns(void);       // Returns buffer columns count
         };


   // Vertex Buffer Settings structure
   struct VertexBufferSettings
      {
      ColumnInfo   column[16]; // Vector holding types of each column
      DataTransfer transfer;   // Buffer can store static/dynamic data or be used for streaming
      DataAccess   access;     // Buffer can be used for reading from or writing data to GPU, or both
      union {                  // Buffer can store geometry vertices or 
            uint32 vertices;   // "elements" of some other type. This is
            uint32 elements;   // why two naming conventions are allowed.
            };
      
      // Default Constructor 
      VertexBufferSettings();
      
      // Basic constructor
      VertexBufferSettings(
         const uint32 vertices,  
         const ColumnInfo col0,
         const ColumnInfo col1  = ColumnInfo(None),
         const ColumnInfo col2  = ColumnInfo(None),
         const ColumnInfo col3  = ColumnInfo(None),
         const ColumnInfo col4  = ColumnInfo(None),
         const ColumnInfo col5  = ColumnInfo(None),
         const ColumnInfo col6  = ColumnInfo(None),
         const ColumnInfo col7  = ColumnInfo(None),
         const ColumnInfo col8  = ColumnInfo(None),
         const ColumnInfo col9  = ColumnInfo(None),
         const ColumnInfo col10 = ColumnInfo(None),
         const ColumnInfo col11 = ColumnInfo(None),
         const ColumnInfo col12 = ColumnInfo(None),
         const ColumnInfo col13 = ColumnInfo(None),
         const ColumnInfo col14 = ColumnInfo(None),
         const ColumnInfo col15 = ColumnInfo(None)
         );
      
      // Advanced constructor
      VertexBufferSettings(
         const DataTransfer transfer, 
         const DataAccess access,
         const uint32 vertices,  
         const ColumnInfo col0,
         const ColumnInfo col1  = ColumnInfo(None),
         const ColumnInfo col2  = ColumnInfo(None),
         const ColumnInfo col3  = ColumnInfo(None),
         const ColumnInfo col4  = ColumnInfo(None),
         const ColumnInfo col5  = ColumnInfo(None),
         const ColumnInfo col6  = ColumnInfo(None),
         const ColumnInfo col7  = ColumnInfo(None),
         const ColumnInfo col8  = ColumnInfo(None),
         const ColumnInfo col9  = ColumnInfo(None),
         const ColumnInfo col10 = ColumnInfo(None),
         const ColumnInfo col11 = ColumnInfo(None),
         const ColumnInfo col12 = ColumnInfo(None),
         const ColumnInfo col13 = ColumnInfo(None),
         const ColumnInfo col14 = ColumnInfo(None),
         const ColumnInfo col15 = ColumnInfo(None)
         );
      };

   // Index Buffer Settings structure
   struct IndexBufferSettings
      {
      ColumnType type;     // Column type, should be one of: UByte, UShort, UInt
      uint32     indices;  // Number indices in buffer
      
      IndexBufferSettings();
      IndexBufferSettings(const uint32 indices, const ColumnType type);
      };
   }
}

#endif