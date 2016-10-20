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

#ifndef ENG_CORE_RENDERING_BUFFER
#define ENG_CORE_RENDERING_BUFFER

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/state.h"
#include "core/utilities/Tproxy.h"
#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/texture.h"

namespace en
{
   namespace gpu
   {
   // Buffer Type
   enum class BufferType : uint32
      {
      Vertex              = 0, // Vertex   - Buffer consumed by Input Assembler (Vertices, Control Point's, etc.)
      Index                  , // Index    - Buffer consumed by Input Assembler (dictates primitives assembly order)
      Uniform                , // Uniform  - Read Only, 16KB to 64KB buffer for data
      Storage                , // Storage  - Read-Write, Atomic, minimum 1MB buffer for data
      Indirect               , // Indirect - Used as source for Draw and Dispatch Indirect commands
      Transfer               , // Upload   - Buffer used to transfer data from CPU RAM to Buffers and Textures in VRAM.
                               // Download - Buffer used to transfer data from Buffers and Textures in VRAM to CPU RAM.
      Count                    //            Upload buffers are used to populate all other resources with data.
      };
 
   // Attributes representing data in fixed point schemes like 16.16, 8.0, 16.0
   // were introduced in OpenGL ES for low end devices without HW acceleration. 
   // Currently all mobile devices has HW acceleration for OpenGL ES which means 
   // that floating point values will be better choose in almost all cases. 
   // Therefore fixed attribute formats are not supported by engine.

   // Format of attribute data
   enum class Attribute : uint32
      {
      None                = 0,
      u8_norm                , // Uncompressed formats:
      s8_norm                ,
      u8                     ,
      s8                     ,
      u16_norm               ,
      s16_norm               ,
      u16                    ,
      s16                    ,
      f16                    ,
      u32                    ,
      s32                    ,
      f32                    ,
      u64                    ,
      s64                    ,
      f64                    ,
      v2u8_norm              ,
      v2s8_norm              ,
      v2u8                   ,
      v2s8                   ,
      v2u16_norm             ,
      v2s16_norm             ,
      v2u16                  ,
      v2s16                  ,
      v2f16                  ,
      v2u32                  ,
      v2s32                  ,
      v2f32                  ,
      v2u64                  ,
      v2s64                  ,
      v2f64                  ,
      v3u8_norm              , //  - Not reccomended due to lack of memory aligment
      v3u8_srgb              , //  - Not reccomended due to lack of memory aligment
      v3s8_norm              , //  - Not reccomended due to lack of memory aligment
      v3u8                   , //  - Not reccomended due to lack of memory aligment
      v3s8                   , //  - Not reccomended due to lack of memory aligment
      v3u16_norm             ,
      v3s16_norm             ,
      v3u16                  ,
      v3s16                  ,
      v3f16                  ,
      v3u32                  ,
      v3s32                  ,
      v3f32                  ,
      v3u64                  ,
      v3s64                  ,
      v3f64                  ,
      v4u8_norm              ,
      v4s8_norm              ,
      v4u8                   ,
      v4s8                   ,
      v4u16_norm             ,
      v4s16_norm             ,
      v4u16                  ,
      v4s16                  ,
      v4f16                  ,
      v4u32                  ,
      v4s32                  ,
      v4f32                  ,
      v4u64                  ,
      v4s64                  ,
      v4f64                  , // Compressed formats:
      v3f11_11_10            , // - Packed unsigned float for HDR Textures, UAV's and Render Targets
      v4u10_10_10_2_norm     ,
      v4s10_10_10_2_norm     ,
      v4u10_10_10_2          ,
      v4s10_10_10_2          ,
      v4u10_10_10_2_norm_rev ,
      v4s10_10_10_2_norm_rev ,
      v4u10_10_10_2_rev      ,
      v4s10_10_10_2_rev      ,
      Count
      };

