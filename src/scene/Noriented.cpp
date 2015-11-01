/*

 Ngine v5.0
 
 Module      : Basic point
 Requirements: math.h gpcpu.h
 Description : Describes basic state of any object in 3d space,
               like position and orientation. Side vector is
               calculated with right-handed orientation rule.
               All rotations are counter-clock-wise.

*/

#include "scene/Noriented.h"

namespace en
{
   Noriented::Noriented()
   {
   this->p = double3(0.0,  0.0,  0.0 );
   this->a = float3( 0.0f, 0.0f, 1.0f); 
   this->b = float3( 0.0f, 1.0f, 0.0f); 
   this->c = float3( 1.0f, 0.0f, 0.0f);
   this->s = float3( 1.0f, 1.0f, 1.0f);
   }
   
   Noriented::Noriented(double3& pos)
   {
   this->p = pos;
   this->a = float3( 0.0f, 0.0f, 1.0f);
   this->b = float3( 0.0f, 1.0f, 0.0f);
   this->c = float3( 1.0f, 0.0f, 0.0f);
   this->s = float3( 1.0f, 1.0f, 1.0f);
   }
   
   Noriented::Noriented(double3& pos, float3& look, float3& up)
   {
   this->p = pos;
   this->a = look;
   this->b = up;
   this->c = cross(up,look); 
   this->a.normalize();
   this->b.normalize();
   this->c.normalize();
   this->s = float3( 1.0f, 1.0f, 1.0f);
   }

   double3 Noriented::position(double3  pos)
   {
   return this->p = pos;
   }
   
   double3 Noriented::position(void)
   {
   return this->p;
   } 
   
   float3 Noriented::scale(float3 s)
   {
   return this->s = s;
   }
   
   float3 Noriented::scale(void)
   {
   return this->s;
   }
   
   void Noriented::direct(float3 look, float3 up)
   {
   this->a = look;
   this->b = up;
   this->c = cross(up,look);
   this->a.normalize();
   this->b.normalize();
   this->c.normalize();
   }

   float3 Noriented::direction(void)
   {
   return this->a;
   }  
                          
   float3 Noriented::top(void)
   {
   return this->b;
   }                                  
   
   float3 Noriented::side(void)
   {
   return this->c;
   } 
   
   void Noriented::yaw(float deg)
   {
   float3 a    = this->a;
   float3 c    = this->c;
   float  fcos = cos(radians(deg));
   float  fsin = sin(radians(deg));
   
   this->a = a*fcos + c*fsin;  
   this->c = c*fcos - a*fsin; 
   
   this->a.normalize();
   this->c.normalize();
   }
   
   void Noriented::roll(float deg)
   {
   float3 b    = this->b;
   float3 c    = this->c;
   float  fcos = cos(radians(deg));
   float  fsin = sin(radians(deg));
   
   this->c = c*fcos + b*fsin;   
   this->b = b*fcos - c*fsin;   
   
   this->b.normalize();
   this->c.normalize();
   }

   void Noriented::pitch(float deg)
   {
   float3 a    = this->a;
   float3 b    = this->b;
   float  fcos = cos(radians(deg));
   float  fsin = sin(radians(deg));
   
   this->b = b*fcos + a*fsin;
   this->a = a*fcos - b*fsin;
   
   this->a.normalize();
   this->b.normalize();
   }
   
   void Noriented::move(double units)
   {
   this->p += this->a * units;
   }
   
   void Noriented::strafe(double units)
   {
   this->p += this->c * units;
   }
   
   void Noriented::moveVertical(double units)
   {
   this->p += this->b * units;
   }

   // Positioning Matrix (Object Space -> World Space)
   float4x4 Noriented::matrix(void)
   {
   float4x4 m;
   
   /* m.m[0] = this->c.x;   m.m[4] = this->b.x;   m.m[8]  = this->a.x;   m.m[12] = (float)this->p.x;
      m.m[1] = this->c.y;   m.m[5] = this->b.y;   m.m[9]  = this->a.y;   m.m[13] = (float)this->p.y;
      m.m[2] = this->c.z;   m.m[6] = this->b.z;   m.m[10] = this->a.z;   m.m[14] = (float)this->p.z;
      m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
      
      float4x4 sc;
      
      sc.m[0] = this->s.x;  sc.m[4] = 0.0f;       sc.m[8]  = 0.0f;       sc.m[12] = 0.0f;
      sc.m[1] = 0.0f;       sc.m[5] = this->s.y;  sc.m[9]  = 0.0f;       sc.m[13] = 0.0f;
      sc.m[2] = 0.0f;       sc.m[6] = 0.0f;       sc.m[10] = this->s.z;  sc.m[14] = 0.0f;
      sc.m[3] = 0.0f;       sc.m[7] = 0.0f;       sc.m[11] = 0.0f;       sc.m[15] = 1.0f;
      
      return mul(m,sc); */ 
   
   m.m[0] = c.x * s.x;   m.m[4] = b.x * s.y;   m.m[8]  = a.x * s.z;   m.m[12] = static_cast<float>( p.x );
   m.m[1] = c.y * s.x;   m.m[5] = b.y * s.y;   m.m[9]  = a.y * s.z;   m.m[13] = static_cast<float>( p.y );
   m.m[2] = c.z * s.x;   m.m[6] = b.z * s.y;   m.m[10] = a.z * s.z;   m.m[14] = static_cast<float>( p.z );
   m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
   
   return m;
   } 
   
   // Rotation Matrix (Object Space -> World Space)
   float4x4 Noriented::rotationMatrix(void)
   {
   float4x4 m;
   
   m.m[0] = this->c.x;   m.m[4] = this->b.x;   m.m[8]  = this->a.x;   m.m[12] = 0.0f;
   m.m[1] = this->c.y;   m.m[5] = this->b.y;   m.m[9]  = this->a.y;   m.m[13] = 0.0f;
   m.m[2] = this->c.z;   m.m[6] = this->b.z;   m.m[10] = this->a.z;   m.m[14] = 0.0f;
   m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
   
   return m;
   } 
   
   // Translation Matrix (Object Space -> World Space)
   float4x4 Noriented::translationMatrix(void)
   {
   float4x4 m;
   
   m.m[0] = 1.0f;        m.m[4] = 0.0f;        m.m[8]  = 0.0f;        m.m[12] = (float)this->p.x;
   m.m[1] = 0.0f;        m.m[5] = 1.0f;        m.m[9]  = 0.0f;        m.m[13] = (float)this->p.y;
   m.m[2] = 0.0f;        m.m[6] = 0.0f;        m.m[10] = 1.0f;        m.m[14] = (float)this->p.z;
   m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
   
   return m;
   } 

}