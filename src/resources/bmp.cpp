/*

 Ngine v5.0
 
 Module      : BMP files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from bmp files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/utilities/parser.h"
#include "utilities/utilities.h"
#include "assets/assets.h"
#include "resources/bmp.h"

#include "core/rendering/device.h"

#include <assert.h>
#include <string>

#define PageSize 4096ull

namespace en
{
namespace bmp
{

// BMP file format is described here:
// https://docs.microsoft.com/en-us/windows/desktop/gdi/bitmap-header-types
// https://www.fileformat.info/format/bmp/egff.htm

enum Compression : uint32
{
    None          = 0,  // Uncompressed
    RLE8          = 1,  // 8bpp RLE
    RLE4          = 2,  // 4bpp RLE
    Huffman       = 3,  // Bit field / Huffman 1D 
    JPEG          = 4,  // JPEG / 24bpp RLE
    PNG           = 5,  // PNG
    AlphaBitField = 6
};

enum ColorSpaceType : uint32
{
    CalibratedRGB       = 0,  // Defined by the 1931 CIE XYZ standard
    DeviceDependentRGB  = 1,
    DeviceDependentCMYK = 2,
};

alignTo(1) 
struct Header
{
    uint16 signature;   // BMP file signature 'BM' -> 0x4D42
    uint32 size;        // File size in bytes
    uint32 reserved;    // Reserved
    uint32 dataOffset;  // Offset in file to image data
};

// Official documentation at:
// https://docs.microsoft.com/en-us/windows/desktop/api/Wingdi/ns-wingdi-tagbitmapcoreheader
//
struct DIBHeaderV2Win
{
    uint32      headerSize;  // DIB header size
    sint16      width;       // Image width
    sint16      height;      // Image height
    uint16      planes;      // Color planes (must be 1)
    uint16      bpp;         // Bits per pixel: 1,4,8,24
};

static_assert(sizeof(DIBHeaderV2Win) == 12, "en::bmp::DIBHeaderV2Win size mismatch!");

struct DIBHeaderV2OS2
{
    uint32      headerSize;  // DIB header size
    uint16      width;       // Image width
    uint16      height;      // Image height
    uint16      planes;      // Color planes (must be 1)
    uint16      bpp;         // Bits per pixel: 1,4,8,24
};

static_assert(sizeof(DIBHeaderV2OS2) == 12, "en::bmp::DIBHeaderV2OS2 size mismatch!");
 
// Official documentation at:
// https://docs.microsoft.com/en-us/previous-versions/dd183376(v%3Dvs.85)
//
struct DIBHeaderV3
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

static_assert(sizeof(DIBHeaderV3) == 40, "en::bmp::DIBHeaderV3 size mismatch!");

// Present after DIBHeaderV3, if 16 or 32bpp
struct DIBHeaderV3NT
{
    uint32 redMask;          // Red component bits
    uint32 greenMask;        // Green component bits
    uint32 blueMask;         // Blue component bits
};

// Official documentation at:
// https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/ns-wingdi-bitmapv4header
//
struct DIBHeaderV4 : public DIBHeaderV3
{
    uint32 redMask;          // Red component bits
    uint32 greenMask;        // Green component bits
    uint32 blueMask;         // Blue component bits
    uint32 alphaMask;        // Alpha component bits
    uint32 colorSpaceType;   // Color space
    sint32 redX;             // CIE X coordinate of red endpoint
    sint32 redY;             // CIE Y coordinate of red endpoint
    sint32 redZ;             // CIE Z coordinate of red endpoint
    sint32 greenX;           // CIE X coordinate of green endpoint
    sint32 greenY;           // CIE Y coordinate of green endpoint
    sint32 greenZ;           // CIE Z coordinate of green endpoint
    sint32 blueX;            // CIE X coordinate of blue endpoint
    sint32 blueY;            // CIE Y coordinate of blue endpoint
    sint32 blueZ;            // CIE Z coordinate of blue endpoint
    uint32 gammaRed;         // Gamma red coordinate scale value
    uint32 gammaGreen;       // Gamma green coordinate scale value
    uint32 gammaBlue;        // Gamma blue coordinate scale value
};

static_assert(sizeof(DIBHeaderV4) == 108, "en::bmp::DIBHeaderV4 size mismatch!");

// Official documentation at:
// https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/ns-wingdi-bitmapv5header
//
struct DIBHeaderV5 : public DIBHeaderV4
{
    uint32 intent;           //
    uint32 profileData;      // Offset from start of this header, to profile data
    uint32 profileSize;      // Profile data size
    uint32 reserved;         //
};

static_assert(sizeof(DIBHeaderV5) == 124, "en::bmp::DIBHeaderV5 size mismatch!");

alignToDefault

// Parses buffer storing metadata of BMP file, and decodes it's header to TextureState
ParsingResult parseMetadata(const uint8* buffer, const uint32 size, gpu::TextureState& settings)
{
    // Check if file has minimum required size
    uint32 minimumFileSize = sizeof(Header) + sizeof(DIBHeaderV2Win);
    if (size < minimumFileSize)
    {
        logError("BMP file size too small!\n");
        return ParsingResult::IncompleteData;
    }

    // Read file header
    const Header& header = *reinterpret_cast<const Header*>(buffer);
    if (header.signature != 0x4D42)
    {
        logError("BMP file header signature is incorrect!\n");
        return ParsingResult::InvalidFormat;
    }
    if (header.size < minimumFileSize)
    {
        logError("BMP file header is incorrect!\n");
        return ParsingResult::InvalidFormat;
    }

    // Detect present DIB header version based on it's size
    const uint32 headerSize = *reinterpret_cast<const uint32*>(buffer + sizeof(Header));

    // Check if image is not compressed
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        const DIBHeaderV3& DIBHeader = *reinterpret_cast<const DIBHeaderV3*>(buffer + sizeof(Header));
        if (DIBHeader.compression != None)
        {
            logError("Compressed BMP files are not supported!\n");
            return ParsingResult::Unsupported;
        }
    }

    // Set texture state to default
    settings.type    = gpu::TextureType::Texture2D;
    settings.format  = gpu::Format::Unsupported;
    settings.usage   = gpu::TextureUsage::Read;
    settings.width   = 1;
    settings.height  = 1;
    settings.layers  = 1;
    settings.mipmaps = 1;
    settings.samples = 1;

    // Determine texture resolution
    if (headerSize == sizeof(DIBHeaderV2Win))
    {
        const DIBHeaderV2Win& DIBHeader = *reinterpret_cast<const DIBHeaderV2Win*>(buffer + sizeof(Header));

        settings.width  = DIBHeader.width;
        settings.height = DIBHeader.height < 0 ? -DIBHeader.height : DIBHeader.height;
    }
    else
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        const DIBHeaderV3& DIBHeader = *reinterpret_cast<const DIBHeaderV3*>(buffer + sizeof(Header));

        settings.width  = DIBHeader.width;
        settings.height = DIBHeader.height < 0 ? -DIBHeader.height : DIBHeader.height;
    }

    // Determine stored texel format
    if (headerSize >= sizeof(DIBHeaderV2Win))
    {
        const DIBHeaderV2Win& DIBHeader = *reinterpret_cast<const DIBHeaderV2Win*>(buffer + sizeof(Header));

        if (DIBHeader.bpp == 24)
        {
            settings.format = gpu::Format::BGR_8;
        }
        else
        {
            // 1, 4, 8 bpp formats are not supported
            logError("Unsupported BMP bits per pixel %ubpp!\n", DIBHeader.bpp);
            return ParsingResult::Unsupported;
        }
    }
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        const DIBHeaderV3& DIBHeader = *reinterpret_cast<const DIBHeaderV3*>(buffer + sizeof(Header));

        if (DIBHeader.bpp == 32)
        {
            settings.format = gpu::Format::BGRA_8;
        }
        else
        {
            // 16bpp formats are not supported
            logError("Unsupported BMP bits per pixel: %ubpp!\n", DIBHeader.bpp);
            return ParsingResult::Unsupported;
        }
    }

    if (settings.format == gpu::Format::Unsupported)
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

    // There is no way to determine if BMP is storing data using linear or sRGB transfer function
    // TODO: Determine based on DIBHeaderV4
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
    ParsingResult result = parseMetadata(buffer, static_cast<uint32>(readSize), storedTextureState);

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    if (result != ParsingResult::Success)
    {
        delete file;
        return false;
    }

    // Verify that file matches expected properties
    if ((storedTextureState.width  != width)  ||
        (storedTextureState.height != height) ||
        (storedTextureState.format != format))
    {
        delete file;
        return false;
    }


    // ### Read file to memory


    // Read whole file at once to memory. 
    // Size aligned to multiple of 4KB Page Size, and allocated at such boundary (can be memory mapped).
    uint64 roundedSize = roundUp(fileSize, PageSize);
    if (roundedSize > 0xFFFFFFFF)
    {
        logError("BMP file size exceeds 4GB limit!\nFile: %s\n", fullPath.c_str());
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


    // Calculates size of raw data to read
    uint32 dataSize = 0;
    uint32 headerSize = *reinterpret_cast<uint32*>(content + sizeof(Header));
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        DIBHeaderV3& DIBHeader = *reinterpret_cast<DIBHeaderV3*>(content + sizeof(Header));
        dataSize = DIBHeader.size;
    }
    else
    {
        dataSize = storedTextureState.surfaceSize(0);
    }

    // Verify that data is correct
    Header& header = *reinterpret_cast<Header*>(content);
    if ( (header.size != fileSize) ||
         (header.dataOffset + dataSize > header.size) )
    {
        logError("File or its header is corrupted!\nFile: %s\n", fullPath.c_str());
        deallocate<uint8>(content);
        return false;
    }
    if (dataSize != alignment.surfaceSize(storedTextureState.width, storedTextureState.height))
    {
        logError("Data layout in memory is not matching expected layout in destination.\nFile: %s\n", fullPath.c_str());
        deallocate<uint8>(content);
        return false;
    }

    // Copy data
    memcpy(destination, content + header.dataOffset, dataSize);

    // Release temporary data
    deallocate<uint8>(content);
    return true;
}

bool save(
    const std::string& filename,
    const uint8* source,
    const uint32 width, 
    const uint32 height)
{
    using namespace en::storage;

    assert( width > 0 );
    assert( height > 0 );
    assert( source != nullptr );

    // Open image file 
    std::string fullPath = filename;
    File* file = Storage->open(fullPath, en::storage::Write);
    if (!file)
    {
        fullPath = Assets().screenshotsPath() + filename;
        file = Storage->open(fullPath, en::storage::Write);
        if (!file)
        {
            logError("Cannot create such file!\nFile: %s\n", fullPath.c_str());
            return false;
        }
    }

    uint32 headersSize = sizeof(Header) + sizeof(DIBHeaderV2Win);
    uint32 dataSize = width * height * 3;

    // Write file headers
    Header header;
    header.signature  = 0x4D42;
    header.size       = headersSize + dataSize;
    header.reserved   = 0;
    header.dataOffset = headersSize;
    file->write(0, 14, &header);

    DIBHeaderV2Win DIBHeader;
    DIBHeader.headerSize = sizeof(DIBHeaderV2Win);
    DIBHeader.width      = width;
    DIBHeader.height     = height;
    DIBHeader.planes     = 1;
    DIBHeader.bpp        = 24;
    file->write(14, sizeof(DIBHeaderV2Win), &DIBHeader);

    file->write(header.dataOffset, dataSize, (void*)(source));

    delete file;
    return true;
}

} // en::bmp
} // en
