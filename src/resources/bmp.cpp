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

#define PageSize 4096

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

// Reads first 4KB page of BMP file, and decodes it's header to TextureState and ColorSpace.
bool readMetadata(uint8* buffer, const uint32 readSize, gpu::TextureState& settings, gpu::ColorSpace& colorSpace)
{
    // Check if file has minimum required size
    uint32 minimumFileSize = sizeof(Header) + sizeof(DIBHeaderV2Win);
    if (readSize < minimumFileSize)
    {
        Log << "ERROR: BMP file size too small!\n";
        return false;
    }

    // Read file header
    Header& header = *reinterpret_cast<Header*>(buffer);
    if (header.signature != 0x4D42)
    {
        Log << "ERROR: BMP file header signature is incorrect!\n";
        return false;
    }
    if (header.size < minimumFileSize)
    {
        Log << "ERROR: BMP file header is incorrect!\n";
        return false;
    }

    // Detect present DIB header version based on it's size
    uint32 headerSize = *reinterpret_cast<uint32*>(buffer + sizeof(Header));

    // Check if image is not compressed
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        DIBHeaderV3& DIBHeader = *reinterpret_cast<DIBHeaderV3*>(buffer + sizeof(Header));
        if (DIBHeader.compression != None)
        {
            Log << "ERROR: Compressed BMP files are not supported!\n";
            return false;
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

    // There is no way to determine if BMP is storing data using sRGB transfer function
    colorSpace = gpu::ColorSpace::ColorSpaceLinear;

    // Determine texture resolution
    if (headerSize == sizeof(DIBHeaderV2Win))
    {
        DIBHeaderV2Win& DIBHeader = *reinterpret_cast<DIBHeaderV2Win*>(buffer + sizeof(Header));

        settings.width  = DIBHeader.width;
        settings.height = DIBHeader.height < 0 ? -DIBHeader.height : DIBHeader.height;
    }
    else
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        DIBHeaderV3& DIBHeader = *reinterpret_cast<DIBHeaderV3*>(buffer + sizeof(Header));

        settings.width  = DIBHeader.width;
        settings.height = DIBHeader.height < 0 ? -DIBHeader.height : DIBHeader.height;
    }

    // Determine stored texel format
    if (headerSize >= sizeof(DIBHeaderV2Win))
    {
        DIBHeaderV2Win& DIBHeader = *reinterpret_cast<DIBHeaderV2Win*>(buffer + sizeof(Header));

        // 1, 4, 8 bpp formats are not supported
        if (DIBHeader.bpp == 24)
        {
            settings.format = gpu::Format::BGR_8;
        }
    }
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        DIBHeaderV3& DIBHeader = *reinterpret_cast<DIBHeaderV3*>(buffer + sizeof(Header));

        // 16bpp formats are not supported
        if (DIBHeader.bpp == 32)
        {
            settings.format = gpu::Format::BGRA_8;
        }
    }

    if (settings.format == gpu::Format::Unsupported)
    {
        Log << "ERROR: Unsupported texture format!\n";
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
    std::shared_ptr<File> file = Storage->open(filename);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.textures + filename);
        if (!file)
        {
            Log << en::ResourcesContext.path.textures + filename << std::endl;
            Log << "ERROR: There is no such file!\n";
            return false;
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
        file = nullptr;
        return false;
    }

    // Verify that file matches expected properties
    if ((settings.width  != width)  ||
        (settings.height != height) ||
        (settings.format != format))
    {
        file = nullptr;
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
        Log << "ERROR: Couldn't read file to memory.\n";
        deallocate<uint8>(content);
        file = nullptr;
        return false;
    }

    // Release file handle and work on copy in memory
    file = nullptr;


    // ### Parse and decompress file 


    // Calculate size of raw data to read
    uint32 dataSize = 0;
    uint32 headerSize = *reinterpret_cast<uint32*>(content + sizeof(Header));
    if (headerSize >= sizeof(DIBHeaderV3))
    {
        DIBHeaderV3& DIBHeader = *reinterpret_cast<DIBHeaderV3*>(content + sizeof(Header));
        dataSize = DIBHeader.size;
    }
    else
    {
        dataSize = settings.surfaceSize(0);
    }

    // Verify that data is correct
    Header& header = *reinterpret_cast<Header*>(content);
    if ( (header.size != fileSize) ||
         (header.dataOffset + dataSize > header.size) )
    {
        Log << "ERROR: File or its header is corrupted.\n";
        deallocate<uint8>(content);
        return false;
    }
    if (dataSize != alignment.surfaceSize(settings.width, settings.height))
    {
        Log << "ERROR: Data layout in memory is not matching expected layout in destination.\n";
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
    std::shared_ptr<File> file = Storage->open(filename, en::storage::Write);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.screenshots + filename, en::storage::Write);
        if (!file)
        {
            Log << en::ResourcesContext.path.screenshots + filename << std::endl;
            Log << "ERROR: Cannot create such file!\n";
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
    return true;
}

} // en::bmp
} // en
