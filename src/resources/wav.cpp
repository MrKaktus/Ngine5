/*

 Ngine v5.0
 
 Module      : WAV files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading sounds from *.wav files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "utilities/utilities.h"

#include "resources/context.h"
#include "resources/wav.h"    
#include "audio/audio.h"

namespace en
{
namespace wav
{
 
alignTo(1)
struct Header 
{
    uint32 id;              // Header ID 'RIFF' in Big Endian
    uint32 size;            // File size minus 8 bytes
    uint32 format;          // Format 'WAVE' in Big Endian
};

struct FMTchunk
{
    uint32 id;              // Chunk ID 'fmt ' in Big Endian
    uint32 size;            // This chunk size - 8 (16 for uncompressed PCM)
    uint16 format;          // Audio Format 1 for Linear Quantization (uncompressed)
    uint16 channels;        // Audio Channels 1-Mono, 2-Stereo, etc.
    uint32 sampleRate;      // Frequency in Hz
    uint32 byteRate;
    uint16 blockAlign;
    uint16 bps;             // Bits Per Sample
};

struct DATAchunk
{
    uint32 id;              // Chunk ID 'data' in Big Endian
    uint32 size;            // Size of raw data in chunk
};
alignToDefault
 
std::shared_ptr<audio::Sample> load(const std::string& filename)
{
    using namespace en::storage;

    // Try to reuse already loaded sound sample
    if (ResourcesContext.sounds.find(filename) != ResourcesContext.sounds.end())
    {
        return std::shared_ptr<audio::Sample>(ResourcesContext.sounds[filename]);
    }

    // Open audio file 
    File* file = Storage->open(filename);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.sounds + filename);
        if (!file)
        {
            enLog << en::ResourcesContext.path.sounds + filename << std::endl;
            enLog << "ERROR: There is no such file!\n";
            return std::shared_ptr<audio::Sample>(NULL);
        }
    }
   
    // Read and check file header
    Header header;
    file->read(0, 12, &header);
    if ( (header.id != 0x46464952) ||           // 'RIFF' in Big Endian
         (header.size != (file->size() - 8)) ||
         (header.format != 0x45564157) )        // 'WAVE' in Big Endian
    {
        enLog << "ERROR: Incorrect WAV file header!\n";
        delete file;
        return std::shared_ptr<audio::Sample>(NULL);
    }

    // Read and check FMT chunk
    FMTchunk fmt;
    file->read(12, 24, &fmt);
    if ( (fmt.id != 0x20746d66) ||              // 'fmt ' in Big Endian
         (fmt.byteRate != (fmt.sampleRate * fmt.channels * fmt.bps/8)) )
    {
        enLog << "ERROR: WAV file has corrupted FMT chunk!\n";
        delete file;
        return std::shared_ptr<audio::Sample>(NULL);
    }  
    if ( (fmt.size != 16) ||
         (fmt.format != 1) )
    {
        enLog << "ERROR: Not supported WAV compression format!\n";
        delete file;
        return std::shared_ptr<audio::Sample>(NULL);
    }

    // Audio Context specific functionality checks (made for speed-up)
    if ( (fmt.bps != 8) &&
         (fmt.bps != 16) )
    {
        enLog << "ERROR: Unsupported Bits Per Sample ratio!\n";
        delete file;
        return std::shared_ptr<audio::Sample>(NULL);
    }
    if (fmt.channels > 2)
    {
        enLog << "ERROR: Unsupported audio channells count!\n";
        delete file;
        return std::shared_ptr<audio::Sample>(NULL);
    }

    // Skip chunks until DATA chunk will be found
    uint32 offset = 36;
    DATAchunk data;
    file->read(offset, 8, &data);
    offset += 8;  
    while(data.id != 0x61746164)            // 0x514E554A - 'JUNQ' in Big Endina
    {                                       // 0x4B4E554A - 'JUNK' in Big Endina
        offset += data.size;
        file->read(offset, 8, &data);       
    }

    // Uncompress audio data from file
    uint8* buffer = new uint8[data.size];
    file->read(offset, data.size, buffer);

    // Create audio sample in device
    std::shared_ptr<audio::Sample> sample = Audio.sample.create(fmt.channels, fmt.sampleRate, fmt.bps, data.size, buffer);

    // Update list of loaded sound samples
    if (sample)
    {
        ResourcesContext.sounds.insert(std::pair<std::string, std::shared_ptr<audio::Sample> >(filename, sample));
    }
    else
    {
        enLog << "ERROR: Cannot create sample in SoundCard!\n";
    }

    delete [] buffer;
    delete file;
    return sample;
}

} // en:wav
} // en
