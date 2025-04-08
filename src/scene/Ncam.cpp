/*

 Ngine v5.0
 
 Module      : Camera
 Requirements: math.h types.h Noriented.h gpcpu.h
 Description : This class describes full state, of
               virtual camera on the scene, that is 
               required to perform rendering.

*/

//#include "core/config.h"
#include "scene/Ncam.h"

namespace en
{

Ncam::Ncam()
{
    this->p = double3(0.0,  0.0,  0.0 );
    this->a = float3( 0.0f, 0.0f,-1.0f); 
    this->b = float3( 0.0f, 1.0f, 0.0f); 
    this->c = float3(-1.0f, 0.0f, 0.0f);
    
    m_aspect = 4.0f/3.0f;
    m_angle  = 60.0f;
    m_zmin   = 0.001f;
    m_zmax   = 100.0f;
}
    
Ncam::Ncam(double3& pos)
{
    this->p = pos;
    this->a = float3( 0.0f, 0.0f,-1.0f);
    this->b = float3( 0.0f, 1.0f, 0.0f);
    this->c = float3(-1.0f, 0.0f, 0.0f); 
    
    m_aspect = 4.0f/3.0f;
    m_angle  = 60.0f;
    m_zmin   = 0.001f;
    m_zmax   = 100.0f;
}

Ncam::Ncam(double3& pos, float3& look, float3& up): Noriented(pos,look,up)
{
    m_aspect = 4.0f/3.0f;
    m_angle  = 60.0f;
    m_zmin   = 0.001f;
    m_zmax   = 100.0f;
}

void Ncam::setOrientation(float4x4& orientation)
{
    a.x = -orientation.m[2]; 
    a.y = -orientation.m[6];
    a.z = -orientation.m[10];
    
    b.x = orientation.m[1];
    b.y = orientation.m[5]; 
    b.z = orientation.m[9];
     
    c.x = -orientation.m[0]; 
    c.y = -orientation.m[4]; 
    c.z = -orientation.m[8]; 
}

void Ncam::setViewport(float angle, float aspect)
{
    m_aspect = aspect;
    m_angle  = angle;
}

void Ncam::setClipping(float znear, float zfar)
{
    m_zmin   = znear;
    m_zmax   = zfar;
}

float Ncam::nearClip(void)
{
    return m_zmin;
}

float Ncam::farClip(void)
{
    return m_zmax;
}

void Ncam::aspect(float a)
{
    m_aspect = a;
}

float Ncam::aspect(void)
{
    return m_aspect;
}

// Positioning Matrix (Object Space -> World Space)
float4x4 Ncam::modelMatrix(void)
{
    float4x4 m;
    
    m.m[0] = this->c.x;   m.m[4] = this->b.x;   m.m[8]  = this->a.x;   m.m[12] = (float)this->p.x;
    m.m[1] = this->c.y;   m.m[5] = this->b.y;   m.m[9]  = this->a.y;   m.m[13] = (float)this->p.y;
    m.m[2] = this->c.z;   m.m[6] = this->b.z;   m.m[10] = this->a.z;   m.m[14] = (float)this->p.z;
    m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
    
    return m;  
} 

// Positioning Matrix (World Space -> Eye Space)
float4x4 Ncam::matrix(void)
{
    float4x4 m;
    
    m.m[0] = -this->c.x;  m.m[4] = -this->c.y;  m.m[8]  = -this->c.z;  m.m[12] = (float)dot(-this->c,-this->p); 
    m.m[1] =  this->b.x;  m.m[5] =  this->b.y;  m.m[9]  =  this->b.z;  m.m[13] = (float)dot( this->b,-this->p); 
    m.m[2] = -this->a.x;  m.m[6] = -this->a.y;  m.m[10] = -this->a.z;  m.m[14] = (float)dot(-this->a,-this->p); 
    m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
    
    return m;
}

// Rotation Matrix (World Space -> Eye Space)
float4x4 Ncam::rotationMatrix(void)
{
    float4x4 m;
    
    m.m[0] = -this->c.x;  m.m[4] = -this->c.y;  m.m[8]  = -this->c.z;  m.m[12] = 0.0f;
    m.m[1] =  this->b.x;  m.m[5] =  this->b.y;  m.m[9]  =  this->b.z;  m.m[13] = 0.0f;
    m.m[2] = -this->a.x;  m.m[6] = -this->a.y;  m.m[10] = -this->a.z;  m.m[14] = 0.0f;
    m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
    
    return m;
}

// Translation Matrix (World Space -> Eye Space)
float4x4 Ncam::translationMatrix(void)
{
    float4x4 m;
    
    m.m[0] = 1.0f;        m.m[4] = 0.0f;        m.m[8]  = 0.0f;        m.m[12] = (float)-this->p.x; 
    m.m[1] = 0.0f;        m.m[5] = 1.0f;        m.m[9]  = 0.0f;        m.m[13] = (float)-this->p.y;
    m.m[2] = 0.0f;        m.m[6] = 0.0f;        m.m[10] = 1.0f;        m.m[14] = (float)-this->p.z;
    m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] = 0.0f;        m.m[15] = 1.0f;
    
