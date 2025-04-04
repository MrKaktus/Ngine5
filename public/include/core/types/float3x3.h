/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3x3 float matrix.

*/

#ifndef ENG_CORE_TYPES_FLOAT3x3
#define ENG_CORE_TYPES_FLOAT3x3

namespace en
{

// Stored in Column-Major order
class float3x3
{
    public:
    float m[9];
   
    float3x3();           
    float3x3(const float* src); 
    float3x3(const float m00, const float m01, const float m02, 
             const float m10, const float m11, const float m12, 
             const float m20, const float m21, const float m22); 
   
    void set(const float* src);
};

} // en

#endif
