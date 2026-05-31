/*

 Ngine v5.0
 
 Module      : EXR files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading images from *.exr files.

*/

#include "core/types/sint32v2.h"
#include "core/storage.h"
#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/utilities/parser.h"
#include "utilities/utilities.h"
#include "assets/assets.h"
#include "resources/exr.h"

#include "core/rendering/device.h"

#include "parallel/scheduler.h"  // For multithreaded decode

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#include "zlib.h"
#endif

#include <cstddef>
#include <string>
#include <vector>

namespace en
{
namespace exr
{

#define PageSize 4096ull

// See "Compression Methods" section in:
// https://en.wikipedia.org/wiki/OpenEXR
enum Compression
{                       // Compression:  Deep-data:      Chunk size (scanlines):
    None          = 0,  // Uncompressed  (all types)     1
    RLE           = 1,  // Lossless      (all types)     1
    ZIPS          = 2,  // Lossless      (all types)     1   (ZIP Scanline)
    ZIP           = 3,  // Lossless      (all types)     16
    PIZ           = 4,  // Lossless      (no deep data)  32  TODO: Apparently supports deep data!
    PXR24         = 5,  // Lossy         (no deep data)  16  TODO: Apparently supports deep data!
    B44           = 6,  // Lossy         (no deep data)  32
    B44A          = 7,  // Lossy         (no deep data)  32
    DWAA          = 8,  // Lossy         (no deep data)  1   (DreamWorks Animation lossy JPEG-like compression)
    DWAB          = 9,  // Lossy         (no deep data)  32  (DreamWorks Animation lossy JPEG-like compression)
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

alignTo(1)
struct Header
{
    uint32 signature;         // EXR file signature 0x01312F76 (Little Endian)
   
    uint32 version      : 8;
    uint32 singleTile   : 1;  // Is single part file with tiles?
    uint32 longNames    : 1;  // Does file contain long names?
    uint32 containsData : 1;  // Is file containing non image data?
    uint32 multiPart    : 1;  // Does file contain multiple parts?
    uint32 reserved     : 20; // Must be zero
};
alignToDefault

struct Channel
{
    std::string name;         // 1-255 length (including terminating null)
    uint32 type;              // Pixel type: 0-uint32 1-half 2-float
    uint8  pLinear;
    uint32 xSampling;         // Channel spatial sampling rate in X and Y axis relative to image resolution.
    uint32 ySampling;         // Values bigger than 1 mean that channel samples are more sparse.
};

struct Box2i
{
    sint32v2 min;  // Lower-left corner
    sint32v2 max;  // Upper-right corner
};

struct TileDescription
{
    uint32 width;    // Tile size in texels
    uint32 height;
    uint8  mode;
    uint8  rounding;
};

struct PartHeader
{
    Box2i    displayWindow;      // Presentation area
    Box2i    dataWindow;         // Data region in presentation area
    float2   screenWindowCenter;
    float    screenWindowWidth;
    float    pixelAspect;        // Should be 1.0
    Channel* channels;           // Channels description array
    uint8    channelsCount;
    uint8    compression;
    uint8    lineOrder;
    std::string comment;
    std::string name;
    PartType type;
    uint32   version;
    uint64*  chunksOffsets;
    sint32   chunksCount;
    uint32   maxSamplesPerPixel;
    TileDescription tile;
    gpu::TextureState storedTextureState;
    bool     supported; // True if engine supports current part configuration (based on this header)

    PartHeader();
   ~PartHeader();

    bool calculateChunksCount(void);

