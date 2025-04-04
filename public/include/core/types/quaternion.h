/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : Quaternion versor.

*/

#ifndef ENG_CORE_TYPES_QUATERNION
#define ENG_CORE_TYPES_QUATERNION

#include "core/types/float3.h"
#include "core/types/float4x4.h"

namespace en
{

class Quaternion
{
    public:
    union { float s; float q0; };
    union { float x; float q1; };
    union { float y; float q2; };
    union { float z; float q3; };
   
    Quaternion();
    Quaternion(const float degrees, const float3 vector);
   ~Quaternion();
   
    float magnitude(void) const;
    void normalize(void);
    float4x4 matrix(void);
};

bool       operator==(const Quaternion a, const Quaternion b);
Quaternion operator- (const Quaternion a, const Quaternion b);
Quaternion operator+ (const Quaternion a, const Quaternion b);
Quaternion mul(const Quaternion a, const Quaternion b);

} // en

#endif