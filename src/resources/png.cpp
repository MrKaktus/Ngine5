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
using namespace en::gpu;           // For RAM -> VRAM transfer

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#include "zlib.h"
#endif

#include "platform/system.h"     // For CPU cores count
using namespace en::system;
#include "parallel/scheduler.h"  // For multithreaded decode
using namespace en::parallel;

#ifdef EN_PROFILE
#include "utilities/timer.h"
#endif

#include <string>

namespace en
{
namespace png
{

// Chunk signatures stored in little endiann
enum class Signature : uint32
{
    IHDR = 0x52444849,  // 'RDHI' -  73 72 68  82 - 0x52444849
    PLTE = 0x45544C50,  // 'PLTE' -  80 76 84  69 - 0x45544C50
    IDAT = 0x54414449,  // 'IDAT' -  73 68 65  84 - 0x54414449
    IEND = 0x444E4549,  // 'IEND' -  73 69 78  68 - 0x444E4549
    cHRM = 0x4D524863,  // 'cHRM' -  99 72 82  77 - 0x4D524863
    gAMA = 0x414D4167,  // 'gAMA' - 103 65 77  65 - 0x414D4167
    iCCP = 0x50434369,  // 'iCCP' - 105 67 67  80 - 0x50434369
    sBIT = 0x54494273,  // 'sBIT' - 115 66 73  84 - 0x54494273
    sRGB = 0x42475273,  // 'sRGB' - 115 82 71  66 - 0x42475273
    bKGD = 0x44474B62,  // 'bKGD' -  98 75 71  68 - 0x44474B62
    hIST = 0x54534968,  // 'hIST' - 104 73 83  84 - 0x54534968
    tRNS = 0x534E5274,  // 'tRNS' - 116 82 78  83 - 0x534E5274
    pHYs = 0x73594870,  // 'pHYs' - 112 72 89 115 - 0x73594870
    sPLT = 0x544C5073,  // 'sPLT' - 115 80 76  84 - 0x544C5073
    tIME = 0x454D4974,  // 'tIME' - 116 73 77  69 - 0x454D4974
    iTXt = 0x74585469,  // 'iTXt' - 105 84 88 116 - 0x74585469
    tEXt = 0x74584574,  // 'tEXt' - 116 69 88 116 - 0x74584574
    zTXt = 0x7458547A,  // 'zTXt' - 122 84 88 116 - 0x7458547A
};

// Signature
alignTo(1) 
struct Header
{
    uint32   signature;   // PNG file signature '.PNG' -> 474E5089 
    uint32   eof;         // Win/Unix EOL and EOF      -> 0A1A0A0D
};

// First chunk
struct IHDR
{
    uint32   length;      // Chunk data length
    uint32   signature;   // Chunk signature
    uint32   width;       // Image width
    uint32   height;      // Image height
    uint8    bps;         // Bits per sample: 1,2,4,8,16
    uint8    type;        // Image type: 0,2,3,4,6
    uint8    compression; // Compression type
    uint8    filter;      // Filter
    uint8    interlace;   // Interlace (not supported)
    uint32   crc;         // CRC32
}; 

struct cHRM
{
    uint32   signature;   // Chunk signature
    uint32v2 whitePoint;  // Floating point values multiplied by 100.000, thus value of 31270 represents 0.3127
    uint32v2 red;
    uint32v2 green;
    uint32v2 blue;
};

struct gAMA
{
    uint32   signature;   // Chunk signature
    uint32   gamma;       // Floating point values multiplied by 100.000, thus value of 45455 represents gamma of 1/2.2
};

struct iCCP
{
    uint32   signature;   // Chunk signature
                          // Variable length chunk
};

struct sRGB
{
    uint32   signature;   // Chunk signature
    uint8    intent;      // Rendering intent, values 0-3
};
alignToDefault

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
        enLog << "Error: Zlib cannot read chunk!\n";
    }
    else
    if (code == Z_STREAM_ERROR)
    {
        enLog << "Error: Zlib invalid compression level!\n";
    }
    else
    if (code == Z_DATA_ERROR)
    {
        enLog << "Error: Zlib invalid or incomplete chunk!\n";
    }
    else
    if (code == Z_MEM_ERROR)
    {
        enLog << "Error: Zlib reports out of memory!\n";
    }
    else
    if (code == Z_BUF_ERROR)
    {
        enLog << "Error: Zlib output buffer is to small!\n";
    }
    else
    if (code == Z_VERSION_ERROR)
    {
        enLog << "Error: Zlib version mismatch!\n";
    }
    else
    {
        enLog << "Error: Zlib unknown error code " << code << "!\n";
    }

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
    {
        return static_cast<uint8>(a);
    }
    if (pb <= pc) 
    {
        return static_cast<uint8>(b);
    }

