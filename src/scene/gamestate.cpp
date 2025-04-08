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

#include "core/defines.h"
#include "core/types.h"
#include "core/log/log.h"
#include "utilities/strings.h"

#include "scene/context.h"
#include "scene/state.h"

namespace en
{
namespace state
{

Context::Context() :
    finish(false),
    action(false),
    stateChange(nullptr),
    stateSet(nullptr),
    stateFinish(false)
{
}

Context::~Context()
{
}

void Context::create(void)
{
    Log << "Starting module: State Manager.\n";
    states.clear();
}

void Context::destroy(void)
{
    Log << "Closing module: State Manager.\n";
}

void Interface::change(State* state) 
{
    if (!StateContext.action)  // TODO: Atomic CmpAndSwap
    {
        StateContext.action      = true;
        StateContext.stateChange = state;
    }
}

bool Interface::set(State* state)    
{
    if (!StateContext.action)  // TODO: Atomic CmpAndSwap
    {
        StateContext.action   = true;
        StateContext.stateSet = state;
    }

    return true;
}

void Interface::finish(void) 
{
    if (!StateContext.action)  // TODO: Atomic CmpAndSwap
    {
        StateContext.action      = true;
        StateContext.stateFinish = true;
    }
}

// Marks current state for destruction.
// Actual action will take place before next update.

// State Manager will close during next update call.
bool Interface::close()                   
{
    StateContext.finish = true;
    return true;
}

// Internal function. Propagates given event 
// through all managed states from current, to
// the oldest one. Each state can decide if it
// want paused states after him to receive this
// event as well.
void HandleEventByState(en::input::Event& event)
{
    for(sint8 i=(StateContext.states.size() - 1); i>=0; --i)
    {
        if (!StateContext.states[i]->input(event))
        {
            break;
        }
    }
}

// Updates all managed states from current, to
// the oldest one. Each state can decide if it
// want paused states after him to be updated.
bool Interface::update(Time dt)
{
    // Finishes state manager execution
    if (StateContext.finish)
    {
        while(!StateContext.states.empty())
        {
            StateContext.states.back()->destroy();
            StateContext.states.pop_back();
        }

        StateContext.finish = false;
        return false;
    }
    else
    if (StateContext.action)
    {
        // Finishes current state and starts new one
        if (StateContext.stateChange)
        {
            if (!StateContext.states.empty()) 
            {
                StateContext.states.back()->destroy();
                StateContext.states.pop_back();
            }
         
            StateContext.states.push_back(StateContext.stateChange);
            StateContext.states.back()->init();

            StateContext.action      = false;
            StateContext.stateChange = nullptr;
        }
        else
        // Pauses current state and switche's to pointed one
        if (StateContext.stateSet)
        {
            if (!StateContext.states.empty())
            {
                StateContext.states.back()->pause();
            }

            StateContext.states.push_back(StateContext.stateSet);
            StateContext.states.back()->init();

            StateContext.action   = false;
            StateContext.stateSet = nullptr;
        }
        else
        if (StateContext.stateFinish)
        {
            if (!StateContext.states.empty()) 
            {
                StateContext.states.back()->destroy();
                StateContext.states.pop_back();
            }

            StateContext.action      = false;
            StateContext.stateFinish = false;
        }
    }

    if (StateContext.states.empty()) 
    {
        return false;
    }

    Input->update();

    for(sint8 i=(StateContext.states.size() - 1); i>=0; --i)
    {
        if (!StateContext.states[i]->update(dt))
        {
            break;
        }
    }

    return true;
}
    
// Draws all managed states from back, oldest
// paused one, to the front, current active one.
bool Interface::draw(void)
{
    if (StateContext.states.empty()) 
    {
        return false;
    }

    for(uint8 i=0; i<StateContext.states.size(); ++i)
    {
        StateContext.states[i]->draw();
    }

    return true;
}

} // en::state

state::Context   StateContext;
state::Interface StateManager;

} // en