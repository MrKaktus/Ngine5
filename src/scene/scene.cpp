/*

 Ngine v5.0
 
 Module      : Scene
 Requirements: none
 Description : Manages renderable objects, cameras,
               lights, souds sources and other types
               of objects that can be used to compose
               a scene.

*/

#include "scene/context.h" 
#include "utilities/utilities.h"   // nextPowerOfTwo, whichPowerOfTwo
#include "utilities/gpcpu/gpcpu.h"

namespace en
{
   namespace scene
   {

   // Logic rules:
   //
   // 1 - Object is constructed outside the Scene.
   //     EX: It can be any type of class that inherits from Entity and Scene 
   //         doesn't know that, so it cannot produce entities on it's own.
   //
   // 2 - Scene wants to store all object elements in POD arrays for DOD.
   //     Objects should be only collectors of references/pointers to their elements.
   //
   // 3 - (1,2)-> when object is constructed, it is not complete.
   //
   // 4 - (3)-> Object construction, and adding to Scene as well as to other managers,
   //           should be encapsulated in higher order function that will result final
   //           proper object or nothing.
   //           Such function call new on preallocated buffer of memory for N objects
   //           of thesame type. Then initializes them in all subsystems like Scene, 
   //           physics, etc. by passing pointer to their memory location.
   //     EX: This ways final object is always correct and has no missing parts.
   //
   // 5 - (2)-> it means that if objects stores pointers, Scene cannot rearrange data!
   //           At least without having pointer back to object.
   //
   // 6 - (5)-> Scene needs to have handle/pointer to object to be able to fully rearrange structures.
   //
   // 7 - As object can be part of many subsystems, it will call it's destruction in
   //     all that subsystems when it's destructor is called. This means Scene cannot 
   //     hold safe "strong" pointer Ptr<Entity> but regular "weak" pointer to object.
   //
   // 8 - (7)-> As object can request it's destruction from subsystem at any moment,
   //     subsystem needs to be ALWAYS able to destroy it. So in Scene, calling remove
   //     on object that has child objects attached, is not a Scene error but objects
   //     destructor fault! ( for e.g. it wasn't maintained with safe pointers ).
   //
   // 9 - Scene wants to store data in CONTINUOUS arrays for cache locality and prefetching!
   //
   // 10 - (9)-> Removing object requires data rearrangement to fulfill that.
   //
   // 11 - (7,8)-> Object can be removed only if it has no childs (it is LEAF in scene).
   //
   // 12 - (11)-> We replace REMOVED object with LAST one in current tree level.
   //             This means, we need to correct PARENT pointers in all CHILDS on lower level, to LAST object that we moved.
   //           - It can be done by brute force interation through next level and searching for matching pointers and correcting them.
   
   // 2015 UPDATE TO LOGIC:
   //
   // 13 - (8, 11) Object need to be able to remove itself and leave childs! 
   //              For example enemy is killed and it's gun is detached from him.
   //              From now on gun is not controled by NPC hand, but is separate
   //              entity that can be picked up.
   //              So childs of removede parent becomes childs of it parent.
   //              This require TRS update of each child, to transfer it from
   //              one local space to another.

   // How to remove object ? (knowing only it's address)
   // --------------------------------------------------
   // - Object stores pointers to POD's in arrays
   // - POINTER's are DIRECT! -> How to extract which level it is? Which array?
   //   A: If object is stored in the same layer/index in different Array!
   
   
   // Requirements:
   // A - Objects want direct access to their data!
   // B - Objects can be of different type (tank, soldier, camera) and they all inherit from Entity!
   // C - We want to store objects internal parameters in CONTINUOS ARRAYS of data aligned POD's!
   // D - Scene should produce final transformation matrices.
   // E - Scene should produce final list of objects that are visible for given frustum (culling).
    
   // C -> Requires maintaining arrays. 
   //      Objects can be randomly removed.
   //      Fastest way is to replace with last element in array.
   //      This requires fixing pointer to data in Object owning last element.
   //      There need to be some kind of way back to object owning data.
    