    return m;
}

// Projection matrix (Eye Space -> Clipping Space [NDC])
float4x4 Ncam::projectionMatrix(void)
{
    float4x4 m;
    float    ea = 1.0f / tan(radians(m_angle) * 0.5f);
    float    e  = (1.0f / m_aspect) * ea;
    float    t1 = -(m_zmax + m_zmin)/(m_zmax - m_zmin);
    float    t2 = -(2.0f * m_zmax * m_zmin)/(m_zmax - m_zmin);
    
    m.m[0] = e;           m.m[4] = 0.0f;        m.m[8]  = 0.0f;        m.m[12] = 0.0f;
    m.m[1] = 0.0f;        m.m[5] = ea;          m.m[9]  = 0.0f;        m.m[13] = 0.0f;
    m.m[2] = 0.0f;        m.m[6] = 0.0f;        m.m[10] = t1;          m.m[14] = t2; 
    m.m[3] = 0.0f;        m.m[7] = 0.0f;        m.m[11] =-1.0f;        m.m[15] = 0.0f;
    
    return m;
}

// View-Projection matrix for stereoscopic rendering 
float4x4 Ncam::vp(uint8 eye, float eyeDistance, float projectionCenterOffset)
{
    float4x4 mc;
    mc.m[12] = eye ? -projectionCenterOffset : projectionCenterOffset;
    
    float4x4 mp;
    float    ea = 1.0f / tan(radians(m_angle) * 0.5f);
    float    e  = (1.0f / m_aspect) * ea;
    float    t1 = -(m_zmax + m_zmin)/(m_zmax - m_zmin);
    float    t2 = -(2.0f * m_zmax * m_zmin)/(m_zmax - m_zmin);
    
    mp.m[0] = e;           mp.m[4] = 0.0f;        mp.m[8]  = 0.0f;        mp.m[12] = 0.0f;
    mp.m[1] = 0.0f;        mp.m[5] = ea;          mp.m[9]  = 0.0f;        mp.m[13] = 0.0f;
    mp.m[2] = 0.0f;        mp.m[6] = 0.0f;        mp.m[10] = t1;          mp.m[14] = t2; 
    mp.m[3] = 0.0f;        mp.m[7] = 0.0f;        mp.m[11] =-1.0f;        mp.m[15] = 0.0f;
    
    float3 pos;
    pos.x = static_cast<float>(this->p.x);
    pos.y = static_cast<float>(this->p.y);
    pos.z = static_cast<float>(this->p.z);
    pos  += this->c * (eye ? -eyeDistance : eyeDistance);
    
    float4x4 mv;
    mv.m[0] = -this->c.x;  mv.m[4] = -this->c.y;  mv.m[8]  = -this->c.z;  mv.m[12] = dot(-this->c,-pos); 
    mv.m[1] =  this->b.x;  mv.m[5] =  this->b.y;  mv.m[9]  =  this->b.z;  mv.m[13] = dot( this->b,-pos); 
    mv.m[2] = -this->a.x;  mv.m[6] = -this->a.y;  mv.m[10] = -this->a.z;  mv.m[14] = dot(-this->a,-pos); 
    mv.m[3] = 0.0f;        mv.m[7] = 0.0f;        mv.m[11] = 0.0f;        mv.m[15] = 1.0f;
     
    return mul(mul(mc,mp),mv);
}

} // en