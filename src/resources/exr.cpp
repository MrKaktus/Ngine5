/*

 Ngine v5.0
 
 Module      : EXR files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from *.exr files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "utilities/utilities.h"
#include "resources/context.h"
#include "resources/exr.h"

#include "core/rendering/device.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#include "zlib.h"
#endif

#include <cstddef>
#include <string>
using namespace std;

namespace en
{
   namespace exr
   {
   enum Compression
        {
        None          = 0,  // Uncompressed
        RLE           = 1,  // Lossless (all types)
        ZIPS          = 2,  // Lossless (all types)
        ZIP           = 3,  // Lossless (all types)
        PIZ           = 4,  // Lossless (no deep data)
        PXR24         = 5,  // Lossy    (no deep data)
        B44           = 6,  // Lossy    (no deep data)
        B44A          = 7   // Lossy    (no deep data)
        };

   enum PartType
        {
        ScanLineImage = 0,
        TiledImage       ,
        DeepScanLine     ,
        DeepTile       
        };

   enum Environment
        {
        LatitudeLongitude = 0,
        CubeMap           = 1
        };

   aligned(1)
   struct Header
          {
          uint32 signature; // EXR file signature 0x01312F76 (Little Endian)

          uint32 version      : 8;
          uint32 singleTile   : 1;  // Is single part file with tiles ?
          uint32 longNames    : 1;  // Does file contain long names?
          uint32 containsData : 1;  // Is file containing non image data ?
          uint32 multiPart    : 1;  // Does file contain multiple parts ?
          uint32 reserved     : 20; // Must be zero
          };
   aligndefault

   struct Channel
          {
          string name;              // 1-255 length
          uint32 type;              // Pixel type: 0-uint32 1-half 2-float
          uint8  pLinear;
          uint32 xSampling;
          uint32 ySampling;
          };

   struct PartHeader
          {
          uint32v4 displayWindow;
          uint32v4 dataWindow;
          float2   screenWindowCenter;
          float    screenWindowWidth;
          float    pixelAspect;        // Should be 1.0
          Channel* channel;            // Channels description array
          uint8    channels;           // Channels count
          uint8    compression;
          uint8    lineOrder;
          string   commenet;
          string   name;
          PartType type;
          uint32   version;
          sint32   chunkCount;
          uint32   maxSamplesPerPixel;

          PartHeader();
          };

   PartHeader::PartHeader() :
      displayWindow(),
      dataWindow(),
      screenWindowCenter(),
      screenWindowWidth(0.0f),
      pixelAspect(1.0f),
      channel(nullptr),
      channels(0),
      compression(0),
      lineOrder(0),
      commenet(""),
      name(""),
      type(ScanLineImage),
      version(1),
      chunkCount(-1),
      maxSamplesPerPixel(1)
   {
   }

   struct HeaderAttribute
          {
          const char*  name;   // Attribute name as null terminated string
          const char*  type;   // Attribute type as null terminated string
          uint32 size;   // Size of data
          uint32 offset; // Offset in PartHeader structure
          };

   const HeaderAttribute attribute[] = {
      // Name                  Type          Size   Offset
      { "displayWindow",      "box2i",       16, offsetof(PartHeader, displayWindow) },
      { "dataWindow",         "box2i",       16, offsetof(PartHeader, dataWindow) },
      { "pixelAspectRatio",   "float",       4,  offsetof(PartHeader, pixelAspect) },
      { "compression",        "compression", 1,  offsetof(PartHeader, compression) },
      { "lineOrder",          "lineOrder",   1,  offsetof(PartHeader, lineOrder) },
      { "screenWindowWidth",  "float",       4,  offsetof(PartHeader, screenWindowWidth) },
      { "screenWindowCenter", "v2f",         8,  offsetof(PartHeader, screenWindowCenter) },
      { "version",            "int",         4,  offsetof(PartHeader, version) },
      { "chunkCount",         "int",         4,  offsetof(PartHeader, chunkCount) },
      { "maxSamplesPerPixel", "int",         4,  offsetof(PartHeader, maxSamplesPerPixel) }  };
      // Attributes of varying size need to be handled separately

   #define attributesCount ( sizeof(attribute) / sizeof(HeaderAttribute) )

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

   Ptr<en::gpu::Texture> load(const string& filename)
   {
   using namespace en::storage;

   // Open image file
   Ptr<File> file = Storage->open(filename);
   if (!file)
      {
      file = Storage->open(en::ResourcesContext.path.textures + filename);
      if (!file)
         {
         Log << en::ResourcesContext.path.textures + filename << endl;
         Log << "ERROR: There is no such file!\n";
         return Ptr<gpu::Texture>();
         }
      }
   
   // Read file header
   Header header;
   file->read(0, sizeof(Header), &header);
   if (header.signature != 0x01312F76)
      {
      Log << "ERROR: EXR file header signature incorrect!\n";
      file = nullptr;
      return Ptr<gpu::Texture>();
      }
   
   // TODO: Support multi-part types
   if (header.multiPart)
      {
      Log << "ERROR: EXR multi-part files are not supported!\n";
      file = nullptr;
      return Ptr<gpu::Texture>();
      }

   // If Single Part, determine part type
   PartType singlePartType;
   if (!header.multiPart)
      {
      if (!header.singleTile && !header.containsData)
         singlePartType = ScanLineImage;
      if (header.singleTile && !header.containsData)
         singlePartType = TiledImage;
      if (!header.singleTile && header.containsData)
         singlePartType = DeepScanLine;
      if (header.singleTile && header.containsData)
         singlePartType = DeepTile;
      };

   // TODO: Support non scan line types
   if (singlePartType != ScanLineImage)
      {
      Log << "ERROR: Engine supports only scan lined EXR images!\n";
      file = nullptr;
      return Ptr<gpu::Texture>();
      }

   // Read Part Headers
   vector<PartHeader> headers;
   bool readingHeaders = true;
   uint64 offset = sizeof(Header);
   uint32 maxChars = header.longNames ? 255 : 31;
   while(readingHeaders)
        { 
        // Read Part Header attributes
        PartHeader partHeader;
        while(true)
             {
             // Read “name”  
             string name;   
             uint32 length = file->read(offset, maxChars, name);
             offset += (length + 1);
             if (length == 0)
                break;

             // Read “type”
             string type;
             length = file->read(offset, maxChars, type); 
             offset += (length + 1);
             
             // Read size
             uint32 size = 0;
             file->read(offset, 4, &size);
             offset += 4;

             // Read value
             bool read = false;
             for(uint8 i=0; i<attributesCount; i++)
                if ( (name.compare(attribute[i].name) == 0) &&
                     (type.compare(attribute[i].type) == 0) &&
                     size == attribute[i].size ) 
                   {
                   file->read(offset, size, reinterpret_cast<void*>(reinterpret_cast<uint64>(&partHeader) + attribute[i].offset) );
                   offset += size;
                   read = true;
                   break;
                   };

             // Read special type value
             if (!read)
                {
                // Channels
                if ( (name.compare("channels") == 0) &&
                     (type.compare("chlist") == 0) ) 
                   {
                   vector<Channel> channels;
                   Channel channel;
                   uint64 counter = offset;

                   while(counter < (offset+size))
                        {
                        // Channel "name"
                        length = file->read(offset, 255, channel.name); 
                        offset += (length + 1);
                        if (length == 0)
                           break;

                        // Channel "type"
                        file->read(offset, 4, &channel.type);
                        offset += 4;
                        
                        // Channel linear
                        file->read(offset, 1, &channel.pLinear);
                        offset += 4;
                        
                        // Channel sampling
                        file->read(offset, 4, &channel.xSampling);
                        offset += 4;
                        file->read(offset, 4, &channel.ySampling);
                        offset += 4;

                        channels.push_back(channel);
                        }
                              
                   partHeader.channels = channels.size();
                   partHeader.channel = new Channel[partHeader.channels];
                   for(uint8 i=0; i<partHeader.channels; ++i)
                      partHeader.channel[i] = channels[i];
                   }

                // Tiles
                if ( (name.compare("tiles") == 0) &&
                     (type.compare("tiledesc") == 0) ) 
                   {
                   // TODO: Decode Tile descriptor
                   }

                // Comment
                if ( (name.compare("comment") == 0) &&
                     (type.compare("string") == 0) ) 
                   {
                   length = file->read(offset, size, partHeader.commenet); 
                   offset += length; 
                   }

                // Multipart Name
                if ( (name.compare("name") == 0) &&
                     (type.compare("string") == 0) ) 
                   {
                   length = file->read(offset, size, partHeader.name); 
                   offset += length; 
                   }

                // Multipart Type
                if ( (name.compare("type") == 0) &&
                     (type.compare("string") == 0) ) 
                   {
                   string type;
                   length = file->read(offset, size, type); 
                   offset += length;

                   if (type.compare("scanlineimage") == 0)
                      partHeader.type = ScanLineImage; 
                   if (type.compare("tiledimage") == 0)
                      partHeader.type = TiledImage; 
                   if (type.compare("deepscanline") == 0)
                      partHeader.type = DeepScanLine; 
                   if (type.compare("deeptile") == 0)
                      partHeader.type = DeepTile; 
                   }

                }
             }
        
        // Data and display windows size is counted from zero,
        // which meands it need to be increased by one to get
        // proper image size.
        partHeader.dataWindow.width++;
        partHeader.dataWindow.height++;
        partHeader.displayWindow.width++;
        partHeader.displayWindow.height++;

        headers.push_back(partHeader);

        // If single part file, there is only one header
        if (!header.multiPart)
           break;
        else
           // In multi-part file, last part header is followed by null
           file->read(offset, 1, &readingHeaders); 
        }

   // If Single Part, set part type
   if (!header.multiPart)
      headers[0].type = singlePartType;
  
   // Read parts
   for(uint32 part=0; part<headers.size(); ++part)
      {
      // Check if compression is supported
      if ( headers[part].compression != None &&
           headers[part].compression != ZIP )
         {
         if (headers[part].compression == RLE)   Log << "ERROR: Engine doesn't supports EXR images with RLE compression!\n";
         if (headers[part].compression == ZIPS)  Log << "ERROR: Engine doesn't supports EXR images with ZIPS compression!\n";
         if (headers[part].compression == PIZ)   Log << "ERROR: Engine doesn't supports EXR images with PIZ compression!\n";
         if (headers[part].compression == PXR24) Log << "ERROR: Engine doesn't supports EXR images with PXR24 compression!\n";
         if (headers[part].compression == B44)   Log << "ERROR: Engine doesn't supports EXR images with B44 compression!\n";
         if (headers[part].compression == B44A)  Log << "ERROR: Engine doesn't supports EXR images with B44A compression!\n";
         Log << "       Can't load part " << part << " from file.\n";
         continue;
         }

      // Check if standard data window
      if ( headers[part].dataWindow.x      != headers[part].displayWindow.x      ||
           headers[part].dataWindow.y      != headers[part].displayWindow.y      ||
           headers[part].dataWindow.width  != headers[part].displayWindow.width  ||
           headers[part].dataWindow.height != headers[part].displayWindow.height )
         {
         Log << "ERROR: Engine doesn't supports clipping data window to display window!\n";
         Log << "       Can't load part " << part << " from file.\n";
         continue;
         }

      // Determine texture parameters
      gpu::TextureState settings;
      settings.width  = headers[part].displayWindow.width;
      settings.height = headers[part].displayWindow.height;

      // Set texture type
      if ( headers[part].type == ScanLineImage || 
           headers[part].type == TiledImage )
         settings.type = gpu::TextureType::Texture2D;
      if ( headers[part].type == DeepScanLine || 
           headers[part].type == DeepTile )
         {
         settings.type  = gpu::TextureType::Texture3D;
         settings.depth = headers[part].maxSamplesPerPixel;
         }
      
      // Determine texture format to use. At this stage,
      // it doesn't matter what chnnels really represent.
      // It only matters what format they are all stored in.
      // Channels are stored in alphabetical order anyway.
      // We can order them in GPU memory the way we want.
      bool correct = false;
      if (headers[part].channels == 1)
         {
         if (headers[part].channel[0].type == 0) { settings.format = gpu::Format::R_32_u;  correct = true; }
         if (headers[part].channel[0].type == 1) { settings.format = gpu::Format::R_16_hf; correct = true; }
         if (headers[part].channel[0].type == 2) { settings.format = gpu::Format::R_32_f;  correct = true; }
         }
      if (headers[part].channels == 2)
         if (headers[part].channel[0].type == headers[part].channel[1].type)
            {
            if (headers[part].channel[0].type == 0) { settings.format = gpu::Format::RG_32_u;  correct = true; }
            if (headers[part].channel[0].type == 1) { settings.format = gpu::Format::RG_16_hf; correct = true; }
            if (headers[part].channel[0].type == 2) { settings.format = gpu::Format::RG_32_f;  correct = true; }
            correct = true;
            }
      if (headers[part].channels == 3)
         if ( (headers[part].channel[0].type == headers[part].channel[1].type) &&
              (headers[part].channel[1].type == headers[part].channel[2].type) )
            {
            if (headers[part].channel[0].type == 0) { settings.format = gpu::Format::RGB_32_u;  correct = true; }
            if (headers[part].channel[0].type == 1) { settings.format = gpu::Format::RGB_16_hf; correct = true; }
            if (headers[part].channel[0].type == 2) { settings.format = gpu::Format::RGB_32_f;  correct = true; }
            correct = true;
            }
      if (headers[part].channels == 4)
         if ( (headers[part].channel[0].type == headers[part].channel[1].type) &&
              (headers[part].channel[1].type == headers[part].channel[2].type) &&
              (headers[part].channel[2].type == headers[part].channel[3].type) )
            {
            if (headers[part].channel[0].type == 0) { settings.format = gpu::Format::RGBA_32_u;  correct = true; }
            if (headers[part].channel[0].type == 1) { settings.format = gpu::Format::RGBA_16_hf; correct = true; }
            if (headers[part].channel[0].type == 2) { settings.format = gpu::Format::RGBA_32_f;  correct = true; }
            correct = true;
            }

      // Unsupported mixed channel formats
      if (!correct)
         {
         Log << "ERROR: Engine doesn't supports EXR images with mixed channel format!\n";
         Log << "       Can't load part " << part << " from file.\n";
         continue;
         }

      // Check that data is contiguous in memory.
      correct = true;
      for(uint32 i=0; i<headers[part].channels; ++i)
         if ( headers[part].channel[i].xSampling != 1 ||
              headers[part].channel[i].ySampling != 1 )
            {
            correct = false;
            break;
            }

      // Unsupported data layout
      if (!correct)
         {
         Log << "ERROR: Engine supports only contiguous data layout for channes!\n";
         Log << "       Can't load part " << part << " from file.\n";
         continue;
         }
   
      // Create texture in gpu
      Ptr<gpu::Texture> texture = en::ResourcesContext.defaults.enHeap->createTexture(settings);
      if (!texture)
         {
         Log << "ERROR: Cannot create texture in GPU!\n";
         continue;
         }
      
      // Single part files can have missing chunkCount field.
      // In such situation chunks count needs to be computed.
      sint32 chunks = headers[part].chunkCount;
      if (chunks == -1)
         {
         if (headers[part].type == ScanLineImage)
            {
            if (headers[part].compression < 3)
               chunks = headers[part].dataWindow.height;
            else
            if (headers[part].compression == ZIP   ||
                headers[part].compression == PXR24 )
               chunks = (headers[part].dataWindow.height / 16) + ( headers[part].dataWindow.height % 16 ? 1 : 0);
            else
               chunks = (headers[part].dataWindow.height / 32) + ( headers[part].dataWindow.height % 16 ? 1 : 0);
            }

         // TODO: Determine chunks count for other types
         // - dataWindow
         // - tileDesc
         // - compression
         assert( chunks != -1 );
         }

      // Calculate block size
      uint32 channelSize = headers[part].channel[0].type % 2 ? 2 : 4;
      uint32 pixelSize   = headers[part].channels * channelSize;
      uint32 blocksize   = headers[part].dataWindow.width * pixelSize * 64; // Allocate 2 times te maxium required block size to give room for INFLATE 
      uint32 blockLines  = 1;
      if (headers[part].type == ScanLineImage)
         {
         if ( headers[part].compression == ZIP   ||
              headers[part].compression == PXR24 )
            blockLines = 16;
         else
         if ( headers[part].compression > 3 )
            blockLines = 32;

         //blocksize = headers[part].dataWindow.width * blockLines * pixelSize;
         }
      // TODO: Determine block size for other types

      // Read offset table
      uint64* offsets = new uint64[chunks];
      for(uint32 i=0; i<chunks; ++i)
         {
         file->read(offset, 8, &offsets[i]); 
         offset += 8;
         }

      // Decompress texture
      
      // For now on it will require 2 copies File > Transfer > Texture (in future , mmap file directly to Transfer)
      uint8* dst = new uint8[texture->size()];

      // Read data
      for(uint32 i=0; i<chunks; ++i)
         {
         uint32 line;
         uint32 size;
         offset = offsets[i];

         // Read chunk header
         if (header.multiPart)
            {
            uint32 partNumber;
            file->read(offset, 4, &partNumber); 
            offset += 4;
            assert(partNumber == part);
            }
         file->read(offset, 4, &line); 
         offset += 4;
         file->read(offset, 4, &size);
         offset += 4;

         // Last block has less lines
         if (i + 1 == chunks)
            blockLines = headers[part].dataWindow.height - ((chunks - 1) * blockLines);

         if (headers[part].compression == None)
            {
            }
         else
         if (headers[part].compression == ZIP)
            {
            // Read compressed data from disk
            uint8* input = new uint8[size];
            file->read(offset, size, input);
            
            // Uncompress data to temporary buffer
            uint8* output = new uint8[blocksize];
            
            // Create zlib stream structure
            z_stream stream;
            stream.zalloc    = Z_NULL;
            stream.zfree     = Z_NULL;
            stream.opaque    = Z_NULL;
            stream.next_in   = input;     // Compressed data
            stream.avail_in  = size;      // Size of compressed data
            stream.avail_out = blocksize; // Available space for uncompressed data
            stream.next_out  = output;    // Output destination
            
            // Decompress data from IDAT chunks to 'inflated' buffer
            if (CheckError(inflateInit(&stream)))
               {
               Log << "Error: Cannot initialize Zlib decompressor!\n";
               delete [] input;
               delete [] output;
               return Ptr<gpu::Texture>(nullptr);
               }
            sint32 ret = 0;
            ret = inflate(&stream, Z_FINISH);
            if (ret != Z_STREAM_END)
               {
               CheckError(ret);
               Log << "Error: Cannot decompress using ZLIB!\n";
               delete [] input;
               delete [] output;
               return Ptr<gpu::Texture>(nullptr);
               }
            inflateEnd(&stream);
            delete [] input;
            
            // Reorder uncompressed data to final buffer
            uint32 srcLineSize = headers[part].dataWindow.width * headers[part].channels * channelSize;
            for(uint32 y=0; y<blockLines; ++y)
               for(uint32 x=0; x<headers[part].dataWindow.width; ++x)
                  for(uint32 c=0; c<headers[part].channels; ++c)
                     {
                     uint32 dstLine = headers[part].dataWindow.height - (line + y) - 1;
                     uint8* srcPtr = output + (y * srcLineSize) + (c * headers[part].dataWindow.width * channelSize) + (x * channelSize); 
                     uint8* dstPtr = dst + (dstLine * srcLineSize) + (x * headers[part].channels * channelSize) + (c * channelSize);
                     memcpy(dstPtr, srcPtr, channelSize);
                     }

            delete [] output;
            }
         else
            {
            assert(0);
            // Unsupported compression type
            }

         }

      delete [] offsets;

      // Create staging buffer
      Ptr<gpu::Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, texture->size());
      if (!staging)
         {
         Log << "ERROR: Cannot create staging buffer!\n";
         file = nullptr;
         return texture;
         }

      // Read texture to temporary buffer
      void* ptr = staging->map();
      memcpy(ptr, dst, texture->size());
      staging->unmap();

      delete [] dst;
      
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

      return texture;   // TODO: Rework for multipart files!
      }

   return Ptr<gpu::Texture>(nullptr);
   }



   } 
} 
