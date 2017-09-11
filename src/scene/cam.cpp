/*

 Ngine v5.0
 
 Module      : Camera
 Requirements: math.h types.h Noriented.h gpcpu.h
 Description : This class describes full state, of
               virtual camera on the scene, that is 
               required to perform rendering.

*/

#include "scene/cam.h"
#include "core/rendering/rendering.hpp"
#include "resources/context.h"

namespace en
{
   namespace scene
   {
   // Stored in Column-Major order
   #define mat(m,r,c) (m)[(c)*4+(r)]

   // Stored in Row-Major order
   // #define mat(m,r,c) (m)[(r)*4+(c)]

   // In OpenGL / GLUT / Oculus - FOV means "Vertical FOV"
   // HOR+ - Horizontal FOV is adjusted to const Vertical FOV taking into notice aspect ratio
   //
   // Default is Widescreen PC monitor.
   //
   // Classic PC monitor:
   // 4:3 aspect, 90 hFOV -> 73.7397953 vFOV
   //
   // Widescreen PC monitor:
   // 16:9 aspect, 106.260205 hFOV <- 73.7397953 vFOV (to maintain the same vFOV like in classic displays)
   //
   // Console Widescreen TV:
   // 16:9 aspect, 60 hFOV -> ??? vFOV
   //

   FrustumSettings::FrustumSettings() :
      aspect(16.0f / 9.0f),
      vFov(74.0f),           
      nearPlane(0.01f),
      farPlane(10000.0f)
   {
   // w  - far plane width
   // h  - far plane height
   // a  - half of far plane height
   // a' - half of far plane width
   // b  - distance to far plane
   //
   // aspect = w / h
   // a  = h / 2
   // a' = w / 2
   // a' = aspect * a
   // 
   // tg( vFOV / 2 ) = a / b
   // 
   tangents.x = tan( radians(vFov / 2.0f) );
   tangents.y = tangents.x;
   //
   // tg( hFOV / 2 ) = a' / b
   // tg( hFOV / 2 ) = aspect * a / b
   // tg( hFOV / 2 ) = aspect * tg( vFOV / 2 )
   //
   tangents.z = aspect * tangents.x;
   tangents.w = tangents.z;
   }

   FrustumSettings::FrustumSettings(const float nearPlane, const float farPlane, const float vFov, const float aspect) :
      aspect(aspect),
      vFov(vFov),
      nearPlane(nearPlane),
      farPlane(farPlane)
   {
   tangents.x = tan( radians(vFov / 2.0f) );
   tangents.y = tangents.x;
   tangents.z = aspect * tangents.x;
   tangents.w = tangents.z;
   }

   FrustumSettings::FrustumSettings(const float nearPlane, const float farPlane, const float4 tangents) :
      tangents(tangents),
      nearPlane(nearPlane),
      farPlane(farPlane)
   {
   // aspect = w / h
   // w = al + ar
   // h = at + ab
   // 
   //     tg( vFOVt ) = at / b
   //     tg( vFOVb ) = ab / b
   // tg( vFOVt ) * b = at
   // tg( vFOVb ) * b = ab
   //         at + ab = (tg( vFOVt ) * b) + (tg( vFOVb ) * b)
   //               h = (tg( vFOVt ) + tg( vFOVb )) * b
   //               ...
   //               w = (tg( hFOVl ) + tg( hFOVr )) * b
   //
   //           w / h = ( (tg( hFOVl ) + tg( hFOVr )) * b ) / ( (tg( vFOVt ) + tg( vFOVb )) * b )
   //          aspect = (tg( hFOVl ) + tg( hFOVr )) / (tg( vFOVt ) + tg( vFOVb ))
   //
   aspect = (tangents.z + tangents.w) / (tangents.x + tangents.y);
   //
   //     tg( vFOVt ) = at / b
   //     tg( vFOVb ) = ab / b
   //           vFOVt = arctg( at / b )
   //           vFOVb = arctg( ab / b )
   //            vFOV = arctg( at / b ) + arctg( ab / b )
   //
   vFov = atan(tangents.x) + atan(tangents.y);
   }

   FrustumSettings& FrustumSettings::operator= (const FrustumSettings& src)
   {
   memcpy(this, reinterpret_cast<const void*>(&src), sizeof(FrustumSettings));
   return *this;
   }

