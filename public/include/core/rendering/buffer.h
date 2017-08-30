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
   // Currently all mobile devices has HW acceleration which means that floating
   // point values will be better choose in almost all cases. Therefore fixed
   // attribute formats are not supported by engine.

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
      v3u8_norm              , //  - Not reccomended due to lack of memory alignment
      v3u8_srgb              , //  - Not reccomended due to lack of memory alignment
      v3s8_norm              , //  - Not reccomended due to lack of memory alignment
      v3u8                   , //  - Not reccomended due to lack of memory alignment
      v3s8                   , //  - Not reccomended due to lack of memory alignment
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
      v4u10_10_10_2_norm_rev , // BGRA
      v4s10_10_10_2_norm_rev , // BGRA
      v4u10_10_10_2_rev      , // BGRA
      v4s10_10_10_2_rev      , // BGRA
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

   #define MaxInputLayoutAttributesCount 32
   
   // Layout of attributes in Input Assembler. 
   // Can be also used to specify Buffer formatting.
   class Formatting
      {
      public:
      Attribute column[MaxInputLayoutAttributesCount]; // Format of each data column
      
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
      virtual uint64 length(void) const = 0;
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
      virtual uint64 offset(void) const = 0;
      virtual uint64 length(void) const = 0;
      virtual Format format(void) const = 0;
      virtual ~BufferView() {};       // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