    bool determineFormat(gpu::Format& format, bool& channelsWithDifferentFormats) const;
    uint8  channelSize(void) const;
    uint32 dataBlockHeight(void) const;
    uint32 dataBlockSize(void) const;
    uint32 dataWindowWidth(void) const;
    uint32 dataWindowHeight(void) const;
    uint32 displayWindowWidth(void) const;
    uint32 displayWindowHeight(void) const;
};

PartHeader::PartHeader() :
    displayWindow(),
    dataWindow(),
    screenWindowCenter(),
    screenWindowWidth(0.0f),
    pixelAspect(1.0f),
    channels(nullptr),
    channelsCount(0),
    compression(0),
    lineOrder(0),
    comment(""),
    name(""),
    type(ScanLineImage),
    version(1),
    chunksOffsets(nullptr),
    chunksCount(0),
    maxSamplesPerPixel(1),
    // TileDescription tile;
    // gpu::TextureState storedTextureState;
    supported(false)
{
}

PartHeader::~PartHeader()
{
    if (channels)
    {
        delete [] channels;
        channels = nullptr;
    }

    if (chunksOffsets)
    {
        delete [] chunksOffsets;
        chunksOffsets = nullptr;
    }
}

bool PartHeader::calculateChunksCount(void)
{
    if (chunksCount > 0)
    {
        return true;
    }

    // Single part files can have missing chunkCount field.
    // In such situation chunks count needs to be computed.
    if (type == ScanLineImage)
    {
        // Handles also negative data window
        uint32 height = dataWindowHeight();

        chunksCount = intDivUp(height, dataBlockHeight());
        return true;
    }

    // TODO: Determine chunks count for other types
    // - dataWindow
    // - tileDesc
    // - compression
    assert(0);
    return false;
}

bool PartHeader::determineFormat(gpu::Format& format, bool& channelsWithDifferentFormats) const
{
    // Determines texture format to use. At this stage,
    // it doesn't matter what chnnels really represent.
    // It only matters what format they are all stored in.
    // Channels are stored in alphabetical order anyway.
    // We can order them in GPU memory the way we want.
    channelsWithDifferentFormats = false;
    if (channelsCount == 1)
    {
        if (channels[0].type == 0) { format = gpu::Format::R_32_u;  return true; }
        if (channels[0].type == 1) { format = gpu::Format::R_16_hf; return true; }
        if (channels[0].type == 2) { format = gpu::Format::R_32_f;  return true; }

        // Unknown pixel format
        return false;
    }
    else
    if (channelsCount == 2)
    {
        if (channels[0].type == channels[1].type)
        {
            if (channels[0].type == 0) { format = gpu::Format::RG_32_u;  return true; }
            if (channels[0].type == 1) { format = gpu::Format::RG_16_hf; return true; }
            if (channels[0].type == 2) { format = gpu::Format::RG_32_f;  return true; }
         
            // Unknown pixel format
            return false;
        }
        else
        {
            channelsWithDifferentFormats = true;
            return false;
        }
    }
    else
    if (channelsCount == 3)
    {
        if (channels[0].type == channels[1].type &&
            channels[1].type == channels[2].type)
        {
            if (channels[0].type == 0) { format = gpu::Format::RGB_32_u;  return true; }
            if (channels[0].type == 1) { format = gpu::Format::RGB_16_hf; return true; }
            if (channels[0].type == 2) { format = gpu::Format::RGB_32_f;  return true; }

            // Unknown pixel format
            return false;
        }
        else
        {
            channelsWithDifferentFormats = true;
            return false;
        }
    }
    else
    if (channelsCount == 4)
    {
        if (channels[0].type == channels[1].type &&
            channels[1].type == channels[2].type &&
            channels[2].type == channels[3].type)
        {
            if (channels[0].type == 0) { format = gpu::Format::RGBA_32_u;  return true; }
            if (channels[0].type == 1) { format = gpu::Format::RGBA_16_hf; return true; }
            if (channels[0].type == 2) { format = gpu::Format::RGBA_32_f;  return true; }

            // Unknown pixel format
            return false;
        }
        else
        {
            channelsWithDifferentFormats = true;
            return false;
        }
    }

    // Unsupported channels count
    return false;
}

uint8 PartHeader::channelSize(void) const
{
    // By default its assumed to be 4 bytes (as it can be uint32 or float).
    // For type "1" - half-float its 2 bytes. Other types are not known at
    // the time of writing this code.
    assert(type <= 2);
    uint32 size = 4;
    if (type == 1)
    {
        size = 2;
    }

    return size;
}

uint32 PartHeader::dataBlockHeight(void) const
{
    if (type == ScanLineImage)
    {
        if (compression == None ||
            compression == RLE  ||
            compression == ZIPS ||
            compression == DWAA)
        {
            return 1;
        }
        else // Compressed blocks of 16 lines:
        if (compression == ZIP ||
            compression == PXR24)
        {
            return 16;
        }
        else // Compressed blocks of 32 lines:
        if (compression == PIZ  ||
            compression == B44  ||
            compression == B44A ||
            compression == DWAB)
        {
            return 32;
        }
        else
        {
            // Unrecognized compression type
            assert(0);
            return 1;
        }
    }

    // TODO: Determine chunks count for other types
    // - dataWindow
    // - tileDesc
    // - compression
    assert(0);
    return 1;
}

uint32 PartHeader::dataBlockSize(void) const
{
    uint32 pixelSize = channelsCount * channelSize();

    // Calculates final block size
    return dataWindowWidth() * dataBlockHeight() * pixelSize;
}

// Data and display windows size is counted from zero,
// which means it need to be increased by one to get
// proper image size.
uint32 PartHeader::dataWindowWidth(void) const
{
    return (dataWindow.max.x - dataWindow.min.x) + 1;
}

uint32 PartHeader::dataWindowHeight(void) const
{
    return (dataWindow.max.y - dataWindow.min.y) + 1;
}

uint32 PartHeader::displayWindowWidth(void) const
{
    return (displayWindow.max.x - displayWindow.min.x) + 1;
}

uint32 PartHeader::displayWindowHeight(void) const
{
    return (displayWindow.max.y - displayWindow.min.y) + 1;
}

struct HeaderAttribute
{
    const char* name;  // Attribute name as null terminated string
    const char* type;  // Attribute type as null terminated string
    uint32 size;       // Size of data
    uint32 offset;     // Offset in PartHeader structure
};

const HeaderAttribute attribute[] = 
{
    // Name                  Type          Size   Offset
    { "displayWindow",      "box2i",       16, offsetof(PartHeader, displayWindow) },
    { "dataWindow",         "box2i",       16, offsetof(PartHeader, dataWindow) },
    { "pixelAspectRatio",   "float",       4,  offsetof(PartHeader, pixelAspect) },
    { "compression",        "compression", 1,  offsetof(PartHeader, compression) },
    { "lineOrder",          "lineOrder",   1,  offsetof(PartHeader, lineOrder) },
    { "screenWindowWidth",  "float",       4,  offsetof(PartHeader, screenWindowWidth) },
    { "screenWindowCenter", "v2f",         8,  offsetof(PartHeader, screenWindowCenter) },
    { "version",            "int",         4,  offsetof(PartHeader, version) },
    { "chunkCount",         "int",         4,  offsetof(PartHeader, chunksCount) },
    { "maxSamplesPerPixel", "int",         4,  offsetof(PartHeader, maxSamplesPerPixel) },
    { "tiles",              "tiledesc",    10, offsetof(PartHeader, tile) }
};
// Attributes of varying size need to be handled separately
// - "channels", "chlist"

#define attributesCount ( sizeof(attribute) / sizeof(HeaderAttribute) )

bool CheckError(sint32 code)
{
    if (code == Z_OK)
    {
        return false;
    }

    if (code > 0)
    {
        return false;
    }

    if (code == Z_ERRNO)
    {
        logError("Zlib cannot read chunk!\n");
    }
    else
    if (code == Z_STREAM_ERROR)
    {
        logError("Zlib invalid compression level!\n");
    }
    else
    if (code == Z_DATA_ERROR)
    {
        logError("Zlib invalid or incomplete chunk!\n");
    }
    else
    if (code == Z_MEM_ERROR)
    {
        logError("Zlib reports out of memory!\n");
    }
    else
    if (code == Z_BUF_ERROR)
    {
        logError("Zlib output buffer is to small!\n");
    }
    else
    if (code == Z_VERSION_ERROR)
    {
        logError("Zlib version mismatch!\n");
    }
    else
    {
        logError("Zlib unknown error code %i!\n", code);
    }

    return true;
}

forceinline uint16 calculateMaximumStringLength(const uint64 offset, const uint64 size, const bool longNames)
{
    // Calculates max name length (including null terminating zero).
    uint64 remaining = size - offset;
    if (longNames)
    {
        return (uint16)min(255ull, remaining);
    }

    return (uint16)min(32ull, remaining);
}

ParsingResult parseMetadata(const uint8* buffer, const uint32 size, std::vector<PartHeader>& headers, gpu::TextureState& storedTextureState, uint64& offsetToData)
{
    uint64 offset = 0;

    if (size < sizeof(Header))
    {
        return ParsingResult::IncompleteData;
    }

    // Parses file header
    const Header& header = *reinterpret_cast<const Header*>(&buffer[0]);
    offset += sizeof(Header);
    if (header.signature != 0x01312F76)
    {
        logError("EXR file header signature incorrect!\n");
        return ParsingResult::InvalidFormat;
    }

    // Determines parts count
    uint32 partsCount = 1;
    if (header.multiPart)
    {
        if (size < offset + sizeof(uint32))
        {
            return ParsingResult::IncompleteData;
        }

        partsCount = *reinterpret_cast<const uint32*>(&buffer[offset]);
        offset += sizeof(uint32);

        // TODO: This should be moved to engine specific verification step
        // TODO: Support multi-part types
        logError("EXR multi-part files are not supported!\n");
        return ParsingResult::Unsupported;
    }

    headers.reserve(partsCount);

    // If Single Part, determine part type
    PartType singlePartType = ScanLineImage;
    if (!header.multiPart)
    {
        if (!header.singleTile && !header.containsData)
        {
            singlePartType = ScanLineImage;
        }
        if (header.singleTile && !header.containsData)
        {
            singlePartType = TiledImage;
        }
        if (!header.singleTile && header.containsData)
        {
            singlePartType = DeepScanLine;
        }
        if (header.singleTile && header.containsData)
        {
            singlePartType = DeepTile;
        }
    }

    // TODO: This should be moved to engine specific verification step
    // TODO: Support non scan line types
    if (singlePartType != ScanLineImage)
    {
        logError("Engine supports only scan lined EXR images!\n");
        return ParsingResult::Unsupported;
    }

    // Reads parts headers
    // (each part is one surface, can be stored as collection of "chunks")
    for(uint32 i=0; i<partsCount; ++i)
    {
        // Reads part header attributes
        PartHeader partHeader;
        while(true)
        {
            // Attribute "name"
            uint64 nameOffset = offset;
            uint64 nameLength = 0;
            uint32 maxStringLength = calculateMaximumStringLength(offset, size, header.longNames);
            ParsingResult result = parseString(buffer + offset, maxStringLength, nameLength);
            if (result != ParsingResult::Success)
            {
                return result;
            }
            offset += nameLength;

            if (nameLength == 1)
            {
                // Attribute name is an empty string which indicates end of current part header
                break;
            }

            // Attribute "type"
            uint64 typeOffset = offset;
            uint64 typeLength = 0;
            maxStringLength = calculateMaximumStringLength(offset, size, header.longNames);
            result = parseString(buffer + offset, maxStringLength, typeLength);
            if (result != ParsingResult::Success)
            {
                return result;
            }
            offset += typeLength;

            // Attribute size
            if (size - offset < 4)
            {
                return ParsingResult::IncompleteData;
            }
            const uint32 attributeSize = *(reinterpret_cast<const uint32*>(buffer + offset));
            offset += 4;

            // Atrribute value
            if (size - offset < attributeSize)
            {
                return ParsingResult::IncompleteData;
            }

            bool read = false;
            for (uint8 j = 0; j < attributesCount; ++j)
            {
                if (attributeSize == attribute[j].size &&
                    (strcmp((const char*)(buffer + nameOffset), attribute[j].name) == 0) &&
                    (strcmp((const char*)(buffer + typeOffset), attribute[j].type) == 0))
                {
                    void* dst = reinterpret_cast<void*>(reinterpret_cast<uint64>(&partHeader) + attribute[j].offset);
                    memcpy(dst, (const void*)(buffer + offset), attributeSize);
                    offset += attributeSize;

                    read = true;
                    break;
                }
            }

            // Read values of variable length types
            if (!read)
            {
                // Channels
                if ((strcmp((const char*)(buffer + nameOffset), "channels") == 0) &&
                    (strcmp((const char*)(buffer + typeOffset), "chlist") == 0))
                {
                    std::vector<Channel> channels;
                    Channel channel;

                    uint64 channelsDataEnd = offset + attributeSize;
                    while(offset < channelsDataEnd)
                    {
                        // Channel "name"
                        uint64 channelNameOffset = offset;
                        uint64 channelNameLength = 0;
                        uint32 maxStringLength = calculateMaximumStringLength(offset, size, header.longNames);
                        ParsingResult result = parseString(buffer + offset, maxStringLength, channelNameLength);
                        if (result != ParsingResult::Success)
                        {
                            return result;
                        }

                        channel.name = std::string((const char*)(buffer + offset), channelNameLength);
                        offset += channelNameLength;

                        // Channel data block is composed of 16 bytes
                        if (offset + 16 > size)
                        {
                            return ParsingResult::IncompleteData;
                        }

                        // Channel "type"
                        channel.type = *reinterpret_cast<const uint32*>(buffer + offset);
                        offset += 4;

                        // Channel linear
                        channel.pLinear = *reinterpret_cast<const uint8*>(buffer + offset);
                        offset += 4; // pLinear field size + 3 padding bytes

                        // Channel sampling
                        channel.xSampling = *reinterpret_cast<const uint32*>(buffer + offset);
                        offset += 4;
                        channel.ySampling = *reinterpret_cast<const uint32*>(buffer + offset);
                        offset += 4;

                        channels.push_back(channel);
                    }

                    partHeader.channelsCount = static_cast<uint8>(channels.size());
                    partHeader.channels = new Channel[partHeader.channelsCount];
                    for (uint8 j = 0; j < partHeader.channelsCount; ++j)
                    {
                        partHeader.channels[j] = channels[j];
                    }
                }
/*
                else // Tiles
                if ((strcmp((const char*)(buffer + nameOffset), "tiles") == 0) &&
                    (strcmp((const char*)(buffer + typeOffset), "tiledesc") == 0))
                {
                    // TODO: Decode Tile descriptor
                    assert( 0 );

                    // Could skipping this attribute
                    // offset += attributeSize;
                }
//*/
                else // Comment
                if ((strcmp((const char*)(buffer + nameOffset), "comment") == 0) &&
                    (strcmp((const char*)(buffer + typeOffset), "string") == 0))
                {
                    uint64 commentOffset = offset;
                    uint64 commentLength = 0;
                    uint32 maxStringLength = calculateMaximumStringLength(offset, size, header.longNames);
                    if (maxStringLength < attributeSize) // unlikely
                    {
                        logError("EXR: Comment attribute data block size is bigger than max allowed string size!\n");
                        return ParsingResult::InvalidFormat;
                    }
                    ParsingResult result = parseString(buffer + offset, maxStringLength, commentLength);
                    if (result != ParsingResult::Success)
                    {
                        return result;
                    }
                    if (commentLength != attributeSize) // unlikely
                    {
                        logError("EXR: Comment attribute data block size (%u) is not matching extracted string size (%u)!\n", attributeSize, commentLength);
                        return ParsingResult::InvalidFormat;
                    }

                    partHeader.comment = std::string((const char*)(buffer + offset), attributeSize);
                    logDebug("EXR comment: %s\n", (const char*)(buffer + offset));

                    offset += commentLength;
                }
                else // Multipart Name
                if ((strcmp((const char*)(buffer + nameOffset), "name") == 0) &&
                    (strcmp((const char*)(buffer + typeOffset), "string") == 0))
                {
                    uint64 partNameOffset = offset;
                    uint64 partNameLength = 0;
                    uint32 maxStringLength = calculateMaximumStringLength(offset, size, header.longNames);
                    if (maxStringLength < attributeSize) // unlikely
                    {
                        logError("EXR: Part name attribute data block size is bigger than max allowed string size!\n");
                        return ParsingResult::InvalidFormat;
                    }
                    ParsingResult result = parseString(buffer + offset, maxStringLength, partNameLength);
                    if (result != ParsingResult::Success)
                    {
                        return result;
                    }
                    if (partNameLength != attributeSize) // unlikely
                    {
                        logError("EXR: Part name attribute data block size (%u) is not matching extracted string size (%u)!\n", attributeSize, partNameLength);
                        return ParsingResult::InvalidFormat;
                    }

                    partHeader.name = std::string((const char*)(buffer + offset), attributeSize);
                    logDebug("EXR part name: %s\n", (const char*)(buffer + offset));

                    offset += partNameLength;
                }
                else // Multipart Type
                if ((strcmp((const char*)(buffer + nameOffset), "type") == 0) &&
                    (strcmp((const char*)(buffer + typeOffset), "string") == 0))
                {
                    uint64 partTypeOffset = offset;
                    uint64 partTypeLength = 0;
                    uint32 maxStringLength = calculateMaximumStringLength(offset, size, header.longNames);
                    if (maxStringLength < attributeSize) // unlikely
                    {
                        logError("EXR: Part type attribute data block size is bigger than max allowed string size!\n");
                        return ParsingResult::InvalidFormat;
                    }
                    ParsingResult result = parseString(buffer + offset, maxStringLength, partTypeLength);
                    if (result != ParsingResult::Success)
                    {
                        return result;
                    }
                    if (partTypeLength != attributeSize) // unlikely
                    {
                        logError("EXR: Part type attribute data block size (%u) is not matching extracted string size (%u)!\n", attributeSize, partTypeLength);
                        return ParsingResult::InvalidFormat;
                    }

                    if (strcmp((const char*)(buffer + partTypeOffset), "scanlineimage") == 0)
                    {
                        partHeader.type = ScanLineImage;
                    }
                    else
                    if (strcmp((const char*)(buffer + partTypeOffset), "tiledimage") == 0)
                    {
                        partHeader.type = TiledImage;
                    }
                    else
                    if (strcmp((const char*)(buffer + partTypeOffset), "deepscanline") == 0)
                    {
                        partHeader.type = DeepScanLine;
                    }
                    else
                    if (strcmp((const char*)(buffer + partTypeOffset), "deeptile") == 0)
                    {
                        partHeader.type = DeepTile;
                    }

                    offset += partTypeLength;
                }
                else
                {
                    logDebug("EXR: Skipping unsupported attribute. Name: %s type: %s\n", (const char*)(buffer + nameOffset), (const char*)(buffer + typeOffset));
                    offset += attributeSize;
                }
            }
        }

        // Calculate chunks count if it wasn't specified
        if (!partHeader.calculateChunksCount())
        {
            logError("Unable to determine chunks count for part %u.\n", i);
            return ParsingResult::InvalidFormat;
        }

        headers.push_back(partHeader);
    }

    // All part headers are read out, there should be followup null byte confirming it
    if (header.multiPart)
    {
        if (offset + 1 > size)
        {
            return ParsingResult::IncompleteData;
        }

        // In multi-part file, last part header is followed by null
        // (otherwise its first byte of following part header which cannot be zero)
        uint8 zero = *reinterpret_cast<const uint8*>(buffer + offset);
        offset++;

        if (zero != 0)
        {
            logError("EXR: Expected null byte after last part header!\n");
            return ParsingResult::InvalidFormat;
        }
    }

    // After part headers, for each part there is array of chunk offsets
    for (uint32 part = 0; part < headers.size(); ++part)
    {
        PartHeader& partHeader = headers[part];

        uint32 blockSize = partHeader.dataBlockSize();
        uint32 blockLines = partHeader.dataBlockHeight();

        // Arrays of chunk offsets (and thus their entries too) need to be placed at 8 bytes boundary in memory.
        // This means its expected that they start at 8 bytes padded offset in file too.
        assert(offset % alignof(uint64) == 0);
        assert(reinterpret_cast<const uint64>(buffer + offset) % alignof(uint64) == 0);
        const uint64* chunkOffsets = reinterpret_cast<const uint64*>(buffer + offset);
        if (offset + (partHeader.chunksCount * sizeof(uint64)) > size)
        {
            return ParsingResult::IncompleteData;
        }

        // Copies offsets to all chunks into given part header
        partHeader.chunksOffsets = new uint64[partHeader.chunksCount];
        for(sint32 i=0; i<partHeader.chunksCount; ++i)
        {
            partHeader.chunksOffsets[i] = chunkOffsets[i];
        }

        offset += (partHeader.chunksCount * sizeof(uint64));
    }

    offsetToData = offset;

    // If Single Part, set part type
    if (!header.multiPart)
    {
        headers[0].type = singlePartType;
    }

    // Parses parts headers to determine texture state of each part
    for(uint32 part=0; part<headers.size(); ++part)
    {
        PartHeader& partHeader = headers[part];

        // Check if compression is supported
        if (partHeader.compression != None &&
            partHeader.compression != ZIP)
        {
            if (partHeader.compression == RLE)   logError("Engine doesn't supports EXR images with RLE compression!\n");
            if (partHeader.compression == ZIPS)  logError("Engine doesn't supports EXR images with ZIPS compression!\n");
            if (partHeader.compression == PIZ)   logError("Engine doesn't supports EXR images with PIZ compression!\n");
            if (partHeader.compression == PXR24) logError("Engine doesn't supports EXR images with PXR24 compression!\n");
            if (partHeader.compression == B44)   logError("Engine doesn't supports EXR images with B44 compression!\n");
            if (partHeader.compression == B44A)  logError("Engine doesn't supports EXR images with B44A compression!\n");
            logError("       Can't load part %u from file.\n", part);
            continue;
        }

        // Check if standard data window
        if (partHeader.dataWindow.min.x != partHeader.displayWindow.min.x ||
            partHeader.dataWindow.min.y != partHeader.displayWindow.min.y ||
            partHeader.dataWindow.max.x != partHeader.displayWindow.max.x ||
            partHeader.dataWindow.max.y != partHeader.displayWindow.max.y )
        {
            logError("Engine doesn't supports clipping data window to display window!\n");
            logError("Can't load part %u from file.\n", part);
            continue;
        }

        // Determine texture parameters
        partHeader.storedTextureState.width  = partHeader.dataWindowWidth();
        partHeader.storedTextureState.height = partHeader.dataWindowHeight();

        // Set texture type
        if (partHeader.type == ScanLineImage || 
            partHeader.type == TiledImage )
        {
            partHeader.storedTextureState.type = gpu::TextureType::Texture2D;
        }
        if (partHeader.type == DeepScanLine || 
            partHeader.type == DeepTile )
        {
            partHeader.storedTextureState.type   = gpu::TextureType::Texture3D;
            partHeader.storedTextureState.layers = partHeader.maxSamplesPerPixel;
        }

        bool channelsWithDifferentFormats = false;
        if (!partHeader.determineFormat(partHeader.storedTextureState.format, channelsWithDifferentFormats))
        {
            if (channelsWithDifferentFormats)
            {
                logError("Engine doesn't supports EXR images with channels having different format!\n");
                logError("Can't load part %u from file.\n", part);
                continue;
            }
            else
            {
                logError("EXR image has unrecognized format or more than 4 channels!\n");
                logError("Can't load part %u from file.\n", part);
                continue;
            }
        }

        // Checks that data is contiguous in memory.
        bool correct = true;
        for(uint32 i=0; i<partHeader.channelsCount; ++i)
        {
            // Channel needs to have full sampling rate (matching 1:1 image resolution)
            if (partHeader.channels[i].xSampling != 1 ||
                partHeader.channels[i].ySampling != 1 )
            {
                correct = false;
                break;
            }
        }

        // Unsupported data layout
        if (!correct)
        {
            logError("Engine supports only contiguous data layout for channes!\n");
            logError("Can't load part %u from file.\n", part);
            continue;
        }

        // This part is valid and can be loaded to memory
        partHeader.supported = true;
    }

    // TODO: Add support for multi-part files
    storedTextureState = headers[0].storedTextureState;

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
    uint64 offsetToData = 0;
    std::vector<PartHeader> partHeaders;
    ParsingResult result = parseMetadata(buffer, static_cast<uint32>(readSize), partHeaders, storedTextureState, offsetToData);

    // EXR always stored data using linear transfer function.
    storedColorSpace = ColorSpace::Linear;

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    // TODO: Combine information from part headers into single storedTextureState for multi-part EXR files (to add support for cube-maps, mip-mapped textures, 3D, etc.)
    // TODO: Re-try with more data loaded if result == ParsingData::InsufficientData;
    return (result == ParsingResult::Success);
}

struct DecompressState
{
    uint8* src;                // Pointer to source data (loaded or memory mapped file contents)
    uint8* dst;                // Pointer to destination data (offset to region in given texture surface)
    uint32 srcSize       : 30; // Size of source data (may differ if compressed)
    uint32 channelsCount : 2;  // Count of channels per texel
    uint32 dstSize       : 30; // Size of destination data (uncompressed)
    uint32 channelSize   : 2;  // Size of single channel data in bytes
    uint16 height;             // Surface height in lines 
    uint16 firstLine;          // First line in surface, where destination data is written (decompressed)
    uint16 linesCount;         // Count of lines writtten into surface
    uint16 lineWidth;          // Single line width in texels
};

static_assert(sizeof(DecompressState) == 32, "en::exr::DecompressState size mismatch.");

void taskDecompressZIP(void* taskData)
{
    DecompressState& state = *(DecompressState*)(taskData);

    // Verifies ZIP chunk header consistency with part header
    uint32 compressedSize = *reinterpret_cast<uint32*>(state.src);
    if (compressedSize != state.srcSize)
    {
        logError("Expected chunk (%u) size does not match one reported in chunk header (%u)!\n", state.srcSize, compressedSize);
        return;
    }

    // Pointer to compressed data
    uint8* input = state.src + sizeof(uint32);

    // Uncompress data to temporary buffer
    // (allocates 2 times the maxium required block size to give room for INFLATE)
    uint32 stagingSize = state.dstSize * 2;
    uint8* staging = new uint8[stagingSize];

    // Create zlib stream structure
    z_stream stream;
    stream.next_in   = input;         // Compressed data
    stream.avail_in  = state.srcSize; // Size of compressed data
    stream.next_out  = staging;       // Output destination
    stream.avail_out = stagingSize;   // Available space for uncompressed data
    stream.zalloc    = Z_NULL;
    stream.zfree     = Z_NULL;
    stream.opaque    = Z_NULL;

    if (CheckError(inflateInit(&stream)))
    {
        logError("Cannot initialize Zlib decompressor!\n");

        delete[] staging;
        delete (DecompressState*)(taskData);
        return;
    }

    // Decompress data to 'output' buffer
    sint32 result = inflate(&stream, Z_FINISH);
    if (result != Z_OK &&
        result != Z_STREAM_END)
    {
        CheckError(result);
        logError("Cannot decompress using ZLIB!\n");

        delete[] staging;
        delete (DecompressState*)(taskData);
        return;
    }
    inflateEnd(&stream);

    // Reorder uncompressed data to final buffer
    uint32 srcLineSize = state.lineWidth * (state.channelsCount * state.channelSize);
    for (uint32 y = 0; y < state.linesCount; ++y)
    {
        for (uint32 x = 0; x < state.lineWidth; ++x)
        {
            for (uint32 c = 0; c <state.channelsCount; ++c)
            {
                // EXR stores image lines (tile rows) data in file, in reverse order 
                // to how texture surface lines (tile rows) should be ordered in memory.
// TODO: Confirm it, describe data order for textures (on their upload/download to/from RAM to/from GPU VRAM / GPU private memory) in renderer API surface
                uint32 dstLine = state.height - (state.firstLine + y) - 1;

                // Data after decompression is ordered as: RRRRR...GGGGG...BBBBB... 
                // (first channel values of all texels, followed by second channel values of all texels, etc.)
                uint8* srcPtr = staging + (y * srcLineSize) + (c * state.lineWidth * state.channelSize) + (x * state.channelSize);

                // Data is then reordered row by row, texel by texel, channel by channel in destination buffer.
                uint8* dstPtr = state.dst + (dstLine * srcLineSize) + (x * state.channelsCount * state.channelSize) + (c * state.channelSize);
 
                // TODO: Optimize this part as its 2 or 4 bytes copy.
                memcpy(dstPtr, srcPtr, state.channelSize);
            }
        }
    }

    delete[] staging;
    delete (DecompressState*)(taskData);
}

void decompressSurfaceFromZIP(const uint32 partsCount, const uint32 partIndex, const PartHeader& partHeader, uint8* buffer, const uint64 bufferSize, uint8* destination)
{
    // All tasks will share this state, thus it can be used to check when all tasks are done
    TaskState sharedState;

    DecompressState* state = new DecompressState;

    // Uncompressed properties
    state->dstSize       = partHeader.dataBlockSize();
    state->height        = partHeader.dataWindowHeight();
    state->linesCount    = partHeader.dataBlockHeight();
    state->lineWidth     = partHeader.dataWindowWidth();
    state->channelsCount = partHeader.channelsCount;
    state->channelSize   = partHeader.channelSize();

    // Assembles surface data from individial data blocks (chunks)
    bool lastChunk = false;
    for (sint32 i = 0; i < partHeader.chunksCount; ++i)
    {
        uint64 offset = partHeader.chunksOffsets[i];

        if (i + 1 == partHeader.chunksCount)
        {
            lastChunk = true;
        }

        // Reads chunk header
        if (partsCount > 1)
        {
            sint32 partNumber = *reinterpret_cast<sint32*>(buffer + offset);
            offset += 4;
            assert(partNumber == partIndex);
        }

        // Destination line or tile block row
        state->firstLine = *reinterpret_cast<uint32*>(buffer + offset);
        offset += 4;

        // Last block has less lines
        if (lastChunk)
        {
            state->linesCount = partHeader.dataWindowHeight() - ((partHeader.chunksCount - 1) * partHeader.dataBlockHeight());
        }

        // Data block source
        state->src = buffer + offset;
        if (lastChunk)
        {
            state->srcSize = bufferSize - offset;
        }
        else
        {
            state->srcSize = partHeader.chunksOffsets[i + 1] - offset;
        }

        // Data block destination in surface (not texture).
        // (chunks may be provided out of order thus using chunk header line index).
// TODO: Fix this for tiled!
        uint32 previousBlocksCount = state->firstLine / partHeader.dataBlockHeight();
        state->dst = destination + (previousBlocksCount * partHeader.dataBlockSize());

        // Create state copy for next task
        DecompressState* temp = nullptr;
        if (!lastChunk)
        {
            temp = new DecompressState;
            memcpy(temp, state, sizeof(DecompressState));
        }

        // Spawn task to decompress part of image
        // TODO: Direct tasks to different workers in the future to directly distribute 
        //       the work, starting from last core and ending on this one.
        Scheduler->run(taskDecompressZIP, (void*)state, &sharedState);
        if (!lastChunk)
        {
            state = temp;
        }
    }

    // Wait until all decoding tasks are done
    // TODO: In future consider passing this task state as output to allow multiple surface async decompressions
    Scheduler->wait(&sharedState);
}

bool load(const std::string& filename,
    uint8* const destination,
    const gpu::TextureState expectedState,
    const gpu::ImageMemoryAlignment alignment,
    const uint16 selectedMipmap,
    const uint16 selectedLayer)
{
    using namespace en::storage;

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
    uint64 offsetToData = 0;
    gpu::TextureState storedTextureState;
    std::vector<PartHeader> partHeaders;
    ParsingResult result = parseMetadata(buffer, static_cast<uint32>(readSize), partHeaders, storedTextureState, offsetToData);

    // EXR always stored data using linear transfer function.
    gpu::ColorSpace storedColorSpace = gpu::ColorSpace::Linear;

    // Free temporary 4KB memory page
    deallocate<uint8>(buffer);

    // TODO: Combine information from part headers into single storedTextureState for multi-part EXR files (to add support for cube-maps, mip-mapped textures, 3D, etc.)

    // TODO: Re-try with more data
    if (result == ParsingResult::IncompleteData)
    {
        logError("Not enough data was loaded to parse file metadata (read %u bytes)!\nFile: %s\n", readSize, fullPath.c_str());
        delete file;
        return false;
    }

    // Verifies metadata consistency
    if (result == ParsingResult::InvalidFormat)
    {
        logError("File metadata is corrupted!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }

    // Verifies that texture stored in EXR file matches destination one
    if (storedTextureState.type    != expectedState.type    ||
        storedTextureState.format  != expectedState.format  ||
        storedTextureState.width   != expectedState.width   ||
        storedTextureState.height  != expectedState.height  ||
        storedTextureState.layers  != expectedState.layers  ||
        storedTextureState.mipmaps != expectedState.mipmaps ||
        storedTextureState.samples != expectedState.samples)
    {
        logError("Stored texture state is not matching expected one!\nFile: %s\n", fullPath.c_str());
        delete file;
        return false;
    }


    // ### Read file to memory


    // TODO: Code below assumes loading all surfaces stored in this file.
    //       Thats why it loads whole file content to memory. At the same
    //       time it is missing GPU streamer guidance on destination 
    //       offsets of each decoded surface (based on their mipmap and 
    //       layer index). Thats why this method is expected to load only
    //       one surface at a time. To do that in optimal way, vector of
    //       part headers should be returned after parsing file metadata,
    //       so that the resource manager could call this method with
    //       specific part offset in file (and let it load only that part
    //       contents from disk instead of reading whole file each time). 
    //       Alternatively whole file could be read from disk by resource
    //       manager and this function would only decode data from given
    //       offset! (this seems like much better idea).

    // Read whole file to memory
    readSize = fileSize;
    buffer = allocate<uint8>(static_cast<uint32>(readSize), PageSize);
    if (!file->read(0, readSize, buffer, &readSize))
    {
        logError("Couldn't read file to memory!\nFile: %s\n", fullPath.c_str());
        deallocate<uint8>(buffer);
        delete file;
        return false;
    }

    if (readSize != fileSize)
    {
        logError("File read failed! Read %u instead of %u bytes.\nFile: %s\n", readSize, fileSize, fullPath.c_str());
        deallocate<uint8>(buffer);
        delete file;
        return false;
    }

    // TODO: It is not known, what the layers and mip-maps
    //       ordering convention is used inside of EXR files.
    //       This prevents us currently from selectively 
    //       loading only given layer mip.

    // Decompresses texture, surface after surface (parts)
    uint64 offset = offsetToData;
    for(uint32 part=0; part<partHeaders.size(); ++part)
    {
        const PartHeader& partHeader = partHeaders[part];

        if (partHeader.compression == None)
        {
            assert(0);
            // Finish!
        }
        else
        if (partHeader.compression == ZIP)
        {
            decompressSurfaceFromZIP(static_cast<uint32>(partHeaders.size()), part, partHeader, buffer, readSize, destination);
        }
        else
        {
            assert(0);
            // Unsupported compression type
        }

        // TODO: Add proper support for multi-part (multi-surface) EXR files.
        //       (currently always loads only the first surface in file)
        break;
    }

    deallocate<uint8>(buffer);
    delete file;

    return true;
}

} // en::exr 
} // en
