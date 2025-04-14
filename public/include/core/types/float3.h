/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component float vector.

*/

#ifndef ENG_CORE_TYPES_FLOAT3
#define ENG_CORE_TYPES_FLOAT3

#include "core/types/double3.h"

namespace en
{

class float3
{
    public:
    union
    {
        struct
        {
            union { float x; float r; float u; };
            union { float y; float g; float v; };
            union { float z; float b; float w; };
        };

        float value[3];
    };

    float3(void);                       
    float3(const float* src); 
    float3(const double3 d3);                
    float3(const float x, const float y, const float z);   

    void   operator-= (const float3 b);          
    void   operator+= (const float3 b);  
    void   operator/= (const float b);
    void   operator*= (const float b);
    bool   operator!= (const float3 b) const;
    float3 operator-  () const;      
    operator double3() const;

    void   normalize(void);               
    float  length(void) const;
};

} // en

#endif
