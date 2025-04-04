/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component float vector.

*/

#ifndef ENG_CORE_TYPES_FLOAT4
#define ENG_CORE_TYPES_FLOAT4

#include "core/types/float3.h"
#include "core/types/double3.h"

namespace en
{

class float4
{
    public:
    union { float x; float r; };
    union { float y; float g; };
    union { float z; float b; };
    union { float w; float a; };
   
    float4(void); 
    float4(const float* src);                                  
    float4(const float3 f3, const float w = 1.0f);                   
    float4(const double3 d3, const float w = 1.0f);                    
    float4(const float x, const float y = 0.0f, const float z = 0.0f, const float w = 1.0f);      

    void operator=  (const float4 b);
    void operator-= (const float4 b);      
    void operator+= (const float4 b);         
   
    float3 xyz(void) const;
    void normalize(void);                    
};

} // en

#endif
