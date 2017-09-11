/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component double vector.

*/

#ifndef ENG_CORE_TYPES_DOUBLE3
#define ENG_CORE_TYPES_DOUBLE3

namespace en
{
  class double3
     {
     public:
     union { double x; double r; };
     union { double y; double g; };
     union { double z; double b; };
     
     double3(void);                         
     double3(const double x, const double y, const double z); 
     
     void    operator-= (const double3 b);          
     void    operator+= (const double3 b);  
     void    operator*= (const double b);  
     void    operator/= (const double b);  
     bool    operator== (const double3 b) const;
     bool    operator!= (const double3 b) const;
     double3 operator-  () const;
     
     void normalize(void);                
     double length(void) const;
     };
}

#endif
