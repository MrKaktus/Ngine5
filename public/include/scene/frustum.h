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

#include "scene/drawable.h"
#include "scene/cam.h"

namespace en
{
   namespace scene
   {
   class Frustum : public Drawable
         {
         public:
         std::shared_ptr<Buffer> buffer;
         //Program   program;
         //Parameter enModelMatrix;
   
         Frustum(const FrustumSettings settings);
         void draw(const std::shared_ptr<Buffer> sceneParameters);
         };
   }
}
#endif
