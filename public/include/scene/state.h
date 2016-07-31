/*

 Ngine v5.0
 
 Module      : State
 Requirements: none
 Description : Basic building block of game - game state.
               Manages current behavior of game, it's
               input support, logic and output. It can be
               switched to another state, or several of 
               them can exist at the same time.

*/

#ifndef ENG_STATE
#define ENG_STATE

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"
#include "utilities/timer.h"
#include "input/input.h"

namespace en
{
   namespace state
   {
   using namespace en::input;

   class State : public SafeObject<State>
         {
         public:
         virtual bool init(void) = 0;   
         virtual bool input(Event& event) = 0;
         virtual bool update(Time dt) = 0;  
         virtual bool draw(void) = 0;         
         virtual bool pause(void) = 0;        
         virtual bool resume(void) = 0;    
         virtual bool destroy(void) = 0;      
         };

   struct Interface
          {
          void change(State* state);  // Finishes current state and starts new one (at the beginning of next update call) 
          bool set(State* state);     // Pauses current state and switche's to pointed one (at the beginning of next update call) 
          void finish(void);          // Finishes current state and restores previous one (at the beginning of next update call) 

          bool update(Time dt);       // Updates all managed states (current and paused) 
          bool draw(void);            // Draws all managed states (current and paused)
          bool close();               // Finishes execution of all states at the next update 
          };
   }

extern state::Interface StateManager; // Default instance of State Manager interface
}

#endif
