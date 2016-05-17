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
         Ptr<Buffer> buffer;
         Program   program;
         Parameter enModelMatrix;
   
         Axes();
         void draw(const Ptr<Buffer> sceneParameters, const uint32 instances = 1U);
         };
   }
}

#endif
