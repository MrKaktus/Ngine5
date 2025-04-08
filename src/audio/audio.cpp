/*

 Ngine v5.0
 
 Module      : Audio context.
 Requirements: OpenAL or OpenSL ES
 Description : Supports playback of short audio
               samples and longer music themes 
               in three dimensional space.

*/

#include "core/defines.h"
#include "core/types.h"
#include "core/log/log.h"

#include "audio/context.h"

#include "assert.h"

// OSX linking:
// gcc -framework OpenAL -o audio audio.cpp

namespace en
{
namespace audio
{

// Sample
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
alSample::alSample(const uint32 id) :
    id(id)
{
}

alSample::~alSample()
{
    alDeleteBuffers(1, &id);  
}
#endif

Sample::~Sample()
{
}

// Source

#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
bool alSource::play(void)
{
    if (!sample)
    {
        return false;
    }

    alSourcePlay(id);
    return true;
}

void alSource::pause(void)
{
    alSourcePause(id);
}

void alSource::stop(void)
{
    alSourceStop(id);
}

void alSource::volume(const float volume)
{
    alSourcef(id, AL_GAIN, volume);
}

void alSource::loop(const bool loop)
{
    alSourcei(id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

bool alSource::set(const std::shared_ptr<Sample> sample)
{
    if (!sample)
    {
        return false;
    }

    this->sample = std::dynamic_pointer_cast<alSample>(sample);
    alSourcei(id, AL_BUFFER, this->sample->id);
    return true;
}

alSource::alSource() :
    sample(nullptr)
{
    alGetError();
    alGenSources(1, &id);
    alSourcef(id, AL_PITCH, 1.0f);          
    alSourcef(id, AL_GAIN, 1.0f);
    alSourcei(id, AL_LOOPING, AL_FALSE);
}

alSource::~alSource()
{
    alGetError();
    alDeleteSources(1, &id);
}
#endif

Source::~Source()
{
}

#if defined(EN_PLATFORM_ANDROID) 
Context::Context()
{

}
#endif
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
Context::Context() :
    device(nullptr),
    context(nullptr)
{
}
#endif

Context::~Context()
{
}

bool Context::create(void)
{
    Log << "Starting module: Audio.\n";

#ifdef EN_PLATFORM_ANDROID
    SLresult result;

    // Create Audio engine
    result = slCreateEngine(&engine, 0, nullptr, 0, nullptr, nullptr);
    assert(result == SL_RESULT_SUCCESS);

    // Realize engine
    result = (*engine)->Realize(engine, SL_BOOLEAN_FALSE);
    assert(result == SL_RESULT_SUCCESS);

    // Acquire engine interface
    result = (*engine)->GetInterface(engine, SL_IID_ENGINE, &engineEngine);
    assert(result == SL_RESULT_SUCCESS);

    //// create output mix, with environmental reverb specified as a non-required interface
    //const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    //const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    //result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    //assert(SL_RESULT_SUCCESS == result);

    //// realize the output mix
    //result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    //assert(SL_RESULT_SUCCESS == result);

    //// get the environmental reverb interface
    //// this could fail if the environmental reverb effect is not available,
    //// either because the feature is not present, excessive CPU load, or
    //// the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    //result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
    //        &outputMixEnvironmentalReverb);
    //if (SL_RESULT_SUCCESS == result) 
    //   {
    //   result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
    //            outputMixEnvironmentalReverb, &reverbSettings);
    //   }
    //// ignore unsuccessful result codes for environmental reverb, as it is optional for this example
#endif
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
    // Find default device
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        return false;
    }

    // Create and activate default context on device
    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
#endif
    return true;
}

void Context::destroy(void)
{
    Log << "Closing module: Audio.\n";
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
    if (!context)
    {
        return;
    }

    alcDestroyContext(context);
    alcCloseDevice(device);

    context = nullptr;
    device  = nullptr;
#endif
}

//   SampleDescriptor* Context::create(const uint32 channels, // Audio channels
//                                     const uint32 freq,     // Frequency in Hz
//                                     const uint32 bps,      // Bits Per Sample
//                                     const uint32 size,     // Size
//                                     const void*  data)     // Data
//   {
//   assert(channels == 1 || channels == 2);
//   assert(bps == 8 || bps == 16);
// 
//   // Create audio sample in device
//   SampleDescriptor* sample = AudioContext.samples.allocate();
//   if (sample == NULL)
//      {
//      Log << "ERROR: Samples pool is full!\n";
//      return NULL;
//      } 
//
//#ifdef EN_PLATFORM_WINDOWS
//   // Generate buffer for data
//   uint32 error;
//   alGetError();
//   alGenBuffers(1, &sample->id);
//   if ((error = alGetError()) != AL_NO_ERROR)
//      {
//      Log << setprecision(2);
//      Log << "Error: Can't create audio sample: " << error << std::endl;
//      AudioContext.samples.free(sample);
//      return NULL;
//      } 
//      
//   // Determine OpenAL audio format
//   uint16 format = 0;
//   if ((channels == 1) && (bps == 8))
//      format = AL_FORMAT_MONO8;  
//   else
//   if ((channels == 1) && (bps == 16))
//      format = AL_FORMAT_MONO16; 
//   else
//   if ((channels == 2) && (bps == 8))
//      format = AL_FORMAT_STEREO8; 
//   else
//   if ((channels == 2) && (bps == 16))
//      format = AL_FORMAT_STEREO16;   
//
//   // Load data to device
//   alBufferData(sample->id, format, data, size, freq);     
//   if ((error = alGetError()) != AL_NO_ERROR)
//      {
//      Log << "ERROR: Cannot create sample in device!\n";
//      AudioContext.samples.free(sample);
//      return NULL;
//      }
//#endif
//
//   return sample;
//   }   

std::shared_ptr<audio::Sample> Interface::Sample::create(
    const uint32 channels, // Audio channels
    const uint32 freq,     // Frequency in Hz
    const uint32 bps,      // Bits Per Sample
    const uint32 size,     // Size
    const void*  data)     // Data    
{
    // Check input data
    if (channels != 1 &&
        channels != 2)
    {
        Log << "ERROR: Unsupported audio channells count!\n";
        return std::shared_ptr<audio::Sample>(nullptr);
    }
    if (bps != 8 &&
        bps != 16)
    {
        Log << "ERROR: Unsupported Bits Per Sample ratio!\n";
        return std::shared_ptr<audio::Sample>(nullptr);
    }

    // Generate buffer for data
    uint32 id = 0;
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
    uint32 error;
    alGetError();
    alGenBuffers(1, &id);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        Log << std::setprecision(2);
        Log << "Error: Can't create audio sample: " << error << std::endl;
        return std::shared_ptr<audio::Sample>(nullptr);
    } 
      
    // Determine OpenAL audio format
    uint16 format = 0;
    if ((channels == 1) && (bps == 8))
    {
        format = AL_FORMAT_MONO8;  
    }
    else
    if ((channels == 1) && (bps == 16))
    {
        format = AL_FORMAT_MONO16;
    }
    else
    if ((channels == 2) && (bps == 8))
    {
        format = AL_FORMAT_STEREO8; 
    }
    else
    if ((channels == 2) && (bps == 16))
    {
        format = AL_FORMAT_STEREO16;   
    }

    // Load data to device
    alBufferData(id, format, data, size, freq);     
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        Log << "ERROR: Cannot create sample in device!\n";
        return std::shared_ptr<audio::Sample>(nullptr);
    }