   // We got OBJECT POINTER to remove.
   // We have access to everything that this object has access to.
   // Let's search for object in object's list (list of pointers). -> It is vector sorted by pointer adresses!
   // vector< { Ptr<Entity> adres, level, index } > -> it points one way into global arrays:
   // vector< vector< { childs, {all entity data in other arrays at the same place} } >index >level
   // POD's referenced by pointers cannot be in vectors, as they can move in memory!
   
   // How to add object ? 
   
   // Store information about childs references in Scene:
   // l0 [ Entity* ] 
   // l1 [ Entity* ][ Entity* ][ Entity* ]
   // store childs count in Entities ? NO! -> DOD!

   // Entity - is a set of pointers to data in DOD structures.
   //          This data are stored in Scene managing object.
   //          Therefore Scene produces entities, and it should
   //          destroy them.
   // 
   //          Different game objects should request new entities 
   //          from Scene object, and later request their destruction.
   //
   //          New entity can only be created as child of other 
   //          entity if it exists or root object. It can later 
   //          destroyed only if all it's childs are destroyed.
   //
   //          Are entities stored in array also? ( in Scene or outside? )

   // (?)
   // It is logical to manipulate directly on entity's data (move, roll, etc.)
   // So entity should be inheritable. But how Scene can produce entities, 
   // when it doesn't know all the classes that will inherit from it?
   //
   // Object that inherits from Entity is created.
   // As it is not connected to any scene, it has dangling pointers. (!) 
   // -> OR pointers to separate objects alocated in RAM (that are freed when object is attached to scene?)
   // It is now being attached to scene, which creates resources for it, and attaches them to pointers.
   // Creation can fail (if parent object is not present) (!) 
   // -> This is why we cannot attach object to scene at Construction time!
   //    -> Except if we will pass Ptr<> or Reference to parent ! -> But reference requires it to be present!




   Layer::Layer() :
      position(0),
      rotation(0),
      scale(0),
      boundingSphere(0),
      entity(0),
      worldMatrix(0),
      worldBoundingSphere(0),
      count(0)
   {
   }
   
   Layer::~Layer()
   {
   for(uint32 i=0; i<entity.size; ++i)
      entity[i].~Ptr();
   }

   Scene::Scene(uint32 entities) :
      count(0)
   {
   // Calculate starting size of root layer and layers count
   uint32 size = static_cast<uint32>(nextPowerOfTwo(entities));
   if (size == 0)
      size = 16384;
   whichPowerOfTwo(size, depth);
   if (depth > 8)
      depth = 8;
   
   this->entities.resize(size);
   
   // By default scene will be up to 8 layers deep, 
   // and each layer will have reserved space two 
   // times smaller than the previous one.
   layers.resize(depth);
   for(uint8 i=0; i<depth; ++i)
      {
      new (&layers[i]) Layer();
      if (size > 0)
         {
         bool success = true;
         success &= layers[i].position.resize(size);
         success &= layers[i].rotation.resize(size);
         success &= layers[i].scale.resize(size);
         success &= layers[i].boundingSphere.resize(size);
         success &= layers[i].entity.resize(size);
         success &= layers[i].worldMatrix.resize(size);
         success &= layers[i].worldBoundingSphere.resize(size);
   
         assert(success);

         for(uint32 j=0; j<size; ++j)
            new (&layers[i].entity[j]) Ptr<Entity>(NULL);

         if (size > 1)
            size /= 2;
         else
            size = 0;
         }
      }
   }
   
   Scene::~Scene()
   {
   }
   
   void Scene::resizeLayer(uint8 i)
   {
   uint32 size = layers[i].entity.size * 2;
   
   bool success = true;
   success &= layers[i].position.resize(size);
   success &= layers[i].rotation.resize(size);
   success &= layers[i].scale.resize(size);
   success &= layers[i].boundingSphere.resize(size);
   success &= layers[i].entity.resize(size);
   success &= layers[i].worldMatrix.resize(size);
   success &= layers[i].worldBoundingSphere.resize(size);
   
   assert(success);

   for(uint32 j=(size/2); j<size; ++j)
      new (&layers[i].entity[j]) Ptr<Entity>(NULL);

   // Update entities direct pointers to data
   for(uint32 j=0; j<layers[i].count; ++j)
      {
      layers[i].entity[j]->pPosition            = &layers[i].position[j];
      layers[i].entity[j]->pRotation            = &layers[i].rotation[j];
      layers[i].entity[j]->pScale               = &layers[i].scale[j];
      layers[i].entity[j]->pBoundingSphere      = &layers[i].boundingSphere[j];
      layers[i].entity[j]->pWorldMatrix         = &layers[i].worldMatrix[j];
      layers[i].entity[j]->pWorldBoundingSphere = &layers[i].worldBoundingSphere[j];
      }
   }
   
