/*

 Ngine v5.0
 
 Module      : TEX file support
 Requirements: none
 Description : Supports engine proprietary file format
               for storing textures of different types
               formats and compressions.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "utilities/utilities.h"
#include "resources/context.h"

#include "core/rendering/device.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#include "zlib.h"
#endif

#include <string>

namespace en
{
   namespace tex
   {
   struct Header_v1
      {
      uint32 signature;   // TEX file signature 'ETEX' -> 0x58455445 
      uint32 version;     // Version of format in which file is stored
      uint64 filesize;    // Total file size for verification
      uint32 textures;    // Textures in file
      };

   struct TextureHeader_v1
      {
      uint16 compression;  // Compression type used on this texture. If 0, texture can be directly uploaded to GPU
      uint16 colorSpace;   // Color space
      uint32 type;         // GPU friendly texture type (enum TextureType)
      uint32 format;       // GPU friendly texture format (enum TextureFormat)
      uint16 width;        // Width (mipmap 0)
      uint16 height;       // Height (mipmap 0)
      uint16 depth;        // Depth (mipmap 0)
      uint16 layers;       // Layers
      uint16 samples;      // Samples (if multisampled)
                           // Texture surface descriptors array 
      uint16 surfaces;     // Size
      uint64 offset;       // Offset in file
      uint64 name;         // Offset to name null terminated string
      uint32 length;       // Name length
      };

   struct TextureSurfaceHeader_v1
      {
      uint16 layer;        // Which layer/face of array/3D/cube texture it is
      uint16 mipmap;       // Which mip-map of texture it is (can load textures with missing mipmaps)
      uint16 width;        // Width (for validation)
      uint16 height;       // Height (for validation)
      uint64 offset;       // Offset in file to surface data
      uint64 size;         // Size (for validation)
      };


   bool save(const gpu::TextureState settings, const uint64 size, const void* data, const std::string& filename)
   {
   // TODO: Finish
   return false;
   }

   bool save(std::shared_ptr<gpu::Texture> texture, const std::string& filename)
   {
   // TODO: Finish
   return false;
   }

   std::shared_ptr<gpu::Texture> load(const std::string& filename)
   {
   using namespace en::storage;
   using namespace en::gpu;

   // Open image file 
   std::shared_ptr<File> file = Storage->open(filename);
   if (!file)
      {
      file = Storage->open(en::ResourcesContext.path.textures + filename);
      if (!file)
         {
         Log << en::ResourcesContext.path.textures + filename << std::endl;
         Log << "ERROR: There is no such file!\n";
         return nullptr;
         }
      }
 
   // Read file signature
   Header_v1 header;
   file->read(0, 20, &header);
   if ( header.signature != 0x58455445 )
      {
      Log << "ERROR: TEX file header signature is incorrect!\n";
      file = nullptr;
      return nullptr;
      }
   if ( header.version != 1 )
      {
      Log << "ERROR: TEX file header version is not supported!\n";
      file = nullptr;
      return nullptr;
      }
   if ( header.filesize != file->size() )
      {
      Log << "ERROR: TEX file size mismatch!\n";
      file = nullptr;
      return nullptr;
      }
   assert( header.textures );
   //assert( Gpu.screen.created() );

   // Alllocate textures array
   TextureHeader_v1* textures = new TextureHeader_v1[header.textures];
   uint32 datasize = sizeof(TextureHeader_v1) * header.textures;
   file->read(20, datasize, textures);
   std::shared_ptr<gpu::Texture>* out = new std::shared_ptr<gpu::Texture>[header.textures];

   // Process textures
   for(uint32 i=0; i<header.textures; ++i)
      {
      // TODO: Add support for custom compressions (like zlib, RLE, etc.)
      assert( textures[i].compression == 0 );

      // Create texture in GPU 
      TextureState settings(static_cast<TextureType>(textures[i].type),
                            static_cast<Format>(textures[i].format),
                            TextureUsage::Read,  // TODO: Fix it
                            textures[i].width,
                            textures[i].height,
                            1, // TODO: Mipmaps count
                            textures[i].layers,
                            textures[i].samples);
      out[i] = en::ResourcesContext.defaults.enHeapTextures->createTexture(settings);

      // TODO: Add proper support for error reporting
      assert( out[i] );

      // Alllocate surfaces array
      TextureSurfaceHeader_v1* surfaces = new TextureSurfaceHeader_v1[textures[i].surfaces];
      uint32 datasize = sizeof(TextureSurfaceHeader_v1) * textures[i].surfaces;
      file->read(textures[i].offset, datasize, surfaces);

      // Load surfaces of texture (mipmaps, faces, layers)
      for(uint16 j=0; j<textures[i].surfaces; ++j)
         {
         // Create staging buffer
         std::shared_ptr<gpu::Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, (uint32)surfaces[j].size);
         if (!staging)
            {
            Log << "ERROR: Cannot create staging buffer!\n";
            file = nullptr;
            return nullptr;
            }

         // Read texture to temporary buffer
         volatile void* ptr = staging->map();
         file->read(surfaces[j].offset, (uint32)surfaces[j].size, ptr);
         staging->unmap();
 
         // TODO: In future distribute transfers to different queues in the same queue type family
         gpu::QueueType type = gpu::QueueType::Universal;
         if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
            type = gpu::QueueType::Transfer;

         // Copy data from staging buffer to final texture
         std::shared_ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(type);
         command->start();
         command->copy(*staging, 0u, settings.rowSize(surfaces[j].mipmap), *out[i], surfaces[j].mipmap, surfaces[j].layer);
         command->commit();
         
         // TODO:
         // here return completion handler callback !!! (no waiting for completion)
         // - this callback destroys CommandBuffer object
         // - destroys staging buffer
         //
         // Till it's done, wait for completion:
         
         command->waitUntilCompleted();
         command = nullptr;
         staging = nullptr;
         }

      delete [] surfaces;
      }
   delete [] textures;

   // TODO: Add support for more than one texture
   return out[0];
   }

   }
}
