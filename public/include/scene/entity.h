/*

 Ngine v5.0
 
 Module      : Entity
 Requirements: none
 Description : Describes basic state of any object in 3d space,
               like position and orientation. Side vector is
               calculated with right-handed orientation rule.
               All rotations are clock-wise positive.

               Default orientation:            All rotations are clock-wise positive:
                                               ( looking from the axis origin to the arrow )

                      Up (Y+)                         Up (Y+)
                      .                               . 
                     /|\                             /|\
                      |                             _ |_
                      |                            /  | \
                      |                            |/ |    .
                      |                               |     \
                      +--------> Side (X+)            +------|-> Side (X+)
                     /           LEFT                /       |
                    /                            /____    |\/
                   /                               /  \                     
                 |/                              |/   /
                 `                               `
                 Look (Z+)                       Look (Z+)

RHS -> "positive rotation is CCW about the axis of rotation"
LHS -> "positive rotation is CW about the axis of rotation"
https://www.evl.uic.edu/ralph/508S98/coordinates.html
THis means that RHS is CCW when looking at rotation from arrow to the origin of coordinate system.

RHS rotated by 180 degree around Y axis:
     +x left    +y top   +z front

    - Engine

RHS  +x right   +y top   -z front

    - OpenGL
    - 3dsMax (camera space)
    - Maya
    - Cheetah3D

LHS  +x right   +y top   +z front

    - Direct3D
    - LightWave
    - Cinema4D

LHS rotated by 180 degree around Y axis:
     +x left    +y top   +z back

    - Unity

LHS (z as Up, y as DEPTH)
    +x right   +y front   +z up

    - 3dsMax (world space)
    - Unreal Engine 4
    - Blender

*/

#ifndef ENG_SCENE_ENTITY
#define ENG_SCENE_ENTITY

#include "core/utilities/NonCopyable.h"
#include "core/utilities/TintrusivePointer.h"
#include "utilities/gpcpu/gpcpu.h"

namespace en
{
   namespace scene
   {
   class Scene;

   class Entity : public SafeObject<Entity>, private NonCopyable
/* 8  */ {
         protected:
         // When attached to scene, these parameters are stored in arrays for data locality.
         // Otherwise they are freely alocated.

         double3*  pPosition;
         float4x4* pRotation;
         float3*   pScale;             
/* 4  */ float4x4* pWorldMatrix; 
/* 4  */ float4*   pBoundingSphere;
/* 4  */ float4*   pWorldBoundingSphere;
 
         // DIRTY:
         Scene*    pScene;   // Scene this entity belongs to
         uint32    handle;   // Handle in scene

/* 4  */ Ptr<Entity> parent; // Parent entity

         // Scene that owns it, can manipulate with all data of entity
         friend class Scene; 

         public:
         Entity(void);                             // Create object 
         Entity(Scene& scene);                     // Create object on Scene
         Entity(Entity& parent);                   // Create child object 
        ~Entity();                                 // Destroy object

         void    position(const double3 pos);      // Set position 
         void    position(const double x,          
                          const double y,          
                          const double z);            
         void    position(const float3 pos);       
         void    position(const float x,           
                          const float y,           
                          const float z);                    
         double3 position(void);                   // Get position
         void    scale(const float3 s);            // Set global scale
         void    scale(const float x,              
                       const float y,              
                       const float z);                  
         void    scaleX(const float x);            
         void    scaleY(const float y);            
         void    scaleZ(const float z);            
         float3  scale(void);                      // Get global scale
         void    direct(const float3 look,         
                        const float3 up);          // Set orientation
         void    direct(float4x4& orientation);
         float3  direction(void);                  // Get direction
         float3  top(void);                        // Get top
         float3  side(void);                       // Get side vector
                                                   
         void    yaw(const float deg);             // Rotates around axis UP   (positive left) 
         void    turn(const float deg);            
         void    roll(const float deg);            // Rotates around axis LOOK (positive right) 
         void    pitch(const float deg);           // Rotates around axis SIDE (positive down)
   
         void    move(const double units);         // Moves forwar/backward (positive forward)
         void    strafe(const double units);       // Moves horizontally (positive left)
         void    moveVertical(const double units); // Moves vertically (positive up)
         void    ascend(const double units);
         void    descend(const double units);

         float4x4 matrix(void);                    // Returns Object Space -> World Space matrix
         float4x4 rotationMatrix(void);            // Returns Object Space -> World Space rotation matrix
         float4x4 translationMatrix(void);         // Returns Object Space -> World Space translations matrix
         };
   }
}

#endif