   bool Scene::add(Ptr<Entity> object)
   {
   // Check if object is not already attached to some scene
   if (object->pScene)
      return false;
   
   // Reserve handle for object
   uint32 handle = 0;
   if (count == entities.size)
      if (!entities.resize(entities.size * 2))
         return false;
   handle = count;
   ++count;
   
   // Fill in objects location
   entities[handle].childs = 0;
   entities[handle].layer  = 0;
   entities[handle].index  = layers[0].count;
   
   // Resize layer if full
   if (layers[0].count == layers[0].entity.size)
      resizeLayer(0);
   ++layers[0].count;
   
   uint32 index = entities[handle].index;
   
   // Attach object to scene
   layers[0].entity[index] = object;
   object->pScene  = this;
   object->handle = handle;

   // Copy object data to scene arrays   
   layers[0].position[index]            = *object->pPosition;
   layers[0].rotation[index]            = *object->pRotation;
   layers[0].scale[index]               = *object->pScale;
   layers[0].boundingSphere[index]      = *object->pBoundingSphere;
   layers[0].worldMatrix[index]         = *object->pWorldMatrix;
   layers[0].worldBoundingSphere[index] = *object->pWorldBoundingSphere;

   // Delete object local memory
   delete object->pPosition;
   delete object->pRotation;
   delete object->pScale;
   delete object->pBoundingSphere;
   delete object->pWorldMatrix;
   delete object->pWorldBoundingSphere;

   // Attach data structures to object
   object->pPosition            = &layers[0].position[index];
   object->pRotation            = &layers[0].rotation[index];
   object->pScale               = &layers[0].scale[index];
   object->pBoundingSphere      = &layers[0].boundingSphere[index];
   object->pWorldMatrix         = &layers[0].worldMatrix[index];
   object->pWorldBoundingSphere = &layers[0].worldBoundingSphere[index];
   
   return true;
   }
   
   bool Scene::add(Ptr<Entity> object, Ptr<Entity> parent)
   {
   // Check if object is not already attached to some scene
   if (object->pScene)
      return false;
   
   // Check if parent is attached to this scene
   if (parent->pScene != this)
      return false;
   
   // Check if parent handle is not out of range
   if (parent->handle >= count)
      return false;
   
   // Resize entities list if needed
   if (count == entities.size)
      if (!entities.resize(entities.size * 2))
         return false;
   
   // Add new scene layer if needed
   uint32 parentLayer = entities[parent->handle].layer;
   if (parentLayer == depth - 1)
      {
      if (!layers.resize(depth + 1))
         return false;
      ++depth;
      }
   
   // Mark as a child
   entities[parent->handle].childs++;
   object->parent = parent;
   
   // Fill in objects location
   uint32 handle = count;
   uint32 layer  = parentLayer + 1;
   uint32 index  = layers[layer].count;
   ++count;
   entities[handle].childs = 0;
   entities[handle].layer  = layer;
   entities[handle].index  = index;
   
   // Resize layer if full
   if (layers[layer].count == layers[layer].entity.size)
      resizeLayer(layer);
   ++layers[layer].count;
   
   // Attach object to scene
   layers[layer].entity[index] = object;
   object->pScene  = this;
   object->handle = handle;
   
   // Copy object data to scene arrays   
   layers[layer].position[index]            = *object->pPosition;
   layers[layer].rotation[index]            = *object->pRotation;
   layers[layer].scale[index]               = *object->pScale;
   layers[layer].boundingSphere[index]      = *object->pBoundingSphere;
   layers[layer].worldMatrix[index]         = *object->pWorldMatrix;
   layers[layer].worldBoundingSphere[index] = *object->pWorldBoundingSphere;

   // Delete object local memory
   delete object->pPosition;
   delete object->pRotation;
   delete object->pScale;
   delete object->pBoundingSphere;
   delete object->pWorldMatrix;
   delete object->pWorldBoundingSphere;

   // Attach data structures to object
   object->pPosition            = &layers[layer].position[index];
   object->pRotation            = &layers[layer].rotation[index];
   object->pScale               = &layers[layer].scale[index];
   object->pBoundingSphere      = &layers[layer].boundingSphere[index];
   object->pWorldMatrix         = &layers[layer].worldMatrix[index];
   object->pWorldBoundingSphere = &layers[layer].worldBoundingSphere[index];
   
   return true;
   }
   
