/*

 Ngine v5.0

 Module      : Pose
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_POSE
#define ENG_CORE_XR_POSE

#include "core/types.h"
#include "core/types/double3.h"

namespace en
{
namespace xr
{

class Orienatation
{
    public:
    virtual float      roll(void) = 0;
    virtual float      pitch(void) = 0;
    virtual float      yaw(void) = 0;
    virtual float4x4   rotationMatrix(void) = 0;
    virtual Quaternion quaternion(void) = 0;

    virtual ~Orienatation() {};
};

class Pose
{
    public:
    double3       position;
    Orienatation* orientation;
    float4x4      transform;
    float3        velocity;
    float3        angularVelocity;
    float3        acceleration;
    float3        angularAcceleration;

    virtual ~Pose() {};
};

} // en::xr
} // en

#endif