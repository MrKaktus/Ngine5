/*

 Ngine v5.0
 
 Module      : Audio support.
 Requirements: OpenAL
 Description : Supports playback of short audio
               samples and longer music themes 
               in three dimensional space.

*/

#ifndef ENG_AUDIO
#define ENG_AUDIO

#include "core/types.h"
//#include "core/utilities/Tproxy.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace audio
   {
   class Sample : public SafeObject<Sample>
         {
         public:
         virtual ~Sample();                              // Polymorphic deletes require a virtual base destructor
         };

   class Source : public SafeObject<Source>
         {
         public:
         virtual bool play(void) = 0;                    // Start playing the sound
         virtual void pause(void) = 0;                   // Pause playing the sound
         virtual void stop(void) = 0;                    // Stop playing the sound
         virtual void volume(const float volume) = 0;    // Sets the volume of the source
         virtual void loop(const bool loop) = 0;         // Define if sound should be looped
         virtual bool set(const Ptr<Sample> sample) = 0; // Attach sound sample to be played by source

         virtual ~Source();                              // Polymorphic deletes require a virtual base destructor
         };
         


   //// Sound sample
   //class Sample : public ProxyInterface<class SampleDescriptor>
   //      {
   //      public:

   //      Sample();
   //      Sample(class SampleDescriptor* src);
   //      };

   //// Sound source on the scene
   //class Source : public ProxyInterface<class SourceDescriptor>
   //      {
   //      public:
   //      bool play(void);                 // Start playing the sound
   //      void pause(void);                // Pause playing the sound
   //      void stop(void);                 // Stop playing the sound
   //      void volume(const float volume); // Sets the volume of the source
   //      void loop(const bool loop);      // Define if sound should be looped

   //      void set(Sample sample);         // Attach sound sample to be played by source

   ////      // TODO: Position, orientation, attach to object
   ////      // TODO: Attach sample
   //      };

   // Listener (player) on the scene
   class Listener
         {
         // TODO: Position, orientation, attach to object
         };
   
   struct Interface
          {
          struct Sample
                 {
                 Ptr<audio::Sample> create(const uint32 channels, // Audio channels
                                           const uint32 freq,     // Frequency in Hz
                                           const uint32 bps,      // Bits Per Sample
                                           const uint32 size,     // Size
                                           const void*  data);    // Data
                 } sample;

          // Audio "Source" and "Listener" objects need to be
          // located in coordinate system of some 3d space. 
          // Therefore they can be created inside "Scene" object.
          struct Source
                 {
                 Ptr<audio::Source> create();
                 } source;
          };
   }

extern audio::Interface Audio;
}

//extern template class ProxyInterface<en::audio::SampleDescriptor>;

#endif