   bool Scene::remove(uint32 handle)
   {
   // Check if handle is not out of range
   if (handle >= count)
      return false;
   
   // Check if object is not a parent object
   if (entities[handle].childs)
      return false;
   
   uint32 layer = entities[handle].layer;
   uint32 index = entities[handle].index;
   uint32 last = count - 1;
   
   // Detach object from scene
   layers[layer].entity[index]->pScene = NULL;
   layers[layer].entity[index]->handle = 0;
   
   // Remove object from entity list 
   entities[handle] = entities[last];
   layers[ entities[last].layer ].entity[ entities[last].index ]->handle = handle;
   --count;
   
   // Remove object from layer arrays 
   last = layers[layer].count - 1;
   layers[layer].position[index]            = layers[layer].position[last];
   layers[layer].rotation[index]            = layers[layer].rotation[last];
   layers[layer].scale[index]               = layers[layer].scale[last];
   layers[layer].boundingSphere[index]      = layers[layer].boundingSphere[last];
   layers[layer].entity[index]              = layers[layer].entity[last];
   layers[layer].worldMatrix[index]         = layers[layer].worldMatrix[last];
   layers[layer].worldBoundingSphere[index] = layers[layer].worldBoundingSphere[last];
   
   uint32 lastHandle = layers[layer].entity[last]->handle;
   entities[lastHandle].index = index;
   return true;
   }
   
   void Scene::update(void)
   {
   // Create world matrix 
   for(uint32 i=0; i<layers[0].count; ++i)
      {
      float4x4& mat = layers[0].worldMatrix[i];
      float4x4& rot = layers[0].rotation[i];
      double3&  p   = layers[0].position[i];
      float3&   s   = layers[0].scale[i];
   
      memcpy(&mat, &rot, sizeof(float4x4));
      mat.m[0] *= s.x;   mat.m[4] *= s.y;   mat.m[8]  *= s.z;   mat.m[12] = static_cast<float>( p.x );
      mat.m[1] *= s.x;   mat.m[5] *= s.y;   mat.m[9]  *= s.z;   mat.m[13] = static_cast<float>( p.y );
      mat.m[2] *= s.x;   mat.m[6] *= s.y;   mat.m[10] *= s.z;   mat.m[14] = static_cast<float>( p.z );
      }
   
   // Update world matrixes for child objects
   float4x4  local;
   for(uint8 layer=1; layer<depth; ++layer)
      {
      for(uint32 index=0; index<layers[layer].count; ++index)
         {
         float4x4& mat = layers[layer].worldMatrix[index];
         float4x4& rot = layers[layer].rotation[index];
         double3&  p   = layers[layer].position[index];
         float3&   s   = layers[layer].scale[index];

         // Create local matrix   
         memcpy(&local, &rot, sizeof(float4x4));
         local.m[0] *= s.x;   local.m[4] *= s.y;   local.m[8]  *= s.z;   local.m[12] = static_cast<float>( p.x );
         local.m[1] *= s.x;   local.m[5] *= s.y;   local.m[9]  *= s.z;   local.m[13] = static_cast<float>( p.y );
         local.m[2] *= s.x;   local.m[6] *= s.y;   local.m[10] *= s.z;   local.m[14] = static_cast<float>( p.z );

         // Parent world matrix
         Ptr<Entity> parent = layers[layer].entity[index]->parent;
         uint32 parentIndex = entities[ parent->handle ].index;
         float4x4& parentMatrix = layers[layer - 1].worldMatrix[parentIndex];
   
         // Update world matrix
         mat = mul( parentMatrix, local );
         }
      }
   }

   }

//scene::Context SceneContext;
//scene::Scene   Scene;
}