   // Vertex Fetch fixed point formats deliberately not supported:
   //  
   // cf - integer cast to float (also known as USCALED/SSCALED)
   //
   // It's better to use unsigned/signed integer formats and cast to float manually if needed.
   //          
   // Attribute::u8_cf
   // Attribute::s8_cf
   // Attribute::u16_cf
   // Attribute::s16_cf
   // Attribute::v2u8_cf
   // Attribute::v2s8_cf
   // Attribute::v2u16_cf
   // Attribute::v2s16_cf
   // Attribute::v3u8_cf
   // Attribute::v3s8_cf
   // Attribute::v3u16_cf
   // Attribute::v3s16_cf
   // Attribute::v4u8_cf    - Endiannes Independent
   // Attribute::v4s8_cf
   // Attribute::v4u8_cf    - Endiannes Dependent
   // Attribute::v4s8_cf
   // Attribute::v4u16_cf
   // Attribute::v4s16_cf
   // Attribute::v3u8_cf_rev
   // Attribute::v3s8_cf_rev
   // Attribute::v4u8_cf_rev
   // Attribute::v4s8_cf_rev
   // Attribute::v4u10_10_10_2_cf
   // Attribute::v4s10_10_10_2_cf
   // Attribute::v4u10_10_10_2_cf_rev
   // Attribute::v4s10_10_10_2_cf_rev

   #define MaxInputAssemblerAttributesCount 32
   
   // Layout of attributes in Input Assembler. 
   // Can be also used to specify Buffer formatting.
   class Formatting
      {
      public:
      Attribute column[MaxInputAssemblerAttributesCount]; // Format of each data column
      
      // Create data formatting layout by passing at least one data column format
      Formatting();
      Formatting(const Attribute col0,
                 const Attribute col1  = Attribute::None,
                 const Attribute col2  = Attribute::None,
                 const Attribute col3  = Attribute::None,
                 const Attribute col4  = Attribute::None,
                 const Attribute col5  = Attribute::None,
                 const Attribute col6  = Attribute::None,
                 const Attribute col7  = Attribute::None,                                                                
                 const Attribute col8  = Attribute::None,
                 const Attribute col9  = Attribute::None,
                 const Attribute col10 = Attribute::None,
                 const Attribute col11 = Attribute::None,
                 const Attribute col12 = Attribute::None,
                 const Attribute col13 = Attribute::None,
                 const Attribute col14 = Attribute::None,
                 const Attribute col15 = Attribute::None
                 );
         
      // Size of all attributes combined together, with platform specific padding.
      // Holes between attributes are not allowed.
      uint32 elementSize(void) const;
      
     ~Formatting();
      };
      
   class BufferView;
   
   class Buffer : public SafeObject<Buffer>
      {
      public:
      virtual uint32 length(void) const = 0;
      virtual BufferType type(void) const = 0;
      
      // Buffers created on heaps with "Streamed" and "Immediate" memory usage, can be mapped
      // to obtain pointers to their content. This is not allowed on "Storage" heaps.
      virtual void* map(void) = 0;
      virtual void* map(const uint64 offset, const uint64 size) = 0;
      virtual void  unmap(void) = 0;
      
//      virtual Ptr<BufferView> view(void) = 0;  // Default buffer view, if it was created with formatting
//      virtual Ptr<BufferView> view(const uint32 elements, 
//                                   const Formatting& formatting, 
//                                   const uint32 step = 0,
//                                   const uint32 offset = 0) = 0; // New buffer view with specified formatting
      
      virtual ~Buffer() {};           // Polymorphic deletes require a virtual base destructor
      };
      
   // Buffer formatted View
   class BufferView : public SafeObject<BufferView>
      {
      public:
      virtual uint32 offset(void) const = 0;
      virtual uint32 length(void) const = 0;
      virtual Format format(void) const = 0;
      virtual ~BufferView() {};       // Polymorphic deletes require a virtual base destructor
      };
      
      
      
     

