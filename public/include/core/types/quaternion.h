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
      quaternion(const float degrees, const float3 vector);
     ~quaternion();
      
      float magnitude(void) const;
      void normalize(void);
      float4x4 matrix(void);
      };
   
   bool       operator==(const quaternion a, const quaternion b);
   quaternion operator- (const quaternion a, const quaternion b);
   quaternion operator+ (const quaternion a, const quaternion b);
   quaternion mul(const quaternion a, const quaternion b);
}

#endif