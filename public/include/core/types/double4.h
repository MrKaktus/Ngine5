/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component double vector.

*/

#ifndef ENG_CORE_TYPES_DOUBLE4
#define ENG_CORE_TYPES_DOUBLE4

#include "core/types/float3.h"

namespace en
{

class double4
{
    public:
    union { double x; double r; };
    union { double y; double g; };
    union { double z; double b; };
    union { double w; double a; };
   
    double4(void);                                   
    double4(const float3 f3);
    double4(const double3 d3, const double w = 1.0f);                   
    double4(const double x, const double y, const double z, const double w); 
   
    void operator-= (const double4 b);      
    void operator+= (const double4 b);         
   
    double3 xyz(void) const;
    void normalize(void);                       
};

} // en

#endif
