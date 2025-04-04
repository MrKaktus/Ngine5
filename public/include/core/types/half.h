/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : IEEE 754 16bit half float.

*/

#ifndef ENG_CORE_TYPES_HALF
#define ENG_CORE_TYPES_HALF

namespace en
{

class half
{
    public:
    //union 
    //{
    //    struct 
    //    {
    //    uint16 mantista : 10;
    //    uint16 exponent : 5;
    //    uint16 sign     : 1; 
    //    };
          uint16 value;
    //};
   
    half(void);                         
    half(const float src); 
};

} // en

#endif
