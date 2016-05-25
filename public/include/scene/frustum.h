/*

 Ngine v5.0
 
 Module      : Frustum Wireframe
 Requirements: 
 Description : This class describes scene primitive
               used to display wireframe of given
               camera frustum.

*/

#ifndef ENG_SCENE_FRUSTUM
#define ENG_SCENE_FRUSTUM

#include "core/rendering/rendering.hpp"
#include "scene/drawable.h"
#include "scene/cam.h"

using namespace en::gpu;

namespace en
{
   namespace scene
   {
   class Frustum : public Drawable
         {
         public:
         Ptr<Buffer> buffer;
         //Program   program;
         //Parameter enModelMatrix;
   
         Frustum(const FrustumSettings settings);
         void draw(const Ptr<Buffer> sceneParameters);
         };
   }
}
#endif