   void FrustumSettings::set(const float4 _tangents)
   {
   tangents = _tangents;
   aspect   = (tangents.z + tangents.w) / (tangents.x + tangents.y);
   vFov     = atan(tangents.x) + atan(tangents.y);
   }

   void FrustumSettings::set(const float _vFov, const float _aspect)
   {
   tangents.x = tan( radians(_vFov / 2.0f) );
   tangents.y = tangents.x;
   tangents.z = _aspect * tangents.x;
   tangents.w = tangents.z;
   aspect     = _aspect;
   vFov       = _vFov;
   }

   // TODO: Input should be pointer to buffer (ideally Staging buffer on Heap).
   Ptr<Buffer> FrustumSettings::wireframe(Ptr<Heap> heap) const
   {
   //assert(Gpu.screen.created());

   // Calculate distance of frustum edges for near and far planes
   float4 nearEdges = tangents * nearPlane;
   float4 farEdges  = tangents * farPlane;

   // Line Stripe
   float3* points = new float3[16];
   points[0]  = float3(-nearEdges.z, -nearEdges.y, nearPlane); // Near frame
   points[1]  = float3( nearEdges.w, -nearEdges.y, nearPlane);
   points[2]  = float3( nearEdges.w,  nearEdges.x, nearPlane);
   points[3]  = float3(-nearEdges.z,  nearEdges.x, nearPlane);
   points[4]  = float3(-nearEdges.z, -nearEdges.y, nearPlane); 
   points[5]  = float3(-farEdges.z,  -farEdges.y,  farPlane);  // Lower-left connector
   points[6]  = float3(-farEdges.z,   farEdges.x,  farPlane);  // Far frame
   points[7]  = float3( farEdges.w,   farEdges.x,  farPlane);
   points[8]  = float3( farEdges.w,  -farEdges.y,  farPlane);
   points[9]  = float3(-farEdges.z,  -farEdges.y,  farPlane);
   points[10] = float3(-farEdges.z,   farEdges.x,  farPlane);
   points[11] = float3(-nearEdges.z,  nearEdges.x, nearPlane); // Upper-left connector
   points[12] = float3( nearEdges.w,  nearEdges.x, nearPlane);
   points[13] = float3( farEdges.w,   farEdges.x,  farPlane);  // Upper-right connector
   points[14] = float3( farEdges.w,  -farEdges.y,  farPlane);
   points[15] = float3( nearEdges.w, -nearEdges.y, nearPlane); // Lower-right connector

   // Create geometry buffer for given frustum
   Ptr<Buffer> buffer = heap->createBuffer(16, Formatting(Attribute::v3f32)); 

   // Create staging buffer
   Ptr<Buffer> staging = en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, 16);
   assert( staging );
   
   // Save wireframe to temporary buffer
   void* dst = staging->map();
   memcpy(dst, points, 16);
   staging->unmap();

   // TODO: In future distribute transfers to different queues in the same queue type family
   gpu::QueueType type = gpu::QueueType::Universal;
   if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
      type = gpu::QueueType::Transfer;

   // Copy data from staging buffer to final texture
   Ptr<CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(type);
   command->start();
   command->copy(staging, buffer);
   command->commit();
   
   // TODO:
   // here return completion handler callback !!! (no waiting for completion)
   // - this callback destroys CommandBuffer object
   // - destroys staging buffer
   //
   // Till it's done, wait for completion:
   
   command->waitUntilCompleted();
   command = nullptr;
   staging = nullptr;

   delete [] points;
   
   return buffer;
   }

   // Projection matrix (Eye Space -> Clipping Space [NDC])
   float4x4 FrustumSettings::projection(void)
   {
   #undef far
   #undef near

   float handed = -1.0f; // Right Handed
   float up     = tangents.x;
   float down   = tangents.y;
   float left   = tangents.z;
   float right  = tangents.w;
   float far    = farPlane;
   float near   = nearPlane;

   float2 scale  = float2( (2.0f / (left + right)), 
                           (2.0f / (up + down)) ); 
   float2 offset = float2( handed *  ((left - right) / (left + right)),  
                           handed * -((up - down) / (up + down)) );
   float  t1     = -handed * (far / (near - far));
   float  t2     = (far * near) / (near - far);

   float4x4 mat;
   float*   m = &mat.m[0];

   mat(m,0,0) = scale.x;     mat(m,0,1) = 0.0f;        mat(m,0,2) = offset.x;    mat(m,0,3) = 0.0f;
   mat(m,1,0) = 0.0f;        mat(m,1,1) = scale.y;     mat(m,1,2) = offset.y;    mat(m,1,3) = 0.0f;
   mat(m,2,0) = 0.0f;        mat(m,2,1) = 0.0f;        mat(m,2,2) = t1;          mat(m,2,3) = t2; 
   mat(m,3,0) = 0.0f;        mat(m,3,1) = 0.0f;        mat(m,3,2) = handed;      mat(m,3,3) = 0.0f;
   
   return mat;
   }