      // R_8                 , Attribute::u8_norm
      // R_8_sn              , Attribute::s8_norm
      // R_8_u               , Attribute::u8
      // R_8_s               , Attribute::s8
      // R_16                , Attribute::u16_norm
      // R_16_sn             , Attribute::s16_norm
      // R_16_u              , Attribute::u16
      // R_16_s              , Attribute::s16
      // R_16_hf             , Attribute::f16
      // R_32_u              , Attribute::u32
      // R_32_s              , Attribute::s32
      // R_32_f              , Attribute::f32
      // R_64_u              , Attribute::u64
      // R_64_s              , Attribute::s64
      // R_64_f              , Attribute::f64
      // RG_8                , Attribute::v2u8_norm
      // RG_8_sn             , Attribute::v2s8_norm
      // RG_8_u              , Attribute::v2u8
      // RG_8_s              , Attribute::v2s8
      // RG_16               , Attribute::v2u16_norm
      // RG_16_sn            , Attribute::v2s16_norm
      // RG_16_u             , Attribute::v2u16
      // RG_16_s             , Attribute::v2s16
      // RG_16_hf            , Attribute::v2f16
      // RG_32_u             , Attribute::v2u32
      // RG_32_s             , Attribute::v2s32
      // RG_32_f             , Attribute::v2f32
      // RG_64_u             , Attribute::v2u64
      // RG_64_s             , Attribute::v2s64
      // RG_64_f             , Attribute::v2f64
      // RGB_8               , Attribute::v3u8_norm
      // RGB_8_sRGB          , Attribute::v3u8_srgb
      // RGB_8_sn            , Attribute::v3s8_norm
      // RGB_8_u             , Attribute::v3u8
      // RGB_8_s             , Attribute::v3s8
      // RGB_16              , Attribute::v3u16_norm
      // RGB_16_sn           , Attribute::v3s16_norm
      // RGB_16_u            , Attribute::v3u16
      // RGB_16_s            , Attribute::v3s16
      // RGB_16_hf           , Attribute::v3f16
      // RGB_32_u            , Attribute::v3u32
      // RGB_32_s            , Attribute::v3s32
      // RGB_32_f            , Attribute::v3f32
      // RGB_64_u            , Attribute::v3u64
      // RGB_64_s            , Attribute::v3s64
      // RGB_64_f            , Attribute::v3f64
      // RGBA_8              , Attribute::v4u8_norm
      // RGBA_8_sn           , Attribute::v4s8_norm
      // RGBA_8_u            , Attribute::v4u8
      // RGBA_8_s            , Attribute::v4s8
      // RGBA_16             , Attribute::v4u16_norm
      // RGBA_16_sn          , Attribute::v4s16_norm
      // RGBA_16_u           , Attribute::v4u16
      // RGBA_16_s           , Attribute::v4s16
      // RGBA_16_hf          , Attribute::v4f16
      // RGBA_32_u           , Attribute::v4u32
      // RGBA_32_s           , Attribute::v4s32
      // RGBA_32_f           , Attribute::v4f32
      // RGBA_64_u           , Attribute::v4u64
      // RGBA_64_s           , Attribute::v4s64
      // RGBA_64_f           , Attribute::v4f64
      // RGB_11_11_10_uf     , Attribute::v3f11_11_10
      // RGBA_10_10_10_2     , Attribute::v4u10_10_10_2_norm
      // RGBA_10_10_10_2_sn  , Attribute::v4s10_10_10_2_norm
      // RGBA_10_10_10_2_u   , Attribute::v4u10_10_10_2
      // RGBA_10_10_10_2_s   , Attribute::v4s10_10_10_2
      // BGRA_10_10_10_2     , Attribute::v4u10_10_10_2_norm_rev
      // BGRA_10_10_10_2_sn  , Attribute::v4s10_10_10_2_norm_rev
      // BGRA_10_10_10_2_u   , Attribute::v4u10_10_10_2_rev
      // BGRA_10_10_10_2_s   , Attribute::v4s10_10_10_2_rev

      
   // OLD API BELOW:
#if 0
   
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
#endif
   }
}

#endif
