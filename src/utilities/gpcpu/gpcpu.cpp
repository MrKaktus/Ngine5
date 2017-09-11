/*

 Ngine v5.0
 
 Module      : General purpose computing
 Requirements: types
 Description : Set of vector and matrix data types.
               Supports quick and intuitive way for
               programming complex geometrical 
               calculations. Allows use of GPU types
               and construction of shader like 
               algorithms on CPU.

*/

#include <math.h>
#include "utilities/gpcpu/gpcpu.h"

namespace en
{
   // Vectors substractions
   float2 operator- (float2 a, float2 b)
   {
   return float2(a.x-b.x, a.y-b.y);
   }
   
   float3 operator- (float3 a, float3 b)
   {
   return float3(a.x-b.x, a.y-b.y, a.z-b.z);
   }
   
   float4 operator- (float4 a, float4 b)
   {
   return float4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
   }
   
   double3 operator- (double3 a, double3 b)
   {
   return double3(a.x-b.x, a.y-b.y, a.z-b.z);
   }
   
   // Vectors additions
   float2 operator+ (float2 a, float2 b)
   {
   return float2(a.x+b.x, a.y+b.y);
   }
   
   float3 operator+ (float3 a, float3 b)
   {
   return float3(a.x+b.x, a.y+b.y, a.z+b.z);
   }
   
   float4 operator+ (float4 a, float4 b)
   {
   return float4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
   }
   
   double3 operator+ (double3 a, double3 b)
   {
   return double3(a.x+b.x, a.y+b.y, a.z+b.z);
   }
   
   // pre and past multiplication of vectors
   float2 operator* (float2 a, float b)
   {
   return float2(a.x*b, a.y*b);
   }

   float2 operator* (float a, float2 b)
   {
   return float2(a*b.x, a*b.y);
   }
   
   float3 operator* (float3 a, float b)
   {
   return float3(a.x*b, a.y*b, a.z*b);
   }
   
   float3 operator* (float a, float3 b)
   {
   return float3(a*b.x, a*b.y, a*b.z);
   }
   
   float4 operator* (float4 a, float b)
   {
   return float4(a.x*b, a.y*b, a.z*b, a.w*b);
   }
   
   float4 operator* (float a, float4 b)
   {
   return float4(a*b.x, a*b.y, a*b.z, a*b.w);
   }
   
   double3 operator* (float3 a, double b)
   {
   return double3((double)a.x*b, (double)a.y*b, (double)a.z*b);
   }
   
   double3 operator* (double a, float3 b)
   {
   return double3(a*(double)b.x, a*(double)b.y, a*(double)b.z);
   }
   
   double3 operator* (double3 a, double b)
   {
   return double3(a.x*b, a.y*b, a.z*b);
   }
   
   // Vectors division by scalar
   float2  operator/ (float2 a, float b)
   {
   return float2(a.x/b, a.y/b);
   }

   double3 operator/ (double3 a, double b)
   {
   return double3(a.x/b, a.y/b, a.z/b);
   }
   
   #define isZero(x) \
   *reinterpret_cast<uint32*>(&x) == 0
   
   // True if all components are different from zero
   bool all(float2& a)
   {
   return !isZero(a.x) && !isZero(a.y);
   }
   
   bool all(float3& a)
   {
   return !isZero(a.x) && !isZero(a.y) && !isZero(a.z);
   }
   
   bool all(float4& a)
   {
   return !isZero(a.x) && !isZero(a.y) && !isZero(a.z) && !isZero(a.w);
   }
   
   // True if any component is different from zero
   bool any(float2& a)
   {
   return !isZero(a.x) || !isZero(a.y);
   }
   
   bool any(float3& a)
   {
   return !isZero(a.x) || !isZero(a.y) || !isZero(a.z);
   }
   
   bool any(float4& a)
   {
   return !isZero(a.x) || !isZero(a.y) || !isZero(a.z) || !isZero(a.w);
   }
   
   // clamp value to a range
   float clamp(float in, float min, float max)
   {    
   return in < min ? min : (in > max ? max : in);
   }
   
   double clamp(double in, double min, double max)
   {    
   return in < min ? min : (in > max ? max : in);
   }
   
   // Cross product
   float3 cross(const float3& a, const float3& b)
   {
   return float3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
   }
   
   double3 cross(double3& a, double3& b)
   {
   return double3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
   }

   // Dot product
   float dot(float2& a, float2& b)
   {
   return a.x*b.x + a.y*b.y;
   }
   
   float dot(const float3& a,const float3& b)
   {
   return a.x*b.x + a.y*b.y + a.z*b.z;
   }
   
