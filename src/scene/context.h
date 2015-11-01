/*

 Ngine v5.0
 
 Module      : Scene
 Requirements: none
 Description : Manages renderable objects, cameras,
               lights, souds sources and other types
               of objects that can be used to compose
               a scene.

*/

#ifndef ENG_SCENE_CONTEXT
#define ENG_SCENE_CONTEXT

#include "scene/scene.h" 
#include "scene/state.h" 

#include <vector>
using namespace std;

namespace en
{
   namespace scene
   {
   //struct Context
   //       {
   //       Context();
   //      ~Context();

   //       void create(void);
   //       void destroy(void);
   //       };
   }

   namespace state
   {
   struct Context
          {
          vector<State*> states;
          bool   finish;
          bool   action;
          State* stateChange;
          State* stateSet;
          bool   stateFinish;

          Context();
         ~Context();

          void create(void);
          void destroy(void);
          };

   void HandleEventByState(en::input::Event& event);
   }

//extern scene::Context SceneContext;
extern state::Context StateContext;
}

#endif
