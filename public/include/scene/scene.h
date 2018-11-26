/*

 Ngine v5.0
 
 Module      : Scene
 Requirements: none
 Description : Manages renderable objects, cameras,
               lights, souds sources and other types
               of objects that can be used to compose
               a scene.

*/

#ifndef ENG_SCENE
#define ENG_SCENE

#include <vector>

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/array.h" 
#include "scene/entity.h"
#include "scene/cam.h"
#include "scene/drawable.h"
#include "scene/axes.h"

namespace en
{
   namespace scene
   {
   // Handle of entity in scene
   struct Handle
          {
          uint32 childs;
          uint32 layer;
          uint32 index; 
          };

   // Structure of Arrays
   struct Layer
          {
          // Source
          array<double3>  position;
          array<float4x4> rotation;
          array<float3>   scale;
          array<float4>   boundingSphere;
          array< std::shared_ptr<Entity> > entity;
       
          // Destination      
          array<float4x4> worldMatrix; 
          array<float4>   worldBoundingSphere;
   
          // Data management 
          uint32 count; 
        
          Layer();
         ~Layer();
          };

   // Scene
   class Scene
         {
         private:
         // Data Oriented Design, Nodes are stored as
         // Structure of Arrays to align data in memory
         // and prevent cache misses.
         array<Handle> entities;
         array<Layer>  layers;
         uint32        depth;   // Layers count
         uint32        count;   // Total entities count

         void resizeLayer(uint8 i);

         public:
         Scene(uint32 entities = 16384);
        ~Scene();

         bool add(std::shared_ptr<Entity> object);                     // Add new entity to the scene
         bool add(std::shared_ptr<Entity> object, std::shared_ptr<Entity> parent); // Add new entity to the scene as child object
         bool remove(uint32 handle);                       // Remove entity from scene
         void update(void);                                // Update scene
         };
   }
}

#endif
