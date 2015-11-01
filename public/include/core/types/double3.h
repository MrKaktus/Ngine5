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
     double3(double x, double y, double z); 
     
     void    operator-= (double3 b);          
     void    operator+= (double3 b);  
     void    operator*= (double b);  
     void    operator/= (double b);  
     bool    operator== (double3 b);
     bool    operator!= (double3 b);
     double3 operator-  ();
     
     void normalize(void);                
     double length(void);
     };
}

#endif