    return std::make_shared<alSample>(id);
#else
    return std::shared_ptr<audio::Sample>(nullptr);
#endif
}   

std::shared_ptr<audio::Source> Interface::Source::create(void)
{
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
    return std::make_shared<alSource>(); 
#else
    return std::shared_ptr<audio::Source>(nullptr);
#endif
}

//SourceDescriptor::SourceDescriptor() :
//   id(0),
//   attached(false)
//{
//}

//en::audio::Source Interface::Source::create(void)
//{
//uint32 id;

//// Generate source id
//alGetError();
//alGenSources(1, &id);
//if ((error = alGetError()) != AL_NO_ERROR)
//   {
//   Log << "Error: Can't create audio source." << error << std::endl;
//   return en::audio::Source(NULL);
//   }

//// Attach audio sample to source of the sound

//alSourcef(id, AL_PITCH, 1.0f);          // TODO: Check these defaults
//alSourcef(id, AL_GAIN , 1.0f);

//
//

//return id;
//}

//Source::set(Sample sample)
//{
//if (!pointer)
//   return;
////if (sample
//pointer->sample = sample;
//alSourcei(pointer->id, AL_BUFFER, sample.);
//}

//Source::loop(bool loop)
//{
//if (!pointer)
//   return;

//alSourcei(pointer->id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
//}

} // en::audio
   
audio::Context   AudioContext;
audio::Interface Audio; 

} // en                    

// When sample is not referenced anymore, free method of 
// custom allocator will call samples destructor which will 
// free it in audio device, and then will free memory it occupies.
//template<> bool (*ProxyInterface<en::audio::SampleDescriptor>::destroy)(en::audio::SampleDescriptor* const) = en::audio::DeleteSample;
//
//template<> bool (*ProxyInterface<en::audio::SourceDescriptor>::destroy)(en::audio::SourceDescriptor* const)   = en::audio::DeleteSource;
//




//uint32 NAudioContext::Source::create(uint32 sampleId, bool loop)
//{
// NLogContext& enLog = NLogContext::getInstance();
//
// uint32 id;
// uint32 error;
//
// // TODO: Check if sample id is correct
//
// // Generate source id
// alGetError();
// alGenSources(1, &id);
// if ((error = alGetError()) != AL_NO_ERROR)
//    {
//    enLog << "Error: Can't create audio source." << error << std::endl;
//    return 0;
//    }
//
// // Attach audio sample to source of the sound
// alSourcei(id, AL_BUFFER, sampleId);
// alSourcef(id, AL_PITCH, 1.0f);          // TODO: Check these defaults
// alSourcef(id, AL_GAIN , 1.0f);
// if (loop == true)
//    alSourcei(id, AL_LOOPING, AL_TRUE);
// else
//    alSourcei(id, AL_LOOPING, AL_FALSE);
//    
// return id;
//}
//
//void NAudioContext::Source::play(const uint32 id)
//{
// alSourcePlay(id);
//}
//
//void NAudioContext::Source::volume(const uint32 id, float volume)
//{
// alSourcef(id, AL_GAIN , volume);
//}
//
//void NAudioContext::Source::stop(const uint32 id)
//{
// alSourceStop(id);
//}
//
//void NAudioContext::Source::loop(const uint32 id, bool loop)
//{
// if (loop == true)
//    alSourcei(id, AL_LOOPING, AL_TRUE);
// else
//    alSourcei(id, AL_LOOPING, AL_FALSE);
//}
//
//bool NAudioContext::Source::destroy(const uint32 id)
//{
// alGetError();
// alDeleteSources(1,&id);
// if (alGetError() != AL_NO_ERROR)
//    return false; 
// return true;
//}
