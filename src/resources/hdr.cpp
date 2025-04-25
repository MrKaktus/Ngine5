/*

 Ngine v5.0
 
 Module      : HDR files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from *.hdr files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/utilities/parser.h"
#include "utilities/utilities.h"
#include "utilities/strings.h"
#include "resources/context.h"
#include "resources/hdr.h"

#include "core/types/half.h"

#include "core/rendering/device.h"

#include <cstddef>
#include <string>

namespace en
{
namespace hdr
{

enum Compression
{
    Unknown   = 0,
    RLE_RGBA     ,
    RLE_XYZ
};
      
// Decompress RGBA shared exponent to float RGB
float3 decompress(uint8* color)
{
    float3 out;
   
    // If color[4] == 0 return black
    if (color[4] != 0)
    {
        double f = ldexp(1.0, static_cast<sint32>(color[3]) - (128 + 8));
        out.r = static_cast<float>((static_cast<double>(color[0]) + 0.5) * f);
        out.g = static_cast<float>((static_cast<double>(color[1]) + 0.5) * f);
        out.b = static_cast<float>((static_cast<double>(color[2]) + 0.5) * f);
    }

    return out;
}

std::shared_ptr<en::gpu::Texture> load(const std::string& filename)
{
    using namespace en::storage;
    using namespace en::gpu;

    // Open image file
    File* file = Storage->open(filename);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.textures + filename);
        if (!file)
        {
            Log << en::ResourcesContext.path.textures + filename << std::endl;
            Log << "ERROR: There is no such file!\n";
            return std::shared_ptr<gpu::Texture>();
        }
    }
   
    // Read file header
    char header[11];
    header[10] = 0;
    const char radiance[] = "#?RADIANCE";
    if (!file->read(0, 10, &header))
    {
        Log << "ERROR: Not HDR file!\n";
        delete file;
        return std::shared_ptr<gpu::Texture>();
    }
    if (strcmp(radiance, header) != 0)
    {
        Log << "ERROR: HDR file header signature incorrect!\n";
        delete file;
        return std::shared_ptr<gpu::Texture>();
    }


    Compression compression = Unknown;
    float exposure;

    // Read parameters
    uint32 offset = 0;
    uint32 read = 0;
    std::string line;
    for(;;)
    {
        read = file->readLine(offset, 256, line);
        offset += (read + 1);
        if (line.empty())
        {
            break;
        }

        // Parse line
        if (line.compare(0, 6, "FORMAT") == 0)
        {
            if (line.compare(7, 15, "32-bit_rle_rgbe") == 0)
            {
                compression = RLE_RGBA;
            }
            if (line.compare(7, 15, "32-bit_rle_xyze") == 0)
            {
                compression = RLE_XYZ;
            }
        }

        if (line.compare(0, 8, "EXPOSURE") == 0)
        {
            exposure = stringTo<float>(line.substr(9));
        }

        // TODO: Parse other parameters
    }

    // TODO: Add support for XYZ color space conversion to RGB
    assert(compression == RLE_RGBA);

    // Read image size and orientation
    std::string word;
    uint32 width  = 0;
    uint32 height = 0;
    bool columnOrder = false;
    bool posX = false;
    bool posY = false;

    // First Axis
    read = file->readWord(offset, 256, word);
    offset += (read + 1);
    if ((word[1] == 'Y') && (word[0] == '+')) posY = true;
    if (word[1] == 'X')
    {
        columnOrder = true;
        if (word[0] == '+')
        {
            posX = true;
        }
    }

    // First Axis size
    read = file->readWord(offset, 256, word);
    offset += (read + 1);
    height = stringTo<uint32>(word);
    if (columnOrder)
    {
        width = height;
    }

    // Second Axis
    read = file->readWord(offset, 256, word);
    offset += (read + 1);
    if ((word[1] == 'Y') && (word[0] == '+')) posY = true;
    if ((word[1] == 'X') && (word[0] == '+')) posX = true;
 
    // Second Axis size
    read = file->readWord(offset, 256, word);
    offset += (read + 1);
    if (columnOrder)
    {
        height = stringTo<uint32>(word);
    }
    else
    {
        width  = stringTo<uint32>(word);
    }

    // Check if data is really compressed with RLE
    bool rle = false;
    uint64 rawSize = width * height * 4;
    if ( (file->size() - offset) != rawSize)
    {
        rawSize = file->size() - offset;
        rle = true;
    }

    // Read raw data to memory
    uint8* raw = allocate<uint8>(static_cast<uint32>(rawSize), cacheline);
    if (!file->read(offset, static_cast<uint32>(rawSize), raw))
    {
        Log << "ERROR: Cannot read HDR file to memory!\n";
        deallocate<uint8>(raw);
        delete file;
        return std::shared_ptr<gpu::Texture>();
    }

    // Texture is compressed using RLE one scan line at a time
    // http://radsite.lbl.gov/radiance/refer/Notes/picture_format.html
    // http://paulbourke.net/dataformats/pic/
    // Data stores texels in OpenGL friendly +Y +X order 
    uint8* data = allocate<uint8>(width * height * 4, cacheline);
    memset(data, 0, width * height * 4);
    uint32 lines = height;
    uint32 length = width;
    uint64 rawOffset = 0;
    for(uint32 scanline=0; scanline<lines; ++scanline)
    {
        // First texel in scanline determines compression method used.
        bool oldCompression = false;
        uint8 r = raw[rawOffset];
        uint8 g, b, e;
        if ( (length < 8 || length > 0x7FFF) ||
             (r != 2) )
        {
            oldCompression = true;
        }

        uint32 texel=0;
        if (!oldCompression)
        {
            rawOffset++;
            g = raw[rawOffset++];
            b = raw[rawOffset++];
            e = raw[rawOffset++];

            if (g != 2 || b & 128)
            {
                oldCompression = true;
            
                // Calculate destination offset in texture
                uint32 x, y;
                x = texel;                 // Pixels already read in row
                y = lines  - scanline - 1; // Destination row

                // Write texel to output
                data[(y*width + x)*4 + 0] = r;
                data[(y*width + x)*4 + 1] = g;
                data[(y*width + x)*4 + 2] = b;
                data[(y*width + x)*4 + 3] = e;
                texel++;
            }
        }

        if (oldCompression)
        {
            // Decompress scanline using old RLE method
            sint32 shift = 0;
            for(; texel<length; )
            {         
                // Read RAW texel
                uint8 r = raw[rawOffset++];
                uint8 g = raw[rawOffset++];
                uint8 b = raw[rawOffset++];
                uint8 e = raw[rawOffset++];

                // Calculate destination offset in texture
                uint32 x, y;
                x = texel;                 // Pixels already read in row
                y = lines  - scanline - 1; // Destination row

                // Write texel to output
                data[(y*width + x)*4 + 0] = r;
                data[(y*width + x)*4 + 1] = g;
                data[(y*width + x)*4 + 2] = b;
                data[(y*width + x)*4 + 3] = e;

                // If RGB == 1, this means that exponent stores information
                // about how many times given texel should be repeated. If
                // next texels also contait RGB == 1, this means that repeating
                // count of texels is greater than 255, and it is divided into
                // several texels, each storing next 8 bits of the total number
                // for which texels should be shifted.
                if (r == 1 && g == 1 && b == 1)
                {
                    uint32 repeat = static_cast<sint32>(e) << shift;

                    // We will repeat last texel value N times
                    r = raw[rawOffset - 8];
                    g = raw[rawOffset - 7];
                    b = raw[rawOffset - 6];
                    e = raw[rawOffset - 5];

                    for(uint32 i=0; i<repeat; ++i)
                    {
                        // Calculate destination offset in texture
                        uint32 x, y;
                        x = texel;                 // Pixels already read in row
                        y = lines  - scanline - 1; // Destination row
                  
                        // Write texel to output
                        data[(y*width + x)*4 + 0] = r;
                        data[(y*width + x)*4 + 1] = g;
                        data[(y*width + x)*4 + 2] = b;
                        data[(y*width + x)*4 + 3] = e;
                        texel++;
                    }

                    shift += 8;
                }
                else
                {
                    shift = 0;
                    texel++;
                }

                assert( texel <= length );
            }

            continue;
        }

    // Decompress using new method.
    // Each channel of RGBE texels is stored as separate
    // array, first RRRR, then GGG etc. Each arrray is
    // compressed using RLE encoding. First byte is a
    // counter byte. Values in range [1-128] represent
    // how many uncompressed values we should copy.
    // Values in range [129-255] after subtracting lower
    // 128 range, gives information about how many times
    // next input value should be repeated (uncompressed).
    for(uint32 channel=0; channel<4; ++channel)
    {
        for(uint32 texel=0; texel<length;)
        {  
            // Read control byte
            bool decompress = false;
            uint8 repeat = raw[rawOffset++];
            if (repeat > 128)
            {
                decompress = true;
                repeat &= 0x7F;
            }

            uint8 value = 0;
            for(uint32 i=0; i<repeat; ++i)
            {
                // If just copying data, read following bytes
                if (!decompress || i == 0)
                {
                    value = raw[rawOffset++];
                }

                // Calculate destination offset in texture
                uint32 x, y;
                x = texel;                 // Pixels already read in row
                y = lines  - scanline - 1; // Destination row
               
                // Write texel current channel value to output
                data[(y*width + x)*4 + channel] = value;
                texel++;
            }

            assert( texel <= length );
            }
        }
    }

    deallocate<uint8>(raw);

    // Determine texture parameters
    TextureState settings;
    settings.width  = width;
    settings.height = height;
    settings.format = Format::RGB_16_hf; // What about FormatEBGR_5_9_9_9_f ?
    settings.type   = TextureType::Texture2D;

    // Create texture in gpu
    std::unique_ptr<Texture> texture(en::ResourcesContext.defaults.enHeapTextures->createTexture(settings));
    if (!texture)
    {
        Log << "ERROR: Cannot create texture in GPU!\n";
        delete file;
        return std::shared_ptr<Texture>(nullptr);
    }
   
    // Create staging buffer
    uint32 stagingSize = texture->size();
    std::unique_ptr<gpu::Buffer> staging(en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, stagingSize));
    if (!staging)
    {
        Log << "ERROR: Cannot create staging buffer!\n";
        delete file;
        return std::shared_ptr<Texture>(nullptr);
    }

   // Recompress texture directly to gpu memory
   //uint16* dst = reinterpret_cast<uint16*>(texture.map());
   //for(uint32 texel=0; texel<(height * width); ++texel)
   //   {
   //   //uint16 mR = static_cast<uint16>(data[texel*4 + 0]);
   //   //uint16 mG = static_cast<uint16>(data[texel*4 + 1]);
   //   //uint16 mB = static_cast<uint16>(data[texel*4 + 2]);
   //   //uint16 e  = static_cast<uint16>(data[texel*4 + 3]);
  
   //   float3 flo = decompress(&data[texel*4]);
   //   half hR(flo.r);
   //   half hG(flo.g);
   //   half hB(flo.b);
   //   dst[texel*3 + 0] = hR.value;
   //   dst[texel*3 + 1] = hG.value;
   //   dst[texel*3 + 2] = hB.value;

   //   // Uncompress RGBE to 3 half's
   //   //dst[texel*3 + 0] = ((e << 10) + mB);
   //   //dst[texel*3 + 1] = ((e << 10) + mG);
   //   //dst[texel*3 + 2] = ((e << 10) + mR);

   //   // TODO: RGBE_8 to RGB_16_hf
   //   }


   //float3* dst = reinterpret_cast<float3*>(texture.map());
   //for(uint32 texel=0; texel<(height * width); ++texel)
   //   {
   //   float3 flo = decompress(&data[texel*4]);
   //   dst[texel] = flo;
   //   }


   // OK!
   //float3* dst = reinterpret_cast<float3*>(texture.map());
   //for(uint32 texel=0; texel<(height * width); ++texel)
   //   {
   //   float3 flo; // = decompress(&data[texel*4]);

   //   float e = powf( 2.0f, static_cast<float>(data[texel*4+3] - 128) );
   //   flo.r = (static_cast<float>(data[texel*4])   / 256.0f) * e;
   //   flo.g = (static_cast<float>(data[texel*4+1]) / 256.0f) * e;
   //   flo.b = (static_cast<float>(data[texel*4+2]) / 256.0f) * e;

   //   dst[texel] = flo;
   //   }

    volatile uint16* dst = reinterpret_cast<volatile uint16*>(staging->map());
    for(uint32 texel=0; texel<(height * width); ++texel)
    {
      //uint16 mR = static_cast<uint16>(data[texel*4 + 0]);
      //uint16 mG = static_cast<uint16>(data[texel*4 + 1]);
      //uint16 mB = static_cast<uint16>(data[texel*4 + 2]);
      //uint16 e  = static_cast<uint16>(data[texel*4 + 3]);
     
      //// Uncompress RGBE to 3 half's
      //dst[texel*3 + 0] = ((e << 10) + mB);
      //dst[texel*3 + 1] = ((e << 10) + mG);
      //dst[texel*3 + 2] = ((e << 10) + mR);


      // Use Decompress or below
      float3 flo  = decompress(&data[texel*4]); 
      
      //float e = powf( 2.0f, static_cast<float>(data[texel*4+3] - 128) );
      //flo.r = (static_cast<float>(data[texel*4])   / 256.0f) * e;
      //flo.g = (static_cast<float>(data[texel*4+1]) / 256.0f) * e;
      //flo.b = (static_cast<float>(data[texel*4+2]) / 256.0f) * e;

        half hR(flo.r);
        half hG(flo.g);
        half hB(flo.b);
        dst[texel*3 + 0] = hR.value;
        dst[texel*3 + 1] = hG.value;
        dst[texel*3 + 2] = hB.value;
    }

    staging->unmap();
    
    // TODO: In future distribute transfers to different queues in the same queue type family
    gpu::QueueType type = gpu::QueueType::Universal;
    if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
    {
        type = gpu::QueueType::Transfer;
    }

    // Copy data from staging buffer to final texture
    std::shared_ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(type);
    command->start();
    command->copy(*staging, 0u, settings.rowSize(0u), *texture, 0u, 0u);
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
   
    deallocate<uint8>(data);
    delete file;
    return texture;
}

} // en::hdr
} // en




//bool decrunch(RGBE *scanline, int len, FILE *file)
//{
//	int  i, j;
//					
//	if (len < MINELEN || len > MAXELEN)
//		return oldDecrunch(scanline, len, file);
//
//	i = fgetc(file);
//	if (i != 2) {
//		fseek(file, -1, SEEK_CUR);
//		return oldDecrunch(scanline, len, file);
//	}
//
//	scanline[0][G] = fgetc(file);
//	scanline[0][B] = fgetc(file);
//	i = fgetc(file);
//
//	if (scanline[0][G] != 2 || scanline[0][B] & 128) {
//		scanline[0][R] = 2;
//		scanline[0][E] = i;
//		return oldDecrunch(scanline + 1, len - 1, file);
//	}
//
//	// read each component
//	for(i = 0; i < 4; i++) {
//	    for(j = 0; j < len; ) {
//			unsigned char code = fgetc(file);
//			if (code > 128) { // run
//			    code &= 127;
//			    unsigned char val = fgetc(file);
//			    while (code--)
//					scanline[j++][i] = val;
//			}
//			else  {	// non-run
//			    while(code--)
//					scanline[j++][i] = fgetc(file);
//			}
//		}
//    }
//
//	return feof(file) ? false : true;
//}
//
//bool oldDecrunch(RGBE *scanline, int len, FILE *file)
//{
//	int i;
//	int rshift = 0;
//	
//	while (len > 0) {
//		scanline[0][R] = fgetc(file);
//		scanline[0][G] = fgetc(file);
//		scanline[0][B] = fgetc(file);
//		scanline[0][E] = fgetc(file);
//		if (feof(file))
//			return false;
//
//		if (scanline[0][R] == 1 &&
//			scanline[0][G] == 1 &&
//			scanline[0][B] == 1) {
//			for(i = scanline[0][E] << rshift; i > 0; i--) {
//				memcpy(&scanline[0][0], &scanline[-1][0], 4);
//				scanline++;
//				len--;
//			}
//			rshift += 8;
//		}
//		else {
//			scanline++;
//			len--;
//			rshift = 0;
//		}
//	}
//	return true;
//}




//// Compress float colors to RGBA shared exponent
//void compress(float3 color, uint8* out)
//{
//// Choose channel with maximum value
//float max = color.r > color.g ? color.r : color.g;
//if (color.b > max) 
//   max = color.b;
//
//// If maximum channel color value is to small to represent, we use pure black
//if (max <= 1e-32) 
//   {
//   out[0] = 0;
//   out[1] = 0;
//   out[2] = 0;
//   out[3] = 0;
//   return;
//   }
//
//// x = significant * 2^exponent
//sint32 exponent;
//float significant = frexpf(max, &exponent);
//
//// Quantization step is calculated using largest of three color values
//float multiplier = (255.9999 / max) * significant;
//
//out[0] = static_cast<uint8>(color.r * multiplier);
//out[1] = static_cast<uint8>(color.g * multiplier);
//out[2] = static_cast<uint8>(color.b * multiplier);
//out[3] = static_cast<uint8>(e + 128);
//}
//







   //// Memory mapped file is read continuously 
   //for(uint32 texel=0; texel<(settings.height * settings.width); ++texel)
   //   {
   //   uint16 mR(0), mG(0), mB(0), e(0);
   //   file->read(offset++, 1, &mR);
   //   file->read(offset++, 1, &mG);
   //   file->read(offset++, 1, &mB);
   //   file->read(offset++, 1, &e);

   //   // Calculate destination offset in texture
   //   uint32 x, y;
   //   if (columnOrder)
   //      {
   //      x = texel / settings.height; // Columns already read
   //      y = texel % settings.height; // Pixels already read in column
   //      }
   //   else
   //      {
   //      y = texel / settings.width;  // Rows already read
   //      x = texel % settings.width;  // Pixels already read in row
   //      }
   //   if (!posY)
   //      y = settings.height - y - 1; // Destination row
   //   if (!posX)
   //      x = settings.width - x - 1;  // Destination column

   //   // Uncompress RGBE to 3 half's
   //   *(dst + (y*settings.width + x)*3 + 0) = ((e << 10) + mB);
   //   *(dst + (y*settings.width + x)*3 + 1) = ((e << 10) + mG);
   //   *(dst + (y*settings.width + x)*3 + 2) = ((e << 10) + mR);
   //   }



   //// Read image size and orientation
   //string stX, stY, stWidth, stHeight;
   //read = file->readWord(offset, 256, stY);
   //offset += (read + 1);
   //read = file->readWord(offset, 256, stHeight);
   //offset += (read + 1);
   //read = file->readWord(offset, 256, stX);
   //offset += (read + 1);
   //read = file->readWord(offset, 256, stWidth);
   //offset += (read + 1);





//   // Texture is compressed using RLE one scan line at a time
//   // http://radsite.lbl.gov/radiance/refer/Notes/picture_format.html
//   // http://paulbourke.net/dataformats/pic/
//   // Data stores texels in OpenGL friendly +Y +X order 
//   uint8* data = allocate<uint8>(width * height * 4, cacheline);
//   uint32 lines = columnOrder ? width : height;
//   uint32 length = columnOrder ? height : width;
//   uint64 rawOffset = 0;
//   for(uint32 scanline=0; scanline<lines; ++scanline)
//      {
////if (scanline == 102) break;
//
//      // First texel in scanline determines compression method used.
//      //bool oldCompression = false;
//      uint8 r = raw[rawOffset];
//      uint8 g = raw[rawOffset + 1];
//      uint8 b = raw[rawOffset + 2];
//      if ( (length < 8 || length > 0x7FFF) ||
//           (r != 2) ||
//           (g != 2 || b & 128) )
//      //if (oldCompression)
//         {
//         // Decompress scanline using old RLE method
//         sint32 shift = 0;
//         for(uint32 texel=0; texel<length; ++texel)
//            {         
//            // Read RAW texel
//            uint8 r = raw[rawOffset++];
//            uint8 g = raw[rawOffset++];
//            uint8 b = raw[rawOffset++];
//            uint8 e = raw[rawOffset++];
//
//            // Calculate destination offset in texture
//            uint32 x, y;
//            if (columnOrder)
//               {
//               x = scanline; // Columns already read
//               y = texel;    // Pixels already read in column
//               if (!posX)
//                  x = lines  - x - 1; // Destination column
//               if (!posY)
//                  y = length - y - 1; // Destination row
//               }
//            else
//               {
//               x = texel;    // Pixels already read in row
//               y = scanline; // Rows already read
//               if (!posX)
//                  x = length - x - 1; // Destination column
//               if (!posY)
//                  y = lines  - y - 1; // Destination row
//               }
//
//            // Write texel to output
//            data[(y*width + x)*4 + 0] = r;
//            data[(y*width + x)*4 + 1] = g;
//            data[(y*width + x)*4 + 2] = b;
//            data[(y*width + x)*4 + 3] = e;
//
//            // If RGB == 1, this means that exponent stores information
//            // about how many times given texel should be repeated. If
//            // next texels also contait RGB == 1, this means that repeating
//            // count of texels is greater than 255, and it is divided into
//            // several texels, each storing next 8 bits of the total number
//            // for which texels should be shifted.
//            if (r == 1 && g == 1 && b == 1)
//               {
//               uint32 repeat = static_cast<sint32>(e) << shift;
//               for(uint32 i=0; i<repeat; ++i)
//                  {
//                  texel++;
//
//                  // Calculate destination offset in texture
//                  uint32 x, y;
//                  if (columnOrder)
//                     {
//                     x = scanline; // Columns already read
//                     y = texel;    // Pixels already read in column
//                     if (!posX)
//                        x = lines  - x - 1; // Destination column
//                     if (!posY)
//                        y = length - y - 1; // Destination row
//                     }
//                  else
//                     {
//                     x = texel;    // Pixels already read in row
//                     y = scanline; // Rows already read
//                     if (!posX)
//                        x = length - x - 1; // Destination column
//                     if (!posY)
//                        y = lines  - y - 1; // Destination row
//                     }
//                  
//                  // Write texel to output
//                  data[(y*width + x)*4 + 0] = r;
//                  data[(y*width + x)*4 + 1] = g;
//                  data[(y*width + x)*4 + 2] = b;
//                  data[(y*width + x)*4 + 3] = e;
//                  }
//
//               shift += 8;
//               }
//            else
//               shift = 0;
//            assert( texel <= length );
//            }
//
//         continue;
//         }
//
//      // Decompress using new method.
//      // Each channel of RGBE texels is stored as separate
//      // array, first RRRR, later GGG etc. Each arrray is
//      // compressed using RLE encoding. First byte is a
//      // counter byte. 7 LO bits represent count of bytes
//      // to write. 1 HI bit stores information if this 
//      // count should be written repeating next byte, or
//      // just copying next N bytes.
//      rawOffset += 4;
//      for(uint32 channel=0; channel<4; ++channel)
//         {
//         for(uint32 texel=0; texel<length;)
//            {  
//            // Read control byte
//            uint8 control = raw[rawOffset++];
//            bool decompress = false;
//            if (control > 128)
//               decompress = true;
//            uint32 repeat = control & 0x7F;
//
//            uint8 value = 0;
//            for(uint32 i=0; i<repeat; ++i)
//               {
//               // If just copying data, read following bytes
//               if (!decompress || i == 0)
//                  value = raw[rawOffset++];
//
//               // Calculate destination offset in texture
//               uint32 x, y;
//               if (columnOrder)
//                  {
//                  x = scanline; // Columns already read
//                  y = texel;    // Pixels already read in column
//                  if (!posX)
//                     x = lines  - x - 1; // Destination column
//                  if (!posY)
//                     y = length - y - 1; // Destination row
//                  }
//               else
//                  {
//                  x = texel;    // Pixels already read in row
//                  y = scanline; // Rows already read
//                  if (!posX)
//                     x = length - x - 1; // Destination column
//                  if (!posY)
//                     y = lines  - y - 1; // Destination row
//                  }
//               
//               // Write texel current channel value to output
//               data[(y*width + x)*4 + channel] = value;
//               texel++;
//
//               if (texel == length) break;
//               }
//            assert( texel <= length );
//            }
//         }
//      }
//   deallocate<uint8>(raw);
