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
   class quaternion
      {
      public:
      union { float s; float q0; };
      union { float x; float q1; };
      union { float y; float q2; };
      union { float z; float q3; };
      
      quaternion();
      quaternion(float degrees, float3 vector);
     ~quaternion();
      
      float magnitude(void);
      void normalize(void);
      float4x4 matrix(void);
      };
   
   bool operator ==(quaternion a, quaternion b);
   quaternion operator- (quaternion a, quaternion b);
   quaternion operator+ (quaternion a, quaternion b);
   quaternion mul(quaternion a, quaternion b);
}

#endif