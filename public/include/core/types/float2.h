/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component float vector.

*/

#ifndef ENG_CORE_TYPES_FLOAT2
#define ENG_CORE_TYPES_FLOAT2

namespace en
{

class float2
{
    public:
    union { float x; float u; float s; float minimum; };
    union { float y; float v; float t; float maximum; };
   
    float2(void);                     
    float2(const float x, const float y);         
   
    void operator-= (const float2 b);      
    void operator+= (const float2 b);         
    bool operator== (const float2 b);
    float2 operator/ (const float2 b) const;
         
    void  normalize(void);       
    float length(void) const;      
};

} // en

#endif
