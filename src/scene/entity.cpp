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

*/

#include "assert.h"
#include "scene/entity.h"

namespace en
{
namespace scene
{

// Stored in Column-Major order
#define mat(m,r,c) (m)[(c)*4+(r)]

// Stored in Row-Major order
// #define mat(m,r,c) (m)[(r)*4+(c)]

Entity::Entity(void) :
    pPosition( new double3() ),
    pRotation( new float4x4() ),
    pScale( new float3() ),         
    pWorldMatrix( new float4x4() ),
    pBoundingSphere( new float4() ),
    pWorldBoundingSphere( new float4() ),
    pScene(nullptr)
{
    // Local allocations need to be successfull!
    assert( pPosition );
    assert( pRotation );
    assert( pScale );
    assert( pWorldMatrix );
    assert( pBoundingSphere );
    assert( pWorldBoundingSphere );

    // Position and rotation are set in constructors
    // of double3 and float4x4 to center of the 
    // coordinate system and identity matrix. 
    *pScale = float3(1.0f, 1.0f, 1.0f);
}

Entity::~Entity() 
{
    if (pScene)
    {
        return;
    }

    delete pPosition;
    delete pRotation;
    delete pScale; 
    delete pWorldMatrix;
    delete pBoundingSphere;
    delete pWorldBoundingSphere;
}

void Entity::position(const double3 pos)
{
    *pPosition = pos;
}

void Entity::position(const double x, const double y, const double z)
{
    *pPosition = double3(x, y, z);
}          
 
void Entity::position(const float3 pos)
{
    *pPosition = double3(pos.x, pos.y, pos.z);
}
      
void Entity::position(const float x, const float y, const float z)
{
    *pPosition = double3(x, y, z);
}
                    
double3 Entity::position(void) const
{
    return *pPosition;
}

void Entity::scale(const float3 s)
{
    *pScale = s;
}

void Entity::scale(const float x, const float y, const float z)
{
    *pScale = float3(x, y, z);
}

void Entity::scaleX(const float x)
{
    pScale->x = x;
}

void Entity::scaleY(const float y)
{
    pScale->y = y;
}

void Entity::scaleZ(const float z)
{
    pScale->z = z;
}

float3 Entity::scale(void) const
{
    return *pScale;
}

void Entity::direct(const float3 look, const float3 up)
{
    // Assumes storage in Column-Major order
    *reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(look);
    *reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(up);
    *reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(cross(look,up));
}

//void Entity::direct(float4x4& orientation)
//{
//    //*pRotation = orientation;
//    //*reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(float3(-orientation.m[8], -orientation.m[9], -orientation.m[10]));
//    //*reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(float3( orientation.m[4],  orientation.m[5],  orientation.m[5]));
//    //*reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(float3(-orientation.m[0], -orientation.m[1], -orientation.m[2]));
//    //*reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(float3( orientation.m[8], orientation.m[9], orientation.m[10]));
//    //*reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(float3( orientation.m[4], orientation.m[5], orientation.m[5]));
//    //*reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(float3( orientation.m[0], orientation.m[1], orientation.m[2]));
//
//    *reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(float3(-orientation.m[2], -orientation.m[6], -orientation.m[10]));
//    *reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(float3( orientation.m[1],  orientation.m[5],  orientation.m[9]));
//    *reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(float3(-orientation.m[0], -orientation.m[4], -orientation.m[8]));
//}

float3 Entity::direction(void) const
{
    // Assumes storage in Column-Major order
    return *reinterpret_cast<float3*>(&pRotation->m[4]);
}

float3 Entity::top(void) const
{
    // Assumes storage in Column-Major order
    return *reinterpret_cast<float3*>(&pRotation->m[8]);
}

float3 Entity::side(void) const
{
    // Assumes storage in Column-Major order
    return *reinterpret_cast<float3*>(&pRotation->m[0]);
}

void Entity::yaw(const float deg)
{
    float3 look = *reinterpret_cast<float3*>(&pRotation->m[4]);
    float3 side = *reinterpret_cast<float3*>(&pRotation->m[0]);
    float  fcos = cosf(radians(deg));
    float  fsin = sinf(radians(deg));

    *reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(look*fcos + side*fsin);  
    *reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(side*fcos - look*fsin); 
}

void Entity::turn(const float deg)   
{
    float3 look = *reinterpret_cast<float3*>(&pRotation->m[4]);
    float3 side = *reinterpret_cast<float3*>(&pRotation->m[0]);
    float  fcos = cosf(radians(deg));
    float  fsin = sinf(radians(deg));

    *reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(look*fcos + side*fsin);  
    *reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(side*fcos - look*fsin); 
}
            
void Entity::roll(const float deg)
{
    float3 up   = *reinterpret_cast<float3*>(&pRotation->m[8]);
    float3 side = *reinterpret_cast<float3*>(&pRotation->m[0]);
    float  fcos = cosf(radians(deg));
    float  fsin = sinf(radians(deg));

    *reinterpret_cast<float3*>(&pRotation->m[0]) = normalize(side*fcos + up*fsin);   
    *reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(up*fcos - side*fsin);   
}

void Entity::pitch(const float deg)
{
    float3 look = *reinterpret_cast<float3*>(&pRotation->m[4]);
    float3 up   = *reinterpret_cast<float3*>(&pRotation->m[8]);
    float  fcos = cosf(radians(deg));
    float  fsin = sinf(radians(deg));

    *reinterpret_cast<float3*>(&pRotation->m[8]) = normalize(up*fcos + look*fsin);
    *reinterpret_cast<float3*>(&pRotation->m[4]) = normalize(look*fcos - up*fsin);
}

void Entity::move(const double units)
{
    // Assumes look vector to be normalized
    *pPosition += *reinterpret_cast<float3*>(&pRotation->m[4]) * units;
}

void Entity::strafe(const double units)
{
    // Assumes side vector to be normalized
    *pPosition += *reinterpret_cast<float3*>(&pRotation->m[0]) * units;
}

void Entity::moveVertical(const double units)
{
    // Assumes up vector to be normalized
    *pPosition += *reinterpret_cast<float3*>(&pRotation->m[8]) * units;
}

void Entity::ascend(const double units)
{
    // Assumes up vector to be normalized
    *pPosition += *reinterpret_cast<float3*>(&pRotation->m[8]) * units;
}

void Entity::descend(const double units)
{
    // Assumes up vector to be normalized
    *pPosition += *reinterpret_cast<float3*>(&pRotation->m[8]) * -units;
}

float4x4 Entity::matrix(void)
{
    if (!pScene)
    {
        // Local Matrix
        float4x4 mat = *pRotation;
        float*   m = &mat.m[0];
        float3   s = *pScale;
      
        mat(m,0,0) *= s.x;   mat(m,0,1) *= s.y;   mat(m,0,2) *= s.z;   mat(m,0,3) = static_cast<float>(pPosition->x);
        mat(m,1,0) *= s.x;   mat(m,1,1) *= s.y;   mat(m,1,2) *= s.z;   mat(m,1,3) = static_cast<float>(pPosition->y);
        mat(m,2,0) *= s.x;   mat(m,2,1) *= s.y;   mat(m,2,2) *= s.z;   mat(m,2,3) = static_cast<float>(pPosition->z);
        mat(m,3,0)  = 0.0f;  mat(m,3,1)  = 0.0f;  mat(m,3,2)  = 0.0f;  mat(m,3,3) = 1.0f;
        return mat;
    }

    return *pWorldMatrix;
} 
   
float4x4 Entity::rotationMatrix(void)
{
    return *pRotation;
} 
   
float4x4 Entity::translationMatrix(void)
{
    float4x4 mat;
    float*   m = &mat.m[0];
 
    // Matrix is set to identity matrix by it's constructor
    mat(m,0,3) = static_cast<float>(pPosition->x);
    mat(m,1,3) = static_cast<float>(pPosition->y);
    mat(m,2,3) = static_cast<float>(pPosition->z);

    return mat;
} 

} // en::scene
} // en
