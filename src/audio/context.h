/*

 Ngine v5.0
 
 Module      : Audio support.
 Requirements: OpenAL
 Description : Supports playback of short audio
               samples and longer music themes 
               in three dimensional space.

*/

#ifndef ENG_AUDIO_CONTEXT
#define ENG_AUDIO_CONTEXT

#if defined(EN_PLATFORM_ANDROID)
// OpenSL ES 1.0.1
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Platform.h> 
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#endif

#if defined(EN_PLATFORM_BLACKBERRY)
//#include <sys/asound.h>
//#include <sys/asoundlib.h>
#include <AL/al.h>
#include <AL/alc.h>
#endif

#if defined(EN_PLATFORM_OSX)
// OpenAL
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif

#if defined(EN_PLATFORM_WINDOWS)
// OpenAL
#include "C:/Program Files (x86)/OpenAL 1.1 SDK/include/al.h"
#include "C:/Program Files (x86)/OpenAL 1.1 SDK/include/alc.h"
#endif

#include "core/configuration.h"
#include "core/utilities/TarrayAdvanced.h"
#include "core/log/log.h"
#include "audio/audio.h"

namespace en
{
   namespace audio
   {
   class alSample : public Sample
         {
         public:                             // Engine internal
         uint32 id;                          // OpenAL sample Id

         alSample(const uint32 id);
         ~alSample();                        // Polymorphic deletes require a virtual base destructor
         };

   class alSource : public Source
         {
         public:
         bool play(void);                    // Start playing the sound
         void pause(void);                   // Pause playing the sound
         void stop(void);                    // Stop playing the sound
         void volume(const float volume);    // Sets the volume of the source
         void loop(const bool loop);         // Define if sound should be looped
         bool set(const shared_ptr<Sample> sample); // Attach sound sample to be played by source

         public:                             // Engine internal
         uint32   id;                        // OpenAL source Id
         shared_ptr<alSample> sample;               // OpenAL sample interface pointer 

         alSource();
         ~alSource();                        // Polymorphic deletes require a virtual base destructor
         };








   //class cachealign SampleDescriptor : public ProxyObject
   //       {
   //       public:
   //       uint32 id;            // OpenAL id

   //       SampleDescriptor();
   //      ~SampleDescriptor();
   //       };

   //class cachealign SourceDescriptor : public ProxyObject
   //       {
   //       public:
   //       uint32 id;            // OpenAL id
   //       bool   attached;      // Is audio sample attached

   //       SourceDescriptor();
   //      ~SourceDescriptor();
   //       };

   struct Context
          {
          #if defined(EN_PLATFORM_ANDROID)
          SLObjectItf engine;   // Audio engine

          #endif
          #if defined(EN_PLATFORM_BLACKBERRY)
          //snd_mixer_t* mixer;   // Analog audio mixer
          //snd_pcm_t*   pcm;     // 
          #endif
          #if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
          ALCdevice*  device;   // Audio device
          ALCcontext* context;  // Audio context on device
          #endif

          //TarrayAdvanced<SampleDescriptor, EN_MAX_ARRAY_SIZE> samples;

          Context();
         ~Context();

          bool create(void);
#ifdef EN_PLATFORM_BLACKBERRY
          //bool recreate(const char* device);   // Recreates audio resources after changing of audio device
#endif
          void destroy(void);

          //SampleDescriptor* create(const uint32 channels, // Audio channels
          //                         const uint32 freq,     // Frequency in Hz
          //                         const uint32 bps,      // Bits Per Sample
          //                         const uint32 size,     // Size
          //                         const void*  data);    // Data
          };
   }

extern audio::Context AudioContext;
}

#endif
