/*

 Ngine v5.0
 
 Module      : PNG file support
 Requirements: none
 Description : Supports reading images from *.png files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "utilities/utilities.h"
#include "resources/context.h"
#include "resources/png.h"

#include "core/rendering/device.h"

#if defined(EN_PLATFORM_WINDOWS)
#include "zlib-1.2.8/zlib.h"
#endif
#if defined(EN_PLATFORM_OSX)
#include "zlib.h"
#endif

#include "platform/system.h"       // For CPU cores count
using namespace en::system;
#include "threading/scheduler.h"   // For multithreaded decode
using namespace en::threads;

#ifdef EN_PROFILE
#include "utilities/timer.h"
#endif

#include <string>
using namespace std;

namespace en
{
   namespace png
   {
   // Signature
   aligned(1) 
   struct Header
          {
          uint32 signature;   // PNG file signature '.PNG' -> 474E5089 
          uint32 eof;         // Win/Unix EOL and EOF      -> 0A1A0A0D
          };

   // First chunk
   struct IHDR
          {
          uint32      length;      // Chunk data length
          uint32      signature;   // Chunk signature -> 82687273
          uint32      width;       // Image width
          uint32      height;      // Image height
          uint8       bps;         // Bits per sample: 1,2,4,8,16
          uint8       type;        // Image type: 0,2,3,4,6
          uint8       compression; // Compression type
          uint8       filter;      // Filter
          uint8       interlace;   // Interlace (not supported)
          uint32      crc;         // CRC32
          }; 
   aligndefault

   bool CheckError(sint32 code)
   {
   if (code == Z_OK)
      return false;

   if (code > 0)
      return false;

   if (code == Z_ERRNO)
      Log << "Error: Zlib cannot read chunk!\n";
   else
   if (code == Z_STREAM_ERROR)
      Log << "Error: Zlib invalid compression level!\n";
   else
   if (code == Z_DATA_ERROR)
      Log << "Error: Zlib invalid or incomplete chunk!\n";
   else
   if (code == Z_MEM_ERROR)
      Log << "Error: Zlib reports out of memory!\n";
   else
   if (code == Z_BUF_ERROR)
      Log << "Error: Zlib output buffer is to small!\n";
   else
   if (code == Z_VERSION_ERROR)
      Log << "Error: Zlib version mismatch!\n";
   else
      Log << "Error: Zlib unknown error code " << code << "!\n";

   return true;
   }

   // Alan W. Paeth filtering predictor, for more details read:
   // "Image File Compression Made Easy", in Graphics Gems II, James Arvo, editor. Academic Press, San Diego, 1991. ISBN 0-12-064480-0.
   uint8 Paeth(sint16 a, sint16 b, sint16 c)
   {
   sint16 p = a + b - c;
   sint16 pa = p > a ? (p - a) : (a - p);
   sint16 pb = p > b ? (p - b) : (b - p);
   sint16 pc = p > c ? (p - c) : (c - p);
   if ((pa <= pb) && (pa <= pc)) 
      return static_cast<uint8>(a);
   if (pb <= pc) 
      return static_cast<uint8>(b);
   return static_cast<uint8>(c);
   }

   struct DecodeState
      {
      uint32 startLine;      // Cannot be decoded with Paeth !
      uint32 lines;          // Lines to decode
      uint32 texelSize;      //
      uint32 width;          //
      uint32 height;         // Image height
      uint8* input;          // Input buffer
      uint8* output;         // Output buffer
      bool invertHorizontal; // Should invert the image ?
      };

   class Worker : public Task
      { 
      private:
      uint32 startLine;      // Cannot be decoded with Paeth !
      uint32 lines;          // Lines to decode
      uint32 texelSize;      //
      uint32 width;          //
      uint32 height;         // Image height
      uint8* input;          // Input buffer
      uint8* output;         // Output buffer
      bool invertHorizontal; // Should invert the image ?
   
      public:  
      Worker(const DecodeState& state);  
      void work(void);     
      }; 

   Worker::Worker(const DecodeState& state) :
      startLine(state.startLine),
      lines(state.lines),
      texelSize(state.texelSize),
      width(state.width),
      height(state.height),
      input(state.input),
      output(state.output),
      invertHorizontal(state.invertHorizontal),
      Task(new TaskState())
   {
   }
   
   void Worker::work(void)
   {
   // Revert filters line by line
   uint32 lineSize   = width * texelSize;
   uint32 inOffset   = startLine * (lineSize + 1);
   uint32 outOffset  = startLine * lineSize;
   uint32 prevOffset = 0;
   for(uint32 i=startLine; i<(startLine + lines); ++i)
      {
      // Offset to previously decoded line
      if (i > 0)
         {
         prevOffset = (i - 1) * lineSize;
         if (invertHorizontal)
            prevOffset = (height - i) * lineSize;
         }
      if (invertHorizontal)
         outOffset = (height - i - 1) * lineSize;

      // Detect which type of filter was used for this line
      uint8 type = input[inOffset];
      inOffset++;

      // Filter type 0 - None
      if (type == 0)
         {
         for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
            output[outOffset] = input[inOffset]; 
         }
      // Filter type 1 - Sub
      if (type == 1)
         {
         // First pixel adds bytes outside scanline (equal 0) so just copy it
         for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
            output[outOffset] = input[inOffset];

         // Add bytes of previous pixel to reconstruct current one
         for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
            output[outOffset] = input[inOffset] + output[outOffset - texelSize];
         }
      // Filter type 2 - Up
      if (type == 2)
         {
         // If this is first scanline, above are only zeroes, so just copy it
         if (i == 0) 
            for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
               output[outOffset] = input[inOffset]; 
         else 
            // Add bytes of above pixel to reconstruct current one
            for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
               output[outOffset] = input[inOffset] + output[prevOffset]; 
         }
      // Filter type 3 - Average
      if (type == 3)
         {
         // If this is first scanline, above are only zeroes
         if (i == 0) 
            {
            // If this is first pixel, average will be zero, so just copy it
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
               output[outOffset] = input[inOffset];

            // Average from A and zero is A divided by two
            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
               output[outOffset] = input[inOffset] + output[outOffset - texelSize] / 2;
            }
         else
            {
            // Average from B and zero is B divided by two
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
               output[outOffset] = input[inOffset] + output[prevOffset] / 2;

            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
               output[outOffset] = input[inOffset] + ((output[outOffset - texelSize] + output[prevOffset]) / 2);
            }
         }
      // Filter type 4 - Paeth
      if (type == 4)
         {
         // If this is first scanline, above are only zeroes
         if (i == 0) 
            {
            // If this is first pixel, previous is zero, so just copy it
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
               output[outOffset] = input[inOffset];

            // Do partial Paeth with zeroes
            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
               output[outOffset] = input[inOffset] + Paeth(output[outOffset - texelSize], 0, 0);
            }
         else
            {
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
               output[outOffset] = input[inOffset] + Paeth(0, output[prevOffset], 0);

            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
               output[outOffset] = input[inOffset] + Paeth(output[outOffset - texelSize], output[prevOffset], output[prevOffset - texelSize]);
            }
         }
      }
   }

   bool load(Ptr<en::gpu::Texture> dst,
             const uint16 layer,
             const string& filename,
             const gpu::ColorSpace colorSpace,
             bool invertHorizontal)
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
 
   // Read file signature
   Header signature;
   file->read(0, 8, &signature);
   if ( signature.signature != 0x474E5089 ||
        signature.eof       != 0x0A1A0A0D )
      {
      Log << "ERROR: PNG file header signature is incorrect!\n";
      delete file;
      return false;
      }

   // Read file header
   IHDR header;
   file->read(8, 25, &header);
   if ( header.length != endiannes(uint32(13)) ||
        header.signature != 0x52444849 )
      {
      Log << "ERROR: PNG file IHDR signature is incorrect!\n";
      delete file;
      return false;
      }

   // Check compression
   if (header.compression != 0)
      {
      Log << "ERROR: This PNG compression method is not supported!\n";
      delete file;
      return false;
      }

   // Check filtering before compression
   if (header.filter != 0)
      {
      Log << "ERROR: This PNG filtering method is not supported!\n";
      delete file;
      return false;
      }

   // Check if image is not interlaced
   if (header.interlace != 0)
      {
      Log << "ERROR: Interlaced PNG files are not supported!\n";
      delete file;
      return false;
      }

   // Determine texture parameters
   gpu::TextureState settings;
   settings.width  = endiannes(header.width);
   settings.height = endiannes(header.height);
   if (header.type == 0 && header.bps == 8)
      settings.format = gpu::Format::R_8;
   else
   if (header.type == 0 && header.bps == 16)
      settings.format = gpu::Format::R_16;
   else
   if (header.type == 2 && header.bps == 8)
      {
      settings.format = gpu::Format::BGR_8;      // Should be RGB_8 but Windows software saves PNG's in BGR_8 (LO->HI)
      if (colorSpace == gpu::ColorSpaceSRGB)
         settings.format = gpu::Format::BGR_8_sRGB;
      }
   else
   if (header.type == 2 && header.bps == 16)
      settings.format = gpu::Format::RGB_16;     
   else
   if (header.type == 4 && header.bps == 8)
      settings.format = gpu::Format::RG_8;         
   else
   if (header.type == 4 && header.bps == 16)
      settings.format = gpu::Format::RG_16;
   else
   if (header.type == 6 && header.bps == 8)
      settings.format = gpu::Format::RGBA_8;      // RGB / RGBA (LO->HI) according to http://www.w3.org/TR/PNG/#4Concepts.PNGImage
   //else
   //if (header.type == 6 && header.bps == 16)
   //   settings.format = gpu::Format::RGBA_16;
   else
      {
      Log << "ERROR: Unsupported texture format!\n";
      delete file;
      return false;
      }

   // Determine texture type
   settings.type    = gpu::TextureType::Texture2D;
   if (settings.height == 1)
      settings.type = gpu::TextureType::Texture1D;
   //if (!powerOfTwo(settings.width) ||
   //    !powerOfTwo(settings.height) )
   //   settings.type = gpu::TextureType::Texture2DRectangle;

   // Check if image can be loaded to texture
   if (dst->type() != gpu::TextureType::Texture1DArray            &&
       dst->type() != gpu::TextureType::Texture2DArray            &&
       dst->type() != gpu::TextureType::Texture2DMultisampleArray &&
       dst->type() != gpu::TextureType::TextureCubeMap            &&
       dst->type() != gpu::TextureType::TextureCubeMapArray)
      return false;
   if (dst->type() == gpu::TextureType::TextureCubeMap &&
       layer > 5)
      return false;
   else
   if (dst->layers() < layer)
      return false;
   if (dst->width() != settings.width)
      return false;
   if (dst->height() != settings.height)
      return false;
   if (dst->format() != settings.format)
      return false;

   // Alloc buffers for IDAT chunks data and decompressed stream
   uint32 size     = (settings.width * settings.height * Graphics->primaryDevice()->texelSize(settings.format));
   uint8* input    = new uint8[size + settings.height];
   uint8* inflated = new uint8[size + settings.height];
   uint32 seek     = 0;

   // Read chunks in loop until all are processed
   uint32 offset = 33;
   uint32 length = 0;
   uint32 name = 0;
   while(name != 0x444E4549)  // 'IEND' -> 73 69 78 68 -> 0x 49 45 4E 44 -> 0x444E4549
        {
        // Read chunk length
        file->read(offset, 4, &length);
        length = endiannes(length);
        offset += 4;

        // Read chunk name
        file->read(offset, 4, &name);
        offset += 4;

        // If ending chunk break
        if (name == 0x444E4549)
           break;
           
        // Read compressed chunk for processing
        if (name == 0x54414449) // 'IDAT' -> 73 68 65 84 -> 0x 49 44 41 54 -> 0x54414449
           {
           file->read(offset, length, &input[seek]);
           seek += length;
           }

        //// Read advanced compression method chunk
        //if (name == 0x50434369) // 'iCCP' -> 105 67 67 80 -> 0x 69 43 43 50 -> 0x50434369
        //   {
        //   Log << "WARNING: iCCP chunk found with info:" << endl;
        //   
        //   char profile[80];
        //   uint8 i = 0;
        //   for(; i<80; ++i)
        //      {
        //      file->read(offset + i, 1, &profile[i]);
        //      if (profile[i] == 0)
        //         break;
        //      }
        //   Log << "Profile name: " << profile << endl;
        //   }

        //// Read chrominance chunk
        //if (name == 0x4D524863) // 'cHRM' -> 99 72 82 77 -> 0x 63 48 52 4D -> 0x4D524863
        //   {
        //   Log << "WARNING: cHRM chunk found with info:" << endl;
        //   
        //   float2 white;
        //   float2 red;
        //   float2 green;
        //   float2 blue;

        //   uint32 value;

        //   // Read white base of chrominance 
        //   file->read(offset,   4, &value);
        //   white.x = (float)value/ 100000.0;
        //   file->read(offset+4, 4, &value);
        //   white.y = (float)value/ 100000.0;

        //   // Read red chrominance
        //   file->read(offset+8,  4, &value);
        //   red.x = (float)value/ 100000.0;
        //   file->read(offset+12, 4, &value);
        //   red.y = (float)value/ 100000.0;

        //   // Read green chrominance
        //   file->read(offset+16,  4, &value);
        //   green.x = (float)value/ 100000.0;
        //   file->read(offset+20, 4, &value);
        //   green.y = (float)value/ 100000.0;

        //   // Read blue chrominance
        //   file->read(offset+24,  4, &value);
        //   blue.x = (float)value/ 100000.0;
        //   file->read(offset+28, 4, &value);
        //   blue.y = (float)value/ 100000.0;

        //   Log << "Chrominance white x=" << white.x << " y=" << white.y << endl;
        //   Log << "Chrominance red   x=" << red.x   << " y=" << red.y   << endl;
        //   Log << "Chrominance green x=" << green.x << " y=" << green.y << endl;
        //   Log << "Chrominance blue  x=" << blue.x  << " y=" << blue.y  << endl;
        //   }

        // Seek to next chunk
        offset += (length + 4);
        };
   delete file;

   // Create zlib stream structure
   z_stream stream;
   stream.zalloc    = Z_NULL;
   stream.zfree     = Z_NULL;
   stream.opaque    = Z_NULL;
   stream.next_in   = input;                         // Compressed data
   stream.avail_in  = seek;                          // Size of compressed data
   stream.avail_out = size + settings.height;        // Available space for uncompressed data
   stream.next_out  = static_cast<uint8*>(inflated); // Output destination

   // Decompress data from IDAT chunks to 'inflated' buffer
   if (CheckError(inflateInit(&stream)))
      {
      Log << "Error: Cannot initialize Zlib decompressor!\n";
      return false;
      }
   sint32 ret = 0;
   ret = inflate(&stream, Z_FINISH);
   if (ret != Z_STREAM_END)
      {
      CheckError(ret);
      Log << "Error: Cannot decompress using ZLIB!\n";
      return false;
      }
   inflateEnd(&stream);
   delete [] input;

   // Create staging buffer
   Ptr<gpu::Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, dst->size());
   if (!staging)
      {
      Log << "ERROR: Cannot create staging buffer!\n";
      return false;
      }

   // Decompress image directly to texture layer in gpu memory
   void* ptr = staging->map();

   // Revert filters line by line
   uint8* buffer     = static_cast<uint8*>(ptr);
   uint32 texelSize  = Graphics->primaryDevice()->texelSize(settings.format);
   uint32 lineSize   = settings.width * texelSize;
   uint32 inOffset   = 0;
   uint32 outOffset  = 0;
   uint32 prevOffset = 0;
   for(uint32 i=0; i<settings.height; ++i)
      {
      // Offset to previously decoded line
      if (i > 0)
         {
         prevOffset = (i - 1) * lineSize;
         if (invertHorizontal)
            prevOffset = (settings.height - i) * lineSize;
         }
      if (invertHorizontal)
         outOffset = (settings.height - i - 1) * lineSize;

      // Detect which type of filter was used for this line
      uint8 type = inflated[inOffset];
      inOffset++;

      // Filter type 0 - None
      if (type == 0)
         {
         for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
            buffer[outOffset] = inflated[inOffset]; 
         }
      // Filter type 1 - Sub
      if (type == 1)
         {
         // First pixel adds bytes outside scanline (equal 0) so just copy it
         for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
            buffer[outOffset] = inflated[inOffset];

         // Add bytes of previous pixel to reconstruct current one
         for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
            buffer[outOffset] = inflated[inOffset] + buffer[outOffset - texelSize];
         }
      // Filter type 2 - Up
      if (type == 2)
         {
         // If this is first scanline, above are only zeroes, so just copy it
         if (i == 0) 
            for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
               buffer[outOffset] = inflated[inOffset]; 
         else 
            // Add bytes of above pixel to reconstruct current one
            for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
               buffer[outOffset] = inflated[inOffset] + buffer[prevOffset]; 
         }
      // Filter type 3 - Average
      if (type == 3)
         {
         // If this is first scanline, above are only zeroes
         if (i == 0) 
            {
            // If this is first pixel, average will be zero, so just copy it
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
               buffer[outOffset] = inflated[inOffset];

            // Average from A and zero is A divided by two
            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
               buffer[outOffset] = inflated[inOffset] + buffer[outOffset - texelSize] / 2;
            }
         else
            {
            // Average from B and zero is B divided by two
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
               buffer[outOffset] = inflated[inOffset] + buffer[prevOffset] / 2;

            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
               buffer[outOffset] = inflated[inOffset] + ((buffer[outOffset - texelSize] + buffer[prevOffset]) / 2);
            }
         }
      // Filter type 4 - Paeth
      if (type == 4)
         {
         // If this is first scanline, above are only zeroes
         if (i == 0) 
            {
            // If this is first pixel, previous is zero, so just copy it
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
               buffer[outOffset] = inflated[inOffset];

            // Do partial Paeth with zeroes
            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
               buffer[outOffset] = inflated[inOffset] + Paeth(buffer[outOffset - texelSize], 0, 0);
            }
         else
            {
            for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
               buffer[outOffset] = inflated[inOffset] + Paeth(0, buffer[prevOffset], 0);

            for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
               buffer[outOffset] = inflated[inOffset] + Paeth(buffer[outOffset - texelSize], buffer[prevOffset], buffer[prevOffset - texelSize]);
            }
         }
      }

   delete [] inflated;
   staging->unmap();
   
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
   
   return true;
   }




   //bool load(Ptr<en::gpu::Texture> dst, const gpu::TextureFace face, const uint16 layer, const string& filename, const gpu::ColorSpace colorSpace, bool invertHorizontal)
   //{
   //using namespace en::storage;

   //// Open image file 
   //Nfile* file = NULL;
   //if (!Storage.open(filename, &file))
   //   if (!Storage.open(en::ResourcesContext.path.textures + filename, &file))
   //      {
   //      Log << en::ResourcesContext.path.textures + filename << endl;
   //      Log << "ERROR: There is no such file!\n";
   //      return false;
   //      }
 
   //// Read file signature
   //Header signature;
   //file->read(0, 8, &signature);
   //if ( signature.signature != 0x474E5089 ||
   //     signature.eof       != 0x0A1A0A0D )
   //   {
   //   Log << "ERROR: PNG file header signature is incorrect!\n";
   //   delete file;
   //   return false;
   //   }

   //// Read file header
   //IHDR header;
   //file->read(8, 25, &header);
   //if ( header.length != endiannes(uint32(13)) ||
   //     header.signature != 0x52444849 )
   //   {
   //   Log << "ERROR: PNG file IHDR signature is incorrect!\n";
   //   delete file;
   //   return false;
   //   }

   //// Check compression
   //if (header.compression != 0)
   //   {
   //   Log << "ERROR: This PNG compression method is not supported!\n";
   //   delete file;
   //   return false;
   //   }

   //// Check filtering before compression
   //if (header.filter != 0)
   //   {
   //   Log << "ERROR: This PNG filtering method is not supported!\n";
   //   delete file;
   //   return false;
   //   }

   //// Check if image is not interlaced
   //if (header.interlace != 0)
   //   {
   //   Log << "ERROR: Interlaced PNG files are not supported!\n";
   //   delete file;
   //   return false;
   //   }

   //// Determine texture parameters
   //gpu::TextureState settings;
   //settings.type   = gpu::TextureCubeMap;
   //settings.width  = endiannes(header.width);
   //settings.height = endiannes(header.height);
   //if (header.type == 0 && header.bps == 8)
   //   settings.format = gpu::FormatR_8;
   //else
   //if (header.type == 0 && header.bps == 16)
   //   settings.format = gpu::FormatR_16;
   //else
   //if (header.type == 2 && header.bps == 8)
   //   {
   //   settings.format = gpu::FormatRGB_8;      // Should be BGR_8 but Windows software saves PNG's in RGB_8
   //   if (colorSpace == gpu::ColorSpaceSRGB)
   //      settings.format = gpu::FormatRGB_8_sRGB;
   //   }
   //else
   //if (header.type == 2 && header.bps == 16)
   //   settings.format = gpu::FormatBGR_16;
   //else
   //if (header.type == 4 && header.bps == 8)
   //   settings.format = gpu::FormatRG_8;          // Should it be GR for PNG ?
   //else
   //if (header.type == 4 && header.bps == 16)
   //   settings.format = gpu::FormatRG_16;
   //else
   //if (header.type == 6 && header.bps == 8)
   //   settings.format = gpu::FormatABGR_8;      // RGB / RGBA according to http://www.w3.org/TR/PNG/#4Concepts.PNGImage
   ////else
   ////if (header.type == 6 && header.bps == 16)
   ////   settings.format = gpu::FormatABGR_16;
   //else
   //   {
   //   Log << "ERROR: Unsupported texture format!\n";
   //   delete file;
   //   return false;
   //   }

   //// Check if image can be loaded to texture
   //if (dst->type() != gpu::TextureCubeMap)
   //   return false;
   //if (settings.width != settings.height)
   //   return false;
   //if (dst->width() != settings.width)
   //   return false;
   //if (dst->format() != settings.format)
   //   return false;

   //// Alloc buffers for IDAT chunks data and decompressed stream
   //uint32 size     = (settings.width * settings.height * Gpu.texture.bitsPerTexel(settings.format)) / 8;
   //uint8* input    = new uint8[size + settings.height];
   //uint8* inflated = new uint8[size + settings.height];
   //uint32 seek     = 0;

   //// Read chunks in loop until all are processed
   //uint32 offset = 33;
   //uint32 length = 0;
   //uint32 name = 0;
   //while(name != 0x444E4549)  // 'IEND' -> 73 69 78 68 -> 0x 49 45 4E 44 -> 0x444E4549
   //     {
   //     // Read chunk length
   //     file->read(offset, 4, &length);
   //     length = endiannes(length);
   //     offset += 4;

   //     // Read chunk name
   //     file->read(offset, 4, &name);
   //     offset += 4;

   //     // If ending chunk break
   //     if (name == 0x444E4549)
   //        break;
   //        
   //     // Read compressed chunk for processing
   //     if (name == 0x54414449) // 'IDAT' -> 73 68 65 84 -> 0x 49 44 41 54 -> 0x54414449
   //        {
   //        file->read(offset, length, &input[seek]);
   //        seek += length;
   //        }

   //     //// Read advanced compression method chunk
   //     //if (name == 0x50434369) // 'iCCP' -> 105 67 67 80 -> 0x 69 43 43 50 -> 0x50434369
   //     //   {
   //     //   Log << "WARNING: iCCP chunk found with info:" << endl;
   //     //   
   //     //   char profile[80];
   //     //   uint8 i = 0;
   //     //   for(; i<80; ++i)
   //     //      {
   //     //      file->read(offset + i, 1, &profile[i]);
   //     //      if (profile[i] == 0)
   //     //         break;
   //     //      }
   //     //   Log << "Profile name: " << profile << endl;
   //     //   }

   //     //// Read chrominance chunk
   //     //if (name == 0x4D524863) // 'cHRM' -> 99 72 82 77 -> 0x 63 48 52 4D -> 0x4D524863
   //     //   {
   //     //   Log << "WARNING: cHRM chunk found with info:" << endl;
   //     //   
   //     //   float2 white;
   //     //   float2 red;
   //     //   float2 green;
   //     //   float2 blue;

   //     //   uint32 value;

   //     //   // Read white base of chrominance 
   //     //   file->read(offset,   4, &value);
   //     //   white.x = (float)value/ 100000.0;
   //     //   file->read(offset+4, 4, &value);
   //     //   white.y = (float)value/ 100000.0;

   //     //   // Read red chrominance
   //     //   file->read(offset+8,  4, &value);
   //     //   red.x = (float)value/ 100000.0;
   //     //   file->read(offset+12, 4, &value);
   //     //   red.y = (float)value/ 100000.0;

   //     //   // Read green chrominance
   //     //   file->read(offset+16,  4, &value);
   //     //   green.x = (float)value/ 100000.0;
   //     //   file->read(offset+20, 4, &value);
   //     //   green.y = (float)value/ 100000.0;

   //     //   // Read blue chrominance
   //     //   file->read(offset+24,  4, &value);
   //     //   blue.x = (float)value/ 100000.0;
   //     //   file->read(offset+28, 4, &value);
   //     //   blue.y = (float)value/ 100000.0;

   //     //   Log << "Chrominance white x=" << white.x << " y=" << white.y << endl;
   //     //   Log << "Chrominance red   x=" << red.x   << " y=" << red.y   << endl;
   //     //   Log << "Chrominance green x=" << green.x << " y=" << green.y << endl;
   //     //   Log << "Chrominance blue  x=" << blue.x  << " y=" << blue.y  << endl;
   //     //   }

   //     // Seek to next chunk
   //     offset += (length + 4);
   //     };
   //delete file;

   //// Create zlib stream structure
   //z_stream stream;
   //stream.zalloc    = Z_NULL;
   //stream.zfree     = Z_NULL;
   //stream.opaque    = Z_NULL;
   //stream.next_in   = input;                         // Compressed data
   //stream.avail_in  = seek;                          // Size of compressed data
   //stream.avail_out = size + settings.height;        // Available space for uncompressed data
   //stream.next_out  = static_cast<uint8*>(inflated); // Output destination

   //// Decompress data from IDAT chunks to 'inflated' buffer
   //if (CheckError(inflateInit(&stream)))
   //   {
   //   Log << "Error: Cannot initialize Zlib decompressor!\n";
   //   return false;
   //   }
   //sint32 ret = 0;
   //ret = inflate(&stream, Z_FINISH);
   //if (ret != Z_STREAM_END)
   //   {
   //   CheckError(ret);
   //   Log << "Error: Cannot decompress using ZLIB!\n";
   //   return false;
   //   }
   //inflateEnd(&stream);
   //delete input;

   //// Decompress image directly to texture layer in gpu memory
   //void* ptr = dst->map(face, 0, layer);

   //// Revert filters line by line
   //uint8* buffer     = static_cast<uint8*>(ptr);
   //uint32 texelSize  = Gpu.texture.bitsPerTexel(settings.format) / 8;
   //uint32 lineSize   = settings.width * texelSize;
   //uint32 inOffset   = 0;
   //uint32 outOffset  = 0;
   //uint32 prevOffset = 0;
   //for(uint32 i=0; i<settings.height; ++i)
   //   {
   //   // Offset to previously decoded line
   //   if (i > 0)
   //      {
   //      prevOffset = (i - 1) * lineSize;
   //      if (invertHorizontal)
   //         prevOffset = (settings.height - i) * lineSize;
   //      }
   //   if (invertHorizontal)
   //      outOffset = (settings.height - i - 1) * lineSize;

   //   // Detect which type of filter was used for this line
   //   uint8 type = inflated[inOffset];
   //   inOffset++;

   //   // Filter type 0 - None
   //   if (type == 0)
   //      {
   //      for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
   //         buffer[outOffset] = inflated[inOffset]; 
   //      }
   //   // Filter type 1 - Sub
   //   if (type == 1)
   //      {
   //      // First pixel adds bytes outside scanline (equal 0) so just copy it
   //      for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
   //         buffer[outOffset] = inflated[inOffset];

   //      // Add bytes of previous pixel to reconstruct current one
   //      for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
   //         buffer[outOffset] = inflated[inOffset] + buffer[outOffset - texelSize];
   //      }
   //   // Filter type 2 - Up
   //   if (type == 2)
   //      {
   //      // If this is first scanline, above are only zeroes, so just copy it
   //      if (i == 0) 
   //         for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
   //            buffer[outOffset] = inflated[inOffset]; 
   //      else 
   //         // Add bytes of above pixel to reconstruct current one
   //         for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
   //            buffer[outOffset] = inflated[inOffset] + buffer[prevOffset]; 
   //      }
   //   // Filter type 3 - Average
   //   if (type == 3)
   //      {
   //      // If this is first scanline, above are only zeroes
   //      if (i == 0) 
   //         {
   //         // If this is first pixel, average will be zero, so just copy it
   //         for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
   //            buffer[outOffset] = inflated[inOffset];

   //         // Average from A and zero is A divided by two
   //         for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
   //            buffer[outOffset] = inflated[inOffset] + buffer[outOffset - texelSize] / 2;
   //         }
   //      else
   //         {
   //         // Average from B and zero is B divided by two
   //         for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
   //            buffer[outOffset] = inflated[inOffset] + buffer[prevOffset] / 2;

   //         for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
   //            buffer[outOffset] = inflated[inOffset] + ((buffer[outOffset - texelSize] + buffer[prevOffset]) / 2);
   //         }
   //      }
   //   // Filter type 4 - Paeth
   //   if (type == 4)
   //      {
   //      // If this is first scanline, above are only zeroes
   //      if (i == 0) 
   //         {
   //         // If this is first pixel, previous is zero, so just copy it
   //         for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset) 
   //            buffer[outOffset] = inflated[inOffset];

   //         // Do partial Paeth with zeroes
   //         for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
   //            buffer[outOffset] = inflated[inOffset] + Paeth(buffer[outOffset - texelSize], 0, 0);
   //         }
   //      else
   //         {
   //         for(uint8 j=0; j<texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
   //            buffer[outOffset] = inflated[inOffset] + Paeth(0, buffer[prevOffset], 0);

   //         for(uint32 j=texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
   //            buffer[outOffset] = inflated[inOffset] + Paeth(buffer[outOffset - texelSize], buffer[prevOffset], buffer[prevOffset - texelSize]);
   //         }
   //      }
   //   }

   //delete inflated;
   //return dst->unmap();
   //}




   Ptr<en::gpu::Texture> load(const string& filename, const gpu::ColorSpace colorSpace, const bool invertHorizontal)
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
 
   // Read file signature
   Header signature;
   file->read(0, 8, &signature);
   if ( signature.signature != 0x474E5089 ||
        signature.eof       != 0x0A1A0A0D )
      {
      Log << "ERROR: PNG file header signature is incorrect!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Read file header
   IHDR header;
   file->read(8, 25, &header);
   if ( header.length != endiannes(uint32(13)) ||
        header.signature != 0x52444849 )
      {
      Log << "ERROR: PNG file IHDR signature is incorrect!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Check compression
   if (header.compression != 0)
      {
      Log << "ERROR: This PNG compression method is not supported!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Check filtering before compression
   if (header.filter != 0)
      {
      Log << "ERROR: This PNG filtering method is not supported!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Check if image is not interlaced
   if (header.interlace != 0)
      {
      Log << "ERROR: Interlaced PNG files are not supported!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Determine texture parameters
   gpu::TextureState settings;
   settings.width  = endiannes(header.width);
   settings.height = endiannes(header.height);
   if (header.type == 0 && header.bps == 8)
      settings.format = gpu::Format::R_8;
   else
   if (header.type == 0 && header.bps == 16)
      settings.format = gpu::Format::R_16;
   else
   if (header.type == 2 && header.bps == 8)
      {
      settings.format = gpu::Format::BGR_8;         // Window
      if (colorSpace == gpu::ColorSpaceSRGB)
         settings.format = gpu::Format::BGR_8_sRGB; // Windows
      }
   else
   if (header.type == 2 && header.bps == 16)
      settings.format = gpu::Format::RGB_16;
   else
   if (header.type == 4 && header.bps == 8)
      settings.format = gpu::Format::RG_8;      
   else
   if (header.type == 4 && header.bps == 16)
      settings.format = gpu::Format::RG_16;
   else
   if (header.type == 6 && header.bps == 8)
      settings.format = gpu::Format::RGBA_8;    // RGB / RGBA according to http://www.w3.org/TR/PNG/#4Concepts.PNGImage
   //else
   //if (header.type == 6 && header.bps == 16)
   //   settings.format = gpu::Format::RGBA_16;
   else
      {
      Log << "ERROR: Unsupported texture format!\n";
      delete file;
      return Ptr<gpu::Texture>(nullptr);
      }

   // Determine texture type
   settings.type    = gpu::TextureType::Texture2D;
   if (settings.height == 1)
      settings.type = gpu::TextureType::Texture1D;
   //if (!powerOfTwo(settings.width) ||
   //    !powerOfTwo(settings.height) )
   //   settings.type = gpu::Texture2DRectangle;

   // Alloc buffers for IDAT chunks data and decompressed stream
   uint32 size     = (settings.width * settings.height * Graphics->primaryDevice()->texelSize(settings.format));
   uint8* input    = new uint8[size + settings.height];
   uint8* inflated = new uint8[size + settings.height];
   uint32 seek     = 0;

   // Read chunks in loop until all are processed
   uint32 offset = 33;
   uint32 length = 0;
   uint32 name = 0;
   while(name != 0x444E4549)  // 'IEND' -> 73 69 78 68 -> 0x 49 45 4E 44 -> 0x444E4549
        {
        // Read chunk length
        file->read(offset, 4, &length);
        length = endiannes(length);
        offset += 4;

        // Read chunk name
        file->read(offset, 4, &name);
        offset += 4;

        // If ending chunk break
        if (name == 0x444E4549)
           break;
           
        // Read compressed chunk for processing
        if (name == 0x54414449) // 'IDAT' -> 73 68 65 84 -> 0x 49 44 41 54 -> 0x54414449
           {
           file->read(offset, length, &input[seek]);
           seek += length;
           }

        //// Read advanced compression method chunk
        //if (name == 0x50434369) // 'iCCP' -> 105 67 67 80 -> 0x 69 43 43 50 -> 0x50434369
        //   {
        //   Log << "WARNING: iCCP chunk found with info:" << endl;
        //   
        //   char profile[80];
        //   uint8 i = 0;
        //   for(; i<80; ++i)
        //      {
        //      file->read(offset + i, 1, &profile[i]);
        //      if (profile[i] == 0)
        //         break;
        //      }
        //   Log << "Profile name: " << profile << endl;
        //   }

        //// Read chrominance chunk
        //if (name == 0x4D524863) // 'cHRM' -> 99 72 82 77 -> 0x 63 48 52 4D -> 0x4D524863
        //   {
        //   Log << "WARNING: cHRM chunk found with info:" << endl;
        //   
        //   float2 white;
        //   float2 red;
        //   float2 green;
        //   float2 blue;

        //   uint32 value;

        //   // Read white base of chrominance 
        //   file->read(offset,   4, &value);
        //   white.x = (float)value/ 100000.0;
        //   file->read(offset+4, 4, &value);
        //   white.y = (float)value/ 100000.0;

        //   // Read red chrominance
        //   file->read(offset+8,  4, &value);
        //   red.x = (float)value/ 100000.0;
        //   file->read(offset+12, 4, &value);
        //   red.y = (float)value/ 100000.0;

        //   // Read green chrominance
        //   file->read(offset+16,  4, &value);
        //   green.x = (float)value/ 100000.0;
        //   file->read(offset+20, 4, &value);
        //   green.y = (float)value/ 100000.0;

        //   // Read blue chrominance
        //   file->read(offset+24,  4, &value);
        //   blue.x = (float)value/ 100000.0;
        //   file->read(offset+28, 4, &value);
        //   blue.y = (float)value/ 100000.0;

        //   Log << "Chrominance white x=" << white.x << " y=" << white.y << endl;
        //   Log << "Chrominance red   x=" << red.x   << " y=" << red.y   << endl;
        //   Log << "Chrominance green x=" << green.x << " y=" << green.y << endl;
        //   Log << "Chrominance blue  x=" << blue.x  << " y=" << blue.y  << endl;
        //   }

        // Seek to next chunk
        offset += (length + 4);
        };
   delete file;


#ifdef EN_PROFILE
   Timer timer;
   timer.start();
#endif

   // Create zlib stream structure
   z_stream stream;
   stream.zalloc    = Z_NULL;
   stream.zfree     = Z_NULL;
   stream.opaque    = Z_NULL;
   stream.next_in   = input;                         // Compressed data
   stream.avail_in  = seek;                          // Size of compressed data
   stream.avail_out = size + settings.height;        // Available space for uncompressed data
   stream.next_out  = static_cast<uint8*>(inflated); // Output destination

   // Decompress data from IDAT chunks to 'inflated' buffer
   if (CheckError(inflateInit(&stream)))
      {
      Log << "Error: Cannot initialize Zlib decompressor!\n";
      return Ptr<gpu::Texture>(nullptr);
      }
   sint32 ret = 0;
   ret = inflate(&stream, Z_FINISH);
   if (ret != Z_STREAM_END)
      {
      CheckError(ret);
      Log << "Error: Cannot decompress using ZLIB!\n";
      return Ptr<gpu::Texture>(nullptr);
      }
   inflateEnd(&stream);
   delete [] input;

#ifdef EN_PROFILE
   Log << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - PNG ZLIB inflate time" << endl;
#endif


   // Create texture in gpu
   Ptr<gpu::Texture> texture = en::ResourcesContext.defaults.enHeap->createTexture(settings);
   if (!texture)
      {
      Log << "ERROR: Cannot create texture in GPU!\n";
      return Ptr<gpu::Texture>(nullptr);
      }

   // Create staging buffer
   Ptr<gpu::Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, texture->size());
   if (!staging)
      {
      Log << "ERROR: Cannot create staging buffer!\n";
      delete file;
      return texture;
      }
   
   // Decompress texture directly to gpu memory
   void* dst = staging->map();

   // If possible revert filters in paraller using several worker threads
   DecodeState decoder;
   decoder.startLine        = 0;
   decoder.lines            = settings.height;
   decoder.texelSize        = Graphics->primaryDevice()->texelSize(settings.format);
   decoder.width            = settings.width;
   decoder.height           = settings.height;
   decoder.input            = inflated;
   decoder.output           = static_cast<uint8*>(dst);
   decoder.invertHorizontal = invertHorizontal;

   uint32 cores = Scheduler.workers();
   if (cores == 1) // TODO: In future add minimum amount of lines, below which we don't use task pool
      {
      TaskState* state = Scheduler.run(new Worker(decoder), true); // Execute decoding on current thread
      }
   else
      {
      // Try to disect image lines between the cores
      TaskState* state[32]; // Handles of up to 32 simultaneous decoding threads

      decoder.startLine = 0;
      decoder.lines     = 0;

      uint32 task=0;
      uint32 linesPerCore = settings.height / cores;
      for(; task<cores; ++task)
         {
         // Calculate lines range for current core
         decoder.startLine += decoder.lines;
         decoder.lines      = linesPerCore;

         // Check if first line of next section is not compressed with pattern
         // 2,3 or 4. In such situation increase curren lines range to prevent
         // data relation between the thread blocks (these patterns use previous line). 
         uint8 type = 4;
         bool lastTask = false;
         while(type > 1)
            {
            // Check if current block is not already reaching end of image
            uint32 nextLine = decoder.startLine + decoder.lines;
            if (nextLine >= settings.height)
               {
               decoder.lines = settings.height - decoder.startLine;
               lastTask = true;
               break;
               }

            // Check if next block will use "Paeth"
            uint32 inOffset = nextLine * (decoder.width * decoder.texelSize + 1);
            uint8 type = inflated[inOffset];
            if (type < 2)
               break;

            decoder.lines++;
            }
         
         // Spawn task to decompress image parts,
         // starting from last core and ending on this one.
         state[task] = Scheduler.run(new Worker(decoder), (cores - task - 1), false);
         if (lastTask) 
            break;
         }

      // TODO: Fix proper handling of tasks and their states pointers
      //       If task is deleted by scheduler after it is finished, how to verify it's completion? Who should delete TaskState ? 
      // Barrier - wait until all threads will finish execution
      //for(uint8 i=0; i<cores; ++i)
      //   Scheduler.wait(state[cores - i - 1]);
      }
      
   staging->unmap();
   delete [] inflated;
    
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
    
   return texture;
   }

   }
}
