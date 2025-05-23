/*

 Ngine v5.0
 
 Module      : Base class for custom drawn objects
 Requirements: none
 Description : This class describes scene base class 
               for all objects that contain custom
               drawing function.

*/

#ifndef ENG_SCENE_DRAWABLE
#define ENG_SCENE_DRAWABLE

#include "scene/entity.h"
//#include "core/rendering/program.h"
#include "core/rendering/device.h"
namespace en
{
namespace scene
{

class Drawable : public Entity
{
    public:
    //en::gpu::Block enScene; // Constant Buffer with per Scene/Frame constants

    virtual void draw(const en::gpu::Buffer& sceneParameters, const uint32 instances = 1U) = 0;

    Drawable();
};

} // en::scene
} // en
#endif
