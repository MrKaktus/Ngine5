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
      float2(float x, float y);         
      
      void operator-= (float2 b);      
      void operator+= (float2 b);         
      bool operator== (float2 b);
      float2 operator/ (float2 b);
            
      void  normalize(void);       
      float length(void);      
      };
}

#endif
