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

#include <memory>

#include "core/utilities/NonCopyable.h"
#include "utilities/gpcpu/gpcpu.h"

namespace en
{
namespace scene
{

class Scene;

class Entity : private NonCopyable
{
    protected: /*  8  */ 
    // When attached to scene, these parameters are stored in arrays 
    // for data locality. Otherwise they are freely alocated.

    double3*  pPosition;
    float4x4* pRotation;
    float3*   pScale;             
    float4x4* pWorldMatrix;           /*  4  */ 
    float4*   pBoundingSphere;        /*  4  */ 
    float4*   pWorldBoundingSphere;   /*  4  */ 
 
    // DIRTY:
    Scene*    pScene;   // Scene this entity belongs to
    uint32    handle;   // Handle in scene

    std::shared_ptr<Entity> parent; // Parent entity/* 4  */ 

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
    double3 position(void) const;             // Get position
    void    scale(const float3 s);            // Set global scale
    void    scale(const float x,              
                  const float y,              
                  const float z);                  
    void    scaleX(const float x);            
    void    scaleY(const float y);            
    void    scaleZ(const float z);            
    float3  scale(void) const;                // Get global scale
    void    direct(const float3 look,         
                   const float3 up);          // Set orientation
    //void    direct(float4x4& orientation);
    float3  direction(void) const;            // Get direction
    float3  top(void) const;                  // Get top
    float3  side(void) const;                 // Get side vector (right vector)
                                              
    void    yaw(const float deg);             // Rotates around axis UP   (positive left) 
    void    turn(const float deg);            
    void    roll(const float deg);            // Rotates around axis LOOK (positive right) 
    void    pitch(const float deg);           // Rotates around axis SIDE (positive up)
    
    void    move(const double units);         // Moves forwar/backward (positive forward)
    void    strafe(const double units);       // Moves horizontally (positive right)
    void    moveVertical(const double units); // Moves vertically (positive up)
    void    ascend(const double units);
    void    descend(const double units);
    
    float4x4 matrix(void);                    // Returns Object Space -> World Space matrix
    float4x4 rotationMatrix(void);            // Returns Object Space -> World Space rotation matrix
    float4x4 translationMatrix(void);         // Returns Object Space -> World Space translations matrix
};

} // en::scene
} // en

#endif
