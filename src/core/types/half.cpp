/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : IEEE 754 16bit half float.

*/

#include <math.h>
#include <string.h>
#include "core/types/basic.h"
#include "core/types/half.h"

#define fp16exponentMask 0x7C00
#define fp32exponentMask 0x7F800000
#define fp64exponentMask 0x7FF0000000000000
#define fp80exponentMask 0x7FFF0000000000000000

#define fp16exponentShift 10
#define fp32exponentShift 23
#define fp64exponentShift 52
#define fp80exponentShift 64

#define fp16exponentFull 0x1F
#define fp32exponentFull 0xFF
#define fp64exponentFull 0x7FF
#define fp80exponentFull 0x7FFF

#define fp16mantistaMask 0x03FF
#define fp32mantistaMask 0x007FFFFF
#define fp64mantistaMask 0x000FFFFFFFFFFFFF
#define fp80mantistaMask 0x0000FFFFFFFFFFFFFFFF

namespace en
{

uint16 base[512];
uint8 shift[512];

void initHalfs(void)
{
    uint32 i;
    sint32 e;
    for(i=0; i<256; ++i)
    {
        e = i-127;
        // Very small numbers map to zero
        if (e < -24)
        { 
            base[ i|0x000 ] = 0x0000;
            base[ i|0x100 ] = 0x8000;
            shift[ i|0x000 ] = 24;
            shift[ i|0x100 ] = 24;
        }
        else 
        // Small numbers map to denorms
        if (e < -14)
        { 
            base[ i|0x000 ] = (0x0400 >> (-e-14));
            base[ i|0x100 ] = (0x0400 >> (-e-14)) | 0x8000;
            shift[ i|0x000 ] = -e-1;
            shift[ i|0x100 ] = -e-1;
        }
        else 
        // Normal numbers just lose precision
        if (e <= 15)
        { 
            base[ i|0x000 ] = ((e+15) << 10);
            base[ i|0x100 ] = ((e+15) << 10) | 0x8000;
            shift[ i|0x000 ] = 13;
            shift[ i|0x100 ] = 13;
        }
        else 
        // Large numbers map to Infinity
        if (e < 128)
        { 
            base[ i|0x000 ] = 0x7C00;
            base[ i|0x100 ] = 0xFC00;
            shift[ i|0x000 ] = 24;
            shift[ i|0x100 ] = 24;
        }
        // Infinity and NaN's stay Infinity and NaN's
        else
        { 
            base[ i|0x000 ] = 0x7C00;
            base[ i|0x100 ] = 0xFC00;
            shift[ i|0x000 ] = 13;
            shift[ i|0x100 ] = 13;
        }
    }
}

half::half(void) :
    value(0)
{
}

half::half(const float src)
{
    // Used fast conversion algorithm described here:
    // ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf
    //
    uint32 exponent = (*reinterpret_cast<const uint32*>(&src) >> fp32exponentShift) & 0x1ff;
    value = base[exponent] + ((*reinterpret_cast<const uint32*>(&src) & fp32mantistaMask) >> shift[exponent]);
}

} // en

//struct fp16
//{
//    uint32 mantista : 10;
//    uint32 exponent : 5;
//    uint32 sign     : 1;
//};
//
//struct fp32
//{
//    uint32 mantista : 23;
//    uint32 exponent : 8;
//    uint32 sign     : 1;
//};
//
//struct fp64
//{
//    uint32 mantista : 52;
//    uint32 exponent : 11;
//    uint32 sign     : 1;
//};
//
//struct fp80
//{
//    uint32 mantista : 64;
//    uint32 exponent : 15;
//    uint32 sign     : 1;
//};
//
//bool isNaN(float src)
//{
//    return ((src & fp32exponentMask >> fp32exponentShift) == fp32exponentFull) &&
//           ((src & fp32mantistaMask) != 0);
//}
