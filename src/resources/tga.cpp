/*

 Ngine v5.0
 
 Module      : TGA files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from tga files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/rendering/device.h"
#include "utilities/utilities.h"
#include "resources/context.h"
#include "resources/tga.h"    

namespace en
{
   namespace tga
   {
   enum Format
        {
        Invalid             = 0,
        Indexed             = 1,
        RGB                 = 2,
        Bitmap              = 3,
        IndexedRLE          = 9,
        RGB_RLE             = 10,
        BitmapRLE           = 11,
        IndexedHuffman      = 32,
        IndexedHuffman4pass = 33
        };

   aligned(1) 
   struct Header
          {
          uint8  idSize;           // Size of identification string placed after header
          uint8  palette;          // Indicates image with color palette
          uint8  format;           // Image storage format
          uint16 paletteStart;     // Offset to color palette
          uint16 paletteEntries;   // Number of entries in color palette
          uint8  paletteBits;      // Bits per field in color palette
          sint16 offsetX;          // X coordinate of the lower-left image corner
          sint16 offsetY;          // Y coordinate of the lower-left image corner
          uint16 width;            // Image width
          uint16 height;           // Image height
          uint8  bpp;              // Bits per pixel: 1,16,24,32
          uint8  alphaBits    : 4; // Bits for alpha channel
          uint8  reserved     : 1; //
          uint8  origin       : 1; // 0-lower left, 1-upper-left 
          uint8  interleaving : 2; // 0 - none
                                   // 1 - two-way even/odd interleaving
                                   // 2 - four-way interleaving
                                   // 3 - reserved
          };
   aligndefault
    
   bool load(Ptr<en::gpu::Texture> dst, const uint16 layer, const string& filename)
   {
   using namespace en::storage;

  // Open image file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file))
      if (!Storage.open(en::ResourcesContext.path.textures + filename, &file))
         {
         Log << "ERROR: There is no such file " << filename.c_str() << "!\n";
         return false;
         }
  
   // Read file header
   Header header;
   file->read(0,18,&header);
  
   // Check for supported image format
   if (header.format != RGB &&
       header.format != RGB_RLE )
      {
      Log << "ERROR: This TGA file format is not supported!\n";
      delete file;
      return false;
      }      
  
   // Check if not paletted
   if (header.palette != 0)
      {
      Log << "ERROR: Paletted TGA files are not supported!\n";
      delete file;
      return false;
      }   
          
   // Determine texture parameters
   gpu::TextureState textureState; 
   textureState.width  = header.width;
   textureState.height = header.height;
   textureState.type   = gpu::TextureType::Texture2D;
   // We shouldn't dynamically switch between 1D and 2D textures in TGA, always use 2D here
   //if (Gpu.support.texture.type(gpu::TextureType::Texture1D))
   //   if (header.height == 1)
   //      textureState.type = gpu::TextureType::Texture1D;
    
   if (header.bpp == 24)
      textureState.format = gpu::Format::BGR_8;
   else
   if (header.bpp == 32)
      textureState.format = gpu::Format::BGRA_8;
   else
      {
      Log << "ERROR: Unsupported Bits Per Pixel quality!\n";
      delete file;
      return false;
      }

   // Check if image can be loaded to texture
   if (dst->type() != gpu::TextureType::Texture1DArray            &&
       dst->type() != gpu::TextureType::Texture2DArray            &&
       dst->type() != gpu::TextureType::Texture2DMultisampleArray &&
       dst->type() != gpu::TextureType::TextureCubeMapArray)
      {
      delete file;
      return false;
      }
   if ( (dst->layers() < layer) ||
        (dst->width()  != textureState.width)  ||
        (dst->height() != textureState.height) ||
        (dst->format() != textureState.format) )
      {
      delete file;
      return false;
      }

   // Calculate size of data to load
   uint32 srcOffset = sizeof(Header) + header.idSize;
   uint32 dataSize   = (header.width * header.height * header.bpp) / 8;
      
   // Create staging buffer
   Ptr<gpu::Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, dataSize);
   if (!staging)
      {
      Log << "ERROR: Cannot create staging buffer!\n";
      delete file;
      return false;
      }
      
   // Read image to gpu memory   
   if (header.format == RGB) 
      {
      void* ptr = staging->map();
      file->read(srcOffset, dataSize, ptr);    
      staging->unmap();
      }
   else
   if (header.format == RGB_RLE)
      {
      uint8 counter;
      uint32 dstOffset = 0;
      uint8  pixelSize = header.bpp / 8;
      void*  pixel = new uint8[pixelSize];
      void*  ptr   = staging->map();

      while(dstOffset < dataSize)
           {
           // Read RLE header
           file->read(srcOffset, 1, &counter);
           srcOffset++;

           // Run-length packet
           if (counter & 0x80)
              {
              counter -= 127;
              file->read(srcOffset, pixelSize, pixel);

              for(uint8 i=0; i<counter; ++i)
                 {
                 memcpy(((uint8*)ptr + dstOffset), pixel, pixelSize);
                 dstOffset += pixelSize;
                 }

              srcOffset += pixelSize;
              }
           // Non-run-length packet
           else
              {
              counter++;
              for(uint8 i=0; i<counter; ++i)
                 {
                 file->read(srcOffset, pixelSize, ((uint8*)ptr + dstOffset));
                 dstOffset += pixelSize;
                 srcOffset += pixelSize;
                 }
              }
           }

      staging->unmap();
      delete [] static_cast<uint8*>(pixel);
      }

   // TODO: In future distribute transfers to different queues in the same queue type family
   gpu::QueueType type = gpu::QueueType::Universal;
   if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
      type = gpu::QueueType::Transfer;

   // Copy data from staging buffer to final texture
   Ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(type);
   command->start();
   command->copy(staging, dst, 0u, layer);
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
 
   delete file;
   return true;
   }

   Ptr<en::gpu::Texture> load(const string& filename)
   {
   using namespace en::storage;

   // Try to reuse already loaded texture
   if (ResourcesContext.textures.find(filename) != ResourcesContext.textures.end())
      return ResourcesContext.textures[filename];

   // Open image file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file))
      if (!Storage.open(en::ResourcesContext.path.textures + filename, &file))
         {
         Log << "ERROR: There is no such file " << filename.c_str() << "!\n";
         return Ptr<gpu::Texture>(nullptr);
         }
  
   // Read file header
   Header header;
   file->read(0,18,&header);
  
   // Check for supported image format
   if (header.format != RGB &&
       header.format != RGB_RLE )
      {
      Log << "ERROR: This TGA file format is not supported!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }      
  
   // Check if not paletted
   if (header.palette != 0)
      {
      Log << "ERROR: Paletted TGA files are not supported!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }   
          
   // Determine texture parameters
   gpu::TextureState textureState; 
   textureState.width  = header.width;
   textureState.height = header.height;
   textureState.type   = gpu::TextureType::Texture2D;
   // We shouldn't dynamically switch between 1D and 2D textures in TGA, always use 2D here
   //if (Gpu.support.texture.type(gpu::TextureType::Texture1D))
   //   if (header.height == 1)
   //      textureState.type = gpu::TextureType::Texture1D;
    
   if (header.bpp == 24)
      textureState.format = gpu::Format::BGR_8;
   else
   if (header.bpp == 32)
      textureState.format = gpu::Format::BGRA_8;
   else
      {
      Log << "ERROR: Unsupported Bits Per Pixel quality!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Calculate size of data to load
   uint32 srcOffset = sizeof(Header) + header.idSize;
   uint32 dataSize  = (header.width * header.height * header.bpp) / 8;
      
   // Create texture
   Ptr<gpu::Texture> texture = en::ResourcesContext.defaults.enHeap->createTexture(textureState);
   if (!texture)
      {
      Log << "ERROR: Cannot create texture object!\n";
      delete file;
      return texture;
      }

   // Create staging buffer
   Ptr<gpu::Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, dataSize);
   if (!staging)
      {
      Log << "ERROR: Cannot create staging buffer!\n";
      delete file;
      return texture;
      }
      
   // Read image to gpu memory   
   if (header.format == RGB) 
      {     
      void* dst = staging->map();
      file->read(srcOffset, dataSize, dst);    
      staging->unmap();
      }
   else
   if (header.format == RGB_RLE)
      {
      uint8 counter;
      uint32 dstOffset = 0;
      uint8  pixelSize = header.bpp / 8;
      void*  pixel = new uint8[pixelSize];
      void*  dst   = staging->map();

      while(dstOffset < dataSize)
           {
           // Read RLE header
           file->read(srcOffset, 1, &counter);
           srcOffset++;

           // Run-length packet
           if (counter & 0x80)
              {
              counter -= 127;
              file->read(srcOffset, pixelSize, pixel);

              for(uint8 i=0; i<counter; ++i)
                 {
                 memcpy(((uint8*)dst + dstOffset), pixel, pixelSize);
                 dstOffset += pixelSize;
                 }

              srcOffset += pixelSize;
              }
           // Non-run-length packet
           else
              {
              counter++;
              for(uint8 i=0; i<counter; ++i)
                 {
                 file->read(srcOffset, pixelSize, ((uint8*)dst + dstOffset));
                 dstOffset += pixelSize;
                 srcOffset += pixelSize;
                 }
              }
           }

      staging->unmap();
      delete [] static_cast<uint8*>(pixel);
      }
 
   // TODO: Now blit from staging to texture
   uint32 mipmap = 0u;
   uint32 slice  = 0u;
   
   // TODO: In future distribute transfers to different queues in the same queue type family
   gpu::QueueType type = gpu::QueueType::Universal;
   if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
      type = gpu::QueueType::Transfer;

   // Copy data from staging buffer to final texture
   Ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(type);
   command->start();
   command->copy(staging, texture, mipmap, slice);
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
 
   // Update list of loaded textures
   ResourcesContext.textures.insert(pair<string, Ptr<en::gpu::Texture> >(filename, texture));
  
   delete file;
   return texture;
   }
  
   }
}