   double dot(float3 a, double3 b)
   {
   return (double)a.x*b.x + (double)a.y*b.y + (double)a.z*b.z;
   }
   
   double dot(float3& a, double3& b)
   {
   return (double)a.x*b.x + (double)a.y*b.y + (double)a.z*b.z;
   }
   
   double dot(double3& a, float3& b)
   {
   return a.x*(double)b.x + a.y*(double)b.y + a.z*(double)b.z;
   }
   
   double dot(double3& a, double3& b)
   {
   return a.x*b.x + a.y*b.y + a.z*b.z;
   }
   
   float dot(float4& a, float4& b)
   {
   return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
   }
   
   // Matrix multiplication by vector
   float3 mul(float3x3& m, float3& v)
   {
   return float3(m.m[0]*v.x + m.m[3]*v.y + m.m[6]*v.z,
                 m.m[1]*v.x + m.m[4]*v.y + m.m[7]*v.z,
                 m.m[2]*v.x + m.m[5]*v.y + m.m[8]*v.z);
   }
   
   float4 mul(float4x4& m, float3& v)
   {
   return float4(m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z  + m.m[12],
                 m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z  + m.m[13],
                 m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14],
                 m.m[3]*v.x + m.m[7]*v.y + m.m[11]*v.z + m.m[15]);
   }
   
   double4 mul(float4x4& m, double3& v)
   {
   return double4(double(m.m[0])*v.x + double(m.m[4])*v.y + double(m.m[8])*v.z  + double(m.m[12]),
                  double(m.m[1])*v.x + double(m.m[5])*v.y + double(m.m[9])*v.z  + double(m.m[13]),
                  double(m.m[2])*v.x + double(m.m[6])*v.y + double(m.m[10])*v.z + double(m.m[14]),
                  double(m.m[3])*v.x + double(m.m[7])*v.y + double(m.m[11])*v.z + double(m.m[15]));
   }
   
   float4 mul(float4x4& m, float4& v)
   {
   return float4(m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z  + m.m[12]*v.w,
                 m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z  + m.m[13]*v.w,
                 m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14]*v.w,
                 m.m[3]*v.x + m.m[7]*v.y + m.m[11]*v.z + m.m[15]*v.w);
   }
   
   // Vector multiplication by matrix
   float3 mul(float3& v, float3x3& m)
   {
   return float3(m.m[0]*v.x + m.m[3]*v.x + m.m[6]*v.x,
                 m.m[1]*v.y + m.m[4]*v.y + m.m[7]*v.y,
                 m.m[2]*v.z + m.m[5]*v.z + m.m[8]*v.z);
   }
   
   float4 mul(float3& v, float4x4& m)
   {
   return float4(m.m[0]*v.x + m.m[4]*v.x + m.m[8]*v.x  + m.m[12]*v.x,
                 m.m[1]*v.y + m.m[5]*v.y + m.m[9]*v.y  + m.m[13]*v.y,
                 m.m[2]*v.z + m.m[6]*v.z + m.m[10]*v.z + m.m[14]*v.z,
                 m.m[3]     + m.m[7]     + m.m[11]     + m.m[15]);
   }
   
   float4 mul(float4& v, float4x4& m)
   {
   return float4(m.m[0]*v.x + m.m[4]*v.x + m.m[8]*v.x  + m.m[12]*v.x,
                 m.m[1]*v.y + m.m[5]*v.y + m.m[9]*v.y  + m.m[13]*v.y,
                 m.m[2]*v.z + m.m[6]*v.z + m.m[10]*v.z + m.m[14]*v.z,
                 m.m[3]*v.w + m.m[7]*v.w + m.m[11]*v.w + m.m[15]*v.w);
   }

   // Matrix multiplication
   float3x3  mul(float3x3& a, float3x3& b)
   {
   float3x3 tmp;
   
   tmp.m[0]  = a.m[0]*b.m[0] + a.m[4]*b.m[1] + a.m[6]*b.m[2];
   tmp.m[1]  = a.m[1]*b.m[0] + a.m[5]*b.m[1] + a.m[7]*b.m[2];
   tmp.m[2]  = a.m[2]*b.m[0] + a.m[6]*b.m[1] + a.m[8]*b.m[2];
   
   tmp.m[3]  = a.m[0]*b.m[3] + a.m[4]*b.m[4] + a.m[6]*b.m[5];
   tmp.m[4]  = a.m[1]*b.m[3] + a.m[5]*b.m[4] + a.m[7]*b.m[5];
   tmp.m[5]  = a.m[2]*b.m[3] + a.m[6]*b.m[4] + a.m[8]*b.m[5];
   
   tmp.m[6]  = a.m[0]*b.m[6] + a.m[4]*b.m[7] + a.m[6]*b.m[8];
   tmp.m[7]  = a.m[1]*b.m[6] + a.m[5]*b.m[7] + a.m[7]*b.m[8];
   tmp.m[8]  = a.m[2]*b.m[6] + a.m[6]*b.m[7] + a.m[8]*b.m[8]; 
   
   return tmp;
   }
   
   float4x4 mul(float4x4 a, float4x4 b)
   {
   float4x4 tmp;
   
   tmp.m[0]  = a.m[0]*b.m[0] + a.m[4]*b.m[1] + a.m[8]*b.m[2]  + a.m[12]*b.m[3];
   tmp.m[1]  = a.m[1]*b.m[0] + a.m[5]*b.m[1] + a.m[9]*b.m[2]  + a.m[13]*b.m[3];
   tmp.m[2]  = a.m[2]*b.m[0] + a.m[6]*b.m[1] + a.m[10]*b.m[2] + a.m[14]*b.m[3];
   tmp.m[3]  = a.m[3]*b.m[0] + a.m[7]*b.m[1] + a.m[11]*b.m[2] + a.m[15]*b.m[3];
   
   tmp.m[4]  = a.m[0]*b.m[4] + a.m[4]*b.m[5] + a.m[8]*b.m[6]  + a.m[12]*b.m[7];
   tmp.m[5]  = a.m[1]*b.m[4] + a.m[5]*b.m[5] + a.m[9]*b.m[6]  + a.m[13]*b.m[7];
   tmp.m[6]  = a.m[2]*b.m[4] + a.m[6]*b.m[5] + a.m[10]*b.m[6] + a.m[14]*b.m[7];
   tmp.m[7]  = a.m[3]*b.m[4] + a.m[7]*b.m[5] + a.m[11]*b.m[6] + a.m[15]*b.m[7];
   
   tmp.m[8]  = a.m[0]*b.m[8] + a.m[4]*b.m[9] + a.m[8]*b.m[10]  + a.m[12]*b.m[11];
   tmp.m[9]  = a.m[1]*b.m[8] + a.m[5]*b.m[9] + a.m[9]*b.m[10]  + a.m[13]*b.m[11];
   tmp.m[10] = a.m[2]*b.m[8] + a.m[6]*b.m[9] + a.m[10]*b.m[10] + a.m[14]*b.m[11];
   tmp.m[11] = a.m[3]*b.m[8] + a.m[7]*b.m[9] + a.m[11]*b.m[10] + a.m[15]*b.m[11];
   
   tmp.m[12] = a.m[0]*b.m[12] + a.m[4]*b.m[13] + a.m[8]*b.m[14]  + a.m[12]*b.m[15];
   tmp.m[13] = a.m[1]*b.m[12] + a.m[5]*b.m[13] + a.m[9]*b.m[14]  + a.m[13]*b.m[15];
   tmp.m[14] = a.m[2]*b.m[12] + a.m[6]*b.m[13] + a.m[10]*b.m[14] + a.m[14]*b.m[15];
   tmp.m[15] = a.m[3]*b.m[12] + a.m[7]*b.m[13] + a.m[11]*b.m[14] + a.m[15]*b.m[15];
   
   return tmp;
   }

   // Vector normalization
   float2 normalize(float2& a)
   {
   float2 o;
   float tmp = sqrt(a.x*a.x + a.y*a.y);
   if (tmp) 
      {
      o.x = a.x / tmp;
      o.y = a.y / tmp;
      }
   return o;
   }
   
   float2 normalize(float2 a)
   {
   float2 o;
   float tmp = sqrt(a.x*a.x + a.y*a.y);
   if (tmp) 
      {			
      o.x = a.x /= tmp;
      o.y = a.y /= tmp;
      }
   return o;
   }
   
   float3 normalize(float3 a)
   {
   float3 o;
   float tmp = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
   if (tmp) 
      {
      o.x = a.x / tmp;
      o.y = a.y / tmp;
      o.z = a.z / tmp;
      }
   return o;
   }

   double3 normalize(double3 a)
   {
   double3 o;
   double tmp = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
   if (tmp) 
      {
      o.x = a.x / tmp;
      o.y = a.y / tmp;
      o.z = a.z / tmp;
      }
   return o;
   }
   
   float4 normalize(float4& a)
   {
   float4 o;
   float tmp = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
   if (tmp) 
      {
      o.x = a.x / tmp;
      o.y = a.y / tmp;
      o.z = a.z / tmp;
      }
   return o;
   }
   
   // Length of vector
   float length(float3& v)
   {
   return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
   }
    
   double length(double3& v)
   {
   return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
   }
}