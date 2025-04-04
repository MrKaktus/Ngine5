/*

 Ngine v5.0
 
 Module      : Basic point
 Requirements: math.h gpcpu.h
 Description : Describes basic state of any object in 3d space,
               like position and orientation. Side vector is
               calculated with right-handed orientation rule.
               All rotations are counter-clock-wise.


                  .(b)Up    Roll right(+)
       (a)Look.   |                \   /|
              \   |                .-> '--> Turn left (+)
               \  |    Pitch       \ \  |
                \ |    down(+) <-.    \ |
   (c)Side.______\|              -|----\|
 Move left (+)                   '
 
*/

#ifndef ENG_SCENE_ORIENTED
#define ENG_SCENE_ORIENTED

// TODO: Is it deprecated ? Should it be removed ?

#include <math.h>
#include "utilities/gpcpu/gpcpu.h"

namespace en
{

class Noriented
{
    protected:    
    double3 p;           // Position
    float3  a;           // LOOK 
    float3  b;           // UP   
    float3  c;           // SIDE
    float3  s;           // Scale
      
    public:
    Noriented();                                         // Constructor
    Noriented(double3& pos);                             // Constructor with given position
    Noriented(double3& pos, float3& look, float3& up);   // Constructor with all needed data

    double3 position(double3 pos);                       // Set position      
    double3 position(void);                              // Get position
    float3  scale(float3 s);                             // Set scale
    float3  scale(void);                                 // Get scale
    void    direct(float3 look, float3 up);              // Set orientation
    float3  direction(void);                             // Get direction
    float3  top(void);                                   // Get top
    float3  side(void);                                  // Get side vector

    void yaw(float deg);                                 // Rotates around axis UP 
    void roll(float deg);                                // Rotates around axis LOOK
    void pitch(float deg);                               // Rotates around axis SIDE

    void move(double units);                             // Moves front(+)/back(-)
    void strafe(double units);                           // Moves left(+)/right(-)
    void moveVertical(double units);                     // Moves up(+)/down(-)    (ascend/descend)

    float4x4 matrix(void);                               // Returns Object Space -> World Space matrix
    float4x4 rotationMatrix(void);                       // Returns Object Space -> World Space rotation matrix
    float4x4 translationMatrix(void);                    // Returns Object Space -> World Space translations matrix
};

} // en
   
#endif
