/*

 Ngine v5.0
 
 Module      : Camera
 Requirements: math.h types.h Noriented.h gpcpu.h
 Description : This class describes full state, of
               virtual camera on the scene, that is 
               required to perform rendering.

*/

#ifndef ENG_SCENE_CAMERA
#define ENG_SCENE_CAMERA

#include "input/input.h"
#include "scene/entity.h"
#include "core/rendering/device.h"

namespace en
{
namespace scene
{

// Hor+ scaling method:
// For different displays with different aspect ratio, 
// Vertical FOV should always be constant, and the 
// Horizontal FOV should be computed taking into notice
// current aspect ratio of the display.
class FrustumSettings
{
    friend class Cam;

    private:
    float4 tangents;  // Tangents, of angles between frustum center (look vector) and frustum planes (in order: up, down, left, right)
    float  aspect;    // Aspect ratio
    float  vFov;      // Vertical Field Of View (between top and bottom plane)
    float  nearPlane; // Distance to near clipping plane 
    float  farPlane;  // Distance to far clipping plane 

    public:
    FrustumSettings();
    FrustumSettings(const float nearPlane,   // Specifies default symmetric frustum
                    const float farPlane, 
                    const float vFov, 
                    const float aspect);
    FrustumSettings(const float nearPlane,   // Specifies asymetric frustum, useful for stereo rendering to better cover human eyes FOV.
                    const float farPlane, 
                    const float4 tangents);


    FrustumSettings& operator= (const FrustumSettings& src);

    void set(const float4 tangents);
    void set(const float vFov, const float aspect);
    std::unique_ptr<gpu::Buffer> wireframe(gpu::Heap& heap) const;
    float4x4 projection(void);
};

class Cam : public Entity
{
    public:
    FrustumSettings frustum[2];                          

    Cam();                                               // Constructor

    float4x4 matrix(void);                                 // Returns Object Space -> World Space matrix (when camera is treated like an object)
    float4x4 viewMatrix(void);                             // Returns World Space -> Camera Space matrix
    float4x4 viewMatrix(float3 eyeVector);                 // Returns World Space -> Camera Space matrix taking into notice eye offset
    float4x4 rotationMatrix(void);                         // Returns World Space -> Camera Space rotation matrix
    float4x4 translationMatrix(void);                      // Returns World Space -> Camera Space translation matrix
    float4x4 projectionMatrix(input::Eye eye = (input::Eye)0);         // Returns Camera Space -> Frustrum Space matrix
    //float4x4 vp(uint8 eye, float eyeDistance, float projectionCenterOffset); // View-Projection matrix for stereoscopic rendering 
};

} // en::scene
} // en

#endif