   //
   // tg( 2g ) != 2 * tg( g )
   //
   // hFOV / 2 = arctg( a' / b )
   //     hFOV = 2 * arctg( a' / b )
   //     hFOV = 2 * arctg( aspect * a / b )
   //     hFOV = 2 * arctg( aspect * tg( vFOV / 2 ) )
   //
   //float horizontalToVerticalFOV(float hFov, float aspect)
   //{
   //return static_cast<float>(degrees(2.0 * atan(tan(radians(static_cast<double>(hFov))  * 0.5) * static_cast<double>(aspect))));
   //}

   Cam::Cam() :
      Entity()
   {
   }

   // Matrix (Object Space -> World Space)
   float4x4 Cam::matrix(void)
   {
   return *pWorldMatrix;  
   } 

   // View Matrix (World Space -> View Space)
   // View Space has the same coordinate space as World Space and is also Right-Handed
   float4x4 Cam::viewMatrix(void)
   {
   float4x4 mat;
   float*   m = &mat.m[0];
   float3   p = *reinterpret_cast<float3*>(&pWorldMatrix->m[12]);
   float3   l = *reinterpret_cast<float3*>(&pWorldMatrix->m[4]);
   float3   u = *reinterpret_cast<float3*>(&pWorldMatrix->m[8]);
   float3   s = *reinterpret_cast<float3*>(&pWorldMatrix->m[0]);

   mat(m,0,0) =  s.x;   mat(m,0,1) =  s.y;   mat(m,0,2) =  s.z;   mat(m,0,3) = dot( s,-p); 
   mat(m,1,0) =  u.x;   mat(m,1,1) =  u.y;   mat(m,1,2) =  u.z;   mat(m,1,3) = dot( u,-p); 
   mat(m,2,0) = -l.x;   mat(m,2,1) = -l.y;   mat(m,2,2) = -l.z;   mat(m,2,3) = dot( l,-p); 
   mat(m,3,0) =  0.0f;  mat(m,3,1) =  0.0f;  mat(m,3,2) =  0.0f;  mat(m,3,3) = 1.0f;

   return mat;
   }
   
   // Eye View Matrix (World Space -> View Space)
   // View Space has the same coordinate space as World Space and is also Right-Handed
   float4x4 Cam::viewMatrix(float3 eyeVector)
   {
   float4x4 mat;
   float*   m = &mat.m[0];
   float3   p = *reinterpret_cast<float3*>(&pWorldMatrix->m[12]);
   float3   l = *reinterpret_cast<float3*>(&pWorldMatrix->m[4]);
   float3   u = *reinterpret_cast<float3*>(&pWorldMatrix->m[8]);
   float3   s = *reinterpret_cast<float3*>(&pWorldMatrix->m[0]);

   p  += (eyeVector.x * s) + (eyeVector.y * l) + (eyeVector.z * u);

   mat(m,0,0) =  s.x;   mat(m,0,1) =  s.y;   mat(m,0,2) =  s.z;   mat(m,0,3) = dot( s,-p); 
   mat(m,1,0) =  u.x;   mat(m,1,1) =  u.y;   mat(m,1,2) =  u.z;   mat(m,1,3) = dot( u,-p); 
   mat(m,2,0) = -l.x;   mat(m,2,1) = -l.y;   mat(m,2,2) = -l.z;   mat(m,2,3) = dot( l,-p); 
   mat(m,3,0) =  0.0f;  mat(m,3,1) =  0.0f;  mat(m,3,2) =  0.0f;  mat(m,3,3) = 1.0f;

   return mat;
   }

