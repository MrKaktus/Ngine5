/*

 Ngine v5.0
 
 Module      : BMP files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from bmp files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "utilities/utilities.h"
#include "resources/context.h"
#include "resources/bmp.h"

#include "core/rendering/device.h"

#include <string>
using namespace std;

namespace en
{
   namespace bmp
   {
   enum Compression
        {
        None          = 0,  // Uncompressed
        RLE8          = 1,  // 8bpp RLE
        RLE4          = 2,  // 4bpp RLE
        Huffman       = 3,  // Bit field / Huffman 1D 
        JPEG          = 4,  // JPEG / 24bpp RLE
        PNG           = 5,  // PNG
        AlphaBitField = 6
        };

   aligned(1) 
   struct Header
          {
          uint16 signature;   // BMP file signature 'BM' -> 0x4D42
          uint32 size;        // File size in bytes
          uint32 reserved;    // Reserved
          uint32 dataOffset;  // Offset in file to image data
          };
 
   struct DIBHeaderOS2
          {
          uint32      headerSize;  // DIB header size
          uint16      width;       // Image width
          uint16      height;      // Image height
          uint16      planes;      // Color planes
          uint16      bpp;         // Bits per pixel: 1,4,8,24
          };
    
   struct DIBHeaderV1
          {
          uint32      headerSize;  // DIB header size
          sint32      width;       // Image width
          sint32      height;      // Image height
          uint16      planes;      // Color planes
          uint16      bpp;         // Bits per pixel: 1,4,8,16,24,32
          Compression compression; // Compression method
          uint32      size;        // Image size
          sint32      hres;        // Horizontal pixels per meter
          sint32      vres;        // Vertical pixels per meter
          uint32      colors;      // Colors used in image
          uint32      colorsImp;   // Important colors in image
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
         Log << en::ResourcesContext.path.textures + filename << endl;
         Log << "ERROR: There is no such file!\n";
         return false;
         }
   
   // Check file header signature
   uint16 signature;
   file->read(0,2,&signature);
   if (signature != 0x4D42)
      {
      Log << "ERROR: BMP file header signature incorrect!\n";
      delete file;
      return false;
      }
   
   // Read file header
   Header header;
   file->read(0,14,&header);
   if (file->size() != header.size)
      {
      Log << "ERROR: BMP file header incorrect!\n";
      delete file;
      return false;
      }
   
   // Read DIB header
   DIBHeaderV1 DIBHeader;
   uint32 headerSize;
   file->read(14,4,&headerSize);
   if (headerSize == sizeof(DIBHeaderOS2))
      file->read(14,sizeof(DIBHeaderOS2),&DIBHeader);
   else
   if (headerSize == sizeof(DIBHeaderV1))
      file->read(14,sizeof(DIBHeaderV1),&DIBHeader);   
   else
      {
      Log << "ERROR: Unsupported DIB header!\n";
      delete file;
      return false;
      }    
   
   // Check if image is not compressed
   if (DIBHeader.compression != None)
      {
      Log << "ERROR: Compressed BMP files are not supported!\n";
      delete file;
      return false;
      }
   
   // Determine texture parameters
   gpu::TextureState settings;
   settings.width  = DIBHeader.width;
   settings.height = DIBHeader.height;
   if (DIBHeader.bpp == 24)
      settings.format = gpu::Format::BGR_8;
   else
   if (DIBHeader.bpp == 32)
      settings.format = gpu::Format::BGRA_8;
   else
      {
      Log << "ERROR: Unsupported Bits Per Pixel quality!\n";
      delete file;
      return false;
      }
   
   // Determine texture type
   settings.type    = gpu::TextureType::Texture2D;
   if (DIBHeader.height == 1)
      settings.type = gpu::TextureType::Texture1D;
   //if (!powerOfTwo(settings.width) ||
   //    !powerOfTwo(settings.height) )
   //   settings.type = gpu::TextureType::Texture2DRectangle;
   
   // Calculate size of data to load
   uint32 lineSize = (DIBHeader.width * DIBHeader.bpp) / 8;
   uint32 dataSize = 0;
   if (DIBHeader.headerSize == sizeof(DIBHeaderV1))
      dataSize = DIBHeader.size; 
   if (dataSize == 0)
      dataSize = lineSize * DIBHeader.height;

   // Check if image can be loaded to texture
   if (dst->type() != gpu::TextureType::Texture1DArray            &&
       dst->type() != gpu::TextureType::Texture2DArray            &&
       dst->type() != gpu::TextureType::Texture2DMultisampleArray &&
       dst->type() != gpu::TextureType::TextureCubeMapArray)
      return false;
   if (dst->layers() < layer)
      return false;
   if (dst->width() != settings.width)
      return false;
   if (dst->height() != settings.height)
      return false;
   if (dst->format() != settings.format)
      return false;

   // Read image to texture layer in gpu memory
   void* ptr = dst->map(0, layer);
   file->read(header.dataOffset, dataSize, ptr);
   return dst->unmap();
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
         Log << en::ResourcesContext.path.textures + filename << endl;
         Log << "ERROR: There is no such file!\n";
         return Ptr<gpu::Texture>(nullptr);
         }
   
   // Check file header signature
   uint16 signature;
   file->read(0,2,&signature);
   if (signature != 0x4D42)
      {
      Log << "ERROR: BMP file header signature incorrect!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }
   
   // Read file header
   Header header;
   file->read(0,14,&header);
   if (file->size() != header.size)
      {
      Log << "ERROR: BMP file header incorrect!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }
   
   // Read DIB header
   DIBHeaderV1 DIBHeader;
   uint32 headerSize;
   file->read(14,4,&headerSize);
   if (headerSize == sizeof(DIBHeaderOS2))
      file->read(14,sizeof(DIBHeaderOS2),&DIBHeader);
   else
   if (headerSize == sizeof(DIBHeaderV1))
      file->read(14,sizeof(DIBHeaderV1),&DIBHeader);   
   else
      {
      Log << "ERROR: Unsupported DIB header!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }    
   
   // Check if image is not compressed
   if (DIBHeader.compression != None)
      {
      Log << "ERROR: Compressed BMP files are not supported!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }
   
   // Determine texture parameters
   gpu::TextureState settings;
   settings.width  = DIBHeader.width;
   settings.height = DIBHeader.height;
   if (DIBHeader.bpp == 24)
      settings.format = gpu::Format::BGR_8;
   else
   if (DIBHeader.bpp == 32)
      settings.format = gpu::Format::BGRA_8;
   else
      {
      Log << "ERROR: Unsupported Bits Per Pixel quality!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }
   
   // Determine texture type
   settings.type    = gpu::TextureType::Texture2D;
   if (DIBHeader.height == 1)
      settings.type = gpu::TextureType::Texture1D;
   //if (!powerOfTwo(settings.width) ||
   //    !powerOfTwo(settings.height) )
   //   settings.type = gpu::TextureType::Texture2DRectangle;
   
   // Calculate size of data to load
   uint32 lineSize = (DIBHeader.width * DIBHeader.bpp) / 8;
   uint32 dataSize = 0;
   if (DIBHeader.headerSize == sizeof(DIBHeaderV1))
      dataSize = DIBHeader.size; 
   if (dataSize == 0)
      dataSize = lineSize * DIBHeader.height;

   // Create texture in gpu
   Ptr<gpu::Texture> texture = en::ResourcesContext.defaults.enHeap->createTexture(settings); // TODO: Use Heap !!
   if (!texture)
      {
      Log << "ERROR: Cannot create texture in GPU!\n";
      delete file;
      return texture;
      }

   // Read texture to gpu memory
   void* dst = texture->map();
   file->read(header.dataOffset, dataSize, dst);
   texture->unmap();
 
   // Update list of loaded textures
   ResourcesContext.textures.insert(pair<string, Ptr<en::gpu::Texture> >(filename, texture));
    
   return texture;
   }

   en::gpu::TextureState settings(const string& filename)
   {
   using namespace en::storage;

   // Open image file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file))
      if (!Storage.open(en::ResourcesContext.path.textures + filename, &file))
         {
         Log << en::ResourcesContext.path.textures + filename << endl;
         Log << "ERROR: There is no such file!\n";
         return gpu::TextureState();
         }
   
   // Check file header signature
   uint16 signature;
   file->read(0,2,&signature);
   if (signature != 0x4D42)
      {
      Log << "ERROR: BMP file header signature incorrect!\n";
      delete file;
      return gpu::TextureState();
      }
   
   // Read file header
   Header header;
   file->read(0,14,&header);
   if (file->size() != header.size)
      {
      Log << "ERROR: BMP file header incorrect!\n";
      delete file;
      return gpu::TextureState();
      }
   
   // Read DIB header
   DIBHeaderV1 DIBHeader;
   uint32 headerSize;
   file->read(14,4,&headerSize);
   if (headerSize == sizeof(DIBHeaderOS2))
      file->read(14,sizeof(DIBHeaderOS2),&DIBHeader);
   else
   if (headerSize == sizeof(DIBHeaderV1))
      file->read(14,sizeof(DIBHeaderV1),&DIBHeader);   
   else
      {
      Log << "ERROR: Unsupported DIB header!\n";
      delete file;
      return gpu::TextureState();
      }    
   
   // Check if image is not compressed
   if (DIBHeader.compression != None)
      {
      Log << "ERROR: Compressed BMP files are not supported!\n";
      delete file;
      return gpu::TextureState();
      }
   
   // Determine texture parameters
   gpu::TextureState settings;
   settings.width  = DIBHeader.width;
   settings.height = DIBHeader.height;
   if (DIBHeader.bpp == 24)
      settings.format = gpu::Format::BGR_8;
   else
   if (DIBHeader.bpp == 32)
      settings.format = gpu::Format::BGRA_8;
   else
      {
      Log << "ERROR: Unsupported Bits Per Pixel quality!\n";
      delete file;
      return en::gpu::TextureState();
      }
   
   // Determine texture type
   settings.type    = gpu::TextureType::Texture2D;
   if (DIBHeader.height == 1)
      settings.type = gpu::TextureType::Texture1D;
   //if (!powerOfTwo(settings.width) ||
   //    !powerOfTwo(settings.height) )
   //   settings.type = gpu::TextureType::Texture2DRectangle;
   
   // Texture can be decompressed
   delete file;
   return settings;
   }

   bool load(const string& filename, en::gpu::TextureState& settings, void* dst)
   {
   using namespace en::storage;
   
   assert(dst);
   
   if(bmp::settings(filename) != settings)
     return false;
   
   // Open image file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file))
      if (!Storage.open(en::ResourcesContext.path.textures + filename, &file))
         return false;
   
   // Read file header
   Header header;
   file->read(0,14,&header);
   
   // Read size of DIB header
   uint32 headerSize;
   file->read(14,4,&headerSize);
   
   // Read DIB header
   DIBHeaderV1 DIBHeader;
   if (headerSize == sizeof(DIBHeaderOS2))
      file->read(14,sizeof(DIBHeaderOS2),&DIBHeader);
   else
   if (headerSize == sizeof(DIBHeaderV1))
      file->read(14,sizeof(DIBHeaderV1),&DIBHeader);   
    
   // Calculate size of data to load
   uint32 lineSize = (DIBHeader.width * DIBHeader.bpp) / 8;
   uint32 dataSize = 0;
   if (DIBHeader.headerSize == sizeof(DIBHeaderV1))
      dataSize = DIBHeader.size; 
   if (dataSize == 0)
      dataSize = lineSize * DIBHeader.height;
   
   file->read(header.dataOffset, dataSize, dst);
   return true;
   }


   bool save(Ptr<en::gpu::Texture> texture, const string& filename)
   {
   using namespace en::storage;

   gpu::Format format = texture->format();
   if (format != gpu::Format::RGB_8 &&
       format != gpu::Format::RGBA_8)  // Check if shouldn't be RGBA, how GPU returns uncompressed data ?
      return false;

   // Open image file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file, en::storage::Write))
      if (!Storage.open(en::ResourcesContext.path.screenshots + filename, &file, en::storage::Write))
         {
         Log << en::ResourcesContext.path.screenshots + filename << endl;
         Log << "ERROR: Cannot create such file!\n";
         return false;
         }
  
   uint32 headersSize = sizeof(Header) + sizeof(DIBHeaderOS2);
   uint32 dataSize    = texture->size();

   // Write file headers
   Header header;
   header.signature  = 0x4D42;  
   header.size       = headersSize + dataSize;
   header.reserved   = 0;       
   header.dataOffset = headersSize;
   file->write(0, 14, &header);

   DIBHeaderOS2 DIBHeader;
   DIBHeader.headerSize = sizeof(DIBHeaderOS2);
   DIBHeader.width      = texture->width();
   DIBHeader.height     = texture->height();
   DIBHeader.planes     = 1;
   if (format == gpu::Format::RGB_8)  DIBHeader.bpp = 24;
   if (format == gpu::Format::RGBA_8) DIBHeader.bpp = 32;
   file->write(14, sizeof(DIBHeaderOS2), &DIBHeader);
 
   // Write texture data
   uint8* buffer = new uint8[texture->size()];
   texture->read(buffer);

   //Swap R and B components - BMP is saved in BGR, BGRA
   if (format == gpu::Format::RGB_8 ||
       format == gpu::Format::RGBA_8) 
      for(uint32 i=0; i<uint32(DIBHeader.width * DIBHeader.height); ++i)
         {
         uint8 temp    = buffer[i*4];
         buffer[i*4]   = buffer[i*4+2]; // R <- B
         buffer[i*4+2] = temp;          // B -> R
         }

   file->write(header.dataOffset, dataSize, buffer);
   delete [] buffer;
   return true;
   }

   bool save(const uint32 width, const uint32 height, const uint8* ptr, const string& filename)
   {
   using namespace en::storage;

   if ( ( width == 0 )   ||
        ( height == 0 )  ||
        ( ptr == nullptr) )
      return false;

   // Open image file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file, en::storage::Write))
      if (!Storage.open(en::ResourcesContext.path.screenshots + filename, &file, en::storage::Write))
         {
         Log << en::ResourcesContext.path.screenshots + filename << endl;
         Log << "ERROR: Cannot create such file!\n";
         return false;
         }
  
   uint32 headersSize = sizeof(Header) + sizeof(DIBHeaderOS2);
   uint32 dataSize    = width * height * 3;

   // Write file headers
   Header header;
   header.signature  = 0x4D42;  
   header.size       = headersSize + dataSize;
   header.reserved   = 0;       
   header.dataOffset = headersSize;
   file->write(0, 14, &header);

   DIBHeaderOS2 DIBHeader;
   DIBHeader.headerSize = sizeof(DIBHeaderOS2);
   DIBHeader.width      = width;
   DIBHeader.height     = height;
   DIBHeader.planes     = 1;
   DIBHeader.bpp        = 24;
   file->write(14, sizeof(DIBHeaderOS2), &DIBHeader);
 
   file->write(header.dataOffset, dataSize, (void*)(ptr));
   return true;
   }

   }
}
