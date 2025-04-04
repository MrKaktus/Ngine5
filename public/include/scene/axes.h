/*

 Ngine v5.0
 
 Module      : Coordinate System Axes
 Requirements: none
 Description : This class describes scene primitive
               used to display axes of given coordinate
               system.

*/

#ifndef ENG_SCENE_AXES
#define ENG_SCENE_AXES

#include "scene/drawable.h"

namespace en
{
namespace scene
{

class Axes : public Drawable
{
    public:
    gpu::Buffer* buffer;  // Weak pointer to global shared mesh (TODO: Replace with higher level concept)
    std::shared_ptr<gpu::Pipeline> pipeline;
   
  //Parameter enModelMatrix;
   
    Axes();
    void draw(const std::shared_ptr<gpu::Buffer> sceneParameters, const uint32 instances = 1U);
};

} // en::scene
} // en

#endif
