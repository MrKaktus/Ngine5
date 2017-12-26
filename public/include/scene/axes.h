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

#include "core/rendering/rendering.hpp"
#include "scene/drawable.h"

using namespace en::gpu;

namespace en
{
   namespace scene
   {
   class Axes : public Drawable
         {
         public:
         shared_ptr<Buffer>   buffer;
         shared_ptr<Pipeline> pipeline;
        
         //Parameter enModelMatrix;
   
         Axes();
         void draw(const shared_ptr<Buffer> sceneParameters, const uint32 instances = 1U);
         };
   }
}

#endif
