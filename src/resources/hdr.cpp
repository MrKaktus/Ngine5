/*

 Ngine v5.0
 
 Module      : HDR files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from *.hdr files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/utilities/parser.h"
#include "utilities/utilities.h"
#include "utilities/strings.h"
#include "assets/assets.h"
#include "resources/hdr.h"

#include "core/types/half.h"

#include "core/rendering/device.h"

#include <assert.h>
#include <cstddef>
#include <string>
#include <string_view>

namespace en
{
namespace hdr
{

#define PageSize 4096ull

#define MinLineLengthForRLE 8      // Minimum line length for encoding each channel as separate RLE compressed block
#define MaxLineLengthForRLE 0x7FFF // Maximum line length for encoding each channel as separate RLE compressed block

struct LineHeader
{
    uint8  unknown0;      // Expected: 2
    uint8  unknown1;      // Expected: 2
    uint8  lengthHi : 7;  // High 7 bits of line length
    uint8  reserved : 1;  // Expected: 0
    uint8  lengthLo;      // Low 8 bits of line length
};

static_assert(sizeof(LineHeader) == 4, "en::hdr::LineHeader size mismatch.");

// Parses buffer storing metadata of HDR file, and decodes it's header to TextureState and ColorSpace.
ParsingResult parseMetadata(const uint8* buffer, const uint64 size, gpu::TextureState& storedTextureState, Metadata& metadata)
{
    // See specification: https://paulbourke.net/dataformats/pic/

    // By default resulting data will be stored in RGB 32f format,
    // and thats whats reported back as baseline format needed to
    // be supported by the caller. Caller can of course request
    // for data to be stored in different format. For e.g. it is 
    // advised to convert data to RGB 16f or store it compressed
    // under RGBA 8un format (and manually uncompress in shader).
    storedTextureState.format = gpu::Format::RGB_32_f;

    // TODO: Determine color space from the header, and define default one.

    // Initis HDR specific metadata to defaults
    metadata.compression = Unknown;
    metadata.exposure = 1.0f;
    metadata.offsetToData = 0ull;
    metadata.columnOrder = false; // Image is stored texel after texel in a line, then line after line.
    metadata.positiveX = true;    // Default coordinate system is +X right, +Y up. Image lower-left corner is at coordinate system center (0, 0). 
    metadata.positiveY = false;   // Image is stored in file from top to bottom though (so from upper-left corner), and thus -Y indicates direction of reading scanlines.

    // Verifies header signature
    const char radianceSignature[] = "#?RADIANCE\0";
    const char rgbeSignature[] = "#?RGBE\0";
    uint64 length = 0;
    ParsingResult result = isLineMatching(buffer, size, radianceSignature, length);
    if (result != ParsingResult::Success)
    {
        if (result == ParsingResult::IncompleteData)
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }

        assert(result == ParsingResult::InvalidFormat);
        
        result = isLineMatching(buffer, size, rgbeSignature, length);
        if (result == ParsingResult::IncompleteData)
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        else
        if (result == ParsingResult::InvalidFormat)
        {
            logError("HDR file header signature incorrect!\n");
            return result;
        }

        assert(result == ParsingResult::Success);
    }

    uint64 offset = length;

    // Parse parameters as set of <key>=<value> pairs. 
    // Each parameter is terminated with EOL.
    // Empty line identifies end of parameters block.
    while(true)
    {
        uint64 remainingSize = size - offset;
        uint64 lineLength = 0;
        uint64 nextLineOffset = 0;
        result = isLine(buffer + offset, remainingSize, lineLength, nextLineOffset);
        if (result == ParsingResult::IncompleteData)
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        else
        if (result == ParsingResult::InvalidFormat)
        {
            logError("HDR file header corrupted!\n");
            return result;
        }

        // Empty line indicates end of HDR header
        if (lineLength == 0)
        {
            offset += nextLineOffset;
            break;
        }

        logDebug("HDR Header: %.*s\n", lineLength, buffer + offset);

        // Creates view of that line for easy parsing
        std::string_view line((const char*)buffer + offset, lineLength);
        offset += nextLineOffset;

        // Parse line
        if (line.compare(0, 7, "FORMAT=") == 0)
        {
            // "FORMAT=32-bit_rle_rgbe"
            if (line.compare(7, 15, "32-bit_rle_rgbe") == 0)
            {
                metadata.compression = RLE_RGBE;
            }
            // "FORMAT="32-bit_rle_xyze"
            if (line.compare(7, 15, "32-bit_rle_xyze") == 0)
            {
                metadata.compression = RLE_XYZE;
            }
        }

        // "EXPOSURE=<float>"
        if (line.compare(0, 9, "EXPOSURE=") == 0)
        {
            metadata.exposure = stringTo<float>(line.substr(9));
        }

        // TODO: Parse other parameters
    }

    // Header is followed with line describing image resolution and orientation
    {
        uint64 remainingSize = size - offset;
        uint64 lineLength = 0;
        uint64 nextLineOffset = 0;
        result = isLine(buffer + offset, remainingSize, lineLength, nextLineOffset);
        if (result == ParsingResult::IncompleteData)
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        else
        if (result == ParsingResult::InvalidFormat)
        {
            logError("HDR file header corrupted!\n");
            return result;
        }

        // Creates view of that line for easy parsing
        std::string_view line((const char*)buffer + offset, lineLength);
        offset += nextLineOffset;

        std::string_view word = {};
        size_t lineOffset = 0;
        
        // First Axis
        if (!nextWord(line, lineOffset, word))
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        if (word.size() < 2)
        {
            logError("HDR file header corrupted!\n");
            return  ParsingResult::InvalidFormat;
        }
        if (word[0] == '+' && word[1] == 'Y')
        {
            metadata.positiveY = true;
        }
        if (word[1] == 'X')
        {
            metadata.columnOrder = true;
            if (word[0] == '-')
            {
                metadata.positiveX = false;
            }
        }
        
        // First Axis size
        if (!nextWord(line, lineOffset, word))
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        storedTextureState.height = stringTo<uint32>(word);
        if (metadata.columnOrder)
        {
            storedTextureState.width = storedTextureState.height;
        }

        // Second Axis
        if (!nextWord(line, lineOffset, word))
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        if (word.size() < 2)
        {
            logError("HDR file header corrupted!\n");
            return  ParsingResult::InvalidFormat;
        }
        if (word[0] == '+' && word[1] == 'Y')
        {
            metadata.positiveY = true;
        }
        if (word[0] == '-' && word[1] == 'X')
        {
            metadata.positiveX = false;
        }

        // Second Axis value
        if (!nextWord(line, lineOffset, word))
        {
            logError("HDR file size too small!\n");
            return ParsingResult::IncompleteData;
        }
        if (metadata.columnOrder)
        {
            storedTextureState.height = stringTo<uint32>(word);
        }
        else
        {
            storedTextureState.width = stringTo<uint32>(word);
        }
    }

    return ParsingResult::Success;
}

bool loadMetadata(
    const std::string& filename,
    gpu::TextureState& storedTextureState,
    gpu::ColorSpace& storedColorSpace,
    Metadata& metadata)
{
    using namespace en::storage;
    using namespace en::gpu;

    // Open file 
    std::string fullPath = filename;
    File* file = Storage->open(fullPath);
    if (!file)
    {
        fullPath = Assets().assetsPath() + filename;
        file = Storage->open(fullPath);
        if (!file)
        {
            logError("There is no such file!\nFile: %s\n", fullPath.c_str());
            return false;
        }
    }


    // ### Read file metadata


    // Read file first 4KB into single 4KB memory page
    uint64 fileSize = file->size();
    uint64 readSize = min(fileSize, PageSize);
    uint8* buffer = allocate<uint8>(static_cast<uint32>(readSize), PageSize);
    if (!buffer)
    {
        logCritical("Run out of memory!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }
    if (!file->read(0, readSize, buffer, &readSize))
    {
        logError("Couldn't read file metadata to memory!\nFile: %s\n", fullPath.c_str());
        deallocate<uint8>(buffer);
        delete file;
        return false;
    }
    delete file;

    // Read file properties
    ParsingResult result = parseMetadata(buffer, readSize, storedTextureState, metadata);

    // Unless explicitly specified as CIE XYZ, HDR file format does
    // not specify color space. Thus by default it is assumed to be
    // sRGB / ~Rec.709. 
    storedColorSpace = ColorSpace::SRGB;
    if (metadata.compression == RLE_XYZE)
    {
        storedColorSpace = ColorSpace::CIE_XYZ;
    }
    // TODO: Check if header doesn't define color space PRIMARIES, and then derive color space from that.

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    return (result == ParsingResult::Success);
}

// Based on the way source image is stored in memory:
// - row after row, or column after column
// - from left to right, or right to left
// - from top to bottom, or bottom to top
// calculates texel offset in destination buffer in memory.
// Destination image is stored row after row, from left to 
// right and from top to bottom (center of coordinate system
// is in upper-left corner of the image).
// 
inline uint64 texelOffset(const Metadata& metadata, const uint32 width, const uint32 height, const uint32 texel, const uint32 scanline)
{
    // Calculate destination offset in texture
    uint32 x, y;

    // Source image is stored column after column, texel after texel in each column.
    if (metadata.columnOrder)
    {
        if (metadata.positiveX)
        {
            // Columns are stored from left to right
            x = scanline;
        }
        else
        {
            // Columns are stored from right to left
            x = width - scanline - 1;
        }

        if (metadata.positiveY)
        {
            // Texels in column are stored bottom to top
            y = height - texel - 1;
        }
        else
        {
            // Texels in column are stored top to bottom
            y = texel;
        }
    }
    else // Source image is stored row after row, texel after texel in each row.
    {
        if (metadata.positiveX)
        {
            // Texels are stored from left to right
            x = texel;
        }
        else
        {
            // Texels in line are stored from right to left
            x = width - texel - 1;
        }

        if (metadata.positiveY)
        {
            // Lines are stored from bottom to top
            y = height - scanline - 1;
        }
        else
        {
            // Lines are stored from top to bottom
            y = scanline;
        }
    }

    // Each destination texel is 4 bytes
    return ((y * width) + x) * 4ull;
}

void decompressRLE(const Metadata metadata, const uint32 width, const uint32 height, const uint8* const src, uint64 offset, const uint32 scanline, const uint32 length, uint8* const dst)
{
    uint8 r = 0;
    uint8 g = 0;
    uint8 b = 0;
    uint8 e = 0;
    sint32 shift = 0;

    // Decompress scanline using old RLE method
    uint32 texel = 0;
    for (; texel < length;)
    {
        // If RGB == 1, this means that exponent stores information
        // about how many times previous texel should be repeated. If
        // next texels also contait RGB == 1, this means that repeating
        // count of texels is greater than 255, and it is divided into
        // several texels, each storing next 8 bits of the total number
        // for which texels should be shifted.
        if (src[offset + 0] == 1 &&
            src[offset + 1] == 1 &&
            src[offset + 2] == 1)
        {
            // Repeat counter is stored in Exponent byte of consecutive 
            // texels, in little endian order. Thus each consecutive 
            // extracted part needs to be shifted by 8, relative to 
            // previous one to reconstruct its partial value. Then 
            // last color texel is repeated that partial value times.
            uint32 repeat = static_cast<sint32>(src[offset + 3]) << shift;
            shift += 8;

            offset += 4;

            for (uint32 i = 0; i < repeat; ++i)
            {
                // Calculate destination offset in texture
                uint64 dstOffset = texelOffset(metadata, width, height, texel, scanline);

                // Write texel to output
                dst[dstOffset + 0] = r;
                dst[dstOffset + 1] = g;
                dst[dstOffset + 2] = b;
                dst[dstOffset + 3] = e;
                texel++;
            }
        }
        else // Bytes store texel color
        {
            // Read source texel and cache it
            r = src[offset++];
            g = src[offset++];
            b = src[offset++];
            e = src[offset++];

            uint64 dstOffset = texelOffset(metadata, width, height, texel, scanline);

            // Write texel to output
            dst[dstOffset + 0] = r;
            dst[dstOffset + 1] = g;
            dst[dstOffset + 2] = b;
            dst[dstOffset + 3] = e;

            shift = 0;
            texel++;
        }

        assert(texel <= length);
    }
}

void decompressPerChannelRLE(const Metadata metadata, const uint32 width, const uint32 height, const uint8* const src, uint64 offset, const uint32 scanline, const uint32 length, uint8* const dst)
{
    // Decompress using new method.
    // Each channel of RGBE texels is stored as separate
    // array, first RRRR, then GGG etc. Each arrray is
    // compressed using RLE encoding. First byte is a
    // counter byte. Values in range [1-128] represent
    // how many uncompressed values we should copy as is.
    // Values in range [129-255] after subtracting lower
    // 128 range, gives information about how many times
    // next input value should be repeated (uncompressed).
    for (uint32 channel = 0; channel < 4; ++channel)
    {
        for (uint32 texel = 0; texel < length;)
        {
            // Read control byte
            bool decompress = false;
            uint8 repeat = src[offset++];
            if (repeat > 128)
            {
                decompress = true;
                repeat &= 0x7F;
            }

            uint8 value = 0;
            for (uint32 i = 0; i < repeat; ++i)
            {
                // Read texel to repeat (or consecutive texels to copy)
                if (i == 0 || !decompress)
                {
                    value = src[offset++];
                }

                // Calculate destination offset in texture
                uint64 dstOffset = texelOffset(metadata, width, height, texel, scanline);

                // Write texel current channel value to output
                dst[dstOffset + channel] = value;
                texel++;
            }

            assert(texel <= length);
        }
    }
}

forceinline float3 decodeRGBE(const uint8(&rgbe)[4])
{
    float3 result;

    // If no exponent, return black
    if (rgbe[3] == 0)
    {
        return result;
    }

    // value = mantissa / 256 * 2^(E - 128)
    // value = mantissa * 2^(E - 128 - 8)
    // value = mantissa * 2^(E - (128 + 8))
    // value = mantissa * 1 * 2^(E - 136)
    //      ldexp(x, e) = x * 2^e  (more precise than pow)
    float scale = ldexp(1.0f, rgbe[3] - 136);

    result.r = rgbe[0] * scale;
    result.g = rgbe[1] * scale;
    result.b = rgbe[2] * scale;

    return result;
}

bool load(const std::string& filename,
    uint8* const destination,                 
    const uint32 width,                       
    const uint32 height,                      
    const gpu::Format format,                 
    const gpu::ImageMemoryAlignment alignment,
    const bool invertHorizontal)      
{
    using namespace en::storage;
    using namespace en::gpu;

    // Open file 
    std::string fullPath = filename;
    File* file = Storage->open(fullPath);
    if (!file)
    {
        fullPath = Assets().assetsPath() + filename;
        file = Storage->open(fullPath);
        if (!file)
        {
            logError("There is no such file!\nFile: %s\n", fullPath.c_str());
            return false;
        }
    }


    // ### Read file metadata


    // Read file first 4KB into single 4KB memory page
    uint64 fileSize = file->size();
    uint64 readSize = min(fileSize, PageSize);
    uint8* buffer = allocate<uint8>(static_cast<uint32>(readSize), PageSize);
    if (!buffer)
    {
        logCritical("Run out of memory!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }
    if (!file->read(0, readSize, buffer, &readSize))
    {
        logError("Couldn't read file metadata to memory!\nFile: %s\n", fullPath.c_str());
        deallocate<uint8>(buffer);
        delete file;
        return false;
    }

    // Read file properties
    TextureState storedTextureState;
    Metadata metadata;
    ParsingResult result = parseMetadata(buffer, readSize, storedTextureState, metadata);

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    if (result != ParsingResult::Success)
    {
        delete file;
        return false;
    }

    // Verify that file matches expected properties
    if (storedTextureState.width  != width ||
        storedTextureState.height != height)
    {
        delete file;
        return false;
    }

    // Verify file is supported by engine
    if (metadata.compression == RLE_XYZE)
    {
        // TODO: Add support for XYZ color space conversion to RGB
        logError("HDR XYZ color space is not supported!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }

    // TODO: Add support for other formats like FormatEBGR_5_9_9_9_f
    if (format != Format::RGB_16_hf &&
        format != Format::RGB_32_f)
    {
        // TODO: Add ability to log any enum name and any other engine component / struct state!
        logError("Couldn't unpack file to expected format! Unsupported destination format!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }


    // ### Read file to memory


    // Read whole file at once to memory. 
    // Size aligned to multiple of 4KB Page Size, and allocated at such boundary (can be memory mapped).
    uint64 roundedSize = roundUp(fileSize, PageSize);
    if (roundedSize > 0xFFFFFFFF)
    {
        logError("HDR file size exceeds 4GB limit!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }

    uint8* content = allocate<uint8>(static_cast<uint32>(roundedSize), PageSize);
    if (!content)
    {
        logCritical("Run out of memory!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }

    if (!file->read(content))
    {
        logError("Couldn't read file to memory!\nFile: %s\n", fullPath.c_str());
        deallocate<uint8>(content);
        delete file;
        return false;
    }

    // Release file handle and work on copy in memory
    delete file;


    // ### Parse and decompress file 


    // Texture data is stored one scan line at a time, in one of three ways:
    // - uncompressed
    // - per-texel RLE (where a color with 255 for all three mantissas indicates a run of the last pixel value (exponent is the run count) => RGB=255, E=count)
    // - per-channel RLE (adaptive RLE)
    //
    // More details on file format and compression can be found below:
    // http://radsite.lbl.gov/radiance/refer/Notes/picture_format.html
    // http://paulbourke.net/dataformats/pic/
 
    // Intermediate allocation (target for uncompressed RGBE data, before quantization conversion)
    uint8* data = allocate<uint8>(width * height * 4, cacheline);
    memset(data, 0, width * height * 4);

    // Determine amount of scanlines and their length in source image
    uint32 lines = height;
    uint32 length = width;
    if (metadata.columnOrder)
    {
        lines = width;
        length = height;
    }

    uint64 offset = metadata.offsetToData;
    for(uint32 scanline=0; scanline<lines; ++scanline)
    {
        // If line length is supported by new compression method..
        bool newCompression = false;
        if (length >= MinLineLengthForRLE || length <= MaxLineLengthForRLE)
        {
            // Checks for presence of line header
            const LineHeader& lineHeader = *((const LineHeader*)(&content[offset]));
            if (lineHeader.unknown0 == 2 &&
                lineHeader.unknown1 == 2 &&
                lineHeader.reserved == 0 &&
                (lineHeader.lengthHi << 8 | lineHeader.lengthLo) == length)
            {
                // Its per-channel RLE compression.
                offset += sizeof(LineHeader);
                decompressPerChannelRLE(metadata, width, height, content, offset, scanline, length, data);

                newCompression = true;
            }
        }

        if (!newCompression)
        {
            decompressRLE(metadata, width, height, content, offset, scanline, length, data);
            continue;
        }
    }

    deallocate<uint8>(content);

    // Re-quantize texels from RGBE, XYZE into expected format and color space like Format::RGB_16_hf or other.
    for (uint32 texel = 0; texel < (height * width); ++texel)
    {
        float3 v3f32 = decodeRGBE((const uint8 (&)[4])data[texel * 4]);

        // TODO: Check if image is tightly packed, or if there are special alignment and take that into notice while writing to destination buffer
        if (format == Format::RGB_32_f)
        {
            *((float3*)(&destination[texel * sizeof(float3)])) = v3f32;
        }
        else
        if (format == Format::RGB_16_hf)
        {
            half& r = *((half*)(&destination[texel * 3 * sizeof(half)]));
            half& g = *((half*)(&destination[texel * 3 * sizeof(half) + 1 * sizeof(half)]));
            half& b = *((half*)(&destination[texel * 3 * sizeof(half) + 2 * sizeof(half)]));

            // Converts f32 to f16
            r = half(v3f32.r);
            g = half(v3f32.g);
            b = half(v3f32.b);
        }
    }

    deallocate<uint8>(data);

    return true;
}

} // en::hdr
} // en



/*
// Decompress RGB shared exponent to float RGB
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
//*/

/*
    // Check if data is really compressed with RLE
    bool rle = false;
    uint64 rawSize = width * height * 4;
    if ((fileSize - metadata.offsetToData) != rawSize)
    {
        rawSize = fileSize - metadata.offsetToData;
        rle = true;
    }
//*/

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




/*
    // Read file header
    char header[11];
    header[10] = 0;
    const char radiance[] = "#?RADIANCE";
    if (!file->read(0, 10, &header))
    {
        enLog << "ERROR: Not HDR file!\n";
        delete file;
        return std::shared_ptr<gpu::Texture>();
    }
    if (strcmp(radiance, header) != 0)
    {
        enLog << "ERROR: HDR file header signature incorrect!\n";
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
//*/