    return static_cast<uint8>(c);
}

struct DecodeState
{
    uint32 startLine;      // Cannot be decoded with Paeth !
    uint32 lines;          // Lines to decode
    uint32 texelSize;      //
    uint32 width;          //
    uint32 height;         // Image height
    uint32 linePadding;    // Padding after each line in output buffer
    uint8* input;          // Input buffer
    uint8* output;         // Output buffer
    bool invertHorizontal; // Should invert the image ?
};

/* Deprecated in favor of new Fiber based scheduler. Remove once implementation is verified.

class Worker : public Task
{ 
    private:
    uint32 startLine;      // Cannot be decoded with Paeth !
    uint32 lines;          // Lines to decode
    uint32 texelSize;      //
    uint32 width;          //
    uint32 height;         // Image height
    uint32 linePadding;    // Padding after each line in output buffer
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
    linePadding(state.linePadding),
    input(state.input),
    output(state.output),
    invertHorizontal(state.invertHorizontal),
    Task(new TaskState())
{
}
*/

// TODO: Optimize using memcpy's and SIMD's
/*
void Worker::work(void)
{
*/
void taskDecodePNG(void* taskData)
{
    DecodeState& state = *(DecodeState*)(taskData);

    // Revert filters line by line
    uint32 lineSize   = state.width * state.texelSize;
    uint32 inOffset   = state.startLine * (lineSize + 1);
    uint32 outOffset  = state.startLine * (lineSize + state.linePadding);
    uint32 prevOffset = 0;
    for(uint32 i=state.startLine; i<(state.startLine + state.lines); ++i)
    {
        // Offset to previously decoded line
        if (i > 0)
        {
            prevOffset = (i - 1) * lineSize;
            if (state.invertHorizontal)
            {
                prevOffset = (state.height - i) * lineSize;
            }
        }
        if (state.invertHorizontal)
        {
            outOffset = (state.height - i - 1) * lineSize;
        }

        // Detect which type of filter was used for this line
        uint8 type = state.input[inOffset];
        inOffset++;

        // Filter type 0 - None
        if (type == 0)
        {
            for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
            {
                state.output[outOffset] = state.input[inOffset]; 
            }
        }
        // Filter type 1 - Sub
        if (type == 1)
        {
            // First pixel adds bytes outside scanline (equal 0) so just copy it
            for(uint8 j=0; j<state.texelSize; ++j, ++inOffset, ++outOffset) 
            {
                state.output[outOffset] = state.input[inOffset];
            }

            // Add bytes of previous pixel to reconstruct current one
            for(uint32 j=state.texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
            {
                state.output[outOffset] = state.input[inOffset] + state.output[outOffset - state.texelSize];
            }
        }
        // Filter type 2 - Up
        if (type == 2)
        {
            // If this is first scanline, above are only zeroes, so just copy it
            if (i == 0) 
            {
                for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset) 
                {
                    state.output[outOffset] = state.input[inOffset]; 
                }
            }
            else 
            {
                // Add bytes of above pixel to reconstruct current one
                for(uint32 j=0; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
                {
                    state.output[outOffset] = state.input[inOffset] + state.output[prevOffset]; 
                }
            }
        }
        // Filter type 3 - Average
        if (type == 3)
        {
            // If this is first scanline, above are only zeroes
            if (i == 0) 
            {
                // If this is first pixel, average will be zero, so just copy it
                for(uint8 j=0; j<state.texelSize; ++j, ++inOffset, ++outOffset) 
                {
                    state.output[outOffset] = state.input[inOffset];
                }

                // Average from A and zero is A divided by two
                for(uint32 j=state.texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
                {
                    state.output[outOffset] = state.input[inOffset] + state.output[outOffset - state.texelSize] / 2;
                }
            }
            else
            {
                // Average from B and zero is B divided by two
                for(uint8 j=0; j<state.texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
                {
                    state.output[outOffset] = state.input[inOffset] + state.output[prevOffset] / 2;
                }

                for(uint32 j=state.texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
                {
                    state.output[outOffset] = state.input[inOffset] + ((state.output[outOffset - state.texelSize] + state.output[prevOffset]) / 2);
                }
            }
        }
        // Filter type 4 - Paeth
        if (type == 4)
        {
            // If this is first scanline, above are only zeroes
            if (i == 0) 
            {
                // If this is first pixel, previous is zero, so just copy it
                for(uint8 j=0; j<state.texelSize; ++j, ++inOffset, ++outOffset) 
                {
                    state.output[outOffset] = state.input[inOffset];
                }

                // Do partial Paeth with zeroes
                for(uint32 j=state.texelSize; j<lineSize; ++j, ++inOffset, ++outOffset)
                {
                    state.output[outOffset] = state.input[inOffset] + Paeth(state.output[outOffset - state.texelSize], 0, 0);
                }
            }
            else
            {
                for(uint8 j=0; j<state.texelSize; ++j, ++inOffset, ++outOffset, ++prevOffset) 
                {
                    state.output[outOffset] = state.input[inOffset] + Paeth(0, state.output[prevOffset], 0);
                }

                for(uint32 j=state.texelSize; j<lineSize; ++j, ++inOffset, ++outOffset, ++prevOffset)
                {
                    state.output[outOffset] = state.input[inOffset] + Paeth(state.output[outOffset - state.texelSize], state.output[prevOffset], state.output[prevOffset - state.texelSize]);
                }
            }
        }
    }

    // Release local state
    delete (DecodeState*)(taskData);
}

#define PageSize 4096

struct ColorSpacePrimaries
{
    float2 red;
    float2 green;
    float2 blue;
    float2 whitePoint;
};

struct ColorSpaceInfo
{
    ColorSpacePrimaries primaries;
    float gamma;

    ColorSpaceInfo();
};

ColorSpaceInfo::ColorSpaceInfo() :
    gamma(1.0f)
{
}



// Designated initializers are not working in Visual Studio
//
//ColorSpacePrimaries sRGB_primaries = { .red = float2(0.64f, 0.33f), 
//                                             .green(0.30f, 0.60f), 
//                                             .blue(0.15f, 0.06f), 
//                                             .whitePoint(0.3127f, 0.3290f) };



//struct TextureMetadata
//   {
//   TextureState state;
//   ColorSpace   colorSpace;
//   uint64       dataSize;
//   uint64       
//   };


// Open file
// read header pages
// decode metadata
// read rest of file
// uncompress
// close file
// alloc memory on a heap
// unpack to heap



// Reads first 4KB page of PNG file, and decodes it's header to TextureState and ColorSpace.
// If operation is successfull, returns handle to that file, so that engine can continue with
// it's decompression.
bool readMetadata(uint8* buffer, const uint32 readSize, gpu::TextureState& settings, gpu::ColorSpace& colorSpace)
{
    // Check if file has minimum required size
    uint32 minimumFileSize = sizeof(Header) + sizeof(IHDR);
    if (readSize < minimumFileSize)
    {
        enLog << "ERROR: PNG file size too small!\n";
        return false;
    }

    // Read file signature
    Header& signature = *reinterpret_cast<Header*>(buffer);
    if ( signature.signature != 0x474E5089 ||
         signature.eof       != 0x0A1A0A0D )
    {
        enLog << "ERROR: PNG file header signature is incorrect!\n";
        return false;
    }

    // Read file header
    IHDR& header = *reinterpret_cast<IHDR*>(buffer + 8);
    if ( header.length != endiannes(uint32(13)) ||
         header.signature != 0x52444849 )
    {
        enLog << "ERROR: PNG file IHDR signature is incorrect!\n";
        return false;
    }

    // Check compression
    if (header.compression != 0)
    {
        enLog << "ERROR: This PNG compression method is not supported!\n";
        return false;
    }

    // Check filtering before compression
    if (header.filter != 0)
    {
        enLog << "ERROR: This PNG filtering method is not supported!\n";
        return false;
    }

    // Check if image is not interlaced
    if (header.interlace != 0)
    {
        enLog << "ERROR: Interlaced PNG files are not supported!\n";
        return false;
    }

    // Determine texture parameters
    settings.width  = endiannes(header.width);
    settings.height = endiannes(header.height);
    if (header.type == 0 && header.bps == 8)
    {
        settings.format = Format::R_8;
    }
    else
    if (header.type == 0 && header.bps == 16)
    {
        settings.format = Format::R_16;
    }
    else
    if (header.type == 2 && header.bps == 8)
    {
        // Should be RGB_8/RGB_8_sRGB but Windows software saves PNG's in BGR_8 (LO->HI)
        settings.format = Format::BGR_8;         
        if (colorSpace == ColorSpaceSRGB)
        {
            settings.format = Format::BGR_8_sRGB;
        }
    }
    else
    if (header.type == 2 && header.bps == 16)
    {
        settings.format = Format::RGB_16;
    }
    else
    if (header.type == 4 && header.bps == 8)
    {
        settings.format = Format::RG_8;   
    }   
    else
    if (header.type == 4 && header.bps == 16)
    {
        settings.format = Format::RG_16;
    }
    else
    if (header.type == 6 && header.bps == 8)
    {
        settings.format = Format::RGBA_8;    // RGB / RGBA according to http://www.w3.org/TR/PNG/#4Concepts.PNGImage
    }
  //else
  //if (header.type == 6 && header.bps == 16)
  //{
  //    settings.format = Format::RGBA_16;
  //}
    else
    {
        enLog << "ERROR: Unsupported texture format!\n";
        return false;
    }

    // Determine texture type
    settings.type    = TextureType::Texture2D;
    if (settings.height == 1)
    {
        settings.type = TextureType::Texture1D;
    }
  //if (!powerOfTwo(settings.width) ||
  //    !powerOfTwo(settings.height) )
  //{
  //    settings.type = Texture2DRectangle;
  //}

    return true;
}

/*
void decode(DecodeState& decoder)
{
    // If possible revert filters in paraller using several worker threads
    uint32 cores = Scheduler->workers();
    if (cores == 1) // TODO: In future add minimum amount of lines, below which we don't use task pool
    {
        TaskState* state = Scheduler->run(new Worker(decoder), true); // Execute decoding on current thread
    }
    else
    {
        // Try to disect image lines between the cores
        TaskState* state[32]; // Handles of up to 32 simultaneous decoding threads

        decoder.startLine = 0;
        decoder.lines     = 0;

        uint32 task=0;
        uint32 linesPerCore = decoder.height / cores;
        for(; task<cores; ++task)
        {
            // Calculate lines range for current core
            decoder.startLine += decoder.lines;
            decoder.lines      = linesPerCore;

            // Check if first line of next section is not compressed with pattern
            // 2,3 or 4. In such situation increase current lines range to prevent
            // data relation between the thread blocks (these patterns use previous line). 
            uint8 type = 4;
            bool lastTask = false;
            while(type > 1)
            {
                // Check if current block is not already reaching end of image
                uint32 nextLine = decoder.startLine + decoder.lines;
                if (nextLine >= decoder.height)
                {
                    decoder.lines = decoder.height - decoder.startLine;
                    lastTask = true;
                    break;
                }

                // Check if next block will use "Paeth"
                uint32 inOffset = nextLine * (decoder.width * decoder.texelSize + 1);
                uint8 type = decoder.input[inOffset];
                if (type < 2)
                {
                    break;
                }

                decoder.lines++;
            }
         
            // Spawn task to decompress image parts,
            // starting from last core and ending on this one.
            state[task] = Scheduler->run(new Worker(decoder), (cores - task - 1), false);
            if (lastTask) 
            {
                break;
            }
        }

        // TODO: Fix proper handling of tasks and their state pointers
        //       If task is deleted by scheduler after it is finished, how to verify it's completion? Who should delete TaskState ? 
        // Barrier - wait until all threads will finish execution
        //for(uint8 i=0; i<cores; ++i)
        //{
        //    Scheduler.wait(state[cores - i - 1]);
        //}
    }
}
*/

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
    File* file = Storage->open(filename);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.textures + filename);
        if (!file)
        {
            enLog << en::ResourcesContext.path.textures + filename << std::endl;
            enLog << "ERROR: There is no such file!\n";
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
        delete file;
        return false;
    }

    // Verify that file matches expected properties
    if ( (settings.width != width) ||
         (settings.height != height) ||
         (settings.format != format) )
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
        delete file;
        return false;
    }

    // Release file handle and work on copy in memory
    delete file;


    // ### Parse and decompress file 


    // Allocate buffer for content of decompressed IDAT chunks
    // Size of decompressed chunks may equal in worst case scenario to (Width x Height x BPP) + extra Height bytes.
    // Extra Height bytes comes from the fact that each line starts with extra Byte specifying filter type applied for that line.
    uint64 inflateBufferSize = roundUp(alignment.surfaceSize(width, height) + settings.height, PageSize);
    uint8* inflated = allocate<uint8>(inflateBufferSize, PageSize);

    ColorSpaceInfo colorSpaceInfo;

    bool firstDataChunk = true;
    z_stream stream;

    // Read chunks in loop until all are processed
    uint64 offset = 33;
    uint32 chunkLength = 0;
    Signature signature = static_cast<Signature>(0);
    while(signature != Signature::IEND)
    {
        // Chunk length
        chunkLength = endiannes(*reinterpret_cast<uint32*>(content + offset));
        assert( offset + chunkLength <= fileSize );
        offset += 4;
      
        // Chunk signature
        signature = *reinterpret_cast<Signature*>(content + offset);
        offset += 4;
      
        // If ending chunk break
        if (signature == Signature::IEND)
        {
            break;
        }
      
        switch(signature)
        {
            // Decompress chunk for processing
            case Signature::IDAT:
            {
                // All IDAT chunks in the file need to appear one after another,
                // and they cannot be separated by any other type of chunk. Thus
                // their content could be decompressed at once, if it would be 
                // copied from inside of the chunks, into continuous memory location.
                // As file content is already read (or memory-mapped) to memory,
                // it is better to just decompress each chunk separately on the
                // fly.

                // Init zlib stream structure
                stream.next_in   = reinterpret_cast<uint8*>(content + offset);       // Compressed data
                stream.avail_in  = chunkLength;                                      // Size of compressed data

                // Init decompressor when first data chunk is found
                if (firstDataChunk)
                {
                    stream.zalloc    = Z_NULL;
                    stream.zfree     = Z_NULL;
                    stream.opaque    = Z_NULL;
                    stream.avail_out = inflateBufferSize;             // Available space for uncompressed data
                    stream.next_out  = static_cast<uint8*>(inflated); // Store uncompressed data together with previous IDAT chunks
            
                    firstDataChunk = false;
                    if (CheckError(inflateInit(&stream)))
                    {
                        enLog << "Error: Cannot initialize Zlib decompressor!\n";
                        deallocate<uint8>(content);
                        deallocate<uint8>(inflated);
                        return false;
                    }
                }

                // Decompress data from IDAT chunk to 'inflated' buffer
                sint32 ret = 0;
                ret = inflate(&stream, Z_NO_FLUSH);
                if (ret != Z_OK &&
                    ret != Z_STREAM_END)
                {
                    CheckError(ret);
                    enLog << "Error: Cannot decompress using ZLIB!\n";
                    deallocate<uint8>(content);
                    deallocate<uint8>(inflated);
                    return false;
                }

                // Move offset to the end of the chunk
                offset += chunkLength;

                break;
            }

            // Read advanced compression method chunk
            case Signature::iCCP:
            {
                char profile[80];
                for(uint32 i=0; i<80; ++i)
                {
                    profile[i] = *reinterpret_cast<char*>(content + offset);
                    offset++;
                    if (profile[i] == 0)
                    {
                        break;
                    }
                }

                enLog << "iCCP chunk info:\n";
                enLog << "Profile name: " << profile << std::endl;

                break;
            }

            // Read chrominance chunk
            case Signature::cHRM:
            {
                // Read white base of chrominance 
                colorSpaceInfo.primaries.whitePoint.x = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
                colorSpaceInfo.primaries.whitePoint.y = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
            
                // Read red chrominance
                colorSpaceInfo.primaries.red.x = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
                colorSpaceInfo.primaries.red.y = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
            
                // Read green chrominance
                colorSpaceInfo.primaries.green.x = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
                colorSpaceInfo.primaries.green.y = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
            
                // Read blue chrominance
                colorSpaceInfo.primaries.blue.x = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
                colorSpaceInfo.primaries.blue.y = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0f;
                offset += 4;
            
                enLog << "cHRM chunk info:\n";
                enLog << "Chrominance white x=" << colorSpaceInfo.primaries.whitePoint.x << " y=" << colorSpaceInfo.primaries.whitePoint.y << std::endl;
                enLog << "Chrominance red   x=" << colorSpaceInfo.primaries.red.x        << " y=" << colorSpaceInfo.primaries.red.y        << std::endl;
                enLog << "Chrominance green x=" << colorSpaceInfo.primaries.green.x      << " y=" << colorSpaceInfo.primaries.green.y      << std::endl;
                enLog << "Chrominance blue  x=" << colorSpaceInfo.primaries.blue.x       << " y=" << colorSpaceInfo.primaries.blue.y       << std::endl;

                break;
            }
            
            // Read gamma chunk
            case Signature::gAMA:
            {
                colorSpaceInfo.gamma = (float)*reinterpret_cast<uint32*>(content + offset) / 100000.0;
                offset += 4;

                enLog << "gAMA chunk info:\n";
                enLog << "Gamma =" << colorSpaceInfo.gamma << std::endl;

                break;
            }

            // Unsupported chunk
            default:
            {
                offset += chunkLength;

                break;
            }
        };

        // Skip CRC of current chunk, moving offset to the next one
        offset += 4;
    };

    // Close decompressor
    inflateEnd(&stream);

    // Free file content
    deallocate<uint8>(content);


    // ### Decode applied filters


    assert( reinterpret_cast<uint64>(destination) % alignment.surfaceAlignment() == 0 );

/*
    DecodeState decoder;
    decoder.startLine        = 0;
    decoder.lines            = settings.height;
    decoder.texelSize        = gpu::texelSize(settings.format);
    decoder.width            = settings.width;
    decoder.height           = settings.height;
    decoder.linePadding      = alignment.rowPitch(width) - alignment.rowSize(width);
    decoder.input            = inflated;
    decoder.output           = destination;
    decoder.invertHorizontal = invertHorizontal;

    // Decode decompressed content of PNG file 
    decode(decoder);
 */




    // All tasks will share this state, thus it can be used to check when all tasks are done
    TaskState sharedState;

    DecodeState* state = new DecodeState;
    state->startLine        = 0;
    state->lines            = 0;  // Will be populated in loop below
    state->texelSize        = gpu::texelSize(settings.format);
    state->width            = settings.width;
    state->height           = settings.height;
    state->linePadding      = alignment.rowPitch(width) - alignment.rowSize(width);
    state->input            = inflated;
    state->output           = destination;
    state->invertHorizontal = invertHorizontal;

    // If possible, will revert filters in paraller, using several worker threads. 
    // It will be done by trying to distribute work as equally as possible across
    // worker threads. Image will be divided into sections having as close amount 
    // of lines to process as possible. Sections count will be equal to worker
    // threads to match available CPU cores.
    uint32 workers = Scheduler->workers();
    uint32 linesPerCore = state->height / workers;
    for(uint32 task=0; task<workers; ++task)
    {
        // Calculate lines range for current task
        state->startLine += state->lines;
        state->lines      = linesPerCore;
   
        // Check if first line of next section is not compressed with pattern
        // 2,3 or 4. In such situation increase current lines range to prevent
        // data dependency between task blocks (these patterns use previous line). 
        uint8 type = 4;
        bool lastTask = false;
        while(type > 1)
        {
            // Check if current block is not already reaching end of image
            uint32 nextLine = state->startLine + state->lines;
            if (nextLine >= state->height)
            {
                state->lines = state->height - state->startLine;
                lastTask = true;
                break;
            }
   
            // Check if next block will use "Paeth"
            uint32 inOffset = nextLine * (state->width * state->texelSize + 1);
            uint8 type = state->input[inOffset];
            if (type < 2)
            {
                break;
            }
   
            state->lines++;
        }
      
        // Create state copy for next task
        DecodeState* temp = nullptr;
        if (!lastTask)
        {
            temp = new DecodeState;
            memcpy(temp, state, sizeof(DecodeState));
        }

        // Spawn task to decompress part of image
        // TODO: Direct tasks to different workers in the future to directly distribute 
        //       the work, starting from last core and ending on this one.
        en::Scheduler->run(taskDecodePNG, (void*)state, &sharedState); // (cores - worker - 1)
        if (lastTask) 
        {
            break;
        }

        state = temp;
    }

    // Wait until all decoding tasks are done
    // TODO: In future consider passing this task state as output to allow multiple async decompressions
    en::Scheduler->wait(&sharedState);

    // Free temporary 'infalte' buffer
    deallocate<uint8>(inflated);
    return true;
}

} // en::png
} // en
