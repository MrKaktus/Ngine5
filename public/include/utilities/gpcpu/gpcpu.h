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

#ifndef ENG_UTILITIES_GPCPU
#define ENG_UTILITIES_GPCPU

#include "core/types.h"
#include <math.h>

namespace en
{
   #define xyz xyz()
   
   float2  operator- (float2 a, float2 b);
   float3  operator- (float3 a, float3 b);
   float4  operator- (float4 a, float4 b);
   double3 operator- (double3 a, double3 b);
   float2  operator+ (float2 a, float2 b);
   float3  operator+ (float3 a, float3 b);
   float4  operator+ (float4 a, float4 b);
   double3 operator+ (double3 a, double3 b);
   float2  operator* (float2 a, float b);
   float2  operator* (float a, float2 b);
   float3  operator* (float3 a, float b);
   float3  operator* (float a, float3 b);
   float4  operator* (float4 a, float b);
   float4  operator* (float a, float4 b);
   double3 operator* (float3 a, double b);
   double3 operator* (double a, float3 b);
   double3 operator* (double3 a, double b);
   float2  operator/ (float2 a, float b);
   double3 operator/ (double3 a, double b);
   
   bool      all(float2& a);
   bool      all(float3& a);
   bool      all(float4& a);
   bool      any(float2& a);
   bool      any(float3& a);
   bool      any(float4& a);
   float     clamp(float in, float min, float max);
   //double    clamp(double in, double min, double max);
   float3    cross(const float3& a, const float3& b);
   double3   cross(double3& a, double3& b);
   float     dot(float2& a, float2& b);
   float     dot(const float3& a, const float3& b);
   float     dot(float4& a, float4& b);
   double    dot(float3 a, double3 b);
   double    dot(float3& a, double3& b);
   double    dot(double3& a, float3& b);
   double    dot(double3& a, double3& b);
   float3    mul(float3x3& m, float3& v);
   float3    mul(float3& v, float3x3& m);
   float3x3  mul(float3x3& a, float3x3& b);
   float4    mul(float4x4& m, float3& v);
   double4   mul(float4x4& m, double3& v);
   float4    mul(float4x4& m, float4& v);
   float4    mul(float3& v, float4x4& m);
   float4    mul(float4& v, float4x4& m);
   float4x4  mul(float4x4 a, float4x4 b);
   float2    normalize(float2 a);
   float3    normalize(float3 a);
   double3   normalize(double3 a);
   float4    normalize(float4& a);
   float     length(float3& v);
   double    length(double3& v);
}

#endif
