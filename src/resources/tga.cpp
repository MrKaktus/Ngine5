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
#include "resources/interface.h"
#include "resources/tga.h"    

#define PageSize 4096

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

// Reads first 4KB page of TGA file, and decodes it's header to TextureState and ColorSpace.
bool readMetadata(uint8* buffer, const uint32 readSize, gpu::TextureState& settings, gpu::ColorSpace& colorSpace)
{
    // Check if file has minimum required size
    uint32 minimumFileSize = sizeof(Header);
    if (readSize < minimumFileSize)
    {
        enLog << "ERROR: TGA file size too small!\n";
        return false;
    }

    // Read file header
    Header& header = *reinterpret_cast<Header*>(buffer);

    // Check for supported image format
    if (header.format != RGB &&
        header.format != RGB_RLE)
    {
        enLog << "ERROR: Unsupported texture format!\n";
        return false;
    }

    // Check if not paletted
    if (header.palette != 0)
    {
        enLog << "ERROR: Paletted TGA files are not supported!\n";
        return false;
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

    // There is no way to determine if TGA is storing data using sRGB transfer function
    colorSpace = gpu::ColorSpace::ColorSpaceLinear;

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
        enLog << "ERROR:  Unsupported texture format!\n";
        return false;
    }

    return true;
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
    File* file = Storage->open(filename);
    if (!file)
    {
        std::string fullPath = Resources->assetsPath() + filename;
        file = Storage->open(fullPath);
        if (!file)
        {
            logError("There is no such file!\n%s\n", fullPath.c_str());
            return nullptr;
        }
    }


    // ### Read file metadata


    // Read file first 4KB into single 4KB memory page
    uint32 readSize = min(file->size(), PageSize);
    uint8* buffer = allocate<uint8>(readSize, PageSize);
    file->read(0, readSize, buffer);

    // Read file properties
    TextureState settings;
    ColorSpace colorSpace; // TODO: Determine file Color Space and compare with expected
    bool success = readMetadata(buffer, readSize, settings, colorSpace);

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    if (!success)
    {
        delete file;
        return false;
    }

    // Verify that file matches expected properties
    if ((settings.width  != width) ||
        (settings.height != height) ||
        (settings.format != format))
    {
        delete file;
        return false;
    }


    // ### Read file to memory


    // Read whole file at once to memory. 
    // Size aligned to multiple of 4KB Page Size, and allocated at such boundary (can be memory mapped).
    uint64 fileSize = file->size();
    uint64 roundedSize = roundUp(fileSize, PageSize);
    uint8* content = allocate<uint8>(roundedSize, PageSize);
    if (!file->read(content))
    {
        enLog << "ERROR: Couldn't read file to memory.\n";
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
    uint32 dataSize  = settings.surfaceSize(0);

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
