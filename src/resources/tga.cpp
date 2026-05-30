/*

 Ngine v5.0
 
 Module      : TGA files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from tga files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/utilities/parser.h"
#include "core/rendering/device.h"
#include "utilities/utilities.h"
#include "assets/assets.h"
#include "resources/tga.h"    

#define PageSize 4096ull

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

alignTo(1) 
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
alignToDefault

// Parses buffer storing metadata of TGA file, and decodes it's header to TextureState
ParsingResult parseMetadata(uint8* buffer, const uint32 size, gpu::TextureState& settings)
{
    // Check if file has minimum required size
    uint32 minimumFileSize = sizeof(Header);
    if (size < minimumFileSize)
    {
        logError("TGA file size too small!\n");
        return ParsingResult::IncompleteData;
    }

    // Read file header
    Header& header = *reinterpret_cast<Header*>(buffer);

    // Check for supported image format
    if (header.format != RGB &&
        header.format != RGB_RLE)
    {
        logError("Unsupported texture format!\n");
        return ParsingResult::Unsupported;
    }

    // Check if not paletted
    if (header.palette != 0)
    {
        logError("Paletted TGA files are not supported!\n");
        return ParsingResult::Unsupported;
    }

    // Set texture state
    settings.type    = gpu::TextureType::Texture2D;
    settings.format  = gpu::Format::Unsupported;
    settings.usage   = gpu::TextureUsage::Read;
    settings.width   = header.width;
    settings.height  = header.height;
    settings.layers  = 1;
    settings.mipmaps = 1;
    settings.samples = 1;

    // Determine stored texel format
    if (header.bpp == 24)
    {
        settings.format = gpu::Format::BGR_8;
    }
    else
    if (header.bpp == 32)
    {
        settings.format = gpu::Format::BGRA_8;
    }
    else
    {
        logError("Unsupported texture format!\n");
        return ParsingResult::Unsupported;
    }

    return ParsingResult::Success;
}

bool loadMetadata(
    const std::string& filename,
    gpu::TextureState& storedTextureState,
    gpu::ColorSpace& storedColorSpace)
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
    ParsingResult result = parseMetadata(buffer, static_cast<uint32>(readSize), storedTextureState);

    // There is no way to determine if TGA is storing data using sRGB transfer function.
    // It depends purely on context (normal maps linear, color sRGB, etc.).
    // TODO: Take that into notice while comparing stored format with expected one.
    storedColorSpace = ColorSpace::Unknown;

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    return (result == ParsingResult::Success);
}

bool load(
    const std::string& filename,
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
    uint32 readSize = min(file->size(), PageSize);
    uint8* buffer = allocate<uint8>(readSize, PageSize);
    if (!buffer)
    {
        logCritical("Run out of memory!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }
    file->read(0, readSize, buffer);

    // Read file properties
    TextureState storedTextureState;
    ParsingResult result = parseMetadata(buffer, static_cast<uint32>(readSize), storedTextureState);

    // There is no way to determine if TGA is storing data using sRGB transfer function.
    // It depends purely on context (normal maps linear, color sRGB, etc.).
    // TODO: Take that into notice while comparing stored format with expected one.

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    if (result != ParsingResult::Success)
    {
        delete file;
        return false;
    }

    // Verify that file matches expected properties
    if ((storedTextureState.width  != width) ||
        (storedTextureState.height != height) ||
        (storedTextureState.format != format))
    {
        delete file;
        return false;
    }


    // ### Read file to memory


    // Read whole file at once to memory. 
    // Size aligned to multiple of 4KB Page Size, and allocated at such boundary (can be memory mapped).
    uint64 fileSize = file->size();
    uint64 roundedSize = roundUp(fileSize, PageSize);
    if (roundedSize > 0xFFFFFFFF)
    {
        logError("TGA file size exceeds 4GB limit!\nFile: %s\n", fullPath.c_str());
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


    // Calculate size of raw data to read
    Header& header = *reinterpret_cast<Header*>(content);
    uint32 srcOffset = sizeof(Header) + header.idSize;
    uint32 dataSize  = storedTextureState.surfaceSize(0);

    // Copy data
    if (header.format == RGB)
    {
        memcpy(destination, content + srcOffset, dataSize);
    }
    else // Decompress data
    if (header.format == RGB_RLE)
    {
        uint8  counter;
        uint32 dstOffset = 0;
        uint8  texelSize = header.bpp / 8;
        uint8* texel = new uint8[texelSize];

        while (dstOffset < dataSize)
        {
            // Read RLE header
            counter = *reinterpret_cast<uint8*>(content + srcOffset);
            srcOffset++;

            // Run-length packet
            if (counter & 0x80)
            {
                counter -= 127;
                memcpy(texel, content + srcOffset, texelSize);
                srcOffset += texelSize;

                // Repeat given texel N times
                for(uint8 i=0; i<counter; ++i)
                {
                    memcpy(destination + dstOffset, texel, texelSize);
                    dstOffset += texelSize;
                }
            }
            // Non-run-length packet
            else
            {
                counter++;
                for(uint8 i=0; i<counter; ++i)
                {
                    memcpy(destination + dstOffset, content + srcOffset, texelSize);
                    dstOffset += texelSize;
                    srcOffset += texelSize;
                }
            }
        }

        delete[] texel;
    }

    // Release temporary data
    deallocate<uint8>(content);
    return true;
}

} // en::tga
} // en
