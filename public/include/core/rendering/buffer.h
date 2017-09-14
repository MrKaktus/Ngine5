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
      v3u32                  ,
      v3s32                  ,
      v3f32                  ,
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
      v4u10_10_10_2_norm     ,
      Count
      };

   // Attribute formats supported only by Direct3D12:
   //
   // DXGI_FORMAT_R11G11B10_FLOAT          - Attribute::v3f11_11_10
   // DXGI_FORMAT_R10G10B10A2_UINT         - Attribute::v4u10_10_10_2       
   //
   // Attribute formats supported only by Metal API:
   //
   // MTLVertexFormatInt1010102Normalized  - Attribute::v4s10_10_10_2_norm  
   //
   // Metal is the only API that supports 3 component attributes, which
   // size is not aligned to four bytes (with exception of v3u8_sRGB): 
   //
   // MTLVertexFormatUChar3Normalized      - Attribute::v3u8_norm              
   // MTLVertexFormatChar3Normalized       - Attribute::v3s8_norm              
   // MTLVertexFormatUChar3                - Attribute::v3u8                   
   // MTLVertexFormatChar3                 - Attribute::v3s8                   
   // MTLVertexFormatUShort3Normalized     - Attribute::v3u16_norm             
   // MTLVertexFormatShort3Normalized      - Attribute::v3s16_norm             
   // MTLVertexFormatUShort3               - Attribute::v3u16                  
   // MTLVertexFormatShort3                - Attribute::v3s16                  
   // MTLVertexFormatHalf3                 - Attribute::v3f16 
   //
   // 64bit attributes are currently not supported by any backing API:
   // (Vulkan specifies their memory layout for all possible types though).
   //
   // VK_FORMAT_R64_UINT                   - Attribute::u64                    
   // VK_FORMAT_R64_SINT                   - Attribute::s64                    
   // VK_FORMAT_R64_SFLOAT                 - Attribute::f64  
   // VK_FORMAT_R64G64_UINT                - Attribute::v2u64                  
   // VK_FORMAT_R64G64_SINT                - Attribute::v2s64                  
   // VK_FORMAT_R64G64_SFLOAT              - Attribute::v2f64 
   // VK_FORMAT_R64G64B64_UINT             - Attribute::v3u64                  
   // VK_FORMAT_R64G64B64_SINT             - Attribute::v3s64                  
   // VK_FORMAT_R64G64B64_SFLOAT           - Attribute::v3f64  
   // VK_FORMAT_R64G64B64A64_UINT          - Attribute::v4u64                  
   // VK_FORMAT_R64G64B64A64_SINT          - Attribute::v4s64                  
   // VK_FORMAT_R64G64B64A64_SFLOAT        - Attribute::v4f64 
   //
   // Legacy Vertex Fetch fixed point formats like 16.16, 8.0, 16.0 were 
   // introduced in OpenGL ES for low end devices without HW acceleration. 
   // Currently all mobile devices have HW acceleration. Therefore fixed
   // attribute formats are not supported anymore by modern API's.
   //  
   // cf - integer cast to float (also known as USCALED/SSCALED)
   //
   // VK_FORMAT_R8_USCALED                 - Attribute::u8_cf
   // VK_FORMAT_R8_SSCALED                 - Attribute::s8_cf
   // VK_FORMAT_R16_USCALED                - Attribute::u16_cf
   // VK_FORMAT_R16_SSCALED                - Attribute::s16_cf
   // VK_FORMAT_R8G8_USCALED               - Attribute::v2u8_cf
   // VK_FORMAT_R8G8_SSCALED               - Attribute::v2s8_cf
   // VK_FORMAT_R16G16_USCALED             - Attribute::v2u16_cf
   // VK_FORMAT_R16G16_SSCALED             - Attribute::v2s16_cf
   // VK_FORMAT_R8G8B8_USCALED             - Attribute::v3u8_cf
   // VK_FORMAT_R8G8B8_SSCALED             - Attribute::v3s8_cf
   // VK_FORMAT_R16G16B16_USCALED          - Attribute::v3u16_cf
   // VK_FORMAT_R16G16B16_SSCALED          - Attribute::v3s16_cf
   // VK_FORMAT_R8G8B8A8_USCALED           - Attribute::v4u8_cf    - Endiannes Independent
   // VK_FORMAT_R8G8B8A8_SSCALED           - Attribute::v4s8_cf
   // VK_FORMAT_A8B8G8R8_USCALED_PACK32    - Attribute::v4u8_cf    - Endiannes Dependent
   // VK_FORMAT_A8B8G8R8_SSCALED_PACK32    - Attribute::v4s8_cf
   // VK_FORMAT_R16G16B16A16_USCALED       - Attribute::v4u16_cf
   // VK_FORMAT_R16G16B16A16_SSCALED       - Attribute::v4s16_cf
   // VK_FORMAT_B8G8R8_USCALED             - Attribute::v3u8_cf_rev
   // VK_FORMAT_B8G8R8_SSCALED             - Attribute::v3s8_cf_rev
   // VK_FORMAT_B8G8R8A8_USCALED           - Attribute::v4u8_cf_rev
   // VK_FORMAT_B8G8R8A8_SSCALED           - Attribute::v4s8_cf_rev
   // VK_FORMAT_A2B10G10R10_USCALED_PACK32 - Attribute::v4u10_10_10_2_cf
   // VK_FORMAT_A2B10G10R10_SSCALED_PACK32 - Attribute::v4s10_10_10_2_cf
   // VK_FORMAT_A2R10G10B10_USCALED_PACK32 - Attribute::v4u10_10_10_2_cf_rev
   // VK_FORMAT_A2R10G10B10_SSCALED_PACK32 - Attribute::v4s10_10_10_2_cf_rev
   //
   // It's worth to mention that OpenGL specified two attribute types marked 
   // below, that differed as one was Endiannes independent on read, while 
   // other was identical except of the fact that it was Endiannes dependent. 
   //
   // VK_FORMAT_R8G8B8A8_USCALED           - Attribute::v4u8_cf    - Endiannes Independent
   // VK_FORMAT_R8G8B8A8_SSCALED           - Attribute::v4s8_cf
   // VK_FORMAT_A8B8G8R8_USCALED_PACK32    - Attribute::v4u8_cf    - Endiannes Dependent
   // VK_FORMAT_A8B8G8R8_SSCALED_PACK32    - Attribute::v4s8_cf
   //
   // USCALED/SSCALED formats are still defined in Vulkan, but their support
   // in API for any kind of operation is not guaranteed nor expected.

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