   // Rotation Matrix (World Space -> Eye Space)
   float4x4 Cam::rotationMatrix(void)
   {
   float4x4 mat;
   float*   m = &mat.m[0];
   float3   l = *reinterpret_cast<float3*>(&pWorldMatrix->m[4]);
   float3   u = *reinterpret_cast<float3*>(&pWorldMatrix->m[8]);
   float3   s = *reinterpret_cast<float3*>(&pWorldMatrix->m[0]);

   mat(m,0,0) =  s.x;   mat(m,0,1) =  s.y;   mat(m,0,2) =  s.z;   mat(m,0,3) = 0.0f; 
   mat(m,1,0) =  u.x;   mat(m,1,1) =  u.y;   mat(m,1,2) =  u.z;   mat(m,1,3) = 0.0f; 
   mat(m,2,0) = -l.x;   mat(m,2,1) = -l.y;   mat(m,2,2) = -l.z;   mat(m,2,3) = 0.0f; 
   mat(m,3,0) =  0.0f;  mat(m,3,1) =  0.0f;  mat(m,3,2) =  0.0f;  mat(m,3,3) = 1.0f;
 
   return mat;
   }

   // Translation Matrix (World Space -> Eye Space)
   float4x4 Cam::translationMatrix(void)
   {
   float4x4 mat;
   float*   m = &mat.m[0];
   float3   p = *reinterpret_cast<float3*>(&pWorldMatrix->m[12]);

   // Matrix is set to identity matrix by it's constructor
   mat(m,0,3) = (float)-p.x; 
   mat(m,1,3) = (float)-p.y;
   mat(m,2,3) = (float)-p.z;
 
   return mat;
   }

   // Projection matrix (Eye Space -> Clipping Space [NDC])
   float4x4 Cam::projectionMatrix(input::Eye eye)
   {
   #undef far
   #undef near

   float4x4 mat;
   float*   m = &mat.m[0];

   uint32 index = static_cast<uint32>(eye);
   float handed = -1.0f; // Right Handed
   float up     = frustum[index].tangents.x;
   float down   = frustum[index].tangents.y;
   float left   = frustum[index].tangents.z;
   float right  = frustum[index].tangents.w;
   float far    = frustum[index].farPlane;
   float near   = frustum[index].nearPlane;

   float2 scale  = float2( (2.0f / (left + right)), 
                           (2.0f / (up + down)) ); 
   float2 offset = float2( handed *  ((left - right) / (left + right)),  
                           handed * -((up - down) / (up + down)) );
   float  t1     = -handed * (far / (near - far));
   float  t2     = (far * near) / (near - far);


   //float LpR = frustum[index].tangents.z + frustum[index].tangents.w; // L + R
   //float RmL = frustum[index].tangents.w - frustum[index].tangents.z; // R - L
   //float UpD = frustum[index].tangents.x + frustum[index].tangents.y; // U + D
   //float UmD = frustum[index].tangents.x - frustum[index].tangents.y; // U - D
   //float FpN = frustum[index].farPlane   + frustum[index].nearPlane;  // F + N
   //float FmN = frustum[index].farPlane   - frustum[index].nearPlane;  // F - N
   //float FxN = frustum[index].farPlane   * frustum[index].nearPlane;  // F * N

   //float2 scale  = float2( (2.0f / LpR), (2.0f / UpD) ); 
   //float2 offset = float2( (RmL / LpR),  (UmD / UpD) );

   //    scale.y  = -scale.y;
   //    scale.x  = -scale.x * 0.5f;
   
   //float    ea = 1.0f / tan(radians(UpD) * 0.5f);
   //float    e  = ea / (16.0f / 9.0f );
   //scale  = float2( e, ea );

   ////float2 scale  = float2( e, ea ); 
   ////float2 offset = float2( 0.0f,  0.0f );

   //float  t1     = -FpN / FmN;
   //float  t2     = -(2.0f * FxN) / FmN;

   mat(m,0,0) = scale.x;    mat(m,0,1) = 0.0f;       mat(m,0,2) = offset.x;    mat(m,0,3) = 0.0f;
   mat(m,1,0) = 0.0f;       mat(m,1,1) = scale.y;    mat(m,1,2) = offset.y;    mat(m,1,3) = 0.0f;
   mat(m,2,0) = 0.0f;       mat(m,2,1) = 0.0f;       mat(m,2,2) = t1;          mat(m,2,3) = t2; 
   mat(m,3,0) = 0.0f;       mat(m,3,1) = 0.0f;       mat(m,3,2) = handed;      mat(m,3,3) = 0.0f;
   
   return mat;
   }

   }
}
