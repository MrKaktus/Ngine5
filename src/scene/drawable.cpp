/*

 Ngine v5.0
 
 Module      : Base class for custom drawn objects
 Requirements: none
 Description : This class describes scene base class 
               for all objects that contain custom
               drawing function.

*/

#include "scene/drawable.h"

namespace en
{
   namespace scene
   {
   Drawable::Drawable() :
      enScene(nullptr),
      Entity()
   {
   }

   }
}