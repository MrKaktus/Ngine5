/*

 Ngine v5.0
 
 Module      : Camera
 Requirements: math.h types.h Noriented.h gpcpu.h
 Description : This class describes full state, of
               virtual camera on the scene, that is 
               required to perform rendering.

*/

#ifndef ENG_SCENE_CAMERA_OLD
#define ENG_SCENE_CAMERA_OLD

#include "core/types.h"             // Data types
#include "utilities/gpcpu/gpcpu.h"
#include "scene/Noriented.h"

namespace en
{
   class Ncam: public Noriented
      {
      private:
      //uint16 m_width;                    // Width of viewport
      //uint16 m_height;                   // Height of viewport
      float  m_aspect;                   // Aspect of viewport
      float  m_angle;                    // Horizontal FOV - Field Of View (between left-plane and right-plane)
      float  m_zmin;                     // Near clipping plane
      float  m_zmax;                     // Far clipping plane
      
      public:
      Ncam();                                              // Constructor
      Ncam(double3& pos);                                  // Constructor with given position
      Ncam(double3& pos, float3& look, float3& up);        // Constructor with all needed data
      
      void     setOrientation(float4x4& orientation);      // Sets camera direction
      void     setViewport(float angle, float aspect);     // Sets camera viewport settings
      
      void     setClipping(float znear, float zfar);       // Sets camera clipping planes
      float    nearClip(void);                             // Returns distance from position to near clip plane
      float    farClip(void);                              // Returns distance from position to far clip plane
      
      void     aspect(float a);                            // Viewport aspect
      float    aspect(void);                               // Viewport aspect
      
      float4x4 modelMatrix(void);                          // Returns Object Space -> World Space matrix (when treated cam like an object)
      float4x4 matrix(void);                               // Returns World Space -> Camera Space matrix
      float4x4 rotationMatrix(void);                       // Returns World Space -> Camera Space rotation matrix
      float4x4 translationMatrix(void);                    // Returns World Space -> Camera Space translation matrix
      float4x4 projectionMatrix(void);                     // Returns Camera Space -> Frustrum Space matrix
      float4x4 vp(uint8 eye, float eyeDistance, float projectionCenterOffset); // View-Projection matrix for stereoscopic rendering 
      };
}

#endif
